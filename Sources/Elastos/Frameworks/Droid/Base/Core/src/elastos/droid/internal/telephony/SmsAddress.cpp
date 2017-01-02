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

#include "elastos/droid/internal/telephony/SmsAddress.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(SmsAddress, Object, ISmsAddress)

ECode SmsAddress::GetAddressString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = address;
    return NOERROR;
}

ECode SmsAddress::IsAlphanumeric(
        /* [out] */  Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ton == TON_ALPHANUMERIC;
    return NOERROR;
}

ECode SmsAddress::IsNetworkSpecific(
        /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ton == TON_NETWORK;
    return NOERROR;
}

ECode SmsAddress::CouldBeEmailGateway(
        /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Some carriers seems to send email gateway messages in this form:
    // from: an UNKNOWN TON, 3 or 4 digits Int64, beginning with a 5
    // PID: 0x00, Data coding scheme 0x03
    // So we just attempt to treat any message from an address length <= 4
    // as an email gateway
    *result = address.GetLength() <= 4;
    return NOERROR;
}

ECode SmsAddress::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("SmsAddress:") +  address;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

