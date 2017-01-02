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

#ifndef __ELASTOS_DROID_TELEPHONY_GSM_CGSMSMSMESSAGEHELPER_H__
#define __ELASTOS_DROID_TELEPHONY_GSM_CGSMSMSMESSAGEHELPER_H__

#include "_Elastos_Droid_Telephony_Gsm_CGSMSmsMessageHelper.h"
#include "elastos/droid/telephony/gsm/GSMSmsMessage.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

CarClass(CGSMSmsMessageHelper)
    , public Singleton
    , public ISmsMessageHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
      * Create an SmsMessage from a raw PDU.
      * @deprecated Use android.telephony.SmsMessage.
      */
    CARAPI CreateFromPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [out] */ ISmsMessage** result);

    /**
      * Get the TP-Layer-Length for the given SMS-SUBMIT PDU Basically, the
      * length in bytes (not hex chars) less the SMSC header
      * @deprecated Use android.telephony.SmsMessage.
      */
    CARAPI GetTPLayerLengthForPDU(
        /* [in] */ const String& pdu,
        /* [out] */ Int32* result);

    CARAPI CalculateLength(
        /* [in] */ ICharSequence* messageBody,
        /* [in] */ Boolean use7bitOnly,
        /* [out, callee] */ ArrayOf<Int32>** result);

    CARAPI CalculateLength(
        /* [in] */ const String& messageBody,
        /* [in] */ Boolean use7bitOnly,
        /* [out, callee] */ ArrayOf<Int32>** result);

    /**
      * Get an SMS-SUBMIT PDU for a destination address and a message
      *
      * @param scAddress Service Centre address.  Null means use default.
      * @return a <code>SubmitPdu</code> containing the encoded SC
      *         address, if applicable, and the encoded message.
      *         Returns null on encode error.
      * @deprecated Use android.telephony.SmsMessage.
      * @hide
      */
    CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ ArrayOf<Byte>* header,
        /* [out] */ ISubmitPdu** result);

    /**
      * Get an SMS-SUBMIT PDU for a destination address and a message
      *
      * @param scAddress Service Centre address.  Null means use default.
      * @return a <code>SubmitPdu</code> containing the encoded SC
      *         address, if applicable, and the encoded message.
      *         Returns null on encode error.
      * @deprecated Use android.telephony.SmsMessage.
      */
    CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [out] */ ISubmitPdu** result);

    /**
      * Get an SMS-SUBMIT PDU for a data message to a destination address &amp; port
      *
      * @param scAddress Service Centre address. null == use default
      * @param destinationAddress the address of the destination for the message
      * @param destinationPort the port to deliver the message to at the
      *        destination
      * @param data the dat for the message
      * @return a <code>SubmitPdu</code> containing the encoded SC
      *         address, if applicable, and the encoded message.
      *         Returns null on encode error.
      * @deprecated Use android.telephony.SmsMessage.
      */
    CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ Int16 destinationPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Boolean statusReportRequested,
        /* [out] */ ISubmitPdu** result);

};

} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_GSM_CGSMSMSMESSAGEHELPER_H__
