#include <dluau.h>
#include <unordered_map>
#include <ranges>
#include <algorithm>
#include <dluau.hpp>
namespace ranges = std::ranges;
using namespace dluau;
static std::unordered_map<String, int16_t> stringatom_registry;
static int16_t current_stringatom_value{0};

int16_t dluau::default_useratom(const char* key, size_t size) {
    const String key_value{key, size};
    if (not stringatom_registry.contains(key_value)) {
        stringatom_registry.emplace(key_value, ++current_stringatom_value);
    }
    return stringatom_registry.at(key_value);
}
const char* dluau_findstringatom(int atom) {
    auto predicate = [&atom](const Pair<String, int16_t>& pair) {return pair.second == atom;};
    auto found = ranges::find_if(stringatom_registry, predicate);
    if (found != ranges::end(stringatom_registry)) return found->first.c_str();
    return nullptr;
}
int dluau_stringatom(lua_State* L, const char *key) {
    if (stringatom_registry.contains(key)) return stringatom_registry.at(key);
    lua_pushstring(L, key);
    int atom;
    lua_tostringatom(L, -1, &atom);
    lua_pop(L, 1);
    return atom;
}
int dluau_lstringatom(lua_State* L, const char *key, size_t len) {
    const String key_value{key, len};
    if (stringatom_registry.contains(key_value)) return stringatom_registry.at(key_value);
    lua_pushlstring(L, key, len);
    int atom;
    lua_tostringatom(L, -1, &atom);
    lua_pop(L, 1);
    return atom;
}
