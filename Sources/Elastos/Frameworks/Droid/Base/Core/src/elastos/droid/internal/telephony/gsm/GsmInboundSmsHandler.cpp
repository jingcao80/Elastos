/*
 * Copyright (C) 2013 The Android Open Source Project
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
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Provider::Telephony::Sms::IIntents;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IInboundSmsHandler;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Telephony::ISmsMessageBase;
using Elastos::Droid::Internal::Telephony::ISmsStorageMonitor;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IUsimServiceTable;

/**
 * This class broadcasts incoming SMS messages to interested apps after storing them in
 * the SmsProvider "raw" table and ACKing them to the SMSC. After each message has been
 */
public class GsmInboundSmsHandler extends InboundSmsHandler {

    /** Handler for SMS-PP data download messages to UICC. */
    private final UsimDataDownloadHandler mDataDownloadHandler;

    /**
     * Create a new GSM inbound SMS handler.
     */
    private GsmInboundSmsHandler(Context context, SmsStorageMonitor storageMonitor,
            PhoneBase phone) {
        Super("GsmInboundSmsHandler", context, storageMonitor, phone,
                GsmCellBroadcastHandler->MakeGsmCellBroadcastHandler(context, phone));
        phone.mCi->SetOnNewGsmSms(GetHandler(), EVENT_NEW_SMS, NULL);
        mDataDownloadHandler = new UsimDataDownloadHandler(phone.mCi);
    }

    /**
     * Unregister for GSM SMS.
     */
    //@Override
    protected void OnQuitting() {
        mPhone.mCi->UnSetOnNewGsmSms(GetHandler());
        mCellBroadcastHandler->Dispose();

        If (DBG) Log("unregistered for 3GPP SMS");
        super->OnQuitting();     // release wakelock
    }

    /**
     * Wait for state machine to enter startup state. We can't send any messages until then.
     */
    public static GsmInboundSmsHandler MakeInboundSmsHandler(Context context,
            SmsStorageMonitor storageMonitor, PhoneBase phone) {
        GsmInboundSmsHandler handler = new GsmInboundSmsHandler(context, storageMonitor, phone);
        handler->Start();
        return handler;
    }

    /**
     * Return TRUE if this handler is for 3GPP2 messages; FALSE for 3GPP format.
     * @return FALSE (3GPP)
     */
    //@Override
    protected Boolean Is3gpp2() {
        return FALSE;
    }

    /**
     * Handle type zero, SMS-PP data download, and 3GPP/CPHS MWI type SMS. Normal SMS messages
     * are handled by {@link #dispatchNormalMessage} in parent class.
     *
     * @param smsb the SmsMessageBase object from the RIL
     * @return a result code from {@link android.provider.Telephony.Sms.Intents},
     *  or {@link Activity#RESULT_OK} for delayed acknowledgment to SMSC
     */
    //@Override
    protected Int32 DispatchMessageRadioSpecific(SmsMessageBase smsb) {
        SmsMessage sms = (SmsMessage) smsb;

        If (sms->IsTypeZero()) {
            // As per 3GPP TS 23.040 9.2.3.9, Type Zero messages should not be
            // Displayed/Stored/Notified. They should only be acknowledged.
            Log("Received short message type 0, Don't display or store it. Send Ack");
            return Intents.RESULT_SMS_HANDLED;
        }

        // Send SMS-PP data download messages to UICC. See 3GPP TS 31.111 section 7.1.1.
        If (sms->IsUsimDataDownload()) {
            UsimServiceTable ust = mPhone->GetUsimServiceTable();
            return mDataDownloadHandler->HandleUsimDataDownload(ust, sms);
        }

        Boolean handled = FALSE;
        If (sms->IsMWISetMessage()) {
            UpdateMessageWaitingIndicator(sms->GetNumOfVoicemails());
            handled = sms->IsMwiDontStore();
            If (DBG) Log("Received voice mail indicator set SMS shouldStore=" + !handled);
        } else If (sms->IsMWIClearMessage()) {
            UpdateMessageWaitingIndicator(0);
            handled = sms->IsMwiDontStore();
            If (DBG) Log("Received voice mail indicator clear SMS shouldStore=" + !handled);
        }
        If (handled) {
            return Intents.RESULT_SMS_HANDLED;
        }

        If (!mStorageMonitor->IsStorageAvailable() &&
                sms->GetMessageClass() != SmsConstants.MessageClass.CLASS_0) {
            // It's a storable message and there's no storage available.  Bail.
            // (See TS 23.038 for a description of class 0 messages.)
            return Intents.RESULT_SMS_OUT_OF_MEMORY;
        }

        return DispatchNormalMessage(smsb);
    }

    /* package */ void UpdateMessageWaitingIndicator(Int32 voicemailCount) {
        // range check
        If (voicemailCount < 0) {
            voicemailCount = -1;
        } else If (voicemailCount > 0xff) {
            // TS 23.040 9.2.3.24.2
            // "The value 255 shall be taken to mean 255 or greater"
            voicemailCount = 0xff;
        }
        // update voice mail count in GsmPhone
        mPhone->SetVoiceMessageCount(voicemailCount);
        // store voice mail count in SIM & shared preferences
        IccRecords records = UiccController->GetInstance()->GetIccRecords(
                mPhone->GetPhoneId(), UiccController.APP_FAM_3GPP);
        If (records != NULL) {
            Log("updateMessageWaitingIndicator: updating SIM Records");
            records->SetVoiceMessageWaiting(1, voicemailCount);
        } else {
            Log("updateMessageWaitingIndicator: SIM Records not found");
        }
        StoreVoiceMailCount();
    }

    /**
     * Send an acknowledge message.
     * @param success indicates that last message was successfully received.
     * @param result result code indicating any error
     * @param response callback message sent when operation completes.
     */
    //@Override
    protected void AcknowledgeLastIncomingSms(Boolean success, Int32 result, Message response) {
        mPhone.mCi->AcknowledgeLastIncomingGsmSms(success, ResultToCause(result), response);
    }

    /**
     * Called when the phone changes the default method updates mPhone
     * mStorageMonitor and mCellBroadcastHandler.updatePhoneObject.
     * Override if different or other behavior is desired.
     *
     * @param phone
     */
    //@Override
    protected void OnUpdatePhoneObject(PhoneBase phone) {
        super->OnUpdatePhoneObject(phone);
        Log("onUpdatePhoneObject: dispose of old CellBroadcastHandler and make a new one");
        mCellBroadcastHandler->Dispose();
        mCellBroadcastHandler = GsmCellBroadcastHandler
                .MakeGsmCellBroadcastHandler(mContext, phone);
    }

    /**
     * Convert Android result code to 3GPP SMS failure cause.
     * @param rc the Android SMS intent result value
     * @return 0 for success, or a 3GPP SMS failure cause value
     */
    private static Int32 ResultToCause(Int32 rc) {
        Switch (rc) {
            case Activity.RESULT_OK:
            case Intents.RESULT_SMS_HANDLED:
                // Cause code is ignored on success.
                return 0;
            case Intents.RESULT_SMS_OUT_OF_MEMORY:
                return CommandsInterface.GSM_SMS_FAIL_CAUSE_MEMORY_CAPACITY_EXCEEDED;
            case Intents.RESULT_SMS_GENERIC_ERROR:
            default:
                return CommandsInterface.GSM_SMS_FAIL_CAUSE_UNSPECIFIED_ERROR;
        }
    }
}
