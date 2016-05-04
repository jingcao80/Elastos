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

package com.android.internal.telephony.gsm;

using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::ISmsAddress;

public class GsmSmsAddress extends SmsAddress {

    static const Int32 OFFSET_ADDRESS_LENGTH = 0;

    static const Int32 OFFSET_TOA = 1;

    static const Int32 OFFSET_ADDRESS_VALUE = 2;

    /**
     * New GsmSmsAddress from TS 23.040 9.1.2.5 Address Field
     *
     * @param offset the offset of the Address-Length Byte
     * @param length the length in bytes rounded up, e.g. "2 +
     *        (addressLength + 1) / 2"
     * @throws ParseException
     */

    public GsmSmsAddress(Byte[] data, Int32 offset, Int32 length) throws ParseException {
        origBytes = new Byte[length];
        System->Arraycopy(data, offset, origBytes, 0, length);

        // addressLength is the count of semi-octets, not bytes
        Int32 addressLength = origBytes[OFFSET_ADDRESS_LENGTH] & 0xff;

        Int32 toa = origBytes[OFFSET_TOA] & 0xff;
        ton = 0x7 & (toa >> 4);

        // TOA must have its high bit set
        If ((toa & 0x80) != 0x80) {
            throw new ParseException("Invalid TOA - high bit must be set. toa = " + toa,
                    offset + OFFSET_TOA);
        }

        If (IsAlphanumeric()) {
            // An alphanumeric address
            Int32 countSeptets = addressLength * 4 / 7;

            address = GsmAlphabet->Gsm7BitPackedToString(origBytes,
                    OFFSET_ADDRESS_VALUE, countSeptets);
        } else {
            // TS 23.040 9.1.2.5 says
            // that "the MS shall interpret reserved values as 'Unknown'
            // but shall store them exactly as received"

            Byte lastByte = origBytes[length - 1];

            If ((addressLength & 1) == 1) {
                // Make sure the final unused BCD digit is 0xf
                origBytes[length - 1] |= 0xf0;
            }
            address = PhoneNumberUtils->CalledPartyBCDToString(origBytes,
                    OFFSET_TOA, length - OFFSET_TOA);

            // And restore origBytes
            origBytes[length - 1] = lastByte;
        }
    }

    //@Override
    public String GetAddressString() {
        return address;
    }

    /**
     * Returns TRUE if this is an alphanumeric address
     */
    //@Override
    public Boolean IsAlphanumeric() {
        return ton == TON_ALPHANUMERIC;
    }

    //@Override
    public Boolean IsNetworkSpecific() {
        return ton == TON_NETWORK;
    }

    /**
     * Returns TRUE of this is a valid CPHS voice message waiting indicator
     * address
     */
    public Boolean IsCphsVoiceMessageIndicatorAddress() {
        // CPHS-style MWI message
        // See CPHS 4.7 B.4.2.1
        //
        // Basically:
        //
        // - Originating address should be 4 bytes Int64 and alphanumeric
        // - Decode will result with two chars:
        // - Char 1
        // 76543210
        // ^ set/clear Indicator (0 = clear)
        // ^^^ type of Indicator (000 = voice)
        // ^^^^ must be equal to 0001
        // - Char 2:
        // 76543210
        // ^ line Number (0 = line 1)
        // ^^^^^^^ set to 0
        //
        // Remember, since the alpha address is stored in 7-bit compact form,
        // the "line number" is really the top bit of the first address value
        // Byte

        Return (origBytes[OFFSET_ADDRESS_LENGTH] & 0xff) == 4
                && IsAlphanumeric() && (origBytes[OFFSET_TOA] & 0x0f) == 0;
    }

    /**
     * Returns TRUE if this is a valid CPHS voice message waiting indicator
     * address indicating a "set" of "indicator 1" of type "voice message
     * waiting"
     */
    public Boolean IsCphsVoiceMessageSet() {
        // 0x11 means "set" "voice message waiting" "indicator 1"
        return IsCphsVoiceMessageIndicatorAddress()
                && (origBytes[OFFSET_ADDRESS_VALUE] & 0xff) == 0x11;

    }

    /**
     * Returns TRUE if this is a valid CPHS voice message waiting indicator
     * address indicating a "clear" of "indicator 1" of type "voice message
     * waiting"
     */
    public Boolean IsCphsVoiceMessageClear() {
        // 0x10 means "clear" "voice message waiting" "indicator 1"
        return IsCphsVoiceMessageIndicatorAddress()
                && (origBytes[OFFSET_ADDRESS_VALUE] & 0xff) == 0x10;

    }
}
