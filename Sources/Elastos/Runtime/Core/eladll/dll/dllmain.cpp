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
#include <stdio.h>

_ELASTOS_NAMESPACE_USING

Boolean AttachElastosDll();
void DetachElastosDll();

EXTERN_C int DllMain(
    void* hDllHandle,
    unsigned int dwReason,
    void* preserved)
{
    _ELASTOS Boolean ret = TRUE;
    // TODO: PALDisableThreadLibraryCalls((HMODULE)hModule);
    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            ret = AttachElastosDll();

            break;

        case DLL_PROCESS_DETACH:
#if !defined(_linux) || defined(_android)
            DetachElastosDll();
#endif
            // Do something
            // TODO: here need recycle the memory used by ezCOM
            break;
    }

    return ret ? 1 : 0;
}

