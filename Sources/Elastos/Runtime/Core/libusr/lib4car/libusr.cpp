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

#include <elastos.h>

_ELASTOS_NAMESPACE_USING

extern "C" Int32  g_dllLockCount;
Int32  g_dllLockCount = 0;

EXTERN_C CARAPI DllCanUnloadNow()
{
    assert(g_dllLockCount >= 0);
    return 0 == g_dllLockCount ? NOERROR : S_FALSE;
}

ELAPI_(void) IncrementDllLockCount()
{
    atomic_inc(&g_dllLockCount);
}

ELAPI_(void) DecrementDllLockCount()
{
    atomic_dec(&g_dllLockCount);
}
