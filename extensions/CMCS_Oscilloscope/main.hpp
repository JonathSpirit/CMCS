#ifndef _CMCSOSCILLOSCOPE_MAIN_HPP_INCLUDED
#define _CMCSOSCILLOSCOPE_MAIN_HPP_INCLUDED

#include <windows.h>

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

void DLL_EXPORT SomeFunction(const LPCSTR sometext);

#ifdef __cplusplus
}
#endif

#endif // _CMCSOSCILLOSCOPE_MAIN_HPP_INCLUDED
