#ifndef PLUTO_API

/* symbol export/import */
#ifdef _WIN32
    #ifdef PLUTO_BUILD_DLL
        #define PLUTO_API __declspec(dllexport)
    #else
        #define PLUTO_API __declspec(dllimport)
    #endif
#else
    #if __GNUC__ >= 4
        #define PLUTO_API __attribute__ ((visibility ("default")))
    #else
        #define PLUTO_API
    #endif
#endif

#endif
