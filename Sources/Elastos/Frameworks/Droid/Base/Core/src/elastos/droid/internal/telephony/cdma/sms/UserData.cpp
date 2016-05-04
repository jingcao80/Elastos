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

using Elastos::Droid::Utility::ISparseIntArray;

using Elastos::Droid::Internal::Telephony::ISmsHeader;
using Elastos::Droid::Internal::Utility::IHexDump;

public class UserData {

    /**
     * User data encoding types.
     * (See 3GPP2 C.R1001-F, v1.0, table 9.1-1)
     */
    public static const Int32 ENCODING_OCTET                      = 0x00;
    public static const Int32 ENCODING_IS91_EXTENDED_PROTOCOL     = 0x01;
    public static const Int32 ENCODING_7BIT_ASCII                 = 0x02;
    public static const Int32 ENCODING_IA5                        = 0x03;
    public static const Int32 ENCODING_UNICODE_16                 = 0x04;
    public static const Int32 ENCODING_SHIFT_JIS                  = 0x05;
    public static const Int32 ENCODING_KOREAN                     = 0x06;
    public static const Int32 ENCODING_LATIN_HEBREW               = 0x07;
    public static const Int32 ENCODING_LATIN                      = 0x08;
    public static const Int32 ENCODING_GSM_7BIT_ALPHABET          = 0x09;
    public static const Int32 ENCODING_GSM_DCS                    = 0x0A;

    /**
     * User data message type encoding types.
     * (See 3GPP2 C.S0015-B, 4.5.2 and 3GPP 23.038, Section 4)
     */
    public static const Int32 ENCODING_GSM_DCS_7BIT               = 0x00;
    public static const Int32 ENCODING_GSM_DCS_8BIT               = 0x01;
    public static const Int32 ENCODING_GSM_DCS_16BIT              = 0x02;

    /**
     * IS-91 message types.
     * (See TIA/EIS/IS-91-A-ENGL 1999, table 3.7.1.1-3)
     */
    public static const Int32 IS91_MSG_TYPE_VOICEMAIL_STATUS   = 0x82;
    public static const Int32 IS91_MSG_TYPE_SHORT_MESSAGE_FULL = 0x83;
    public static const Int32 IS91_MSG_TYPE_CLI                = 0x84;
    public static const Int32 IS91_MSG_TYPE_SHORT_MESSAGE      = 0x85;

    /**
     * US ASCII character mapping table.
     *
     * This table contains only the printable ASCII characters, with a
     * 0x20 offset, meaning that the ASCII SPACE character is at index
     * 0, with the resulting code of 0x20.
     *
     * Note this mapping is also equivalent to that used by both the
     * IA5 and the IS-91 encodings.  For the former this is defined
     * using CCITT Rec. T.50 Tables 1 and 3.  For the latter IS 637 B,
     * Table 4.3.1.4.1-1 -- and note the encoding uses only 6 bits,
     * and hence only maps entries up to the '_' character.
     *
     */
    public static const Char32[] ASCII_MAP = {
        ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
        '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
        'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
        '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
        'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~'};

    /**
     * Character to use when forced to encode otherwise unencodable
     * characters, meaning those not in the respective ASCII or GSM
     * 7-bit encoding tables.  Current choice is SPACE, which is 0x20
     * in both the GSM-7bit and ASCII-7bit encodings.
     */
    static const Byte UNENCODABLE_7_BIT_CHAR = 0x20;

    /**
     * Only elements between these indices in the ASCII table are printable.
     */
    public static const Int32 PRINTABLE_ASCII_MIN_INDEX = 0x20;
    public static const Int32 ASCII_NL_INDEX = 0x0A;
    public static const Int32 ASCII_CR_INDEX = 0x0D;
    public static const SparseIntArray charToAscii = new SparseIntArray();
    static {
        For (Int32 i = 0; i < ASCII_MAP.length; i++) {
            charToAscii->Put(ASCII_MAP[i], PRINTABLE_ASCII_MIN_INDEX + i);
        }
        charToAscii->Put('\n', ASCII_NL_INDEX);
        charToAscii->Put('\r', ASCII_CR_INDEX);
    }

    /*
     * TODO(cleanup): Move this very generic functionality somewhere
     * more general.
     */
    /**
     * Given a string generate a corresponding ASCII-encoded Byte
     * array, but limited to printable characters.  If the input
     * contains unprintable characters, return NULL.
     */
    public static Byte[] StringToAscii(String str) {
        Int32 len = str->Length();
        Byte[] result = new Byte[len];
        For (Int32 i = 0; i < len; i++) {
            Int32 charCode = charToAscii->Get(str->CharAt(i), -1);
            If (charCode == -1) return NULL;
            result[i] = (Byte)charCode;
        }
        return result;
    }

    /**
     * Mapping for ASCII values less than 32 are flow control signals
     * and not used here.
     */
    public static const Int32 ASCII_MAP_BASE_INDEX = 0x20;
    public static const Int32 ASCII_MAP_MAX_INDEX = ASCII_MAP_BASE_INDEX + ASCII_MAP.length - 1;

    /**
     * Contains the data header of the user data
     */
    public SmsHeader userDataHeader;

    /**
     * Contains the data encoding type for the SMS message
     */
    public Int32 msgEncoding;
    public Boolean msgEncodingSet = FALSE;

    public Int32 msgType;

    /**
     * Number of invalid bits in the last Byte of data.
     */
    public Int32 paddingBits;

    public Int32 numFields;

    /**
     * Contains the user data of a SMS message
     * (See 3GPP2 C.S0015-B, v2, 4.5.2)
     */
    public Byte[] payload;
    public String payloadStr;

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        StringBuilder builder = new StringBuilder();
        builder->Append("UserData ");
        builder->Append("{ msgEncoding=" + (msgEncodingSet ? msgEncoding : "unset"));
        builder->Append(", msgType=" + msgType);
        builder->Append(", paddingBits=" + paddingBits);
        builder->Append(", numFields=" + numFields);
        builder->Append(", userDataHeader=" + userDataHeader);
        builder->Append(", payload='" + HexDump->ToHexString(payload) + "'");
        builder->Append(", payloadStr='" + payloadStr + "'");
        builder->Append(" }");
        return builder->ToString();
    }

}
