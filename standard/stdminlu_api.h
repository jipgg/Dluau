#pragma once
#ifdef STDMINLU_EXPORT_API
#define STDMINLU_API extern "C" __declspec(dllexport)
#else
#define STDMINLU_API extern "C" __declspec(dllimport)
#endif
