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

#include "UnsafeArrayList.h"

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(UnsafeArrayList, AbstractList, IUnsafeArrayList)

UnsafeArrayList::UnsafeArrayList()
    : mElementType(EIID_IInterface)
    , mArray(NULL)
    , mSize(0)
{}

UnsafeArrayList::UnsafeArrayList(
    /* [in] */ const InterfaceID& elementType,
    /* [in] */ Int32 initialCapacity)
    : mElementType(elementType)
    , mArray(ArrayOf<IInterface*>::Alloc(initialCapacity))
    , mSize(0)
{
}

ECode UnsafeArrayList::constructor()
{
    AbstractList::constructor();
    mElementType = EIID_IInterface;
    mArray = NULL;
    mSize = 0;
    return NOERROR;
}

ECode UnsafeArrayList::constructor(
    /* [in] */ const InterfaceID& elementType,
    /* [in] */ Int32 initialCapacity)
{
    AbstractList::constructor();
    mElementType = elementType;
    mArray = ArrayOf<IInterface*>::Alloc(initialCapacity);
    mSize = 0;
    return NOERROR;
}

ECode UnsafeArrayList::Add(
    /* [in] */ IInterface* element,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (mSize == mArray->GetLength()) {
        AutoPtr< ArrayOf <IInterface*> > newArray = ArrayOf <IInterface*>::Alloc(mSize * 2);
        for(Int32 i = 0; i < mArray->GetLength(); i++) {
            newArray->Set(i, (*mArray)[i]);
        }
        mArray = newArray;
    }
    (*mArray)[mSize++] = element;
    ++mModCount;
    *res = TRUE;
    return NOERROR;
}

ECode UnsafeArrayList::GetArrays(
    /* [out, callee] */ ArrayOf<IInterface*> ** array)
{
    VALIDATE_NOT_NULL(array)

    *array = mArray;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode UnsafeArrayList::Get(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** element)
{
    VALIDATE_NOT_NULL(element)

    *element = (*mArray)[position];
    REFCOUNT_ADD(*element)
    return NOERROR;
}

ECode UnsafeArrayList::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)

    *size = mSize;
    return NOERROR;
}

ECode UnsafeArrayList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** prevObject)
{
    VALIDATE_NOT_NULL(prevObject)

    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode UnsafeArrayList::Add(
    /* [in] */ Int32 index,
    /* [in] */ IInterface* object)
{
    return AbstractList::Add(index, object);
}

ECode UnsafeArrayList::Add(
    /* [in] */ IInterface* object)
{
    return AbstractList::Add(object);
}

ECode UnsafeArrayList::AddAll(
    /* [in] */ Int32 index,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractList::AddAll(index, collection, result);
}

ECode UnsafeArrayList::AddAll(
    /* [in] */ Int32 index,
    /* [in] */ ICollection* collection)
{
    return AbstractList::AddAll(index, collection);
}

ECode UnsafeArrayList::IndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    return AbstractList::IndexOf(object, index);
}

ECode UnsafeArrayList::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    return AbstractList::LastIndexOf(object, index);
}

ECode UnsafeArrayList::GetListIterator(
    /* [in] */ IListIterator** it)
{
    return AbstractList::GetListIterator(it);
}

ECode UnsafeArrayList::GetListIterator(
    /* [in] */ Int32 index,
    /* [in] */ IListIterator** it)
{
    return AbstractList::GetListIterator(index, it);
}

ECode UnsafeArrayList::Remove(
    /* [in] */ Int32 index,
    /* [in] */ IInterface** object)
{
    return AbstractList::Remove(index, object);
}

ECode UnsafeArrayList::Remove(
    /* [in] */ Int32 index)
{
    return AbstractList::Remove(index);
}

ECode UnsafeArrayList::Set(
    /* [in] */ Int32 index,
    /* [in] */ IInterface* object)
{
    return AbstractList::Set(index, object);
}

ECode UnsafeArrayList::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ IList** list)
{
    return AbstractList::GetSubList(start, end, list);
}

ECode UnsafeArrayList::GetIterator(
    /* [in] */ IIterator** it)
{
    return AbstractList::GetIterator(it);
}

ECode UnsafeArrayList::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractList::AddAll(collection, result);
}

ECode UnsafeArrayList::AddAll(
    /* [in] */ ICollection* collection)
{
    return AbstractList::AddAll(collection);
}

ECode UnsafeArrayList::Clear()
{
    return AbstractList::Clear();
}

ECode UnsafeArrayList::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractList::Contains(object, result);
}

ECode UnsafeArrayList::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractList::ContainsAll(collection, result);
}

ECode UnsafeArrayList::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractList::Equals(object, result);
}

ECode UnsafeArrayList::GetHashCode(
    /* [out] */ Int32* index)
{
    return AbstractList::GetHashCode(index);
}

ECode UnsafeArrayList::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractList::IsEmpty(result);
}

ECode UnsafeArrayList::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractList::Remove(object, result);
}

ECode UnsafeArrayList::Remove(
    /* [in] */ IInterface* object)
{
    return AbstractList::Remove(object);
}

ECode UnsafeArrayList::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractList::RemoveAll(collection, result);
}

ECode UnsafeArrayList::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return AbstractList::RemoveAll(collection);
}

ECode UnsafeArrayList::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractList::RetainAll(collection, result);
}

ECode UnsafeArrayList::RetainAll(
    /* [in] */ ICollection* collection)
{
    return AbstractList::RetainAll(collection);
}

ECode UnsafeArrayList::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractList::ToArray(array);
}

ECode UnsafeArrayList::ToArray(
    /* [in] */ Elastos::ArrayOf<IInterface*>* array,
    /* [out, callee] */ ArrayOf<IInterface*>** outarray)
{
    return AbstractList::ToArray(array, outarray);
}

} // namespace Utility
} // namespace Elastos