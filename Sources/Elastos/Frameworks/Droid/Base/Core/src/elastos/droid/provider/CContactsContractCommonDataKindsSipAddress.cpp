
#include "Elastos.Droid.Content.h"
#include "elastos/droid/provider/CContactsContractCommonDataKindsSipAddress.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/coredef.h>
#include "elastos/droid/R.h"

using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractCommonDataKindsSipAddress)

CAR_INTERFACE_IMPL_3(CContactsContractCommonDataKindsSipAddress, Singleton
    , IContactsContractCommonDataKindsSipAddress
    , IContactsContractDataColumnsWithJoins
    , IContactsContractCommonDataKindsCommonColumns)

ECode CContactsContractCommonDataKindsSipAddress::GetTypeLabelResource(
    /* [in] */ Int32 type,
    /* [out] */ Int32* resource)
{
    VALIDATE_NOT_NULL(resource);

    switch (type) {
        case TYPE_HOME:
            *resource = Elastos::Droid::R::string::sipAddressTypeHome;
            break;
        case TYPE_WORK:
            *resource = Elastos::Droid::R::string::sipAddressTypeWork;
            break;
        case TYPE_OTHER:
            *resource = Elastos::Droid::R::string::sipAddressTypeOther;
            break;
        default:
            *resource = Elastos::Droid::R::string::sipAddressTypeCustom;
            break;
    }
     return NOERROR;
}

ECode CContactsContractCommonDataKindsSipAddress::GetTypeLabel(
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