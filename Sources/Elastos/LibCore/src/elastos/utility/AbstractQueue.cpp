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

#include "AbstractQueue.h"

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(AbstractQueue, AbstractCollection, IQueue)

AbstractQueue::AbstractQueue()
{}

ECode AbstractQueue::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Boolean r;
    if (FAILED(Offer(obj, &r))) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *result = TRUE;
    return NOERROR;
}

ECode AbstractQueue::Remove(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;

    AutoPtr<IInterface> x;
    Poll((IInterface**)&x);
    if (x == NULL) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }

    *obj = x;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode AbstractQueue::GetElement(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;

    AutoPtr<IInterface> x;
    Peek((IInterface**)&x);
    if (x == NULL) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }

    *obj = x;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode AbstractQueue::Clear()
{
    AutoPtr<IInterface> x;
    while (Poll((IInterface**)&x), x != NULL) {
        x = NULL;
    }
    return NOERROR;
}

ECode AbstractQueue::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    VALIDATE_NOT_NULL(c);

    if (c == this->Probe(EIID_ICollection)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean modified = FALSE;
    AutoPtr<IIterator> it;
    (IIterable::Probe(c))->GetIterator((IIterator**)&it);
    Boolean hasNext, added;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> e;
        it->GetNext((IInterface**)&e);
        if (Add(e, &added), added) {
            modified = TRUE;
        }
    }

    *result = modified;
    return NOERROR;
}

ECode AbstractQueue::GetIterator(
    /* [out] */ IIterator** it)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode AbstractQueue::Add(
    /* [in] */  IInterface* object)
{
    return AbstractCollection::Add(object);
}

ECode AbstractQueue::AddAll(
    /* [in] */  ICollection* collection)
{
    return AbstractCollection::AddAll(collection);
}

ECode AbstractQueue::Contains(
    /* [in] */  IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::Contains(object, result);
}

ECode AbstractQueue::ContainsAll(
    /* [in] */  ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::ContainsAll(collection, result);
}

ECode AbstractQueue::Equals(
    /* [in] */  IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::Equals(object, result);
}

ECode AbstractQueue::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractCollection::GetHashCode(hashCode);
}

ECode AbstractQueue::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractCollection::IsEmpty(result);
}

ECode AbstractQueue::Remove(
    /* [in] */  IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::Remove(object, modified);
}

ECode AbstractQueue::RemoveAll(
    /* [in] */  ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::RemoveAll(collection, modified);
}

ECode AbstractQueue::RemoveAll(
    /* [in] */  ICollection* collection)
{
    return AbstractCollection::RemoveAll(collection);
}

ECode AbstractQueue::RetainAll(
    /* [in] */  ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::RetainAll(collection, modified);
}

ECode AbstractQueue::RetainAll(
    /* [in] */  ICollection* collection)
{
    return AbstractCollection::RetainAll(collection);
}

ECode AbstractQueue::GetSize(
    /* [out] */ Int32* size)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode AbstractQueue::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractCollection::ToArray(array);
}

ECode AbstractQueue::ToArray(
    /* [in] */  ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractCollection::ToArray(inArray, outArray);
}


} // Utility
} // Elastos
