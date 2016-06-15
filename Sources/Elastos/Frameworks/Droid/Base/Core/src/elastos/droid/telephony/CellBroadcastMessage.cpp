
#include "elastos/droid/telephony/CellBroadcastMessage.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

////=====================================================================
////             CellBroadcastMessage::InnerParcelableCreator
////=====================================================================
//CellBroadcastMessage::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ CellBroadcastMessage* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//AutoPtr<CellBroadcastMessage> CellBroadcastMessage::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in)
//{
//    // ==================before translated======================
//    // return new CellBroadcastMessage(in);
//    assert(0);
//    AutoPtr<CellBroadcastMessage> empty;
//    return empty;
//}
//
//AutoPtr< ArrayOf< AutoPtr<CellBroadcastMessage> > > CellBroadcastMessage::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size)
//{
//    // ==================before translated======================
//    // return new CellBroadcastMessage[size];
//    assert(0);
//    AutoPtr< ArrayOf< AutoPtr<CellBroadcastMessage> > > empty;
//    return empty;
//}
//
//=====================================================================
//                         CellBroadcastMessage
//=====================================================================
CAR_INTERFACE_IMPL_2(CellBroadcastMessage, Object, ICellBroadcastMessage, IParcelable);
const String CellBroadcastMessage::SMS_CB_MESSAGE_EXTRA("com.android.cellbroadcastreceiver.SMS_CB_MESSAGE");
//const AutoPtr<IParcelable> AutoPtr< ::Creator<CellBroadcastMessage> > CellBroadcastMessage::CREATOR = new InnerParcelableCreator(this);

CellBroadcastMessage::CellBroadcastMessage()
{
}

ECode CellBroadcastMessage::constructor(
    /* [in] */ ISmsCbMessage* message)
{
    // ==================before translated======================
    // mSmsCbMessage = message;
    // mDeliveryTime = System.currentTimeMillis();
    // mIsRead = false;
    // mSubId = SubscriptionManager.getDefaultSmsSubId();
    return NOERROR;
}

ECode CellBroadcastMessage::constructor()
{
    return NOERROR;
}

ECode CellBroadcastMessage::SetSubId(
    /* [in] */ Int64 subId)
{
    // ==================before translated======================
    // mSubId = subId;
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::GetSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSubId;
    assert(0);
    return NOERROR;
}

//Int32 CellBroadcastMessage::DescribeContents()
//{
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return 0;
//}

ECode CellBroadcastMessage::WriteToParcel(
    /* [in] */ IParcel* out)
{
    VALIDATE_NOT_NULL(out);
    // ==================before translated======================
    // mSmsCbMessage.writeToParcel(out, flags);
    // out.writeLong(mDeliveryTime);
    // out.writeInt(mIsRead ? 1 : 0);
    // out.writeLong(mSubId);
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    // ==================before translated======================
    // mSmsCbMessage = new SmsCbMessage(in);
    // mDeliveryTime = in.readLong();
    // mIsRead = (in.readInt() != 0);
    // mSubId = in.readLong();
    assert(0);
    return NOERROR;
}


AutoPtr<CellBroadcastMessage> CellBroadcastMessage::CreateFromCursor(
    /* [in] */ ICursor* cursor)
{
    // ==================before translated======================
    // int geoScope = cursor.getInt(
    //         cursor.getColumnIndexOrThrow(Telephony.CellBroadcasts.GEOGRAPHICAL_SCOPE));
    // int serialNum = cursor.getInt(
    //         cursor.getColumnIndexOrThrow(Telephony.CellBroadcasts.SERIAL_NUMBER));
    // int category = cursor.getInt(
    //         cursor.getColumnIndexOrThrow(Telephony.CellBroadcasts.SERVICE_CATEGORY));
    // String language = cursor.getString(
    //         cursor.getColumnIndexOrThrow(Telephony.CellBroadcasts.LANGUAGE_CODE));
    // String body = cursor.getString(
    //         cursor.getColumnIndexOrThrow(Telephony.CellBroadcasts.MESSAGE_BODY));
    // int format = cursor.getInt(
    //         cursor.getColumnIndexOrThrow(Telephony.CellBroadcasts.MESSAGE_FORMAT));
    // int priority = cursor.getInt(
    //         cursor.getColumnIndexOrThrow(Telephony.CellBroadcasts.MESSAGE_PRIORITY));
    //
    // String plmn;
    // int plmnColumn = cursor.getColumnIndex(Telephony.CellBroadcasts.PLMN);
    // if (plmnColumn != -1 && !cursor.isNull(plmnColumn)) {
    //     plmn = cursor.getString(plmnColumn);
    // } else {
    //     plmn = null;
    // }
    //
    // int lac;
    // int lacColumn = cursor.getColumnIndex(Telephony.CellBroadcasts.LAC);
    // if (lacColumn != -1 && !cursor.isNull(lacColumn)) {
    //     lac = cursor.getInt(lacColumn);
    // } else {
    //     lac = -1;
    // }
    //
    // int cid;
    // int cidColumn = cursor.getColumnIndex(Telephony.CellBroadcasts.CID);
    // if (cidColumn != -1 && !cursor.isNull(cidColumn)) {
    //     cid = cursor.getInt(cidColumn);
    // } else {
    //     cid = -1;
    // }
    //
    // SmsCbLocation location = new SmsCbLocation(plmn, lac, cid);
    //
    // SmsCbEtwsInfo etwsInfo;
    // int etwsWarningTypeColumn = cursor.getColumnIndex(
    //         Telephony.CellBroadcasts.ETWS_WARNING_TYPE);
    // if (etwsWarningTypeColumn != -1 && !cursor.isNull(etwsWarningTypeColumn)) {
    //     int warningType = cursor.getInt(etwsWarningTypeColumn);
    //     etwsInfo = new SmsCbEtwsInfo(warningType, false, false, null);
    // } else {
    //     etwsInfo = null;
    // }
    //
    // SmsCbCmasInfo cmasInfo;
    // int cmasMessageClassColumn = cursor.getColumnIndex(
    //         Telephony.CellBroadcasts.CMAS_MESSAGE_CLASS);
    // if (cmasMessageClassColumn != -1 && !cursor.isNull(cmasMessageClassColumn)) {
    //     int messageClass = cursor.getInt(cmasMessageClassColumn);
    //
    //     int cmasCategory;
    //     int cmasCategoryColumn = cursor.getColumnIndex(
    //             Telephony.CellBroadcasts.CMAS_CATEGORY);
    //     if (cmasCategoryColumn != -1 && !cursor.isNull(cmasCategoryColumn)) {
    //         cmasCategory = cursor.getInt(cmasCategoryColumn);
    //     } else {
    //         cmasCategory = SmsCbCmasInfo.CMAS_CATEGORY_UNKNOWN;
    //     }
    //
    //     int responseType;
    //     int cmasResponseTypeColumn = cursor.getColumnIndex(
    //             Telephony.CellBroadcasts.CMAS_RESPONSE_TYPE);
    //     if (cmasResponseTypeColumn != -1 && !cursor.isNull(cmasResponseTypeColumn)) {
    //         responseType = cursor.getInt(cmasResponseTypeColumn);
    //     } else {
    //         responseType = SmsCbCmasInfo.CMAS_RESPONSE_TYPE_UNKNOWN;
    //     }
    //
    //     int severity;
    //     int cmasSeverityColumn = cursor.getColumnIndex(
    //             Telephony.CellBroadcasts.CMAS_SEVERITY);
    //     if (cmasSeverityColumn != -1 && !cursor.isNull(cmasSeverityColumn)) {
    //         severity = cursor.getInt(cmasSeverityColumn);
    //     } else {
    //         severity = SmsCbCmasInfo.CMAS_SEVERITY_UNKNOWN;
    //     }
    //
    //     int urgency;
    //     int cmasUrgencyColumn = cursor.getColumnIndex(
    //             Telephony.CellBroadcasts.CMAS_URGENCY);
    //     if (cmasUrgencyColumn != -1 && !cursor.isNull(cmasUrgencyColumn)) {
    //         urgency = cursor.getInt(cmasUrgencyColumn);
    //     } else {
    //         urgency = SmsCbCmasInfo.CMAS_URGENCY_UNKNOWN;
    //     }
    //
    //     int certainty;
    //     int cmasCertaintyColumn = cursor.getColumnIndex(
    //             Telephony.CellBroadcasts.CMAS_CERTAINTY);
    //     if (cmasCertaintyColumn != -1 && !cursor.isNull(cmasCertaintyColumn)) {
    //         certainty = cursor.getInt(cmasCertaintyColumn);
    //     } else {
    //         certainty = SmsCbCmasInfo.CMAS_CERTAINTY_UNKNOWN;
    //     }
    //
    //     cmasInfo = new SmsCbCmasInfo(messageClass, cmasCategory, responseType, severity,
    //             urgency, certainty);
    // } else {
    //     cmasInfo = null;
    // }
    //
    // SmsCbMessage msg = new SmsCbMessage(format, geoScope, serialNum, location, category,
    //         language, body, priority, etwsInfo, cmasInfo);
    //
    // long deliveryTime = cursor.getLong(cursor.getColumnIndexOrThrow(
    //         Telephony.CellBroadcasts.DELIVERY_TIME));
    // boolean isRead = (cursor.getInt(cursor.getColumnIndexOrThrow(
    //         Telephony.CellBroadcasts.MESSAGE_READ)) != 0);
    //
    // return new CellBroadcastMessage(msg, deliveryTime, isRead);
    assert(0);
    AutoPtr<CellBroadcastMessage> empty;
    return empty;
}

ECode CellBroadcastMessage::GetContentValues(
    /* [out] */ IContentValues** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // ContentValues cv = new ContentValues(16);
    // SmsCbMessage msg = mSmsCbMessage;
    // cv.put(Telephony.CellBroadcasts.GEOGRAPHICAL_SCOPE, msg.getGeographicalScope());
    // SmsCbLocation location = msg.getLocation();
    // if (location.getPlmn() != null) {
    //     cv.put(Telephony.CellBroadcasts.PLMN, location.getPlmn());
    // }
    // if (location.getLac() != -1) {
    //     cv.put(Telephony.CellBroadcasts.LAC, location.getLac());
    // }
    // if (location.getCid() != -1) {
    //     cv.put(Telephony.CellBroadcasts.CID, location.getCid());
    // }
    // cv.put(Telephony.CellBroadcasts.SERIAL_NUMBER, msg.getSerialNumber());
    // cv.put(Telephony.CellBroadcasts.SERVICE_CATEGORY, msg.getServiceCategory());
    // cv.put(Telephony.CellBroadcasts.LANGUAGE_CODE, msg.getLanguageCode());
    // cv.put(Telephony.CellBroadcasts.MESSAGE_BODY, msg.getMessageBody());
    // cv.put(Telephony.CellBroadcasts.DELIVERY_TIME, mDeliveryTime);
    // cv.put(Telephony.CellBroadcasts.MESSAGE_READ, mIsRead);
    // cv.put(Telephony.CellBroadcasts.MESSAGE_FORMAT, msg.getMessageFormat());
    // cv.put(Telephony.CellBroadcasts.MESSAGE_PRIORITY, msg.getMessagePriority());
    //
    // SmsCbEtwsInfo etwsInfo = mSmsCbMessage.getEtwsWarningInfo();
    // if (etwsInfo != null) {
    //     cv.put(Telephony.CellBroadcasts.ETWS_WARNING_TYPE, etwsInfo.getWarningType());
    // }
    //
    // SmsCbCmasInfo cmasInfo = mSmsCbMessage.getCmasWarningInfo();
    // if (cmasInfo != null) {
    //     cv.put(Telephony.CellBroadcasts.CMAS_MESSAGE_CLASS, cmasInfo.getMessageClass());
    //     cv.put(Telephony.CellBroadcasts.CMAS_CATEGORY, cmasInfo.getCategory());
    //     cv.put(Telephony.CellBroadcasts.CMAS_RESPONSE_TYPE, cmasInfo.getResponseType());
    //     cv.put(Telephony.CellBroadcasts.CMAS_SEVERITY, cmasInfo.getSeverity());
    //     cv.put(Telephony.CellBroadcasts.CMAS_URGENCY, cmasInfo.getUrgency());
    //     cv.put(Telephony.CellBroadcasts.CMAS_CERTAINTY, cmasInfo.getCertainty());
    // }
    //
    // return cv;
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::SetIsRead(
    /* [in] */ Boolean isRead)
{
    // ==================before translated======================
    // mIsRead = isRead;
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::GetLanguageCode(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSmsCbMessage.getLanguageCode();
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::GetServiceCategory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSmsCbMessage.getServiceCategory();
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::GetDeliveryTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDeliveryTime;
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::GetMessageBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSmsCbMessage.getMessageBody();
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::IsRead(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIsRead;
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::GetSerialNumber(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSmsCbMessage.getSerialNumber();
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::GetCmasWarningInfo(
    /* [out] */ ISmsCbCmasInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSmsCbMessage.getCmasWarningInfo();
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::GetEtwsWarningInfo(
    /* [out] */ ISmsCbEtwsInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSmsCbMessage.getEtwsWarningInfo();
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::IsPublicAlertMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSmsCbMessage.isEmergencyMessage();
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::IsEmergencyAlertMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSmsCbMessage.isEmergencyMessage();
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::IsEtwsMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSmsCbMessage.isEtwsMessage();
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::IsCmasMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSmsCbMessage.isCmasMessage();
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::GetCmasMessageClass(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mSmsCbMessage.isCmasMessage()) {
    //     return mSmsCbMessage.getCmasWarningInfo().getMessageClass();
    // } else {
    //     return SmsCbCmasInfo.CMAS_CLASS_UNKNOWN;
    // }
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::IsEtwsPopupAlert(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // SmsCbEtwsInfo etwsInfo = mSmsCbMessage.getEtwsWarningInfo();
    // return etwsInfo != null && etwsInfo.isPopupAlert();
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::IsEtwsEmergencyUserAlert(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // SmsCbEtwsInfo etwsInfo = mSmsCbMessage.getEtwsWarningInfo();
    // return etwsInfo != null && etwsInfo.isEmergencyUserAlert();
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::IsEtwsTestMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // SmsCbEtwsInfo etwsInfo = mSmsCbMessage.getEtwsWarningInfo();
    // return etwsInfo != null &&
    //         etwsInfo.getWarningType() == SmsCbEtwsInfo.ETWS_WARNING_TYPE_TEST_MESSAGE;
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::GetDateString(
    /* [in] */ IContext* context,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // int flags = DateUtils.FORMAT_NO_NOON_MIDNIGHT | DateUtils.FORMAT_SHOW_TIME |
    //         DateUtils.FORMAT_ABBREV_ALL | DateUtils.FORMAT_SHOW_DATE |
    //         DateUtils.FORMAT_CAP_AMPM;
    // return DateUtils.formatDateTime(context, mDeliveryTime, flags);
    assert(0);
    return NOERROR;
}

ECode CellBroadcastMessage::GetSpokenDateString(
    /* [in] */ IContext* context,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // int flags = DateUtils.FORMAT_SHOW_TIME | DateUtils.FORMAT_SHOW_DATE;
    // return DateUtils.formatDateTime(context, mDeliveryTime, flags);
    assert(0);
    return NOERROR;
}

CellBroadcastMessage::CellBroadcastMessage(
    /* [in] */ ISmsCbMessage* message,
    /* [in] */ Int64 deliveryTime,
    /* [in] */ Boolean isRead)
{
    // ==================before translated======================
    // mSmsCbMessage = message;
    // mDeliveryTime = deliveryTime;
    // mIsRead = isRead;
    // mSubId = SubscriptionManager.getDefaultSmsSubId();
}

CellBroadcastMessage::CellBroadcastMessage(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // mSmsCbMessage = new SmsCbMessage(in);
    // mDeliveryTime = in.readLong();
    // mIsRead = (in.readInt() != 0);
    // mSubId = in.readLong();
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

