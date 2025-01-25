#include <shared.hpp>
#include <dluau.h>

namespace shared::context_utils {
bool has_permissions(lua_State* L) {
    lua_Debug ar;
    if (not lua_getinfo(L, 1, "s", &ar)) return false;
    if (ar.source[0] == '@' or ar.source[0] == '=') return true;
    return false;
}
}
