#include <dlfcn.h>
#include <windows.h>

static int DYN_OP_ERROR = 0;
#define DYN_OPEN_ERROR "dlopen error\n"
#define DYN_SYM_ERROR "dlsym error\n"

void* dlopen(const char*  filename, int flag)
{
    HMODULE hModule = NULL;
    hModule = LoadLibraryA(filename);
    if (NULL != hModule) {
        return hModule;
    }
}

void* dlsym(void*  handle, const char*  symbol)
{
    return GetProcAddress(handle, symbol);
}

int dlclose(void*  handle)
{
    return !FreeLibrary(handle);
}

int dlGetClassInfo(void *handle, void **pAddress, int *pSize)
{
    HRSRC   hResInfo = NULL;
    HGLOBAL hResData = NULL;
    LPVOID  pResAddr = NULL;
    DWORD resSize = 0;

    hResInfo = FindResource(handle, "#1", "ClassInfo");

    if (NULL == hResInfo) {
        goto E_FAIL_EXIT;
    }

    resSize = SizeofResource(handle, hResInfo);
    if (0 == resSize) {
        goto E_FAIL_EXIT;
    }

     hResData = LoadResource(handle, hResInfo);
    if (NULL == hResData) {
        goto E_FAIL_EXIT;
    }

    pResAddr = LockResource(hResData);
    if (NULL == pResAddr) {
        goto E_FAIL_EXIT;
    }

    *pAddress = pResAddr;
    *pSize = resSize;

    return 0;

E_FAIL_EXIT:
    return -1;
}

