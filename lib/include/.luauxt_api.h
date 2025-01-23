#ifdef LUAUXT_API_EXPORT
#define LUAUXT_API extern "C" __declspec(dllexport)
#else
#define LUAUXT_API extern "C" __declspec(dllimport)
#endif

#define LUMIN_FUNCS_API LUAUXT_API
#define LUMIN_UTILS_API LUAUXT_API
