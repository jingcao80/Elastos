#include "elastos/droid/internal/telephony/gsm/CSsData.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(CSsData, Object, ISsData)

CAR_OBJECT_IMPL(CSsData)

CSsData::CSsData()
    : mServiceClass(0)
    , mResult(0)
{
}

ECode CSsData::constructor()
{
    return NOERROR;
}

ECode CSsData::ServiceTypeFromRILInt(
    /* [in] */ Int32 type,
    /* [out] */ ServiceType* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    assert(0);
    // return ServiceType.values()[type];
    // } catch (IndexOutOfBoundsException e) {
    //     Rlog.e(GSMPhone.LOG_TAG, "Invalid Service type");
    //     return null;
    // }
    *result = 0;
    return NOERROR;
}

ECode CSsData::RequestTypeFromRILInt(
    /* [in] */ Int32 type,
    /* [out] */ RequestType* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    assert(0);
    // return RequestType.values()[type];
    // } catch (IndexOutOfBoundsException e) {
    //     Rlog.e(GSMPhone.LOG_TAG, "Invalid Request type");
    //     return null;
    // }
    *result = 0;
    return NOERROR;
}

ECode CSsData::TeleserviceTypeFromRILInt(
    /* [in] */ Int32 type,
    /* [out] */ TeleserviceType* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    assert(0);
    // return TeleserviceType.values()[type];
    // } catch (IndexOutOfBoundsException e) {
    //     Rlog.e(GSMPhone.LOG_TAG, "Invalid Teleservice type");
    //     return null;
    // }
    *result = 0;
    return NOERROR;
}

ECode CSsData::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("[SsData] ServiceType: ") + StringUtils::ToString(mServiceType)
        + " RequestType: " + StringUtils::ToString(mRequestType)
        + " TeleserviceType: " + StringUtils::ToString(mTeleserviceType)
        + " ServiceClass: " + StringUtils::ToString(mServiceClass)
        + " Result: " + StringUtils::ToString(mResult);
// TODO: Need ServiceType::isTypeCF
        // + " Is Service Type CF: " + StringUtils::ToString(mServiceType).isTypeCF();
    return NOERROR;
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos