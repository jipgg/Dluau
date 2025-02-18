#include "fs.hpp"
#include <fstream>
#include <lua_utility.hpp>
#include <filesystem>
#include <memory>
namespace fs = std::filesystem;

static const SymlinkType::Registry namecall = {
    {"open", [](lua_State* L, Path& s) -> int {
        const auto p = fs::read_symlink(s);
        switch (fs::status(p).type()) {
            case fs::file_type::regular:
                FileType::make(L, p);
            break;
            case fs::file_type::directory:
                DirType::make(L, p);
            break;
            case fs::file_type::symlink:
                SymlinkType::make(L, p);
            break;
            default:
                lu::push(L, p);
            break;
        }
        return 1;
    }},
};
static const SymlinkType::Registry index = {
    {"link", [](lua_State* L, Path& s) -> int {
        lu::push(L, fs::read_symlink(s));
        return 1;
    }},
    {"path", [](lua_State* L, Path& s) -> int {
        lu::push(L, s.string());
        return 1;
    }},
    {"parent", [](lua_State* L, Path& s) -> int {
        DirType::make(L, s.parent_path());
        return 1;
    }},
    {"name", [](lua_State* L, Path& s) -> int {
        lu::push(L, s.filename().string());
        return 1;
    }},
};
static const SymlinkType::Registry newindex = {
    {"parent", [](lua_State* L, Path& s) -> int {
        newindex_parent(L, s);
        return 0;
    }},
    {"name", [](lua_State* L, Path& s) -> int {
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
    auto& p = SymlinkType::check(L, 1);
    lu::push(L, p.string());
    return 1;
}

constexpr luaL_Reg meta[] = {
    {"__tostring", tostring},
    {nullptr, nullptr}
};
template<> const SymlinkType::InitInfo SymlinkType::init_info{
    .index = index,
    .newindex = newindex,
    .namecall = namecall,
    .meta = meta,
};
