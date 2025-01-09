#ifdef MINLU_EXPORT_API
#define MINLU_API extern "C" __declspec(dllexport)
#else
#define MINLU_API extern "C" __declspec(dllimport)
#endif
