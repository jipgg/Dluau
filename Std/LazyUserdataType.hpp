#pragma once
#include <dluau.h>
#include <string>
#include <boost/container/flat_map.hpp>
#include <print>
#include <cassert>
template <class Ty, class ...ArgTys>
concept Makeable_from = requires(ArgTys&&...args) {
    {Ty(std::forward<ArgTys>(args)...)};
};
template <class Ty>
concept Like_type_info = requires {
    {Ty::type_namespace()} -> std::same_as<const char*>;
    {Ty::type_name()} -> std::same_as<const char*>;
};
template <class Ty>
struct DefaultTypeInfo {
    static consteval const char* type_namespace() {return "gpm";};
    static const char* type_name() {return typeid(Ty).name();}
};
template<class Ty, Like_type_info TypeInfo_ = DefaultTypeInfo<Ty>>
class LazyUserdataType {
public:
    static constexpr const char* type_info_metamethod_key{"__cpp_std_type_info"};
    using Type = Ty;
    using TypeInfo = TypeInfo_;
    using Action = int(*)(lua_State*, Ty&);
    using Registry = boost::container::flat_map<std::string, Action>;
    struct InitInfo {
        Registry index{};
        Registry newindex{};
        Registry namecall{};
        Action checker{};
        const luaL_Reg* meta{};
    };
    static const InitInfo init_info;
    static constexpr std::string full_type_name() {
        using namespace std::string_literals;
        return TypeInfo::type_namespace() + "."s + TypeInfo::type_name();
    }
    static bool is(lua_State* L, int idx) {
        luaL_getmetatable(L, full_type_name().c_str());
        lua_getmetatable(L, idx);
        const bool equal = lua_rawequal(L, -1, -2);
        lua_pop(L, 2);
        return equal;
    }
    [[nodiscard]] static Ty* dynamic_view(lua_State* L, int idx) {
        luaL_getmetatable(L, full_type_name().c_str());
        lua_getfield(L, -1, type_info_metamethod_key);
        lua_remove(L, -2);
        lua_getmetatable(L, idx);
        lua_getfield(L, -1, type_info_metamethod_key);
        lua_remove(L, -2);
        const bool equal = lua_rawequal(L, -1, -2);
        lua_pop(L, 2);
        return equal ? view(L, idx) : nullptr;
    }
    [[nodiscard]] static Ty& view(lua_State* L, int idx) {
        return *static_cast<Ty*>(lua_touserdata(L, idx));
    }
    [[nodiscard]] static Ty& check(lua_State* L, int idx) {
        if (not is(L, idx)) luaL_typeerrorL(L, idx, TypeInfo::type_name());
        return view(L, idx);
    }
    static Ty& create(lua_State* L, const Ty& v) {
        Ty* p = static_cast<Ty*>(lua_newuserdatadtor(L, sizeof(Ty), [](void* ud) {static_cast<Ty*>(ud)->~Ty();}));
        push_metatable(L);
        lua_setmetatable(L, -2);
        new (p) Ty{v};
        return *p;
    }
    static Ty& create(lua_State* L, Ty&& v) {
        Ty* p = static_cast<Ty*>(lua_newuserdatadtor(L, sizeof(Ty), [](void* ud) {static_cast<Ty*>(ud)->~Ty();}));
        push_metatable(L);
        lua_setmetatable(L, -2);
        new (p) Ty{std::move(v)};
        return *p;
    }
    template <Makeable_from<Ty> ...Params>
    static Ty& make(lua_State* L, Params&&...args) {
        Ty* p = static_cast<Ty*>(lua_newuserdatadtor(L, sizeof(Ty), [](void* ud) {static_cast<Ty*>(ud)->~Ty();}));
        new (p) Ty{std::forward<Params>(args)...};
        push_metatable(L);
        lua_setmetatable(L, -2);
        return *p;
    }
    static bool initialized(lua_State* L) {
        if (not initialized_) {
            luaL_getmetatable(L, full_type_name().c_str());
            if (not lua_isnil(L, -1)
                and dluau_istyperegistered(full_type_name().c_str())) initialized_ = true;
            lua_pop(L, 1);
        }
        return initialized_;
    }
    static void push_metatable(lua_State* L) {
        if (luaL_newmetatable(L, full_type_name().c_str())) {
            std::string ti = TypeInfo::type_name();
            const luaL_Reg meta[] = {
                {"__index", metamethod_index},
                {"__newindex", metamethod_newindex},
                {"__namecall", metamethod_namecall},
                {nullptr, nullptr}
            };
            luaL_register(L, nullptr, meta);
            if (init_info.meta) luaL_register(L, nullptr, init_info.meta);
            lua_pushstring(L, TypeInfo::type_name());
            lua_setfield(L, -2, "__type");
            lua_pushstring(L, full_type_name().c_str());
            lua_setfield(L, -2, "__fulltype");
            lua_pushstring(L, typeid(Ty).name());
            lua_setfield(L, -2, type_info_metamethod_key);
            const std::type_info& e  = typeid(Ty);
            init_namecalls(L, init_info.namecall);
            auto& index = init_info.index;
        }
        if (not dluau_istyperegistered(full_type_name().c_str())) {
            dluau_registertype(full_type_name().c_str());
        }
    }
private:
    static int metamethod_namecall(lua_State* L) {
        auto& self = check(L, 1);
        int atom;
        lua_namecallatom(L, &atom);
        auto found_it = namecall_.find(atom);
        if (found_it == namecall_.end()) luaL_errorL(L, "unknown method");
        if (init_info.checker) init_info.checker(L, self); 
        return found_it->second(L, self);
    }
    static int metamethod_index(lua_State* L) {
        Ty& self = check(L, 1);
        const std::string key = luaL_checkstring(L, 2); 
        auto found_it = init_info.index.find(key);
        if (found_it == init_info.index.end()) {
            if (key == "type") {
                lua_pushstring(L, TypeInfo::type_name());
                return 1;
            } else if (key == "full_type") {
                lua_pushstring(L, full_type_name().c_str());
                return 1;
            }
            luaL_errorL(L, "unknown field '%s'", key.c_str());
        }
        if (init_info.checker) init_info.checker(L, self); 
        return found_it->second(L, self);
    }
    static int metamethod_newindex(lua_State* L) {
        Ty& self = check(L, 1);
        const std::string key = luaL_checkstring(L, 2); 
        auto found_it = init_info.newindex.find(key);
        if (found_it == init_info.newindex.end()) luaL_errorL(L, "unknown field '%s'", key.c_str());
        if (init_info.checker) init_info.checker(L, self); 
        return found_it->second(L, self);
    }
    static void init_namecalls(lua_State* L, const Registry& namecalls) {
        for (const auto& [key, call] : namecalls) {
            lua_pushlstring(L, key.data(), key.size());
            int atom;
            lua_tostringatom(L, -1, &atom);
            lua_pop(L, 1);
            namecall_.emplace(atom, call);
        }
    }
    inline static bool initialized_ = false;
    inline static boost::container::flat_map<int, Action> namecall_;
};
