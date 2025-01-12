#pragma once
#ifdef LUMIN_EXTRAS_EXPORT_API
#define LUMIN_EXTRAS_API extern "C" __declspec(dllexport)
#else
#define LUMIN_EXTRAS_API extern "C" __declspec(dllimport)
#endif
