
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
