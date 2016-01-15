
#include "Elastos.Droid.Content.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsContractCommonDataKindsStructuredPostal.h"
#include "elastos/droid/provider/ContactsContractData.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Net::Uri;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractCommonDataKindsStructuredPostal)

CAR_INTERFACE_IMPL_3(CContactsContractCommonDataKindsStructuredPostal, Singleton
    , IContactsContractCommonDataKindsStructuredPostal
    , IContactsContractDataColumnsWithJoins
    , IContactsContractCommonDataKindsCommonColumns)

ECode CContactsContractCommonDataKindsStructuredPostal::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> _uri;
    FAIL_RETURN(ContactsContractData::GetCONTENT_URI((IUri**)&_uri))
    return Uri::WithAppendedPath(_uri, String("postals"), uri);
}

ECode CContactsContractCommonDataKindsStructuredPostal::GetTypeLabelResource(
    /* [in] */ Int32 type,
    /* [out] */ Int32* resource)
{
    VALIDATE_NOT_NULL(resource);

    switch (type) {
        case TYPE_HOME:
            *resource = Elastos::Droid::R::string::postalTypeHome;
            break;
        case TYPE_WORK:
            *resource = Elastos::Droid::R::string::postalTypeWork;
            break;
        case TYPE_OTHER:
            *resource = Elastos::Droid::R::string::postalTypeOther;
            break;
        default:
            *resource = Elastos::Droid::R::string::postalTypeCustom;
            break;
    }
    return NOERROR;
}

ECode CContactsContractCommonDataKindsStructuredPostal::GetTypeLabel(
    /* [in] */ IResources* res,
    /* [in] */ Int32 type,
    /* [in] */ ICharSequence* label,
    /* [out] */ ICharSequence** lb)
{
    VALIDATE_NOT_NULL(lb);

    if (type == IContactsContractCommonDataKindsBaseTypes::TYPE_CUSTOM && !TextUtils::IsEmpty(label)) {
        *lb = label;
        REFCOUNT_ADD(*lb);
        return NOERROR;
    } else {
        Int32 labelRes;
        FAIL_RETURN(GetTypeLabelResource(type, &labelRes))
        return res->GetText(labelRes, lb);
    }
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos