#ifndef __MAIN_H__
#define __MAIN_H__

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


EXTERN_C {

bool DLL_EXPORT OpenHook(HINSTANCE);
void DLL_EXPORT CloseHook(void);

}

#endif // __MAIN_H__
