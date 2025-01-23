#pragma once
#ifdef LUAUXTM_API_EXPORT
#define LUAUXTM_API extern "C" __declspec(dllexport)
#else
#define LUAUXTM_API extern "C" __declspec(dllimport)
#endif
