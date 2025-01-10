#pragma once
#ifdef LUMINSTD_EXPORT_API 
#define LUMINSTD_API extern "C" __declspec(dllexport)
#else
#define LUMINSTD_API extern "C" __declspec(dllimport)
#endif
