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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSSMSDISPATCHER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSSMSDISPATCHER_H__

#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/SMSDispatcher.h"
#include "elastos/droid/internal/telephony/SmsStorageMonitor.h"
#include "elastos/droid/internal/telephony/SmsUsageMonitor.h"

using Elastos::Droid::Internal::Telephony::Cdma::ICdmaInboundSmsHandler;
using Elastos::Droid::Internal::Telephony::Gsm::IGsmInboundSmsHandler;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class ImsSMSDispatcher
    : public SMSDispatcher
    , public IImsSMSDispatcher
{
public:
    CAR_INTERFACE_DECL();

    TO_STRING_IMPL("ImsSMSDispatcher");

    ImsSMSDispatcher(
        /* [in] */ IPhoneBase* phone,
        /* [in] */ SmsStorageMonitor* storageMonitor,
        /* [in] */ SmsUsageMonitor* usageMonitor);

    CARAPI Dispose();

    /**
     * Handles events coming from the phone stack. Overridden from handler.
     *
     * @param msg the message to handle
     */
    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    // @Override
    CARAPI SendRetrySms(
        /* [in] */ SmsTracker* tracker);

    // @Override
    CARAPI IsIms(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetImsSmsFormat(
        /* [out] */ String* result);

    /**
     * Enables MO SMS over IMS
     *
     * @param enable
     */
    CARAPI EnableSendSmsOverIms(
        /* [in] */ Boolean enable);

    /**
     * Determines whether MO SMS over IMS is currently enabled.
     *
     * @return TRUE if MO SMS over IMS is enabled, FALSE otherwise.
     */
    CARAPI IsImsSmsEnabled(
        /* [out] */ Boolean* result);

    /**
     * Determines whether SMS should be sent over IMS if UE is attached to eHRPD
     * and there is an active voice call
     *
     * @return TRUE if SMS should be sent over IMS based on value in config.xml
     *         or system property FALSE otherwise
     */
    CARAPI ShouldSendSmsOverIms(
        /* [out] */ Boolean* result);

    /* Updates the phone object when there is a change */
    // @Override
protected:
    CARAPI_(void) UpdatePhoneObject(
        /* [in] */ IPhoneBase* phone);

    // @Override
    CARAPI SendData(
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ Int32 destPort,
        /* [in] */ Int32 origPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    // @Override
    CARAPI_(void) SendMultipartText(
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ IArrayList/*<String>*/* parts,
        /* [in] */ IArrayList/*<PendingIntent>*/* sentIntents,
        /* [in] */ IArrayList/*<PendingIntent>*/* deliveryIntents,
        /* [in] */ IUri* messageUri,
        /* [in] */ const String& callingPkg,
        /* [in] */ Int32 priority,
        /* [in] */ Boolean isExpectMore,
        /* [in] */ Int32 validityPeriod);

    // @Override
    CARAPI_(void) SendSms(
        /* [in] */ SmsTracker* tracker);

    // @Override
    CARAPI_(void) SendSmsByPstn(
        /* [in] */ SmsTracker* tracker);

    // @Override
    CARAPI_(void) UpdateSmsSendStatus(
        /* [in] */ Int32 messageRef,
        /* [in] */ Boolean success);

    // @Override
    CARAPI InjectSmsPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ const String& format,
        /* [in] */ IPendingIntent* receivedIntent);

    // @Override
    CARAPI GetFormat(
        /* [out] */ String* result);

    // @Override
    CARAPI CalculateLength(
        /* [in] */ ICharSequence* messageBody,
        /* [in] */ Boolean use7bitOnly,
        /* [out] */ IGsmAlphabetTextEncodingDetails** result);

    // @Override
    CARAPI SendNewSubmitPdu(
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& scAddress,
        /* [in] */ const String& message,
        /* [in] */ ISmsHeader* smsHeader,
        /* [in] */ Int32 format,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent,
        /* [in] */ Boolean lastPart,
        /* [in] */ Int32 priority,
        /* [in] */ Boolean isExpectMore,
        /* [in] */ Int32 validityPeriod,
        /* [in] */ IAtomicInteger32* unsentPartCount,
        /* [in] */ IAtomicBoolean* anyPartFailed,
        /* [in] */ IUri* messageUri);

    // @Override
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
        /* [in] */ Int32 validityPeriod);

private:
    CARAPI_(void) SetImsSmsFormat(
        /* [in] */ Int32 format);

    CARAPI_(void) UpdateImsInfo(
        /* [in] */ AsyncResult* ar);

    /**
     * Find a tracker in a list to update its status. If the status is successful,
     * send an EVENT_SEND_SMS_COMPLETE message. Otherwise, resend the message by PSTN if
     * feasible.
     *
     * @param messageRef the reference number of the tracker.
     * @param sendPendingList the list of trackers to look into.
     * @param smsDispatcher the dispatcher for resending the message by PSTN.
     * @param success TRUE iff the message was sent successfully.
     */
    CARAPI_(void) UpdateSmsSendStatusHelper(
        /* [in] */ Int32 messageRef,
        /* [in] */ IList/*<SmsTracker>*/* sendPendingList,
        /* [in] */ SMSDispatcher* smsDispatcher,
        /* [in] */ Boolean success);

    /**
     * Determines whether or not to use CDMA format for MO SMS.
     * If SMS over IMS is supported, then format is based on IMS SMS format,
     * otherwise format is based on current phone type.
     *
     * @return TRUE if Cdma format should be used for MO SMS, FALSE otherwise.
     */
    CARAPI_(Boolean) IsCdmaMo();

    /**
     * Determines whether or not format given is CDMA format.
     *
     * @param format
     * @return TRUE if format given is CDMA format, FALSE otherwise.
     */
    CARAPI_(Boolean) IsCdmaFormat(
        /* [in] */ const String& format);

private:
    static const String TAG;

    AutoPtr<ISMSDispatcher> mCdmaDispatcher;
    AutoPtr<ISMSDispatcher> mGsmDispatcher;

    AutoPtr<IGsmInboundSmsHandler> mGsmInboundSmsHandler;
    AutoPtr<ICdmaInboundSmsHandler> mCdmaInboundSmsHandler;


    /** TRUE if IMS is registered and sms is supported, FALSE otherwise.*/
    Boolean mIms;
    String mImsSmsFormat;

    /**
     * TRUE if MO SMS over IMS is enabled. Default value is TRUE. FALSE for
     * carriers with config_send_sms1x_on_voice_call = TRUE when attached to
     * eHRPD and during active 1x voice call
     */
    Boolean mImsSmsEnabled;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSSMSDISPATCHER_H__
