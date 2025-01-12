#pragma once
#ifdef LUMIN_MODULES_EXPORT_API
#define LUMIN_MODULES_API extern "C" __declspec(dllexport)
#else
#define LUMIN_MODULES_API extern "C" __declspec(dllimport)
#endif
