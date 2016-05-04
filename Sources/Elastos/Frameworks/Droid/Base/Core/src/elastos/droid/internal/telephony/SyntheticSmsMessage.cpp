/*
 * Copyright (C) 2014 The CyanogenMod Project
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

package com.android.internal.telephony;

using org::Json::IJSONObject;

using Elastos::Droid::Internal::Telephony::SmsConstants::IMessageClass;

public class SyntheticSmsMessage extends SmsMessageBase {
    public static class SyntheticAddress extends SmsAddress {
    }

    public static Boolean IsSyntheticPdu(Byte[] pdu) {
        try {
            JSONObject json = new JSONObject(new String(pdu));
            return json->OptBoolean("synthetic", FALSE);
        }
        Catch (Exception e) {
        }
        return FALSE;
    }

    public static SyntheticSmsMessage CreateFromPdu(Byte[] pdu) {
        try {
            // TODO: use Parcelable or Bundle or something that serializes?
            JSONObject json = new JSONObject(new String(pdu));
            SyntheticSmsMessage message = new SyntheticSmsMessage(
                    json->GetString("originatingAddress"),
                    json->OptString("scAddress", NULL),
                    json->GetString("messageBody"),
                    json->GetLong("timestampMillis"));
            return message;
        }
        Catch (Exception e) {
            e->PrintStackTrace();
        }
        return NULL;
    }

    public SyntheticSmsMessage(String originatingAddress, String scAddress, String messageBody, Int64 timestampMillis) {
        this.mOriginatingAddress = new SyntheticAddress();
        this.mOriginatingAddress.address = originatingAddress;

        this.mMessageBody = messageBody;
        this.mScTimeMillis = timestampMillis;
        this.mScAddress = scAddress;

        try {
            JSONObject json = new JSONObject();
            json->Put("originatingAddress", originatingAddress);
            json->Put("scAddress", scAddress);
            json->Put("messageBody", messageBody);
            json->Put("timestampMillis", timestampMillis);
            json->Put("synthetic", TRUE);
            this.mPdu = json->ToString()->GetBytes();
        }
        Catch (Exception e) {
            e->PrintStackTrace();
        }
    }

    //@Override
    public MessageClass GetMessageClass() {
        return SmsConstants.MessageClass.UNKNOWN;
    }

    //@Override
    public Int32 GetProtocolIdentifier() {
        return 0;
    }

    //@Override
    public Boolean IsReplace() {
        return FALSE;
    }

    //@Override
    public Boolean IsCphsMwiMessage() {
        return FALSE;
    }

    //@Override
    public Boolean IsMWIClearMessage() {
        return FALSE;
    }

    //@Override
    public Boolean IsMWISetMessage() {
        return FALSE;
    }

    //@Override
    public Boolean IsMwiDontStore() {
        return FALSE;
    }

    //@Override
    public Int32 GetStatus() {
        return 0;
    }

    //@Override
    public Boolean IsStatusReportMessage() {
        return FALSE;
    }

    //@Override
    public Boolean IsReplyPathPresent() {
        return FALSE;
    }
}