
#include "Elastos.Droid.Content.h"
#include "elastos/droid/provider/CContactsContractCommonDataKindsIm.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/coredef.h>

using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractCommonDataKindsIm)

CAR_INTERFACE_IMPL_4(CContactsContractCommonDataKindsIm, Singleton
    , IContactsContractCommonDataKindsIm
    , IContactsContractDataColumnsWithJoins
    , IContactsContractCommonDataKindsCommonColumns
    , IContactsContractCommonDataKindsBaseTypes)

ECode CContactsContractCommonDataKindsIm::GetTypeLabelResource(
    /* [in] */ Int32 type,
    /* [out] */ Int32* resource)
{
    VALIDATE_NOT_NULL(resource);

    switch (type) {
        case TYPE_HOME:
            *resource = Elastos::Droid::R::string::imTypeHome;
            break;
        case TYPE_WORK:
            *resource = Elastos::Droid::R::string::imTypeWork;
            break;
        case TYPE_OTHER:
            *resource = Elastos::Droid::R::string::imTypeOther;
            break;
        default:
            *resource = Elastos::Droid::R::string::imTypeCustom;
            break;
    }
     return NOERROR;
}

ECode CContactsContractCommonDataKindsIm::GetTypeLabel(
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

ECode CContactsContractCommonDataKindsIm::GetProtocolLabelResource(
    /* [in] */ Int32 type,
    /* [out] */ Int32* resource)
{
    VALIDATE_NOT_NULL(resource);

    switch (type) {
        case PROTOCOL_AIM:
            *resource = Elastos::Droid::R::string::imProtocolAim;
            break;
        case PROTOCOL_MSN:
            *resource = Elastos::Droid::R::string::imProtocolMsn;
            break;
        case PROTOCOL_YAHOO:
            *resource = Elastos::Droid::R::string::imProtocolYahoo;
            break;
        case PROTOCOL_SKYPE:
            *resource = Elastos::Droid::R::string::imProtocolSkype;
            break;
        case PROTOCOL_QQ:
            *resource = Elastos::Droid::R::string::imProtocolQq;
            break;
        case PROTOCOL_GOOGLE_TALK:
            *resource = Elastos::Droid::R::string::imProtocolGoogleTalk;
            break;
        case PROTOCOL_ICQ:
            *resource = Elastos::Droid::R::string::imProtocolIcq;
            break;
        case PROTOCOL_JABBER:
            *resource = Elastos::Droid::R::string::imProtocolJabber;
            break;
        case PROTOCOL_NETMEETING:
            *resource = Elastos::Droid::R::string::imProtocolNetMeeting;
            break;
        default:
            *resource = Elastos::Droid::R::string::imProtocolCustom;
            break;
    }
    return NOERROR;
}

ECode CContactsContractCommonDataKindsIm::GetProtocolLabel(
    /* [in] */ IResources* res,
    /* [in] */ Int32 type,
    /* [in] */ ICharSequence* label,
    /* [out] */ ICharSequence** lb)
{
    VALIDATE_NOT_NULL(lb);

    if (type == PROTOCOL_CUSTOM && !TextUtils::IsEmpty(label)) {
        *lb = label;
        REFCOUNT_ADD(*lb);
        return NOERROR;
    } else {
        Int32 labelRes;
        FAIL_RETURN(GetProtocolLabelResource(type, &labelRes))
        return res->GetText(labelRes, lb);
    }
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos