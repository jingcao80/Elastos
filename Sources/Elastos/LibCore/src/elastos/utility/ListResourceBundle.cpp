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

#include "ListResourceBundle.h"
#include "CHashMap.h"
#include "CString.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;

namespace Elastos{
namespace Utility{

/////////////////////////////////////////////////////
//      ListResourceBundle::_FirstEnumeration
/////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(ListResourceBundle::_FirstEnumeration, Object, IEnumeration)

ListResourceBundle::_FirstEnumeration::_FirstEnumeration(
    /* [in] */ ListResourceBundle* host)
    : mHost(host)
    , mNextElement(String())
{
    AutoPtr<ISet> set;
    mHost->mTable->GetKeySet((ISet**)&set);
    set->GetIterator((IIterator**)&mLocal);
    (mHost->mParent)->GetKeys((IEnumeration**)&mEnum);
}

ECode ListResourceBundle::_FirstEnumeration::HasMoreElements(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Boolean hasNext;
    if (mLocal->HasNext(&hasNext), hasNext) {
        *res = TRUE;
        return NOERROR;
    }
    *res = FindNext();
    return NOERROR;
}

ECode ListResourceBundle::_FirstEnumeration::GetNextElement(
    /* [out] */ IInterface** res)
{
    VALIDATE_NOT_NULL(res)
    Boolean hasNext;
    if (mLocal->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        mLocal->GetNext((IInterface**)&next);
        *res = next;
        REFCOUNT_ADD(*res)
        return NOERROR;
    }
    if (FindNext()) {
        AutoPtr<ICharSequence> seq;
        CString::New(mNextElement, (ICharSequence**)&seq);
        mNextElement = NULL;
        *res = (IInterface*)seq->Probe(EIID_IInterface);
        REFCOUNT_ADD(*res)
        return NOERROR;
    }

    mEnum->GetNextElement(res);
    return NOERROR;
}

Boolean ListResourceBundle::_FirstEnumeration::FindNext()
{
    if (mNextElement.IsNull()) {
        return TRUE;
    }
    Boolean hasMoreElements;
    while (mEnum->HasMoreElements(&hasMoreElements), hasMoreElements) {
        AutoPtr<IInterface> next;
        mEnum->GetNextElement((IInterface**)&next);
        Boolean containsKey;
        if (!(mHost->mTable->ContainsKey(next, &containsKey), &containsKey)) {
            AutoPtr<ICharSequence> seq = ICharSequence::Probe(next);
            assert(seq);
            seq->ToString(&mNextElement);
            return TRUE;
        }
    }
    return FALSE;
}

/////////////////////////////////////////////////////
//      ListResourceBundle::_SecondEnumeration
/////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(ListResourceBundle::_SecondEnumeration, Object, IEnumeration)

ListResourceBundle::_SecondEnumeration::_SecondEnumeration(
    /* [in] */ ListResourceBundle* host)
    : mHost(host)
{
    AutoPtr<ISet> set;
    mHost->mTable->GetKeySet((ISet**)&set);
    set->GetIterator((IIterator**)&mIt);
}

ECode ListResourceBundle::_SecondEnumeration::HasMoreElements(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    mIt->HasNext(res);
    return NOERROR;;
}

ECode ListResourceBundle::_SecondEnumeration::GetNextElement(
    /* [out] */ IInterface** res)
{
    mIt->GetNext(res);
    return NOERROR;
}

/////////////////////////////////////////////////////
//              ListResourceBundle
/////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(ListResourceBundle, ResourceBundle, IListResourceBundle)

ListResourceBundle::ListResourceBundle()
{
    CHashMap::New((IHashMap**)&mTable);
}

ECode ListResourceBundle::GetKeys(
    /* [out] */ IEnumeration** enu)
{
    VALIDATE_NOT_NULL(enu)
    InitializeTable();
    AutoPtr<IEnumeration> result;
    if (mParent) {
        result = new _FirstEnumeration(this);
        *enu = result;
        REFCOUNT_ADD(*enu)
        return NOERROR;
    } else {
        result = new _SecondEnumeration(this);
        *enu = result;
        REFCOUNT_ADD(*enu)
        return NOERROR;
    }
}

ECode ListResourceBundle::HandleGetObject(
    /* [in] */ const String& key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)
    InitializeTable();
    if (key.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ICharSequence> seq;
    CString::New(key, (ICharSequence**)&seq);
    (IMap::Probe(mTable))->Get(seq, outface);
    return NOERROR;
}

ECode ListResourceBundle::HandleKeySet(
    /* [out] */ ISet** outset)
{
    VALIDATE_NOT_NULL(outset)
    InitializeTable();
    return (IMap::Probe(mTable))->GetKeySet(outset);
}

ECode ListResourceBundle::InitializeTable()
{
    if (!mTable) {
        AutoPtr< ArrayOf< AutoPtr< ArrayOf<IInterface*> > > > contents = GetContents();
        CHashMap::New(contents->GetLength() / 3 * 4 + 3, (IHashMap**)&mTable);

        for (Int32 i = 0; i < contents->GetLength(); i++) {
            if ((*contents)[0] == NULL || (*contents)[1] == NULL) {
                return E_NULL_POINTER_EXCEPTION;
            }
            (IMap::Probe(mTable))->Put((*(*contents)[i])[0], (*(*contents)[i])[1]);
        }
    }
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos