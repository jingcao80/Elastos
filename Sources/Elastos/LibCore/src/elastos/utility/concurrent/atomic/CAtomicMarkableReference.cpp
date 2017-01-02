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

#include "CAtomicMarkableReference.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

const Int64 CAtomicMarkableReference::mPairOffset = 0; // = objectFieldOffset(UNSAFE, "pair", AtomicMarkableReference.class);

CAR_INTERFACE_IMPL(CAtomicMarkableReference, Object, IAtomicMarkableReference)

CAR_OBJECT_IMPL(CAtomicMarkableReference)

ECode CAtomicMarkableReference::constructor(
    /* [in] */ IInterface* initialRef,
    /* [in] */ Boolean initialMark)
{
    mPair = Pair::Of(initialRef, initialMark);
    return NOERROR;
}

ECode CAtomicMarkableReference::GetReference(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    *outface = mPair->mReference;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode CAtomicMarkableReference::IsMarked(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mPair->mMark;
    return NOERROR;
}

ECode CAtomicMarkableReference::Get(
    /* [in] */ ArrayOf<Boolean>* markHolder,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    (*markHolder)[0] = mPair->mMark;
    *outface = mPair->mReference;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode CAtomicMarkableReference::WeakCompareAndSet(
    /* [in] */ IInterface* expectedReference,
    /* [in] */ IInterface* newReference,
    /* [in] */ Boolean expectedMark,
    /* [in] */ Boolean newMark,
    /* [out] */ Boolean* value)
{
    return CompareAndSet(expectedReference, newReference, expectedMark, newMark, value);
}

ECode CAtomicMarkableReference::CompareAndSet(
    /* [in] */ IInterface* expectedReference,
    /* [in] */ IInterface* newReference,
    /* [in] */ Boolean expectedMark,
    /* [in] */ Boolean newMark,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    Pair* current = mPair;
    *value = expectedReference == current->mReference &&
            expectedMark == current->mMark &&
            ((newReference == current->mReference &&
            newMark == current->mMark) ||
            CasPair(current, Pair::Of(newReference, newMark)));
    return NOERROR;
}

ECode CAtomicMarkableReference::Set(
    /* [in] */ IInterface* newReference,
    /* [in] */ Boolean newMark)
{
    Pair* current = mPair;
    if (newReference != current->mReference || newMark != current->mMark)
        mPair = Pair::Of(newReference, newMark);
    return NOERROR;
}

ECode CAtomicMarkableReference::AttemptMark(
    /* [in] */ IInterface* expectedReference,
    /* [in] */ Boolean newMark,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    Pair* current = mPair;
    *value = expectedReference == current->mReference &&
            (newMark == current->mMark ||
            CasPair(current, Pair::Of(expectedReference, newMark)));
    return NOERROR;
}

Boolean CAtomicMarkableReference::CasPair(
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
