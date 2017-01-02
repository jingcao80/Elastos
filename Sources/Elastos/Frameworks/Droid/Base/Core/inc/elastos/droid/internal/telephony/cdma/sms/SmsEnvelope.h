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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_SMSENVELOPE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_SMSENVELOPE_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package com.android.internal.telephony.cdma.sms;
// import com.android.internal.telephony.cdma.sms.CdmaSmsSubaddress;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

class SmsEnvelope
    : public Object
    , public ISmsEnvelope
{
public:
    CAR_INTERFACE_DECL();

    SmsEnvelope();

public:
    /**
      * Provides the type of a SMS message like point to point, broadcast or acknowledge
      */
    Int32 messageType;
    /**
      * The 16-bit Teleservice parameter identifies which upper layer service access point is sending
      * or receiving the message.
      * (See 3GPP2 C.S0015-B, v2, 3.4.3.1)
      */
    Int32 teleService;
    /**
      * The 16-bit service category parameter identifies the type of service provided
      * by the SMS message.
      * (See 3GPP2 C.S0015-B, v2, 3.4.3.2)
      */
    Int32 serviceCategory;
    /**
      * The origination address identifies the originator of the SMS message.
      * (See 3GPP2 C.S0015-B, v2, 3.4.3.3)
      */
    AutoPtr<ICdmaSmsAddress> origAddress;
    /**
      * The destination address identifies the target of the SMS message.
      * (See 3GPP2 C.S0015-B, v2, 3.4.3.3)
      */
    AutoPtr<ICdmaSmsAddress> destAddress;
    /**
      * The origination subaddress identifies the originator of the SMS message.
      * (See 3GPP2 C.S0015-B, v2, 3.4.3.4)
      */
    AutoPtr<ICdmaSmsSubaddress> origSubaddress;
    /**
      * The 6-bit bearer reply parameter is used to request the return of a
      * SMS Acknowledge Message.
      * (See 3GPP2 C.S0015-B, v2, 3.4.3.5)
      */
    Int32 bearerReply;
    /**
      * Cause Code values:
      * The cause code parameters are an indication whether an SMS error has occurred and if so,
      * whether the condition is considered temporary or permanent.
      * ReplySeqNo 6-bit value,
      * ErrorClass 2-bit value,
      * CauseCode 0-bit or 8-bit value
      * (See 3GPP2 C.S0015-B, v2, 3.4.3.6)
      */
    Byte replySeqNo;
    Byte errorClass;
    Byte causeCode;
    /**
      * encoded bearer data
      * (See 3GPP2 C.S0015-B, v2, 3.4.3.7)
      */
    AutoPtr<ArrayOf<Byte> > bearerData;
};

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_SMSENVELOPE_H__
