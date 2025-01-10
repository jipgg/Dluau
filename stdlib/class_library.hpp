#pragma once
#include <lumin.h>
#include <string>
#include <unordered_map>
#include <iostream>
#include <optional>
#include <lualib.h>

template<class T>
struct class_library {
    using meta_function = int(*)(lua_State*, T&);
    template <class U> using meta_map = std::unordered_map<U, meta_function>;
    inline static std::string tname{"userdata"};
    inline static meta_map<int> namecall_atoms;
    inline static meta_map<std::string> namecall_map;
    inline static meta_map<std::string> index_map;
    inline static meta_map<std::string> newindex_map;
    static int tag() {
        static int typetag = lumin_newtypetag();
        return typetag;
    }
    static T& check(lua_State* L, int idx) {
        if (lua_userdatatag(L, idx) != tag()) luaL_typeerrorL(L, idx, tname.c_str());
        return *static_cast<T*>(lua_touserdatatagged(L, idx, tag()));
    }
    static int namecall(lua_State* L) {
        auto& self = *static_cast<T*>(lua_touserdatatagged(L, 1, tag()));
        int atom;
        lua_namecallatom(L, &atom);
        auto found_it = namecall_atoms.find(atom);
        if (found_it == namecall_atoms.end()) luaL_errorL(L, "invalid namecall");
        return found_it->second(L, self);
    }
    static int index(lua_State* L) {
        T& self = check(L, 1);
        const std::string key = luaL_checkstring(L, 2); 
        std::cout << "KEEEEY: '" + key + "'\n";
        auto found_it = index_map.find(key);
        if (found_it == index_map.end()) luaL_errorL(L, "invalid index");
        return found_it->second(L, self);
    }
    static int newindex(lua_State* L) {
        T& self = check(L, 1);
        const std::string key = luaL_checkstring(L, 2); 
        auto found_it = newindex_map.find(key);
        if (found_it == newindex_map.end()) luaL_errorL(L, "invalid index");
        return found_it->second(L, self);
    }
    static T& create(lua_State* L, const T& v) {
        if (luaL_newmetatable(L, tname.c_str())) {
            const luaL_Reg meta[] = {
                {"__index", index},
                {"__newindex", newindex},
                {"__namecall", namecall},
                {nullptr, nullptr}
            };
            luaL_register(L, nullptr, meta);
            lua_pushstring(L, tname.c_str());
            lua_setfield(L, -2, "__type");
            init_namecall_map(L);
            lua_setuserdatadtor(L, tag(), [](lua_State* L, void* ud) {
                static_cast<T*>(ud)->~T();
            });
        }
        lua_pop(L, 1);
        T* p = static_cast<T*>(lua_newuserdatatagged(L, sizeof(T), tag()));
        luaL_getmetatable(L, tname.c_str());
        lua_setmetatable(L, -2);
        new (p) T{v};
        return *p;
    }
    static void init_namecall_map(lua_State* L) {
        for (const auto& [key, call] : namecall_map) {
            lua_pushlstring(L, key.data(), key.size());
            int atom;
            lua_tostringatom(L, -1, &atom);
            lua_pop(L, 1);
            namecall_atoms.emplace(atom, call);
        }

    }
};
