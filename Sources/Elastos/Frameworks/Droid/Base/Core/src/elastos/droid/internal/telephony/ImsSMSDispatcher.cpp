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

package com.android.internal.telephony;

using static android::Telephony::SmsManager::IRESULT_ERROR_GENERIC_FAILURE;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger;

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Internal::IR;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSMSDispatcher;
using Elastos::Droid::Internal::Telephony::Gsm::IGsmSMSDispatcher;
using Elastos::Droid::Internal::Telephony::Gsm::IGsmInboundSmsHandler;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaInboundSmsHandler;

public class ImsSMSDispatcher extends SMSDispatcher {
    private static const String TAG = "RIL_ImsSms";

    private SMSDispatcher mCdmaDispatcher;
    private SMSDispatcher mGsmDispatcher;

    private GsmInboundSmsHandler mGsmInboundSmsHandler;
    private CdmaInboundSmsHandler mCdmaInboundSmsHandler;


    /** TRUE if IMS is registered and sms is supported, FALSE otherwise.*/
    private Boolean mIms = FALSE;
    private String mImsSmsFormat = SmsConstants.FORMAT_UNKNOWN;

    /**
     * TRUE if MO SMS over IMS is enabled. Default value is TRUE. FALSE for
     * carriers with config_send_sms1x_on_voice_call = TRUE when attached to
     * eHRPD and during active 1x voice call
     */
    private Boolean mImsSmsEnabled = TRUE;

    public ImsSMSDispatcher(PhoneBase phone, SmsStorageMonitor storageMonitor,
            SmsUsageMonitor usageMonitor) {
        Super(phone, usageMonitor, NULL);
        Rlog->D(TAG, "ImsSMSDispatcher created");

        // Create dispatchers, inbound SMS handlers and
        // broadcast undelivered messages in raw table.
        mCdmaDispatcher = new CdmaSMSDispatcher(phone, usageMonitor, this);
        mGsmInboundSmsHandler = GsmInboundSmsHandler->MakeInboundSmsHandler(phone->GetContext(),
                storageMonitor, phone);
        mCdmaInboundSmsHandler = CdmaInboundSmsHandler->MakeInboundSmsHandler(phone->GetContext(),
                storageMonitor, phone, (CdmaSMSDispatcher) mCdmaDispatcher);
        mGsmDispatcher = new GsmSMSDispatcher(phone, usageMonitor, this, mGsmInboundSmsHandler);
        Thread broadcastThread = new Thread(new SmsBroadcastUndelivered(phone->GetContext(),
                mGsmInboundSmsHandler, mCdmaInboundSmsHandler));
        broadcastThread->Start();

        mCi->RegisterForOn(this, EVENT_RADIO_ON, NULL);
        mCi->RegisterForImsNetworkStateChanged(this, EVENT_IMS_STATE_CHANGED, NULL);
    }

    /* Updates the phone object when there is a change */
    //@Override
    protected void UpdatePhoneObject(PhoneBase phone) {
        Rlog->D(TAG, "In IMS updatePhoneObject ");
        super->UpdatePhoneObject(phone);
        mCdmaDispatcher->UpdatePhoneObject(phone);
        mGsmDispatcher->UpdatePhoneObject(phone);
        mGsmInboundSmsHandler->UpdatePhoneObject(phone);
        mCdmaInboundSmsHandler->UpdatePhoneObject(phone);
    }

    CARAPI Dispose() {
        mCi->UnregisterForOn(this);
        mCi->UnregisterForImsNetworkStateChanged(this);
        mGsmDispatcher->Dispose();
        mCdmaDispatcher->Dispose();
        mGsmInboundSmsHandler->Dispose();
        mCdmaInboundSmsHandler->Dispose();
    }

    /**
     * Handles events coming from the phone stack. Overridden from handler.
     *
     * @param msg the message to handle
     */
    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;

        Switch (msg.what) {
        case EVENT_RADIO_ON:
        case EVENT_IMS_STATE_CHANGED: // received unsol
            mCi->GetImsRegistrationState(this->ObtainMessage(EVENT_IMS_STATE_DONE));
            break;

        case EVENT_IMS_STATE_DONE:
            ar = (AsyncResult) msg.obj;

            If (ar.exception == NULL) {
                UpdateImsInfo(ar);
            } else {
                Rlog->E(TAG, "IMS State query failed with exp "
                        + ar.exception);
            }
            break;

        default:
            super->HandleMessage(msg);
        }
    }

    private void SetImsSmsFormat(Int32 format) {
        // valid format?
        Switch (format) {
            case PhoneConstants.PHONE_TYPE_GSM:
                mImsSmsFormat = "3gpp";
                break;
            case PhoneConstants.PHONE_TYPE_CDMA:
                mImsSmsFormat = "3gpp2";
                break;
            default:
                mImsSmsFormat = "unknown";
                break;
        }
    }

    private void UpdateImsInfo(AsyncResult ar) {
        Int32[] responseArray = (Int32[])ar.result;

        mIms = FALSE;
        If (responseArray[0] == 1) {  // IMS is registered
            Rlog->D(TAG, "IMS is registered!");
            mIms = TRUE;
        } else {
            Rlog->D(TAG, "IMS is NOT registered!");
        }

        SetImsSmsFormat(responseArray[1]);

        If (("unknown".Equals(mImsSmsFormat))) {
            Rlog->E(TAG, "IMS format was unknown!");
            // failed to retrieve valid IMS SMS format info, set IMS to unregistered
            mIms = FALSE;
        }
    }

    //@Override
    protected void SendData(String destAddr, String scAddr, Int32 destPort, Int32 origPort,
            Byte[] data, PendingIntent sentIntent, PendingIntent deliveryIntent) {
        If (IsCdmaMo()) {
            mCdmaDispatcher->SendData(destAddr, scAddr, destPort, origPort,
                    data, sentIntent, deliveryIntent);
        } else {
            mGsmDispatcher->SendData(destAddr, scAddr, destPort, origPort,
                    data, sentIntent, deliveryIntent);
        }
    }

    //@Override
    protected void SendMultipartText(String destAddr, String scAddr,
            ArrayList<String> parts, ArrayList<PendingIntent> sentIntents,
            ArrayList<PendingIntent> deliveryIntents, Uri messageUri, String callingPkg,
            Int32 priority, Boolean isExpectMore, Int32 validityPeriod) {
        If (IsCdmaMo()) {
            mCdmaDispatcher->SendMultipartText(destAddr, scAddr,
                    parts, sentIntents, deliveryIntents, messageUri, callingPkg,
                    priority, isExpectMore, validityPeriod);
        } else {
            mGsmDispatcher->SendMultipartText(destAddr, scAddr,
                    parts, sentIntents, deliveryIntents, messageUri, callingPkg,
                    priority, isExpectMore, validityPeriod);
        }
    }

    //@Override
    protected void SendSms(SmsTracker tracker) {
        //  sendSms is a helper function to other send functions, sendText/Data...
        //  it is not part of ISms.stub
        Rlog->E(TAG, "sendSms should never be called from here!");
    }

    //@Override
    protected void SendSmsByPstn(SmsTracker tracker) {
        // This function should be defined in Gsm/CdmaDispatcher.
        Rlog->E(TAG, "sendSmsByPstn should never be called from here!");
    }

    //@Override
    protected void UpdateSmsSendStatus(Int32 messageRef, Boolean success) {
        If (IsCdmaMo()) {
            UpdateSmsSendStatusHelper(messageRef, mCdmaDispatcher.sendPendingList,
                                      mCdmaDispatcher, success);
            UpdateSmsSendStatusHelper(messageRef, mGsmDispatcher.sendPendingList,
                                      NULL, success);
        } else {
            UpdateSmsSendStatusHelper(messageRef, mGsmDispatcher.sendPendingList,
                                      mGsmDispatcher, success);
            UpdateSmsSendStatusHelper(messageRef, mCdmaDispatcher.sendPendingList,
                                      NULL, success);
        }
    }

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
    private void UpdateSmsSendStatusHelper(Int32 messageRef,
                                           List<SmsTracker> sendPendingList,
                                           SMSDispatcher smsDispatcher,
                                           Boolean success) {
        Synchronized (sendPendingList) {
            For (Int32 i = 0, count = sendPendingList->Size(); i < count; i++) {
                SmsTracker tracker = sendPendingList->Get(i);
                If (tracker.mMessageRef == messageRef) {
                    // Found it.  Remove from list and broadcast.
                    sendPendingList->Remove(i);
                    If (success) {
                        Rlog->D(TAG, "Sending SMS by IP succeeded.");
                        SendMessage(ObtainMessage(EVENT_SEND_SMS_COMPLETE,
                                                  new AsyncResult(tracker, NULL, NULL)));
                    } else {
                        Rlog->D(TAG, "Sending SMS by IP failed.");
                        If (smsDispatcher != NULL) {
                            smsDispatcher->SendSmsByPstn(tracker);
                        } else {
                            Rlog->E(TAG, "No feasible way to send this SMS.");
                        }
                    }
                    // Only expect to see one tracker matching this messageref.
                    break;
                }
            }
        }
    }

    //@Override
    protected void SendText(String destAddr, String scAddr, String text, PendingIntent sentIntent,
            PendingIntent deliveryIntent, Uri messageUri, String callingPkg,
            Int32 priority, Boolean isExpectMore, Int32 validityPeriod ) {
        Rlog->D(TAG, "sendText");
        If (IsCdmaMo()) {
            mCdmaDispatcher->SendText(destAddr, scAddr,
                    text, sentIntent, deliveryIntent, messageUri, callingPkg,
                    priority, isExpectMore, validityPeriod);
        } else {
            mGsmDispatcher->SendText(destAddr, scAddr,
                    text, sentIntent, deliveryIntent, messageUri, callingPkg,
                    priority, isExpectMore, validityPeriod);
        }
    }

    //@Override
    protected void InjectSmsPdu(Byte[] pdu, String format, PendingIntent receivedIntent) {
        Rlog->D(TAG, "ImsSMSDispatcher:injectSmsPdu");
        try {
            // TODO We need to decide whether we should allow injecting GSM(3gpp)
            // SMS pdus when the phone is camping on CDMA(3gpp2) network and vice versa.
            android.telephony.SmsMessage msg =
                    android.telephony.SmsMessage->CreateFromPdu(pdu, format);

            // Only class 1 SMS are allowed to be injected.
            If (msg->GetMessageClass() != android.telephony.SmsMessage.MessageClass.CLASS_1) {
                If (receivedIntent != NULL)
                    receivedIntent->Send(Intents.RESULT_SMS_GENERIC_ERROR);
                return;
            }

            AsyncResult ar = new AsyncResult(receivedIntent, msg, NULL);

            If (format->Equals(SmsConstants.FORMAT_3GPP)) {
                Rlog->I(TAG, "ImsSMSDispatcher:injectSmsText Sending msg=" + msg +
                        ", format=" + format + "to mGsmInboundSmsHandler");
                mGsmInboundSmsHandler->SendMessage(InboundSmsHandler.EVENT_INJECT_SMS, ar);
            } else If (format->Equals(SmsConstants.FORMAT_3GPP2)) {
                Rlog->I(TAG, "ImsSMSDispatcher:injectSmsText Sending msg=" + msg +
                        ", format=" + format + "to mCdmaInboundSmsHandler");
                mCdmaInboundSmsHandler->SendMessage(InboundSmsHandler.EVENT_INJECT_SMS, ar);
            } else {
                // Invalid pdu format.
                Rlog->E(TAG, "Invalid pdu format: " + format);
                If (receivedIntent != NULL)
                    receivedIntent->Send(Intents.RESULT_SMS_GENERIC_ERROR);
            }
        } Catch (Exception e) {
            Rlog->E(TAG, "injectSmsPdu failed: ", e);
            try {
                If (receivedIntent != NULL)
                    receivedIntent->Send(Intents.RESULT_SMS_GENERIC_ERROR);
            } Catch (CanceledException ex) {}
        }
    }

    //@Override
    CARAPI SendRetrySms(SmsTracker tracker) {
        String oldFormat = tracker.mFormat;

        // newFormat will be based on voice technology
        String newFormat =
            (PhoneConstants.PHONE_TYPE_CDMA == mPhone->GetPhoneType()) ?
                    mCdmaDispatcher->GetFormat() :
                        mGsmDispatcher->GetFormat();

        // was previously sent sms format match with voice tech?
        If (oldFormat->Equals(newFormat)) {
            If (IsCdmaFormat(newFormat)) {
                Rlog->D(TAG, "old format matched new Format (cdma)");
                ShouldSendSmsOverIms();
                mCdmaDispatcher->SendSms(tracker);
                return;
            } else {
                Rlog->D(TAG, "old format matched new Format (gsm)");
                mGsmDispatcher->SendSms(tracker);
                return;
            }
        }

        // format didn't match, need to re-encode.
        HashMap map = tracker.mData;

        // to re-encode, fields needed are:  scAddr, destAddr, and
        //   text if originally sent as sendText or
        //   data and destPort if originally sent as sendData.
        If (!( map->ContainsKey("scAddr") && map->ContainsKey("destAddr") &&
               ( map->ContainsKey("text") ||
                       (map->ContainsKey("data") && map->ContainsKey("destPort"))))) {
            // should never come here...
            Rlog->E(TAG, "sendRetrySms failed to re-encode per missing fields!");
            tracker->OnFailed(mContext, RESULT_ERROR_GENERIC_FAILURE, 0/*errorCode*/);
            return;
        }
        String scAddr = (String)map->Get("scAddr");
        String destAddr = (String)map->Get("destAddr");

        SmsMessageBase.SubmitPduBase pdu = NULL;
        //    figure out from tracker if this was sendText/Data
        If (map->ContainsKey("text")) {
            Rlog->D(TAG, "sms failed was text");
            String text = (String)map->Get("text");

            If (IsCdmaFormat(newFormat)) {
                Rlog->D(TAG, "old Format (gsm) ==> new Format (cdma)");
                pdu = com.android.internal.telephony.cdma.SmsMessage->GetSubmitPdu(
                        scAddr, destAddr, text, (tracker.mDeliveryIntent != NULL), NULL);
                ShouldSendSmsOverIms();
            } else {
                Rlog->D(TAG, "old Format (cdma) ==> new Format (gsm)");
                pdu = com.android.internal.telephony.gsm.SmsMessage->GetSubmitPdu(
                        scAddr, destAddr, text, (tracker.mDeliveryIntent != NULL), NULL);
            }
        } else If (map->ContainsKey("data")) {
            Rlog->D(TAG, "sms failed was data");
            Byte[] data = (Byte[])map->Get("data");
            Integer destPort = (Integer)map->Get("destPort");

            If (IsCdmaFormat(newFormat)) {
                Rlog->D(TAG, "old Format (gsm) ==> new Format (cdma)");
                pdu = com.android.internal.telephony.cdma.SmsMessage->GetSubmitPdu(
                            scAddr, destAddr, destPort->IntValue(), data,
                            (tracker.mDeliveryIntent != NULL));
                ShouldSendSmsOverIms();
            } else {
                Rlog->D(TAG, "old Format (cdma) ==> new Format (gsm)");
                pdu = com.android.internal.telephony.gsm.SmsMessage->GetSubmitPdu(
                            scAddr, destAddr, destPort->IntValue(), data,
                            (tracker.mDeliveryIntent != NULL));
            }
        }

        // replace old smsc and pdu with newly encoded ones
        map->Put("smsc", pdu.encodedScAddress);
        map->Put("pdu", pdu.encodedMessage);

        SMSDispatcher dispatcher = (IsCdmaFormat(newFormat)) ?
                mCdmaDispatcher : mGsmDispatcher;

        tracker.mFormat = dispatcher->GetFormat();
        dispatcher->SendSms(tracker);
    }

    //@Override
    protected String GetFormat() {
        // this function should be defined in Gsm/CdmaDispatcher.
        Rlog->E(TAG, "getFormat should never be called from here!");
        return "unknown";
    }

    //@Override
    protected GsmAlphabet.TextEncodingDetails CalculateLength(
            CharSequence messageBody, Boolean use7bitOnly) {
        Rlog->E(TAG, "Error! Not implemented for IMS.");
        return NULL;
    }

    //@Override
    protected void SendNewSubmitPdu(String destinationAddress, String scAddress, String message,
            SmsHeader smsHeader, Int32 format, PendingIntent sentIntent,
            PendingIntent deliveryIntent, Boolean lastPart, Int32 priority, Boolean isExpectMore,
            Int32 validityPeriod, AtomicInteger unsentPartCount, AtomicBoolean anyPartFailed,
            Uri messageUri) {
        Rlog->E(TAG, "Error! Not implemented for IMS.");
    }

    //@Override
    public Boolean IsIms() {
        return mIms;
    }

    //@Override
    public String GetImsSmsFormat() {
        return mImsSmsFormat;
    }

    /**
     * Determines whether or not to use CDMA format for MO SMS.
     * If SMS over IMS is supported, then format is based on IMS SMS format,
     * otherwise format is based on current phone type.
     *
     * @return TRUE if Cdma format should be used for MO SMS, FALSE otherwise.
     */
    private Boolean IsCdmaMo() {
        If (!IsIms() || !ShouldSendSmsOverIms()) {
            // Either IMS is not registered or there is an active 1x voice call
            // while on eHRPD, use Voice technology to determine SMS format.
            Return (PhoneConstants.PHONE_TYPE_CDMA == mPhone->GetPhoneType());
        }
        // IMS is registered with SMS support
        return IsCdmaFormat(mImsSmsFormat);
    }

    /**
     * Determines whether or not format given is CDMA format.
     *
     * @param format
     * @return TRUE if format given is CDMA format, FALSE otherwise.
     */
    private Boolean IsCdmaFormat(String format) {
        Return (mCdmaDispatcher->GetFormat()->Equals(format));
    }

    /**
     * Enables MO SMS over IMS
     *
     * @param enable
     */
    CARAPI EnableSendSmsOverIms(Boolean enable) {
        mImsSmsEnabled = enable;
    }

    /**
     * Determines whether MO SMS over IMS is currently enabled.
     *
     * @return TRUE if MO SMS over IMS is enabled, FALSE otherwise.
     */
    public Boolean IsImsSmsEnabled() {
        return mImsSmsEnabled;
    }

    /**
     * Determines whether SMS should be sent over IMS if UE is attached to eHRPD
     * and there is an active voice call
     *
     * @return TRUE if SMS should be sent over IMS based on value in config.xml
     *         or system property FALSE otherwise
     */
    public Boolean ShouldSendSmsOverIms() {
        Boolean sendSmsOn1x = mContext->GetResources()->GetBoolean(
                R.bool.config_send_sms1x_on_voice_call);
        Int32 currentCallState = mTelephonyManager->GetCallState();
        Int32 currentVoiceNetwork = mTelephonyManager->GetVoiceNetworkType();
        Int32 currentDataNetwork = mTelephonyManager->GetDataNetworkType();

        Rlog->D(TAG, "data = " + currentDataNetwork + " voice = " + currentVoiceNetwork
                + " call state = " + currentCallState);

        If (sendSmsOn1x) {
            // The UE shall use 1xRTT for SMS if the UE is attached to an eHRPD
            // network and there is an active 1xRTT voice call.
            If (currentDataNetwork == TelephonyManager.NETWORK_TYPE_EHRPD
                    && currentVoiceNetwork == TelephonyManager.NETWORK_TYPE_1xRTT
                    && currentCallState != mTelephonyManager.CALL_STATE_IDLE) {
                EnableSendSmsOverIms(FALSE);
                return FALSE;
            }
        }
        return TRUE;
    }
}
