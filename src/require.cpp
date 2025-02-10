#include <dluau.hpp>
#include <common.hpp>
#include <boost/container/flat_map.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <optional>
using namespace dluau::type_aliases;
namespace fs = std::filesystem;
namespace ranges = std::ranges;
using Json = nlohmann::json;
using dluau::default_file_extensions;

static Flat_map<String, String> aliases;
static Flat_map<String, int> luamodules;
static Flat_map<lua_State*, String> script_paths;
static bool config_file_initialized{false};

static auto get_precompiled_library_values(const Path& p) {
    auto as_string_literal = [](const Path& path) {
        auto str = path.string();
        ranges::replace(str, '\\', '/');
        return format("(\"{}\")", str);
    };
    const auto arr = std::to_array<Pair<Regex, String>>({
        {Regex(R"(\bscript.directory\b)"), as_string_literal(p.parent_path())},
        {Regex(R"(\bscript.path\b)"), as_string_literal(p)},
        {Regex(R"(\bscript.name\b)"), as_string_literal(Path(p).stem())},
    });
    return arr;
}

static Expected<Path> find_config_file(Path base = fs::current_path(), int search_depth = 5) {
    const auto config_file_names = std::to_array<Str_view>({
        ".luaurc", ".dluaurc",
        ".dluaurc.json", ".luaurc.json",
        "luaurc.json", "dluaurc.json",
    });
    auto exists = [&cfn = config_file_names, &base](Path& out) -> bool {
        for (Str_view name : cfn) {
            const Path potential_path = base / name;
            if (fs::exists(potential_path)) {
                out = potential_path;
                return true;
            } 
        }
        return false;
    };
    int depth{};
    Path potential_path;
    while (not exists(potential_path) and depth++ < search_depth) base = base.parent_path();
    if (potential_path.empty()) return Unexpected("could not find config file");
    return potential_path;
}
static bool has_alias(const String& str) {
    return str[0] == '@';
}
static Expected<void> load_aliases(const fs::path& base = fs::current_path(), int search_depth = 5) {
    auto found_config = find_config_file(base, search_depth);
    if (not found_config) return Expected<void>();

    auto source = common::read_file(*found_config);
    if (not source) return Unexpected(std::format("couldn't read source '{}.'", found_config.value().string()));
    Json parsed = Json::parse(*source);
    if (not parsed.contains("aliases")) return Expected<void>();
    for (auto [key, val] : parsed["aliases"].items()) {
        if (not val.is_string()) {
            return Unexpected(format("value in [{}] must be a string '{}'.", key, nlohmann::to_string(val)));
        }
        Path path{String(val)};
        auto with_user_folder = common::substitute_user_folder(path);
        if (auto err = get_if<common::error_trail>(&with_user_folder)) return Unexpected(*err);
        else path = get<fs::path>(with_user_folder);
        auto normalized = common::normalize_path(path);
        aliases.emplace(key, std::move(normalized.string()));
    }
    return Expected<void>();
}
static Expected<void> substitute_alias(String& str) {
    static const Regex alias_regex{R"(\@[A-Za-z][A-Za-z0-9_-]*)"};
    std::smatch sm;
    if (not std::regex_search(str, sm, alias_regex)) {
        return Unexpected(format("failed match allias regex {}", str));
    }
    const String alias = sm.str().substr(1);
    if (not aliases.contains(alias)) {
        return Unexpected(format("alias '{}' does not exist.", alias));
    }
    str.replace(sm.position(), sm.length(), aliases[alias]);
    return Expected<void>();
}
static int lazyrequire_handler(lua_State* L) {
    try {
        const String name = lua_tostring(L, lua_upvalueindex(1));
        dluau_require(L, name.c_str());
        lua_getmetatable(L, 1);
        lua_pushvalue(L, -2);
        lua_setfield(L, -2, "__index");
        lua_pop(L, 1);
        if (lua_istable(L, -1)) {
            lua_pushvalue(L, 2);
            lua_gettable(L, -2);
            lua_remove(L, -2);
        }
        return 1;
    } catch(std::exception& e) {
        luaL_errorL(L, e.what());
    }
}
int dluau_lazyrequire(lua_State* L, const char* name) {
    lua_newtable(L);
    lua_newtable(L);
    lua_pushstring(L, "__index");
    auto resolved = dluau::resolve_require_path(L, name);
    if (!resolved) dluau::error(L, resolved.error());
    dluau::push(L, *resolved);
    lua_pushcclosure(L, lazyrequire_handler, "lazyrequire_handler", 1);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    return 1;
}
int dluau_require(lua_State* L, const char* name) {
    auto resolved = ::dluau::resolve_require_path(L, name);
    if (not resolved) ::dluau::error(L, resolved.error());
    const String file_path{std::move(*resolved)};
    if (luamodules.contains(file_path)) {
        lua_getref(L, luamodules[file_path]);
        return 1;
    }
    auto source = common::read_file(file_path).value_or("");
    if (source.empty()) [[unlikely]] luaL_errorL(L, "couldn't read source '%s'", file_path.c_str());
    lua_State* M = lua_newthread(lua_mainthread(L));
    luaL_sandboxthread(M);
    script_paths.emplace(M, file_path);
    dluau::precompile(source, get_precompiled_library_values(file_path));
    size_t bc_len;
    char* bc_arr = luau_compile(source.data(), source.size(), ::dluau::compile_options, &bc_len);
    common::raii free_after([&bc_arr]{std::free(bc_arr);});
    String chunkname = file_path;
    chunkname = '@' + chunkname;
    int status{-1};
    if (luau_load(M, chunkname.c_str(), bc_arr, bc_len, 0) == LUA_OK) {
        status = lua_resume(M, L, 0);
        const int top = lua_gettop(M);
        switch (status) {
            case LUA_OK:
                if (top != 1) {
                    lua_pushstring(M, "module must return 1 value.");
                    status = -1;
                }
            break;
            case LUA_YIELD:
                lua_pushstring(M, "module can not yield.");
            break;
        }
    }
    lua_xmove(M, L, 1);
    if (status != LUA_OK) {
        luaL_errorL(L, lua_tostring(L, -1));
    }
    lua_pushvalue(L, -1);
    luamodules.emplace(file_path, lua_ref(L, -1));
    return 1;
}
static Opt<Path> find_source(Path p, const Path& base, Span<const String> file_exts = default_file_extensions) {
    if (p.is_relative()) p = base / p;
    if (not fs::exists(p)) {
        if (not p.has_extension()) {
            for (const String& ext : file_exts) {
                p.replace_extension(ext);
                if (fs::exists(p)) return p;
            }
        }
    } else if (fs::is_directory(p)) {
        for (const String& ext : file_exts) {
            p /= std::format("init{}", ext);
            if (fs::exists(p)) return p; 
            p = p.parent_path();
        }
    } else return p;
    return std::nullopt;
}
namespace dluau {
Expected<lua_State*> load_file(lua_State* L, Str_view path) {
    String script_path{path};
    Opt<String> source = common::read_file(script_path);
    if (not source) return Unexpected(format("couldn't read source '{}'.", script_path));
    auto normalized = common::normalize_path(script_path);
    dluau::precompile(*source, get_precompiled_library_values(normalized.string()));
    String identifier{fs::relative(script_path).string()};
    ranges::replace(identifier, '\\', '/');
    identifier = '=' + identifier;
    size_t outsize;
    char* bc = luau_compile(
        source->data(), source->size(),
        compile_options, &outsize
    );
    String bytecode{bc, outsize};
    std::free(bc);
    lua_State* script_thread = lua_newthread(L);
    script_paths.emplace(script_thread, normalized.string());
    const int load_status = luau_load(script_thread, identifier.c_str(), bytecode.data(), bytecode.size(), 0);
    if (load_status == LUA_OK) {
        luaL_sandboxthread(script_thread);
        return script_thread;
    }
    return Unexpected(format("failed to load '{}'\nreason: {}\nsource: {}", script_path, lua_tostring(script_thread, -1), *source));
}

Expected<String> resolve_require_path(lua_State* L, String name, Span<const String> file_exts) {
    if (not config_file_initialized) {
        config_file_initialized = true;
        if (auto loaded = load_aliases(); !loaded) return loaded.error();
    }
    if (not script_paths.contains(L)) return Unexpected("require is only allowed from a script thread");
    const Path script_path{Path(script_paths.at(L)).parent_path()};
    if (name[0] == '@') {
        if (auto success = substitute_alias(name); !success) return success.error();
    } else if (name[0] == '~') {
        auto with_user = common::substitute_user_folder(name);
        if (auto err = get_if<common::error_trail>(&with_user)) return err->propagate();
        name = get<fs::path>(with_user).string();
    }
    auto found_source = find_source(name, script_path, file_exts);
    if (not found_source) return Unexpected(format("couldn't find source for '{}'", name));
    return common::normalize_path(*found_source).string();
}
Expected<String> resolve_path(String name, const Path& base, Span<const String> file_exts) {
    if (has_alias(name)) {
        if (auto ok = substitute_alias(name); !ok) return ok.error();
    } else if (name[0] == '~') {
        auto with_user = common::substitute_user_folder(name);
        if (auto err = get_if<common::error_trail>(&with_user)) return err->propagate();
        name = get<fs::path>(with_user).string();
    }
    auto found_source = find_source(name, base, file_exts);
    if (not found_source) return Unexpected(format("couldn't find source for '{}'", name));
    return common::normalize_path(*found_source).string();
}
const Flat_map<String, String>& get_aliases() {
    return aliases;
}
const Flat_map<lua_State*, String>& get_script_paths() {
    return script_paths;
}
}
