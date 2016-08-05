#include "elastos/droid/internal/telephony/IccUtils.h"
#include "elastos/droid/telephony/SmsCbEtwsInfo.h"
#include "elastos/droid/text/format/CTime.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Internal::Telephony::IccUtils;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Telephony {

//=====================================================================
//                            SmsCbEtwsInfo
//=====================================================================
CAR_INTERFACE_IMPL_2(SmsCbEtwsInfo, Object, ISmsCbEtwsInfo, IParcelable);

SmsCbEtwsInfo::SmsCbEtwsInfo()
    : mWarningType(0)
    , mEmergencyUserAlert(FALSE)
    , mActivatePopup(FALSE)
{
}

ECode SmsCbEtwsInfo::constructor(
    /* [in] */ Int32 warningType,
    /* [in] */ Boolean emergencyUserAlert,
    /* [in] */ Boolean activatePopup,
    /* [in] */ ArrayOf<Byte>* warningSecurityInformation)
{
    mWarningType = warningType;
    mEmergencyUserAlert = emergencyUserAlert;
    mActivatePopup = activatePopup;
    mWarningSecurityInformation = warningSecurityInformation;
    return NOERROR;
}

ECode SmsCbEtwsInfo::constructor()
{
    return NOERROR;
}

ECode SmsCbEtwsInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    dest->WriteInt32(mWarningType);
    dest->WriteInt32(mEmergencyUserAlert ? 1 : 0);
    dest->WriteInt32(mActivatePopup ? 1 : 0);
    dest->WriteArrayOf((Handle32)mWarningSecurityInformation.Get());
    return NOERROR;
}

ECode SmsCbEtwsInfo::ReadFromParcel(
      /* [in] */ IParcel* source)
{
    source->ReadInt32(&mWarningType);
    Int32 val;
    mEmergencyUserAlert = (source->ReadInt32(&val), val) == 1 ? TRUE : FALSE;
    mActivatePopup = (source->ReadInt32(&val), val) == 1 ? TRUE : FALSE;
    source->ReadArrayOf((Handle32*)(&mWarningSecurityInformation));
    return NOERROR;
}

ECode SmsCbEtwsInfo::GetWarningType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mWarningType;
    return NOERROR;
}

ECode SmsCbEtwsInfo::IsEmergencyUserAlert(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEmergencyUserAlert;
    return NOERROR;
}

ECode SmsCbEtwsInfo::IsPopupAlert(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mActivatePopup;
    return NOERROR;
}

ECode SmsCbEtwsInfo::GetPrimaryNotificationTimestamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    if (mWarningSecurityInformation == NULL || mWarningSecurityInformation->GetLength() < 7) {
        *result = 0;
        return NOERROR;
    }

    Int32 year = IccUtils::GsmBcdByteToInt((*mWarningSecurityInformation)[0]);
    Int32 month = IccUtils::GsmBcdByteToInt((*mWarningSecurityInformation)[1]);
    Int32 day = IccUtils::GsmBcdByteToInt((*mWarningSecurityInformation)[2]);
    Int32 hour = IccUtils::GsmBcdByteToInt((*mWarningSecurityInformation)[3]);
    Int32 minute = IccUtils::GsmBcdByteToInt((*mWarningSecurityInformation)[4]);
    Int32 second = IccUtils::GsmBcdByteToInt((*mWarningSecurityInformation)[5]);

    // For the timezone, the most significant bit of the
    // least significant nibble is the sign byte
    // (meaning the max range of this field is 79 quarter-hours,
    // which is more than enough)

    Byte tzByte = (*mWarningSecurityInformation)[6];

    // Mask out sign bit.
    Int32 timezoneOffset = IccUtils::GsmBcdByteToInt((Byte) (tzByte & (~0x08)));

    timezoneOffset = ((tzByte & 0x08) == 0) ? timezoneOffset : -timezoneOffset;

    AutoPtr<CTime> time;
    CTime::NewByFriend(ITime::TIMEZONE_UTC, (CTime**)&time);

    // We only need to support years above 2000.
    time->mYear = year + 2000;
    time->mMonth = month - 1;
    time->mMonthDay = day;
    time->mHour = hour;
    time->mMinute = minute;
    time->mSecond = second;

    // Timezone offset is in quarter hours.
    Int64 val;
    time->ToMillis(TRUE, &val);
    *result = val - timezoneOffset * 15 * 60 * 1000;
    return NOERROR;
}

ECode SmsCbEtwsInfo::GetPrimaryNotificationSignature(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    if (mWarningSecurityInformation == NULL || mWarningSecurityInformation->GetLength() < 50) {
        *result = NULL;
        return NOERROR;
    }
    Arrays::CopyOfRange(mWarningSecurityInformation, 7, 50, result);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SmsCbEtwsInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("SmsCbEtwsInfo{warningType=") + StringUtils::ToString(mWarningType)
            + ", emergencyUserAlert=" + StringUtils::BooleanToString(mEmergencyUserAlert)
            + ", activatePopup=" + StringUtils::BooleanToString(mActivatePopup) + '}';
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
