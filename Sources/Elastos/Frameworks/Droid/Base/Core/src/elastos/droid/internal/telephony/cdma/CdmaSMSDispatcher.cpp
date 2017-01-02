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

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/internal/telephony/cdma/CdmaSMSDispatcher.h"
#include "elastos/droid/internal/telephony/cdma/CDMASmsMessage.h"
#include "elastos/droid/internal/telephony/cdma/sms/CUserData.h"
#include "elastos/droid/internal/telephony/CSmsApplication.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::CUserData;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::IUserData;
using Elastos::Droid::Internal::Telephony::CSmsApplication;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::ISmsApplication;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Provider::ITelephonySmsIntents;
using Elastos::Droid::Provider::ITelephonyTextBasedSmsColumns;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Core::CoreUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                          CdmaSMSDispatcher
//=====================================================================
CAR_INTERFACE_IMPL(CdmaSMSDispatcher, SMSDispatcher, ICdmaSMSDispatcher);

const String CdmaSMSDispatcher::TAG("CdmaSMSDispatcher");
const Boolean CdmaSMSDispatcher::VDBG = FALSE;

CdmaSMSDispatcher::CdmaSMSDispatcher()
{
}

ECode CdmaSMSDispatcher::constructor(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ ISmsUsageMonitor* usageMonitor,
    /* [in] */ IImsSMSDispatcher* imsSMSDispatcher)
{
    SMSDispatcher::constructor(phone, usageMonitor, imsSMSDispatcher);
    Logger::D(TAG, "CdmaSMSDispatcher created");
    return NOERROR;
}

ECode CdmaSMSDispatcher::SendStatusReportMessage(
    /* [in] */ ISmsMessage* sms)
{
    VALIDATE_NOT_NULL(sms);
    if (VDBG) Logger::D(TAG, "sending EVENT_HANDLE_STATUS_REPORT message");
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_HANDLE_STATUS_REPORT, sms, (IMessage**)&msg);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode CdmaSMSDispatcher::HandleCdmaStatusReport(
    /* [in] */ ISmsMessage* sms)
{
    VALIDATE_NOT_NULL(sms);
    Int32 count;
    mDeliveryPendingList->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        mDeliveryPendingList->Get(i, (IInterface**)&obj);
        AutoPtr<SmsTracker> tracker = (SmsTracker*)(IObject*)obj.Get();

        if (tracker->mMessageRef == ((CDMASmsMessage*)sms)->mMessageRef) {
            // Found it.  Remove from list and broadcast.
            mDeliveryPendingList->Remove(i);
            // Update the message status (COMPLETE)
            tracker->UpdateSentMessageStatus(mContext, ITelephonyTextBasedSmsColumns::STATUS_COMPLETE);

            AutoPtr<IPendingIntent> intent = tracker->mDeliveryIntent;
            AutoPtr<IIntent> fillIn;
            CIntent::New((IIntent**)&fillIn);
            AutoPtr<ArrayOf<Byte> > array;
            ISmsMessageBase::Probe(sms)->GetPdu((ArrayOf<Byte>**)&array);
            fillIn->PutByteArrayExtra(String("pdu"), array);
            String str;
            GetFormat(&str);
            fillIn->PutExtra(String("format"), str);
            // try {
            intent->Send(mContext, IActivity::RESULT_OK, fillIn);
            // } catch (CanceledException ex) {}
            break;  // Only expect to see one tracker matching this message.
        }
    }
    return NOERROR;
}

ECode CdmaSMSDispatcher::GetFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ISmsConstants::FORMAT_3GPP2;
    return NOERROR;
}

ECode CdmaSMSDispatcher::HandleStatusReport(
    /* [in] */ IInterface* o)
{
    if (ISmsMessage::Probe(o) != NULL) {
        if (VDBG) Logger::D(TAG, "calling handleCdmaStatusReport()");
        HandleCdmaStatusReport(ISmsMessage::Probe(o));
    }
    else {
        // Logger::E(TAG, "handleStatusReport() called for object type " + o.getClass().getName());
    }
    return NOERROR;
}

ECode CdmaSMSDispatcher::SendData(
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ Int32 origPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    AutoPtr<ISmsMessageSubmitPdu> pdu = CDMASmsMessage::GetSubmitPdu(
            scAddr, destAddr, destPort, origPort, data, (deliveryIntent != NULL));
    AutoPtr<IHashMap> map;
// TODO: Need SMSDispatcher::GetSmsTrackerMap
    // map = GetSmsTrackerMap(destAddr, scAddr, destPort, origPort, data, pdu);
    String str;
    GetFormat(&str);
    AutoPtr<SmsTracker> tracker;
// TODO: Need SMSDispatcher::GetSmsTracker
    // tracker = GetSmsTracker(map, sentIntent, deliveryIntent, str, NULL /*messageUri*/, FALSE);
    SendSubmitPdu(tracker);
    return NOERROR;
}

ECode CdmaSMSDispatcher::SendText(
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ const String& text,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent,
    /* [in] */ IUri* messageUri,
    /* [in] */ const String& callingPkg,
    /* [in] */ Int32 priority,
    /* [in] */ Boolean isExpectMore,
    /* [in] */ Int32 validityPeriod)
{
    AutoPtr<ISmsMessageSubmitPdu> pdu = CDMASmsMessage::GetSubmitPdu(
            scAddr, destAddr, text, (deliveryIntent != NULL), NULL, priority);
    if (pdu != NULL) {
        if (messageUri == NULL) {
            Boolean b;

            AutoPtr<ISmsApplication> application;
            CSmsApplication::AcquireSingleton((ISmsApplication**)&application);
            if (application->ShouldWriteMessageForPackage(callingPkg, mContext, &b), b) {
                messageUri = WriteOutboxMessage(
                        GetSubId(),
                        destAddr,
                        text,
                        deliveryIntent != NULL,
                        callingPkg);
            }
        }
        else {
            MoveToOutbox(GetSubId(), messageUri, callingPkg);
        }
        AutoPtr<IHashMap> map;
// TODO: Need SMSDispatcher::GetSmsTrackerMap
        // mag = GetSmsTrackerMap(destAddr, scAddr, text, pdu);
        String str;
        GetFormat(&str);
        AutoPtr<SmsTracker> tracker;
// TODO: Need SMSDispatcher::GetSmsTracker
        // track = GetSmsTracker(map, sentIntent, deliveryIntent, str,
        //         messageUri, isExpectMore, validityPeriod);
        SendSubmitPdu(tracker);
    }
    else {
        Logger::E(TAG, "CdmaSMSDispatcher.sendText(): getSubmitPdu() returned NULL");
    }
    return NOERROR;
}

ECode CdmaSMSDispatcher::InjectSmsPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ const String& format,
    /* [in] */ IPendingIntent* receivedIntent)
{
    // throw new IllegalStateException("This method must be called only on ImsSMSDispatcher");
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode CdmaSMSDispatcher::CalculateLength(
    /* [in] */ ICharSequence* messageBody,
    /* [in] */ Boolean use7bitOnly,
    /* [out] */ IGsmAlphabetTextEncodingDetails** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IGsmAlphabetTextEncodingDetails> gsm = CDMASmsMessage::CalculateLength(messageBody, use7bitOnly);
    *result = gsm;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CdmaSMSDispatcher::SendNewSubmitPdu(
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
    /* [in] */ IUri* messageUri)
{
    AutoPtr<CUserData> uData;
    CUserData::NewByFriend((CUserData**)&uData);
    uData->payloadStr = message;
    uData->userDataHeader = smsHeader;
    if (encoding == ISmsConstants::ENCODING_7BIT) {
        uData->msgEncoding = IUserData::ENCODING_GSM_7BIT_ALPHABET;
        AutoPtr<IContext> context;
        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        Boolean ascii7bitForLongMsg;
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        res->GetBoolean(R::bool_::config_ascii_7bit_support_for_long_message, &ascii7bitForLongMsg);
        if (ascii7bitForLongMsg) {
            Logger::D(TAG, "ascii7bitForLongMsg = %d", ascii7bitForLongMsg);
            uData->msgEncoding = IUserData::ENCODING_7BIT_ASCII;
        }
    }
    else { // assume UTF-16
        uData->msgEncoding = IUserData::ENCODING_UNICODE_16;
    }
    uData->msgEncodingSet = TRUE;

    /* By setting the statusReportRequested bit only for the
     * last message fragment, this will result in only one
     * callback to the sender when that last fragment delivery
     * has been acknowledged. */
    AutoPtr<ISmsMessageSubmitPdu> submitPdu = CDMASmsMessage::GetSubmitPdu(destinationAddress,
            uData, (deliveryIntent != NULL) && lastPart, priority);

    AutoPtr<IHashMap> map;
// TODO: Need SMSDispatcher::GetSmsTrackerMap
    // map = GetSmsTrackerMap(destinationAddress, scAddress, message, submitPdu);
    String str;
    GetFormat(&str);
    AutoPtr<SmsTracker> tracker;
// TODO: Need SMSDispatcher::GetSmsTracker
    // tracker = GetSmsTracker(map, sentIntent,
    //         deliveryIntent, str, unsentPartCount, anyPartFailed, messageUri, smsHeader,
    //         (!lastPart || isExpectMore), validityPeriod);
    SendSubmitPdu(tracker);
    return NOERROR;
}

void CdmaSMSDispatcher::SendSubmitPdu(
    /* [in] */ SMSDispatcher::SmsTracker* tracker)
{
    Boolean b;
    if (SystemProperties::GetBoolean(ITelephonyProperties::PROPERTY_INECM_MODE, FALSE, &b), b) {
        if (VDBG) {
            Logger::D(TAG, "Block SMS in Emergency Callback mode");
        }
        tracker->OnFailed(mContext, ISmsManager::RESULT_ERROR_NO_SERVICE, 0/*errorCode*/);
        return;
    }
    SendRawPdu(tracker);
}

void CdmaSMSDispatcher::SendSms(
    /* [in] */ SMSDispatcher::SmsTracker* tracker)
{
    AutoPtr<IHashMap> map = tracker->mData;

    // byte[] smsc = (byte[]) map.get("smsc");  // unused for CDMA
    AutoPtr<IInterface> obj;
    map->Get(CoreUtils::Convert(String("pdu")), (IInterface**)&obj);

    AutoPtr<IArrayOf> iArray = IArrayOf::Probe(obj);

    // Logger::D(TAG, "sendSms: "
    //         + " isIms()=" + isIms()
    //         + " mRetryCount=" + tracker->mRetryCount
    //         + " mImsRetry=" + tracker->mImsRetry
    //         + " mMessageRef=" + tracker->mMessageRef
    //         + " SS=" + mPhone.getServiceState().getState());

    // Send SMS via the carrier app.
    AutoPtr<SMSDispatcherReceiver> resultReceiver = new SMSDispatcherReceiver(tracker, this);

    // Direct the intent to only the default carrier app.
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonySmsIntents::SMS_SEND_ACTION, (IIntent**)&intent);
    String carrierPackage = GetCarrierAppPackageName(intent);
    if (carrierPackage != NULL) {
        intent->SetPackage(GetCarrierAppPackageName(intent));

        Int32 size;
        iArray->GetLength(&size);
        AutoPtr<ArrayOf<Byte> > pdu = ArrayOf<Byte>::Alloc(size);

        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            iArray->Get(i, (IInterface**)&obj);
            Byte byte;
            IByte::Probe(obj)->GetValue(&byte);
            (*pdu)[i] = byte;
        }
        intent->PutByteArrayExtra(String("pdu"), pdu);
        String str;
        GetFormat(&str);
        intent->PutExtra(String("format"), str);
// TODO: Need SmsHeader.ConcatRef
        // if (tracker->mSmsHeader != NULL && tracker->mSmsHeader->concatRef != NULL) {
        //     AutoPtr<SmsHeader.ConcatRef concatRef = tracker->mSmsHeader->concatRef;
        //     intent->PutExtra(String("concat.refNumber"), concatRef.refNumber);
        //     intent->PutExtra(String("concat.seqNumber"), concatRef.seqNumber);
        //     intent->PutExtra(String("concat.msgCount"), concatRef.msgCount);
        // }
        intent->AddFlags(IIntent::FLAG_RECEIVER_NO_ABORT);
        Logger::D(TAG, "Sending SMS by carrier app.");
        mContext->SendOrderedBroadcast(intent, Manifest::permission::RECEIVE_SMS,
                                      IAppOpsManager::OP_RECEIVE_SMS,
                                      IBroadcastReceiver::Probe(resultReceiver),
                                      NULL, IActivity::RESULT_CANCELED, String(NULL), NULL);
    }
    else {
        SendSmsByPstn(tracker);
    }
}

void CdmaSMSDispatcher::UpdateSmsSendStatus(
    /* [in] */ Int32 messageRef,
    /* [in] */ Boolean success)
{
    // This function should be defined in ImsDispatcher.
    Logger::E(TAG, "updateSmsSendStatus should never be called from here!");
}

void CdmaSMSDispatcher::SendSmsByPstn(
    /* [in] */ SMSDispatcher::SmsTracker* tracker)
{
    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 ss;
    serviceState->GetState(&ss);
    // if sms over IMS is not supported on data and voice is not available...
    Boolean b;
    if ((IsIms(&b), !b) && ss != IServiceState::STATE_IN_SERVICE) {
        tracker->OnFailed(mContext, GetNotInServiceError(ss), 0/*errorCode*/);
        return;
    }

    AutoPtr<IMessage> reply;
    ObtainMessage(EVENT_SEND_SMS_COMPLETE, (IInterface*)(IObject*)tracker, (IMessage**)&reply);

    AutoPtr<IInterface> obj;
    tracker->mData->Get(CoreUtils::Convert(String("pdu")), (IInterface**)&obj);

    AutoPtr<IArrayOf> iArray = IArrayOf::Probe(obj);
    Int32 size;
    iArray->GetLength(&size);
    AutoPtr<ArrayOf<Byte> > pdu = ArrayOf<Byte>::Alloc(size);

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        iArray->Get(i, (IInterface**)&obj);
        Byte byte;
        IByte::Probe(obj)->GetValue(&byte);
        (*pdu)[i] = byte;
    }

    // sms over cdma is used:
    //   if sms over IMS is not supported AND
    //   this is not a retry case after sms over IMS failed
    //     indicated by mImsRetry > 0
    if (0 == tracker->mImsRetry && (IsIms(&b), !b)) {
        mCi->SendCdmaSms(pdu, reply);
    }
    // If sending SMS over IMS is not enabled, send SMS over cdma. Simply
    // calling shouldSendSmsOverIms() to check for that here might yield a
    // different result if the conditions of UE being attached to eHRPD and
    // active 1x voice call have changed since we last called it in
    // ImsSMSDispatcher.isCdmaMo()
    else if (mImsSMSDispatcher->IsImsSmsEnabled(&b), !b) {
        mCi->SendCdmaSms(pdu, reply);
        mImsSMSDispatcher->EnableSendSmsOverIms(TRUE);
    }
    else {
        mCi->SendImsCdmaSms(pdu, tracker->mImsRetry, tracker->mMessageRef, reply);
        // increment it here, so in case of SMS_FAIL_RETRY over IMS
        // next retry will be sent using IMS request again.
        tracker->mImsRetry++;
    }
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
