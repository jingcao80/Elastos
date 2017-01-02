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

#ifndef __ELASTOS_UTILITY_CARRAYDEQUE_H__
#define __ELASTOS_UTILITY_CARRAYDEQUE_H__

#include "_Elastos_Utility_CArrayDeque.h"
#include "AbstractCollection.h"

using Elastos::Core::ICloneable;
using Elastos::IO::ISerializable;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectInputStream;

namespace Elastos {
namespace Utility {

CarClass(CArrayDeque)
    , public AbstractCollection
    , public IArrayDeque
    , public IDeque
    , public ICloneable
    , public ISerializable
{
private:
    class DeqIterator
        : public Object
        , public IIterator
    {
    public:
        DeqIterator(
            /* [in] */ CArrayDeque* owner)
            : mCursor(owner->mHead)
            , mFence(owner->mTail)
            , mLastRet(-1)
            , mOwner(owner)
        {}

        CAR_INTERFACE_DECL()

        CARAPI HasNext(
            /* [out] */ Boolean* value);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI Remove();

    private:
        /**
         * Index of element to be returned by subsequent call to next.
         */
        Int32 mCursor;

        /**
         * Tail recorded at construction (also in remove), to stop
         * iterator and also to check for comodification.
         */
        Int32 mFence;

        /**
         * Index of element returned by most recent call to next.
         * Reset to -1 if element is deleted by a call to remove.
         */
        Int32 mLastRet;

        AutoPtr<CArrayDeque> mOwner;
    };

    class DescendingIterator
        : public Object
        , public IIterator
    {
    public:
        DescendingIterator(
            /* [in] */ CArrayDeque* owner)
            : mCursor(owner->mTail)
            , mFence(owner->mHead)
            , mLastRet(-1)
            , mOwner(owner)
        {}

        CAR_INTERFACE_DECL()

        CARAPI HasNext(
            /* [out] */ Boolean* value);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI Remove();

    private:
        /*
         * This class is nearly a mirror-image of DeqIterator, using
         * tail instead of head for initial mCursor, and head instead of
         * tail for mFence.
         */
        Int32 mCursor;
        Int32 mFence;
        Int32 mLastRet;
        AutoPtr<CArrayDeque> mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CArrayDeque();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 numElements);

    CARAPI constructor(
        /* [in] */ ICollection* c);

    CARAPI AddFirst(
        /* [in] */ IInterface* e);

    CARAPI AddLast(
        /* [in] */ IInterface* e);

    CARAPI OfferFirst(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* value);

    CARAPI OfferLast(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* value);

    CARAPI RemoveFirst(
        /* [out] */ IInterface** e);

    CARAPI RemoveLast(
        /* [out] */ IInterface** e);

    CARAPI PollFirst(
        /* [out] */ IInterface** e);

    CARAPI PollLast(
        /* [out] */ IInterface** e);

    CARAPI GetFirst(
        /* [out] */ IInterface** e);

    CARAPI GetLast(
        /* [out] */ IInterface** e);

    CARAPI PeekFirst(
        /* [out] */ IInterface** e);

    CARAPI PeekLast(
        /* [out] */ IInterface** e);

    CARAPI RemoveFirstOccurrence(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* value);

    CARAPI RemoveLastOccurrence(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* value);

    CARAPI Add(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* value);

    CARAPI Offer(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* value);

    CARAPI Remove(
        /* [out] */ IInterface** e);

    CARAPI Poll(
        /* [out] */ IInterface** e);

    CARAPI GetElement(
        /* [out] */ IInterface** e);

    CARAPI Peek(
        /* [out] */ IInterface** e);

    CARAPI Push(
        /* [in] */ IInterface* e);

    CARAPI Pop(
        /* [out] */ IInterface** e);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI IsEmpty(
        /* [out] */ Boolean* value);

    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    CARAPI GetDescendingIterator(
        /* [out] */ IIterator** it);

    CARAPI Contains(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* value);

    CARAPI Remove(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* value);

    CARAPI Clear();

    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    CARAPI ToArray(
        /* [in] */ ArrayOf<IInterface*>* a,
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI AddAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI ContainsAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI Add(
        /* [in] */ IInterface* object);

    CARAPI AddAll(
        /* [in] */ ICollection* collection);

    CARAPI Remove(
        /* [in] */ IInterface* object);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection);
private:
    CARAPI_(void) AllocateElements(
        /* [in] */ Int32 numElements);

    CARAPI DoubleCapacity();

    CARAPI_(AutoPtr< ArrayOf<IInterface*> >) CopyElements(
        /* [in] */ ArrayOf<IInterface*>* a);

    CARAPI CheckInvariants();

    CARAPI Delete(
        /* [in] */ Int32 i,
        /* [out] */ Boolean* result);

    CARAPI WriteObject(
        /* [in] */ IObjectOutputStream* stream);

    CARAPI ReadObject(
        /* [in] */ IObjectInputStream* stream);

protected:
    CARAPI_(String) GetClassName() { return String("CArrayDeque"); }

private:
    /**
     * The array in which the elements of the deque are stored.
     * The capacity of the deque is the length of this array, which is
     * always a power of two. The array is never allowed to become
     * full, except transiently within an addX method where it is
     * resized (see doubleCapacity) immediately upon becoming full,
     * thus avoiding head and tail wrapping around to equal each
     * other.  We also guarantee that all array cells not holding
     * deque elements are always null.
     */
    /* transient */ AutoPtr< ArrayOf<IInterface*> > mElements;

    /**
     * The index of the element at the head of the deque (which is the
     * element that would be removed by remove() or pop()); or an
     * arbitrary number equal to tail if the deque is empty.
     */
    /* transient */ Int32 mHead;

    /**
     * The index at which the next element would be added to the tail
     * of the deque (via addLast(E), add(E), or push(E)).
     */
    /* transient */ Int32 mTail;

    /**
     * The minimum capacity that we'll use for a newly created deque.
     * Must be a power of 2.
     */
    static const Int32 MIN_INITIAL_CAPACITY;

    /**
     * Appease the serialization gods.
     */
    // static const Int64 serialVersionUID;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__CARRAYDEQUE_H__
