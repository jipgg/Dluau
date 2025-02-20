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
using std::expected;
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
        if (r) {
            src.replace(match.position(), match.length(), *r);
        } else {
            return std::unexpected(r.error());
        }
    }
    return src;
}
static auto replace_nameof_specifiers(const string& source) -> decltype(auto) {
    const regex expression(R"(\bnameof\((.*?)\))");
    auto to_string = [](const string& str) -> expected<string, string> {
        constexpr const char* fmt{"(\"{}\")"};
        std::println("NAMEOF HERE {}", str);
        /*
        if (str.find('.') != str.npos) {
            return format(fmt, str.substr(str.find_last_of('.') + 1));
        }
    */
        std::println("FORMAT HERE {}", format(fmt, str));
        return expected<string, string>(format(fmt, str));
    };
    return replace_meta_specifiers(source, expression, to_string);
}
auto dluau::preprocess_source(const fs::path& path) -> expected<Preprocessed_file, string> {
    auto source = common::read_file(path);
    if (not source) {
        return unexpected(format("couldn't read source '{}'.", path.string()));
    }
    Preprocessed_file data{};
    std::string src = *source;
    std::regex pattern(R"(std\.(\w+))");
    std::smatch match;
    while (std::regex_search(src, match, pattern)) {
        data.depends_on_std.emplace_back(match[1].str());
        src = match.suffix();
    }
    const fs::path dir = path.parent_path();
    //data.depends_on_scripts = expand_require_specifiers(*source, dir);
    data.normalized_path = common::normalize_path(path);
    //dluau::precompile(*source, get_precompiled_library_values(data.normalized_path));
    auto r = replace_nameof_specifiers(*source);
    if (!r) return std::unexpected(r.error());
    src = r.value();
    //std::println("\n\nSOURCE OF {}:\n{}\n\n", path.string(), src);

    string identifier{fs::relative(path).string()};
    std::ranges::replace(identifier, '\\', '/');
    identifier = '=' + identifier;
    data.identifier = std::move(identifier);
    data.source = std::move(src);
    return data;
}
auto dluau::expand_require_specifiers(string& source, const fs::path& base) -> std::vector<std::string> {
    regex pattern(R"(require\s*[\(\s]*["'\[\[]([^"'\]\)]+)["'\]\]]\s*\)?)");
    std::vector<std::string> expanded_sources;
    auto expanded = [&](const string& str) -> expected<string, string> {
        auto r = dluau::resolve_require_path(base, str);
        if (r) {
            auto resolved = r.value();
            expanded_sources.emplace_back(resolved);
            const std::string normalized = std::format("require(\"{}\")", resolved);
            return normalized;
        }
        return std::unexpected(r.error());
    };
    auto r = replace_meta_specifiers(source, pattern, expanded);
    return expanded_sources;
}
auto dluau::precompile(string &source, span<const pair<regex, string>> static_values) -> bool {
    source = replace_nameof_specifiers(source).value_or(source);
    for (const auto& v : static_values) {
        auto to_value = [&val = v.second](const string& e) {return val;};
        auto r = replace_meta_specifiers(source, v.first, to_value);
        if (r) source = r.value();
    }
    return true;
}

