
#include "Elastos.CoreLibrary.IO.h"
#include "CopyOnWriteArrayList.h"
#include "CCopyOnWriteArrayList.h"
#include "Arrays.h"
#include "AutoLock.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::Arrays;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//==========================================================
//       CopyOnWriteArrayList
//==========================================================
const AutoPtr< ArrayOf<IInterface*> > CopyOnWriteArrayList::sEmptyArray = ArrayOf<IInterface*>::Alloc(0);

CAR_INTERFACE_IMPL_7(CopyOnWriteArrayList, Object, \
    ICopyOnWriteArrayList, IList, ICollection, IIterable, IRandomAccess, ICloneable, ISerializable)

ECode CopyOnWriteArrayList::constructor()
{
    mElements = sEmptyArray;
    return NOERROR;
}

ECode CopyOnWriteArrayList::constructor(
    /* [in] */ ICollection* collection)
{
    VALIDATE_NOT_NULL(collection)

    AutoPtr< ArrayOf<IInterface*> > outarr;
    collection->ToArray((ArrayOf<IInterface*>**)&outarr);
    return constructor(outarr);
}

ECode CopyOnWriteArrayList::constructor(
    /* [in] */ ArrayOf<IInterface*>* array)
{
    mElements = array->Clone();
    return NOERROR;
}

ECode CopyOnWriteArrayList::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    AutoLock lock(this);
    AutoPtr< ArrayOf<IInterface*> > newElements = ArrayOf<IInterface*>::Alloc(mElements->GetLength() + 1);
    newElements->Copy(mElements);
    newElements->Set(mElements->GetLength(), object);
    mElements = newElements;
    *modified = TRUE;
    return NOERROR;
}

ECode CopyOnWriteArrayList::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AddAll(mElements->GetLength(), collection, modified);
}

ECode CopyOnWriteArrayList::AddAll(
    /* [in] */ ICollection* collection)
{
    Boolean bval;
    return AddAll(collection, &bval);
}

ECode CopyOnWriteArrayList::Clear()
{
    mElements = sEmptyArray;
    return NOERROR;
}

ECode CopyOnWriteArrayList::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 value = 0;
    FAIL_RETURN(IndexOf(object, &value));
    *result = value != -1;
    return NOERROR;
}

ECode CopyOnWriteArrayList::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr< ArrayOf<IInterface*> > snapshot = mElements;
    return ContainsAll(collection, snapshot, 0, snapshot->GetLength(), result);
}

ECode CopyOnWriteArrayList::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)


    if (ICopyOnWriteArrayList::Probe(object)) {
        CopyOnWriteArrayList* o = (CopyOnWriteArrayList*)ICopyOnWriteArrayList::Probe(object);
        *result = TO_IINTERFACE(this) == IInterface::Probe(object)
                || Arrays::Equals(mElements, o->mElements);
        return NOERROR;
    }
    else if (IList::Probe(object)) {
        AutoPtr< ArrayOf<IInterface*> > snapshot = mElements;
        AutoPtr<IIterator> i;
        IList::Probe(object)->GetIterator((IIterator**)&i);
        Boolean isflag = FALSE;
        for (Int32 j = 0; j < snapshot->GetLength(); j++) {
            AutoPtr<IInterface> o = (*snapshot)[j];
            AutoPtr<IInterface> outface;
            if (!(i->HasNext(&isflag), isflag) || !(Object::Equals(o, (i->GetNext((IInterface**)&outface), outface)))) {
                *result = FALSE;
                return NOERROR;
            }
        }
        *result = !(i->HasNext(&isflag), isflag);
        return NOERROR;
    }
    else {
        *result = FALSE;
        return NOERROR;
    }
}

ECode CopyOnWriteArrayList::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    assert(0);
//    return Arrays::_HashCodeObject(mElements, hashCode);
    return NOERROR;
}

ECode CopyOnWriteArrayList::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mElements->GetLength() == 0;
    return NOERROR;
}

ECode CopyOnWriteArrayList::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    AutoLock lock(this);
    Int32 index = 0;
    IndexOf(object, &index);
    if (index == -1) {
        *modified = FALSE;
        return NOERROR;
    }
    AutoPtr<IInterface> outface;
    Remove(index, (IInterface**)&outface);
    *modified = TRUE;
    return NOERROR;
}


ECode CopyOnWriteArrayList::Remove(
    /* [in] */ IInterface* object)
{
    Boolean bval;
    return Remove(object, &bval);
}

ECode CopyOnWriteArrayList::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    AutoLock lock(this);
    *modified = RemoveOrRetain(collection, FALSE, 0, mElements->GetLength()) != 0;
    return NOERROR;
}


ECode CopyOnWriteArrayList::RemoveAll(
    /* [in] */ ICollection* collection)
{
    Boolean bval;
    return RemoveAll(collection, &bval);
}

ECode CopyOnWriteArrayList::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    AutoLock lock(this);
    *modified = RemoveOrRetain(collection, TRUE, 0, mElements->GetLength()) != 0;
    return NOERROR;
}

ECode CopyOnWriteArrayList::RetainAll(
    /* [in] */ ICollection* collection)
{
    Boolean bval;
    return RetainAll(collection, &bval);
}

ECode CopyOnWriteArrayList::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)

    *size = mElements->GetLength();
    return NOERROR;
}

ECode CopyOnWriteArrayList::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array)

    AutoPtr< ArrayOf<IInterface*> > outarr = mElements->Clone();
    *array = outarr;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode CopyOnWriteArrayList::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray)

    AutoPtr< ArrayOf<IInterface*> > snapshot = mElements;
    if (snapshot->GetLength() > inArray->GetLength()) {
        AutoPtr< ArrayOf<IInterface*> > outarr = snapshot->Clone();
        *outArray = outarr;
        REFCOUNT_ADD(*outArray)
        return NOERROR;
    }
    inArray->Copy(0, snapshot, 0, snapshot->GetLength());
    if (snapshot->GetLength() < inArray->GetLength()) {
        (*inArray)[snapshot->GetLength()] = NULL;
    }
    *outArray = inArray;
    REFCOUNT_ADD(*outArray)
    return NOERROR;
}

ECode CopyOnWriteArrayList::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)

    AutoPtr< ArrayOf<IInterface*> > snapshot = mElements;
    AutoPtr<IIterator> outit = (IIterator*) new CowIterator(snapshot, 0, snapshot->GetLength());
    *it = outit;
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode CopyOnWriteArrayList::Add(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    AutoLock lock(this);
    AutoPtr< ArrayOf<IInterface*> > newElements = ArrayOf<IInterface*>::Alloc(mElements->GetLength() + 1);
    newElements->Copy(0, mElements, 0, location);
    newElements->Set(location, object);
    newElements->Copy(location + 1, mElements, location, mElements->GetLength() - location);
    mElements = newElements;
    return NOERROR;
}

ECode CopyOnWriteArrayList::Add(
    /* [in] */ IInterface* object)
{
    Boolean result;
    return Add(object, &result);
}

ECode CopyOnWriteArrayList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    AutoLock lock(this);
    AutoPtr< ArrayOf<IInterface*> > toAdd;
    collection->ToArray((ArrayOf<IInterface*>**)&toAdd);
    AutoPtr< ArrayOf<IInterface*> > newElements = ArrayOf<IInterface*>::Alloc(mElements->GetLength() + toAdd->GetLength());
    newElements->Copy(0, mElements, 0, location);
    newElements->Copy(location, toAdd, 0, toAdd->GetLength());
    newElements->Copy(location + toAdd->GetLength(), mElements, location, mElements->GetLength() - location);
    mElements = newElements;
    *modified = toAdd->GetLength() > 0;
    return NOERROR;
}

ECode CopyOnWriteArrayList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection)
{
    AutoLock lock(this);
    Boolean res;
    return AddAll(location, collection, &res);
}

ECode CopyOnWriteArrayList::Get(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    AutoPtr<IInterface> result = (*mElements)[location];
    *object = result;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CopyOnWriteArrayList::IndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    AutoPtr< ArrayOf<IInterface*> > snapshot = mElements;
    return IndexOf(object, snapshot, 0, snapshot->GetLength(), index);
}

ECode CopyOnWriteArrayList::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    AutoPtr< ArrayOf<IInterface*> > snapshot = mElements;
    return LastIndexOf(object, snapshot, 0, snapshot->GetLength(), index);
}

ECode CopyOnWriteArrayList::GetListIterator(
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it)

    AutoPtr< ArrayOf<IInterface*> > snapshot = mElements;
    AutoPtr<IListIterator> outit = (IListIterator*) new CowIterator(snapshot, 0, snapshot->GetLength());
    *it = outit;
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode CopyOnWriteArrayList::GetListIterator(
    /* [in] */ Int32 location,
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it)

    AutoPtr< ArrayOf<IInterface*> > snapshot = mElements;
    if (location < 0 || location > snapshot->GetLength()) {
        *it = NULL;
        // throw new IndexOutOfBoundsException("index=" + index + ", length=" + snapshot.length);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    AutoPtr<CowIterator> result = new CowIterator(snapshot, 0, snapshot->GetLength());
    result->mIndex = location;
    *it = (IListIterator*)result.Get();
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode CopyOnWriteArrayList::Remove(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    AutoLock lock(this);
    AutoPtr<IInterface> removed = (*mElements)[location];
    RemoveRange(location, location + 1);
    *object = removed;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CopyOnWriteArrayList::Remove(
    /* [in] */ Int32 location)
{
    AutoPtr<IInterface> res;
    return Remove(location, (IInterface**)&res);
}

ECode CopyOnWriteArrayList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** prevObject)
{
    VALIDATE_NOT_NULL(prevObject)

    AutoPtr< ArrayOf<IInterface*> > newElements = mElements->Clone();
    AutoPtr<IInterface> result = (*newElements)[location];
    newElements->Set(location, object);
    mElements = newElements;
    *prevObject = result;
    REFCOUNT_ADD(*prevObject)
    return NOERROR;
}

ECode CopyOnWriteArrayList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    AutoPtr<IInterface> res;
    return Set(location, object, (IInterface**)&res);
}

ECode CopyOnWriteArrayList::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList)

    AutoPtr< ArrayOf<IInterface*> > snapshot = mElements;
    if (start < 0 || start > end || end > snapshot->GetLength()) {
        *subList = NULL;
        // throw new IndexOutOfBoundsException("from=" + from + ", to=" + to +
        //         ", list size=" + snapshot.length);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    AutoPtr<IList> outlist = (IList*) new CowSubList(snapshot, start, end, this);
    *subList = outlist;
    REFCOUNT_ADD(*subList)
    return NOERROR;
}

ECode CopyOnWriteArrayList::IndexOf(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 from,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr< ArrayOf<IInterface*> > snapshot = mElements;
    return IndexOf(object, snapshot, from, snapshot->GetLength(), value);
}

ECode CopyOnWriteArrayList::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 to,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr< ArrayOf<IInterface*> > snapshot = mElements;
    return LastIndexOf(object, snapshot, 0, to, value);
}

ECode CopyOnWriteArrayList::AddAllAbsent(
    /* [in] */ ICollection* collection,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    AutoLock lock(this);
    AutoPtr< ArrayOf<IInterface*> > toAdd;
    collection->ToArray((ArrayOf<IInterface*>**)&toAdd);
    AutoPtr< ArrayOf<IInterface*> > newElements = ArrayOf<IInterface*>::Alloc(mElements->GetLength() + toAdd->GetLength());
    newElements->Copy(0, mElements, 0, mElements->GetLength());
    Int32 addedCount = 0;
    for (Int32 i = 0; i < toAdd->GetLength(); i++) {
        AutoPtr<IInterface> o = (*toAdd)[i];
        Int32 outvalue = 0;
        if ((IndexOf(o, newElements, 0, mElements->GetLength() + addedCount, &outvalue), outvalue) == -1) {
            newElements->Set(mElements->GetLength() + addedCount++, o);
        }
    }
    if (addedCount < toAdd->GetLength()) {
        assert(0);
//        Arrays::_CopyOfRange(newElements, 0, mElements->GetLength() + addedCount, (ArrayOf<IInterface*>**)&newElements); // trim to size
    }
    mElements = newElements;
    *value = addedCount;
    return NOERROR;
}

ECode CopyOnWriteArrayList::AddIfAbsent(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    AutoLock lock(this);
    Boolean isflag = FALSE;
    if (Contains(object, &isflag), isflag) {
        *value = FALSE;
        return NOERROR;
    }
    Add(object, &isflag);
    *value = TRUE;
    return NOERROR;
}

ECode CopyOnWriteArrayList::GetArray(
    /* [out, callee] */ ArrayOf<IInterface*>** outarr)
{
    VALIDATE_NOT_NULL(outarr)

    // CopyOnWriteArraySet needs this.
    *outarr = mElements;
    REFCOUNT_ADD(*outarr)
    return NOERROR;
}

ECode CopyOnWriteArrayList::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    // try {
    AutoPtr<CCopyOnWriteArrayList> result;
    CCopyOnWriteArrayList::NewByFriend((CCopyOnWriteArrayList**)&result);
    result->mElements = mElements->Clone();
    *object = (ICopyOnWriteArrayList*)result.Get();
    REFCOUNT_ADD(*object)
    // } catch (CloneNotSupportedException e) {
    //     throw new AssertionError(e);
    // }
    return NOERROR;
}

ECode CopyOnWriteArrayList::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [in] */ ArrayOf<IInterface*>* snapshot,
    /* [in] */ Int32 from,
    /* [in] */ Int32 to,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr<IIterator> it;
    collection->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        Int32 midvalue = 0;
        if ((IndexOf(obj, snapshot, from, to, &midvalue), midvalue) == -1) {
            *value = FALSE;
            return NOERROR;
        }
    }
    *value = TRUE;
    return NOERROR;
}

ECode CopyOnWriteArrayList::LastIndexOf(
    /* [in] */ IInterface* o,
    /* [in] */ ArrayOf<IInterface*>* data,
    /* [in] */ Int32 from,
    /* [in] */ Int32 to,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    if (o == NULL) {
        for (Int32 i = to - 1; i >= from; i--) {
            if ((*data)[i] == NULL) {
                *value = i;
                return NOERROR;
            }
        }
    }
    else {
        for (Int32 i = to - 1; i >= from; i--) {
            if (Object::Equals(o, (*data)[i])) {
                *value = i;
                return NOERROR;
            }
        }
    }
    *value = -1;
    return NOERROR;
}

ECode CopyOnWriteArrayList::IndexOf(
    /* [in] */ IInterface* o,
    /* [in] */ ArrayOf<IInterface*>* data,
    /* [in] */ Int32 from,
    /* [in] */ Int32 to,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    if (o == NULL) {
        for (Int32 i = from; i < to; i++) {
            if ((*data)[i] == NULL) {
                *value = i;
                return NOERROR;
            }
        }
    }
    else {
        for (Int32 i = from; i < to; i++) {
            if (Object::Equals(o, (*data)[i])) {
                *value = i;
                return NOERROR;
            }
        }
    }
    *value = -1;
    return NOERROR;
}

Int32 CopyOnWriteArrayList::RemoveOrRetain(
    /* [in] */ ICollection* collection,
    /* [in] */ Boolean retain,
    /* [in] */ Int32 from,
    /* [in] */ Int32 to)
{
    Boolean isflag = FALSE;
    for (Int32 i = from; i < to; i++) {
        if ((collection->Contains((*mElements)[i], &isflag), isflag) == retain) {
            continue;
        }

        /*
         * We've encountered an element that must be removed! Create a new
         * array and copy in the surviving mElements one by one.
         */
        AutoPtr< ArrayOf<IInterface*> > newElements = ArrayOf<IInterface*>::Alloc(mElements->GetLength() - 1);
        newElements->Copy(0, mElements, 0, i);
        Int32 newSize = i;
        for (Int32 j = i + 1; j < to; j++) {
            if ((collection->Contains((*mElements)[j], &isflag), isflag) == retain) {
                newElements->Set(newSize++, (*mElements)[j]);
            }
        }

        /*
         * Copy the mElements after 'to'. This is only useful for sub lists,
         * where 'to' will be less than mElements.length.
         */
        newElements->Copy(newSize, mElements, to, mElements->GetLength() - to);
        newSize += (mElements->GetLength() - to);

        if (newSize < newElements->GetLength()) {
            AutoPtr< ArrayOf<IInterface*> > newElements2 = ArrayOf<IInterface*>::Alloc(newSize);
            newElements2->Copy(newElements, 0, newSize);
            //Arrays::_CopyOfRange(newElements, 0, newSize, (ArrayOf<IInterface*>**)&newElements2); // trim to size
            newElements = newElements2;
        }
        Int32 removed = mElements->GetLength() - newElements->GetLength();
        mElements = newElements;
        return removed;
    }

    // we made it all the way through the loop without making any changes
    return 0;
}

void CopyOnWriteArrayList::RemoveRange(
    /* [in] */ Int32 from,
    /* [in] */ Int32 to)
{
    AutoPtr< ArrayOf<IInterface*> > newElements = ArrayOf<IInterface*>::Alloc(mElements->GetLength() - (to - from));
    newElements->Copy(0, mElements, 0, from);
    newElements->Copy(from, mElements, to, mElements->GetLength() - to);
    mElements = newElements;
}

ECode CopyOnWriteArrayList::WriteObject(
    /* [in] */ IObjectOutputStream* out)
{
    AutoPtr< ArrayOf<IInterface*> > snapshot = mElements;
    out->DefaultWriteObject();
//    out->Write(snapshot->GetLength());
    for (Int32 i = 0; i < snapshot->GetLength(); i++) {
        AutoPtr<IInterface> o = (*snapshot)[i];
        assert(0 && "TODO");
        // out->WriteObject(o);
    }
    return NOERROR;
}

ECode CopyOnWriteArrayList::ReadObject(
    /* [in] */ IObjectInputStream* in)
{
    in->DefaultReadObject();
    Int32 length = 0;
//    in->Read(&length);
    AutoPtr< ArrayOf<IInterface*> > snapshot = ArrayOf<IInterface*>::Alloc(length);
    for (Int32 i = 0; i < snapshot->GetLength(); i++) {
        assert(0 && "TODO");
        // snapshot[i] = in.readObject();
    }
    mElements = snapshot;
    return NOERROR;
}


//==========================================================
//       CopyOnWriteArrayList::Slice
//==========================================================
CopyOnWriteArrayList::Slice::Slice(
    /* [in] */ ArrayOf<IInterface*>* expectedElements,
    /* [in] */ Int32 from,
    /* [in] */ Int32 to)
{
    mExpectedElements = expectedElements;
    mFrom = from;
    mTo = to;
}

ECode CopyOnWriteArrayList::Slice::CheckElementIndex(
    /* [in] */ Int32 index)
{
    if (index < 0 || index >= mTo - mFrom) {
        // throw new IndexOutOfBoundsException("index=" + index + ", size=" + (to - from));
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

ECode CopyOnWriteArrayList::Slice::CheckPositionIndex(
    /* [in] */ Int32 index)
{
    if (index < 0 || index > mTo - mFrom) {
        // throw new IndexOutOfBoundsException("index=" + index + ", size=" + (to - from));
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

ECode CopyOnWriteArrayList::Slice::CheckConcurrentModification(
    /* [in] */ ArrayOf<IInterface*>* snapshot)
{
    if (!Arrays::Equals(mExpectedElements, snapshot)) {
        // throw new ConcurrentModificationException();
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    return NOERROR;
}


//==========================================================
//       CopyOnWriteArrayList::CowSubList
//==========================================================
CopyOnWriteArrayList::CowSubList::CowSubList(
    /* [in] */ ArrayOf<IInterface*>* expectedElements,
    /* [in] */ Int32 from,
    /* [in] */ Int32 to,
    /* [in] */ CopyOnWriteArrayList* host)
{
    mSlice = new Slice(expectedElements, from, to);
    mHost = host;
}

ECode CopyOnWriteArrayList::CowSubList::Add(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    AutoLock lock(this);
    FAIL_RETURN(mSlice->CheckPositionIndex(location));
    FAIL_RETURN(mSlice->CheckConcurrentModification(mHost->mElements));
    mHost->Add(location + mSlice->mFrom, object);
    mSlice = new Slice(mHost->mElements, mSlice->mFrom, mSlice->mTo + 1);
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoLock lock(this);
    FAIL_RETURN(mSlice->CheckPositionIndex(location));
    FAIL_RETURN(mSlice->CheckConcurrentModification(mHost->mElements));
    Int32 oldSize = mHost->mElements->GetLength();
    Boolean isflag = FALSE;
    mHost->AddAll(location + mSlice->mFrom, collection, &isflag);
    mSlice = new Slice(mHost->mElements, mSlice->mFrom, mSlice->mTo + (mHost->mElements->GetLength() - oldSize));
    *result = isflag;
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoLock lock(this);
    Int32 length = 0;
    GetSize(&length);
    return AddAll(length, collection, result);
}

ECode CopyOnWriteArrayList::CowSubList::Get(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    AutoPtr<Slice> slice = mSlice;
    AutoPtr< ArrayOf<IInterface*> > snapshot = mHost->mElements;
    FAIL_RETURN(mSlice->CheckElementIndex(location));
    FAIL_RETURN(mSlice->CheckConcurrentModification(snapshot));
    *object = (*snapshot)[location + slice->mFrom];
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)

    AutoPtr<IListIterator> outlist;
    FAIL_RETURN(GetListIterator(0, (IListIterator**)&outlist));
    *it = IIterator::Probe(outlist);
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::GetListIterator(
    /* [in] */ Int32 location,
    /* [out] */ IListIterator** listiterator)
{
    VALIDATE_NOT_NULL(listiterator)

    AutoPtr<Slice> slice = mSlice;
    AutoPtr< ArrayOf<IInterface*> > snapshot = mHost->mElements;
    FAIL_RETURN(slice->CheckPositionIndex(location));
    FAIL_RETURN(slice->CheckConcurrentModification(snapshot));
    AutoPtr<CowIterator> result = new CowIterator(snapshot, slice->mFrom, slice->mTo);
    result->mIndex = slice->mFrom + location;
    *listiterator = (IListIterator*) result->Probe(EIID_IListIterator);
    REFCOUNT_ADD(*listiterator)
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::Remove(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    AutoLock lock(this);
    FAIL_RETURN(mSlice->CheckElementIndex(location));
    FAIL_RETURN(mSlice->CheckConcurrentModification(mHost->mElements));
    AutoPtr<IInterface> removed;
    mHost->Remove(mSlice->mFrom + location, (IInterface**)&removed);
    mSlice = new Slice(mHost->mElements, mSlice->mFrom, mSlice->mTo - 1);
    *object = removed;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* inobject,
    /* [out] */ IInterface** outobject)
{
    VALIDATE_NOT_NULL(outobject)

    AutoLock lock(this);
    FAIL_RETURN(mSlice->CheckElementIndex(location));
    FAIL_RETURN(mSlice->CheckConcurrentModification(mHost->mElements));
    AutoPtr<IInterface> result;
    mHost->Set(location + mSlice->mFrom, inobject, (IInterface**)&result);
    mSlice = new Slice(mHost->mElements, mSlice->mFrom, mSlice->mTo);
    *outobject = result;
    REFCOUNT_ADD(*outobject)
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)

    AutoPtr<Slice> slice = mSlice;
    *size = slice->mTo - slice->mFrom;
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::RemoveRange(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    return AbstractList::RemoveRange(start, end);
}

ECode CopyOnWriteArrayList::CowSubList::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 value = 0;
    IndexOf(object, &value);
    *result = value != -1;
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<Slice> slice = mSlice;
    AutoPtr< ArrayOf<IInterface*> > snapshot = mHost->mElements;
    FAIL_RETURN(slice->CheckConcurrentModification(snapshot));
    return CopyOnWriteArrayList::ContainsAll(collection, snapshot, slice->mFrom, slice->mTo, result);
}

ECode CopyOnWriteArrayList::CowSubList::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<Slice> slice = mSlice;
    *result = slice->mFrom == slice->mTo;
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoLock lock(this);
    Int32 index = 0;
    IndexOf(object, &index);
    if (index == -1) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IInterface> outface;
    Remove(index, (IInterface**)&outface);
    *result = TRUE;
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoLock lock(this);
    FAIL_RETURN(mSlice->CheckConcurrentModification(mHost->mElements));
    Int32 removed = mHost->RemoveOrRetain(collection, FALSE, mSlice->mFrom, mSlice->mTo);
    mSlice = new Slice(mHost->mElements, mSlice->mFrom, mSlice->mTo - removed);
    *result = removed != 0;
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoLock lock(this);
    FAIL_RETURN(mSlice->CheckConcurrentModification(mHost->mElements));
    Int32 removed = mHost->RemoveOrRetain(collection, TRUE, mSlice->mFrom, mSlice->mTo);
    mSlice = new Slice(mHost->mElements, mSlice->mFrom, mSlice->mTo - removed);
    *result = removed != 0;
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractList::ToArray(array);
}

ECode CopyOnWriteArrayList::CowSubList::ToArray(
    /* [in] */ ArrayOf<IInterface*>* contents,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractList::ToArray(contents, outArray);
}

ECode CopyOnWriteArrayList::CowSubList::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    assert(0);
//    return Arrays::_ToStringObject(mHost->mElements, result);
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    AutoLock lock(this);
    FAIL_RETURN(Add(mSlice->mTo - mSlice->mFrom, object));
    *modified = TRUE;
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::Clear()
{
    AutoLock lock(this);
    FAIL_RETURN(mSlice->CheckConcurrentModification(mHost->mElements));
    mHost->RemoveRange(mSlice->mFrom, mSlice->mTo);
    mSlice = new Slice(mHost->mElements, mSlice->mFrom, mSlice->mFrom);
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractList::Equals(object, result);
}

ECode CopyOnWriteArrayList::CowSubList::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractList::GetHashCode(hashCode);
}

ECode CopyOnWriteArrayList::CowSubList::IndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    AutoPtr<Slice> slice = mSlice;
    AutoPtr< ArrayOf<IInterface*> > snapshot = mHost->mElements;
    FAIL_RETURN(mSlice->CheckConcurrentModification(snapshot));
    Int32 result = 0;
    CopyOnWriteArrayList::IndexOf(object, snapshot, mSlice->mFrom, mSlice->mTo, &result);
    *index = (result != -1) ? (result - mSlice->mFrom) : -1;
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    AutoPtr<Slice> slice = mSlice;
    AutoPtr< ArrayOf<IInterface*> > snapshot = mHost->mElements;
    FAIL_RETURN(mSlice->CheckConcurrentModification(snapshot));
    Int32 result = 0;
    CopyOnWriteArrayList::LastIndexOf(object, snapshot, mSlice->mFrom, mSlice->mTo, &result);
    *index = (result != -1) ? (result - mSlice->mFrom) : -1;
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowSubList::GetListIterator(
    /* [out] */ IListIterator** it)
{
    return GetListIterator(0, it);
}

ECode CopyOnWriteArrayList::CowSubList::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList)

    AutoPtr<Slice> slice = mSlice;
    Int32 length = 0;
    GetSize(&length);
    if (start < 0 || start > end || end > length) {
        *subList = NULL;
        // throw new IndexOutOfBoundsException("from=" + from + ", to=" + to +
        //         ", list size=" + size());
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    AutoPtr<IList> res = (IList*) new CowSubList(mSlice->mExpectedElements, mSlice->mFrom + start,
                                                mSlice->mFrom + end, mHost);
    *subList = res;
    REFCOUNT_ADD(*subList)
    return NOERROR;
}


//==========================================================
//       CopyOnWriteArrayList::CowIterator
//==========================================================
CAR_INTERFACE_IMPL_2(CopyOnWriteArrayList::CowIterator, Object, IListIterator, IIterator)

CopyOnWriteArrayList::CowIterator::CowIterator(
    /* [in] */ ArrayOf<IInterface*>* snapshot,
    /* [in] */ Int32 from,
    /* [in] */ Int32 to)
{
    mSnapshot = snapshot;
    mFrom = from;
    mTo = to;
    mIndex = from;
}

ECode CopyOnWriteArrayList::CowIterator::Add(
    /* [in] */ IInterface* object)
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CopyOnWriteArrayList::CowIterator::HasPrevious(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIndex > mFrom;
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowIterator::GetNextIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    *index = mIndex;
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowIterator::GetPrevious(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    if (mIndex > mFrom) {
        AutoPtr<IInterface> res = (*mSnapshot)[--mIndex];
        *object = res;
        REFCOUNT_ADD(*object)
        return NOERROR;
    }
    else {
        *object = NULL;
        // throw new NoSuchElementException();
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowIterator::GetPreviousIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    *index = mIndex - 1;
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowIterator::Set(
    /* [in] */ IInterface* object)
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CopyOnWriteArrayList::CowIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIndex < mTo;
    return NOERROR;
}

ECode CopyOnWriteArrayList::CowIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    if (mIndex < mTo) {
        AutoPtr<IInterface> res = (*mSnapshot)[mIndex++];
        *object = res;
        REFCOUNT_ADD(*object)
        return NOERROR;
    }
    else {
        *object = NULL;
        // throw new NoSuchElementException();
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
}

ECode CopyOnWriteArrayList::CowIterator::Remove()
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
