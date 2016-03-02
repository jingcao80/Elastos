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
    (IMap::Probe(mHost->mTable))->GetKeySet((ISet**)&set);
    (IIterable::Probe(set))->GetIterator((IIterator**)&mLocal);
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
        return NOERROR;
    }
    if (FindNext()) {
        AutoPtr<ICharSequence> seq;
        CString::New(mNextElement, (ICharSequence**)&seq);
        mNextElement = NULL;
        *res = (IInterface*)seq->Probe(EIID_IInterface);
        return NOERROR;
    }
    AutoPtr<IInterface> next;
    mEnum->GetNextElement((IInterface**)&next);
    *res = next;
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
        if (!((IMap::Probe(mHost->mTable))->ContainsKey(next, &containsKey), &containsKey)) {
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
    (IMap::Probe(mHost->mTable))->GetKeySet((ISet**)&set);
    (IIterable::Probe(set))->GetIterator((IIterator**)&mIt);
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
    VALIDATE_NOT_NULL(res)
    AutoPtr<IInterface> next;
    mIt->GetNext((IInterface**)&next);
    *res = next;
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
        return NOERROR;
    } else {
        result = new _SecondEnumeration(this);
        *enu = result;
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
    (IMap::Probe(mTable))->Get(seq, (IInterface**)&outface);
    return NOERROR;
}

AutoPtr<ISet> ListResourceBundle::HandleKeySet()
{
    InitializeTable();
    AutoPtr<ISet> set;
    (IMap::Probe(mTable))->GetKeySet((ISet**)&set);
    return set;
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