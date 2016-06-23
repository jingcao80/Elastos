#include "elastos/droid/internal/telephony/gsm/CSuppServiceNotification.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"

using Elastos::Droid::Telephony::PhoneNumberUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

const Int32 CSuppServiceNotification::NOTIFICATION_TYPE_MO;
const Int32 CSuppServiceNotification::NOTIFICATION_TYPE_MT;
const Int32 CSuppServiceNotification::MO_CODE_UNCONDITIONAL_CF_ACTIVE;
const Int32 CSuppServiceNotification::MO_CODE_SOME_CF_ACTIVE;
const Int32 CSuppServiceNotification::MO_CODE_CALL_FORWARDED;
const Int32 CSuppServiceNotification::MO_CODE_CALL_IS_WAITING;
const Int32 CSuppServiceNotification::MO_CODE_CUG_CALL;
const Int32 CSuppServiceNotification::MO_CODE_OUTGOING_CALLS_BARRED;
const Int32 CSuppServiceNotification::MO_CODE_INCOMING_CALLS_BARRED;
const Int32 CSuppServiceNotification::MO_CODE_CLIR_SUPPRESSION_REJECTED;
const Int32 CSuppServiceNotification::MO_CODE_CALL_DEFLECTED;
const Int32 CSuppServiceNotification::MT_CODE_FORWARDED_CALL;
const Int32 CSuppServiceNotification::MT_CODE_CUG_CALL;
const Int32 CSuppServiceNotification::MT_CODE_CALL_ON_HOLD;
const Int32 CSuppServiceNotification::MT_CODE_CALL_RETRIEVED;
const Int32 CSuppServiceNotification::MT_CODE_MULTI_PARTY_CALL;
const Int32 CSuppServiceNotification::MT_CODE_ON_HOLD_CALL_RELEASED;
const Int32 CSuppServiceNotification::MT_CODE_FORWARD_CHECK_RECEIVED;
const Int32 CSuppServiceNotification::MT_CODE_CALL_CONNECTING_ECT;
const Int32 CSuppServiceNotification::MT_CODE_CALL_CONNECTED_ECT;
const Int32 CSuppServiceNotification::MT_CODE_DEFLECTED_CALL;
const Int32 CSuppServiceNotification::MT_CODE_ADDITIONAL_CALL_FORWARDED;

CAR_INTERFACE_IMPL(CSuppServiceNotification, Object, ISuppServiceNotification)

CAR_OBJECT_IMPL(CSuppServiceNotification)

CSuppServiceNotification::CSuppServiceNotification()
    : mNotificationType(0)
    , mCode(0)
    , mIndex(0)
    , mType(0)
{
}

ECode CSuppServiceNotification::constructor()
{
    return NOERROR;
}

ECode CSuppServiceNotification::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String str;
    *result = String(" mobile")
        + (mNotificationType == NOTIFICATION_TYPE_MO ? " originated " : " terminated ")
        + " code: " + mCode
        + " index: " + mIndex
        + " \""
        + (PhoneNumberUtils::StringFromStringAndTOA(mNumber, mType, &str), str) + "\" ";
    return NOERROR;
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos