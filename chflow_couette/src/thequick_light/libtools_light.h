//===========================================================================
#ifndef libtools_lightH
#define libtools_lightH
//===========================================================================
#include <string>
#include <iostream>

#if defined _WIN32
    #include <windows.h>
    #if defined(_WINDLL)
        #define MAC_DLLEXPORT __declspec(dllexport)
        #define MAC_DLLEXPORT_FROMEXE __declspec(dllimport)
    #else
        #define MAC_DLLEXPORT __declspec(dllimport)
        #define MAC_DLLEXPORT_FROMEXE __declspec(dllexport)
    #endif
#elif defined __linux__
    #include <dlfcn.h>
    #define MAC_DLLEXPORT
    #define MAC_DLLEXPORT_FROMEXE
#endif

//===========================================================================
#endif
//===========================================================================
