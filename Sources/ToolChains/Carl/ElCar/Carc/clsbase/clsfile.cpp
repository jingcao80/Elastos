//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#ifdef _linux
#include <sys/io.h>
#else
#include <io.h>
#endif
#include <fcntl.h>

#include "clsbase.h"

#ifdef _linux
#define _alloca alloca

#define _MAX_PATH 256

#define _stricmp strcasecmp
#endif

static const char *s_pszLibraryPath = NULL;
static const char *s_pszPublicPath = NULL;

#ifdef _linux
extern int LoadResourceFromELF(const char *pszName, CLSModule **ppDest);
#endif


#ifdef _win32

typedef void *HRSRC;
typedef void *HGLOBAL;
typedef void *HMODULE;
typedef void *HANDLE;

#define LOAD_LIBRARY_AS_DATAFILE 0x00000002

EXTERN_C HANDLE  __stdcall LoadLibraryExA(
                   const char* lpFileName,  // file name of module
                   HANDLE hFile,           // reserved, must be NULL
                   DWORD dwFlags           // entry-point execution option
                  );

EXTERN_C HRSRC __stdcall FindResourceA(
    HMODULE hModule,
    char* lpName,
    char* lpType
    );

EXTERN_C DWORD __stdcall SizeofResource(
    HMODULE hModule,
    HRSRC hResInfo
    );

EXTERN_C HGLOBAL __stdcall LoadResource(
    HMODULE hModule,
    HRSRC hResInfo
    );

EXTERN_C void* __stdcall LockResource(
        HGLOBAL hResData
        );

EXTERN_C bool __stdcall FreeLibrary(
                   HMODULE hModule
                  );

#endif // _win32

void SetLibraryPath(const char *pszPath)
{
    s_pszLibraryPath = pszPath;
}

int SaveCLS(const char *pszName, const CLSModule *pModule)
{
    int nSize;
    FILE *pFile;
    void *pDest;

    pFile = fopen(pszName, "w+b");
    if (!pFile) {
        ExtraMessage(pszName);
        _ReturnError (CLSError_OpenFile);
    }

    nSize = FlatCLS(pModule, &pDest);
    if (nSize < 0) _Return (nSize);

    fwrite(pDest, 1, nSize, pFile);

    DisposeFlattedCLS(pDest);
    _ReturnOK (CLS_NoError);
}

int LoadCLSFromFile(const char *pszName, CLSModule **ppDest)
{
    int r, nLength, nRet = CLSError_OutOfMemory;
    FILE *pFile;
    CLSModule *pSrc;

    pFile = fopen(pszName, "rb");
    if (!pFile) {
        ExtraMessage(pszName);
        _ReturnError(CLSError_OpenFile);
    }

    fseek(pFile, 0L, SEEK_END);
    nLength = ftell(pFile);

    pSrc = (CLSModule *)_alloca(nLength);
    if (!pSrc) goto ErrorExit;

    fseek(pFile, 0L, SEEK_SET);
    fread(pSrc, 1, nLength, pFile);

    r = IsValidCLS(pSrc, nLength, pszName);

    nRet = (r < 0) ? r : RelocFlattedCLS(pSrc, nLength, ppDest);

ErrorExit:
    fclose(pFile);
    _Return(nRet);
}

#ifdef _win32
int LoadResourceFromPE(const char *pszName, CLSModule **ppDest)
{
    UINT nSize;
    void *lpLockRes;
    int nRet, r;
    HRSRC hResInfo;
    HGLOBAL hRes;
    HMODULE handle;

    if (pszName) {
        handle = LoadLibraryExA(pszName, NULL, LOAD_LIBRARY_AS_DATAFILE);
        if (!handle) {
            ExtraMessage(pszName, "LoadLibraryExA");
            _ReturnError (CLSError_LoadResource);
        }
    } else {
        handle = NULL;
    }

    hResInfo = FindResourceA(handle, "#1", "ClassInfo");
    if (!hResInfo) {
        ExtraMessage(pszName, "FindResourceA", "<#1 ClassInfo>");
        _ReturnError (CLSError_LoadResource);
    }

    nSize = SizeofResource(handle, hResInfo);
    if (0 == nSize) {
        ExtraMessage(pszName, "SizeofResource", "0");
        _ReturnError (CLSError_LoadResource);
    }

    hRes = LoadResource(handle, hResInfo);
    if (!hRes) {
        ExtraMessage(pszName, "LoadResource");
        _ReturnError (CLSError_LoadResource);
    }

    lpLockRes = LockResource(hRes);
    if (!lpLockRes) {
        ExtraMessage(pszName, "LockResource");
        _ReturnError (CLSError_LoadResource);
    }

    r = IsValidCLS((CLSModule *)lpLockRes, nSize, pszName);
    nRet = (r < 0) ? r : RelocFlattedCLS((CLSModule *)lpLockRes, nSize, ppDest);
    FreeLibrary(handle);
    return nRet;
}
#endif


int LoadCLSFromDll(const char *pszName, CLSModule **ppDest)
{
    FILE *pFile  = NULL;

    if( pszName == NULL ){
#ifdef _linux
        return LoadResourceFromELF(pszName, ppDest);
#else
        return LoadResourceFromPE(pszName, ppDest);
#endif
    }
    else {
        char szResult[_MAX_PATH] = {0};
        int  n                   = -1;

        pFile = fopen(pszName, "rb");

        if(!pFile) {
            if (s_pszLibraryPath != NULL) {
                n = SearchFileFromPath(s_pszLibraryPath, pszName, szResult);
            }

            if (n < 0) {
                if (s_pszPublicPath == NULL)
                    s_pszPublicPath = getenv("PATH");
                n = SearchFileFromPath(s_pszPublicPath, pszName, szResult);
                if (n < 0)
                    _ReturnError (CLSError_NotFound);
            }

            pFile = fopen(szResult, "rb");
            if(!pFile) {
                ExtraMessage("fopen", szResult, "failed");
                _ReturnError (CLSError_OpenFile);
            }
        } else {
            strcpy(szResult, pszName);
        }

        fclose(pFile);

#ifdef _linux
            return LoadResourceFromELF(szResult, ppDest);
#else
            return LoadResourceFromPE(szResult, ppDest);
#endif
    }
}

int Cls2DllName(const char *szName, char *szResult)
{
    strcpy(szResult, szName);
    int i = strlen(szResult);
    if(!_stricmp(szResult + i - 4, ".cls")){
        strcpy(szResult + i - 4, ".eco");
        return 0;
    }
    return 1;
}

int LoadCLS(const char *pszName, CLSModule **ppModule)
{
    int  n                   = -1;
    char szFileName[_MAX_PATH] = {0};
    char szResult[_MAX_PATH] = {0};

    if (!pszName) {
        _Return (LoadCLSFromDll(NULL, ppModule));
    }

    if (s_pszLibraryPath != NULL) {
        n = SearchFileFromPath(s_pszLibraryPath, pszName, szResult);
    }

    if (n < 0) {
        if (s_pszPublicPath == NULL) {
            s_pszPublicPath = getenv("PATH");
        }

        n = SearchFileFromPath(s_pszPublicPath, pszName, szResult);

        if (n < 0) {
            Cls2DllName(pszName, szFileName);
            if (s_pszLibraryPath != NULL) {
                n = SearchFileFromPath(s_pszLibraryPath, szFileName, szResult);
            }
            if (n < 0) {
                n = SearchFileFromPath(s_pszPublicPath, szFileName, szResult);
                if (n < 0) {
                    _ReturnError (CLSError_NotFound);
                }
            }
        }
    }

    n = strlen(szResult);

    if (!_stricmp(szResult + n - 4, ".cls")) {
        _Return (LoadCLSFromFile(szResult, ppModule));
    } else if (!_stricmp(szResult + n - 4, ".eco")) {
        _Return (LoadCLSFromDll(szResult, ppModule));
    }

    ExtraMessage(szResult);
    _ReturnError (CLSError_UnknownFileType);
}
