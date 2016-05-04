/*
 * Copyright (C) 2006 The Android Open Source Project
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

package com.android.internal.telephony.gsm;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Provider::Telephony::ISms;
using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IServiceState;

using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::IImsSMSDispatcher;
using Elastos::Droid::Internal::Telephony::IInboundSmsHandler;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::ISMSDispatcher;
using Elastos::Droid::Internal::Telephony::ISmsApplication;
using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Telephony::ISmsHeader;
using Elastos::Droid::Internal::Telephony::ISmsUsageMonitor;
using Elastos::Droid::Internal::Telephony::Uicc::IIccConstants;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;
using Elastos::Droid::Internal::Telephony::Uicc::ISIMRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;

using Elastos::Utility::IHashMap;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger;
using Elastos::Utility::Concurrent::Atomic::IAtomicReference;

public class GsmSMSDispatcher extends SMSDispatcher {
    private static const String TAG = "GsmSMSDispatcher";
    private static const Boolean VDBG = FALSE;
    protected UiccController mUiccController = NULL;
    private AtomicReference<IccRecords> mIccRecords = new AtomicReference<IccRecords>();
    private AtomicReference<UiccCardApplication> mUiccApplication =
            new AtomicReference<UiccCardApplication>();
    private GsmInboundSmsHandler mGsmInboundSmsHandler;

    /** Status report received */
    private static const Int32 EVENT_NEW_SMS_STATUS_REPORT = 100;

    public GsmSMSDispatcher(PhoneBase phone, SmsUsageMonitor usageMonitor,
            ImsSMSDispatcher imsSMSDispatcher,
            GsmInboundSmsHandler gsmInboundSmsHandler) {
        Super(phone, usageMonitor, imsSMSDispatcher);
        mCi->SetOnSmsStatus(this, EVENT_NEW_SMS_STATUS_REPORT, NULL);
        mCi->SetOnSmsOnSim(this, EVENT_SMS_ON_ICC, NULL);
        mGsmInboundSmsHandler = gsmInboundSmsHandler;
        mUiccController = UiccController->GetInstance();
        mUiccController->RegisterForIccChanged(this, EVENT_ICC_CHANGED, NULL);
        Rlog->D(TAG, "GsmSMSDispatcher created");
    }

    //@Override
    CARAPI Dispose() {
        super->Dispose();
        mCi->UnSetOnSmsStatus(this);
        mUiccController->UnregisterForIccChanged(this);
        mCi->UnSetOnSmsOnSim(this);
        If (mIccRecords->Get() != NULL) {
            mIccRecords->Get()->UnregisterForNewSms(this);
        }
    }

    //@Override
    protected String GetFormat() {
        return SmsConstants.FORMAT_3GPP;
    }

    /**
     * Handles 3GPP format-specific events coming from the phone stack.
     * Other events are handled by {@link SMSDispatcher#handleMessage}.
     *
     * @param msg the message to handle
     */
    //@Override
    CARAPI HandleMessage(Message msg) {
        Switch (msg.what) {
        case EVENT_NEW_SMS_STATUS_REPORT:
            HandleStatusReport((AsyncResult) msg.obj);
            break;

        case EVENT_NEW_ICC_SMS:
        // pass to InboundSmsHandler to process
        mGsmInboundSmsHandler->SendMessage(InboundSmsHandler.EVENT_NEW_SMS, msg.obj);
        break;

        case EVENT_ICC_CHANGED:
            OnUpdateIccAvailability();
            break;

        case EVENT_SMS_ON_ICC:
            If (mIccRecords->Get() != NULL) {
                ((SIMRecords)(mIccRecords->Get())).HandleSmsOnIcc((AsyncResult) msg.obj);
            }
            break;

        default:
            super->HandleMessage(msg);
        }
    }

    /**
     * Called when a status report is received.  This should correspond to
     * a previously successful SEND.
     *
     * @param ar AsyncResult passed into the message handler.  ar.result should
     *           be a String representing the status report PDU, as ASCII hex.
     */
    private void HandleStatusReport(AsyncResult ar) {
        String pduString = (String) ar.result;
        SmsMessage sms = SmsMessage->NewFromCDS(pduString);

        If (sms != NULL) {
            Int32 tpStatus = sms->GetStatus();
            Int32 messageRef = sms.mMessageRef;
            For (Int32 i = 0, count = deliveryPendingList->Size(); i < count; i++) {
                SmsTracker tracker = deliveryPendingList->Get(i);
                If (tracker.mMessageRef == messageRef) {
                    // Found it.  Remove from list and broadcast.
                    If(tpStatus >= Sms.STATUS_FAILED || tpStatus < Sms.STATUS_PENDING ) {
                       deliveryPendingList->Remove(i);
                       // Update the message Status (COMPLETE or FAILED)
                       tracker->UpdateSentMessageStatus(mContext, tpStatus);
                    }
                    PendingIntent intent = tracker.mDeliveryIntent;
                    Intent fillIn = new Intent();
                    fillIn->PutExtra("pdu", IccUtils->HexStringToBytes(pduString));
                    fillIn->PutExtra("format", GetFormat());
                    try {
                        intent->Send(mContext, Activity.RESULT_OK, fillIn);
                    } Catch (CanceledException ex) {}

                    // Only expect to see one tracker matching this messageref
                    break;
                }
            }
        }
        mCi->AcknowledgeLastIncomingGsmSms(TRUE, Intents.RESULT_SMS_HANDLED, NULL);
    }

    /** {@inheritDoc} */
    //@Override
    protected void SendData(String destAddr, String scAddr, Int32 destPort, Int32 origPort,
            Byte[] data, PendingIntent sentIntent, PendingIntent deliveryIntent) {
        SmsMessage.SubmitPdu pdu = SmsMessage->GetSubmitPdu(
                scAddr, destAddr, destPort, origPort, data, (deliveryIntent != NULL));
        If (pdu != NULL) {
            HashMap map = GetSmsTrackerMap(destAddr, scAddr, destPort, origPort, data, pdu);
            SmsTracker tracker = GetSmsTracker(map, sentIntent, deliveryIntent, GetFormat(),
                    NULL /*messageUri*/, FALSE);
            SendRawPdu(tracker);
        } else {
            Rlog->E(TAG, "GsmSMSDispatcher->SendData(): GetSubmitPdu() returned NULL");
        }
    }

    /** {@inheritDoc} */
    //@Override
    protected void SendText(String destAddr, String scAddr, String text, PendingIntent sentIntent,
            PendingIntent deliveryIntent, Uri messageUri, String callingPkg,
            Int32 priority, Boolean isExpectMore, Int32 validityPeriod ) {
        SmsMessage.SubmitPdu pdu = SmsMessage->GetSubmitPdu(
                scAddr, destAddr, text, (deliveryIntent != NULL), validityPeriod);
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
            SendRawPdu(tracker);
        } else {
            Rlog->E(TAG, "GsmSMSDispatcher->SendText(): GetSubmitPdu() returned NULL");
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
        SmsMessage.SubmitPdu pdu = SmsMessage->GetSubmitPdu(scAddress, destinationAddress,
                message, deliveryIntent != NULL, SmsHeader->ToByteArray(smsHeader),
                encoding, smsHeader.languageTable, smsHeader.languageShiftTable, validityPeriod);
        If (pdu != NULL) {
            HashMap map =  GetSmsTrackerMap(destinationAddress, scAddress,
                    message, pdu);
            SmsTracker tracker = GetSmsTracker(map, sentIntent,
                    deliveryIntent, GetFormat(), unsentPartCount, anyPartFailed, messageUri,
                    smsHeader, (!lastPart || isExpectMore), validityPeriod);
            SendRawPdu(tracker);
        } else {
            Rlog->E(TAG, "GsmSMSDispatcher->SendNewSubmitPdu(): GetSubmitPdu() returned NULL");
        }
    }

    /** {@inheritDoc} */
    //@Override
    protected void SendSms(SmsTracker tracker) {
        HashMap<String, Object> map = tracker.mData;

        Byte pdu[] = (Byte[]) map->Get("pdu");

        If (tracker.mRetryCount > 0) {
            Rlog->D(TAG, "sendSms: "
                    + " mRetryCount=" + tracker.mRetryCount
                    + " mMessageRef=" + tracker.mMessageRef
                    + " SS=" + mPhone->GetServiceState()->GetState());

            // per TS 23.040 Section 9.2.3.6:  If TP-MTI SMS-SUBMIT (0x01) type
            //   TP-RD (bit 2) is 1 for retry
            //   and TP-MR is set to previously failed sms TP-MR
            If (((0x01 & pdu[0]) == 0x01)) {
                pdu[0] |= 0x04; // TP-RD
                pdu[1] = (Byte) tracker.mMessageRef; // TP-MR
            }
        }
        Rlog->D(TAG, "sendSms: "
                + " IsIms()=" + IsIms()
                + " mRetryCount=" + tracker.mRetryCount
                + " mImsRetry=" + tracker.mImsRetry
                + " mMessageRef=" + tracker.mMessageRef
                + " SS=" + mPhone->GetServiceState()->GetState());

        // Send SMS via the carrier app.
        BroadcastReceiver resultReceiver = new SMSDispatcherReceiver(tracker);

        Intent intent = new Intent(Intents.SMS_SEND_ACTION);
        String carrierPackage = GetCarrierAppPackageName(intent);
        If (carrierPackage != NULL) {
            intent->SetPackage(carrierPackage);
            intent->PutExtra("pdu", pdu);
            intent->PutExtra("smsc", (Byte[]) map->Get("smsc"));
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
    protected void SendSmsByPstn(SmsTracker tracker) {
        Int32 ss = mPhone->GetServiceState()->GetState();
        // if sms over IMS is not supported on data and voice is not available...
        If (!IsIms() && ss != ServiceState.STATE_IN_SERVICE) {
            tracker->OnFailed(mContext, GetNotInServiceError(ss), 0/*errorCode*/);
            return;
        }

        HashMap<String, Object> map = tracker.mData;

        Byte smsc[] = (Byte[]) map->Get("smsc");
        Byte[] pdu = (Byte[]) map->Get("pdu");
        Message reply = ObtainMessage(EVENT_SEND_SMS_COMPLETE, tracker);

        // sms over gsm is used:
        //   if sms over IMS is not supported AND
        //   this is not a retry case after sms over IMS failed
        //     indicated by mImsRetry > 0
        If (0 == tracker.mImsRetry && !IsIms()) {
            If (tracker.mRetryCount > 0) {
                // per TS 23.040 Section 9.2.3.6:  If TP-MTI SMS-SUBMIT (0x01) type
                //   TP-RD (bit 2) is 1 for retry
                //   and TP-MR is set to previously failed sms TP-MR
                If (((0x01 & pdu[0]) == 0x01)) {
                    pdu[0] |= 0x04; // TP-RD
                    pdu[1] = (Byte) tracker.mMessageRef; // TP-MR
                }
            }
            If (tracker.mRetryCount == 0 && tracker.mExpectMore) {
                mCi->SendSMSExpectMore(IccUtils->BytesToHexString(smsc),
                        IccUtils->BytesToHexString(pdu), reply);
            } else {
                mCi->SendSMS(IccUtils->BytesToHexString(smsc),
                        IccUtils->BytesToHexString(pdu), reply);
            }
        } else {
            mCi->SendImsGsmSms(IccUtils->BytesToHexString(smsc),
                    IccUtils->BytesToHexString(pdu), tracker.mImsRetry,
                    tracker.mMessageRef, reply);
            // increment it here, so in case of SMS_FAIL_RETRY over IMS
            // next retry will be sent using IMS request again.
            tracker.mImsRetry++;
        }
    }

    /** {@inheritDoc} */
    //@Override
    protected void UpdateSmsSendStatus(Int32 messageRef, Boolean success) {
        // This function should be defined in ImsDispatcher.
        Rlog->E(TAG, "updateSmsSendStatus should never be called from here!");
    }

    protected UiccCardApplication GetUiccCardApplication() {
            Rlog->D(TAG, "GsmSMSDispatcher: subId = " + mPhone->GetSubId()
                    + " slotId = " + mPhone->GetPhoneId());
                return mUiccController->GetUiccCardApplication(mPhone->GetPhoneId(),
                        UiccController.APP_FAM_3GPP);
    }

    private void OnUpdateIccAvailability() {
        If (mUiccController == NULL ) {
            return;
        }

        UiccCardApplication newUiccApplication = GetUiccCardApplication();

        UiccCardApplication app = mUiccApplication->Get();
        If (app != newUiccApplication) {
            If (app != NULL) {
                Rlog->D(TAG, "Removing stale icc objects.");
                If (mIccRecords->Get() != NULL) {
                    mIccRecords->Get()->UnregisterForNewSms(this);
                }
                mIccRecords->Set(NULL);
                mUiccApplication->Set(NULL);
            }
            If (newUiccApplication != NULL) {
                Rlog->D(TAG, "New Uicc application found");
                mUiccApplication->Set(newUiccApplication);
                mIccRecords->Set(newUiccApplication->GetIccRecords());
                If (mIccRecords->Get() != NULL) {
                    mIccRecords->Get()->RegisterForNewSms(this, EVENT_NEW_ICC_SMS, NULL);
                }
            }
        }
    }
}
