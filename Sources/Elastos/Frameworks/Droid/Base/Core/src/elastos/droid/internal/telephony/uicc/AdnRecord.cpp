/*
 * Copyright (C) 2006 The Android Open Source Project
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

package com.android.internal.telephony.uicc;

using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Utility::IArrays;


/**
 *
 * Used to load or store ADNs (Abbreviated Dialing Numbers).
 *
 * {@hide}
 *
 */
public class AdnRecord implements Parcelable {
    static const String LOG_TAG = "AdnRecord";

    //***** Instance Variables

    String mAlphaTag = NULL;
    String mNumber = NULL;
    String[] mEmails;
    String[] mAdditionalNumbers = NULL;
    Int32 mExtRecord = 0xff;
    Int32 mEfid;                   // or 0 if none
    Int32 mRecordNumber;           // or 0 if none


    //***** Constants

    // In an ADN record, everything but the alpha identifier
    // is in a footer that's 14 bytes
    static const Int32 FOOTER_SIZE_BYTES = 14;

    // Maximum size of the un-extended number field
    static const Int32 MAX_NUMBER_SIZE_BYTES = 11;

    static const Int32 EXT_RECORD_LENGTH_BYTES = 13;
    static const Int32 EXT_RECORD_TYPE_ADDITIONAL_DATA = 2;
    static const Int32 EXT_RECORD_TYPE_MASK = 3;
    static const Int32 MAX_EXT_CALLED_PARTY_LENGTH = 0xa;

    // ADN offset
    static const Int32 ADN_BCD_NUMBER_LENGTH = 0;
    static const Int32 ADN_TON_AND_NPI = 1;
    static const Int32 ADN_DIALING_NUMBER_START = 2;
    static const Int32 ADN_DIALING_NUMBER_END = 11;
    static const Int32 ADN_CAPABILITY_ID = 12;
    static const Int32 ADN_EXTENSION_ID = 13;

    //***** Static Methods

    public static const Parcelable.Creator<AdnRecord> CREATOR
            = new Parcelable.Creator<AdnRecord>() {
        //@Override
        public AdnRecord CreateFromParcel(Parcel source) {
            Int32 efid;
            Int32 recordNumber;
            String alphaTag;
            String number;
            String[] emails;
            String[] additionalNumbers;
            efid = source->ReadInt();
            recordNumber = source->ReadInt();
            alphaTag = source->ReadString();
            number = source->ReadString();
            emails = source->ReadStringArray();
            additionalNumbers = source->ReadStringArray();

            return new AdnRecord(efid, recordNumber, alphaTag, number, emails, additionalNumbers);
        }

        //@Override
        public AdnRecord[] NewArray(Int32 size) {
            return new AdnRecord[size];
        }
    };


    //***** Constructor
    public AdnRecord (Byte[] record) {
        This(0, 0, record);
    }

    public AdnRecord (Int32 efid, Int32 recordNumber, Byte[] record) {
        this.mEfid = efid;
        this.mRecordNumber = recordNumber;
        ParseRecord(record);
    }

    public AdnRecord (String alphaTag, String number) {
        This(0, 0, alphaTag, number);
    }

    public AdnRecord (String alphaTag, String number, String[] emails) {
        This(0, 0, alphaTag, number, emails);
    }

    public AdnRecord(String alphaTag, String number, String[] emails, String[] additionalNumbers) {
        This(0, 0, alphaTag, number, emails, additionalNumbers);
    }

    public AdnRecord (Int32 efid, Int32 recordNumber, String alphaTag, String number, String[] emails) {
        this.mEfid = efid;
        this.mRecordNumber = recordNumber;
        this.mAlphaTag = alphaTag;
        this.mNumber = number;
        this.mEmails = emails;
        this.mAdditionalNumbers = NULL;
    }

    public AdnRecord(Int32 efid, Int32 recordNumber, String alphaTag, String number, String[] emails,
            String[] additionalNumbers) {
        this.mEfid = efid;
        this.mRecordNumber = recordNumber;
        this.mAlphaTag = alphaTag;
        this.mNumber = number;
        this.mEmails = emails;
        this.mAdditionalNumbers = additionalNumbers;
    }

    public AdnRecord(Int32 efid, Int32 recordNumber, String alphaTag, String number) {
        this.mEfid = efid;
        this.mRecordNumber = recordNumber;
        this.mAlphaTag = alphaTag;
        this.mNumber = number;
        this.mEmails = NULL;
        this.mAdditionalNumbers = NULL;
    }

    //***** Instance Methods

    public String GetAlphaTag() {
        return mAlphaTag;
    }

    public String GetNumber() {
        return mNumber;
    }

    public String[] GetEmails() {
        return mEmails;
    }

    CARAPI SetEmails(String[] emails) {
        this.mEmails = emails;
    }

    public String[] GetAdditionalNumbers() {
        return mAdditionalNumbers;
    }

    CARAPI SetAdditionalNumbers(String[] additionalNumbers) {
        this.mAdditionalNumbers = additionalNumbers;
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "ADN Record '" + mAlphaTag + "' '" + mNumber + " " + mEmails + " "
                + mAdditionalNumbers + "'";
    }

    public Boolean IsEmpty() {
        return TextUtils->IsEmpty(mAlphaTag) && TextUtils->IsEmpty(mNumber) && mEmails == NULL
                && mAdditionalNumbers == NULL;
    }

    public Boolean HasExtendedRecord() {
        return mExtRecord != 0 && mExtRecord != 0xff;
    }

    /** Helper function for {@link #isEqual}. */
    private static Boolean StringCompareNullEqualsEmpty(String s1, String s2) {
        If (s1 == s2) {
            return TRUE;
        }
        If (s1 == NULL) {
            s1 = "";
        }
        If (s2 == NULL) {
            s2 = "";
        }
        Return (s1->Equals(s2));
    }

    /** Help function for ANR/EMAIL array compare. */
    private static Boolean ArrayCompareNullEqualsEmpty(String s1[], String s2[]) {
        If (s1 == s2) {
            return TRUE;
        }

        If (s1 == NULL) {
            s1 = new String[1];
            s1[0] = "";
        }

        If (s2 == NULL) {
            s2 = new String[1];
            s2[0] = "";
        }

        For (String str:s1) {
            If (TextUtils->IsEmpty(str)) {
                continue;
            } else {
                If (Arrays->AsList(s2).Contains(str)) {
                    continue;
                } else {
                    return FALSE;
                }
            }
        }

        For (String str:s2) {
            If (TextUtils->IsEmpty(str)) {
                continue;
            } else {
                If (Arrays->AsList(s1).Contains(str)) {
                    continue;
                } else {
                    return FALSE;
                }
            }
        }

        return TRUE;
    }

    public Boolean IsEqual(AdnRecord adn) {
        Return ( StringCompareNullEqualsEmpty(mAlphaTag, adn.mAlphaTag) &&
                StringCompareNullEqualsEmpty(mNumber, adn.mNumber) &&
                ArrayCompareNullEqualsEmpty(mEmails, adn.mEmails)
                && ArrayCompareNullEqualsEmpty(mAdditionalNumbers, adn.mAdditionalNumbers));
    }

    public String[] UpdateAnrEmailArrayHelper(String dest[], String src[], Int32 fileCount) {
        If (fileCount == 0) {
            return NULL;
        }

        // delete insert scenario
        If (dest == NULL || src == NULL) {
            return dest;
        }

        String[] ref = new String[fileCount];
        For (Int32 i = 0; i < fileCount; i++) {
            ref[i] = "";
        }

        // Find common elements and put in the ref
        // To save SIM_IO
        For (Int32 i = 0; i < src.length; i++) {
            If (TextUtils->IsEmpty(src[i])) {
                continue;
            }
            For (Int32 j = 0; j < dest.length; j++) {
                If (src[i].Equals(dest[j])) {
                    ref[i] = src[i];
                    break;
                }
            }
        }

        // fill out none common element into the ""
        For (Int32 i = 0; i < dest.length; i++) {
            If (Arrays->AsList(ref).Contains(dest[i])) {
                continue;
            } else {
                For (Int32 j = 0; j < ref.length; j++) {
                    If (TextUtils->IsEmpty(ref[j])) {
                        ref[j] = dest[i];
                        break;
                    }
                }
            }
        }
        return ref;
    }

    CARAPI UpdateAnrEmailArray(AdnRecord adn, Int32 emailFileNum, Int32 anrFileNum) {
        mEmails = UpdateAnrEmailArrayHelper(mEmails, adn.mEmails, emailFileNum);
        mAdditionalNumbers = UpdateAnrEmailArrayHelper(mAdditionalNumbers,
                    adn.mAdditionalNumbers, anrFileNum);
    }
    //***** Parcelable Implementation

    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteInt(mEfid);
        dest->WriteInt(mRecordNumber);
        dest->WriteString(mAlphaTag);
        dest->WriteString(mNumber);
        dest->WriteStringArray(mEmails);
        dest->WriteStringArray(mAdditionalNumbers);
    }

    /**
     * Build adn hex Byte array based on record size
     * The format of Byte array is defined in 51.011 10.5.1
     *
     * @param recordSize is the size X of EF record
     * @return hex Byte[recordSize] to be written to EF record
     *          return NULL for wrong format of dialing number or tag
     */
    public Byte[] BuildAdnString(Int32 recordSize) {
        Byte[] bcdNumber;
        Byte[] byteTag;
        Byte[] adnString;
        Int32 footerOffset = recordSize - FOOTER_SIZE_BYTES;

        // create an empty record
        adnString = new Byte[recordSize];
        For (Int32 i = 0; i < recordSize; i++) {
            adnString[i] = (Byte) 0xFF;
        }

        If ((TextUtils->IsEmpty(mNumber)) && (TextUtils->IsEmpty(mAlphaTag))) {
            Rlog->W(LOG_TAG, "[buildAdnString] Empty dialing number");
            return adnString;   // return the empty Record (for delete)
        } else If ((mNumber != NULL) && (mNumber->Length()
                > (ADN_DIALING_NUMBER_END - ADN_DIALING_NUMBER_START + 1) * 2)) {
            Rlog->W(LOG_TAG,
                    "[buildAdnString] Max length of dialing number is 20");
            return NULL;
        } else If (mAlphaTag != NULL && mAlphaTag->Length() > footerOffset) {
            Rlog->W(LOG_TAG,
                    "[buildAdnString] Max length of tag is " + footerOffset);
            return NULL;
        } else {
            If (!(TextUtils->IsEmpty(mNumber))) {
                bcdNumber = PhoneNumberUtils->NumberToCalledPartyBCD(mNumber);

            System->Arraycopy(bcdNumber, 0, adnString,
                    footerOffset + ADN_TON_AND_NPI, bcdNumber.length);

                adnString[footerOffset + ADN_BCD_NUMBER_LENGTH]
                        = (Byte) (bcdNumber.length);
            }
            adnString[footerOffset + ADN_CAPABILITY_ID]
                    = (Byte) 0xFF; // Capability Id
            adnString[footerOffset + ADN_EXTENSION_ID]
                    = (Byte) 0xFF; // Extension Record Id

            If (!TextUtils->IsEmpty(mAlphaTag)) {
                byteTag = IccUtils->StringToAdnStringField(mAlphaTag);
                System->Arraycopy(byteTag, 0, adnString, 0, byteTag.length);
            }

            return adnString;
        }
    }

    /**
     * See TS 51.011 10.5.10
     */
    CARAPI
    AppendExtRecord (Byte[] extRecord) {
        try {
            If (extRecord.length != EXT_RECORD_LENGTH_BYTES) {
                return;
            }

            If ((extRecord[0] & EXT_RECORD_TYPE_MASK)
                    != EXT_RECORD_TYPE_ADDITIONAL_DATA) {
                return;
            }

            If ((0xff & extRecord[1]) > MAX_EXT_CALLED_PARTY_LENGTH) {
                // invalid or empty record
                return;
            }

            mNumber += PhoneNumberUtils->CalledPartyBCDFragmentToString(
                                        extRecord, 2, 0xff & extRecord[1]);

            // We don't support ext record chaining.

        } Catch (RuntimeException ex) {
            Rlog->W(LOG_TAG, "Error parsing AdnRecord ext record", ex);
        }
    }

    //***** Private Methods

    /**
     * alphaTag and number are set to NULL on invalid format
     */
    private void
    ParseRecord(Byte[] record) {
        try {
            mAlphaTag = IccUtils->AdnStringFieldToString(
                            record, 0, record.length - FOOTER_SIZE_BYTES);

            Int32 footerOffset = record.length - FOOTER_SIZE_BYTES;

            Int32 numberLength = 0xff & record[footerOffset];

            If (numberLength > MAX_NUMBER_SIZE_BYTES) {
                // Invalid number length
                mNumber = "";
                return;
            }

            // Please note 51.011 10.5.1:
            //
            // "If the Dialling Number/SSC String does not contain
            // a dialling number, e.g. a control string deactivating
            // a service, the TON/NPI Byte shall be set to 'FF' by
            // the ME (see note 2)."

            mNumber = PhoneNumberUtils->CalledPartyBCDToString(
                            record, footerOffset + 1, numberLength);


            mExtRecord = 0xff & record[record.length - 1];

            mEmails = NULL;
            mAdditionalNumbers = NULL;

        } Catch (RuntimeException ex) {
            Rlog->W(LOG_TAG, "Error parsing AdnRecord", ex);
            mNumber = "";
            mAlphaTag = "";
            mEmails = NULL;
            mAdditionalNumbers = NULL;
        }
    }

    public String[] GetAnrNumbers() {
        return GetAdditionalNumbers();
    }
}
