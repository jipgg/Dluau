#ifdef LUMIN_CORE_EXPORT_API
#define LUMIN_API extern "C" __declspec(dllexport)
#define LUMIN_CPP_API __declspec(dllexport)
#else
#define LUMIN_API extern "C" __declspec(dllimport)
#define LUMIN_CPP_API __declspec(dllimport)
#endif

#define LUMIN_FUNCS_API LUMIN_API
#define LUMIN_UTILS_API LUMIN_API
