#include <dluau.hpp>
#include <regex>
#include <format>
#include <common.hpp>
#include <span>
#include <functional>
using namespace dluau::type_aliases;
using String_replace = Func<String(const String& str)>;
constexpr const char* str_format{"(\"{}\")"};

static bool is_global_scope(const String& input, const Smatch& m) {
    if (m.position() == 0) return true;
    size_t position = m.position();
    char preceding = input[--position];
    while (preceding == '\n' or preceding == ' ' or preceding == '\t') {
        preceding = input[--position];
    }
    return preceding != '.' and preceding != ':';
}
static String name_to_string(const String& str) {
    if (str.find('.') != str.npos) {
        return str.substr(str.find_last_of('.') + 1);
    }
    return str;
}
static bool replace_meta_specifiers(String& source, const Regex& expression, const String_replace& fn) {
    const Sregex_iterator begin{source.begin(), source.end(), expression};
    const Sregex_iterator end{};

    Vector<Smatch> entries;
    for (auto it = begin; it != end; ++it) {
        if (not is_global_scope(source, *it)) continue;
        entries.emplace_back(*it);
    }
    for (const Smatch& match : views::reverse(entries)) {
        //constexpr const char* fmt{"(\"{}\")"};
        source.replace(match.position(), match.length(), fn(match.str(1)));
    }
    return not entries.empty();

}
static bool replace_nameof_specifiers(String& source) {
    const Regex expression(R"(\bnameof\((.*?)\))");
    auto to_string = [](const String& str) {
        constexpr const char* fmt{"(\"{}\")"};
        if (str.find('.') != str.npos) {
            return format(fmt, str.substr(str.find_last_of('.') + 1));
        }
        return format(fmt, str);
    };
    return replace_meta_specifiers(source, expression, to_string);
}
bool dluau::precompile(String &source) {
    return replace_nameof_specifiers(source);
}
bool dluau::precompile(String &source, Span<const Pair<Regex, String>> static_values) {
    bool did_something{};
    did_something = replace_nameof_specifiers(source);
    for (const auto& v : static_values) {
        auto to_value = [&val = v.second](const String& e) -> String {return val;};
        did_something = replace_meta_specifiers(source, v.first, to_value);
    }
    return did_something;
}

char* dluau_precompile(const char* src, size_t src_len, size_t* outsize) {
    String source{src, src_len};
    if (not dluau::precompile(source)) {
        *outsize = 0;
        return nullptr;
    }
    Span<char> precompiled{static_cast<char*>(std::malloc(source.size())), source.size()};
    std::memcpy(precompiled.data(), source.data(), precompiled.size());
    *outsize = precompiled.size();
    return precompiled.data();
}
