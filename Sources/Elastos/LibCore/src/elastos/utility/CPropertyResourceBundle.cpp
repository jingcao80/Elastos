
#include "CPropertyResourceBundle.h"
#include "CProperties.h"
#include "CString.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;

namespace Elastos {
namespace Utility {

//==========================================================
//       CPropertyResourceBundle::_Enumeration
//==========================================================
CAR_INTERFACE_IMPL(CPropertyResourceBundle::_Enumeration, Object, IEnumeration)

CPropertyResourceBundle::_Enumeration::_Enumeration(
    /* [in] */ CPropertyResourceBundle* host)
    : mHost(host)
{
    mLocal = mHost->GetLocalKeys();

    mHost->mParent->GetKeys((IEnumeration**)&mPEnum);
}

ECode CPropertyResourceBundle::_Enumeration::HasMoreElements(
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value)

    Boolean isflag = FALSE;
    if (mLocal->HasMoreElements(&isflag), isflag) {
        *value = TRUE;
        return NOERROR;
    }
    *value = FindNext();
    return NOERROR;
}

ECode CPropertyResourceBundle::_Enumeration::GetNextElement(
    /* [out] */ IInterface ** inter)
{
    VALIDATE_NOT_NULL(inter)
    *inter = NULL;

    Boolean isflag = FALSE;
    if (mLocal->HasMoreElements(&isflag), isflag) {
        return mLocal->GetNextElement(inter);
    }
    if (FindNext()) {
        String result = mNextElement;
        mNextElement = String(NULL);
        AutoPtr<ICharSequence> sq;
        CString::New(result, (ICharSequence**)&sq);
        *inter = sq;
        REFCOUNT_ADD(*inter)
        return NOERROR;
    }
    // Cause an exception
    return mPEnum->GetNextElement(inter);
}

Boolean CPropertyResourceBundle::_Enumeration::FindNext()
{
    if (!mNextElement.IsNull()) {
        return TRUE;
    }
    Boolean isflag = FALSE;
    while (mPEnum->HasMoreElements(&isflag), isflag) {
        AutoPtr<IInterface> next;
        mPEnum->GetNextElement((IInterface**)&next);
        if (IMap::Probe(mHost->mResources)->ContainsKey(next, &isflag), !isflag) {
            AutoPtr<ICharSequence> sq = ICharSequence::Probe(next);
            if (sq != NULL) {
                sq->ToString(&mNextElement);
            }
            return TRUE;
        }
    }
    return FALSE;
}

//==========================================================
//       CPropertyResourceBundle
//==========================================================
CAR_INTERFACE_IMPL(CPropertyResourceBundle, ResourceBundle, IPropertyResourceBundle)

CAR_OBJECT_IMPL(CPropertyResourceBundle)

ECode CPropertyResourceBundle::constructor()
{
    return NOERROR;
}

ECode CPropertyResourceBundle::constructor(
    /* [in] */ IInputStream* stream)
{
    if (stream == NULL) {
        // throw new NullPointerException("stream == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    FAIL_RETURN(CProperties::New((IProperties**)&mResources));
    return mResources->Load(stream);
}

ECode CPropertyResourceBundle::constructor(
    /* [in] */ IReader* reader)
{
    FAIL_RETURN(CProperties::New((IProperties**)&mResources));
    return mResources->Load(reader);
}

ECode CPropertyResourceBundle::GetKeys(
    /* [out] */ IEnumeration** enu)
{
    VALIDATE_NOT_NULL(enu)
    *enu = NULL;

    if (mParent == NULL) {
        AutoPtr<IEnumeration> outenu = GetLocalKeys();
        *enu = outenu;
        REFCOUNT_ADD(*enu)
        return NOERROR;
    }
    AutoPtr<IEnumeration> outenu = (IEnumeration*) new _Enumeration(this);
    *enu = outenu;
    REFCOUNT_ADD(*enu)
    return NOERROR;
}

ECode CPropertyResourceBundle::HandleGetObject(
    /* [in] */ const String& key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)
    AutoPtr<ICharSequence> sq;
    CString::New(key, (ICharSequence**)&sq);
    return (IDictionary::Probe(mResources))->Get(sq, outface);
}

AutoPtr<IEnumeration> CPropertyResourceBundle::GetLocalKeys()
{
    AutoPtr<IEnumeration> outenu;
    mResources->PropertyNames((IEnumeration**)&outenu);
    return outenu;
}

} // namespace Utility
} // namespace Elastos
