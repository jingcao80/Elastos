
#include "elastos/droid/telephony/SmsCbEtwsInfo.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

////=====================================================================
////                     SmsCbEtwsInfo::InnerCreator
////=====================================================================
//SmsCbEtwsInfo::InnerCreator::InnerCreator(
//    /* [in] */ SmsCbEtwsInfo* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//AutoPtr<SmsCbEtwsInfo> SmsCbEtwsInfo::InnerCreator::CreateFromParcel(
//    /* [in] */ IParcel* in)
//{
//    // ==================before translated======================
//    // return new SmsCbEtwsInfo(in);
//    assert(0);
//    AutoPtr<SmsCbEtwsInfo> empty;
//    return empty;
//}
//
//AutoPtr< ArrayOf< AutoPtr<SmsCbEtwsInfo> > > SmsCbEtwsInfo::InnerCreator::NewArray(
//    /* [in] */ Int32 size)
//{
//    // ==================before translated======================
//    // return new SmsCbEtwsInfo[size];
//    assert(0);
//    AutoPtr< ArrayOf< AutoPtr<SmsCbEtwsInfo> > > empty;
//    return empty;
//}

//=====================================================================
//                            SmsCbEtwsInfo
//=====================================================================
CAR_INTERFACE_IMPL_2(SmsCbEtwsInfo, Object, ISmsCbEtwsInfo, IParcelable);

SmsCbEtwsInfo::SmsCbEtwsInfo()
{
}

ECode SmsCbEtwsInfo::constructor(
    /* [in] */ Int32 warningType,
    /* [in] */ Boolean emergencyUserAlert,
    /* [in] */ Boolean activatePopup,
    /* [in] */ ArrayOf<Byte>* warningSecurityInformation)
{
    // ==================before translated======================
    // mWarningType = warningType;
    // mEmergencyUserAlert = emergencyUserAlert;
    // mActivatePopup = activatePopup;
    // mWarningSecurityInformation = warningSecurityInformation;
    return NOERROR;
}

ECode SmsCbEtwsInfo::constructor(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // mWarningType = in.readInt();
    // mEmergencyUserAlert = (in.readInt() != 0);
    // mActivatePopup = (in.readInt() != 0);
    // mWarningSecurityInformation = in.createByteArray();
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
    // ==================before translated======================
    // dest.writeInt(mWarningType);
    // dest.writeInt(mEmergencyUserAlert ? 1 : 0);
    // dest.writeInt(mActivatePopup ? 1 : 0);
    // dest.writeByteArray(mWarningSecurityInformation);
    assert(0);
    return NOERROR;
}

ECode SmsCbEtwsInfo::ReadFromParcel(
      /* [in] */ IParcel* source)
{
    assert(0);
    return NOERROR;
}

ECode SmsCbEtwsInfo::GetWarningType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWarningType;
    assert(0);
    return NOERROR;
}

ECode SmsCbEtwsInfo::IsEmergencyUserAlert(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mEmergencyUserAlert;
    assert(0);
    return NOERROR;
}

ECode SmsCbEtwsInfo::IsPopupAlert(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mActivatePopup;
    assert(0);
    return NOERROR;
}

ECode SmsCbEtwsInfo::GetPrimaryNotificationTimestamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mWarningSecurityInformation == null || mWarningSecurityInformation.length < 7) {
    //     return 0;
    // }
    //
    // int year = IccUtils.gsmBcdByteToInt(mWarningSecurityInformation[0]);
    // int month = IccUtils.gsmBcdByteToInt(mWarningSecurityInformation[1]);
    // int day = IccUtils.gsmBcdByteToInt(mWarningSecurityInformation[2]);
    // int hour = IccUtils.gsmBcdByteToInt(mWarningSecurityInformation[3]);
    // int minute = IccUtils.gsmBcdByteToInt(mWarningSecurityInformation[4]);
    // int second = IccUtils.gsmBcdByteToInt(mWarningSecurityInformation[5]);
    //
    // // For the timezone, the most significant bit of the
    // // least significant nibble is the sign byte
    // // (meaning the max range of this field is 79 quarter-hours,
    // // which is more than enough)
    //
    // byte tzByte = mWarningSecurityInformation[6];
    //
    // // Mask out sign bit.
    // int timezoneOffset = IccUtils.gsmBcdByteToInt((byte) (tzByte & (~0x08)));
    //
    // timezoneOffset = ((tzByte & 0x08) == 0) ? timezoneOffset : -timezoneOffset;
    //
    // Time time = new Time(Time.TIMEZONE_UTC);
    //
    // // We only need to support years above 2000.
    // time.year = year + 2000;
    // time.month = month - 1;
    // time.monthDay = day;
    // time.hour = hour;
    // time.minute = minute;
    // time.second = second;
    //
    // // Timezone offset is in quarter hours.
    // return time.toMillis(true) - timezoneOffset * 15 * 60 * 1000;
    assert(0);
    return NOERROR;
}

ECode SmsCbEtwsInfo::GetPrimaryNotificationSignature(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mWarningSecurityInformation == null || mWarningSecurityInformation.length < 50) {
    //     return null;
    // }
    // return Arrays.copyOfRange(mWarningSecurityInformation, 7, 50);
    assert(0);
    return NOERROR;
}

ECode SmsCbEtwsInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    // ==================before translated======================
    // return "SmsCbEtwsInfo{warningType=" + mWarningType + ", emergencyUserAlert="
    //         + mEmergencyUserAlert + ", activatePopup=" + mActivatePopup + '}';
    assert(0);
    return NOERROR;
}

//Int32 SmsCbEtwsInfo::DescribeContents()
//{
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return 0;
//}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
