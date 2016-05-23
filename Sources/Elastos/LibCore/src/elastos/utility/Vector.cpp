
#include "Elastos.CoreLibrary.IO.h"
#include "Vector.h"
#include "Arrays.h"
#include "Collections.h"
#include "StringBuilder.h"
#include "AutoLock.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {

//====================================================================
// Vector::Enumeration
//====================================================================
CAR_INTERFACE_IMPL(Vector::Enumeration, Object, IEnumeration)

ECode Vector::Enumeration::HasMoreElements(
    /* [out] */ Boolean* result)
{
    *result = mPos < mOwner->mElementCount;
    return NOERROR;
}

ECode Vector::Enumeration::GetNextElement(
    /* [out] */ IInterface** out)
{
    {    AutoLock syncLock(mOwner);
        if (mPos < mOwner->mElementCount) {
            AutoPtr< ArrayOf<IInterface*> > a = mOwner->mElementData;
            *out = (*a)[mPos++];
            REFCOUNT_ADD(*out);
            return NOERROR;
        }
    }

    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

//====================================================================
// Vector::
//====================================================================

CAR_INTERFACE_IMPL_4(Vector, AbstractList, IVector, IRandomAccess, ICloneable, ISerializable)

const Int32 Vector::DEFAULT_SIZE = 10;

ECode Vector::constructor()
{
    return constructor(DEFAULT_SIZE, 0);
}

ECode Vector::constructor(
    /* [in] */ Int32 capacity)
{
    return constructor(capacity, 0);;
}

ECode Vector::constructor(
    /* [in] */ Int32 capacity,
    /* [in] */ Int32 capacityIncrement)
{
    if (capacity < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mElementData = NewElementArray(capacity);
    mElementCount = 0;
    mCapacityIncrement = capacityIncrement;
    return NOERROR;
}

ECode Vector::constructor(
    /* [in] */ ICollection* collection)
{
    VALIDATE_NOT_NULL(collection);
    Int32 size;
    collection->GetSize(&size);
    constructor(size, 0);
    AutoPtr<IIterator> it;
    (IIterable::Probe(collection))->GetIterator((IIterator**)&it);
    Boolean result;
    while ((it->HasNext(&result),result)) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        mElementData->Set(mElementCount++, next);
    }
    return NOERROR;
}

AutoPtr<ArrayOf<IInterface*> > Vector::NewElementArray(
    /* [in] */ Int32 size)
{
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(size);
    return array;
}

ECode Vector::Add(
    /* [in] */ IInterface* object)
{
    return AbstractList::Add(object);
}

ECode Vector::Add(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    return InsertElementAt(object, location);
}

ECode Vector::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)
    {    AutoLock syncLock(this);
        if (mElementCount == mElementData->GetLength()) {
            GrowByOne();
        }
        mElementData->Set(mElementCount++, object);
        mModCount++;
        *modified = TRUE;
    }
    return NOERROR;
}

ECode Vector::AddAll(
        /* [in] */ ICollection* collection)
{
    return AbstractList::AddAll(collection);
}

ECode Vector::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection)
{
    return AbstractList::AddAll(location, collection);
}

ECode Vector::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    *modified = FALSE;
    VALIDATE_NOT_NULL(collection);
    {    AutoLock syncLock(this);
        if (location >= 0 && location <= mElementCount) {
            Int32 size;
            collection->GetSize(&size);
            if (size == 0) {
                *modified = FALSE;
                return NOERROR;
            }
            Int32 required = size - (mElementData->GetLength() - mElementCount);
            if (required > 0) {
                GrowBy(required);
            }
            Int32 count = mElementCount - location;
            if (count > 0) {
                mElementData->Copy(location + size, mElementData, location, count);
            }
            AutoPtr<IIterator> it;
            (IIterable::Probe(collection))->GetIterator((IIterator**)&it);
            Boolean result = FALSE;
            while ((it->HasNext(&result),result)) {
                AutoPtr<IInterface> next;
                it->GetNext((IInterface**)&next);
                mElementData->Set(location++, next);
            }
            mElementCount += size;
            mModCount++;
            *modified = TRUE;
            return NOERROR;
        }
    }
    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode Vector::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AddAll(mElementCount, collection, modified);
}

ECode Vector::AddElement(
    /* [in] */ IInterface* object)
{
    {    AutoLock syncLock(this);
        if (mElementCount == mElementData->GetLength()) {
            GrowByOne();
        }
        mElementData->Set(mElementCount++, object);
        mModCount++;
    }
    return NOERROR;
}

ECode Vector::GetCapacity(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    {    AutoLock syncLock(this);
        *value = mElementData->GetLength();
    }
    return NOERROR;
}

ECode Vector::Clear()
{
    return RemoveAllElements();
}

ECode Vector::Clone(
    /* [out] */ IInterface** object)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Vector::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 val;
    FAIL_RETURN(IndexOf(object, 0, &val));
    *result = (val != -1);
    return NOERROR;
}

ECode Vector::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    {    AutoLock syncLock(this);
        AbstractList::ContainsAll(collection, result);
    }
    return NOERROR;
}

ECode Vector::CopyInto(
    /* [in] */ ArrayOf<IInterface*>* elements)
{
    VALIDATE_NOT_NULL(elements);
    {    AutoLock syncLock(this);
        elements->Copy(mElementData, mElementCount);
    }
    return NOERROR;
}

ECode Vector::ElementAt(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)
    {    AutoLock syncLock(this);
        if (location >= 0 && location < mElementCount) {
            *outface = (*mElementData)[location];
            REFCOUNT_ADD(*outface);
            return NOERROR;
        }
    }
    return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode Vector::GetElements(
    /* [out] */ IEnumeration** enu)
{
    VALIDATE_NOT_NULL(enu)
    *enu = new Enumeration(this);
    REFCOUNT_ADD(*enu);
    return NOERROR;
}

ECode Vector::EnsureCapacity(
    /* [in] */ Int32 minimumCapacity)
{
    {    AutoLock syncLock(this);
        if (mElementData->GetLength() < minimumCapacity) {
            Int32 next = (mCapacityIncrement <= 0 ? mElementData->GetLength()
                    : mCapacityIncrement)
                    + mElementData->GetLength();
            Grow(minimumCapacity > next ? minimumCapacity : next);
        }
    }
    return NOERROR;
}

ECode Vector::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    {    AutoLock syncLock(this);
        if (TO_IINTERFACE(this) == object) {
            *result = TRUE;
            return NOERROR;
        }

        if (IList::Probe(object) != NULL) {
            AutoPtr<IList> list = IList::Probe(object);
            Int32 size;
            (ICollection::Probe(list))->GetSize(&size);
            if (size != mElementCount) {
                *result = FALSE;
                return NOERROR;
            }

            Int32 index = 0;
            Boolean hasNext = FALSE;
            AutoPtr<IIterator> it;
            GetIterator((IIterator**)&it);
            while ((it->HasNext(&hasNext), hasNext)) {
                AutoPtr<IInterface> e1 = (*mElementData)[index++];
                AutoPtr<IInterface> e2;
                it->GetNext((IInterface**)&e2);
                if (!(e1 == NULL ? e2 == NULL : Object::Equals(e1, e2))) {
                    *result = FALSE;
                    return NOERROR;
                }
            }
            *result = TRUE;
        }
    }
    return NOERROR;
}

ECode Vector::FirstElement(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)
    {    AutoLock syncLock(this);
        if (mElementCount > 0) {
            *outface = (*mElementData)[0];
            REFCOUNT_ADD(*outface);
            return NOERROR;
        }
        *outface = NULL;
    }
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode Vector::Get(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    return ElementAt(location, object);
}

ECode Vector::Grow(
    /* [in] */ Int32 newCapacity)
{
    AutoPtr< ArrayOf<IInterface*> > newData = NewElementArray(newCapacity);
    // Assumes elementCount is <= newCapacity
    assert(mElementCount <= newCapacity);
    newData->Copy(mElementData, mElementCount);
    mElementData = newData;
    return NOERROR;
}

ECode Vector::GrowByOne()
{
    Int32 adding = 0;
    if (mCapacityIncrement <= 0) {
        if ((adding = mElementData->GetLength()) == 0) {
            adding = 1;
        }
    }
    else {
        adding = mCapacityIncrement;
    }

    AutoPtr< ArrayOf<IInterface*> > newData = NewElementArray(mElementData->GetLength() + adding);
    newData->Copy(mElementData, mElementCount);
    mElementData = newData;
    return NOERROR;
}

ECode Vector::GrowBy(
    /* [in] */ Int32 required)
{
    Int32 adding = 0;
    if (mCapacityIncrement <= 0) {
        if ((adding = mElementData->GetLength()) == 0) {
            adding = required;
        }
        while (adding < required) {
            adding += adding;
        }
    }
    else {
        adding = (required / mCapacityIncrement) * mCapacityIncrement;
        if (adding < required) {
            adding += mCapacityIncrement;
        }
    }
    AutoPtr< ArrayOf<IInterface*> > newData = NewElementArray(mElementData->GetLength() + adding);
    newData->Copy(mElementData, mElementCount);
    mElementData = newData;
    return NOERROR;
}

ECode Vector::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    {    AutoLock syncLock(this);
        Int32 result = 1;
        for (Int32 i = 0; i < mElementCount; i++) {
            result = (31 * result)
                    + ((*mElementData)[i] == NULL ? 0 : Object::GetHashCode((*mElementData)[i]));
        }
        *hashCode = result;
    }
    return NOERROR;
}

ECode Vector::IndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    return IndexOf(object, 0, index);
}

ECode Vector::IndexOf(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 location,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    if (location < 0) {
        *value = -1;
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    {    AutoLock syncLock(this);
        if (object != NULL) {
            for (Int32 i = location; i < mElementCount; i++) {
                if (Object::Equals(object, (*mElementData)[i])) {
                    *value = i;
                    return NOERROR;
                }
            }
        }
        else {
            for (Int32 i = location; i < mElementCount; i++) {
                if ((*mElementData)[i] == NULL) {
                    *value = i;
                    return NOERROR;
                }
            }
        }
    }
    *value = -1;
    return NOERROR;
}

ECode Vector::InsertElementAt(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 location)
{
    {    AutoLock syncLock(this);
        if (location >= 0 && location <= mElementCount) {
            if (mElementCount == mElementData->GetLength()) {
                GrowByOne();
            }
            Int32 count = mElementCount - location;
            if (count > 0) {
                mElementData->Copy(location + 1, mElementData, location, count);
            }
            mElementData->Set(location, object);
            mElementCount++;
            mModCount++;
            return NOERROR;
        }
    }
    return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode Vector::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    {    AutoLock syncLock(this);
        *result = (mElementCount == 0);
    }
    return NOERROR;
}

ECode Vector::LastElement(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)
    {    AutoLock syncLock(this);
        if (mElementCount == 0 || mElementData->GetLength() < mElementCount) {
            return E_NO_SUCH_ELEMENT_EXCEPTION;
        }
        *outface = (*mElementData)[mElementCount - 1];
        REFCOUNT_ADD(*outface);
    }

    return NOERROR;
}

ECode Vector::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    return LastIndexOf(object, mElementCount - 1, index);
}

ECode Vector::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 location,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    {    AutoLock syncLock(this);
        if (location >= 0 && location < mElementCount) {
            if (object != NULL) {
                for (Int32 i = location; i >= 0; i--) {
                    if (Object::Equals(object, (*mElementData)[i])) {
                        *value = i;
                        return NOERROR;
                    }
                }
            }
            else {
                for (Int32 i = location; i >= 0; i--) {
                    if ((*mElementData)[i] == NULL) {
                        *value = i;
                        return NOERROR;
                    }
                }
            }
            *value = -1;
            return NOERROR;
        }
    }
    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode Vector::Remove(
        /* [in] */ Int32 location)
{
    return AbstractList::Remove(location);;
}

ECode Vector::Remove(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    {    AutoLock syncLock(this);
        if (location >= 0 && location < mElementCount) {
            AutoPtr<IInterface> result = (*mElementData)[location];
            mElementCount--;
            Int32 size = mElementCount - location;
            if (size > 0) {
                mElementData->Copy(location, mElementData, location + 1, size);
            }
            mElementData->Set(mElementCount, NULL);
            mModCount++;
            *object = result;
            REFCOUNT_ADD(*object);
            return NOERROR;
        }
    }
    return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode Vector::Remove(
    /* [in] */ IInterface* object)
{
    Boolean result;
    return Remove(object, &result);
}

ECode Vector::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return RemoveElement(object, modified);
}

ECode Vector::RemoveAll(
        /* [in] */ ICollection* collection)
{
    return AbstractList::RemoveAll(collection);
}

ECode Vector::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)
    {    AutoLock syncLock(this);
        AbstractList::RemoveAll(collection, modified);
    }
    return NOERROR;
}

ECode Vector::RemoveAllElements()
{
    {    AutoLock syncLock(this);
        for (Int32 i = 0; i < mElementCount; i++) {
            mElementData->Set(i, NULL);
        }
        mModCount++;
        mElementCount = 0;
    }
    return NOERROR;
}

ECode Vector::RemoveElement(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    {    AutoLock syncLock(this);
        Int32 index;
        IndexOf(object, 0, &index);
        if (index == -1) {
            *value = FALSE;
            return NOERROR;
        }
        RemoveElementAt(index);
        *value = TRUE;
    }
    return NOERROR;
}


ECode Vector::RemoveElement(
    /* [in] */ IInterface* object)
{
    Boolean bval;
    return RemoveElement(object, &bval);
}

ECode Vector::RemoveElementAt(
    /* [in] */ Int32 location)
{
    {    AutoLock syncLock(this);
        if (location >= 0 && location < mElementCount) {
            mElementCount--;
            Int32 size = mElementCount - location;
            if (size > 0) {
                mElementData->Copy(location, mElementData, location + 1, size);
            }
            mElementData->Set(mElementCount, NULL);
            mModCount++;
            return NOERROR;
        }
    }
    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode Vector::RemoveRange(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (start >= 0 && start <= end && end <= mElementCount) {
        if (start == end) {
            return NOERROR;
        }
        if (end != mElementCount) {
            mElementData->Copy(start, mElementData, end, mElementCount - end);
            Int32 newCount = mElementCount - (end - start);
            for (Int32 i = newCount; i < mElementCount; ++i) {
                mElementData->Set(i, NULL);
            }
            mElementCount = newCount;
        }
        else {
            for (Int32 i = start; i < mElementCount; ++i) {
                mElementData->Set(i, NULL);
            }
            mElementCount = start;
        }
        mModCount++;
        return NOERROR;
    }
    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode Vector::RetainAll(
        /* [in] */ ICollection* collection)
{
    return AbstractList::RetainAll(collection);
}

ECode Vector::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)
    {    AutoLock syncLock(this);
        AbstractList::RetainAll(collection, modified);
    }
    return NOERROR;
}

ECode Vector::Set(
        /* [in] */ Int32 location,
        /* [in] */ IInterface* object)
{
    return AbstractList::Set(location, object);
}

ECode Vector::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** prevObject)
{
    VALIDATE_NOT_NULL(prevObject)
    {    AutoLock syncLock(this);
        if (location >= 0 && location < mElementCount) {
            AutoPtr<IInterface> result = (*mElementData)[location];
            mElementData->Set(location, object);
            *prevObject = result;
            REFCOUNT_ADD(*prevObject);
            return NOERROR;
        }
    }
    return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode Vector::SetElementAt(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 location)
{
    {    AutoLock syncLock(this);
        if (location >=0 && location < mElementCount) {
            mElementData->Set(location, object);
            return NOERROR;
        }
    }
    return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode Vector::SetSize(
    /* [in] */ Int32 length)
{
    {    AutoLock syncLock(this);
        if (length == mElementCount){
            return NOERROR;
        }
        EnsureCapacity(length);
        if (mElementCount > length) {
            FAIL_RETURN(Arrays::Fill(mElementData, length, mElementCount, (IInterface*)NULL));
        }
        mElementCount = length;
        mModCount++;
    }

    return NOERROR;
}

ECode Vector::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    {    AutoLock syncLock(this);
        *size = mElementCount;
    }
    return NOERROR;
}

ECode Vector::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList)
    {    AutoLock syncLock(this);
        AutoPtr<IList> ssList;
        AbstractList::GetSubList(start, end, (IList**)&ssList);
        *subList = new Collections::SynchronizedRandomAccessList(ssList, this);
        REFCOUNT_ADD(*subList);
    }
    return NOERROR;
}

ECode Vector::GetIterator(
        /* [out] */ IIterator** it)
{
    return AbstractList::GetIterator(it);
}

ECode Vector::GetListIterator(
        /* [out] */ IListIterator** it)
{
    return AbstractList::GetListIterator(it);
}

ECode Vector::GetListIterator(
        /* [in] */ Int32 location,
        /* [out] */ IListIterator** it)
{
    return AbstractList::GetListIterator(location, it);
}

ECode Vector::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array)
    {    AutoLock syncLock(this);
        AutoPtr< ArrayOf<IInterface*> > result = ArrayOf<IInterface*>::Alloc(mElementCount);
        result->Copy(mElementData, mElementCount);
        *array = result;
        REFCOUNT_ADD(*array);
    }
    return NOERROR;
}

ECode Vector::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray)
    {    AutoLock syncLock(this);
        AutoPtr< ArrayOf<IInterface*> > contents = inArray;
        if (mElementCount > contents->GetLength()) {
            contents = ArrayOf<IInterface*>::Alloc(mElementCount);
        }
        contents->Copy(mElementData, mElementCount);
        if (mElementCount < contents->GetLength()) {
            contents->Set(mElementCount, NULL);
        }
        *outArray = contents;
        REFCOUNT_ADD(*outArray);
    }
    return NOERROR;
}

CARAPI Vector::ToString(
    /* [out] */ String* result)
{
    {    AutoLock syncLock(this);
        if (mElementCount == 0) {
            *result = String("[]");
            return NOERROR;
        }
        Int32 length = mElementCount - 1;
        StringBuilder buffer(mElementCount * 16);
        buffer.AppendChar('[');
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<IInterface> ele = (*mElementData)[i];
            if (ele->Probe(EIID_IInterface) == Probe(EIID_IInterface)) {
                buffer.Append(String("(this Collection)"));
            }
            else {
                buffer.Append((*mElementData)[i]);
            }
            buffer.Append(String(", "));
        }
        AutoPtr<IInterface> lastEle = (*mElementData)[length];
        if (lastEle->Probe(EIID_IInterface) == Probe(EIID_IInterface)) {
            buffer.Append(String("(this Collection)"));
        }
        else {
            buffer.Append((*mElementData)[length]);
        }
        buffer.AppendChar(']');
        *result = buffer.ToString();
    }
    return NOERROR;
}

ECode Vector::TrimToSize()
{
    {    AutoLock syncLock(this);
        if (mElementData->GetLength() != mElementCount) {
            Grow(mElementCount);
        }
    }
    return NOERROR;
}

ECode Vector::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    {    AutoLock syncLock(this);
        stream->DefaultWriteObject();
    }

    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
