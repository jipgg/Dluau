#include "shared.hpp"
#include <regex>
#include <format>
#include <span>
#include <functional>
using std::string, std::format;
using std::span, std::vector, std::function;
using std::smatch, std::regex, std::sregex_iterator;
using string_replace = function<string(const string& str)>;
namespace vw = std::views;
constexpr const char* str_format{"(\"{}\")"};

static bool is_global_scope(const string& input, const smatch& m) {
    if (m.position() == 0) return true;
    size_t position = m.position();
    char preceding = input[--position];
    while (preceding == '\n' or preceding == ' ' or preceding == '\t') {
        preceding = input[--position];
    }
    return preceding != '.' and preceding != ':';
}
static string name_to_string(const string& str) {
    if (str.find('.') != str.npos) {
        return str.substr(str.find_last_of('.') + 1);
    }
    return str;
}
static bool replace_meta_specifiers(string& source, const regex& expression, const string_replace& fn) {
    const sregex_iterator begin{source.begin(), source.end(), expression};
    const sregex_iterator end{};

    vector<smatch> entries;
    for (auto it = begin; it != end; ++it) {
        if (not is_global_scope(source, *it)) continue;
        entries.emplace_back(*it);
    }
    for (const smatch& match : vw::reverse(entries)) {
        constexpr const char* fmt{"(\"{}\")"};
        source.replace(match.position(), match.length(), format(fmt, fn(match.str(1))));
    }
    return not entries.empty();

}
static bool replace_nameof_specifiers(string& source) {
    const regex expression(R"(\bnameof\((.*?)\))");
    auto to_string = [](const string& str) {
        constexpr const char* fmt{"(\"{}\")"};
        if (str.find('.') != str.npos) {
            return str.substr(str.find_last_of('.') + 1);
        }
        return str;
    };
    return replace_meta_specifiers(source, expression, name_to_string);
}
bool shared::precompile(string &source) {
    return replace_nameof_specifiers(source);
}

char* dluau_precompile(const char* src, size_t src_len, size_t* outsize) {
    string source{src, src_len};
    if (not shared::precompile(source)) {
        *outsize = 0;
        return nullptr;
    }
    span<char> precompiled{static_cast<char*>(std::malloc(source.size())), source.size()};
    std::memcpy(precompiled.data(), source.data(), precompiled.size());
    *outsize = precompiled.size();
    return precompiled.data();
}
