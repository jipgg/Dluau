#include "misc.hpp"
#include <blaze/Blaze.h>
#include <boost/container/flat_map.hpp>
#include <functional>
#include <lua_utility.hpp>
#include "VectorType.hpp"


template <Numeric Num, int N>
struct SquareMatrixType {
    using Matrix = blaze::StaticMatrix<Num, N, N, blaze::defaultStorageOrder, blaze::unaligned, blaze::unpadded>;
    using NamecallFn = std::function<int(lua_State*)>;
    using NamecallMap = boost::container::flat_map<int, NamecallFn>;
    using State = lua_State*;
    inline static NamecallMap namecalls{};
    inline static const std::string name_t{"SquareMatrixType"}; 
    inline static const std::string full_name_t{
        "gpm.math." + name_t
        + std::string("@") + std::to_string(N)
        + std::string("@") + typeid(Num).name()
    };
    inline static int tag_t = dluau_registertypetagged(full_name_t.c_str());
    [[nodiscard]] static Matrix& view(State L, int idx) {
        return *static_cast<Matrix*>(lua_touserdatatagged(L, idx, tag_t));
    }
    static bool is_type(State L, int idx) {
        return lua_userdatatag(L, idx) == tag_t;
    }
    [[nodiscard]] static Matrix& check(lua_State* L, int idx) {
        if (not is_type(L, idx)) lu::type_error(L, idx, name_t.c_str());
        return view(L, idx);
    }
    static Matrix& init(State L) {
        auto& ud = lu::new_userdata_tagged<Matrix>(L, tag_t);
        ud = Matrix();
        if (luaL_newmetatable(L, full_name_t.c_str())) {
            constexpr luaL_Reg meta[] = {
                {"__index", Metatable::index},
                {"__newindex", Metatable::newindex},
                {"__add", Metatable::add},
                {"__sub", Metatable::sub},
                {"__mul", Metatable::mul},
                {"__tostring", Metatable::tostring},
                {nullptr, nullptr}
            };
            luaL_register(L, nullptr, meta);
            lu::push(L, name_t);
            lua_setfield(L, -2, "__type");
            lu::push(L, full_name_t);
            lua_setfield(L, -2, "__fulltype");
            lu::push(L, typeid(Num).name());
            lua_setfield(L, -2, "__T");
            lu::push(L, std::to_string(N));
            lua_setfield(L, -2, "__N");
        }
        lua_setmetatable(L, -2);
        return ud;
    }
    static int lua_sizeof(State L ) {
        lua_pushinteger(L, sizeof(Matrix));
        return 1;
    }
    static int lua_ctor(State L) {
        Matrix mat{};
        for (int idx{}; idx < N * N; ++idx) {
            const int i = idx / N;
            const int j = idx % N;
            const Num m = static_cast<Num>(luaL_optnumber(L, idx + 1, 0));
            mat.at(i, j) = m;
        }
        init(L) = mat;
        return 1;
    }
    static int lua_zero(State L) {
        init(L) = Matrix(0);
        return 1;
    }
    static int lua_identity(State L) {
        init(L) = blaze::IdentityMatrix<Num>(N);
        return 1;
    }
    static int read_from_buffer(lua_State* L) {
        size_t len;
        void* buf = luaL_checkbuffer(L, 1, &len);
    }
    static void register_type(State L, const char* libname) {
        const luaL_Reg ctors[] = {
            {"identity", lua_identity},
            {"sizeof", lua_sizeof},
            {nullptr, nullptr}
        };
        lua_newtable(L);
        luaL_register(L, nullptr, ctors);
        lua_newtable(L);
        lua_pushcfunction(L, [](State L) {
            lua_remove(L, 1);
            return lua_ctor(L);
        }, "__call");
        lua_setfield(L, -2, "__call");
        lua_setmetatable(L, -2);
        if (libname) lua_setfield(L, -2, libname);
    }
    struct Metatable {
        static int index(State L) {
            Matrix& self = view(L, 1);
            const int combined = luaL_checkinteger(L, 2);
            const int i = combined / 10 - 1;
            const int j = combined % 10 - 1;
            lu::push(L, self.at(i, j));
            return 1;
        }
        static int newindex(lua_State* L) {
            Matrix& self = view(L, 1);
            const int combined = luaL_checkinteger(L, 2);
            const int i = combined / 10 - 1;
            const int j = combined % 10 - 1;
            self.at(i, j) = static_cast<Num>(luaL_checknumber(L, 3));
            return 1;
        }
        static int namecall(lua_State* L) {
            int atom{};
            lua_namecallatom(L, &atom);
            if (not namecalls.contains(atom)) lu::error(L, "invalid namecall '{}'", dluau_findstringatom(atom));
            return namecalls[atom](L);
        }
        static int add(lua_State* L) {
            init(L) = check(L, 1) + check(L, 2);
            return 1;
        }
        static int sub(lua_State* L) {
            init(L) = check(L, 1) - check(L, 2);
            return 1;
        }
        static int mul(lua_State* L) {
            using VecT = VectorType<Num, N>;
            if (lua_isnumber(L, 1)) {
                init(L) = static_cast<Num>(lua_tonumber(L, 1)) * view(L, 2);
            } else if (lua_isnumber(L, 2)) {
                init(L) = view(L, 1) * static_cast<Num>(lua_tonumber(L, 2));
            } else if (VecT::is_type(L, 2)) {
                VecT::init(L) = view(L, 1) * VecT::view(L, 2);
            } else {
                init(L) = check(L, 1) * check(L, 2);
            }
            return 1;
        }
        static int tostring(lua_State* L) {
            Matrix& self = view(L, 1);
            std::string str{"{"};
            for (int i{}; i < N; ++i) {
                str += "{";
                const std::string separator{", "};
                for (int j{}; j < N; ++j) {
                    str += std::format("{}{}", self.at(i, j), separator);
                }
                str.resize(str.size() - separator.size());
                str += "}, ";
            }
            str.pop_back();
            str.back() = '}';
            lu::push(L, str);
            return 1;
        }
    };
};
