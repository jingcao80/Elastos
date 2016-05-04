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

/**
 * Contains CMAS warning notification Type 1 elements for a {@link SmsCbMessage}.
 * Supported values for each element are defined in TIA-1149-0-1 (CMAS over CDMA) and
 * 3GPP TS 23.041 (for GSM/UMTS).
 *
 * {@hide}
 */
public class SmsCbCmasInfo implements Parcelable {

    // CMAS message Class (in GSM/UMTS message identifier or CDMA service category).

    /** Presidential-level Alert (Korean Public Alert System Class 0 message). */
    public static const Int32 CMAS_CLASS_PRESIDENTIAL_LEVEL_ALERT = 0x00;

    /** Extreme threat to life and Property (Korean Public Alert System Class 1 message). */
    public static const Int32 CMAS_CLASS_EXTREME_THREAT = 0x01;

    /** Severe threat to life and Property (Korean Public Alert System Class 1 message). */
    public static const Int32 CMAS_CLASS_SEVERE_THREAT = 0x02;

    /** Child abduction Emergency (AMBER Alert). */
    public static const Int32 CMAS_CLASS_CHILD_ABDUCTION_EMERGENCY = 0x03;

    /** CMAS test message. */
    public static const Int32 CMAS_CLASS_REQUIRED_MONTHLY_TEST = 0x04;

    /** CMAS exercise. */
    public static const Int32 CMAS_CLASS_CMAS_EXERCISE = 0x05;

    /** CMAS category for operator defined use. */
    public static const Int32 CMAS_CLASS_OPERATOR_DEFINED_USE = 0x06;

    /** CMAS category for warning types that are reserved for future extension. */
    public static const Int32 CMAS_CLASS_UNKNOWN = -1;

    // CMAS alert Category (in CDMA type 1 elements record).

    /** CMAS alert category: Geophysical including landslide. */
    public static const Int32 CMAS_CATEGORY_GEO = 0x00;

    /** CMAS alert category: Meteorological including flood. */
    public static const Int32 CMAS_CATEGORY_MET = 0x01;

    /** CMAS alert category: General emergency and public safety. */
    public static const Int32 CMAS_CATEGORY_SAFETY = 0x02;

    /** CMAS alert category: Law enforcement, military, homeland/local/private security. */
    public static const Int32 CMAS_CATEGORY_SECURITY = 0x03;

    /** CMAS alert category: Rescue and recovery. */
    public static const Int32 CMAS_CATEGORY_RESCUE = 0x04;

    /** CMAS alert category: Fire suppression and rescue. */
    public static const Int32 CMAS_CATEGORY_FIRE = 0x05;

    /** CMAS alert category: Medical and public health. */
    public static const Int32 CMAS_CATEGORY_HEALTH = 0x06;

    /** CMAS alert category: Pollution and other environmental. */
    public static const Int32 CMAS_CATEGORY_ENV = 0x07;

    /** CMAS alert category: Public and private transportation. */
    public static const Int32 CMAS_CATEGORY_TRANSPORT = 0x08;

    /** CMAS alert category: Utility, telecom, other non-transport infrastructure. */
    public static const Int32 CMAS_CATEGORY_INFRA = 0x09;

    /** CMAS alert category: Chem, bio, radiological, nuclear, high explosive threat or attack. */
    public static const Int32 CMAS_CATEGORY_CBRNE = 0x0a;

    /** CMAS alert category: Other events. */
    public static const Int32 CMAS_CATEGORY_OTHER = 0x0b;

    /**
     * CMAS alert category is unknown. The category is only available for CDMA broadcasts
     * containing a type 1 elements record, so GSM and UMTS broadcasts always return unknown.
     */
    public static const Int32 CMAS_CATEGORY_UNKNOWN = -1;

    // CMAS response Type (in CDMA type 1 elements record).

    /** CMAS response type: Take shelter in place. */
    public static const Int32 CMAS_RESPONSE_TYPE_SHELTER = 0x00;

    /** CMAS response type: Evacuate (Relocate). */
    public static const Int32 CMAS_RESPONSE_TYPE_EVACUATE = 0x01;

    /** CMAS response type: Make preparations. */
    public static const Int32 CMAS_RESPONSE_TYPE_PREPARE = 0x02;

    /** CMAS response type: Execute a pre-planned activity. */
    public static const Int32 CMAS_RESPONSE_TYPE_EXECUTE = 0x03;

    /** CMAS response type: Attend to information sources. */
    public static const Int32 CMAS_RESPONSE_TYPE_MONITOR = 0x04;

    /** CMAS response type: Avoid hazard. */
    public static const Int32 CMAS_RESPONSE_TYPE_AVOID = 0x05;

    /** CMAS response type: Evaluate the information in this Message (not for public warnings). */
    public static const Int32 CMAS_RESPONSE_TYPE_ASSESS = 0x06;

    /** CMAS response type: No action recommended. */
    public static const Int32 CMAS_RESPONSE_TYPE_NONE = 0x07;

    /**
     * CMAS response type is unknown. The response type is only available for CDMA broadcasts
     * containing a type 1 elements record, so GSM and UMTS broadcasts always return unknown.
     */
    public static const Int32 CMAS_RESPONSE_TYPE_UNKNOWN = -1;

    // 4-bit CMAS Severity (in GSM/UMTS message identifier or CDMA type 1 elements record).

    /** CMAS severity type: Extraordinary threat to life or property. */
    public static const Int32 CMAS_SEVERITY_EXTREME = 0x0;

    /** CMAS severity type: Significant threat to life or property. */
    public static const Int32 CMAS_SEVERITY_SEVERE = 0x1;

    /**
     * CMAS alert severity is unknown. The severity is available for CDMA warning alerts
     * containing a type 1 elements record and for all GSM and UMTS alerts except for the
     * Presidential-level alert Class (Korean Public Alert System Class 0).
     */
    public static const Int32 CMAS_SEVERITY_UNKNOWN = -1;

    // CMAS Urgency (in GSM/UMTS message identifier or CDMA type 1 elements record).

    /** CMAS urgency type: Responsive action should be taken immediately. */
    public static const Int32 CMAS_URGENCY_IMMEDIATE = 0x0;

    /** CMAS urgency type: Responsive action should be taken within the next hour. */
    public static const Int32 CMAS_URGENCY_EXPECTED = 0x1;

    /**
     * CMAS alert urgency is unknown. The urgency is available for CDMA warning alerts
     * containing a type 1 elements record and for all GSM and UMTS alerts except for the
     * Presidential-level alert Class (Korean Public Alert System Class 0).
     */
    public static const Int32 CMAS_URGENCY_UNKNOWN = -1;

    // CMAS Certainty (in GSM/UMTS message identifier or CDMA type 1 elements record).

    /** CMAS certainty type: Determined to have occurred or to be ongoing. */
    public static const Int32 CMAS_CERTAINTY_OBSERVED = 0x0;

    /** CMAS certainty type: Likely (probability > ~50%). */
    public static const Int32 CMAS_CERTAINTY_LIKELY = 0x1;

    /**
     * CMAS alert certainty is unknown. The certainty is available for CDMA warning alerts
     * containing a type 1 elements record and for all GSM and UMTS alerts except for the
     * Presidential-level alert Class (Korean Public Alert System Class 0).
     */
    public static const Int32 CMAS_CERTAINTY_UNKNOWN = -1;

    /** CMAS message class. */
    private final Int32 mMessageClass;

    /** CMAS category. */
    private final Int32 mCategory;

    /** CMAS response type. */
    private final Int32 mResponseType;

    /** CMAS severity. */
    private final Int32 mSeverity;

    /** CMAS urgency. */
    private final Int32 mUrgency;

    /** CMAS certainty. */
    private final Int32 mCertainty;

    /** Create a new SmsCbCmasInfo object with the specified values. */
    public SmsCbCmasInfo(Int32 messageClass, Int32 category, Int32 responseType, Int32 severity,
            Int32 urgency, Int32 certainty) {
        mMessageClass = messageClass;
        mCategory = category;
        mResponseType = responseType;
        mSeverity = severity;
        mUrgency = urgency;
        mCertainty = certainty;
    }

    /** Create a new SmsCbCmasInfo object from a Parcel. */
    SmsCbCmasInfo(Parcel in) {
        mMessageClass = in->ReadInt();
        mCategory = in->ReadInt();
        mResponseType = in->ReadInt();
        mSeverity = in->ReadInt();
        mUrgency = in->ReadInt();
        mCertainty = in->ReadInt();
    }

    /**
     * Flatten this object into a Parcel.
     *
     * @param dest  The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be Written (ignored).
     */
    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteInt(mMessageClass);
        dest->WriteInt(mCategory);
        dest->WriteInt(mResponseType);
        dest->WriteInt(mSeverity);
        dest->WriteInt(mUrgency);
        dest->WriteInt(mCertainty);
    }

    /**
     * Returns the CMAS message class, e.g. {@link #CMAS_CLASS_PRESIDENTIAL_LEVEL_ALERT}.
     * @return one of the {@code CMAS_CLASS} values
     */
    public Int32 GetMessageClass() {
        return mMessageClass;
    }

    /**
     * Returns the CMAS category, e.g. {@link #CMAS_CATEGORY_GEO}.
     * @return one of the {@code CMAS_CATEGORY} values
     */
    public Int32 GetCategory() {
        return mCategory;
    }

    /**
     * Returns the CMAS response type, e.g. {@link #CMAS_RESPONSE_TYPE_SHELTER}.
     * @return one of the {@code CMAS_RESPONSE_TYPE} values
     */
    public Int32 GetResponseType() {
        return mResponseType;
    }

    /**
     * Returns the CMAS severity, e.g. {@link #CMAS_SEVERITY_EXTREME}.
     * @return one of the {@code CMAS_SEVERITY} values
     */
    public Int32 GetSeverity() {
        return mSeverity;
    }

    /**
     * Returns the CMAS urgency, e.g. {@link #CMAS_URGENCY_IMMEDIATE}.
     * @return one of the {@code CMAS_URGENCY} values
     */
    public Int32 GetUrgency() {
        return mUrgency;
    }

    /**
     * Returns the CMAS certainty, e.g. {@link #CMAS_CERTAINTY_OBSERVED}.
     * @return one of the {@code CMAS_CERTAINTY} values
     */
    public Int32 GetCertainty() {
        return mCertainty;
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "SmsCbCmasInfo{messageClass=" + mMessageClass + ", category=" + mCategory
                + ", responseType=" + mResponseType + ", severity=" + mSeverity
                + ", urgency=" + mUrgency + ", certainty=" + mCertainty + '}';
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
    public static const Parcelable.Creator<SmsCbCmasInfo>
            CREATOR = new Parcelable.Creator<SmsCbCmasInfo>() {
        //@Override
        public SmsCbCmasInfo CreateFromParcel(Parcel in) {
            return new SmsCbCmasInfo(in);
        }

        //@Override
        public SmsCbCmasInfo[] NewArray(Int32 size) {
            return new SmsCbCmasInfo[size];
        }
    };
}
