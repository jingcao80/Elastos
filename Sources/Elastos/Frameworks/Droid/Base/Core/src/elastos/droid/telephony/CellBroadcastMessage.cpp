/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

package android.telephony;

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Text::Format::IDateUtils;

/**
 * Application wrapper for {@link SmsCbMessage}. This is Parcelable so that
 * decoded broadcast message objects can be passed between running Services.
 * New broadcasts are received by the CellBroadcastReceiver app, which exports
 * the database of previously received broadcasts at "content://cellbroadcasts/".
 * The "android.permission.READ_CELL_BROADCASTS" permission is required to read
 * from the ContentProvider, and writes to the database are not allowed.<p>
 *
 * Use {@link #createFromCursor} to create CellBroadcastMessage objects from rows
 * in the database cursor returned by the ContentProvider.
 *
 * {@hide}
 */
public class CellBroadcastMessage implements Parcelable {

    /** Identifier for GetExtra() when adding this object to an Intent. */
    public static const String SMS_CB_MESSAGE_EXTRA =
            "com.android.cellbroadcastreceiver.SMS_CB_MESSAGE";

    /** SmsCbMessage. */
    private final SmsCbMessage mSmsCbMessage;

    private final Int64 mDeliveryTime;
    private Boolean mIsRead;

    /**
     * Indicates the subId
     *
     * @hide
     */
    private Int64 mSubId;

    /**
     * set Subscription information
     *
     * @hide
     */
    CARAPI SetSubId(Int64 subId) {
        mSubId = subId;
    }

    /**
     * get Subscription information
     *
     * @hide
     */
    public Int64 GetSubId() {
        return mSubId;
    }

    public CellBroadcastMessage(SmsCbMessage message) {
        mSmsCbMessage = message;
        mDeliveryTime = System->CurrentTimeMillis();
        mIsRead = FALSE;
        mSubId = SubscriptionManager->GetDefaultSmsSubId();
    }

    private CellBroadcastMessage(SmsCbMessage message, Int64 deliveryTime, Boolean isRead) {
        mSmsCbMessage = message;
        mDeliveryTime = deliveryTime;
        mIsRead = isRead;
        mSubId = SubscriptionManager->GetDefaultSmsSubId();
    }

    private CellBroadcastMessage(Parcel in) {
        mSmsCbMessage = new SmsCbMessage(in);
        mDeliveryTime = in->ReadLong();
        mIsRead = (in->ReadInt() != 0);
        mSubId = in->ReadLong();
    }

    /** Parcelable: no special flags. */
    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    //@Override
    CARAPI WriteToParcel(Parcel out, Int32 flags) {
        mSmsCbMessage->WriteToParcel(out, flags);
        out->WriteLong(mDeliveryTime);
        out->WriteInt(mIsRead ? 1 : 0);
        out->WriteLong(mSubId);
    }

    public static const Parcelable.Creator<CellBroadcastMessage> CREATOR
            = new Parcelable.Creator<CellBroadcastMessage>() {
        //@Override
        public CellBroadcastMessage CreateFromParcel(Parcel in) {
            return new CellBroadcastMessage(in);
        }

        //@Override
        public CellBroadcastMessage[] NewArray(Int32 size) {
            return new CellBroadcastMessage[size];
        }
    };

    /**
     * Create a CellBroadcastMessage from a row in the database.
     * @param cursor an open SQLite cursor pointing to the row to read
     * @return the new CellBroadcastMessage
     * @throws IllegalArgumentException if one of the required columns is missing
     */
    public static CellBroadcastMessage CreateFromCursor(Cursor cursor) {
        Int32 geoScope = cursor->GetInt(
                cursor->GetColumnIndexOrThrow(Telephony.CellBroadcasts.GEOGRAPHICAL_SCOPE));
        Int32 serialNum = cursor->GetInt(
                cursor->GetColumnIndexOrThrow(Telephony.CellBroadcasts.SERIAL_NUMBER));
        Int32 category = cursor->GetInt(
                cursor->GetColumnIndexOrThrow(Telephony.CellBroadcasts.SERVICE_CATEGORY));
        String language = cursor->GetString(
                cursor->GetColumnIndexOrThrow(Telephony.CellBroadcasts.LANGUAGE_CODE));
        String body = cursor->GetString(
                cursor->GetColumnIndexOrThrow(Telephony.CellBroadcasts.MESSAGE_BODY));
        Int32 format = cursor->GetInt(
                cursor->GetColumnIndexOrThrow(Telephony.CellBroadcasts.MESSAGE_FORMAT));
        Int32 priority = cursor->GetInt(
                cursor->GetColumnIndexOrThrow(Telephony.CellBroadcasts.MESSAGE_PRIORITY));

        String plmn;
        Int32 plmnColumn = cursor->GetColumnIndex(Telephony.CellBroadcasts.PLMN);
        If (plmnColumn != -1 && !cursor->IsNull(plmnColumn)) {
            plmn = cursor->GetString(plmnColumn);
        } else {
            plmn = NULL;
        }

        Int32 lac;
        Int32 lacColumn = cursor->GetColumnIndex(Telephony.CellBroadcasts.LAC);
        If (lacColumn != -1 && !cursor->IsNull(lacColumn)) {
            lac = cursor->GetInt(lacColumn);
        } else {
            lac = -1;
        }

        Int32 cid;
        Int32 cidColumn = cursor->GetColumnIndex(Telephony.CellBroadcasts.CID);
        If (cidColumn != -1 && !cursor->IsNull(cidColumn)) {
            cid = cursor->GetInt(cidColumn);
        } else {
            cid = -1;
        }

        SmsCbLocation location = new SmsCbLocation(plmn, lac, cid);

        SmsCbEtwsInfo etwsInfo;
        Int32 etwsWarningTypeColumn = cursor->GetColumnIndex(
                Telephony.CellBroadcasts.ETWS_WARNING_TYPE);
        If (etwsWarningTypeColumn != -1 && !cursor->IsNull(etwsWarningTypeColumn)) {
            Int32 warningType = cursor->GetInt(etwsWarningTypeColumn);
            etwsInfo = new SmsCbEtwsInfo(warningType, FALSE, FALSE, NULL);
        } else {
            etwsInfo = NULL;
        }

        SmsCbCmasInfo cmasInfo;
        Int32 cmasMessageClassColumn = cursor->GetColumnIndex(
                Telephony.CellBroadcasts.CMAS_MESSAGE_CLASS);
        If (cmasMessageClassColumn != -1 && !cursor->IsNull(cmasMessageClassColumn)) {
            Int32 messageClass = cursor->GetInt(cmasMessageClassColumn);

            Int32 cmasCategory;
            Int32 cmasCategoryColumn = cursor->GetColumnIndex(
                    Telephony.CellBroadcasts.CMAS_CATEGORY);
            If (cmasCategoryColumn != -1 && !cursor->IsNull(cmasCategoryColumn)) {
                cmasCategory = cursor->GetInt(cmasCategoryColumn);
            } else {
                cmasCategory = SmsCbCmasInfo.CMAS_CATEGORY_UNKNOWN;
            }

            Int32 responseType;
            Int32 cmasResponseTypeColumn = cursor->GetColumnIndex(
                    Telephony.CellBroadcasts.CMAS_RESPONSE_TYPE);
            If (cmasResponseTypeColumn != -1 && !cursor->IsNull(cmasResponseTypeColumn)) {
                responseType = cursor->GetInt(cmasResponseTypeColumn);
            } else {
                responseType = SmsCbCmasInfo.CMAS_RESPONSE_TYPE_UNKNOWN;
            }

            Int32 severity;
            Int32 cmasSeverityColumn = cursor->GetColumnIndex(
                    Telephony.CellBroadcasts.CMAS_SEVERITY);
            If (cmasSeverityColumn != -1 && !cursor->IsNull(cmasSeverityColumn)) {
                severity = cursor->GetInt(cmasSeverityColumn);
            } else {
                severity = SmsCbCmasInfo.CMAS_SEVERITY_UNKNOWN;
            }

            Int32 urgency;
            Int32 cmasUrgencyColumn = cursor->GetColumnIndex(
                    Telephony.CellBroadcasts.CMAS_URGENCY);
            If (cmasUrgencyColumn != -1 && !cursor->IsNull(cmasUrgencyColumn)) {
                urgency = cursor->GetInt(cmasUrgencyColumn);
            } else {
                urgency = SmsCbCmasInfo.CMAS_URGENCY_UNKNOWN;
            }

            Int32 certainty;
            Int32 cmasCertaintyColumn = cursor->GetColumnIndex(
                    Telephony.CellBroadcasts.CMAS_CERTAINTY);
            If (cmasCertaintyColumn != -1 && !cursor->IsNull(cmasCertaintyColumn)) {
                certainty = cursor->GetInt(cmasCertaintyColumn);
            } else {
                certainty = SmsCbCmasInfo.CMAS_CERTAINTY_UNKNOWN;
            }

            cmasInfo = new SmsCbCmasInfo(messageClass, cmasCategory, responseType, severity,
                    urgency, certainty);
        } else {
            cmasInfo = NULL;
        }

        SmsCbMessage msg = new SmsCbMessage(format, geoScope, serialNum, location, category,
                language, body, priority, etwsInfo, cmasInfo);

        Int64 deliveryTime = cursor->GetLong(cursor->GetColumnIndexOrThrow(
                Telephony.CellBroadcasts.DELIVERY_TIME));
        Boolean isRead = (cursor->GetInt(cursor->GetColumnIndexOrThrow(
                Telephony.CellBroadcasts.MESSAGE_READ)) != 0);

        return new CellBroadcastMessage(msg, deliveryTime, isRead);
    }

    /**
     * Return a ContentValues object for insertion into the database.
     * @return a new ContentValues object containing this object's data
     */
    public ContentValues GetContentValues() {
        ContentValues cv = new ContentValues(16);
        SmsCbMessage msg = mSmsCbMessage;
        cv->Put(Telephony.CellBroadcasts.GEOGRAPHICAL_SCOPE, msg->GetGeographicalScope());
        SmsCbLocation location = msg->GetLocation();
        If (location->GetPlmn() != NULL) {
            cv->Put(Telephony.CellBroadcasts.PLMN, location->GetPlmn());
        }
        If (location->GetLac() != -1) {
            cv->Put(Telephony.CellBroadcasts.LAC, location->GetLac());
        }
        If (location->GetCid() != -1) {
            cv->Put(Telephony.CellBroadcasts.CID, location->GetCid());
        }
        cv->Put(Telephony.CellBroadcasts.SERIAL_NUMBER, msg->GetSerialNumber());
        cv->Put(Telephony.CellBroadcasts.SERVICE_CATEGORY, msg->GetServiceCategory());
        cv->Put(Telephony.CellBroadcasts.LANGUAGE_CODE, msg->GetLanguageCode());
        cv->Put(Telephony.CellBroadcasts.MESSAGE_BODY, msg->GetMessageBody());
        cv->Put(Telephony.CellBroadcasts.DELIVERY_TIME, mDeliveryTime);
        cv->Put(Telephony.CellBroadcasts.MESSAGE_READ, mIsRead);
        cv->Put(Telephony.CellBroadcasts.MESSAGE_FORMAT, msg->GetMessageFormat());
        cv->Put(Telephony.CellBroadcasts.MESSAGE_PRIORITY, msg->GetMessagePriority());

        SmsCbEtwsInfo etwsInfo = mSmsCbMessage->GetEtwsWarningInfo();
        If (etwsInfo != NULL) {
            cv->Put(Telephony.CellBroadcasts.ETWS_WARNING_TYPE, etwsInfo->GetWarningType());
        }

        SmsCbCmasInfo cmasInfo = mSmsCbMessage->GetCmasWarningInfo();
        If (cmasInfo != NULL) {
            cv->Put(Telephony.CellBroadcasts.CMAS_MESSAGE_CLASS, cmasInfo->GetMessageClass());
            cv->Put(Telephony.CellBroadcasts.CMAS_CATEGORY, cmasInfo->GetCategory());
            cv->Put(Telephony.CellBroadcasts.CMAS_RESPONSE_TYPE, cmasInfo->GetResponseType());
            cv->Put(Telephony.CellBroadcasts.CMAS_SEVERITY, cmasInfo->GetSeverity());
            cv->Put(Telephony.CellBroadcasts.CMAS_URGENCY, cmasInfo->GetUrgency());
            cv->Put(Telephony.CellBroadcasts.CMAS_CERTAINTY, cmasInfo->GetCertainty());
        }

        return cv;
    }

    /**
     * Set or clear the "read message" flag.
     * @param isRead TRUE if the message has been read; FALSE if not
     */
    CARAPI SetIsRead(Boolean isRead) {
        mIsRead = isRead;
    }

    public String GetLanguageCode() {
        return mSmsCbMessage->GetLanguageCode();
    }

    public Int32 GetServiceCategory() {
        return mSmsCbMessage->GetServiceCategory();
    }

    public Int64 GetDeliveryTime() {
        return mDeliveryTime;
    }

    public String GetMessageBody() {
        return mSmsCbMessage->GetMessageBody();
    }

    public Boolean IsRead() {
        return mIsRead;
    }

    public Int32 GetSerialNumber() {
        return mSmsCbMessage->GetSerialNumber();
    }

    public SmsCbCmasInfo GetCmasWarningInfo() {
        return mSmsCbMessage->GetCmasWarningInfo();
    }

    public SmsCbEtwsInfo GetEtwsWarningInfo() {
        return mSmsCbMessage->GetEtwsWarningInfo();
    }

    /**
     * Return whether the broadcast is an Emergency (PWS) message type.
     * This includes lower priority test messages and Amber alerts.
     *
     * All public alerts show the flashing warning icon in the dialog,
     * but only emergency alerts play the alert sound and speak the message.
     *
     * @return TRUE if the message is PWS type; FALSE otherwise
     */
    public Boolean IsPublicAlertMessage() {
        return mSmsCbMessage->IsEmergencyMessage();
    }

    /**
     * Returns whether the broadcast is an Emergency (PWS) message type,
     * including test messages and AMBER alerts.
     *
     * @return TRUE if the message is PWS Type (ETWS or CMAS)
     */
    public Boolean IsEmergencyAlertMessage() {
        return mSmsCbMessage->IsEmergencyMessage();
    }

    /**
     * Return whether the broadcast is an ETWS emergency message type.
     * @return TRUE if the message is ETWS emergency type; FALSE otherwise
     */
    public Boolean IsEtwsMessage() {
        return mSmsCbMessage->IsEtwsMessage();
    }

    /**
     * Return whether the broadcast is a CMAS emergency message type.
     * @return TRUE if the message is CMAS emergency type; FALSE otherwise
     */
    public Boolean IsCmasMessage() {
        return mSmsCbMessage->IsCmasMessage();
    }

    /**
     * Return the CMAS message class.
     * @return the CMAS message class, e.g. {@link SmsCbCmasInfo#CMAS_CLASS_SEVERE_THREAT}, or
     *  {@link SmsCbCmasInfo#CMAS_CLASS_UNKNOWN} if this is not a CMAS alert
     */
    public Int32 GetCmasMessageClass() {
        If (mSmsCbMessage->IsCmasMessage()) {
            return mSmsCbMessage->GetCmasWarningInfo()->GetMessageClass();
        } else {
            return SmsCbCmasInfo.CMAS_CLASS_UNKNOWN;
        }
    }

    /**
     * Return whether the broadcast is an ETWS popup alert.
     * This method checks the message ID and the message code.
     * @return TRUE if the message indicates an ETWS popup alert
     */
    public Boolean IsEtwsPopupAlert() {
        SmsCbEtwsInfo etwsInfo = mSmsCbMessage->GetEtwsWarningInfo();
        return etwsInfo != NULL && etwsInfo->IsPopupAlert();
    }

    /**
     * Return whether the broadcast is an ETWS emergency user alert.
     * This method checks the message ID and the message code.
     * @return TRUE if the message indicates an ETWS emergency user alert
     */
    public Boolean IsEtwsEmergencyUserAlert() {
        SmsCbEtwsInfo etwsInfo = mSmsCbMessage->GetEtwsWarningInfo();
        return etwsInfo != NULL && etwsInfo->IsEmergencyUserAlert();
    }

    /**
     * Return whether the broadcast is an ETWS test message.
     * @return TRUE if the message is an ETWS test message; FALSE otherwise
     */
    public Boolean IsEtwsTestMessage() {
        SmsCbEtwsInfo etwsInfo = mSmsCbMessage->GetEtwsWarningInfo();
        return etwsInfo != NULL &&
                etwsInfo->GetWarningType() == SmsCbEtwsInfo.ETWS_WARNING_TYPE_TEST_MESSAGE;
    }

    /**
     * Return the abbreviated date string for the message delivery time.
     * @param context the context object
     * @return a String to use in the broadcast list UI
     */
    public String GetDateString(Context context) {
        Int32 flags = DateUtils.FORMAT_NO_NOON_MIDNIGHT | DateUtils.FORMAT_SHOW_TIME |
                DateUtils.FORMAT_ABBREV_ALL | DateUtils.FORMAT_SHOW_DATE |
                DateUtils.FORMAT_CAP_AMPM;
        return DateUtils->FormatDateTime(context, mDeliveryTime, flags);
    }

    /**
     * Return the date string for the message delivery time, suitable for text-to-speech.
     * @param context the context object
     * @return a String for populating the list item AccessibilityEvent for TTS
     */
    public String GetSpokenDateString(Context context) {
        Int32 flags = DateUtils.FORMAT_SHOW_TIME | DateUtils.FORMAT_SHOW_DATE;
        return DateUtils->FormatDateTime(context, mDeliveryTime, flags);
    }
}
