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

#include "CAtomicStampedReference.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

CAR_INTERFACE_IMPL(CAtomicStampedReference, Object, IAtomicStampedReference)

CAR_OBJECT_IMPL(CAtomicStampedReference)

ECode CAtomicStampedReference::constructor(
    /* [in] */ IInterface* initialRef,
    /* [in] */ Int32 initialStamp)
{
    mPair = Pair::Of(initialRef, initialStamp);
    return NOERROR;
}

ECode CAtomicStampedReference::GetReference(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    *outface = mPair->mReference;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode CAtomicStampedReference::GetStamp(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mPair->mStamp;
    return NOERROR;
}

ECode CAtomicStampedReference::Get(
    /* [in] */ ArrayOf<Int32>* stampHolder,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    Pair* pair = mPair;
    (*stampHolder)[0] = pair->mStamp;
    *outface = mPair->mReference;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode CAtomicStampedReference::WeakCompareAndSet(
    /* [in] */ IInterface* expectedReference,
    /* [in] */ IInterface* newReference,
    /* [in] */ Int32 expectedStamp,
    /* [in] */ Int32 newStamp,
    /* [out] */ Boolean* value)
{
    return CompareAndSet(expectedReference, newReference, expectedStamp, newStamp, value);
}

ECode CAtomicStampedReference::CompareAndSet(
    /* [in] */ IInterface* expectedReference,
    /* [in] */ IInterface* newReference,
    /* [in] */ Int32 expectedStamp,
    /* [in] */ Int32 newStamp,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    Pair* current = mPair;
    *value = expectedReference == current->mReference &&
           expectedStamp == current->mStamp &&
           ((newReference == current->mReference &&
           newStamp == current->mStamp) ||
           CasPair(current, Pair::Of(newReference, newStamp)));
    return NOERROR;
}

ECode CAtomicStampedReference::Set(
    /* [in] */ IInterface* newReference,
    /* [in] */ Int32 newStamp)
{
    Pair* current = mPair;
    if (newReference != current->mReference || newStamp != current->mStamp)
        mPair = Pair::Of(newReference, newStamp);
    return NOERROR;
}

ECode CAtomicStampedReference::AttemptStamp(
    /* [in] */ IInterface* expectedReference,
    /* [in] */ Int32 newStamp,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    Pair* current = mPair;
    return expectedReference == current->mReference &&
           (newStamp == current->mStamp ||
           CasPair(current, Pair::Of(expectedReference, newStamp)));
    return NOERROR;
}

Boolean CAtomicStampedReference::CasPair(
    /* [in] */ Pair* cmp,
    /* [in] */ Pair* val)
{
    assert(0 && "TODO");
    // return UNSAFE.compareAndSwapObject(this, pairOffset, cmp, val);
    return FALSE;
}

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
