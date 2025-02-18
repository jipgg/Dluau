#include <dluau.h>
#include <unordered_map>
#include <ranges>
#include <algorithm>
#include <dluau.hpp>
namespace rngs = std::ranges;
using namespace dluau;
using std::string, std::pair;
static std::unordered_map<string, int16_t> stringatom_registry;
static int16_t current_stringatom_value{0};

auto dluau::default_useratom(const char* key, size_t size) -> int16_t {
    const string key_value{key, size};
    if (not stringatom_registry.contains(key_value)) {
        stringatom_registry.emplace(key_value, ++current_stringatom_value);
    }
    return stringatom_registry.at(key_value);
}
auto dluau_findstringatom(int atom) -> const char* {
    auto predicate = [&atom](const pair<string, int16_t>& pair) {return pair.second == atom;};
    auto found = rngs::find_if(stringatom_registry, predicate);
    if (found != rngs::end(stringatom_registry)) return found->first.c_str();
    return nullptr;
}
auto dluau_stringatom(lua_State* L, const char *key) -> int {
    if (stringatom_registry.contains(key)) return stringatom_registry.at(key);
    lua_pushstring(L, key);
    int atom;
    lua_tostringatom(L, -1, &atom);
    lua_pop(L, 1);
    return atom;
}
auto dluau_lstringatom(lua_State* L, const char *key, size_t len) -> int {
    const string key_value{key, len};
    if (stringatom_registry.contains(key_value)) return stringatom_registry.at(key_value);
    lua_pushlstring(L, key, len);
    int atom;
    lua_tostringatom(L, -1, &atom);
    lua_pop(L, 1);
    return atom;
}
