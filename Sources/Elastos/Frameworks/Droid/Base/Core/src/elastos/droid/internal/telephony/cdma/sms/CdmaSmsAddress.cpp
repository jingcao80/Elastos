/*
 * Copyright (C) 2008 The Android Open Source Project
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

package com.android.internal.telephony.cdma.sms;

using Elastos::Droid::Utility::ISparseBooleanArray;

using Elastos::Droid::Internal::Telephony::ISmsAddress;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::IUserData;
using Elastos::Droid::Internal::Utility::IHexDump;

public class CdmaSmsAddress extends SmsAddress {

    /**
     * Digit Mode Indicator is a 1-bit value that indicates whether
     * the address digits are 4-bit DTMF codes or 8-bit codes.  (See
     * 3GPP2 C.S0015-B, v2, 3.4.3.3)
     */
    static public final Int32 DIGIT_MODE_4BIT_DTMF              = 0x00;
    static public final Int32 DIGIT_MODE_8BIT_CHAR              = 0x01;

    public Int32 digitMode;

    /**
     * Number Mode Indicator is 1-bit value that indicates whether the
     * address type is a data network address or not.  (See 3GPP2
     * C.S0015-B, v2, 3.4.3.3)
     */
    static public final Int32 NUMBER_MODE_NOT_DATA_NETWORK      = 0x00;
    static public final Int32 NUMBER_MODE_DATA_NETWORK          = 0x01;

    public Int32 numberMode;

    /**
     * Number Types for data networks.
     * (See 3GPP2 C.S005-D, table2.7.1.3.2.4-2 for complete table)
     * (See 3GPP2 C.S0015-B, v2, 3.4.3.3 for data network subset)
     * NOTE: value is stored in the parent class ton field.
     */
    static public final Int32 TON_UNKNOWN                   = 0x00;
    static public final Int32 TON_INTERNATIONAL_OR_IP       = 0x01;
    static public final Int32 TON_NATIONAL_OR_EMAIL         = 0x02;
    static public final Int32 TON_NETWORK                   = 0x03;
    static public final Int32 TON_SUBSCRIBER                = 0x04;
    static public final Int32 TON_ALPHANUMERIC              = 0x05;
    static public final Int32 TON_ABBREVIATED               = 0x06;
    static public final Int32 TON_RESERVED                  = 0x07;

    /**
     * Maximum lengths for fields as defined in ril_cdma_sms.h.
     */
    static public final Int32 SMS_ADDRESS_MAX          =  36;
    static public final Int32 SMS_SUBADDRESS_MAX       =  36;

    /**
     * This field shall be set to the number of address digits
     * (See 3GPP2 C.S0015-B, v2, 3.4.3.3)
     */
    public Int32 numberOfDigits;

    /**
     * Numbering Plan identification is a 0 or 4-bit value that
     * indicates which numbering plan identification is set.  (See
     * 3GPP2, C.S0015-B, v2, 3.4.3.3 and C.S005-D, table2.7.1.3.2.4-3)
     */
    static public final Int32 NUMBERING_PLAN_UNKNOWN           = 0x0;
    static public final Int32 NUMBERING_PLAN_ISDN_TELEPHONY    = 0x1;
    //static protected final Int32 NUMBERING_PLAN_DATA              = 0x3;
    //static protected final Int32 NUMBERING_PLAN_TELEX             = 0x4;
    //static protected final Int32 NUMBERING_PLAN_PRIVATE           = 0x9;

    public Int32 numberPlan;

    /**
     * NOTE: the parsed string address and the raw Byte array values
     * are stored in the parent class address and origBytes fields,
     * respectively.
     */

    public CdmaSmsAddress(){
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        StringBuilder builder = new StringBuilder();
        builder->Append("CdmaSmsAddress ");
        builder->Append("{ digitMode=" + digitMode);
        builder->Append(", numberMode=" + numberMode);
        builder->Append(", numberPlan=" + numberPlan);
        builder->Append(", numberOfDigits=" + numberOfDigits);
        builder->Append(", ton=" + ton);
        builder->Append(", address=\"" + address + "\"");
        builder->Append(", origBytes=" + HexDump->ToHexString(origBytes));
        builder->Append(" }");
        return builder->ToString();
    }

    /*
     * TODO(cleanup): Refactor the parsing for addresses to better
     * share code and logic with GSM.  Also, gather all DTMF/BCD
     * processing code in one place.
     */

    private static Byte[] ParseToDtmf(String address) {
        Int32 digits = address->Length();
        Byte[] result = new Byte[digits];
        For (Int32 i = 0; i < digits; i++) {
            Char32 c = address->CharAt(i);
            Int32 val = 0;
            If ((c >= '1') && (c <= '9')) val = c - '0';
            else If (c == '0') val = 10;
            else If (c == '*') val = 11;
            else If (c == '#') val = 12;
            else return NULL;
            result[i] = (Byte)val;
        }
        return result;
    }

    private static const Char32[] numericCharsDialable = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '#'
    };

    private static const Char32[] numericCharsSugar = {
        '(', ')', ' ', '-', '+', '.', '/', '\\'
    };

    private static const SparseBooleanArray numericCharDialableMap = new SparseBooleanArray (
            numericCharsDialable.length + numericCharsSugar.length);
    static {
        For (Int32 i = 0; i < numericCharsDialable.length; i++) {
            numericCharDialableMap->Put(numericCharsDialable[i], TRUE);
        }
        For (Int32 i = 0; i < numericCharsSugar.length; i++) {
            numericCharDialableMap->Put(numericCharsSugar[i], FALSE);
        }
    }

    /**
     * Given a numeric address string, return the string without
     * syntactic sugar, meaning parens, spaces, hyphens/minuses, or
     * plus signs.  If the input string contains non-numeric
     * non-punctuation characters, return NULL.
     */
    private static String FilterNumericSugar(String address) {
        StringBuilder builder = new StringBuilder();
        Int32 len = address->Length();
        For (Int32 i = 0; i < len; i++) {
            Char32 c = address->CharAt(i);
            Int32 mapIndex = numericCharDialableMap->IndexOfKey(c);
            If (mapIndex < 0) return NULL;
            If (! numericCharDialableMap->ValueAt(mapIndex)) continue;
            builder->Append(c);
        }
        return builder->ToString();
    }

    /**
     * Given a string, return the string without whitespace,
     * including CR/LF.
     */
    private static String FilterWhitespace(String address) {
        StringBuilder builder = new StringBuilder();
        Int32 len = address->Length();
        For (Int32 i = 0; i < len; i++) {
            Char32 c = address->CharAt(i);
            If ((c == ' ') || (c == '\r') || (c == '\n') || (c == '\t')) continue;
            builder->Append(c);
        }
        return builder->ToString();
    }

    /**
     * Given a string, create a corresponding CdmaSmsAddress object.
     *
     * The result will be NULL if the input string is not
     * representable using printable ASCII.
     *
     * For numeric addresses, the string is cleaned up by removing
     * common punctuation.  For alpha addresses, the string is cleaned
     * up by removing whitespace.
     */
    public static CdmaSmsAddress Parse(String address) {
        CdmaSmsAddress addr = new CdmaSmsAddress();
        addr.address = address;
        addr.digitMode = DIGIT_MODE_4BIT_DTMF; // Default to 4 bit dtmf encoding
        addr.ton = TON_UNKNOWN;
        addr.numberMode = NUMBER_MODE_NOT_DATA_NETWORK;
        addr.numberPlan = NUMBERING_PLAN_UNKNOWN;
        Byte[] origBytes = NULL;

        If (address->IndexOf('+') != -1) {
            // This is international phone number
            addr.digitMode = DIGIT_MODE_8BIT_CHAR;
            addr.ton = TON_INTERNATIONAL_OR_IP;
            addr.numberMode = NUMBER_MODE_NOT_DATA_NETWORK;
            addr.numberPlan = NUMBERING_PLAN_ISDN_TELEPHONY;
        }
        If (address->IndexOf('@') != -1) {
            // This is email address
            addr.digitMode = DIGIT_MODE_8BIT_CHAR;
            addr.ton = TON_NATIONAL_OR_EMAIL;
            addr.numberMode = NUMBER_MODE_DATA_NETWORK;
        }

        // A.S0014-C 4.2.40 states: "Prefix or escape digits shall not be included"
        String filteredAddr = FilterNumericSugar(address);
        If (addr.digitMode == DIGIT_MODE_4BIT_DTMF) {
            If (filteredAddr != NULL) {
                origBytes = ParseToDtmf(filteredAddr);
            }
            If (origBytes == NULL) {
                // Failed to encode in 4 bit. Try in 8 bit.
                addr.digitMode = DIGIT_MODE_8BIT_CHAR;
            }
        }

        If (addr.digitMode == DIGIT_MODE_8BIT_CHAR) {
            filteredAddr = FilterWhitespace(filteredAddr);
            origBytes = UserData->StringToAscii(filteredAddr);
            If (origBytes == NULL) {
                return NULL;
            }
        }
        addr.origBytes = origBytes;
        addr.numberOfDigits = origBytes.length;
        return addr;
    }

}
