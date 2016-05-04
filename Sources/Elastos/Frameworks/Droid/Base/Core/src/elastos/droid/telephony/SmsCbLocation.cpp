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
 * Represents the location and geographical scope of a cell broadcast message.
 * For GSM/UMTS, the Location Area and Cell ID are set when the broadcast
 * geographical scope is cell wide or Location Area wide. For CDMA, the
 * broadcast geographical scope is always PLMN wide.
 *
 * @hide
 */
public class SmsCbLocation implements Parcelable {

    /** The PLMN. Note that this field may be an empty string, but isn't allowed to be NULL. */
    private final String mPlmn;

    private final Int32 mLac;
    private final Int32 mCid;

    /**
     * Construct an empty location object. This is used for some test cases, and for
     * cell broadcasts saved in older versions of the database without location info.
     */
    public SmsCbLocation() {
        mPlmn = "";
        mLac = -1;
        mCid = -1;
    }

    /**
     * Construct a location object for the PLMN. This class is immutable, so
     * the same object can be reused for multiple broadcasts.
     */
    public SmsCbLocation(String plmn) {
        mPlmn = plmn;
        mLac = -1;
        mCid = -1;
    }

    /**
     * Construct a location object for the PLMN, LAC, and Cell ID. This class is immutable, so
     * the same object can be reused for multiple broadcasts.
     */
    public SmsCbLocation(String plmn, Int32 lac, Int32 cid) {
        mPlmn = plmn;
        mLac = lac;
        mCid = cid;
    }

    /**
     * Initialize the object from a Parcel.
     */
    public SmsCbLocation(Parcel in) {
        mPlmn = in->ReadString();
        mLac = in->ReadInt();
        mCid = in->ReadInt();
    }

    /**
     * Returns the MCC/MNC of the network as a String.
     * @return the PLMN Identifier (MCC+MNC) as a String
     */
    public String GetPlmn() {
        return mPlmn;
    }

    /**
     * Returns the GSM location area code, or UMTS service area code.
     * @return location area code, -1 if unknown, 0xffff max legal value
     */
    public Int32 GetLac() {
        return mLac;
    }

    /**
     * Returns the GSM or UMTS cell ID.
     * @return gsm cell id, -1 if unknown, 0xffff max legal value
     */
    public Int32 GetCid() {
        return mCid;
    }

    //@Override
    public Int32 HashCode() {
        Int32 hash = mPlmn->HashCode();
        hash = hash * 31 + mLac;
        hash = hash * 31 + mCid;
        return hash;
    }

    //@Override
    public Boolean Equals(Object o) {
        If (o == this) {
            return TRUE;
        }
        If (o == NULL || !(o instanceof SmsCbLocation)) {
            return FALSE;
        }
        SmsCbLocation other = (SmsCbLocation) o;
        return mPlmn->Equals(other.mPlmn) && mLac == other.mLac && mCid == other.mCid;
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return '[' + mPlmn + ',' + mLac + ',' + mCid + ']';
    }

    /**
     * Test whether this location is within the location area of the specified object.
     *
     * @param area the location area to compare with this location
     * @return TRUE if this location is contained within the specified location area
     */
    public Boolean IsInLocationArea(SmsCbLocation area) {
        If (mCid != -1 && mCid != area.mCid) {
            return FALSE;
        }
        If (mLac != -1 && mLac != area.mLac) {
            return FALSE;
        }
        return mPlmn->Equals(area.mPlmn);
    }

    /**
     * Test whether this location is within the location area of the CellLocation.
     *
     * @param plmn the PLMN to use for comparison
     * @param lac the Location Area (GSM) or Service Area (UMTS) to compare with
     * @param cid the Cell ID to compare with
     * @return TRUE if this location is contained within the specified PLMN, LAC, and Cell ID
     */
    public Boolean IsInLocationArea(String plmn, Int32 lac, Int32 cid) {
        If (!mPlmn->Equals(plmn)) {
            return FALSE;
        }

        If (mLac != -1 && mLac != lac) {
            return FALSE;
        }

        If (mCid != -1 && mCid != cid) {
            return FALSE;
        }

        return TRUE;
    }

    /**
     * Flatten this object into a Parcel.
     *
     * @param dest  The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be Written (ignored).
     */
    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteString(mPlmn);
        dest->WriteInt(mLac);
        dest->WriteInt(mCid);
    }

    public static const Parcelable.Creator<SmsCbLocation> CREATOR
            = new Parcelable.Creator<SmsCbLocation>() {
        //@Override
        public SmsCbLocation CreateFromParcel(Parcel in) {
            return new SmsCbLocation(in);
        }

        //@Override
        public SmsCbLocation[] NewArray(Int32 size) {
            return new SmsCbLocation[size];
        }
    };

    /**
     * Describe the kinds of special objects contained in the marshalled representation.
     * @return a bitmask indicating this Parcelable contains no special objects
     */
    //@Override
    public Int32 DescribeContents() {
        return 0;
    }
}
