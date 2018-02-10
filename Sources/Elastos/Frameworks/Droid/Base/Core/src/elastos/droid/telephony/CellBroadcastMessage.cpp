//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Text.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/telephony/CellBroadcastMessage.h"
#include "elastos/droid/telephony/CSmsCbCmasInfo.h"
#include "elastos/droid/telephony/CSmsCbEtwsInfo.h"
#include "elastos/droid/telephony/CSmsCbLocation.h"
#include "elastos/droid/telephony/CSmsCbMessage.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/text/format/DateUtils.h"

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Provider::ITelephonyCellBroadcasts;
using Elastos::Droid::Telephony::CSmsCbCmasInfo;
using Elastos::Droid::Telephony::CSmsCbEtwsInfo;
using Elastos::Droid::Telephony::CSmsCbLocation;
using Elastos::Droid::Telephony::CSmsCbMessage;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;

namespace Elastos {
namespace Droid {
namespace Telephony {


//=====================================================================
//                         CellBroadcastMessage
//=====================================================================
CAR_INTERFACE_IMPL_2(CellBroadcastMessage, Object, ICellBroadcastMessage, IParcelable);
const String CellBroadcastMessage::SMS_CB_MESSAGE_EXTRA("com.android.cellbroadcastreceiver.SMS_CB_MESSAGE");

CellBroadcastMessage::CellBroadcastMessage()
    : mDeliveryTime(0)
    , mIsRead(FALSE)
    , mSubId(0)
{
}

ECode CellBroadcastMessage::constructor(
    /* [in] */ ISmsCbMessage* message)
{
    mSmsCbMessage = message;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mDeliveryTime);
    mIsRead = FALSE;
    SubscriptionManager::GetDefaultSmsSubId(&mSubId);
    return NOERROR;
}

ECode CellBroadcastMessage::constructor()
{
    return NOERROR;
}

ECode CellBroadcastMessage::SetSubId(
    /* [in] */ Int64 subId)
{
    mSubId = subId;
    return NOERROR;
}

ECode CellBroadcastMessage::GetSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSubId;
    return NOERROR;
}

ECode CellBroadcastMessage::WriteToParcel(
    /* [in] */ IParcel* out)
{
    VALIDATE_NOT_NULL(out);
    out->WriteInterfacePtr(mSmsCbMessage);
    out->WriteInt64(mDeliveryTime);
    out->WriteInt32(mIsRead ? 1 : 0);
    out->WriteInt64(mSubId);
    return NOERROR;
}

ECode CellBroadcastMessage::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mSmsCbMessage = ISmsCbMessage::Probe(obj);
    source->ReadInt64(&mDeliveryTime);
    Int32 val;
    source->ReadInt32(&val);
    mIsRead = val ? TRUE : FALSE;
    source->ReadInt64(&mSubId);
    return NOERROR;
}

AutoPtr<CellBroadcastMessage> CellBroadcastMessage::CreateFromCursor(
    /* [in] */ ICursor* cursor)
{
    Int32 val;
    cursor->GetColumnIndexOrThrow(ITelephonyCellBroadcasts::GEOGRAPHICAL_SCOPE, &val);
    Int32 geoScope;
    cursor->GetInt32(val, &geoScope);

    cursor->GetColumnIndexOrThrow(ITelephonyCellBroadcasts::SERIAL_NUMBER, &val);
    Int32 serialNum;
    cursor->GetInt32(val, &serialNum);

    cursor->GetColumnIndexOrThrow(ITelephonyCellBroadcasts::SERVICE_CATEGORY, &val);
    Int32 category;
    cursor->GetInt32(val, &category);

    cursor->GetColumnIndexOrThrow(ITelephonyCellBroadcasts::LANGUAGE_CODE, &val);
    String language;
    cursor->GetString(val, &language);

    cursor->GetColumnIndexOrThrow(ITelephonyCellBroadcasts::MESSAGE_BODY, &val);
    String body;
    cursor->GetString(val, &body);

    cursor->GetColumnIndexOrThrow(ITelephonyCellBroadcasts::MESSAGE_FORMAT, &val);
    Int32 format;
    cursor->GetInt32(val, &format);

    cursor->GetColumnIndexOrThrow(ITelephonyCellBroadcasts::MESSAGE_PRIORITY, &val);
    Int32 priority;
    cursor->GetInt32(val, &priority);

    String plmn;
    Boolean b;
    Int32 plmnColumn;
    cursor->GetColumnIndex(ITelephonyCellBroadcasts::PLMN, &plmnColumn);
    if (plmnColumn != -1 && (cursor->IsNull(plmnColumn, &b), !b)) {
        cursor->GetString(plmnColumn, &plmn);
    }
    else {
        plmn = String(NULL);
    }

    Int32 lac;
    Int32 lacColumn;
    cursor->GetColumnIndex(ITelephonyCellBroadcasts::LAC, &lacColumn);
    if (lacColumn != -1 && (cursor->IsNull(lacColumn, &b), !b)) {
        cursor->GetInt32(lacColumn, &lac);
    }
    else {
        lac = -1;
    }

    Int32 cid;
    Int32 cidColumn;
    cursor->GetColumnIndex(ITelephonyCellBroadcasts::CID, &cidColumn);
    if (cidColumn != -1 && (cursor->IsNull(cidColumn, &b), !b)) {
        cursor->GetInt32(cidColumn, &cid);
    }
    else {
        cid = -1;
    }

    AutoPtr<ISmsCbLocation> location;
    CSmsCbLocation::New(plmn, lac, cid, (ISmsCbLocation**)&location);

    AutoPtr<ISmsCbEtwsInfo> etwsInfo;
    Int32 etwsWarningTypeColumn;
    cursor->GetColumnIndex(ITelephonyCellBroadcasts::ETWS_WARNING_TYPE,
            &etwsWarningTypeColumn);
    if (etwsWarningTypeColumn != -1 && (cursor->IsNull(etwsWarningTypeColumn, &b), !b)) {
        Int32 warningType;
        cursor->GetInt32(etwsWarningTypeColumn, &warningType);
        CSmsCbEtwsInfo::New(warningType, FALSE, FALSE, NULL, (ISmsCbEtwsInfo**)&etwsInfo);
    }
    else {
        etwsInfo = NULL;
    }

    AutoPtr<ISmsCbCmasInfo> cmasInfo;
    Int32 cmasMessageClassColumn;
    cursor->GetColumnIndex(ITelephonyCellBroadcasts::CMAS_MESSAGE_CLASS,
                &cmasMessageClassColumn);
    if (cmasMessageClassColumn != -1 && (cursor->IsNull(cmasMessageClassColumn, &b), !b)) {
        Int32 messageClass;
        cursor->GetInt32(cmasMessageClassColumn, &messageClass);

        Int32 cmasCategory;
        Int32 cmasCategoryColumn;
        cursor->GetColumnIndex(ITelephonyCellBroadcasts::CMAS_CATEGORY,
                &cmasCategoryColumn);
        if (cmasCategoryColumn != -1 && (cursor->IsNull(cmasCategoryColumn, &b), !b)) {
            cursor->GetInt32(cmasCategoryColumn, &cmasCategory);
        }
        else {
            cmasCategory = ISmsCbCmasInfo::CMAS_CATEGORY_UNKNOWN;
        }

        Int32 responseType;
        Int32 cmasResponseTypeColumn;
        cursor->GetColumnIndex(ITelephonyCellBroadcasts::CMAS_RESPONSE_TYPE,
                &cmasResponseTypeColumn);
        if (cmasResponseTypeColumn != -1 && (cursor->IsNull(cmasResponseTypeColumn, &b), !b)) {
            cursor->GetInt32(cmasResponseTypeColumn, &responseType);
        }
        else {
            responseType = ISmsCbCmasInfo::CMAS_RESPONSE_TYPE_UNKNOWN;
        }

        Int32 severity;
        Int32 cmasSeverityColumn;
        cursor->GetColumnIndex(ITelephonyCellBroadcasts::CMAS_SEVERITY,
                &cmasSeverityColumn);
        if (cmasSeverityColumn != -1 && (cursor->IsNull(cmasSeverityColumn, &b), !b)) {
            cursor->GetInt32(cmasSeverityColumn, &severity);
        }
        else {
            severity = ISmsCbCmasInfo::CMAS_SEVERITY_UNKNOWN;
        }

        Int32 urgency;
        Int32 cmasUrgencyColumn;
        cursor->GetColumnIndex(ITelephonyCellBroadcasts::CMAS_URGENCY,
                &cmasUrgencyColumn);
        if (cmasUrgencyColumn != -1 && (cursor->IsNull(cmasUrgencyColumn, &b), !b)) {
            cursor->GetInt32(cmasUrgencyColumn, &urgency);
        }
        else {
            urgency = ISmsCbCmasInfo::CMAS_URGENCY_UNKNOWN;
        }

        Int32 certainty;
        Int32 cmasCertaintyColumn;
        cursor->GetColumnIndex(ITelephonyCellBroadcasts::CMAS_CERTAINTY,
                &cmasCertaintyColumn);
        if (cmasCertaintyColumn != -1 && (cursor->IsNull(cmasCertaintyColumn, &b), !b)) {
            cursor->GetInt32(cmasCertaintyColumn, &certainty);
        }
        else {
            certainty = ISmsCbCmasInfo::CMAS_CERTAINTY_UNKNOWN;
        }

        CSmsCbCmasInfo::New(messageClass, cmasCategory, responseType, severity,
                urgency, certainty, (ISmsCbCmasInfo**)&cmasInfo);
    }
    else {
        cmasInfo = NULL;
    }

    AutoPtr<ISmsCbMessage> msg;
    CSmsCbMessage::New(format, geoScope, serialNum, location, category,
            language, body, priority, etwsInfo, cmasInfo, (ISmsCbMessage**)&msg);

    cursor->GetColumnIndexOrThrow(ITelephonyCellBroadcasts::DELIVERY_TIME, &val);
    Int64 deliveryTime;
    cursor->GetInt64(val, &deliveryTime);
    cursor->GetColumnIndexOrThrow(ITelephonyCellBroadcasts::MESSAGE_READ, &val);
    Int32 val2;
    cursor->GetInt32(val, &val2);
    Boolean isRead = (val2 != 0);

    return new CellBroadcastMessage(msg, deliveryTime, isRead);
}

ECode CellBroadcastMessage::GetContentValues(
    /* [out] */ IContentValues** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IContentValues> cv;
    CContentValues::New(16, (IContentValues**)&cv);
    AutoPtr<ISmsCbMessage> msg = mSmsCbMessage;
    Int32 val;
    msg->GetGeographicalScope(&val);
    cv->Put(ITelephonyCellBroadcasts::GEOGRAPHICAL_SCOPE, val);

    AutoPtr<ISmsCbLocation> location;
    msg->GetLocation((ISmsCbLocation**)&location);

    String str;
    location->GetPlmn(&str);
    if (!str.IsNull()) {
        cv->Put(ITelephonyCellBroadcasts::PLMN, str);
    }
    location->GetLac(&val);
    if (val != -1) {
        cv->Put(ITelephonyCellBroadcasts::LAC, val);
    }
    location->GetCid(&val);
    if (val != -1) {
        cv->Put(ITelephonyCellBroadcasts::CID, val);
    }
    cv->Put(ITelephonyCellBroadcasts::SERIAL_NUMBER, (msg->GetSerialNumber(&val), val));
    cv->Put(ITelephonyCellBroadcasts::SERVICE_CATEGORY, (msg->GetServiceCategory(&val), val));
    cv->Put(ITelephonyCellBroadcasts::LANGUAGE_CODE, (msg->GetLanguageCode(&str), str));
    cv->Put(ITelephonyCellBroadcasts::MESSAGE_BODY, (msg->GetMessageBody(&str), str));
    cv->Put(ITelephonyCellBroadcasts::DELIVERY_TIME, mDeliveryTime);
    cv->Put(ITelephonyCellBroadcasts::MESSAGE_READ, mIsRead);
    cv->Put(ITelephonyCellBroadcasts::MESSAGE_FORMAT, (msg->GetMessageFormat(&val), val));
    cv->Put(ITelephonyCellBroadcasts::MESSAGE_PRIORITY, (msg->GetMessagePriority(&val), val));

    AutoPtr<ISmsCbEtwsInfo> etwsInfo;
    mSmsCbMessage->GetEtwsWarningInfo((ISmsCbEtwsInfo**)&etwsInfo);
    if (etwsInfo != NULL) {
        cv->Put(ITelephonyCellBroadcasts::ETWS_WARNING_TYPE, (etwsInfo->GetWarningType(&val), val));
    }

    AutoPtr<ISmsCbCmasInfo> cmasInfo;
    mSmsCbMessage->GetCmasWarningInfo((ISmsCbCmasInfo**)&cmasInfo);
    if (cmasInfo != NULL) {
        cv->Put(ITelephonyCellBroadcasts::CMAS_MESSAGE_CLASS, (cmasInfo->GetMessageClass(&val), val));
        cv->Put(ITelephonyCellBroadcasts::CMAS_CATEGORY, (cmasInfo->GetCategory(&val), val));
        cv->Put(ITelephonyCellBroadcasts::CMAS_RESPONSE_TYPE, (cmasInfo->GetResponseType(&val), val));
        cv->Put(ITelephonyCellBroadcasts::CMAS_SEVERITY, (cmasInfo->GetSeverity(&val), val));
        cv->Put(ITelephonyCellBroadcasts::CMAS_URGENCY, (cmasInfo->GetUrgency(&val), val));
        cv->Put(ITelephonyCellBroadcasts::CMAS_CERTAINTY, (cmasInfo->GetCertainty(&val), val));
    }

    *result = cv;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CellBroadcastMessage::SetIsRead(
    /* [in] */ Boolean isRead)
{
    mIsRead = isRead;
    return NOERROR;
}

ECode CellBroadcastMessage::GetLanguageCode(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mSmsCbMessage->GetLanguageCode(result);
}

ECode CellBroadcastMessage::GetServiceCategory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mSmsCbMessage->GetServiceCategory(result);
}

ECode CellBroadcastMessage::GetDeliveryTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDeliveryTime;
    return NOERROR;
}

ECode CellBroadcastMessage::GetMessageBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mSmsCbMessage->GetMessageBody(result);
}

ECode CellBroadcastMessage::IsRead(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsRead;
    return NOERROR;
}

ECode CellBroadcastMessage::GetSerialNumber(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mSmsCbMessage->GetSerialNumber(result);
}

ECode CellBroadcastMessage::GetCmasWarningInfo(
    /* [out] */ ISmsCbCmasInfo** result)
{
    VALIDATE_NOT_NULL(result);
    return mSmsCbMessage->GetCmasWarningInfo(result);
}

ECode CellBroadcastMessage::GetEtwsWarningInfo(
    /* [out] */ ISmsCbEtwsInfo** result)
{
    VALIDATE_NOT_NULL(result);
    return mSmsCbMessage->GetEtwsWarningInfo(result);
}

ECode CellBroadcastMessage::IsPublicAlertMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mSmsCbMessage->IsEmergencyMessage(result);
}

ECode CellBroadcastMessage::IsEmergencyAlertMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mSmsCbMessage->IsEmergencyMessage(result);
}

ECode CellBroadcastMessage::IsEtwsMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mSmsCbMessage->IsEtwsMessage(result);
}

ECode CellBroadcastMessage::IsCmasMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mSmsCbMessage->IsCmasMessage(result);
}

ECode CellBroadcastMessage::GetCmasMessageClass(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b;
    if (mSmsCbMessage->IsCmasMessage(&b), b) {
        AutoPtr<ISmsCbCmasInfo> info;
        mSmsCbMessage->GetCmasWarningInfo((ISmsCbCmasInfo**)&info);
        info->GetMessageClass(result);
    }
    else {
        *result = ISmsCbCmasInfo::CMAS_CLASS_UNKNOWN;
    }
    return NOERROR;
}

ECode CellBroadcastMessage::IsEtwsPopupAlert(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsCbEtwsInfo> etwsInfo;
    mSmsCbMessage->GetEtwsWarningInfo((ISmsCbEtwsInfo**)&etwsInfo);
    Boolean b;
    *result = etwsInfo != NULL && (etwsInfo->IsPopupAlert(&b), b);
    return NOERROR;
}

ECode CellBroadcastMessage::IsEtwsEmergencyUserAlert(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsCbEtwsInfo> etwsInfo;
    mSmsCbMessage->GetEtwsWarningInfo((ISmsCbEtwsInfo**)&etwsInfo);
    Boolean b;
    *result = etwsInfo != NULL && (etwsInfo->IsEmergencyUserAlert(&b), b);
    return NOERROR;
}

ECode CellBroadcastMessage::IsEtwsTestMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsCbEtwsInfo> etwsInfo;
    mSmsCbMessage->GetEtwsWarningInfo((ISmsCbEtwsInfo**)&etwsInfo);
    Int32 type;
    *result = etwsInfo != NULL && (etwsInfo->GetWarningType(&type), type)
            == ISmsCbEtwsInfo::ETWS_WARNING_TYPE_TEST_MESSAGE;
    return NOERROR;
}

ECode CellBroadcastMessage::GetDateString(
    /* [in] */ IContext* context,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(result);
    Int32 flags = IDateUtils::FORMAT_NO_NOON_MIDNIGHT | IDateUtils::FORMAT_SHOW_TIME |
            IDateUtils::FORMAT_ABBREV_ALL | IDateUtils::FORMAT_SHOW_DATE |
            IDateUtils::FORMAT_CAP_AMPM;
    *result = DateUtils::FormatDateTime(context, mDeliveryTime, flags);
    return NOERROR;
}

ECode CellBroadcastMessage::GetSpokenDateString(
    /* [in] */ IContext* context,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(result);
    Int32 flags = IDateUtils::FORMAT_SHOW_TIME | IDateUtils::FORMAT_SHOW_DATE;
    *result = DateUtils::FormatDateTime(context, mDeliveryTime, flags);
    return NOERROR;
}

CellBroadcastMessage::CellBroadcastMessage(
    /* [in] */ ISmsCbMessage* message,
    /* [in] */ Int64 deliveryTime,
    /* [in] */ Boolean isRead)
{
    mSmsCbMessage = message;
    mDeliveryTime = deliveryTime;
    mIsRead = isRead;
    SubscriptionManager::GetDefaultSmsSubId(&mSubId);
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

