

#include "library.h"
#include "logging.h"
#include <iostream>


/*
 * #define RTLD_LAZY   1
 * #define RTLD_NOW    2
 * #define RTLD_GLOBAL 4
 * */

void* LoadSharedLibrary(char *pcDllname, int iMode)
{
    std::string sDllName = pcDllname;
#if defined(OS_WIN32) 
    //sDllName += ".dll";
    return (void*)LoadLibrary(pcDllname);
#elif defined(OS_DARWIN)
    //sDllName += ".so";
    return dlopen(sDllName.c_str(), iMode);
#else
    #error    
#endif
}
void *GetFunction(void *Lib, char *Fnname)
{
#if defined(OS_WIN32) 
    return (void*)GetProcAddress((HINSTANCE)Lib,Fnname);
#elif defined(OS_DARWIN)
    
    /* Reset errors */
    dlerror();

    /* Load the symbol */
    void * ptr = dlsym(Lib,Fnname);

    const char *dlsym_error = dlerror();
    
    if (dlsym_error) 
    {
        report_error (dlsym_error);
        dlclose(Lib);
        return NULL;
    }
    
    return ptr;
#endif
}

bool FreeSharedLibrary(void *hDLL)
{
#if defined(OS_WIN32) 
    return FreeLibrary((HINSTANCE)hDLL);
#elif defined(OS_DARWIN)
    return dlclose(hDLL);
#endif
}

