#pragma once
#ifdef HALUA_API_EXPORT
#define HALUA_API extern "C" __declspec(dllexport)
#else
#define HALUA_API extern "C" __declspec(dllimport)
#endif
