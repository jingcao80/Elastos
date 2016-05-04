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

package com.android.internal.telephony;

using Elastos::Droid::IManifest;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ISmsCbMessage;

/**
 * Dispatch new Cell Broadcasts to receivers. Acquires a private wakelock until the broadcast
 * completes and our result receiver is called.
 */
public class CellBroadcastHandler extends WakeLockStateMachine {

    private CellBroadcastHandler(Context context, PhoneBase phone) {
        This("CellBroadcastHandler", context, phone);
    }

    protected CellBroadcastHandler(String debugTag, Context context, PhoneBase phone) {
        Super(debugTag, context, phone);
    }

    /**
     * Create a new CellBroadcastHandler.
     * @param context the context to use for dispatching Intents
     * @return the new handler
     */
    public static CellBroadcastHandler MakeCellBroadcastHandler(Context context, PhoneBase phone) {
        CellBroadcastHandler handler = new CellBroadcastHandler(context, phone);
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
        If (message.obj instanceof SmsCbMessage) {
            HandleBroadcastSms((SmsCbMessage) message.obj);
            return TRUE;
        } else {
            Loge("handleMessage got object of type: " + message.obj->GetClass()->GetName());
            return FALSE;
        }
    }

    /**
     * Dispatch a Cell Broadcast message to listeners.
     * @param message the Cell Broadcast to broadcast
     */
    protected void HandleBroadcastSms(SmsCbMessage message) {
        String receiverPermission;
        Int32 appOp;
        Intent intent;
        If (message->IsEmergencyMessage()) {
            Log("Dispatching emergency SMS CB");
            intent = new Intent(Telephony.Sms.Intents.SMS_EMERGENCY_CB_RECEIVED_ACTION);
            receiverPermission = Manifest::permission::RECEIVE_EMERGENCY_BROADCAST;
            appOp = AppOpsManager.OP_RECEIVE_EMERGECY_SMS;
        } else {
            Log("Dispatching SMS CB");
            intent = new Intent(Telephony.Sms.Intents.SMS_CB_RECEIVED_ACTION);
            receiverPermission = Manifest::permission::RECEIVE_SMS;
            appOp = AppOpsManager.OP_RECEIVE_SMS;
        }
        intent->PutExtra("message", message);
        SubscriptionManager->PutPhoneIdAndSubIdExtra(intent, mPhone->GetPhoneId());
        mContext->SendOrderedBroadcast(intent, receiverPermission, appOp, mReceiver,
                GetHandler(), Activity.RESULT_OK, NULL, NULL);
    }
}
