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

#include "CCollectionUtils.h"
#include "CCollectionUtils.h"
#include "Collections.h"

using Elastos::Utility::Collections;
using Elastos::Utility::EIID_IIterator;
using Elastos::Utility::EIID_IIterable;

namespace Libcore {
namespace Utility {

//===============================================================================
// CCollectionUtils::_Iterator
//===============================================================================

CCollectionUtils::_Iterator::_Iterator(
    /* [in] */ IIterable* iterable,
    /* [in] */ Boolean trim)
    : mRemoveIsOkay(FALSE)
    , mIter(iterable)
    , mTrim(trim)
{
    mIter->GetIterator((IIterator**)&mDelegate);
}

CAR_INTERFACE_IMPL(CCollectionUtils::_Iterator, Object, IIterator)
CAR_INTERFACE_IMPL(CCollectionUtils::_Iterable, Object, IIterable)

ECode CCollectionUtils::_Iterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    ComputeNext();
    *result = mNext != NULL;
    return NOERROR;
}

ECode CCollectionUtils::_Iterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    Boolean isflag = FALSE;
    if (HasNext(&isflag), !isflag) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IInterface> result = mNext;
    mRemoveIsOkay = TRUE;
    mNext = NULL;
    *object = result;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CCollectionUtils::_Iterator::Remove()
{
    if (!mRemoveIsOkay) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return mDelegate->Remove();
}

ECode CCollectionUtils::_Iterator::ComputeNext()
{
    mRemoveIsOkay = FALSE;
    Boolean isflag = FALSE;
    while (mNext == NULL && (mDelegate->HasNext(&isflag), isflag)) {
        mDelegate->GetNext((IInterface**)&mNext);
        if (mTrim && mNext == NULL) {
            mDelegate->Remove();
        }
    }
    return NOERROR;
}

//===============================================================================
// CCollectionUtils::_Iterable
//===============================================================================

CCollectionUtils::_Iterable::_Iterable(
    /* [in] */ IIterable* iterable,
    /* [in] */ Boolean trim)
    : mIter(iterable)
    , mTrim(trim)
{
}

ECode CCollectionUtils::_Iterable::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)

    AutoPtr<_Iterator> res = new _Iterator(mIter, mTrim);
    AutoPtr<IIterator> iit = IIterator::Probe(res);
    *it = res;
    REFCOUNT_ADD(*it)
    return NOERROR;
}

//===============================================================================
// CCollectionUtils
//===============================================================================

CAR_SINGLETON_IMPL(CCollectionUtils)

CAR_INTERFACE_IMPL(CCollectionUtils, Singleton, ICollectionUtils)

ECode CCollectionUtils::DereferenceIterable(
    /* [in] */ IIterable* iterable,
    /* [in] */ Boolean trim,
    /* [out] */ IIterable** outiter)
{
    return _DereferenceIterable(iterable, trim, outiter);
}

ECode CCollectionUtils::RemoveDuplicates(
    /* [in] */ IList* list,
    /* [in] */ IComparator* comparator)
{
    return _RemoveDuplicates(list, comparator);
}

ECode CCollectionUtils::_DereferenceIterable(
    /* [in] */ IIterable* iterable,
    /* [in] */ Boolean trim,
    /* [out] */ IIterable** outiter)
{
    VALIDATE_NOT_NULL(outiter)

    AutoPtr<IIterable> res = (IIterable*) new _Iterable(iterable, trim);
    *outiter = res;
    REFCOUNT_ADD(*outiter)
    return NOERROR;
}

ECode CCollectionUtils::_RemoveDuplicates(
    /* [in] */ IList* list,
    /* [in] */ IComparator* comparator)
{
    FAIL_RETURN(Collections::Sort(list, comparator));
    Int32 j = 1;
    Int32 listsize = 0;
    AutoPtr<ICollection> clt = ICollection::Probe(list);
    if (clt == NULL) {
        return NOERROR;
    }
    clt->GetSize(&listsize);
    for (Int32 i = 1; i < listsize; i++) {
        Int32 comvalue = 0;
        AutoPtr<IInterface> joutface;
        list->Get(j - 1, (IInterface**)&joutface);
        AutoPtr<IInterface> ioutface;
        list->Get(i, (IInterface**)&ioutface);
        if (comparator->Compare(joutface, ioutface, &comvalue), comvalue != 0) {
            AutoPtr<IInterface> object;
            list->Get(i, (IInterface**)&object);
            list->Set(j++, object, (IInterface**)&ioutface);
        }
    }
    clt->GetSize(&listsize);
    if (j < listsize) {
        AutoPtr<IList> outlist;
        list->GetSubList(j, listsize, (IList**)&outlist);
        AutoPtr<ICollection> cltOut = ICollection::Probe(outlist);
        if (cltOut == NULL) {
            return NOERROR;
        }
        cltOut->Clear();
    }
    return NOERROR;
}

} // namespace Utility
} // namespace Libcore

