#pragma once
#if defined(_WIN32) || defined(_WIN64)
    #ifdef BUILD_HALUA_STD_DLL
        #define HALUA_STD_API extern "C" __declspec(dllexport)
    #else
        #define HALUA_STD_API extern "C" __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef BUILD_HALUA_STD_DLL
        #define HALUA_STD_API extern "C" __attribute__((visibility("default")))
    #else
        #define HALUA_STD_API extern "C"
    #endif
#else
    #define HALUA_STD_API 
    #pragma warning Unknown dynamic link import/export semantics
#endif
