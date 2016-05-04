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

package com.android.internal.telephony;

public abstract class SmsAddress {
    // From TS 23.040 9.1.2.5 and TS 24.008 table 10.5.118
    // and C.S0005-D table 2.7.1.3.2.4-2
    public static const Int32 TON_UNKNOWN = 0;
    public static const Int32 TON_INTERNATIONAL = 1;
    public static const Int32 TON_NATIONAL = 2;
    public static const Int32 TON_NETWORK = 3;
    public static const Int32 TON_SUBSCRIBER = 4;
    public static const Int32 TON_ALPHANUMERIC = 5;
    public static const Int32 TON_ABBREVIATED = 6;

    public Int32 ton;
    public String address;
    public Byte[] origBytes;

    /**
     * Returns the address of the SMS message in String form or NULL if unavailable
     */
    public String GetAddressString() {
        return address;
    }

    /**
     * Returns TRUE if this is an alphanumeric address
     */
    public Boolean IsAlphanumeric() {
        return ton == TON_ALPHANUMERIC;
    }

    /**
     * Returns TRUE if this is a network address
     */
    public Boolean IsNetworkSpecific() {
        return ton == TON_NETWORK;
    }

    public Boolean CouldBeEmailGateway() {
        // Some carriers seems to send email gateway messages in this form:
        // from: an UNKNOWN TON, 3 or 4 digits Int64, beginning with a 5
        // PID: 0x00, Data coding scheme 0x03
        // So we just attempt to treat any message from an address length <= 4
        // as an email gateway

        return address->Length() <= 4;
    }

}
