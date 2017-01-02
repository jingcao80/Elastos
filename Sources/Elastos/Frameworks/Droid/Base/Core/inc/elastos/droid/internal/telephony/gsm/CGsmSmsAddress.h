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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMSMSADDRESS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMSMSADDRESS_H__

#include "_Elastos_Droid_Internal_Telephony_Gsm_CGsmSmsAddress.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/telephony/SmsAddress.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telephony::SmsAddress;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CarClass(CGsmSmsAddress)
    , public SmsAddress
    , public IGsmSmsAddress
{
public:
    CGsmSmsAddress();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * New GsmSmsAddress from TS 23.040 9.1.2.5 Address Field
     *
     * @param offset the offset of the Address-Length byte
     * @param length the length in bytes rounded up, e.g. "2 +
     *        (addressLength + 1) / 2"
     * @throws ParseException
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI GetAddressString(
        /* [out] */ String* result);

    /**
     * Returns true if this is an alphanumeric address
     */
    CARAPI IsAlphanumeric(
        /* [out] */ Boolean* result);

    CARAPI IsNetworkSpecific(
        /* [out] */ Boolean* result);

    /**
     * Returns true of this is a valid CPHS voice message waiting indicator
     * address
     */
    CARAPI IsCphsVoiceMessageIndicatorAddress(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this is a valid CPHS voice message waiting indicator
     * address indicating a "set" of "indicator 1" of type "voice message
     * waiting"
     */
    CARAPI IsCphsVoiceMessageSet(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this is a valid CPHS voice message waiting indicator
     * address indicating a "clear" of "indicator 1" of type "voice message
     * waiting"
     */
    CARAPI IsCphsVoiceMessageClear(
        /* [out] */ Boolean* result);
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMSMSADDRESS_H__
