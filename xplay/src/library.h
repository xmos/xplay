
#ifndef __LIBRARY_H__
#define __LIBRARY_H__

#if OS_DARWIN
#include <dlfcn.h>
#else   
#include <windows.h>
#endif

/*
 * #define RTLD_LAZY   1
 * #define RTLD_NOW    2
 * #define RTLD_GLOBAL 4
 * */

void* LoadSharedLibrary(char *pcDllname, int iMode = 2);
void *GetFunction(void *Lib, char *Fnname);
bool FreeSharedLibrary(void *hDLL);

#endif
