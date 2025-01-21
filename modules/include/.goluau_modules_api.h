#pragma once
#ifdef GOLUAU_MODULES_API_EXPORT
#define GOLUAU_MODULES_API extern "C" __declspec(dllexport)
#else
#define GOLUAU_MODULES_API extern "C" __declspec(dllimport)
#endif
