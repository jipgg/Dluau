#ifdef LUMIN_EXPORT_API
#define LUMIN_API extern "C" __declspec(dllexport)
#else
#define LUMIN_API extern "C" __declspec(dllimport)
#endif

#define LUMINFUNCS_API LUMIN_API
