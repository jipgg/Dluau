#include "fs.hpp"
#include <fstream>
#include <lua_utility.hpp>
#include <filesystem>
#include <memory>
namespace fs = std::filesystem;
using Registry = T_file::Registry;
using Init_info = T_file::Init_info;

static const Registry namecall = {
    {"reader", [](lua_State* L, auto& s) -> int {
        auto f = std::make_unique<std::ifstream>();
        f->open(s);
        if (not f->is_open()) lu::error(L, "couldn't open file");
        gpm::Reader_type::create(L, std::move(f));
        return 1;
    }},
    {"writer", [](lua_State* L, auto& s) -> int {
        auto f = std::make_unique<std::ofstream>();
        f->open(s, std::ios::app);
        if (not f->is_open()) lu::error(L, "couldn't open file");
        gpm::Writer_type::create(L, std::move(f));
        return 1;
    }},
    {"append_writer", [](lua_State* L, auto& s) -> int {
        auto f = std::make_unique<std::ofstream>();
        f->open(s, std::ios::app);
        if (not f->is_open()) lu::error(L, "couldn't open file");
        gpm::Writer_type::create(L, std::move(f));
        return 1;
    }},
};
static const Registry index = {
    {"path", [](lua_State* L, auto& s) -> int {
        lu::push(L, s.string());
        return 1;
    }},
    {"parent", [](lua_State* L, auto& s) -> int {
        T_directory::make(L, s.parent_path());
        return 1;
    }},
    {"stem", [](lua_State* L, auto& s) -> int {
        lu::push(L, s.stem().string());
        return 1;
    }},
    {"extension", [](lua_State* L, auto& s) -> int {
        lu::push(L, s.extension().string());
        return 1;
    }},
    {"name", [](lua_State* L, auto& s) -> int {
        lu::push(L, s.filename().string());
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
            lu::error(L, e.what());
        }
        return 0;
    }},
};
static auto tostring(lua_State* L) -> int {
    auto& p = T_file::check(L, 1);
    lu::push(L, p.string());
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
