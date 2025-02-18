#pragma once
#ifdef DLUAUSTD_EXPORT
#define DLUAUSTD_API extern "C" __declspec(dllexport)
#else
#define DLUAUSTD_API extern "C" __declspec(dllimport)
#endif
