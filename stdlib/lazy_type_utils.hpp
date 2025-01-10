#pragma once
#include <lumin.h>
#include <string>
#include <unordered_map>
#include <lualib.h>
#include <cassert>

template<class T>
class lazy_type_utils {
public:
    using meta_function = int(*)(lua_State*, T&);
    using map = std::unordered_map<std::string, meta_function>;
    static int tag() {
        static int typetag = lumin_newtypetag();
        return typetag;
    }
    static T& check(lua_State* L, int idx) {
        if (lua_userdatatag(L, idx) != tag()) luaL_typeerrorL(L, idx, tname_.c_str());
        return *static_cast<T*>(lua_touserdatatagged(L, idx, tag()));
    }
    static T& create(lua_State* L, const T& v) {
        assert(initialized_);
        T* p = static_cast<T*>(lua_newuserdatatagged(L, sizeof(T), tag()));
        luaL_getmetatable(L, tname_.c_str());
        lua_setmetatable(L, -2);
        new (p) T{v};
        return *p;
    }
    static bool initialized() {
        return initialized_;
    }
    static void init(lua_State* L,
        const std::string& tname,
        const map& index,
        const map& newindex,
        const map& namecall,
        const luaL_Reg* meta_ext = nullptr) {
        assert(not initialized_);
        initialized_ = true;
        tname_ = tname;
        if (luaL_newmetatable(L, tname_.c_str())) {
            const luaL_Reg meta[] = {
                {"__index", metamethod_index},
                {"__newindex", metamethod_newindex},
                {"__namecall", metamethod_namecall},
                {nullptr, nullptr}
            };
            luaL_register(L, nullptr, meta);
            if (meta_ext) luaL_register(L, nullptr, meta_ext);
            lua_pushstring(L, tname.c_str());
            lua_setfield(L, -2, "__type");
            init_namecalls(L, namecall);
            index_ = index;
            newindex_ = newindex;
            lua_setuserdatadtor(L, tag(), [](lua_State* L, void* ud) {
                static_cast<T*>(ud)->~T();
            });
        }
        lua_pop(L, 1);
    }
private:
    static int metamethod_namecall(lua_State* L) {
        auto& self = *static_cast<T*>(lua_touserdatatagged(L, 1, tag()));
        int atom;
        lua_namecallatom(L, &atom);
        auto found_it = namecall_.find(atom);
        if (found_it == namecall_.end()) luaL_errorL(L, "invalid namecall");
        return found_it->second(L, self);
    }
    static int metamethod_index(lua_State* L) {
        T& self = check(L, 1);
        const std::string key = luaL_checkstring(L, 2); 
        auto found_it = index_.find(key);
        if (found_it == index_.end()) luaL_errorL(L, "invalid index");
        return found_it->second(L, self);
    }
    static int metamethod_newindex(lua_State* L) {
        T& self = check(L, 1);
        const std::string key = luaL_checkstring(L, 2); 
        auto found_it = newindex_.find(key);
        if (found_it == newindex_.end()) luaL_errorL(L, "invalid index");
        return found_it->second(L, self);
    }
    static void init_namecalls(lua_State* L, const map& namecalls) {
        for (const auto& [key, call] : namecalls) {
            lua_pushlstring(L, key.data(), key.size());
            int atom;
            lua_tostringatom(L, -1, &atom);
            lua_pop(L, 1);
            namecall_.emplace(atom, call);
        }
    }
    inline static bool initialized_ = false;
    inline static std::unordered_map<int, meta_function> namecall_;
    inline static map index_;
    inline static map newindex_;
    inline static std::string tname_{""};
};
