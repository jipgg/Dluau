#include "shared.hpp"
#include <regex>
#include <format>
#include <span>
using std::string;
using std::span;

static bool is_free_function(const std::string& input, const std::smatch& m) {
    if (m.position() == 0) return true;
    size_t position = m.position();
    char preceding = input[--position];
    while (preceding == '\n' or preceding == ' ' or preceding == '\t') {
        preceding = input[--position];
    }
    return preceding != '.' and preceding != ':';
}
static std::string name_to_string(const std::string& str) {
    constexpr const char* fmt{"\"{}\""};
    if (str.find('.') != str.npos) {
        return std::format(fmt, str.substr(str.find_last_of('.') + 1));
    }
    return std::format(fmt, str);
}
static bool replace_nameof_specifiers(std::string& source) {
    const std::regex expression(R"(\bnameof\((.*?)\))");
    const std::sregex_iterator begin{source.begin(), source.end(), expression};
    const std::sregex_iterator end{};

    std::vector<std::smatch> entries;
    for (auto it = begin; it != end; ++it) {
        if (not is_free_function(source, *it)) continue;
        entries.emplace_back(*it);
    }
    for (const std::smatch& match : std::views::reverse(entries)) {
        source.replace(match.position(), match.length(), name_to_string(match.str(1)));
    }
    return not entries.empty();
}

bool shared::precompile(std::string &source) {
    return replace_nameof_specifiers(source);
}

char* dluau_precompile(const char* source_arr, size_t source_size, size_t* outsize) {
    string source{source_arr, source_size};
    if (not shared::precompile(source)) {
        *outsize = 0;
        return nullptr;
    }
    span<char> precompiled{static_cast<char*>(std::malloc(source.size())), source.size()};
    std::memcpy(precompiled.data(), source.data(), precompiled.size());
    *outsize = precompiled.size();
    return precompiled.data();
}
