#pragma once
#ifdef MINLUAU_MODULES_EXPORT_API
#define MINLUAU_MODULES_API extern "C" __declspec(dllexport)
#else
#define MINLUAU_MODULES_API extern "C" __declspec(dllimport)
#endif
