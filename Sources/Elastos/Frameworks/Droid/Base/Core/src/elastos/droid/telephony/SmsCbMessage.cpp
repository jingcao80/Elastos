/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.telephony;

using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;

/**
 * Parcelable object containing a received cell broadcast message. There are four different types
 * of Cell Broadcast messages:
 *
 * <ul>
 * <li>opt-in informational broadcasts, e.g. news, weather, stock quotes, sports scores</li>
 * <li>cell information messages, broadcast on channel 50, indicating the current cell name for
 *  roaming Purposes (required to display on the idle screen in Brazil)</li>
 * <li>emergency broadcasts for the Japanese Earthquake and Tsunami Warning System (ETWS)</li>
 * <li>emergency broadcasts for the American Commercial Mobile Alert Service (CMAS)</li>
 * </ul>
 *
 * <p>There are also four different CB message formats: GSM, ETWS Primary Notification (GSM only),
 * UMTS, and CDMA. Some fields are only applicable for some message formats. Other fields were
 * unified under a common name, avoiding some names, such as "Message Identifier", that refer to
 * two completely different concepts in 3GPP and CDMA.
 *
 * <p>The GSM/UMTS Message Identifier field is available via {@link #getServiceCategory}, the name
 * of the equivalent field in CDMA. In both cases the service category is a 16-bit value, but 3GPP
 * and 3GPP2 have completely different meanings for the respective values. For ETWS and CMAS, the
 * application should
 *
 * <p>The CDMA Message Identifier field is available via {@link #getSerialNumber}, which is used
 * to detect the receipt of a duplicate message to be discarded. In CDMA, the message ID is
 * unique to the current PLMN. In GSM/UMTS, there is a 16-bit serial number containing a 2-bit
 * Geographical Scope field which indicates whether the 10-bit message code and 4-bit update number
 * are considered unique to the PLMN, to the current cell, or to the current Location Area (or
 * Service Area in UMTS). The relevant values are concatenated into a single String which will be
 * unique if the messages are not duplicates.
 *
 * <p>The SMS dispatcher does not detect duplicate messages. However, it does concatenate the
 * pages of a GSM multi-page cell broadcast into a single SmsCbMessage object.
 *
 * <p>Interested applications with {@code RECEIVE_SMS_PERMISSION} can register to receive
 * {@code SMS_CB_RECEIVED_ACTION} broadcast intents for incoming non-emergency broadcasts.
 * Only system applications such as the CellBroadcastReceiver may receive notifications for
 * emergency Broadcasts (ETWS and CMAS). This is intended to prevent any potential for delays or
 * interference with the immediate display of the alert message and playing of the alert sound and
 * vibration pattern, which could be caused by poorly written or malicious non-system code.
 *
 * @hide
 */
public class SmsCbMessage implements Parcelable {

    protected static const String LOG_TAG = "SMSCB";

    /** Cell wide geographical scope with immediate Display (GSM/UMTS only). */
    public static const Int32 GEOGRAPHICAL_SCOPE_CELL_WIDE_IMMEDIATE = 0;

    /** PLMN wide geographical Scope (GSM/UMTS and all CDMA broadcasts). */
    public static const Int32 GEOGRAPHICAL_SCOPE_PLMN_WIDE = 1;

    /** Location / service area wide geographical Scope (GSM/UMTS only). */
    public static const Int32 GEOGRAPHICAL_SCOPE_LA_WIDE = 2;

    /** Cell wide geographical Scope (GSM/UMTS only). */
    public static const Int32 GEOGRAPHICAL_SCOPE_CELL_WIDE = 3;

    /** GSM or UMTS format cell broadcast. */
    public static const Int32 MESSAGE_FORMAT_3GPP = 1;

    /** CDMA format cell broadcast. */
    public static const Int32 MESSAGE_FORMAT_3GPP2 = 2;

    /** Normal message priority. */
    public static const Int32 MESSAGE_PRIORITY_NORMAL = 0;

    /** Interactive message priority. */
    public static const Int32 MESSAGE_PRIORITY_INTERACTIVE = 1;

    /** Urgent message priority. */
    public static const Int32 MESSAGE_PRIORITY_URGENT = 2;

    /** Emergency message priority. */
    public static const Int32 MESSAGE_PRIORITY_EMERGENCY = 3;

    /** Format of this Message (for interpretation of service category values). */
    private final Int32 mMessageFormat;

    /** Geographical scope of broadcast. */
    private final Int32 mGeographicalScope;

    /**
     * Serial number of Broadcast (message identifier for CDMA, geographical scope + message code +
     * update number for GSM/UMTS). The serial number plus the location code uniquely identify
     * a cell broadcast for duplicate detection.
     */
    private final Int32 mSerialNumber;

    /**
     * Location identifier for this message. It consists of the current operator MCC/MNC as a
     * 5 or 6-digit decimal string. In addition, for GSM/UMTS, if the Geographical Scope of the
     * message is not binary 01, the Location Area is included for comparison. If the GS is
     * 00 or 11, the Cell ID is also included. LAC and Cell ID are -1 if not specified.
     */
    private final SmsCbLocation mLocation;

    /**
     * 16-bit CDMA service category or GSM/UMTS message identifier. For ETWS and CMAS warnings,
     * the information provided by the category is also available via {@link #GetEtwsWarningInfo()}
     * or {@link #GetCmasWarningInfo()}.
     */
    private final Int32 mServiceCategory;

    /** Message language, as a two-character string, e.g. "en". */
    private final String mLanguage;

    /** Message body, as a String. */
    private final String mBody;

    /** Message Priority (including emergency priority). */
    private final Int32 mPriority;

    /** ETWS warning notification Information (ETWS warnings only). */
    private final SmsCbEtwsInfo mEtwsWarningInfo;

    /** CMAS warning notification Information (CMAS warnings only). */
    private final SmsCbCmasInfo mCmasWarningInfo;

    /**
     * Create a new SmsCbMessage with the specified data.
     */
    public SmsCbMessage(Int32 messageFormat, Int32 geographicalScope, Int32 serialNumber,
            SmsCbLocation location, Int32 serviceCategory, String language, String body,
            Int32 priority, SmsCbEtwsInfo etwsWarningInfo, SmsCbCmasInfo cmasWarningInfo) {
        mMessageFormat = messageFormat;
        mGeographicalScope = geographicalScope;
        mSerialNumber = serialNumber;
        mLocation = location;
        mServiceCategory = serviceCategory;
        mLanguage = language;
        mBody = body;
        mPriority = priority;
        mEtwsWarningInfo = etwsWarningInfo;
        mCmasWarningInfo = cmasWarningInfo;
    }

    /** Create a new SmsCbMessage object from a Parcel. */
    public SmsCbMessage(Parcel in) {
        mMessageFormat = in->ReadInt();
        mGeographicalScope = in->ReadInt();
        mSerialNumber = in->ReadInt();
        mLocation = new SmsCbLocation(in);
        mServiceCategory = in->ReadInt();
        mLanguage = in->ReadString();
        mBody = in->ReadString();
        mPriority = in->ReadInt();
        Int32 type = in->ReadInt();
        Switch (type) {
            case 'E':
                // unparcel ETWS warning information
                mEtwsWarningInfo = new SmsCbEtwsInfo(in);
                mCmasWarningInfo = NULL;
                break;

            case 'C':
                // unparcel CMAS warning information
                mEtwsWarningInfo = NULL;
                mCmasWarningInfo = new SmsCbCmasInfo(in);
                break;

            default:
                mEtwsWarningInfo = NULL;
                mCmasWarningInfo = NULL;
        }
    }

    /**
     * Flatten this object into a Parcel.
     *
     * @param dest  The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be Written (ignored).
     */
    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteInt(mMessageFormat);
        dest->WriteInt(mGeographicalScope);
        dest->WriteInt(mSerialNumber);
        mLocation->WriteToParcel(dest, flags);
        dest->WriteInt(mServiceCategory);
        dest->WriteString(mLanguage);
        dest->WriteString(mBody);
        dest->WriteInt(mPriority);
        If (mEtwsWarningInfo != NULL) {
            // parcel ETWS warning information
            dest->WriteInt('E');
            mEtwsWarningInfo->WriteToParcel(dest, flags);
        } else If (mCmasWarningInfo != NULL) {
            // parcel CMAS warning information
            dest->WriteInt('C');
            mCmasWarningInfo->WriteToParcel(dest, flags);
        } else {
            // no ETWS or CMAS warning information
            dest->WriteInt('0');
        }
    }

    public static const Parcelable.Creator<SmsCbMessage> CREATOR
            = new Parcelable.Creator<SmsCbMessage>() {
        //@Override
        public SmsCbMessage CreateFromParcel(Parcel in) {
            return new SmsCbMessage(in);
        }

        //@Override
        public SmsCbMessage[] NewArray(Int32 size) {
            return new SmsCbMessage[size];
        }
    };

    /**
     * Return the geographical scope of this Message (GSM/UMTS only).
     *
     * @return Geographical scope
     */
    public Int32 GetGeographicalScope() {
        return mGeographicalScope;
    }

    /**
     * Return the broadcast serial number of Broadcast (message identifier for CDMA, or
     * geographical scope + message code + update number for GSM/UMTS). The serial number plus
     * the location code uniquely identify a cell broadcast for duplicate detection.
     *
     * @return the 16-bit CDMA message identifier or GSM/UMTS serial number
     */
    public Int32 GetSerialNumber() {
        return mSerialNumber;
    }

    /**
     * Return the location identifier for this message, consisting of the MCC/MNC as a
     * 5 or 6-digit decimal string. In addition, for GSM/UMTS, if the Geographical Scope of the
     * message is not binary 01, the Location Area is included. If the GS is 00 or 11, the
     * cell ID is also included. The {@link SmsCbLocation} object includes a method to test
     * if the location is included within another location area or within a PLMN and CellLocation.
     *
     * @return the geographical location code for duplicate message detection
     */
    public SmsCbLocation GetLocation() {
        return mLocation;
    }

    /**
     * Return the 16-bit CDMA service category or GSM/UMTS message identifier. The interpretation
     * of the category is radio technology specific. For ETWS and CMAS warnings, the information
     * provided by the category is available via {@link #GetEtwsWarningInfo()} or
     * {@link #GetCmasWarningInfo()} in a radio technology independent format.
     *
     * @return the radio technology specific service category
     */
    public Int32 GetServiceCategory() {
        return mServiceCategory;
    }

    /**
     * Get the ISO-639-1 language code for this message, or NULL if unspecified
     *
     * @return Language code
     */
    public String GetLanguageCode() {
        return mLanguage;
    }

    /**
     * Get the body of this message, or NULL if no body available
     *
     * @return Body, or NULL
     */
    public String GetMessageBody() {
        return mBody;
    }

    /**
     * Get the message Format ({@link #MESSAGE_FORMAT_3GPP} or {@link #MESSAGE_FORMAT_3GPP2}).
     * @return an integer representing 3GPP or 3GPP2 message format
     */
    public Int32 GetMessageFormat() {
        return mMessageFormat;
    }

    /**
     * Get the message priority. Normal broadcasts return {@link #MESSAGE_PRIORITY_NORMAL}
     * and emergency broadcasts return {@link #MESSAGE_PRIORITY_EMERGENCY}. CDMA also may return
     * {@link #MESSAGE_PRIORITY_INTERACTIVE} or {@link #MESSAGE_PRIORITY_URGENT}.
     * @return an integer representing the message priority
     */
    public Int32 GetMessagePriority() {
        return mPriority;
    }

    /**
     * If this is an ETWS warning notification then this method will return an object containing
     * the ETWS warning type, the emergency user alert flag, and the popup flag. If this is an
     * ETWS primary Notification (GSM only), there will also be a 7-Byte timestamp and 43-Byte
     * digital signature. As of Release 10, 3GPP TS 23.041 states that the UE shall ignore the
     * ETWS primary notification timestamp and digital signature if received.
     *
     * @return an SmsCbEtwsInfo object, or NULL if this is not an ETWS warning notification
     */
    public SmsCbEtwsInfo GetEtwsWarningInfo() {
        return mEtwsWarningInfo;
    }

    /**
     * If this is a CMAS warning notification then this method will return an object containing
     * the CMAS message class, category, response type, severity, urgency and certainty.
     * The message class is always present. Severity, urgency and certainty are present for CDMA
     * warning notifications containing a type 1 elements record and for GSM and UMTS warnings
     * except for the Presidential-level alert category. Category and response type are only
     * available for CDMA notifications containing a type 1 elements record.
     *
     * @return an SmsCbCmasInfo object, or NULL if this is not a CMAS warning notification
     */
    public SmsCbCmasInfo GetCmasWarningInfo() {
        return mCmasWarningInfo;
    }

    /**
     * Return whether this message is an Emergency (PWS) message type.
     * @return TRUE if the message is a public warning notification; FALSE otherwise
     */
    public Boolean IsEmergencyMessage() {
        return mPriority == MESSAGE_PRIORITY_EMERGENCY;
    }

    /**
     * Return whether this message is an ETWS warning alert.
     * @return TRUE if the message is an ETWS warning notification; FALSE otherwise
     */
    public Boolean IsEtwsMessage() {
        return mEtwsWarningInfo != NULL;
    }

    /**
     * Return whether this message is a CMAS warning alert.
     * @return TRUE if the message is a CMAS warning notification; FALSE otherwise
     */
    public Boolean IsCmasMessage() {
        return mCmasWarningInfo != NULL;
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "SmsCbMessage{geographicalScope=" + mGeographicalScope + ", serialNumber="
                + mSerialNumber + ", location=" + mLocation + ", serviceCategory="
                + mServiceCategory + ", language=" + mLanguage + ", body=" + mBody
                + ", priority=" + mPriority
                + (mEtwsWarningInfo != NULL ? (", " + mEtwsWarningInfo->ToString()) : "")
                + (mCmasWarningInfo != NULL ? (", " + mCmasWarningInfo->ToString()) : "") + '}';
    }

    /**
     * Describe the kinds of special objects contained in the marshalled representation.
     * @return a bitmask indicating this Parcelable contains no special objects
     */
    //@Override
    public Int32 DescribeContents() {
        return 0;
    }
}
