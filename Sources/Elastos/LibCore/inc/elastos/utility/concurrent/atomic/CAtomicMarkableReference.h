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

#ifndef __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICMARKABLEREFERENCE_H__
#define __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICMARKABLEREFERENCE_H__

#include "_Elastos_Utility_Concurrent_Atomic_CAtomicMarkableReference.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

CarClass(CAtomicMarkableReference)
    , public Object
    , public IAtomicMarkableReference
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()
    /**
     * Creates a new {@code AtomicMarkableReference} with the given
     * initial values.
     *
     * @param initialRef the initial reference
     * @param initialMark the initial mark
     */
    CARAPI constructor(
        /* [in] */ IInterface* initialRef,
        /* [in] */ Boolean initialMark);

    /**
     * Returns the current value of the reference.
     *
     * @return the current value of the reference
     */
    CARAPI GetReference(
        /* [out] */ IInterface** outface);

    /**
     * Returns the current value of the mark.
     *
     * @return the current value of the mark
     */
    CARAPI IsMarked(
        /* [out] */ Boolean* value);

    /**
     * Returns the current values of both the reference and the mark.
     * Typical usage is {@code boolean[1] holder; ref = v.get(holder); }.
     *
     * @param markHolder an array of size of at least one. On return,
     * {@code markholder[0]} will hold the value of the mark.
     * @return the current value of the reference
     */
    CARAPI Get(
        /* [in] */ ArrayOf<Boolean>* markHolder,
        /* [out] */ IInterface** outface);

    /**
     * Atomically sets the value of both the reference and mark
     * to the given update values if the
     * current reference is {@code ==} to the expected reference
     * and the current mark is equal to the expected mark.
     *
     * <p>May <a href="package-summary.html#Spurious">fail spuriously</a>
     * and does not provide ordering guarantees, so is only rarely an
     * appropriate alternative to {@code compareAndSet}.
     *
     * @param expectedReference the expected value of the reference
     * @param newReference the new value for the reference
     * @param expectedMark the expected value of the mark
     * @param newMark the new value for the mark
     * @return true if successful
     */
    CARAPI WeakCompareAndSet(
        /* [in] */ IInterface* expectedReference,
        /* [in] */ IInterface* newReference,
        /* [in] */ Boolean expectedMark,
        /* [in] */ Boolean newMark,
        /* [out] */ Boolean* value);

    /**
     * Atomically sets the value of both the reference and mark
     * to the given update values if the
     * current reference is {@code ==} to the expected reference
     * and the current mark is equal to the expected mark.
     *
     * @param expectedReference the expected value of the reference
     * @param newReference the new value for the reference
     * @param expectedMark the expected value of the mark
     * @param newMark the new value for the mark
     * @return true if successful
     */
    CARAPI CompareAndSet(
        /* [in] */ IInterface* expectedReference,
        /* [in] */ IInterface* newReference,
        /* [in] */ Boolean expectedMark,
        /* [in] */ Boolean newMark,
        /* [out] */ Boolean* value);

    /**
     * Unconditionally sets the value of both the reference and mark.
     *
     * @param newReference the new value for the reference
     * @param newMark the new value for the mark
     */
    CARAPI Set(
        /* [in] */ IInterface* newReference,
        /* [in] */ Boolean newMark);

    /**
     * Atomically sets the value of the mark to the given update value
     * if the current reference is {@code ==} to the expected
     * reference.  Any given invocation of this operation may fail
     * (return {@code false}) spuriously, but repeated invocation
     * when the current value holds the expected value and no other
     * thread is also attempting to set the value will eventually
     * succeed.
     *
     * @param expectedReference the expected value of the reference
     * @param newMark the new value for the mark
     * @return true if successful
     */
    CARAPI AttemptMark(
        /* [in] */ IInterface* expectedReference,
        /* [in] */ Boolean newMark,
        /* [out] */ Boolean* value);

protected:
    // static long objectFieldOffset(sun.misc.Unsafe UNSAFE, String field, Class<?> klazz);

private:
    class Pair {
    private:
        Pair(
            /* [in] */ IInterface* reference,
            /* [in] */ Boolean mark) {
            mReference = reference;
            mMark = mark;
        }
    public:
        static CARAPI_(Pair*) Of(
            /* [in] */ IInterface* reference,
            /* [in] */ Boolean mark) {
            Pair* outpair = new Pair(reference, mark);
            return outpair;
        }

    public:
        AutoPtr<IInterface> mReference;
        Boolean mMark;
    };

    CARAPI_(Boolean) CasPair(
        /* [in] */ Pair* cmp,
        /* [in] */ Pair* val);

private:
    /* volatile */ Pair* mPair;
    // private static final sun.misc.Unsafe UNSAFE = sun.misc.Unsafe.getUnsafe();
    static const Int64 mPairOffset; // = objectFieldOffset(UNSAFE, "pair", AtomicMarkableReference.class);
};

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICMARKABLEREFERENCE_H__
