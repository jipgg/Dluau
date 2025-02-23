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
## dluau_newuserdatatag
```c
DLUAU_API int dluau_newuserdatatag();
```

## dluau_newlightuserdatatag
```c
DLUAU_API int dluau_newlightuserdatatag();
```

## dluau_gettagfromtype
```c
DLUAU_API int dluau_gettagfromtype(const char* tname);
```

## dluau_istyperegistered

## dluau_registertype

## dluau_registertypetagged
```c
DLUAU_API int dluau_registertypetagged(const char* tname);
```

## dluau_findstringatom

## dluau_stringatom

## dluau_lstringatom

## dluau_run

## dluau_addctask

## dluau_tasksinprogress

## dluau_taskstep

## dluau_pushopaque

