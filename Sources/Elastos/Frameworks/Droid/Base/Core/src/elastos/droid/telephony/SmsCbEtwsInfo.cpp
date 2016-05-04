/*
 * Copyright (C) 2012 The Android Open Source Project
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
using Elastos::Droid::Text::Format::ITime;

using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;

using Elastos::Utility::IArrays;

/**
 * Contains information elements for a GSM or UMTS ETWS warning notification.
 * Supported values for each element are defined in 3GPP TS 23.041.
 *
 * {@hide}
 */
public class SmsCbEtwsInfo implements Parcelable {

    /** ETWS warning type for earthquake. */
    public static const Int32 ETWS_WARNING_TYPE_EARTHQUAKE = 0x00;

    /** ETWS warning type for tsunami. */
    public static const Int32 ETWS_WARNING_TYPE_TSUNAMI = 0x01;

    /** ETWS warning type for earthquake and tsunami. */
    public static const Int32 ETWS_WARNING_TYPE_EARTHQUAKE_AND_TSUNAMI = 0x02;

    /** ETWS warning type for test messages. */
    public static const Int32 ETWS_WARNING_TYPE_TEST_MESSAGE = 0x03;

    /** ETWS warning type for other emergency types. */
    public static const Int32 ETWS_WARNING_TYPE_OTHER_EMERGENCY = 0x04;

    /** Unknown ETWS warning type. */
    public static const Int32 ETWS_WARNING_TYPE_UNKNOWN = -1;

    /** One of the ETWS warning type constants defined in this class. */
    private final Int32 mWarningType;

    /** Whether or not to activate the emergency user alert tone and vibration. */
    private final Boolean mEmergencyUserAlert;

    /** Whether or not to activate a popup alert. */
    private final Boolean mActivatePopup;

    /**
     * 50-Byte security Information (ETWS primary notification for GSM only). As of Release 10,
     * 3GPP TS 23.041 states that the UE shall ignore the ETWS primary notification timestamp
     * and digital signature if received. Therefore it is treated as a raw Byte array and
     * parceled with the broadcast intent if present, but the timestamp is only computed if an
     * application asks for the individual components.
     */
    private final Byte[] mWarningSecurityInformation;

    /** Create a new SmsCbEtwsInfo object with the specified values. */
    public SmsCbEtwsInfo(Int32 warningType, Boolean emergencyUserAlert, Boolean activatePopup,
            Byte[] warningSecurityInformation) {
        mWarningType = warningType;
        mEmergencyUserAlert = emergencyUserAlert;
        mActivatePopup = activatePopup;
        mWarningSecurityInformation = warningSecurityInformation;
    }

    /** Create a new SmsCbEtwsInfo object from a Parcel. */
    SmsCbEtwsInfo(Parcel in) {
        mWarningType = in->ReadInt();
        mEmergencyUserAlert = (in->ReadInt() != 0);
        mActivatePopup = (in->ReadInt() != 0);
        mWarningSecurityInformation = in->CreateByteArray();
    }

    /**
     * Flatten this object into a Parcel.
     *
     * @param dest  The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be Written (ignored).
     */
    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteInt(mWarningType);
        dest->WriteInt(mEmergencyUserAlert ? 1 : 0);
        dest->WriteInt(mActivatePopup ? 1 : 0);
        dest->WriteByteArray(mWarningSecurityInformation);
    }

    /**
     * Returns the ETWS warning type.
     * @return a warning type such as {@link #ETWS_WARNING_TYPE_EARTHQUAKE}
     */
    public Int32 GetWarningType() {
        return mWarningType;
    }

    /**
     * Returns the ETWS emergency user alert flag.
     * @return TRUE to notify terminal to activate emergency user alert; FALSE otherwise
     */
    public Boolean IsEmergencyUserAlert() {
        return mEmergencyUserAlert;
    }

    /**
     * Returns the ETWS activate popup flag.
     * @return TRUE to notify terminal to activate display popup; FALSE otherwise
     */
    public Boolean IsPopupAlert() {
        return mActivatePopup;
    }

    /**
     * Returns the Warning-Security-Information Timestamp (GSM primary notifications only).
     * As of Release 10, 3GPP TS 23.041 states that the UE shall ignore this value if received.
     * @return a UTC timestamp in System->CurrentTimeMillis() format, or 0 if not present
     */
    public Int64 GetPrimaryNotificationTimestamp() {
        If (mWarningSecurityInformation == NULL || mWarningSecurityInformation.length < 7) {
            return 0;
        }

        Int32 year = IccUtils->GsmBcdByteToInt(mWarningSecurityInformation[0]);
        Int32 month = IccUtils->GsmBcdByteToInt(mWarningSecurityInformation[1]);
        Int32 day = IccUtils->GsmBcdByteToInt(mWarningSecurityInformation[2]);
        Int32 hour = IccUtils->GsmBcdByteToInt(mWarningSecurityInformation[3]);
        Int32 minute = IccUtils->GsmBcdByteToInt(mWarningSecurityInformation[4]);
        Int32 second = IccUtils->GsmBcdByteToInt(mWarningSecurityInformation[5]);

        // For the timezone, the most significant bit of the
        // least significant nibble is the sign Byte
        // (meaning the max range of this field is 79 quarter-hours,
        // which is more than enough)

        Byte tzByte = mWarningSecurityInformation[6];

        // Mask out sign bit.
        Int32 timezoneOffset = IccUtils->GsmBcdByteToInt((Byte) (tzByte & (~0x08)));

        timezoneOffset = ((tzByte & 0x08) == 0) ? timezoneOffset : -timezoneOffset;

        Time time = new Time(Time.TIMEZONE_UTC);

        // We only need to support years above 2000.
        time.year = year + 2000;
        time.month = month - 1;
        time.monthDay = day;
        time.hour = hour;
        time.minute = minute;
        time.second = second;

        // Timezone offset is in quarter hours.
        return time->ToMillis(TRUE) - timezoneOffset * 15 * 60 * 1000;
    }

    /**
     * Returns the digital Signature (GSM primary notifications only). As of Release 10,
     * 3GPP TS 23.041 states that the UE shall ignore this value if received.
     * @return a Byte array containing a copy of the primary notification digital signature
     */
    public Byte[] GetPrimaryNotificationSignature() {
        If (mWarningSecurityInformation == NULL || mWarningSecurityInformation.length < 50) {
            return NULL;
        }
        return Arrays->CopyOfRange(mWarningSecurityInformation, 7, 50);
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "SmsCbEtwsInfo{warningType=" + mWarningType + ", emergencyUserAlert="
                + mEmergencyUserAlert + ", activatePopup=" + mActivatePopup + '}';
    }

    /**
     * Describe the kinds of special objects contained in the marshalled representation.
     * @return a bitmask indicating this Parcelable contains no special objects
     */
    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    /** Creator for unparcelling objects. */
    public static const Creator<SmsCbEtwsInfo> CREATOR = new Creator<SmsCbEtwsInfo>() {
        //@Override
        public SmsCbEtwsInfo CreateFromParcel(Parcel in) {
            return new SmsCbEtwsInfo(in);
        }

        //@Override
        public SmsCbEtwsInfo[] NewArray(Int32 size) {
            return new SmsCbEtwsInfo[size];
        }
    };
}
