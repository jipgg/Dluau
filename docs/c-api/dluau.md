# Types
## dluau_RunOptions `struct`
```c
struct dluau_RunOptions {
    const char* scripts;
    const char* args;
    const luaL_Reg* global_functions;
    int debug_level;
    int optimization_level;
};
```
### Fields
#### scripts `cont char*`
The scripts to run separated with ','.
#### args `const char*`
The startup args passed to the runtime separated with ','.
#### global_functions `const luaL_Reg*`
#### debug_level `int`
The debug level \[0, 2] passed to the internal `lua_CompileOptions`
#### optimization_level `int`
The optimization level \[0, 2] passed to the internal `lua_CompileOptions`


## dluau_CTaskStatus `enum`
```c
enum dluau_CTaskStatus {
    DLUAU_CTASK_DONE,
    DLUAU_CTASK_CONTINUE,
    DLUAU_CTASK_ERROR,
};
```
## dluau_CTask `function pointer`
```c
typedef dluau_CTaskStatus(*dluau_CTask)(const char** errmsg);
```

## dluau_Opaque `alias`
```c
typedef void* dluau_Opaque;
```

## dluau_Dlmodule `alias`
```c
struct dluau_Dlmodule;
```
Opaque pointer to a dlmodule object.

# Functions
## dluau_newuserdatatag `(): int`

## dluau_newlightuserdatatag `(): int`

## dluau_gettagfromtype `(const char*): int`

## dluau_istyperegistered `(const char*): bool`

## dluau_registertype `(const char*): void`

## dluau_findstringatom `(int): const char*`

## dluau_stringatom `(lua_State*, const char*): int`

## dluau_lstringatom `(lua_State*, const char*, size_t): int`

## dluau_run `(const dluau_RunOptions*): int`

## dluau_addctask `(dluau_CTask): void`

## dluau_tasksinprogress `(): bool`

## dluau_taskstep `(lua_State*): bool`

## dluau_pushopaque `(lua_State*, dluau_Opaque): void`

