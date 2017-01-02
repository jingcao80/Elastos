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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMASMSMESSAGEHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMASMSMESSAGEHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_CCdmaSmsMessageHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Internal::Telephony::Cdma::Sms::IUserData;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CarClass(CCdmaSmsMessageHelper)
    , public Singleton
    , public ISmsMessageHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI CreateFromPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [out] */ ISmsMessage** result);

    CARAPI NewFromParcel(
        /* [in] */ IParcel* p,
        /* [out] */ ISmsMessage** result);

    CARAPI CreateFromEfRecord(
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ ISmsMessage** result);

    CARAPI GetTPLayerLengthForPDU(
        /* [in] */ const String& pdu,
        /* [out] */ Int32* result);

    CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddr,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ ISmsHeader* smsHeader,
        /* [out] */ ISmsMessageSubmitPdu** result);

    CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddr,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ ISmsHeader* smsHeader,
        /* [in] */ Int32 priority,
        /* [out] */ ISmsMessageSubmitPdu** result);

    CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddr,
        /* [in] */ const String& destAddr,
        /* [in] */ Int32 destPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Boolean statusReportRequested,
        /* [out] */ ISmsMessageSubmitPdu** result);

    CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddr,
        /* [in] */ const String& destAddr,
        /* [in] */ Int32 destPort,
        /* [in] */ Int32 origPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Boolean statusReportRequested,
        /* [out] */ ISmsMessageSubmitPdu** result);

    CARAPI GetSubmitPdu(
        /* [in] */ const String& destAddr,
        /* [in] */ IUserData* userData,
        /* [in] */ Boolean statusReportRequested,
        /* [out] */ ISmsMessageSubmitPdu** result);

    CARAPI GetSubmitPdu(
        /* [in] */ const String& destAddr,
        /* [in] */ IUserData* userData,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ Int32 priority,
        /* [out] */ ISmsMessageSubmitPdu** result);

    CARAPI CalculateLength(
        /* [in] */ ICharSequence* messageBody,
        /* [in] */ Boolean use7bitOnly,
        /* [out] */ IGsmAlphabetTextEncodingDetails** result);

};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMASMSMESSAGEHELPER_H__
