//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/internal/telephony/gsm/CGsmSmsAddress.h"
#include "elastos/droid/internal/telephony/GsmAlphabet.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"

using Elastos::Droid::Internal::Telephony::GsmAlphabet;
using Elastos::Droid::Telephony::PhoneNumberUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(CGsmSmsAddress, SmsAddress, IGsmSmsAddress)

CAR_OBJECT_IMPL(CGsmSmsAddress)

CGsmSmsAddress::CGsmSmsAddress()
{
}

ECode CGsmSmsAddress::constructor()
{
    return NOERROR;
}

ECode CGsmSmsAddress::constructor(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    origBytes = ArrayOf<Byte>::Alloc(length);
    origBytes->Copy(0, data, offset, length);

    // addressLength is the count of semi-octets, not bytes
    Int32 addressLength = (*origBytes)[OFFSET_ADDRESS_LENGTH] & 0xff;

    Int32 toa = (*origBytes)[OFFSET_TOA] & 0xff;
    ton = 0x7 & (toa >> 4);

    // TOA must have its high bit set
    if ((toa & 0x80) != 0x80) {
        // throw new ParseException("Invalid TOA - high bit must be set. toa = " + toa,
        //         offset + OFFSET_TOA);
        return E_PARSE_EXCEPTION;
    }

    Boolean b;
    if (IsAlphanumeric(&b), b) {
        // An alphanumeric address
        Int32 countSeptets = addressLength * 4 / 7;

        GsmAlphabet::Gsm7BitPackedToString(origBytes,
                OFFSET_ADDRESS_VALUE, countSeptets, &address);
    }
    else {
        // TS 23.040 9.1.2.5 says
        // that "the MS shall interpret reserved values as 'Unknown'
        // but shall store them exactly as received"

        Byte lastByte = (*origBytes)[length - 1];

        if ((addressLength & 1) == 1) {
            // Make sure the final unused BCD digit is 0xf
            (*origBytes)[length - 1] |= 0xf0;
        }
        PhoneNumberUtils::CalledPartyBCDToString(origBytes,
                OFFSET_TOA, length - OFFSET_TOA, &address);

        // And restore origBytes
        (*origBytes)[length - 1] = lastByte;
    }
    return NOERROR;
}

ECode CGsmSmsAddress::GetAddressString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = address;
    return NOERROR;
}

ECode CGsmSmsAddress::IsAlphanumeric(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ton == TON_ALPHANUMERIC;
    return NOERROR;
}

ECode CGsmSmsAddress::IsNetworkSpecific(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ton == TON_NETWORK;
    return NOERROR;
}

ECode CGsmSmsAddress::IsCphsVoiceMessageIndicatorAddress(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // CPHS-style MWI message
    // See CPHS 4.7 B.4.2.1
    //
    // Basically:
    //
    // - Originating address should be 4 bytes long and alphanumeric
    // - Decode will result with two chars:
    // - Char 1
    // 76543210
    // ^ set/clear indicator (0 = clear)
    // ^^^ type of indicator (000 = voice)
    // ^^^^ must be equal to 0001
    // - Char 2:
    // 76543210
    // ^ line number (0 = line 1)
    // ^^^^^^^ set to 0
    //
    // Remember, since the alpha address is stored in 7-bit compact form,
    // the "line number" is really the top bit of the first address value
    // byte

    Boolean b;
    *result = ((*origBytes)[OFFSET_ADDRESS_LENGTH] & 0xff) == 4
            && (IsAlphanumeric(&b), b) && ((*origBytes)[OFFSET_TOA] & 0x0f) == 0;
    return NOERROR;
}

ECode CGsmSmsAddress::IsCphsVoiceMessageSet(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // 0x11 means "set" "voice message waiting" "indicator 1"
    Boolean b;
    *result = (IsCphsVoiceMessageIndicatorAddress(&b), b)
            && ((*origBytes)[OFFSET_ADDRESS_VALUE] & 0xff) == 0x11;
    return NOERROR;
}

ECode CGsmSmsAddress::IsCphsVoiceMessageClear(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // 0x10 means "clear" "voice message waiting" "indicator 1"
    Boolean b;
    *result = (IsCphsVoiceMessageIndicatorAddress(&b), b)
            && ((*origBytes)[OFFSET_ADDRESS_VALUE] & 0xff) == 0x10;
    return NOERROR;
}



} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos