#ifdef GOLUAU_API_EXPORT
#define GOLUAU_API extern "C" __declspec(dllexport)
#else
#define GOLUAU_API extern "C" __declspec(dllimport)
#endif

#define LUMIN_FUNCS_API GOLUAU_API
#define LUMIN_UTILS_API GOLUAU_API
