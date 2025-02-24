#include <dluau.hpp>
#include <format>
#include <common.hpp>
#include <print>
#include <functional>
#include <boost/regex.hpp>
using std::string, std::size_t;
using std::function, std::vector;
using boost::smatch, boost::regex;
using std::expected, std::unexpected;
using String_replace = function<expected<string, string>(const string& str)>;
namespace vws = std::views;
constexpr const char* str_format{"(\"{}\")"};

/*static auto remove_brackets(const std::string& str) -> std::string_view {*/
/*    std::string_view sv = str;*/
/*    while (!sv.empty() and sv.front() == '(') {*/
/*        sv.remove_prefix(1);*/
/*    }*/
/*    while (!sv.empty() and sv.back() == ')') {*/
/*        sv.remove_suffix(1);*/
/*    }*/
/*    return sv;*/
/*}*/
template <std::same_as<char> ...Chars>
static auto trim_sides(std::string_view sv, Chars&&...cs) -> std::string_view {
    while (!sv.empty() and ((sv.front() == cs) or ...)) {
        sv.remove_prefix(1);
    }
    while (!sv.empty() and ((sv.back() == cs) or ...)) {
        sv.remove_suffix(1);
    }
    return sv;
};
static auto is_global_scope(const string& input, const boost::smatch& m) -> bool {
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
static auto replace_meta_specifiers(string source, const boost::regex& expression, const String_replace& fn) -> expected<string, string> {
    const boost::sregex_iterator begin{source.begin(), source.end(), expression};
    const boost::sregex_iterator end{};

    vector<boost::smatch> entries;
    string src = source;
    for (auto it = begin; it != end; ++it) {
        if (not is_global_scope(source, *it)) continue;
        entries.emplace_back(*it);
    }
    for (const boost::smatch& match : vws::reverse(entries)) {
        auto r = fn(match.str(1));
        if (!r) return unexpected(r.error());
        src.replace(match.position(), match.length(), *r);
    }
    return src;
}
static auto replace_nameof_specifiers(const string& source) -> decltype(auto) {
    const boost::regex expression(R"(\bnameof\(*(.*?)\)*)");
    auto to_string = [](const string& str) -> expected<string, string> {
        constexpr const char* fmt{"(\"{}\")"};
        if (str.find('.') != str.npos) {
            return format(fmt, str.substr(str.find_last_of('.') + 1));
        }
        return expected<string, string>(format(fmt, str));
    };
    return replace_meta_specifiers(source, expression, to_string);
}
static auto replace_dlsearch_specifiers(const string& source) -> decltype(auto) {
    const boost::regex expression(R"(\bdlsearch\(*(.*?)\)\)*)");
    auto to_string = [](const string& str) -> expected<string, string> {
        constexpr const char* fmt{"(\"{}\")"};
        auto dlpath = trim_sides(str, '\"', '\'', '`');
        auto path = dluau::search_path(dlpath);
        if (not path) return unexpected(std::format("Couldn't find dl path for '{}'", dlpath));
        return std::format(fmt, path->string());
    };
    return replace_meta_specifiers(source, expression, to_string);
}
auto dluau::preprocess_script(const fs::path& path) -> expected<Preprocessed_script, string> {
    auto source = common::read_file(path);
    if (not source) {
        return unexpected(format("couldn't read source '{}'.", path.string()));
    }
    Preprocessed_script data{};

    auto nameof_done = replace_nameof_specifiers(*source);
    if (nameof_done) *source = nameof_done.value();
    else return unexpected(nameof_done.error());
    auto dlfind_done = replace_dlsearch_specifiers(*source);
    if (dlfind_done) *source = dlfind_done.value();
    else return unexpected(dlfind_done.error());

    std::string src = *source;
    boost::regex pattern(R"(std\.(\w+))");
    boost::smatch match;
    while (boost::regex_search(src, match, pattern)) {
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

    string identifier{fs::relative(path).string()};
    std::ranges::replace(identifier, '\\', '/');
    identifier = '=' + identifier;
    data.identifier = std::move(identifier);
    data.source = std::move(*source);
    return data;
}
auto dluau::expand_require_specifiers(string& source, const fs::path& base, string_view fname) -> expected<vector<string>, string> {
    boost::regex pattern(std::format(R"({}\s*[\(\s]*["'\[\[]([^"'\]\)]+)["'\]\]]\s*\)?)", fname));
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

