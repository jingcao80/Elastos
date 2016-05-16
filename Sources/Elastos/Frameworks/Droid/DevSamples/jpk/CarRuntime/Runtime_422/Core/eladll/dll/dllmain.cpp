//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <elapi.h>
#include <eladef.h>
#include <stdio.h>

_ELASTOS_NAMESPACE_USING

Boolean AttachElastosDll();
void DetachElastosDll();

EXTERN_C _ELASTOS Boolean __stdcall DllMain(
    PVoid hDllHandle,
    UInt32 dwReason,
    PVoid preserved)
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

    return ret;
}

