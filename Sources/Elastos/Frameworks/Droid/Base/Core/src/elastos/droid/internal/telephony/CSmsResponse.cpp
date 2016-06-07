
#include <elastos/core/IntegralToString.h>
#include "elastos/droid/internal/telephony/CSmsResponse.h"

using Elastos::Core::IntegralToString;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * Object returned by the RIL upon successful completion of sendSMS.
 * Contains message reference and ackPdu.
 *
 */
CAR_OBJECT_IMPL(CSmsResponse)

CAR_INTERFACE_IMPL(CSmsResponse, Object, ISmsResponse)

ECode CSmsResponse::constructor(
    /* [in] */ Int32 messageRef,
    /* [in] */ String ackPdu,
    /* [in] */ Int32 errorCode)
{
    mMessageRef = messageRef;
    mAckPdu = ackPdu;
    mErrorCode = errorCode;
    return NOERROR;
}

//@Override
ECode CSmsResponse::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    String ret = String("{ mMessageRef = ") + IntegralToString::ToString(mMessageRef)
        + String(", mErrorCode = ") + IntegralToString::ToString(mErrorCode)
        + String(", mAckPdu = ") + mAckPdu
        + String("}");
    *str = ret;
    return NOERROR;
}

} //namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
