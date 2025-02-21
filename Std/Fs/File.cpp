#include "fs.hpp"
#include <fstream>
#include <filesystem>
#include <memory>
namespace fs = std::filesystem;
using Registry = T_file::Registry;
using Init_info = T_file::Init_info;
constexpr const char* err_file_not_open = "couldn't open file";
template <class Ty>
concept Has_is_open_method = requires(const Ty& obj) {
    {obj.is_open()} -> std::same_as<bool>;
};

template <class Ty> requires Has_is_open_method<Ty>
constexpr void check_file_open(lua_State* L, const Ty& fstream) {
    if (not fstream.is_open()) dluau::error(L, err_file_not_open);
}

static const Registry namecall = {
    {"read_contents", [](lua_State* L, auto& self) -> int {
        auto file = std::ifstream(self);
        check_file_open(L, file);
        string line, contents;
        while (std::getline(file, line)) {
            contents.append(line) += '\n';
        }
        dluau::push(L, contents);
        return 1;
    }},
    {"append", [](lua_State* L, auto& self) -> int {
        auto file = std::ofstream{self, std::ios::app};
        file << luaL_checkstring(L, 2) << std::endl;
        return 1;
    }},
    {"overwrite", [](lua_State* L, auto& self) -> int {
        auto file = std::ofstream{self};
        file << luaL_checkstring(L, 2) << std::endl;
        return 1;
    }},
    {"reader", [](lua_State* L, auto& s) -> int {
        auto f = std::make_unique<std::ifstream>();
        f->open(s);
        check_file_open(L, *f);
        dluaustd::Reader_type::create(L, std::move(f));
        return 1;
    }},
    {"writer", [](lua_State* L, auto& s) -> int {
        auto f = std::make_unique<std::ofstream>();
        f->open(s, std::ios::app);
        check_file_open(L, *f);
        dluaustd::Writer_type::create(L, std::move(f));
        return 1;
    }},
    {"append_writer", [](lua_State* L, auto& s) -> int {
        auto f = std::make_unique<std::ofstream>();
        f->open(s, std::ios::app);
        check_file_open(L, *f);
        dluaustd::Writer_type::create(L, std::move(f));
        return 1;
    }},
};
static const Registry index = {
    {"path", [](lua_State* L, auto& s) -> int {
        dluau::push(L, s.string());
        return 1;
    }},
    {"parent", [](lua_State* L, auto& s) -> int {
        T_directory::make(L, s.parent_path());
        return 1;
    }},
    {"stem", [](lua_State* L, auto& s) -> int {
        dluau::push(L, s.stem().string());
        return 1;
    }},
    {"extension", [](lua_State* L, auto& s) -> int {
        dluau::push(L, s.extension().string());
        return 1;
    }},
    {"name", [](lua_State* L, auto& s) -> int {
        dluau::push(L, s.filename().string());
        return 1;
    }},
};
static const Registry newindex = {
    {"parent", [](lua_State* L, auto& s) -> int {
        newindex_parent(L, s);
        return 0;
    }},
    {"name", [](lua_State* L, auto& s) -> int {
        auto parent_dir = s.parent_path();
        try {
            fs::rename(s, parent_dir / luaL_checkstring(L, 3));
        } catch (const fs::filesystem_error& e) {
            dluau::error(L, e.what());
        }
        return 0;
    }},
};
static auto tostring(lua_State* L) -> int {
    auto& p = T_file::check(L, 1);
    dluau::push(L, p.string());
    return 1;
}

constexpr luaL_Reg meta[] = {
    {"__tostring", tostring},
    {nullptr, nullptr}
};
template<> const Init_info T_file::init_info{
    .index = index,
    .newindex = newindex,
    .namecall = namecall,
    .meta = meta,
};
