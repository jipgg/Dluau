#include <dluau.hpp>
#include <common.hpp>
#include <boost/container/flat_map.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <optional>
#include <boost/regex.hpp>
namespace fs = std::filesystem;
using nlohmann::json;
using std::span;
using dluau::def_file_exts;
using boost::container::flat_map;
using std::string;
using std::expected, std::unexpected;
using fs::path, std::string_view;
using std::format, std::optional;

static flat_map<string, string> aliases;
static bool config_file_initialized{false};

static auto find_config_file(path base = fs::current_path(), int search_depth = 5) -> expected<path, string> {
    const std::array config_file_names = {
        ".luaurc", ".dluaurc",
        ".dluaurc.json", ".luaurc.json",
        "luaurc.json", "dluaurc.json",
    };
    auto exists = [&cfn = config_file_names, &base](path& out) -> bool {
        for (string_view name : cfn) {
            const path potential_path = base / name;
            if (fs::exists(potential_path)) {
                out = potential_path;
                return true;
            } 
        }
        return false;
    };
    int depth{};
    path potential_path;
    while (not exists(potential_path) and depth++ < search_depth) base = base.parent_path();
    if (potential_path.empty()) return std::unexpected("could not find config file");
    return potential_path;
}
static auto has_alias(const string& str) -> bool {
    return str[0] == '@';
}
static auto load_aliases(const path& base = fs::current_path(), int search_depth = 5) -> expected<void, string> {
    auto found_config = find_config_file(base, search_depth);
    if (not found_config) return expected<void, string>();

    auto source = common::read_file(*found_config);
    if (not source) return unexpected(std::format("couldn't read source '{}.'", found_config.value().string()));
    json parsed = json::parse(*source);
    if (not parsed.contains("aliases")) return expected<void, string>();
    for (auto [key, val] : parsed["aliases"].items()) {
        if (not val.is_string()) {
            return unexpected(format("value in [{}] must be a string '{}'.", key, nlohmann::to_string(val)));
        }
        path path{string(val)};
        auto with_user_folder = common::substitute_user_folder(path);
        if (not with_user_folder) return unexpected(with_user_folder.error());
        else path = *with_user_folder;
        auto normalized = common::normalize_path(path);
        aliases.emplace(key, std::move(normalized.string()));
    }
    return expected<void, string>();
}
static auto substitute_alias(string& str) -> expected<void, string> {
    static const boost::regex alias_regex{R"(\@[A-Za-z][A-Za-z0-9_-]*)"};
    boost::smatch sm;
    if (not boost::regex_search(str, sm, alias_regex)) {
        return unexpected(format("failed match allias regex {}", str));
    }
    const string alias = sm.str().substr(1);
    if (not aliases.contains(alias)) {
        return unexpected(format("alias '{}' does not exist.", alias));
    }
    str.replace(sm.position(), sm.length(), aliases[alias]);
    return expected<void, string>{};
}
static auto find_source(path p, const path& base, span<const string> file_exts = def_file_exts) -> optional<path> {
    if (p.is_relative()) p = base / p;
    if (not fs::exists(p)) {
        if (not p.has_extension()) {
            for (const string& ext : file_exts) {
                p.replace_extension(ext);
                if (fs::exists(p)) return p;
            }
        }
    } else if (fs::is_directory(p)) {
        for (const string& ext : file_exts) {
            p /= std::format("init{}", ext);
            if (fs::exists(p)) return p; 
            p = p.parent_path();
        }
    } else return p;
    return std::nullopt;
}

auto dluau::resolve_require_path(const fs::path& base, string name, span<const string> file_exts) -> expected<string, string> {
    if (not config_file_initialized) {
        config_file_initialized = true;
        if (auto loaded = load_aliases(); !loaded) return loaded.error();
    }
    const fs::path path_name{name};
    const bool maybe_preprocessed = path_name.is_absolute() and fs::is_regular_file(path_name) and path_name.extension() == ".luau";
    if (maybe_preprocessed) {
        //normalize just to be sure
        return common::normalize_path(path_name).string();
    }
    path script_path{base};
    if (name[0] == '@') {
        if (auto success = substitute_alias(name); !success) return success.error();
    } else if (name[0] == '~') {
        auto with_user = common::substitute_user_folder(name);
        if (not with_user) return unexpected(with_user.error());
        name = with_user.value().string();
    }
    auto found_source = find_source(name, script_path, file_exts);
    if (not found_source) {
        return unexpected(format("couldn't find source for '{}'", name));
    }
    return common::normalize_path(*found_source).string();
}
auto dluau::get_aliases() -> const flat_map<string, string>& {
    return aliases;
}
