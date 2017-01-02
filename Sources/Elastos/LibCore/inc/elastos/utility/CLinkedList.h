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

#ifndef __ELASTOS_UTILITY_CLINKEDLIST_H__
#define __ELASTOS_UTILITY_CLINKEDLIST_H__

#include "_Elastos_Utility_CLinkedList.h"
#include "AbstractSequentialList.h"

using Elastos::Core::ICloneable;
using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Utility {

CarClass(CLinkedList)
    , public AbstractSequentialList
    , public ILinkedList
    , public IDeque
    , public IQueue
    , public ICloneable
    , public ISerializable
{
private:
    class Link
        : public Object
    {
    public:
        Link(
            /* [in] */ IInterface* o,
            /* [in] */ Link* p,
            /* [in] */ Link* n);

    public:
        AutoPtr<IInterface> mData;
        AutoPtr<Link> mPrevious;
        AutoPtr<Link> mNext;
    };

    class LinkIterator
        : public Object
        , public IListIterator
        , public IIterator
    {
    public:
        LinkIterator(
            /* [in] */ CLinkedList* object,
            /* [in] */ Int32 location);

        CAR_INTERFACE_DECL()

        CARAPI Add(
            /* [in] */ IInterface* object);

        CARAPI HasNext(
            /* [out] */ Boolean* result);

        CARAPI HasPrevious(
            /* [out] */ Boolean* result);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI NextIndex(
            /* [out] */ Int32* index);

        CARAPI Previous(
            /* [out] */ IInterface** object);

        CARAPI PreviousIndex(
            /* [out] */ Int32* index);

        CARAPI Remove();

        CARAPI Set(
            /* [in] */ IInterface* object);

        CARAPI GetNextIndex(
            /* [out] */ Int32* index);

        CARAPI GetPrevious(
            /* [out] */ IInterface** object);

        CARAPI GetPreviousIndex(
            /* [out] */ Int32* index);

    public:
        Int32 mPos;
        Int32 mExpectedModCount;
        const AutoPtr<CLinkedList> mList;
        AutoPtr<Link> mLink;
        AutoPtr<Link> mLastLink;
    };

    class ReverseLinkIterator
        : public Object
        , public IIterator
    {
    public:
        ReverseLinkIterator(
            /* [in] */ CLinkedList* linkedList);

        CAR_INTERFACE_DECL()

        CARAPI HasNext(
            /* [out] */ Boolean* result);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI Remove();

    private:
        Int32 mExpectedModCount;
        const AutoPtr<CLinkedList> mList;
        AutoPtr<Link> mLink;
        Boolean mCanRemove;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CLinkedList();

    ~CLinkedList();

    /**
     * Constructs a new empty instance of {@code CLinkedList}.
     */
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICollection* collection);

    CARAPI Add(
        /* [in] */ Int32 location,
        /* [in] */ IInterface* object);

    CARAPI Add(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI AddAll(
        /* [in] */ Int32 location,
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI AddAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI AddFirst(
        /* [in] */ IInterface* object);

    CARAPI AddLast(
        /* [in] */ IInterface* object);

    CARAPI Clear();

    /**
     * Returns a new {@code CLinkedList} with the same elements and size as this
     * {@code CLinkedList}.
     *
     * @return a shallow copy of this {@code CLinkedList}.
     * @see java.lang.Cloneable
     */
    virtual CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI Contains(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI Get(
        /* [in] */ Int32 location,
        /* [out] */ IInterface** object);

    CARAPI GetFirst(
        /* [out] */ IInterface** object);

    CARAPI GetLast(
        /* [out] */ IInterface** object);

    CARAPI IndexOf(
        /* [in] */ IInterface* object,
        /* [out] */ Int32* index);

    CARAPI LastIndexOf(
        /* [in] */ IInterface* object,
        /* [out] */ Int32* index);

    CARAPI GetListIterator(
        /* [in] */ Int32 location,
        /* [out] */ IListIterator** it);

    CARAPI Remove(
        /* [in] */ Int32 location,
        /* [out] */ IInterface** object);

    CARAPI Remove(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI RemoveFirst(
        /* [out] */ IInterface** object);

    CARAPI RemoveLast(
        /* [out] */ IInterface** object);

    CARAPI GetDescendingIterator(
        /* [out] */ IIterator** iterator);

    CARAPI OfferFirst(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI OfferLast(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI PeekFirst(
        /* [out] */ IInterface** object);

    CARAPI PeekLast(
        /* [out] */ IInterface** object);

    CARAPI PollFirst(
        /* [out] */ IInterface** object);

    CARAPI PollLast(
        /* [out] */ IInterface** object);

    CARAPI Pop(
        /* [out] */ IInterface** object);

    CARAPI Push(
        /* [in] */ IInterface* object);

    CARAPI RemoveFirstOccurrence(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI RemoveLastOccurrence(
       /* [in] */ IInterface* object,
       /* [out] */ Boolean* result);

    CARAPI Set(
        /* [in] */ Int32 location,
        /* [in] */ IInterface* object,
        /* [out] */ IInterface** prevObject);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI Offer(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI Poll(
        /* [out] */ IInterface** object);

    CARAPI Remove(
        /* [out] */ IInterface** object);

    CARAPI Peek(
        /* [out] */ IInterface** object);

    CARAPI GetElement(
        /* [out] */ IInterface** object);

    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    CARAPI ToArray(
        /* [in] */ ArrayOf<IInterface*>* contents,
        /* [out, callee] */ ArrayOf<IInterface*>** outArray);

    CARAPI Element(
        /* [out] */ IInterface** e);

    CARAPI GetIterator(
        /* [in] */ IIterator** it);

    CARAPI Add(
        /* [in] */ IInterface* object);

    CARAPI AddAll(
        /* [in] */ ICollection* collection);

    CARAPI ContainsAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI Remove(
        /* [in] */ IInterface* object);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection);

    CARAPI AddAll(
        /* [in] */ Int32 index,
        /* [in] */ ICollection* collection);

    CARAPI GetListIterator(
        /* [in] */ IListIterator** it);

    CARAPI Remove(
        /* [in] */ Int32 index);

    CARAPI Set(
        /* [in] */ Int32 index,
        /* [in] */ IInterface* object);

    CARAPI GetSubList(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ IList** list);

protected:
    CARAPI_(String) GetClassName() { return String("CLinkedList"); }

private:
    CARAPI AddLastImpl(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI AddFirstImpl(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI GetFirstImpl(
        /* [out] */ IInterface** object);

    CARAPI RemoveFirstImpl(
        /* [out] */ IInterface** object);

    CARAPI RemoveLastImpl(
        /* [out] */ IInterface** object);

    CARAPI RemoveFirstOccurrenceImpl(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI RemoveOneOccurrence(
        /* [in] */ IInterface* object,
        /* [in] */ IIterator* iter,
        /* [out] */ Boolean* result);

    CARAPI PeekFirstImpl(
        /* [out] */ IInterface** object);

    CARAPI WriteObject(
        /* [in] */ IObjectOutputStream* stream);

    CARAPI ReadObject(
        /* [in] */ IObjectInputStream* stream);

public:
    Int32 mSize;
    AutoPtr<Link> mVoidLink;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__CLINKEDLIST_H__
