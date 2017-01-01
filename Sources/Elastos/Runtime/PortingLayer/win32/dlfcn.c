//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

