#pragma once
#include <lumin.h>
#include <string>
#include <unordered_map>
#include <lualib.h>
#include <cassert>
#include <optional>

template<class T>
class userdatatagged_lazybuilder {
public:
    static const std::string type_name;
    using action = int(*)(lua_State*, T&);
    using registry = std::unordered_map<std::string, action>;
    static bool is_type(lua_State* L, int idx) {
        return lua_userdatatag(L, idx) == type_tag();
    }
    static int type_tag() {
        static int tag = lumin_newtypetag();
        return tag;
    }
    static T& check_userdata(lua_State* L, int idx) {
        if (not is_type(L, idx)) luaL_typeerrorL(L, idx, type_name.c_str());
        return *static_cast<T*>(lua_touserdatatagged(L, idx, type_tag()));
    }
    static T& new_userdata(lua_State* L, const T& v) {
        assert(initialized_);
        T* p = static_cast<T*>(lua_newuserdatatagged(L, sizeof(T), type_tag()));
        luaL_getmetatable(L, type_name.c_str());
        lua_setmetatable(L, -2);
        new (p) T{v};
        return *p;
    }
    static bool initialized() {
        return initialized_;
    }
    struct init_info {
        std::optional<registry> index{std::nullopt};
        std::optional<registry> newindex{std::nullopt};
        std::optional<registry> namecall{std::nullopt};
        const luaL_Reg* meta_ext{nullptr};
    };
    static void init(lua_State* L, init_info info) {
        assert(not initialized_);
        initialized_ = true;
        if (luaL_newmetatable(L, type_name.c_str())) {
            const luaL_Reg meta[] = {
                {"__index", metamethod_index},
                {"__newindex", metamethod_newindex},
                {"__namecall", metamethod_namecall},
                {nullptr, nullptr}
            };
            luaL_register(L, nullptr, meta);
            if (info.meta_ext) luaL_register(L, nullptr, info.meta_ext);
            lua_pushstring(L, type_name);
            lua_setfield(L, -2, "__type");
            init_namecalls(L, std::move(info.namecall));
            index_ = std::move(info.index);
            newindex_ = std::move(info.newindex);
            lua_setuserdatadtor(L, type_tag(), [](lua_State* L, void* ud) {
                static_cast<T*>(ud)->~T();
            });
        }
        lua_pop(L, 1);
    }
private:
    static int metamethod_namecall(lua_State* L) {
        T& self = check_userdata(L, 1);
        int atom;
        lua_namecallatom(L, &atom);
        auto found_it = namecall_.find(atom);
        if (found_it == namecall_.end()) luaL_errorL(L, "invalid namecall");
        return found_it->second(L, self);
    }
    static int metamethod_index(lua_State* L) {
        T& self = check_userdata(L, 1);
        const std::string key = luaL_checkstring(L, 2); 
        auto found_it = index_.find(key);
        if (found_it == index_.end()) luaL_errorL(L, "invalid index");
        return found_it->second(L, self);
    }
    static int metamethod_newindex(lua_State* L) {
        T& self = check_userdata(L, 1);
        const std::string key = luaL_checkstring(L, 2); 
        auto found_it = newindex_.find(key);
        if (found_it == newindex_.end()) luaL_errorL(L, "invalid index");
        return found_it->second(L, self);
    }
    static void init_namecalls(lua_State* L, std::optional<registry> namecalls) {
        if (not namecalls) return;
        for (const auto& [key, call] : *namecalls) {
            lua_pushlstring(L, key.data(), key.size());
            int atom;
            lua_tostringatom(L, -1, &atom);
            lua_pop(L, 1);
            namecall_.emplace(atom, call);
        }
    }
    inline static bool initialized_ = false;
    inline static std::optional<std::unordered_map<int, action>> namecall_;
    inline static std::optional<registry> index_;
    inline static std::optional<registry> newindex_;
};
