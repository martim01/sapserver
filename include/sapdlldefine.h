#pragma once

#ifdef __WIN32__
    #ifdef SAP_DLL
        #define SAP_EXPORT __declspec(dllexport)
    #else
        #define SAP_EXPORT __declspec(dllimport)
    #endif // DNSSD_EXPORT
#else
#define DNSSD_EXPORT
#endif

