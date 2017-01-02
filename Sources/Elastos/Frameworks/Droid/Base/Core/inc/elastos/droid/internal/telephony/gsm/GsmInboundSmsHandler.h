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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_GSMINBOUNDSMSHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_GSMINBOUNDSMSHANDLER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/telephony/InboundSmsHandler.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::InboundSmsHandler;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::ISmsMessageBase;
using Elastos::Droid::Internal::Telephony::ISmsStorageMonitor;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * This class broadcasts incoming SMS messages to interested apps after storing them in
 * the SmsProvider "raw" table and ACKing them to the SMSC. After each message has been
 */
class GsmInboundSmsHandler
    : public InboundSmsHandler
    , public IGsmInboundSmsHandler
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Wait for state machine to enter startup state. We can't send any messages until then.
     */
    static CARAPI MakeInboundSmsHandler(
        /* [in] */ IContext* context,
        /* [in] */ ISmsStorageMonitor* storageMonitor,
        /* [in] */ IPhoneBase* phone,
        /* [out] */ IGsmInboundSmsHandler** result);

    /* package */
    CARAPI UpdateMessageWaitingIndicator(
        /* [in] */ Int32 voicemailCount);

protected:
    /**
     * Unregister for GSM SMS.
     */
    CARAPI_(void) OnQuitting();

    /**
     * Return true if this handler is for 3GPP2 messages; false for 3GPP format.
     * @return false (3GPP)
     */
    CARAPI_(Boolean) Is3gpp2();

    /**
     * Handle type zero, SMS-PP data download, and 3GPP/CPHS MWI type SMS. Normal SMS messages
     * are handled by {@link #dispatchNormalMessage} in parent class.
     *
     * @param smsb the SmsMessageBase object from the RIL
     * @return a result code from {@link android.provider.Telephony.Sms.Intents},
     *  or {@link Activity#RESULT_OK} for delayed acknowledgment to SMSC
     */
    CARAPI_(Int32) DispatchMessageRadioSpecific(
        /* [in] */ ISmsMessageBase* smsb);

    /**
     * Send an acknowledge message.
     * @param success indicates that last message was successfully received.
     * @param result result code indicating any error
     * @param response callback message sent when operation completes.
     */
    CARAPI_(void) AcknowledgeLastIncomingSms(
        /* [in] */ Boolean success,
        /* [in] */ Int32 result,
        /* [in] */ IMessage* response);

    /**
     * Called when the phone changes the default method updates mPhone
     * mStorageMonitor and mCellBroadcastHandler.updatePhoneObject.
     * Override if different or other behavior is desired.
     *
     * @param phone
     */
    CARAPI_(void) OnUpdatePhoneObject(
        /* [in] */ IPhoneBase* phone);

private:
    /**
     * Create a new GSM inbound SMS handler.
     */
    GsmInboundSmsHandler(
        /* [in] */ IContext* context,
        /* [in] */ ISmsStorageMonitor* storageMonitor,
        /* [in] */ IPhoneBase* phone);

    /**
     * Convert Android result code to 3GPP SMS failure cause.
     * @param rc the Android SMS intent result value
     * @return 0 for success, or a 3GPP SMS failure cause value
     */
    static CARAPI_(Int32) ResultToCause(
        /* [in] */ Int32 rc);

private:
    /** Handler for SMS-PP data download messages to UICC. */
    AutoPtr<IUsimDataDownloadHandler> mDataDownloadHandler;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_GSMINBOUNDSMSHANDLER_H__
