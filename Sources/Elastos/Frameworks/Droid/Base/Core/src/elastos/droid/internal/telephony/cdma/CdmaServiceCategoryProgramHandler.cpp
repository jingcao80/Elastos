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

package com.android.internal.telephony.cdma;

using Elastos::Droid::IManifest;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::Cdma::ICdmaSmsCbProgramData;
using Elastos::Droid::Telephony::Cdma::ICdmaSmsCbProgramResults;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IWakeLockStateMachine;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::IBearerData;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::ICdmaSmsAddress;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::ISmsEnvelope;

using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::Utility::IArrayList;

/**
 * Handle CDMA Service Category Program Data requests and responses.
 */
public class CdmaServiceCategoryProgramHandler extends WakeLockStateMachine {

    final CommandsInterface mCi;

    /**
     * Create a new CDMA inbound SMS handler.
     */
    CdmaServiceCategoryProgramHandler(Context context, CommandsInterface commandsInterface) {
        Super("CdmaServiceCategoryProgramHandler", context, NULL);
        mContext = context;
        mCi = commandsInterface;
    }

    /**
     * Create a new State machine for SCPD requests.
     * @param context the context to use
     * @param commandsInterface the radio commands interface
     * @return the new SCPD handler
     */
    static CdmaServiceCategoryProgramHandler MakeScpHandler(Context context,
            CommandsInterface commandsInterface) {
        CdmaServiceCategoryProgramHandler handler = new CdmaServiceCategoryProgramHandler(
                context, commandsInterface);
        handler->Start();
        return handler;
    }

    /**
     * Handle Cell Broadcast messages from {@code CdmaInboundSmsHandler}.
     * 3GPP-format Cell Broadcast messages sent from radio are handled in the subclass.
     *
     * @param message the message to process
     * @return TRUE if an ordered broadcast was sent; FALSE on failure
     */
    //@Override
    protected Boolean HandleSmsMessage(Message message) {
        If (message.obj instanceof SmsMessage) {
            return HandleServiceCategoryProgramData((SmsMessage) message.obj);
        } else {
            Loge("handleMessage got object of type: " + message.obj->GetClass()->GetName());
            return FALSE;
        }
    }


    /**
     * Send SCPD request to CellBroadcastReceiver as an ordered broadcast.
     * @param sms the CDMA SmsMessage containing the SCPD request
     * @return TRUE if an ordered broadcast was sent; FALSE on failure
     */
    private Boolean HandleServiceCategoryProgramData(SmsMessage sms) {
        ArrayList<CdmaSmsCbProgramData> programDataList = sms->GetSmsCbProgramData();
        If (programDataList == NULL) {
            Loge("handleServiceCategoryProgramData: program data list is NULL!");
            return FALSE;
        }

        Intent intent = new Intent(Intents.SMS_SERVICE_CATEGORY_PROGRAM_DATA_RECEIVED_ACTION);
        intent->PutExtra("sender", sms->GetOriginatingAddress());
        intent->PutParcelableArrayListExtra("program_data", programDataList);
        SubscriptionManager->PutPhoneIdAndSubIdExtra(intent, mPhone->GetPhoneId());
        mContext->SendOrderedBroadcast(intent, Manifest::permission::RECEIVE_SMS,
                AppOpsManager.OP_RECEIVE_SMS, mScpResultsReceiver,
                GetHandler(), Activity.RESULT_OK, NULL, NULL);
        return TRUE;
    }

    /**
     * Broadcast receiver to handle results of ordered broadcast. Sends the SCPD results
     * as a reply SMS, then sends a message to state machine to transition to idle.
     */
    private final BroadcastReceiver mScpResultsReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            SendScpResults();
            If (DBG) Log("mScpResultsReceiver finished");
            SendMessage(EVENT_BROADCAST_COMPLETE);
        }

        private void SendScpResults() {
            Int32 resultCode = GetResultCode();
            If ((resultCode != Activity.RESULT_OK) && (resultCode != Intents.RESULT_SMS_HANDLED)) {
                Loge("SCP results error: result code = " + resultCode);
                return;
            }
            Bundle extras = GetResultExtras(FALSE);
            If (extras == NULL) {
                Loge("SCP results error: missing extras");
                return;
            }
            String sender = extras->GetString("sender");
            If (sender == NULL) {
                Loge("SCP results error: missing sender extra.");
                return;
            }
            ArrayList<CdmaSmsCbProgramResults> results
                    = extras->GetParcelableArrayList("results");
            If (results == NULL) {
                Loge("SCP results error: missing results extra.");
                return;
            }

            BearerData bData = new BearerData();
            bData.messageType = BearerData.MESSAGE_TYPE_SUBMIT;
            bData.messageId = SmsMessage->GetNextMessageId();
            bData.serviceCategoryProgramResults = results;
            Byte[] encodedBearerData = BearerData->Encode(bData);

            ByteArrayOutputStream baos = new ByteArrayOutputStream(100);
            DataOutputStream dos = new DataOutputStream(baos);
            try {
                dos->WriteInt(SmsEnvelope.TELESERVICE_SCPT);
                dos->WriteInt(0); //servicePresent
                dos->WriteInt(0); //serviceCategory
                CdmaSmsAddress destAddr = CdmaSmsAddress->Parse(
                        PhoneNumberUtils->CdmaCheckAndProcessPlusCodeForSms(sender));
                dos->Write(destAddr.digitMode);
                dos->Write(destAddr.numberMode);
                dos->Write(destAddr.ton); // number_type
                dos->Write(destAddr.numberPlan);
                dos->Write(destAddr.numberOfDigits);
                dos->Write(destAddr.origBytes, 0, destAddr.origBytes.length); // digits
                // Subaddress is not supported.
                dos->Write(0); //subaddressType
                dos->Write(0); //subaddr_odd
                dos->Write(0); //subaddr_nbr_of_digits
                dos->Write(encodedBearerData.length);
                dos->Write(encodedBearerData, 0, encodedBearerData.length);
                // Ignore the RIL response. TODO: implement retry if SMS send fails.
                mCi->SendCdmaSms(baos->ToByteArray(), NULL);
            } Catch (IOException e) {
                Loge("exception creating SCP results PDU", e);
            } finally {
                try {
                    dos->Close();
                } Catch (IOException ignored) {
                }
            }
        }
    };
}
