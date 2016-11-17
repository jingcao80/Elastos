
#include "AtomicReference.h"
#include <cutils/atomic.h>

using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

static Boolean CompareAndSwapObject(volatile int32_t* address, IInterface* expect, IInterface* update)
{
    // Note: android_atomic_cmpxchg() returns 0 on success, not failure.
    int ret = android_atomic_release_cas((int32_t)expect,
            (int32_t)update, address);
    if (ret == 0) {
        REFCOUNT_ADD(update)
        REFCOUNT_RELEASE(expect)
    }
    return (ret == 0);
}

const Int64 AtomicReference::mSerialVersionUID = -1848883965231344442L;

const Int64 AtomicReference::mValueOffset = 0; //unsafe.objectFieldOffset(AtomicReference.class.getDeclaredField("value"));

CAR_INTERFACE_IMPL_2(AtomicReference, Object, IAtomicReference, ISerializable)

ECode AtomicReference::constructor(
    /* [in] */ IInterface* initialValue)
{
    mValue = initialValue;
    return NOERROR;
}

ECode AtomicReference::constructor()
{
    return NOERROR;
}

ECode AtomicReference::Get(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    *outface = mValue;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode AtomicReference::Set(
    /* [in] */ IInterface* newValue)
{
    mValue = newValue;
    return NOERROR;
}

ECode AtomicReference::LazySet(
    /* [in] */ IInterface* newValue)
{
    assert(0 && "TODO");
    // unsafe.putOrderedObject(this, valueOffset, newValue);
    return NOERROR;
}

ECode AtomicReference::CompareAndSet(
    /* [in] */ IInterface* expect,
    /* [in] */ IInterface* update,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = CompareAndSwapObject((volatile int32_t*)&mValue, expect, update);
    return NOERROR;
}

ECode AtomicReference::WeakCompareAndSet(
    /* [in] */ IInterface* expect,
    /* [in] */ IInterface* update,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = CompareAndSwapObject((volatile int32_t*)&mValue, expect, update);
    return NOERROR;
}

ECode AtomicReference::GetAndSet(
    /* [in] */ IInterface* newValue,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    while (TRUE) {
        AutoPtr<IInterface> x;
        Get((IInterface**)&x);
        Boolean isflag = FALSE;
        if (CompareAndSet(x, newValue, &isflag), isflag) {
            *outface = x;
            REFCOUNT_ADD(*outface)
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode AtomicReference::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    AutoPtr<IInterface> obj;
    Get((IInterface**)&obj);
    *str = Object::ToString(obj);
    return NOERROR;
}

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
