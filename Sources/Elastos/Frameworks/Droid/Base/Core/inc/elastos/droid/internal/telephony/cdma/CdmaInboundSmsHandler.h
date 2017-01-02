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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMAINBOUNDSMSHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMAINBOUNDSMSHANDLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/telephony/InboundSmsHandler.h"

// import android.app.Activity;
// import android.content.res.Resources;
// import android.os.SystemProperties;
// import android.provider.Telephony.Sms.Intents;
// import android.telephony.SmsCbMessage;
// import com.android.internal.telephony.CellBroadcastHandler;
// import com.android.internal.telephony.CommandsInterface;
// import com.android.internal.telephony.InboundSmsTracker;
// import com.android.internal.telephony.SmsConstants;
// import com.android.internal.telephony.TelephonyProperties;
// import com.android.internal.telephony.WspTypeDecoder;
// import com.android.internal.telephony.cdma.sms.SmsEnvelope;
// import java.util.Arrays;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::InboundSmsHandler;
using Elastos::Droid::Internal::Telephony::ISmsStorageMonitor;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::ISmsMessageBase;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

/**
  * Subclass of {@link InboundSmsHandler} for 3GPP2 type messages.
  */
class CdmaInboundSmsHandler
    : public InboundSmsHandler
    , public ICdmaInboundSmsHandler
{
public:
    CAR_INTERFACE_DECL();

    /**
      * Wait for state machine to enter startup state. We can't send any messages until then.
      */
    static CARAPI_(AutoPtr<ICdmaInboundSmsHandler>) MakeInboundSmsHandler(
        /* [in] */ IContext* context,
        /* [in] */ ISmsStorageMonitor* storageMonitor,
        /* [in] */ IPhoneBase* phone,
        /* [in] */ ICdmaSMSDispatcher* smsDispatcher);

protected:
    /**
      * Unregister for CDMA SMS.
      */
    // @Override
    CARAPI_(void) OnQuitting();

    /**
      * Return true if this handler is for 3GPP2 messages; false for 3GPP format.
      * @return true (3GPP2)
      */
    // @Override
    CARAPI_(Boolean) Is3gpp2();

    /**
      * Process Cell Broadcast, Voicemail Notification, and other 3GPP/3GPP2-specific messages.
      * @param smsb the SmsMessageBase object from the RIL
      * @return true if the message was handled here; false to continue processing
      */
    // @Override
    CARAPI_(Int32) DispatchMessageRadioSpecific(
        /* [in] */ ISmsMessageBase* smsb);

    /**
      * Send an acknowledge message.
      * @param success indicates that last message was successfully received.
      * @param result result code indicating any error
      * @param response callback message sent when operation completes.
      */
    // @Override
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
    // @Override
    CARAPI_(void) OnUpdatePhoneObject(
        /* [in] */ IPhoneBase* phone);

private:
    /**
      * Create a new inbound SMS handler for CDMA.
      */
    CdmaInboundSmsHandler(
        /* [in] */ IContext* context,
        /* [in] */ ISmsStorageMonitor* storageMonitor,
        /* [in] */ IPhoneBase* phone,
        /* [in] */ ICdmaSMSDispatcher* smsDispatcher);

    /**
      * Return whether the device is in Emergency Call Mode (only for 3GPP2).
      * @return true if the device is in ECM; false otherwise
      */
    static CARAPI_(Boolean) IsInEmergencyCallMode();

    /**
      * Convert Android result code to CDMA SMS failure cause.
      * @param rc the Android SMS intent result value
      * @return 0 for success, or a CDMA SMS failure cause value
      */
    static CARAPI_(Int32) ResultToCause(
        /* [in] */ Int32 rc);

    /**
      * Handle {@link SmsEnvelope#TELESERVICE_VMN} and {@link SmsEnvelope#TELESERVICE_MWI}.
      * @param sms the message to process
      */
    CARAPI_(void) HandleVoicemailTeleservice(
        /* [in] */ ISmsMessage* sms);

    /**
      * Processes inbound messages that are in the WAP-WDP PDU format. See
      * wap-259-wdp-20010614-a section 6.5 for details on the WAP-WDP PDU format.
      * WDP segments are gathered until a datagram completes and gets dispatched.
      *
      * @param pdu The WAP-WDP PDU segment
      * @return a result code from {@link android.provider.Telephony.Sms.Intents}, or
      *         {@link Activity#RESULT_OK} if the message has been broadcast
      *         to applications
      */
    CARAPI_(Int32) ProcessCdmaWapPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ Int32 referenceNumber,
        /* [in] */ const String& address,
        /* [in] */ Int64 timestamp);

    /**
      * Optional check to see if the received WapPush is an OMADM notification with erroneous
      * extra port fields.
      * - Some carriers make this mistake.
      * ex: MSGTYPE-TotalSegments-CurrentSegment
      *       -SourcePortDestPort-SourcePortDestPort-OMADM PDU
      * @param origPdu The WAP-WDP PDU segment
      * @param index Current Index while parsing the PDU.
      * @return True if OrigPdu is OmaDM Push Message which has duplicate ports.
      *         False if OrigPdu is NOT OmaDM Push Message which has duplicate ports.
      */
    static CARAPI_(Boolean) CheckDuplicatePortOmadmWapPush(
        /* [in] */ ArrayOf<Byte>* origPdu,
        /* [in] */ Int32 index);

private:
    AutoPtr<ICdmaSMSDispatcher> mSmsDispatcher;
    AutoPtr<ICdmaServiceCategoryProgramHandler> mServiceCategoryProgramHandler;
    AutoPtr<ArrayOf<Byte> > mLastDispatchedSmsFingerprint;
    AutoPtr<ArrayOf<Byte> > mLastAcknowledgedSmsFingerprint;
    Boolean mCheckForDuplicatePortsInOmadmWapPush;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMAINBOUNDSMSHANDLER_H__
