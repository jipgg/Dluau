#pragma once
#include <type_traits>
#include <lua_utility.hpp>
#include <boost/container/flat_map.hpp>
#include <functional>
#include <blaze/Blaze.h>
#include "misc.hpp"


template <Numeric Number, int Size>
struct VectorType {
    using Vector = blaze::StaticVector<Number, Size, blaze::defaultTransposeFlag, blaze::unaligned, blaze::unpadded>;
    using NamecallFn = std::function<int(lua_State*, Vector&)>;
    using NamecallMap = boost::container::flat_map<int, NamecallFn>;
    inline static NamecallMap namecalls{};
    static auto name_t() -> std::string {
        return std::string("VectorType");
    }
    static auto full_name_t() -> std::string {
        return std::string("gpm.math.") + name_t()
        + std::string("@") + std::to_string(Size)
        + std::string("@") + typeid(Number).name();
    }
    static int tag_t() {
        static const int t{dluau_registertypetagged(full_name_t().c_str())};
        return t;
    }
    static auto view(lua_State* L, int idx) -> Vector& {
        return *static_cast<Vector*>(lua_touserdatatagged(L, idx, tag_t()));
    }
    static auto is_type(lua_State* L, int idx) -> bool {
        return lua_userdatatag(L, idx) == tag_t();
    }
    static Vector& check(lua_State* L, int idx) {
        if (not is_type(L, idx)) lu::type_error(L, idx, std::string(name_t()));
        return view(L, idx);
    }
    static Vector& init(lua_State* L) {
        Vector& ud = lu::new_userdata_tagged<Vector>(L, tag_t());
        ud = Vector();
        if (luaL_newmetatable(L, full_name_t().c_str())) {
            constexpr luaL_Reg meta[] = {
                {"__index", metamethod_index},
                {"__newindex", metamethod_newindex},
                {"__namecall", metamethod_namecall},
                {"__add", metamethod_add},
                {"__sub", metamethod_sub},
                {"__div", metamethod_div},
                {"__mul", metamethod_mul},
                {"__tostring", metamethod_tostring},
                {nullptr, nullptr}
            };
            luaL_register(L, nullptr, meta);
            lu::push(L, name_t());
            lua_setfield(L, -2, "__type");
            register_namecalls(L);
        }
        lua_setmetatable(L, -2);
        return ud;
    }
    static int metamethod_index(lua_State* L) {
        Vector& v = view(L, 1);
        if (lua_isnumber(L, 2)) {
            const int at = lua_tointeger(L, 2);
            lu::push(L, v[check_range(L, at - 1)]);
            return 1;
        }
        const char k = *luaL_checkstring(L, 2);
        switch(k) {
            case 'x': lu::push(L, v[check_range(L, 0)]); return 1;
            case 'y': lu::push(L, v[check_range(L, 1)]); return 1;
            case 'z': lu::push(L, v[check_range(L, 2)]); return 1;
        }
        lu::error(L, "invalid index '{}'", luaL_checkstring(L, 2));
    }
    static int metamethod_newindex(lua_State* L) {
        Vector& v = view(L, 1);
        const Number val = static_cast<Number>(luaL_checknumber(L, 3));
        if (lua_isnumber(L, 2)) {
            const int at = lua_tointeger(L, 2);
            v[check_range(L, at - 1)] = val;
            return 1;
        }
        const char k = *luaL_checkstring(L, 2);
        switch(k) {
            case 'x': v[check_range(L, 0)] = val; return 0;
            case 'y': v[check_range(L, 1)] = val; return 0;
            case 'z': v[check_range(L, 2)] = val; return 0;
        }
        lu::error(L, "invalid index '{}'", luaL_checkstring(L, 2));
    }
    static int metamethod_namecall(lua_State* L) {
        int atom{};
        lua_namecallatom(L, &atom);
        if (not namecalls.contains(atom)) {
            const char* name = dluau_findstringatom(atom);
            if (name == nullptr) lu::error(L, "invalid namecall");
            else lu::error(L, "invali namecall '{}'", name); 
        }
        return namecalls[atom](L, view(L, 1));
    }
    static int metamethod_add(lua_State* L) {
        init(L) = check(L, 1) + check(L, 2);
        return 1;
    }
    static int metamethod_sub(lua_State* L) {
        init(L) = check(L, 1) - check(L, 2);
        return 1;
    }
    static int metamethod_mul(lua_State* L) {
        if (lua_isnumber(L, 1)) {
            init(L) = lua_tonumber(L, 1) * view(L, 2);
        } else if (lua_isnumber(L, 2)) {
            init(L) = view(L, 1) * lua_tonumber(L, 2);
        } else {
            init(L) = check(L, 1) * check(L, 2);
        }
        return 1;
    }
    static int metamethod_div(lua_State* L) {
        if (lua_isnumber(L, 2)) {
            init(L) = view(L, 1) / lua_tonumber(L, 2);
        } else {
            init(L) = check(L, 1) / check(L, 2);
        }
        return 1;
    }
    static auto metamethod_tostring(lua_State* L) -> int {
        Vector& self = view(L, 1);
        std::string str{"{"};
        const std::string separator{", "};
        for (const Number& v : self) {
            str += std::format("{}", v) + separator;
        }
        str.resize(str.size() - separator.size());
        str += "}";
        lu::push(L, str);
        return 1;
    }
    static auto lua_sizeof(lua_State* L) -> int{
        lua_pushinteger(L, sizeof(Vector));
        return 1;
    } 
    static auto lua_ctor(lua_State* L) -> int {
        Vector self{};
        for (int i{}; i < Size; ++i) {
            self[i] = static_cast<Number>(luaL_optnumber(L, i + 1, 0));
        }
        init(L) = self;
        return 1;
    }
    static void register_type(lua_State* L, const char* libname) {
        const luaL_Reg lib[] = {
            {"sizeof", lua_sizeof},
            {nullptr, nullptr}
        };
        lua_newtable(L);
        luaL_register(L, nullptr, lib);
        lua_newtable(L);
        constexpr const char* call{"__call"};
        lua_pushcfunction(L, [](lua_State* L) {
            lua_remove(L, 1);
            return lua_ctor(L);
        }, call);
        lua_setfield(L, -2, call);
        lua_setmetatable(L, -2);
        if (libname) lua_setfield(L, -2, libname);
    }
    static auto check_range(lua_State* L, int at) -> int {
        if (at < 0 or at >= Size) lu::arg_error(L, 2, "out of range [{}, {}]", 1, Size);
        return at;
    }
    static void register_namecalls(lua_State* L) {
        register_this_namecall(L, "dot", [](lua_State* L, Vector& self) {
            lu::push(L, blaze::dot(self, check(L, 2)));
            return 1;
        });
        register_this_namecall(L, "normalize", [](lua_State* L, Vector& self) {
            init(L) = blaze::normalize(self);
            return 1;
        });
        register_this_namecall(L, "length", [](lua_State* L, Vector& self) {
            lu::push(L, blaze::length(self));
            return 1;
        });
        register_this_namecall(L, "clone", [](lua_State* L, Vector& self) {
            init(L) = self;
            return 1;
        }); 
    }
    static void register_this_namecall(lua_State* L, const std::string& key, NamecallFn&& fn) {
        namecalls.emplace(dluau_stringatom(L, key.c_str()), std::move(fn));
    }
};
