#include <dluau.hpp>
#include <regex>
#include <format>
#include <common.hpp>
#include <span>
#include <print>
#include <functional>
#include <set>
using std::string, std::size_t;
using std::regex, std::smatch, std::sregex_iterator;
using std::function, std::vector;
using std::span, std::pair;
using std::expected, std::unexpected;
using String_replace = function<expected<string, string>(const string& str)>;
namespace vws = std::views;
namespace fs = std::filesystem;
constexpr const char* str_format{"(\"{}\")"};

static auto is_global_scope(const string& input, const smatch& m) -> bool {
    if (m.position() == 0) return true;
    size_t position = m.position();
    char preceding = input[--position];
    while (preceding == '\n' or preceding == ' ' or preceding == '\t') {
        preceding = input[--position];
    }
    return preceding != '.' and preceding != ':';
}
static auto name_to_string(const string& str) -> string {
    if (str.find('.') != str.npos) {
        return str.substr(str.find_last_of('.') + 1);
    }
    return str;
}
static auto replace_meta_specifiers(string source, const regex& expression, const String_replace& fn) -> expected<string, string> {
    const sregex_iterator begin{source.begin(), source.end(), expression};
    const sregex_iterator end{};

    vector<smatch> entries;
    string src = source;
    for (auto it = begin; it != end; ++it) {
        if (not is_global_scope(source, *it)) continue;
        entries.emplace_back(*it);
    }
    for (const smatch& match : vws::reverse(entries)) {
        auto r = fn(match.str(1));
        if (!r) return unexpected(r.error());
        src.replace(match.position(), match.length(), *r);
    }
    return src;
}
static auto replace_nameof_specifiers(const string& source) -> decltype(auto) {
    const regex expression(R"(\bnameof\((.*?)\))");
    auto to_string = [](const string& str) -> expected<string, string> {
        constexpr const char* fmt{"(\"{}\")"};
        if (str.find('.') != str.npos) {
            return format(fmt, str.substr(str.find_last_of('.') + 1));
        }
        return expected<string, string>(format(fmt, str));
    };
    return replace_meta_specifiers(source, expression, to_string);
}
static auto precompile(string &source, span<const pair<regex, string>> static_values) -> expected<void, string> {
    auto r = replace_nameof_specifiers(source);
    if (r) source = r.value();
    else return unexpected(r.error());
    for (const auto& v : static_values) {
        auto to_value = [&val = v.second](const string& e) {return val;};
        auto r = replace_meta_specifiers(source, v.first, to_value);
        if (r) source = r.value();
        else return unexpected(r.error());
    }
    return expected<void, string>{};
}
auto dluau::preprocess_script(const fs::path& path) -> expected<Preprocessed_script, string> {
    auto source = common::read_file(path);
    if (not source) {
        return unexpected(format("couldn't read source '{}'.", path.string()));
    }
    Preprocessed_script data{};
    std::string src = *source;
    std::regex pattern(R"(std\.(\w+))");
    std::smatch match;
    while (std::regex_search(src, match, pattern)) {
        data.depends_on_std.emplace_back(match[1].str());
        src = match.suffix();
    }
    const fs::path dir = path.parent_path();
    auto script_dependencies = expand_require_specifiers(*source, dir);
    if (!script_dependencies) return unexpected(script_dependencies.error());
    data.depends_on_scripts = std::move(script_dependencies.value());
    data.normalized_path = common::normalize_path(path);
    auto dlload_dependencies = expand_require_specifiers(*source, dir, "dlload");
    if (!dlload_dependencies) return unexpected(dlload_dependencies.error());
    data.depends_on_dls = std::move(dlload_dependencies.value());
    auto dlrequire_dependencies = expand_require_specifiers(*source, dir, "dlrequire");
    if (!dlrequire_dependencies) return unexpected(dlrequire_dependencies.error());
    data.depends_on_dls.append_range(std::move(dlrequire_dependencies.value()));
    auto precompiled = precompile(*source, get_precompiled_script_library_values(data.normalized_path));
    if (!precompiled) return unexpected(precompiled.error());

    string identifier{fs::relative(path).string()};
    std::ranges::replace(identifier, '\\', '/');
    identifier = '=' + identifier;
    data.identifier = std::move(identifier);
    data.source = std::move(*source);
    return data;
}
auto dluau::expand_require_specifiers(string& source, const fs::path& base, string_view fname) -> expected<vector<string>, string> {
    regex pattern(std::format(R"({}\s*[\(\s]*["'\[\[]([^"'\]\)]+)["'\]\]]\s*\)?)", fname));
    std::vector<std::string> expanded_sources;
    auto expanded = [&](const string& str) -> expected<string, string> {
        auto r = dluau::resolve_require_path(base, str);
        if (!r) return unexpected(r.error());
        if (r) {
            auto resolved = r.value();
            expanded_sources.emplace_back(resolved);
            const std::string normalized = std::format("{}(\"{}\")", fname, resolved);
            return normalized;
        }
        return std::unexpected(r.error());
    };
    auto r = replace_meta_specifiers(source, pattern, expanded);
    if (!r) return unexpected(r.error());
    source = r.value(); 
    return expanded_sources;
}

