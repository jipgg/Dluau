#ifdef MINLUAU_EXPORT_API
#define MINLUAU_API extern "C" __declspec(dllexport)
#else
#define MINLUAU_API extern "C" __declspec(dllimport)
#endif
