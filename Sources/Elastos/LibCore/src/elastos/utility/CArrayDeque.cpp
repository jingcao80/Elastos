
#include "Elastos.CoreLibrary.IO.h"
#include "CArrayDeque.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::IO::EIID_ISerializable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Utility {

//====================================================================
// CArrayDeque::DeqIterator
//====================================================================

CAR_INTERFACE_IMPL_4(CArrayDeque, AbstractCollection, IArrayDeque, IDeque, ICloneable, ISerializable)

CAR_OBJECT_IMPL(CArrayDeque)

CAR_INTERFACE_IMPL(CArrayDeque::DeqIterator, Object, IIterator)

ECode CArrayDeque::DeqIterator::HasNext(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mCursor != mFence;
    return NOERROR;
}

ECode CArrayDeque::DeqIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    if (mCursor == mFence) {
        *object = NULL;
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    AutoPtr<IInterface> result = (*mOwner->mElements)[mCursor];
    // This check doesn't catch all possible comodifications,
    // but does catch the ones that corrupt traversal
    if (mOwner->mTail != mFence || result == NULL) {
        *object = NULL;
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mLastRet = mCursor;
    mCursor = (mCursor + 1) & (mOwner->mElements->GetLength() - 1);
    *object = result;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CArrayDeque::DeqIterator::Remove()
{
    if (mLastRet < 0) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Boolean result;
    FAIL_RETURN(mOwner->Delete(mLastRet, &result));
    if (result) { // if left-shifted, undo increment in next()
        mCursor = (mCursor - 1) & (mOwner->mElements->GetLength() - 1);
        mFence = mOwner->mTail;
    }
    mLastRet = -1;
    return NOERROR;
}


//====================================================================
// CArrayDeque::DescendingIterator
//====================================================================
CAR_INTERFACE_IMPL(CArrayDeque::DescendingIterator, Object, IIterator)

ECode CArrayDeque::DescendingIterator::HasNext(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mCursor != mFence;
    return NOERROR;
}

ECode CArrayDeque::DescendingIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    if (mCursor == mFence) {
        *object = NULL;
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    mCursor = (mCursor - 1) & (mOwner->mElements->GetLength() - 1);
    AutoPtr<IInterface> result = (*mOwner->mElements)[mCursor];
    if (mOwner->mHead != mFence || result == NULL) {
        *object = NULL;
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mLastRet = mCursor;
    *object = result;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CArrayDeque::DescendingIterator::Remove()
{
    if (mLastRet < 0) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Boolean result;
    FAIL_RETURN(mOwner->Delete(mLastRet, &result));
    if (!result) {
        mCursor = (mCursor + 1) & (mOwner->mElements->GetLength() - 1);
        mFence = mOwner->mHead;
    }
    mLastRet = -1;
    return NOERROR;
}


//====================================================================
// CArrayDeque
//====================================================================
const Int32 CArrayDeque::MIN_INITIAL_CAPACITY = 8;
// const Int64 CArrayDeque::serialVersionUID = 2340985798034038923L;

CArrayDeque::CArrayDeque()
    : mHead(0)
    , mTail(0)
{}

ECode CArrayDeque::constructor()
{
    mElements = ArrayOf<IInterface*>::Alloc(16);
    return NOERROR;
}

ECode CArrayDeque::constructor(
    /* [in] */ Int32 numElements)
{
    AllocateElements(numElements);
    return NOERROR;
}

ECode CArrayDeque::constructor(
    /* [in] */ ICollection* c)
{
    Int32 value = 0;
    c->GetSize(&value);
    AllocateElements(value);
    Boolean isflag = FALSE;
    AddAll(c, &isflag);
    return NOERROR;
}

void CArrayDeque::AllocateElements(
    /* [in] */ Int32 numElements)
{
    Int32 initialCapacity = MIN_INITIAL_CAPACITY;
    // Find the best power of two to hold elements.
    // Tests "<=" because arrays aren't kept full.
    if (numElements >= initialCapacity) {
        initialCapacity = numElements;
        initialCapacity |= (initialCapacity >>  1);
        initialCapacity |= (initialCapacity >>  2);
        initialCapacity |= (initialCapacity >>  4);
        initialCapacity |= (initialCapacity >>  8);
        initialCapacity |= (initialCapacity >> 16);
        initialCapacity++;

        if (initialCapacity < 0)   // Too many elements, must back off
            initialCapacity = ((UInt32)initialCapacity) >> 1;// Good luck allocating 2 ^ 30 elements
    }
    mElements = ArrayOf<IInterface*>::Alloc(initialCapacity);
}

ECode CArrayDeque::DoubleCapacity()
{
    assert(mHead == mTail);
    Int32 p = mHead;
    Int32 n = mElements->GetLength();
    Int32 r = n - p; // number of elements to the right of p
    Int32 newCapacity = n << 1;
    if (newCapacity < 0) {
        // throw new IllegalStateException("Sorry, deque too big");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<ArrayOf<IInterface*> > a = ArrayOf<IInterface*>::Alloc(newCapacity);
    a->Copy(0, mElements, p, r);
    a->Copy(r, mElements, 0, p);

    mElements = a;
    mHead = 0;
    mTail = n;
    return NOERROR;
}

AutoPtr< ArrayOf<IInterface*> > CArrayDeque::CopyElements(
    /* [in] */ ArrayOf<IInterface*>* a)
{
    if (mHead < mTail) {
        Int32 value = 0;
        GetSize(&value);
        a->Copy(0, mElements, mHead, value);
    }
    else if (mHead > mTail) {
        Int32 headPortionLen = mElements->GetLength() - mHead;
        a->Copy(0, mElements, mHead, headPortionLen);
        a->Copy(headPortionLen, mElements, 0, mTail);
    }
    return a;
}

ECode CArrayDeque::AddFirst(
    /* [in] */ IInterface* e)
{
    if (e == NULL) {
        // throw new NullPointerException("e == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mElements->Set(mHead = (mHead - 1) & (mElements->GetLength() - 1), e);
    if (mHead == mTail) {
        return DoubleCapacity();
    }
    return NOERROR;
}

ECode CArrayDeque::AddLast(
    /* [in] */ IInterface* e)
{
    if (e == NULL) {
        // throw new NullPointerException("e == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mElements->Set(mTail, e);
    if ( (mTail = (mTail + 1) & (mElements->GetLength() - 1)) == mHead) {
        return DoubleCapacity();
    }
    return NOERROR;
}

ECode CArrayDeque::OfferFirst(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    FAIL_RETURN(AddFirst(e));
    *value = TRUE;
    return NOERROR;
}

ECode CArrayDeque::OfferLast(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    FAIL_RETURN(AddLast(e));
    *value = TRUE;
    return NOERROR;
}

ECode CArrayDeque::RemoveFirst(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)

    AutoPtr<IInterface> x;
    PollFirst((IInterface**)&x);
    if (x == NULL) {
        // throw new NoSuchElementException();
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    *e = x;
    REFCOUNT_ADD(*e)
    return NOERROR;
}

ECode CArrayDeque::RemoveLast(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)

    AutoPtr<IInterface> x;
    PollLast((IInterface**)&x);
    if (x == NULL) {
        // throw new NoSuchElementException();
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    *e = x;
    REFCOUNT_ADD(*e)
    return NOERROR;
}

ECode CArrayDeque::PollFirst(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)

    Int32 h = mHead;
    // @SuppressWarnings("unchecked") E result = (E) elements[h];
    AutoPtr<IInterface> result = (*mElements)[h];
    // Element is null if deque empty
    if (result == NULL) {
        *e = NULL;
        return NOERROR;
    }
    mElements->Set(h, NULL);    // Must null out slot
    mHead = (h + 1) & (mElements->GetLength() - 1);
    *e = result;
    REFCOUNT_ADD(*e)
    return NOERROR;
}

ECode CArrayDeque::PollLast(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)

    Int32 t = (mTail - 1) & (mElements->GetLength() - 1);
    // @SuppressWarnings("unchecked") E result = (E) elements[t];
    AutoPtr<IInterface> result = (*mElements)[t];
    if (result == NULL) {
        *e = NULL;
        return NOERROR;
    }
    mElements->Set(t, NULL);
    mTail = t;
    *e = result;
    REFCOUNT_ADD(*e)
    return NOERROR;
}

ECode CArrayDeque::GetFirst(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)

    // @SuppressWarnings("unchecked") E result = (E) elements[head];
    AutoPtr<IInterface> result = (*mElements)[mHead];
    if (result == NULL) {
        // throw new NoSuchElementException();
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    *e = result;
    REFCOUNT_ADD(*e)
    return NOERROR;
}

ECode CArrayDeque::GetLast(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)

    // @SuppressWarnings("unchecked")
    // E result = (E) elements[(tail - 1) & (elements.length - 1)];
    AutoPtr<IInterface> result = (*mElements)[(mTail - 1) & (mElements->GetLength() - 1)];
    if (result == NULL) {
        // throw new NoSuchElementException();
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    (*e) = result;
    REFCOUNT_ADD(*e)

    return NOERROR;
}

ECode CArrayDeque::PeekFirst(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)

    // @SuppressWarnings("unchecked") E result = (E) elements[head];
    AutoPtr<IInterface> result = (*mElements)[mHead];
    // elements[head] is null if deque empty
    *e = result;
    REFCOUNT_ADD(*e)

    return NOERROR;
}

ECode CArrayDeque::PeekLast(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)

    // @SuppressWarnings("unchecked")
    // E result = (E) elements[(tail - 1) & (elements.length - 1)];
    AutoPtr<IInterface> result = (*mElements)[(mTail - 1) & (mElements->GetLength() - 1)];
    *e = result;
    REFCOUNT_ADD(*e)

    return NOERROR;
}

ECode CArrayDeque::RemoveFirstOccurrence(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    if (o == NULL) {
        *value = FALSE;
        return NOERROR;
    }
    Int32 mask = mElements->GetLength() - 1;
    Int32 i = mHead;
    AutoPtr<IInterface> x;
    while ( (x = (*mElements)[i]) != NULL) {
        if (o == x) {
            Boolean isflag = FALSE;
            Delete(i, &isflag);
            *value = TRUE;
            return NOERROR;
        }
        i = (i + 1) & mask;
    }
    *value = FALSE;
    return NOERROR;
}

ECode CArrayDeque::RemoveLastOccurrence(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    if (o == NULL) {
        *value = FALSE;
        return NOERROR;
    }
    Int32 mask = mElements->GetLength() - 1;
    Int32 i = (mTail - 1) & mask;
    AutoPtr<IInterface> x;
    while ( (x = (*mElements)[i]) != NULL) {
        if (o == x) {
            Boolean isflag = FALSE;
            Delete(i, &isflag);
            *value = TRUE;
            return NOERROR;
        }
        i = (i - 1) & mask;
    }
    *value = FALSE;
    return NOERROR;
}

ECode CArrayDeque::Add(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    AddLast(e);
    *value = TRUE;
    return NOERROR;
}

ECode CArrayDeque::Offer(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    return OfferLast(e, value);
}

ECode CArrayDeque::Remove(
    /* [out] */ IInterface** e)
{
    return RemoveFirst(e);
}

ECode CArrayDeque::Poll(
    /* [out] */ IInterface** e)
{
    return PollFirst(e);
}

ECode CArrayDeque::GetElement(
    /* [out] */ IInterface** e)
{
    return GetFirst(e);
}

ECode CArrayDeque::Peek(
    /* [out] */ IInterface** e)
{
    return PeekFirst(e);
}

ECode CArrayDeque::Push(
    /* [in] */ IInterface* e)
{
    return AddFirst(e);
}

ECode CArrayDeque::Pop(
    /* [out] */ IInterface** e)
{
    return RemoveFirst(e);
}

ECode CArrayDeque::CheckInvariants()
{
    assert((*mElements)[mTail] == NULL);
    assert(mHead == mTail ? (*mElements)[mHead] == NULL :
        ((*mElements)[mHead] != NULL && (*mElements)[(mTail - 1) & (mElements->GetLength() - 1)] != NULL));
    assert((*mElements)[(mHead - 1) & (mElements->GetLength() - 1)] == NULL);
    return NOERROR;
}

ECode CArrayDeque::Delete(
    /* [in] */ Int32 i,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    CheckInvariants();
    AutoPtr<ArrayOf<IInterface*> > elements = mElements;
    Int32 mask = mElements->GetLength() - 1;
    Int32 h = mHead;
    Int32 t = mTail;
    Int32 front = (i - h) & mask;
    Int32 back  = (t - i) & mask;

    // Invariant: head <= i < tail mod circularity
    if (front >= ((t - h) & mask)) {
        // throw new ConcurrentModificationException();
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }

    // Optimize for least element motion
    if (front < back) {
        if (h <= i) {
            mElements->Copy(h + 1, mElements, h, front);
        }
        else { // Wrap around
            mElements->Copy(1, mElements, 0, i);
            mElements->Set(0, (*mElements)[mask]);
            mElements->Copy(h + 1, mElements, h, mask - h);
        }
        mElements->Set(h, NULL);
        mHead = (h + 1) & mask;
        *result = FALSE;
    }
    else {
        if (i < t) { // Copy the null tail as well
            mElements->Copy(i, mElements, i + 1, back);
            mTail = t - 1;
        }
        else { // Wrap around
            mElements->Copy(i, mElements, i + 1, mask - i);
            mElements->Set(mask, (*mElements)[0]);
            mElements->Copy(0, mElements, 1, t);
            mTail = (t - 1) & mask;
        }
        *result = TRUE;
    }
    return NOERROR;
}

ECode CArrayDeque::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)

    *size = (mTail - mHead) & (mElements->GetLength() - 1);
    return NOERROR;
}

ECode CArrayDeque::IsEmpty(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mHead == mTail;
    return NOERROR;
}

ECode CArrayDeque::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)

    AutoPtr<IIterator> res = new DeqIterator(this);
    *it = res;
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode CArrayDeque::GetDescendingIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)

    AutoPtr<IIterator> res = new DescendingIterator(this);
    *it = res;
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode CArrayDeque::Contains(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    if (o == NULL) {
        *value = FALSE;
        return NOERROR;
    }
    Int32 mask = mElements->GetLength() - 1;
    Int32 i = mHead;
    AutoPtr<IInterface> x;
    while ( (x = (*mElements)[i]) != NULL) {
        if (o == x) {
            *value = TRUE;
            return NOERROR;
        }
        i = (i + 1) & mask;
    }
    *value = FALSE;
    return NOERROR;
}

ECode CArrayDeque::Remove(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* value)
{
    return RemoveFirstOccurrence(o, value);
}

ECode CArrayDeque::Clear()
{
    Int32 h = mHead;
    Int32 t = mTail;
    if (h != t) { // clear all cells
        mHead = mTail = 0;
        Int32 i = h;
        Int32 mask = mElements->GetLength() - 1;
        do {
            mElements->Set(i, NULL);
            i = (i + 1) & mask;
        } while (i != t);
    }
    return NOERROR;
}

ECode CArrayDeque::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array)

    Int32 value = 0;
    GetSize(&value);
    AutoPtr<ArrayOf<IInterface*> > res = ArrayOf<IInterface*>::Alloc(value);
    *array = CopyElements(res);
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode CArrayDeque::ToArray(
    /* [in] */ ArrayOf<IInterface*>* _a,
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array)

    Int32 size = 0;
    GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > a = a->GetLength() < size ?
        ArrayOf<IInterface*>::Alloc(size) : _a;

    CopyElements(a);
    if (a->GetLength() > size) {
        a->Set(size, NULL);
    }
    *array = a;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode CArrayDeque::Clone(
    /* [out] */ IInterface** outarray)
{
    VALIDATE_NOT_NULL(outarray)

    // try {
    // @SuppressWarnings("unchecked")
    AutoPtr<CArrayDeque> result;
    FAIL_RETURN(CArrayDeque::NewByFriend((CArrayDeque**)&result));
    result->mElements->Copy(mElements, mElements->GetLength());
    *outarray = result->Probe(EIID_IInterface);
    REFCOUNT_ADD(*outarray)

    // } catch (CloneNotSupportedException e) {
    //     throw new AssertionError();
    // }
    return NOERROR;
}

ECode CArrayDeque::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::AddAll(collection, modified);
}

ECode CArrayDeque::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::ContainsAll(collection, result);
}

ECode CArrayDeque::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = this->Probe(EIID_IInterface) == object;
    return NOERROR;
}

ECode CArrayDeque::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = (Int32)this;
    return NOERROR;
}

ECode CArrayDeque::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::RemoveAll(collection, modified);
}

ECode CArrayDeque::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::RetainAll(collection, modified);
}

ECode CArrayDeque::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    stream->DefaultWriteObject();

    // Write out size
    Int32 value = 0;
    GetSize(&value);
    (IOutputStream::Probe(stream))->Write(value);

    // Write out elements in order.
    Int32 mask = mElements->GetLength() - 1;
    for (Int32 i = mHead; i != mTail; i = (i + 1) & mask) {
        assert(0 && "TODO");
        // stream->WriteObject((*mElements)[i]);
    }
    return NOERROR;
}

ECode CArrayDeque::ReadObject(
    /* [in] */ IObjectInputStream* stream)
{
    stream->DefaultReadObject();

    // Read in size and allocate array
    Int32 size = 0;
    (IInputStream::Probe(stream))->Read(&size);
    AllocateElements(size);
    mHead = 0;
    mTail = size;

    // Read in all elements in the proper order.
    for (Int32 i = 0; i < size; i++) {
        assert(0 && "TODO");
        // (*mElements)[i] = stream->ReadObject();
    }
    return NOERROR;
}

ECode CArrayDeque::Add(
    /* [in] */ IInterface* object)
{
    Boolean bval;
    return Add(object, &bval);
}

ECode CArrayDeque::AddAll(
    /* [in] */ ICollection* collection)
{
    Boolean bval;
    return AddAll(collection, &bval);
}

ECode CArrayDeque::Remove(
    /* [in] */ IInterface* object)
{
    Boolean bval;
    return Remove(object, &bval);
}

ECode CArrayDeque::RemoveAll(
    /* [in] */ ICollection* collection)
{
    Boolean bval;
    return RemoveAll(collection, &bval);
}

ECode CArrayDeque::RetainAll(
    /* [in] */ ICollection* collection)
{
    Boolean bval;
    return RetainAll(collection, &bval);
}

} // namespace Utility
} // namespace Elastos
