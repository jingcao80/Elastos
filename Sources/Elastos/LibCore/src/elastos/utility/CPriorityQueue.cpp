
#include "Elastos.CoreLibrary.IO.h"
#include "CPriorityQueue.h"
#include "Arrays.h"
#include "Math.h"

using Elastos::Core::IComparable;
using Elastos::IO::EIID_ISerializable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Utility {

//==========================================================
//       CPriorityQueue::PriorityIterator
//==========================================================

CPriorityQueue::PriorityIterator::PriorityIterator(
    /* [in] */ CPriorityQueue* host)
    : mCurrentIndex(-1)
    , mAllowRemove(FALSE)
    , mHost(host)
{
}

CAR_INTERFACE_IMPL(CPriorityQueue::PriorityIterator, Object, IIterator)

ECode CPriorityQueue::PriorityIterator::HasNext(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mCurrentIndex < mHost->mSize - 1;
    return NOERROR;
}

ECode CPriorityQueue::PriorityIterator::GetNext(
    /* [out] */ IInterface** outinter)
{
    VALIDATE_NOT_NULL(outinter)
    *outinter = NULL;

    Boolean isflag = FALSE;
    if (!(HasNext(&isflag), isflag)) {
        // throw new NoSuchElementException();
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    mAllowRemove = TRUE;
    *outinter = (*mHost->mElements)[++mCurrentIndex];
    REFCOUNT_ADD(*outinter)
    return NOERROR;
}

ECode CPriorityQueue::PriorityIterator::Remove()
{
    if (!mAllowRemove) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mAllowRemove = FALSE;
    return mHost->RemoveAt(mCurrentIndex--);
}


//==========================================================
//       CPriorityQueue
//==========================================================

CAR_INTERFACE_IMPL_2(CPriorityQueue, AbstractQueue, IPriorityQueue, ISerializable)

const Int64 CPriorityQueue::sSerialVersionUID;

const Int32 CPriorityQueue::DEFAULT_CAPACITY;

const Double CPriorityQueue::DEFAULT_INIT_CAPACITY_RATIO;

const Int32 CPriorityQueue::DEFAULT_CAPACITY_RATIO;

ECode CPriorityQueue::constructor()
{
    return this->constructor(DEFAULT_CAPACITY);
}

ECode CPriorityQueue::constructor(
    /* [in] */ Int32 initialCapacity)
{
    return this->constructor(initialCapacity, NULL);
}

ECode CPriorityQueue::constructor(
    /* [in] */ Int32 initialCapacity,
    /* [in] */ IComparator* comparator)
{
    if (initialCapacity < 1) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mElements = NewElementArray(initialCapacity);
    mComparator = comparator;
    return NOERROR;
}

ECode CPriorityQueue::constructor(
    /* [in] */ ICollection* c)
{
    if (IPriorityQueue::Probe(c)) {
        GetFromPriorityQueue(IPriorityQueue::Probe(c));
    }
    else if (ISortedSet::Probe(c)) {
        GetFromSortedSet(ISortedSet::Probe(c));
    }
    else {
        InitSize(c);
        Boolean isflag = FALSE;
        AddAll(c, &isflag);
    }
    return NOERROR;
}

ECode CPriorityQueue::constructor(
    /* [in] */ IPriorityQueue* c)
{
    return GetFromPriorityQueue(c);
}

ECode CPriorityQueue::constructor(
    /* [in] */ ISortedSet* c)
{
    return GetFromSortedSet(c);
}

ECode CPriorityQueue::GetComparator(
    /* [out] */ IComparator** outcom)
{
    VALIDATE_NOT_NULL(outcom)
    *outcom = NULL;

    *outcom = mComparator;
    REFCOUNT_ADD(*outcom)
    return NOERROR;
}

ECode CPriorityQueue::Offer(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* result)
{
    if (e == NULL) {
        // throw new NullPointerException("o == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    GrowToSize(mSize + 1);
    mElements->Set(mSize, e);
    SiftUp(mSize++);
    *result = TRUE;
    return NOERROR;
}

ECode CPriorityQueue::Remove(
    /* [out] */ IInterface** e)
{
    return AbstractQueue::Remove(e);
}

ECode CPriorityQueue::Poll(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)
    *e = NULL;

    Boolean isflag = FALSE;
    if (IsEmpty(&isflag), isflag) {
        *e = NULL;
        return NOERROR;
    }
    AutoPtr<IInterface> result = (*mElements)[0];
    RemoveAt(0);
    *e = result;
    REFCOUNT_ADD(*e)
    return NOERROR;
}

ECode CPriorityQueue::GetElement(
    /* [out] */ IInterface** e)
{
    return AbstractQueue::GetElement(e);
}

ECode CPriorityQueue::Peek(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)
    *e = NULL;

    Boolean isflag = FALSE;
    if (IsEmpty(&isflag), isflag) {
        *e = NULL;
        return NOERROR;
    }
    *e = (*mElements)[0];
    REFCOUNT_ADD(*e)
    return NOERROR;
}

ECode CPriorityQueue::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return Offer(object, modified);
}

ECode CPriorityQueue::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractQueue::AddAll(collection, modified);
}

ECode CPriorityQueue::Clear()
{
    Arrays::Fill(mElements, (IInterface*)NULL);
    mSize = 0;
    return NOERROR;
}

ECode CPriorityQueue::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractQueue::Contains(object, result);
}

ECode CPriorityQueue::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractQueue::ContainsAll(collection, result);
}

ECode CPriorityQueue::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = Object::Equals(object, this);
    return NOERROR;
}

ECode CPriorityQueue::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    *hashCode = Object::GetHashCode(this);
    return NOERROR;
}

ECode CPriorityQueue::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractQueue::IsEmpty(result);
}

ECode CPriorityQueue::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    if (object == NULL) {
        *modified = FALSE;
        return NOERROR;
    }
    for (Int32 targetIndex = 0; targetIndex < mSize; targetIndex++) {
        if (Object::Equals(object, (*mElements)[targetIndex])) {
            RemoveAt(targetIndex);
            *modified = TRUE;
            return NOERROR;
        }
    }
    *modified = FALSE;
    return NOERROR;
}

ECode CPriorityQueue::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractQueue::RemoveAll(collection, modified);
}

ECode CPriorityQueue::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractQueue::RetainAll(collection, modified);
}

ECode CPriorityQueue::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)

    *size = mSize;
    return NOERROR;
}

ECode CPriorityQueue::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractQueue::ToArray(array);
}

ECode CPriorityQueue::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractQueue::ToArray(inArray, outArray);
}

ECode CPriorityQueue::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)
    *it = NULL;

    AutoPtr<IIterator> outit = (IIterator*) new PriorityIterator(this);
    *it = outit;
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode CPriorityQueue::ReadObject(
    /* [in] */ IObjectInputStream* in)
{
    in->DefaultReadObject();
    Int32 capacity = 0;
    (IInputStream::Probe(in))->Read(&capacity);
    mElements = NewElementArray(capacity);
    for (Int32 i = 0; i < mSize; i++) {
        assert(0 && "TODO");
        // mElements[i] = (E) in.readObject();
    }
    return NOERROR;
}

AutoPtr< ArrayOf<IInterface*> > CPriorityQueue::NewElementArray(
    /* [in] */ Int32 capacity)
{
    AutoPtr< ArrayOf<IInterface*> > outres = ArrayOf<IInterface*>::Alloc(capacity);
    return outres;
}

ECode CPriorityQueue::WriteObject(
    /* [in] */ IObjectOutputStream* out)
{
    out->DefaultWriteObject();
    (IOutputStream::Probe(out))->Write(mElements->GetLength());
    for (Int32 i = 0; i < mSize; i++) {
        assert(0 && "TODO");
        // out.writeObject(mElements[i]);
    }
    return NOERROR;
}

ECode CPriorityQueue::GetFromPriorityQueue(
    /* [in] */ IPriorityQueue* c)
{
    FAIL_RETURN(InitSize(ICollection::Probe(c)));
    c->GetComparator((IComparator**)&mComparator);
    mElements->Copy(0, ((CPriorityQueue*)c)->mElements, 0, ((CPriorityQueue*)c)->mSize);
    mSize = ((CPriorityQueue*)c)->mSize;
    return NOERROR;
}

ECode CPriorityQueue::GetFromSortedSet(
    /* [in] */ ISortedSet* c)
{
    FAIL_RETURN(InitSize(ICollection::Probe(c)));
    c->GetComparator((IComparator**)&mComparator);
    AutoPtr<IIterator> iter;
    (IIterable::Probe(c))->GetIterator((IIterator**)&iter);
    Boolean isflag = FALSE;
    while (iter->HasNext(&isflag), isflag) {
        AutoPtr<IInterface> outface;
        iter->GetNext((IInterface**)&outface);
        mElements->Set(mSize++, outface);
    }
    return NOERROR;
}

ECode CPriorityQueue::RemoveAt(
    /* [in] */ Int32 index)
{
    mSize--;
    mElements->Set(index, (*mElements)[mSize]);
    SiftDown(index);
    (*mElements)[mSize] = NULL;
    return NOERROR;
}

Int32 CPriorityQueue::Compare(
    /* [in] */ IInterface* o1,
    /* [in] */ IInterface* o2)
{
    Int32 value = 0;
    if (mComparator != NULL) {
        mComparator->Compare(o1, o2, &value);
        return value;
    }
    else {
        IComparable::Probe(o1)->CompareTo(o2, &value);
    }
    return value;
}

ECode CPriorityQueue::SiftUp(
    /* [in] */ Int32 childIndex)
{
    AutoPtr<IInterface> target = (*mElements)[childIndex];
    Int32 parentIndex = 0;
    while (childIndex > 0) {
        parentIndex = (childIndex - 1) / 2;
        AutoPtr<IInterface> parent = (*mElements)[parentIndex];
        if (Compare(parent, target) <= 0) {
            break;
        }
        mElements->Set(childIndex, parent);
        childIndex = parentIndex;
    }
    mElements->Set(childIndex, target);
    return NOERROR;
}

ECode CPriorityQueue::SiftDown(
    /* [in] */ Int32 rootIndex)
{
    AutoPtr<IInterface> target = (*mElements)[rootIndex];
    Int32 childIndex = 0;
    while ((childIndex = rootIndex * 2 + 1) < mSize) {
        if (childIndex + 1 < mSize
                && Compare((*mElements)[childIndex + 1], (*mElements)[childIndex]) < 0) {
            childIndex++;
        }
        if (Compare(target, (*mElements)[childIndex]) <= 0) {
            break;
        }
        mElements->Set(rootIndex, (*mElements)[childIndex]);
        rootIndex = childIndex;
    }
    mElements->Set(rootIndex, target);
    return NOERROR;
}

ECode CPriorityQueue::InitSize(
    /* [in] */ ICollection* c)
{
    if (c == NULL) {
        // throw new NullPointerException("c == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    Boolean isflag = FALSE;
    if (c->IsEmpty(&isflag), isflag) {
        mElements = NewElementArray(1);
    }
    else {
        Int32 sizelen = 0;
        c->GetSize(&sizelen);
        Int32 capacity = (Int32) Elastos::Core::Math::Ceil(sizelen
                * DEFAULT_INIT_CAPACITY_RATIO);
        mElements = NewElementArray(capacity);
    }
    return NOERROR;
}

ECode CPriorityQueue::GrowToSize(
    /* [in] */ Int32 mSize)
{
    if (mSize > mElements->GetLength()) {
        AutoPtr< ArrayOf<IInterface*> > newElements = NewElementArray(mSize * DEFAULT_CAPACITY_RATIO);
        newElements->Copy(0, mElements, 0, mElements->GetLength());
        mElements = newElements;
    }
    return NOERROR;
}

ECode CPriorityQueue::Add(
    /* [in] */ IInterface* object)
{
    return AbstractQueue::Add(object);
}

ECode CPriorityQueue::AddAll(
    /* [in] */ ICollection* collection)
{
    return AbstractQueue::AddAll(collection);
}

ECode CPriorityQueue::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return AbstractQueue::RemoveAll(collection);
}

ECode CPriorityQueue::RetainAll(
    /* [in] */ ICollection* collection)
{
    return AbstractQueue::RetainAll(collection);
}

} // namespace Utility
} // namespace Elastos
