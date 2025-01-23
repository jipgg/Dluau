#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include <luauxt.h>
//lua.h
lua_State* lua_newstate(lua_Alloc f, void* ud) {return 0;}
void lua_close(lua_State* L) {}
lua_State* lua_newthread(lua_State* L) {return 0;}
lua_State* lua_mainthread(lua_State* L) {return 0;}
void lua_resetthread(lua_State* L) {}
int lua_isthreadreset(lua_State* L) {return 0;}
int lua_absindex(lua_State* L, int idx) {return 0;}
int lua_gettop(lua_State* L) {return 0;}
void lua_settop(lua_State* L, int idx) {}
void lua_pushvalue(lua_State* L, int idx) {}
void lua_remove(lua_State* L, int idx) {}
void lua_insert(lua_State* L, int idx) {}
void lua_replace(lua_State* L, int idx) {}
int lua_checkstack(lua_State* L, int sz) {return 0;}
void lua_rawcheckstack(lua_State* L, int sz) {}
void lua_xmove(lua_State* from, lua_State* to, int n) {}
void lua_xpush(lua_State* from, lua_State* to, int idx) {}
int lua_isnumber(lua_State* L, int idx) {return 0;}
int lua_isstring(lua_State* L, int idx) {return 0;}
int lua_iscfunction(lua_State* L, int idx) {return 0;}
int lua_isLfunction(lua_State* L, int idx) {return 0;}
int lua_isuserdata(lua_State* L, int idx) {return 0;}
int lua_type(lua_State* L, int idx) {return 0;}
const char* lua_typename(lua_State* L, int tp) {return 0;}
int lua_equal(lua_State* L, int idx1, int idx2) {return 0;}
int lua_rawequal(lua_State* L, int idx1, int idx2) {return 0;}
int lua_lessthan(lua_State* L, int idx1, int idx2) {return 0;}
double lua_tonumberx(lua_State* L, int idx, int* isnum) {return 0;}
int lua_tointegerx(lua_State* L, int idx, int* isnum) {return 0;}
unsigned lua_tounsignedx(lua_State* L, int idx, int* isnum) {return 0;}
const float* lua_tovector(lua_State* L, int idx) {return 0;}
int lua_toboolean(lua_State* L, int idx) {return 0;}
const char* lua_tolstring(lua_State* L, int idx, size_t* len) {return 0;}
const char* lua_tostringatom(lua_State* L, int idx, int* atom) {return 0;}
const char* lua_namecallatom(lua_State* L, int* atom) {return 0;}
int lua_objlen(lua_State* L, int idx) {return 0;}
lua_CFunction lua_tocfunction(lua_State* L, int idx) {return 0;}
void* lua_tolightuserdata(lua_State* L, int idx) {return 0;}
void* lua_tolightuserdatatagged(lua_State* L, int idx, int tag) {return 0;}
void* lua_touserdata(lua_State* L, int idx) {return 0;}
void* lua_touserdatatagged(lua_State* L, int idx, int tag) {return 0;}
int lua_userdatatag(lua_State* L, int idx) {return 0;}
int lua_lightuserdatatag(lua_State* L, int idx) {return 0;}
lua_State* lua_tothread(lua_State* L, int idx) {return 0;}
void* lua_tobuffer(lua_State* L, int idx, size_t* len) {return 0;}
const void* lua_topointer(lua_State* L, int idx) {return 0;}
void lua_pushnil(lua_State* L) {}
void lua_pushnumber(lua_State* L, double n) {}
void lua_pushinteger(lua_State* L, int n) {}
void lua_pushunsigned(lua_State* L, unsigned n) {}
#if LUA_VECTOR_SIZE == 4
void lua_pushvector(lua_State* L, float x, float y, float z, float w);
#else
void lua_pushvector(lua_State* L, float x, float y, float z) {}
#endif
void lua_pushlstring(lua_State* L, const char* s, size_t l) {}
void lua_pushstring(lua_State* L, const char* s) {}
const char* lua_pushvfstring(lua_State* L, const char* fmt, va_list argp) {return 0;}
LUA_PRINTF_ATTR(2, 3) const char* lua_pushfstringL(lua_State* L, const char* fmt, ...) {return 0;}
void lua_pushcclosurek(lua_State* L, lua_CFunction fn, const char* debugname, int nup, lua_Continuation cont) {}
void lua_pushboolean(lua_State* L, int b) {}
int lua_pushthread(lua_State* L) {return 0;}
void lua_pushlightuserdatatagged(lua_State* L, void* p, int tag) {}
void* lua_newuserdatatagged(lua_State* L, size_t sz, int tag) {return 0;}
void* lua_newuserdatataggedwithmetatable(lua_State* L, size_t sz, int tag) {return 0;} // metatable fetched with lua_getuserdatametatable
void* lua_newuserdatadtor(lua_State* L, size_t sz, void (*dtor)(void*)) {return 0;}
void* lua_newbuffer(lua_State* L, size_t sz) {return 0;}
int lua_gettable(lua_State* L, int idx) {return 0;}
int lua_getfield(lua_State* L, int idx, const char* k) {return 0;}
int lua_rawgetfield(lua_State* L, int idx, const char* k) {return 0;}
int lua_rawget(lua_State* L, int idx) {return 0;}
int lua_rawgeti(lua_State* L, int idx, int n) {return 0;}
void lua_createtable(lua_State* L, int narr, int nrec) {}
void lua_setreadonly(lua_State* L, int idx, int enabled) {}
int lua_getreadonly(lua_State* L, int idx) {return 0;}
void lua_setsafeenv(lua_State* L, int idx, int enabled) {}
int lua_getmetatable(lua_State* L, int objindex) {return 0;}
void lua_getfenv(lua_State* L, int idx) {}
void lua_settable(lua_State* L, int idx) {}
void lua_setfield(lua_State* L, int idx, const char* k) {}
void lua_rawsetfield(lua_State* L, int idx, const char* k) {}
void lua_rawset(lua_State* L, int idx) {}
void lua_rawseti(lua_State* L, int idx, int n) {}
int lua_setmetatable(lua_State* L, int objindex) {return 0;}
int lua_setfenv(lua_State* L, int idx) {return 0;}
int luau_load(lua_State* L, const char* chunkname, const char* data, size_t size, int env) {return 0;}
void lua_call(lua_State* L, int nargs, int nresults) {}
int lua_pcall(lua_State* L, int nargs, int nresults, int errfunc) {return 0;}
int lua_yield(lua_State* L, int nresults) {return 0;}
int lua_break(lua_State* L) {return 0;}
int lua_resume(lua_State* L, lua_State* from, int narg) {return 0;}
int lua_resumeerror(lua_State* L, lua_State* from) {return 0;}
int lua_status(lua_State* L) {return 0;}
int lua_isyieldable(lua_State* L) {return 0;}
void* lua_getthreaddata(lua_State* L) {return 0;}
void lua_setthreaddata(lua_State* L, void* data) {}
int lua_costatus(lua_State* L, lua_State* co) {return 0;}
int lua_gc(lua_State* L, int what, int data) {return 0;}
void lua_setmemcat(lua_State* L, int category) {}
size_t lua_totalbytes(lua_State* L, int category) {return 0;}
l_noret lua_error(lua_State* L) {}
int lua_next(lua_State* L, int idx) {return 0;}
int lua_rawiter(lua_State* L, int idx, int iter) {return 0;}
void lua_concat(lua_State* L, int n) {}
uintptr_t lua_encodepointer(lua_State* L, uintptr_t p) {return 0;}
double lua_clock() {return 0;}
void lua_setuserdatatag(lua_State* L, int idx, int tag) {}
void lua_setuserdatadtor(lua_State* L, int tag, lua_Destructor dtor) {}
lua_Destructor lua_getuserdatadtor(lua_State* L, int tag) {return 0;}
void lua_setuserdatametatable(lua_State* L, int tag, int idx) {}
void lua_getuserdatametatable(lua_State* L, int tag) {}
void lua_setlightuserdataname(lua_State* L, int tag, const char* name) {}
const char* lua_getlightuserdataname(lua_State* L, int tag) {return 0;}
void lua_clonefunction(lua_State* L, int idx) {}
void lua_cleartable(lua_State* L, int idx) {}
lua_Alloc lua_getallocf(lua_State* L, void** ud) {return 0;}
#define LUA_NOREF -1
#define LUA_REFNIL 0
int lua_ref(lua_State* L, int idx) {return 0;}
void lua_unref(lua_State* L, int ref) {}
int lua_stackdepth(lua_State* L) {return 0;}
int lua_getinfo(lua_State* L, int level, const char* what, lua_Debug* ar) {return 0;}
int lua_getargument(lua_State* L, int level, int n) {return 0;}
const char* lua_getlocal(lua_State* L, int level, int n) {return 0;}
const char* lua_setlocal(lua_State* L, int level, int n) {return 0;}
const char* lua_getupvalue(lua_State* L, int funcindex, int n) {return 0;}
const char* lua_setupvalue(lua_State* L, int funcindex, int n) {return 0;}
void lua_singlestep(lua_State* L, int enabled) {}
int lua_breakpoint(lua_State* L, int funcindex, int line, int enabled) {return 0;}
void lua_getcoverage(lua_State* L, int funcindex, void* context, lua_Coverage callback) {}
const char* lua_debugtrace(lua_State* L) {return 0;}
lua_Callbacks* lua_callbacks(lua_State* L) {return 0;}
//lualib.h
void luaL_register(lua_State* L, const char* libname, const luaL_Reg* l) {}
int luaL_getmetafield(lua_State* L, int obj, const char* e) {return 0;}
int luaL_callmeta(lua_State* L, int obj, const char* e) {return 0;}
l_noret luaL_typeerrorL(lua_State* L, int narg, const char* tname) {}
l_noret luaL_argerrorL(lua_State* L, int narg, const char* extramsg) {}
const char* luaL_checklstring(lua_State* L, int numArg, size_t* l) {return 0;}
const char* luaL_optlstring(lua_State* L, int numArg, const char* def, size_t* l) {return 0;}
double luaL_checknumber(lua_State* L, int numArg) {return 0;}
double luaL_optnumber(lua_State* L, int nArg, double def) {return 0;}
int luaL_checkboolean(lua_State* L, int narg) {return 0;}
int luaL_optboolean(lua_State* L, int narg, int def) {return 0;}
int luaL_checkinteger(lua_State* L, int numArg) {return 0;}
int luaL_optinteger(lua_State* L, int nArg, int def) {return 0;}
unsigned luaL_checkunsigned(lua_State* L, int numArg) {return 0;}
unsigned luaL_optunsigned(lua_State* L, int numArg, unsigned def) {return 0;}
const float* luaL_checkvector(lua_State* L, int narg) {return 0;}
const float* luaL_optvector(lua_State* L, int narg, const float* def) {return 0;}
void luaL_checkstack(lua_State* L, int sz, const char* msg) {}
void luaL_checktype(lua_State* L, int narg, int t) {}
void luaL_checkany(lua_State* L, int narg) {}
int luaL_newmetatable(lua_State* L, const char* tname) {return 0;}
void* luaL_checkudata(lua_State* L, int ud, const char* tname) {return 0;}
void* luaL_checkbuffer(lua_State* L, int narg, size_t* len) {return 0;}
void luaL_where(lua_State* L, int lvl) {}
LUA_PRINTF_ATTR(2, 3) l_noret luaL_errorL(lua_State* L, const char* fmt, ...) {}
int luaL_checkoption(lua_State* L, int narg, const char* def, const char* const lst[]) {return 0;}
const char* luaL_tolstring(lua_State* L, int idx, size_t* len) {return 0;}
lua_State* luaL_newstate(void) {return 0;}
const char* luaL_findtable(lua_State* L, int idx, const char* fname, int szhint) {return 0;}
const char* luaL_typename(lua_State* L, int idx) {return 0;}
void luaL_buffinit(lua_State* L, luaL_Strbuf* B) {}
char* luaL_buffinitsize(lua_State* L, luaL_Strbuf* B, size_t size) {return 0;}
char* luaL_prepbuffsize(luaL_Buffer* B, size_t size) {return 0;}
void luaL_addlstring(luaL_Strbuf* B, const char* s, size_t l) {}
void luaL_addvalue(luaL_Strbuf* B) {}
void luaL_addvalueany(luaL_Strbuf* B, int idx) {}
void luaL_pushresult(luaL_Strbuf* B) {}
void luaL_pushresultsize(luaL_Strbuf* B, size_t size) {}
int luaopen_base(lua_State* L) {return 0;}
int luaopen_coroutine(lua_State* L) {return 0;}
int luaopen_table(lua_State* L) {return 0;}
int luaopen_os(lua_State* L) {return 0;}
int luaopen_string(lua_State* L) {return 0;}
int luaopen_bit32(lua_State* L) {return 0;}
int luaopen_buffer(lua_State* L) {return 0;}
int luaopen_utf8(lua_State* L) {return 0;}
int luaopen_math(lua_State* L) {return 0;}
int luaopen_debug(lua_State* L) {return 0;}
int luaopen_vector(lua_State* L) {return 0;}
void luaL_openlibs(lua_State* L) {}
void luaL_sandbox(lua_State* L) {}
void luaL_sandboxthread(lua_State* L) {}
//luacode.h
char* luau_compile(const char* source, size_t size, lua_CompileOptions* options, size_t* outsize) {return 0;}
void luau_set_compile_constant_nil(lua_CompileConstant* constant) {}
void luau_set_compile_constant_boolean(lua_CompileConstant* constant, int b) {}
void luau_set_compile_constant_number(lua_CompileConstant* constant, double n) {}
void luau_set_compile_constant_vector(lua_CompileConstant* constant, float x, float y, float z, float w) {}
void luau_set_compile_constant_string(lua_CompileConstant* constant, const char* s, size_t l) {}
//luacodegen.h
int luau_codegen_supported(void) {return 0;}
void luau_codegen_create(lua_State* L) {}
void luau_codegen_compile(lua_State* L, int idx) {}
//lumin.h
int luauxt_newuserdatatag() {return 0;}
int luauxt_newlightuserdatatag() {return 0;}
bool luauxt_samemeta(lua_State* L, int idx, const char* tname) {return 0;}
int16_t luauxt_defaultuseratom(const char* key, size_t len) {return 0i16;}
void luauxt_loadfuncs(lua_State* L) {}
int luauxt_registertypetagged(const char *tname) {return 0;}
void luauxt_registertype(const char *tname) {}
int luauxt_gettagfromtname(const char *tname) {return 0;}
bool luauxt_istyperegistered(const char *tname) {return 0;}
int luauxt_stringatom(lua_State *L, const char *key) {return 0;}
int luauxt_lstringatom(lua_State *L, const char *key, size_t len) {return 0;}
lua_State* luauxt_newstate() {return 0;}
void luauxt_init(lua_State* L) {}
lua_CompileOptions* lumin_compileopts{nullptr};
int luauxt_run(const goluau_GoOptions* opts) {return 0;}

lua_State* luminU_loadscript(lua_State* L, const char* script_path, size_t len) {return 0;}
const char* luminU_spawnscript(lua_State* L, const char* script_path, size_t len) {return 0;}
