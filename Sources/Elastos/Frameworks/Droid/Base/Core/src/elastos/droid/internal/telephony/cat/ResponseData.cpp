/*
 * Copyright (C) 2006-2007 Google Inc.
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

package com.android.internal.telephony.cat;

using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ITimeZone;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Droid::Internal::Telephony::Cat::AppInterface::ICommandType;

using Elastos::IO::IByteArrayOutputStream;

abstract class ResponseData {
    /**
     * Format the data appropriate for TERMINAL RESPONSE and write it into
     * the ByteArrayOutputStream object.
     */
    public abstract void Format(ByteArrayOutputStream buf);

    public static void WriteLength(ByteArrayOutputStream buf, Int32 length) {
        // As per ETSI 102.220 Sec7.1.2, if the total length is greater
        // than 0x7F, it should be coded in two bytes and the first Byte
        // should be 0x81.
        If (length > 0x7F) {
            buf->Write(0x81);
        }
        buf->Write(length);
    }
}

class SelectItemResponseData extends ResponseData {
    // members
    private Int32 mId;

    public SelectItemResponseData(Int32 id) {
        Super();
        mId = id;
    }

    //@Override
    CARAPI Format(ByteArrayOutputStream buf) {
        // Item identifier object
        Int32 tag = 0x80 | ComprehensionTlvTag.ITEM_ID->Value();
        buf->Write(tag); // tag
        buf->Write(1); // length
        buf->Write(mId); // identifier of item chosen
    }
}

class GetInkeyInputResponseData extends ResponseData {
    // members
    private Boolean mIsUcs2;
    private Boolean mIsPacked;
    private Boolean mIsYesNo;
    private Boolean mYesNoResponse;
    public String mInData;

    // GetInKey Yes/No response characters constants.
    protected static const Byte GET_INKEY_YES = 0x01;
    protected static const Byte GET_INKEY_NO = 0x00;

    public GetInkeyInputResponseData(String inData, Boolean ucs2, Boolean packed) {
        Super();
        mIsUcs2 = ucs2;
        mIsPacked = packed;
        mInData = inData;
        mIsYesNo = FALSE;
    }

    public GetInkeyInputResponseData(Boolean yesNoResponse) {
        Super();
        mIsUcs2 = FALSE;
        mIsPacked = FALSE;
        mInData = "";
        mIsYesNo = TRUE;
        mYesNoResponse = yesNoResponse;
    }

    //@Override
    CARAPI Format(ByteArrayOutputStream buf) {
        If (buf == NULL) {
            return;
        }

        // Text string object
        Int32 tag = 0x80 | ComprehensionTlvTag.TEXT_STRING->Value();
        buf->Write(tag); // tag

        Byte[] data;

        If (mIsYesNo) {
            data = new Byte[1];
            data[0] = mYesNoResponse ? GET_INKEY_YES : GET_INKEY_NO;
        } else If (mInData != NULL && mInData->Length() > 0) {
            try {
                // ETSI TS 102 223 8.15, should use the same format as in SMS messages
                // on the network.
                If (mIsUcs2) {
                    // ucs2 is by definition big endian.
                    data = mInData->GetBytes("UTF-16BE");
                } else If (mIsPacked) {
                    Int32 size = mInData->Length();

                    Byte[] tempData = GsmAlphabet
                            .StringToGsm7BitPacked(mInData, 0, 0);
                    data = new Byte[size];
                    // Since StringToGsm7BitPacked() set Byte 0 in the
                    // returned Byte array to the count of septets used...
                    // copy to a new array without Byte 0.
                    System->Arraycopy(tempData, 1, data, 0, size);
                } else {
                    data = GsmAlphabet->StringToGsm8BitPacked(mInData);
                }
            } Catch (UnsupportedEncodingException e) {
                data = new Byte[0];
            } Catch (EncodeException e) {
                data = new Byte[0];
            }
        } else {
            data = new Byte[0];
        }

        // length - one more for data coding scheme.
        WriteLength(buf, data.length + 1);

        // data coding scheme
        If (mIsUcs2) {
            buf->Write(0x08); // UCS2
        } else If (mIsPacked) {
            buf->Write(0x00); // 7 bit packed
        } else {
            buf->Write(0x04); // 8 bit unpacked
        }

        For (Byte b : data) {
            buf->Write(b);
        }
    }
}

// For "PROVIDE LOCAL INFORMATION" command.
// See TS 31.111 section 6.4.15/ETSI TS 102 223
// TS 31.124 section 27.22.4.15 for test spec
class LanguageResponseData extends ResponseData {
    private String mLang;

    public LanguageResponseData(String lang) {
        Super();
        mLang = lang;
    }

    //@Override
    CARAPI Format(ByteArrayOutputStream buf) {
        If (buf == NULL) {
            return;
        }

        // Text string object
        Int32 tag = 0x80 | ComprehensionTlvTag.LANGUAGE->Value();
        buf->Write(tag); // tag

        Byte[] data;

        If (mLang != NULL && mLang->Length() > 0) {
            data = GsmAlphabet->StringToGsm8BitPacked(mLang);
        }
        else {
            data = new Byte[0];
        }

        buf->Write(data.length);

        For (Byte b : data) {
            buf->Write(b);
        }
    }
}

// For "PROVIDE LOCAL INFORMATION" command.
// See TS 31.111 section 6.4.15/ETSI TS 102 223
// TS 31.124 section 27.22.4.15 for test spec
class DTTZResponseData extends ResponseData {
    private Calendar mCalendar;

    public DTTZResponseData(Calendar cal) {
        Super();
        mCalendar = cal;
    }

    //@Override
    CARAPI Format(ByteArrayOutputStream buf) {
        If (buf == NULL) {
            return;
        }

        // DTTZ object
        Int32 tag = 0x80 | CommandType.PROVIDE_LOCAL_INFORMATION->Value();
        buf->Write(tag); // tag

        Byte[] data = new Byte[8];

        data[0] = 0x07; // Write length of DTTZ data

        If (mCalendar == NULL) {
            mCalendar = Calendar->GetInstance();
        }
        // Fill year Byte
        data[1] = ByteToBCD(mCalendar->Get(java.util.Calendar.YEAR) % 100);

        // Fill month Byte
        data[2] = ByteToBCD(mCalendar->Get(java.util.Calendar.MONTH) + 1);

        // Fill day Byte
        data[3] = ByteToBCD(mCalendar->Get(java.util.Calendar.DATE));

        // Fill hour Byte
        data[4] = ByteToBCD(mCalendar->Get(java.util.Calendar.HOUR_OF_DAY));

        // Fill minute Byte
        data[5] = ByteToBCD(mCalendar->Get(java.util.Calendar.MINUTE));

        // Fill second Byte
        data[6] = ByteToBCD(mCalendar->Get(java.util.Calendar.SECOND));

        String tz = SystemProperties->Get("persist.sys.timezone", "");
        If (TextUtils->IsEmpty(tz)) {
            data[7] = (Byte) 0xFF;    // set FF in terminal response
        } else {
            TimeZone zone = TimeZone->GetTimeZone(tz);
            Int32 zoneOffset = zone->GetRawOffset() + zone->GetDSTSavings();
            data[7] = GetTZOffSetByte(zoneOffset);
        }

        For (Byte b : data) {
            buf->Write(b);
        }
    }

    private Byte ByteToBCD(Int32 value) {
        If (value < 0 && value > 99) {
            CatLogger::D(this, "Err: byteToBCD conversion Value is " + value +
                           " Value has to be between 0 and 99");
            return 0;
        }

        Return (Byte) ((value / 10) | ((value % 10) << 4));
    }

    private Byte GetTZOffSetByte(Int64 offSetVal) {
        Boolean isNegative = (offSetVal < 0);

        /*
         * The 'offSetVal' is in milliseconds. Convert it to hours and compute
         * offset While sending T.R to UICC, offset is expressed is 'quarters of
         * hours'
         */

         Int64 tzOffset = offSetVal / (15 * 60 * 1000);
         tzOffset = (isNegative ? -1 : 1) * tzOffset;
         Byte bcdVal = ByteToBCD((Int32) tzOffset);
         // For negative offsets, put '1' in the msb
         return isNegative ?  (bcdVal |= 0x08) : bcdVal;
    }

}

