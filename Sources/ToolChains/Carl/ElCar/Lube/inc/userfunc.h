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

#ifndef __USERFUNC_H__
#define __USERFUNC_H__

#define _stricmp strcasecmp

typedef int (*UserFn_t)(PLUBECTX, PSTATEDESC, PVOID);

#define IMPL_USERFUNC(func) int UserFunc_##func
#define DECL_USERFUNC(func) \
    extern int UserFunc_##func(PLUBECTX, PSTATEDESC, PVOID)
#define CALL_USERFUNC(func) UserFunc_##func

typedef struct UserFuncEntry {
    const char *    mName;
    DWORD           dwArgType;
    UserFn_t        fnUserFunc;
    const char *    pszDesc;
}   UserFuncEntry;

extern const UserFuncEntry g_userFuncs[];
EXTERN_C const int c_cUserFuncs;

#define USERFUNC_(func, type, desc) { #func, type, UserFunc_##func, desc }
#define ARGTYPE_STRING          (DWORD)-1
#define ARGTYPE_(obj, mem)      (DWORD)((obj << 16) | mem)

inline UserFn_t GetUserFunction(int nIndex)
{
    assert(0 <= nIndex && nIndex < c_cUserFuncs);
    return g_userFuncs[nIndex].fnUserFunc;
}

inline int GetUserFuncIndex(const char * pszName)
{
    for (int n = 0; n < c_cUserFuncs; n++) {
        if (!_stricmp(g_userFuncs[n].mName, pszName)) return n;
    }
    return LUBE_FAIL;
}

#endif // __USERFUNC_H__
