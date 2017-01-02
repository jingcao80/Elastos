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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SYNTHETICSMSMESSAGE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SYNTHETICSMSMESSAGE_H__

#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/SmsMessageBase.h"
#include "elastos/droid/internal/telephony/SmsAddress.h"

using Elastos::Droid::Internal::Telephony::SmsMessageBase;
using Elastos::Droid::Internal::Telephony::SmsAddress;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class SyntheticSmsMessage
    : public SmsMessageBase
    , public ISyntheticSmsMessage
{
public:
    class SyntheticAddress
        : public SmsAddress
    {
    };

public:
    CAR_INTERFACE_DECL()

    static CARAPI_(Boolean) IsSyntheticPdu(
        /* [in] */ ArrayOf<Byte>* pdu);

    static CARAPI_(AutoPtr<ISyntheticSmsMessage>) CreateFromPdu(
        /* [in] */ ArrayOf<Byte>* pdu);

    CARAPI constructor(
        /* [in] */ String originatingAddress,
        /* [in] */ String scAddress,
        /* [in] */ String messageBody,
        /* [in] */ Int64 timestampMillis);

    //@Override
    CARAPI GetMessageClass(
        /* [out] */ MessageClass* result);

    //@Override
    CARAPI GetProtocolIdentifier(
        /* [out] */ Int32* result);

    //@Override
    CARAPI IsReplace(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsCphsMwiMessage(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsMWIClearMessage(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsMWISetMessage(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsMwiDontStore(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetStatus(
        /* [out] */ Int32* result);

    //@Override
    CARAPI IsStatusReportMessage(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsReplyPathPresent(
        /* [out] */ Boolean* result);
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SYNTHETICSMSMESSAGE_H__
