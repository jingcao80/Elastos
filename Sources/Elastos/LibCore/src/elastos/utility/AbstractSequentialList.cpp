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

#include "AbstractSequentialList.h"

namespace Elastos {
namespace Utility {

ECode AbstractSequentialList::Add(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    AutoPtr<IListIterator> it;
    GetListIterator(location, (IListIterator**)&it);
    return it->Add(object);
}

ECode AbstractSequentialList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(collection);
    VALIDATE_NOT_NULL(result);
    AutoPtr<IListIterator> it;
    GetListIterator(0, (IListIterator**)&it);
    AutoPtr<IIterator> colIt;
    (IIterable::Probe(collection))->GetIterator((IIterator**)&colIt);
    Int32 next;
    it->GetNextIndex(&next);
    Boolean hasnext = FALSE;
    while ((colIt->HasNext(&hasnext), hasnext)) {
        AutoPtr<IInterface> nextobject;
        colIt->GetNext((IInterface**)&nextobject);
        it->Add(nextobject);
    }
    Int32 nextindex;
    it->GetNextIndex(&nextindex);
    *result = next != nextindex;
    return NOERROR;
}

ECode AbstractSequentialList::Get(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    // try {
    AutoPtr<IListIterator> listiterator;
    GetListIterator(location, (IListIterator**)&listiterator);
    ECode ec = (IIterator::Probe(listiterator))->GetNext(object);
    if (FAILED(ec)) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
    // } catch (NoSuchElementException e) {
        // throw new IndexOutOfBoundsException();
    // }
}

ECode AbstractSequentialList::GetIterator(
    /* [out] */ IIterator** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IListIterator> listiterator;
    GetListIterator(0, (IListIterator**)&listiterator);
    *result = IIterator::Probe(listiterator);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AbstractSequentialList::Remove(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    // try {
    AutoPtr<IListIterator> it;
    GetListIterator(location, (IListIterator**)&it);
    ECode ec = (IIterator::Probe(it))->GetNext(object);
    if (FAILED(ec)) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return (IIterator::Probe(it))->Remove();
    // } catch (NoSuchElementException e) {
        // throw new IndexOutOfBoundsException();
    // }
}

ECode AbstractSequentialList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** prevObject)
{
    VALIDATE_NOT_NULL(prevObject);
    AutoPtr<IListIterator> it;
    GetListIterator(location, (IListIterator**)&it);
    Boolean hasnext = FALSE;
    (IIterator::Probe(it))->HasNext(&hasnext);
    if (!hasnext) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    (IIterator::Probe(it))->GetNext(prevObject);
    return it->Set(object);
}

} // Utility
} // Elastos