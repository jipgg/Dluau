#ifdef LUMIN_CORE_EXPORT_API
#define LUMIN_CORE_API extern "C" __declspec(dllexport)
#else
#define LUMIN_CORE_API extern "C" __declspec(dllimport)
#endif

#define LUMIN_CORE_FUNCS_API LUMIN_CORE_API
