#ifndef PML_SAPSERVER_DLL_H
#define PML_SAPSERVER_DLL_H

#ifdef __WIN32
    #ifdef SAP_DLL
        #define SAP_EXPORT __declspec(dllexport)
    #else
        #define SAP_EXPORT __declspec(dllimport)
    #endif // DNSSD_EXPORT
#else
#define SAP_EXPORT
#endif

#endif