
#include "elastos/droid/telephony/SmsCbMessage.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

////=====================================================================
////                 SmsCbMessage::InnerParcelableCreator
////=====================================================================
//SmsCbMessage::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ SmsCbMessage* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//AutoPtr<SmsCbMessage> SmsCbMessage::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in)
//{
//    // ==================before translated======================
//    // return new SmsCbMessage(in);
//    assert(0);
//    AutoPtr<SmsCbMessage> empty;
//    return empty;
//}
//
//AutoPtr< ArrayOf< AutoPtr<SmsCbMessage> > > SmsCbMessage::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size)
//{
//    // ==================before translated======================
//    // return new SmsCbMessage[size];
//    assert(0);
//    AutoPtr< ArrayOf< AutoPtr<SmsCbMessage> > > empty;
//    return empty;
//}

//=====================================================================
//                             SmsCbMessage
//=====================================================================
CAR_INTERFACE_IMPL_2(SmsCbMessage, Object, ISmsCbMessage, IParcelable);

const String SmsCbMessage::LOGTAG("SMSCB");

SmsCbMessage::SmsCbMessage()
{
}

SmsCbMessage::SmsCbMessage(
    /* [in] */ Int32 messageFormat,
    /* [in] */ Int32 geographicalScope,
    /* [in] */ Int32 serialNumber,
    /* [in] */ ISmsCbLocation* location,
    /* [in] */ Int32 serviceCategory,
    /* [in] */ const String& language,
    /* [in] */ const String& body,
    /* [in] */ Int32 priority,
    /* [in] */ ISmsCbEtwsInfo* etwsWarningInfo,
    /* [in] */ ISmsCbCmasInfo* cmasWarningInfo)
{
    // ==================before translated======================
    // mMessageFormat = messageFormat;
    // mGeographicalScope = geographicalScope;
    // mSerialNumber = serialNumber;
    // mLocation = location;
    // mServiceCategory = serviceCategory;
    // mLanguage = language;
    // mBody = body;
    // mPriority = priority;
    // mEtwsWarningInfo = etwsWarningInfo;
    // mCmasWarningInfo = cmasWarningInfo;
}

SmsCbMessage::SmsCbMessage(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // mMessageFormat = in.readInt();
    // mGeographicalScope = in.readInt();
    // mSerialNumber = in.readInt();
    // mLocation = new SmsCbLocation(in);
    // mServiceCategory = in.readInt();
    // mLanguage = in.readString();
    // mBody = in.readString();
    // mPriority = in.readInt();
    // int type = in.readInt();
    // switch (type) {
    //     case 'E':
    //         // unparcel ETWS warning information
    //         mEtwsWarningInfo = new SmsCbEtwsInfo(in);
    //         mCmasWarningInfo = null;
    //         break;
    //
    //     case 'C':
    //         // unparcel CMAS warning information
    //         mEtwsWarningInfo = null;
    //         mCmasWarningInfo = new SmsCbCmasInfo(in);
    //         break;
    //
    //     default:
    //         mEtwsWarningInfo = null;
    //         mCmasWarningInfo = null;
    // }
}

ECode SmsCbMessage::constructor()
{
    return NOERROR;
}

ECode SmsCbMessage::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    // ==================before translated======================
    // dest.writeInt(mMessageFormat);
    // dest.writeInt(mGeographicalScope);
    // dest.writeInt(mSerialNumber);
    // mLocation.writeToParcel(dest, flags);
    // dest.writeInt(mServiceCategory);
    // dest.writeString(mLanguage);
    // dest.writeString(mBody);
    // dest.writeInt(mPriority);
    // if (mEtwsWarningInfo != null) {
    //     // parcel ETWS warning information
    //     dest.writeInt('E');
    //     mEtwsWarningInfo.writeToParcel(dest, flags);
    // } else if (mCmasWarningInfo != null) {
    //     // parcel CMAS warning information
    //     dest.writeInt('C');
    //     mCmasWarningInfo.writeToParcel(dest, flags);
    // } else {
    //     // no ETWS or CMAS warning information
    //     dest.writeInt('0');
    // }
    assert(0);
    return NOERROR;
}

ECode SmsCbMessage::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    return NOERROR;
}

ECode SmsCbMessage::GetGeographicalScope(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mGeographicalScope;
    assert(0);
    return NOERROR;
}

ECode SmsCbMessage::GetSerialNumber(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSerialNumber;
    assert(0);
    return NOERROR;
}

ECode SmsCbMessage::GetLocation(
    /* [out] */ ISmsCbLocation** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mLocation;
    assert(0);
    return NOERROR;
}

ECode SmsCbMessage::GetServiceCategory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mServiceCategory;
    assert(0);
    return NOERROR;
}

ECode SmsCbMessage::GetLanguageCode(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mLanguage;
    assert(0);
    return NOERROR;
}

ECode SmsCbMessage::GetMessageBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mBody;
    assert(0);
    return NOERROR;
}

ECode SmsCbMessage::GetMessageFormat(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMessageFormat;
    assert(0);
    return NOERROR;
}

ECode SmsCbMessage::GetMessagePriority(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPriority;
    assert(0);
    return NOERROR;
}

ECode SmsCbMessage::GetEtwsWarningInfo(
    /* [out] */ ISmsCbEtwsInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mEtwsWarningInfo;
    assert(0);
    return NOERROR;
}

ECode SmsCbMessage::GetCmasWarningInfo(
    /* [out] */ ISmsCbCmasInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCmasWarningInfo;
    assert(0);
    return NOERROR;
}

ECode SmsCbMessage::IsEmergencyMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPriority == MESSAGE_PRIORITY_EMERGENCY;
    assert(0);
    return NOERROR;
}

ECode SmsCbMessage::IsEtwsMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mEtwsWarningInfo != null;
    assert(0);
    return NOERROR;
}

ECode SmsCbMessage::IsCmasMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCmasWarningInfo != null;
    assert(0);
    return NOERROR;
}

ECode SmsCbMessage::ToString(
    /* [out] */ String* str)
{
    // ==================before translated======================
    // return "SmsCbMessage{geographicalScope=" + mGeographicalScope + ", serialNumber="
    //         + mSerialNumber + ", location=" + mLocation + ", serviceCategory="
    //         + mServiceCategory + ", language=" + mLanguage + ", body=" + mBody
    //         + ", priority=" + mPriority
    //         + (mEtwsWarningInfo != null ? (", " + mEtwsWarningInfo.toString()) : "")
    //         + (mCmasWarningInfo != null ? (", " + mCmasWarningInfo.toString()) : "") + '}';
    assert(0);
    return NOERROR;
}

//Int32 SmsCbMessage::DescribeContents()
//{
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return 0;
//}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos


