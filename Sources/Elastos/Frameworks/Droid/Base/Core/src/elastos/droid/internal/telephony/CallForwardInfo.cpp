
#include "elastos/droid/internal/telephony/CallForwardInfo.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"

using Elastos::Droid::Telephony::PhoneNumberUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CallForwardInfo, Object, ICallForwardInfo)
CallForwardInfo::CallForwardInfo()
    : mStatus(0)
    , mReason(0)
    , mServiceClass(0)
    , mToa(0)
    , mTimeSeconds(0)
    , mStartHour(0)
    , mStartMinute(0)
    , mEndHour(0)
    , mEndMinute(0)
{}

ECode CallForwardInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    String value;
    Object::ToString(&value);
    StringBuilder sb(value);
    sb.Append(mStatus == 0 ? " not active " : " active ");
    sb.Append(" mReason: ");
    sb.Append(mReason);
    sb.Append(" mServiceClass: ");
    sb.Append(mServiceClass);
    sb.Append(" \"");

    PhoneNumberUtils::StringFromStringAndTOA(mNumber, mToa, &value);
    sb.Append(value);
    sb.Append("\" ");
    sb.Append(mTimeSeconds);
    sb.Append(" seconds");
    sb.Append(" mStartHour:");
    sb.Append(mStartHour);
    sb.Append(" mStartMinute: ");
    sb.Append(mStartMinute);
    sb.Append(" mEndHour: ");
    sb.Append(mEndHour);
    sb.Append(" mEndMinute: ");
    sb.Append(mEndMinute);
    return sb.ToString(str);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
