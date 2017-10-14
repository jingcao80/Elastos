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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMSMSDISPATCHER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMSMSDISPATCHER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "_Elastos_Droid_Internal_Telephony_Gsm_CGsmSMSDispatcher.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/telephony/SMSDispatcher.h"
#include "elastos/droid/os/AsyncResult.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Internal::Telephony::SMSDispatcher;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IMessage;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::IAtomicReference;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * {@hide}
 */
CarClass(CGsmSMSDispatcher)
    , public SMSDispatcher
    , public IGsmSMSDispatcher
{
public:
    TO_STRING_IMPL("CGsmSMSDispatcher")

    CGsmSMSDispatcher();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IPhoneBase* phone,
        /* [in] */ ISmsUsageMonitor* usageMonitor,
        /* [in] */ IImsSMSDispatcher* imsSMSDispatcher,
        /* [in] */ IGsmInboundSmsHandler* gsmInboundSmsHandler);

    CARAPI Dispose();

    /**
     * Handles 3GPP format-specific events coming from the phone stack.
     * Other events are handled by {@link SMSDispatcher#handleMessage}.
     *
     * @param msg the message to handle
     */
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

protected:
    CARAPI GetFormat(
        /* [out] */ String* result);

    CARAPI SendData(
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ Int32 destPort,
        /* [in] */ Int32 origPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    CARAPI SendText(
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ const String& text,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent,
        /* [in] */ IUri* messageUri,
        /* [in] */ const String& callingPkg,
        /* [in] */ Int32 priority,
        /* [in] */ Boolean isExpectMore,
        /* [in] */ Int32 validityPeriod );

    CARAPI InjectSmsPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ const String& format,
        /* [in] */ IPendingIntent* receivedIntent);

    CARAPI CalculateLength(
        /* [in] */ ICharSequence* messageBody,
        /* [in] */ Boolean use7bitOnly,
        /* [out] */ IGsmAlphabetTextEncodingDetails** result);

    CARAPI SendNewSubmitPdu(
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& scAddress,
        /* [in] */ const String& message,
        /* [in] */ ISmsHeader* smsHeader,
        /* [in] */ Int32 encoding,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent,
        /* [in] */ Boolean lastPart,
        /* [in] */ Int32 priority,
        /* [in] */ Boolean isExpectMore,
        /* [in] */ Int32 validityPeriod,
        /* [in] */ IAtomicInteger32* unsentPartCount,
        /* [in] */ IAtomicBoolean* anyPartFailed,
        /* [in] */ IUri* messageUri);

    CARAPI_(void) SendSms(
        /* [in] */ SmsTracker* tracker);

    CARAPI_(void) SendSmsByPstn(
        /* [in] */ SmsTracker* tracker);

    CARAPI_(void) UpdateSmsSendStatus(
        /* [in] */ Int32 messageRef,
        /* [in] */ Boolean success);

    CARAPI_(AutoPtr<IUiccCardApplication>) GetUiccCardApplication();

private:
    using SMSDispatcher::HandleStatusReport;

    /**
     * Called when a status report is received.  This should correspond to
     * a previously successful SEND.
     *
     * @param ar AsyncResult passed into the message handler.  ar.result should
     *           be a String representing the status report PDU, as ASCII hex.
     */
    CARAPI_(void) HandleStatusReport(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) OnUpdateIccAvailability();

protected:
    AutoPtr<IUiccController> mUiccController;

private:
    static const String TAG; // = "GsmSMSDispatcher";
    static const Boolean VDBG = FALSE;
    AutoPtr<IAtomicReference> mIccRecords; // = new AtomicReference<IccRecords>();
    AutoPtr<IAtomicReference> mUiccApplication; // = new AtomicReference<UiccCardApplication>();
    AutoPtr<IGsmInboundSmsHandler> mGsmInboundSmsHandler;

    /** Status report received */
    static const Int32 EVENT_NEW_SMS_STATUS_REPORT = 100;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMSMSDISPATCHER_H__
