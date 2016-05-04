/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.cdma;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Provider::Telephony::ISms;
using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISmsManager;

using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::IImsSMSDispatcher;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::ISMSDispatcher;
using Elastos::Droid::Internal::Telephony::ISmsApplication;
using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Telephony::ISmsHeader;
using Elastos::Droid::Internal::Telephony::ISmsUsageMonitor;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::IUserData;

using Elastos::Utility::IHashMap;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger;

public class CdmaSMSDispatcher extends SMSDispatcher {
    private static const String TAG = "CdmaSMSDispatcher";
    private static const Boolean VDBG = FALSE;

    public CdmaSMSDispatcher(PhoneBase phone, SmsUsageMonitor usageMonitor,
            ImsSMSDispatcher imsSMSDispatcher) {
        Super(phone, usageMonitor, imsSMSDispatcher);
        Rlog->D(TAG, "CdmaSMSDispatcher created");
    }

    //@Override
    protected String GetFormat() {
        return SmsConstants.FORMAT_3GPP2;
    }

    /**
     * Send the SMS status report to the dispatcher thread to process.
     * @param sms the CDMA SMS message containing the status report
     */
    void SendStatusReportMessage(SmsMessage sms) {
        If (VDBG) Rlog->D(TAG, "sending EVENT_HANDLE_STATUS_REPORT message");
        SendMessage(ObtainMessage(EVENT_HANDLE_STATUS_REPORT, sms));
    }

    //@Override
    protected void HandleStatusReport(Object o) {
        If (o instanceof SmsMessage) {
            If (VDBG) Rlog->D(TAG, "calling HandleCdmaStatusReport()");
            HandleCdmaStatusReport((SmsMessage) o);
        } else {
            Rlog->E(TAG, "HandleStatusReport() called for object type " + o->GetClass()->GetName());
        }
    }

    /**
     * Called from parent class to handle status report from {@code CdmaInboundSmsHandler}.
     * @param sms the CDMA SMS message to process
     */
    void HandleCdmaStatusReport(SmsMessage sms) {
        For (Int32 i = 0, count = deliveryPendingList->Size(); i < count; i++) {
            SmsTracker tracker = deliveryPendingList->Get(i);
            If (tracker.mMessageRef == sms.mMessageRef) {
                // Found it.  Remove from list and broadcast.
                deliveryPendingList->Remove(i);
                // Update the message Status (COMPLETE)
                tracker->UpdateSentMessageStatus(mContext, Sms.STATUS_COMPLETE);

                PendingIntent intent = tracker.mDeliveryIntent;
                Intent fillIn = new Intent();
                fillIn->PutExtra("pdu", sms->GetPdu());
                fillIn->PutExtra("format", GetFormat());
                try {
                    intent->Send(mContext, Activity.RESULT_OK, fillIn);
                } Catch (CanceledException ex) {}
                break;  // Only expect to see one tracker matching this message.
            }
        }
    }

    /** {@inheritDoc} */
    //@Override
    protected void SendData(String destAddr, String scAddr, Int32 destPort, Int32 origPort,
            Byte[] data, PendingIntent sentIntent, PendingIntent deliveryIntent) {
        SmsMessage.SubmitPdu pdu = SmsMessage->GetSubmitPdu(
                scAddr, destAddr, destPort, origPort, data, (deliveryIntent != NULL));
        HashMap map = GetSmsTrackerMap(destAddr, scAddr, destPort, origPort, data, pdu);
        SmsTracker tracker = GetSmsTracker(map, sentIntent, deliveryIntent, GetFormat(),
                NULL /*messageUri*/, FALSE);
        SendSubmitPdu(tracker);
    }

    /** {@inheritDoc} */
    //@Override
    protected void SendText(String destAddr, String scAddr, String text, PendingIntent sentIntent,
            PendingIntent deliveryIntent, Uri messageUri, String callingPkg,
            Int32 priority, Boolean isExpectMore, Int32 validityPeriod) {
        SmsMessage.SubmitPdu pdu = SmsMessage->GetSubmitPdu(
                scAddr, destAddr, text, (deliveryIntent != NULL), NULL, priority);
        If (pdu != NULL) {
            If (messageUri == NULL) {
                If (SmsApplication->ShouldWriteMessageForPackage(callingPkg, mContext)) {
                    messageUri = WriteOutboxMessage(
                            GetSubId(),
                            destAddr,
                            text,
                            deliveryIntent != NULL,
                            callingPkg);
                }
            } else {
                MoveToOutbox(GetSubId(), messageUri, callingPkg);
            }
            HashMap map = GetSmsTrackerMap(destAddr, scAddr, text, pdu);
            SmsTracker tracker = GetSmsTracker(map, sentIntent, deliveryIntent, GetFormat(),
                    messageUri, isExpectMore, validityPeriod);
            SendSubmitPdu(tracker);
        } else {
            Rlog->E(TAG, "CdmaSMSDispatcher->SendText(): GetSubmitPdu() returned NULL");
        }
    }

    /** {@inheritDoc} */
    //@Override
    protected void InjectSmsPdu(Byte[] pdu, String format, PendingIntent receivedIntent) {
        throw new IllegalStateException("This method must be called only on ImsSMSDispatcher");
    }

    /** {@inheritDoc} */
    //@Override
    protected GsmAlphabet.TextEncodingDetails CalculateLength(CharSequence messageBody,
            Boolean use7bitOnly) {
        return SmsMessage->CalculateLength(messageBody, use7bitOnly);
    }

    /** {@inheritDoc} */
    //@Override
    protected void SendNewSubmitPdu(String destinationAddress, String scAddress,
            String message, SmsHeader smsHeader, Int32 encoding,
            PendingIntent sentIntent, PendingIntent deliveryIntent, Boolean lastPart,
            Int32 priority, Boolean isExpectMore, Int32 validityPeriod,
            AtomicInteger unsentPartCount, AtomicBoolean anyPartFailed, Uri messageUri) {
        UserData uData = new UserData();
        uData.payloadStr = message;
        uData.userDataHeader = smsHeader;
        If (encoding == SmsConstants.ENCODING_7BIT) {
            uData.msgEncoding = UserData.ENCODING_GSM_7BIT_ALPHABET;
            Context context = mPhone->GetContext();
            Boolean ascii7bitForLongMsg = context->GetResources().
                GetBoolean(R.bool.config_ascii_7bit_support_for_long_message);
            If (ascii7bitForLongMsg) {
                Rlog->D(TAG, "ascii7bitForLongMsg = " + ascii7bitForLongMsg);
                uData.msgEncoding = UserData.ENCODING_7BIT_ASCII;
            }
        } else { // assume UTF-16
            uData.msgEncoding = UserData.ENCODING_UNICODE_16;
        }
        uData.msgEncodingSet = TRUE;

        /* By setting the statusReportRequested bit only for the
         * last message fragment, this will result in only one
         * callback to the sender when that last fragment delivery
         * has been acknowledged. */
        SmsMessage.SubmitPdu submitPdu = SmsMessage->GetSubmitPdu(destinationAddress,
                uData, (deliveryIntent != NULL) && lastPart, priority);

        HashMap map = GetSmsTrackerMap(destinationAddress, scAddress,
                message, submitPdu);
        SmsTracker tracker = GetSmsTracker(map, sentIntent,
                deliveryIntent, GetFormat(), unsentPartCount, anyPartFailed, messageUri, smsHeader,
                (!lastPart || isExpectMore), validityPeriod);
        SendSubmitPdu(tracker);
    }

    protected void SendSubmitPdu(SmsTracker tracker) {
        If (SystemProperties->GetBoolean(TelephonyProperties.PROPERTY_INECM_MODE, FALSE)) {
            If (VDBG) {
                Rlog->D(TAG, "Block SMS in Emergency Callback mode");
            }
            tracker->OnFailed(mContext, SmsManager.RESULT_ERROR_NO_SERVICE, 0/*errorCode*/);
            return;
        }
        SendRawPdu(tracker);
    }

    /** {@inheritDoc} */
    //@Override
    protected void SendSms(SmsTracker tracker) {
        HashMap<String, Object> map = tracker.mData;

        // Byte[] smsc = (Byte[]) map->Get("smsc");  // unused for CDMA
        Byte[] pdu = (Byte[]) map->Get("pdu");

        Rlog->D(TAG, "sendSms: "
                + " IsIms()=" + IsIms()
                + " mRetryCount=" + tracker.mRetryCount
                + " mImsRetry=" + tracker.mImsRetry
                + " mMessageRef=" + tracker.mMessageRef
                + " SS=" + mPhone->GetServiceState()->GetState());

        // Send SMS via the carrier app.
        BroadcastReceiver resultReceiver = new SMSDispatcherReceiver(tracker);

        // Direct the intent to only the default carrier app.
        Intent intent = new Intent(Intents.SMS_SEND_ACTION);
        String carrierPackage = GetCarrierAppPackageName(intent);
        If (carrierPackage != NULL) {
            intent->SetPackage(GetCarrierAppPackageName(intent));
            intent->PutExtra("pdu", pdu);
            intent->PutExtra("format", GetFormat());
            If (tracker.mSmsHeader != NULL && tracker.mSmsHeader.concatRef != NULL) {
                SmsHeader.ConcatRef concatRef = tracker.mSmsHeader.concatRef;
                intent->PutExtra("concat.refNumber", concatRef.refNumber);
                intent->PutExtra("concat.seqNumber", concatRef.seqNumber);
                intent->PutExtra("concat.msgCount", concatRef.msgCount);
            }
            intent->AddFlags(IIntent::FLAG_RECEIVER_NO_ABORT);
            Rlog->D(TAG, "Sending SMS by carrier app.");
            mContext->SendOrderedBroadcast(intent, Manifest::permission::RECEIVE_SMS,
                                          AppOpsManager.OP_RECEIVE_SMS, resultReceiver,
                                          NULL, Activity.RESULT_CANCELED, NULL, NULL);
        } else {
            SendSmsByPstn(tracker);
        }
    }

    /** {@inheritDoc} */
    //@Override
    protected void UpdateSmsSendStatus(Int32 messageRef, Boolean success) {
        // This function should be defined in ImsDispatcher.
        Rlog->E(TAG, "updateSmsSendStatus should never be called from here!");
    }

    /** {@inheritDoc} */
    //@Override
    protected void SendSmsByPstn(SmsTracker tracker) {
        Int32 ss = mPhone->GetServiceState()->GetState();
        // if sms over IMS is not supported on data and voice is not available...
        If (!IsIms() && ss != ServiceState.STATE_IN_SERVICE) {
            tracker->OnFailed(mContext, GetNotInServiceError(ss), 0/*errorCode*/);
            return;
        }

        Message reply = ObtainMessage(EVENT_SEND_SMS_COMPLETE, tracker);
        Byte[] pdu = (Byte[]) tracker.mData->Get("pdu");

        // sms over cdma is used:
        //   if sms over IMS is not supported AND
        //   this is not a retry case after sms over IMS failed
        //     indicated by mImsRetry > 0
        If (0 == tracker.mImsRetry && !IsIms()) {
            mCi->SendCdmaSms(pdu, reply);
        }
        // If sending SMS over IMS is not enabled, send SMS over cdma. Simply
        // calling ShouldSendSmsOverIms() to check for that here might yield a
        // different result if the conditions of UE being attached to eHRPD and
        // active 1x voice call have changed since we last called it in
        // ImsSMSDispatcher->IsCdmaMo()
        else If (!mImsSMSDispatcher->IsImsSmsEnabled()) {
            mCi->SendCdmaSms(pdu, reply);
            mImsSMSDispatcher->EnableSendSmsOverIms(TRUE);
        }
        else {
            mCi->SendImsCdmaSms(pdu, tracker.mImsRetry, tracker.mMessageRef, reply);
            // increment it here, so in case of SMS_FAIL_RETRY over IMS
            // next retry will be sent using IMS request again.
            tracker.mImsRetry++;
        }
    }
}
