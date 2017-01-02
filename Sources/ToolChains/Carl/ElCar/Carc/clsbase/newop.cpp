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

#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include <clsbase.h>

const int c_nMemMagic = 0x43615263;

inline void *_NewOperator(size_t size)
{
#ifdef _DEBUG
    int *p;

    size = RoundUp4(size + 4 * 3);
    p = (int *)malloc(size);
    if (!p) return NULL;

    size /= 4;
    *p = size;
    *(p + 1) = c_nMemMagic;
    *(p + size - 1) = c_nMemMagic;

    return (void *)(p + 2);

#else
    return malloc(size);
#endif // _DEBUG
}

inline void _DeleteOperator(void *pv)
{
#ifdef _DEBUG
    int *p = (int *)pv;

    p -= 2;
    if (*p < 3) goto ErrorExit;
    if (*(p + 1) != c_nMemMagic) goto ErrorExit;
    if (*(p + *p - 1) != c_nMemMagic) goto ErrorExit;
    *p = 0;

    free(p);
    return;

ErrorExit:
    assert(TRUE == FALSE && "delete illegal memory block.");

#else
    if (pv != NULL) free(pv);
#endif // _DEBUG
}
/*
void * __cdecl operator new(size_t size)
{
    return _NewOperator(size);
}

void __cdecl operator delete(void *pv)
{
    _DeleteOperator(pv);
}

void * __cdecl operator new[](size_t size)
{
    return _NewOperator(size);
}

void __cdecl operator delete[](void *pv)
{
    _DeleteOperator(pv);
}
*/
