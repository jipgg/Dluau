#include <dluau.h>
#include <unordered_map>
#include <shared.hpp>
static std::unordered_map<std::string, int16_t> stringatom_registry;
static int16_t current_stringatom_value{0};

int16_t shared::default_useratom(const char* key, size_t size) {
    const std::string key_value{key, size};
    if (not stringatom_registry.contains(key_value)) {
        stringatom_registry.emplace(key_value, ++current_stringatom_value);
    }
    return stringatom_registry.at(key_value);
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
    const std::string key_value{key, len};
    if (stringatom_registry.contains(std::string(key_value))) return stringatom_registry.at(key_value);
    lua_pushlstring(L, key, len);
    int atom;
    lua_tostringatom(L, -1, &atom);
    lua_pop(L, 1);
    return atom;
}
