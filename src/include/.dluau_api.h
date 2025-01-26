#ifdef DLUAU_LIB_EXPORT
#define DLUAU_API extern "C" __declspec(dllexport)
#else
#define DLUAU_API extern "C" __declspec(dllimport)
#endif

