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

#include "AtomicReference.h"
#include <cutils/atomic.h>

using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

static Boolean CompareAndSwapObject(volatile uintptr_t* address, IInterface* expect, IInterface* update)
{
    // Note: android_atomic_cmpxchg() returns 0 on success, not failure.
#if defined(_arm)
    int ret = android_atomic_release_cas((int32_t)expect,
            (int32_t)update, (int32_t*)address);
#elif defined(_aarch64)
    int ret = !__sync_bool_compare_and_swap(address,
            (uintptr_t)expect, (uintptr_t)update);
#endif
    if (ret == 0) {
        REFCOUNT_ADD(update)
        REFCOUNT_RELEASE(expect)
    }
    return (ret == 0);
}

const Int64 AtomicReference::mSerialVersionUID = -1848883965231344442LL;

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
    *value = CompareAndSwapObject((volatile uintptr_t*)&mValue, expect, update);
    return NOERROR;
}

ECode AtomicReference::WeakCompareAndSet(
    /* [in] */ IInterface* expect,
    /* [in] */ IInterface* update,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = CompareAndSwapObject((volatile uintptr_t*)&mValue, expect, update);
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
