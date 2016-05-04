/*
 * Copyright (C) 2011 The Android Open Source Project
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

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ISubscriptionManager;

/**
 * Monitors the device and ICC storage, and sends the appropriate events.
 *
 * This code was formerly part of {@link SMSDispatcher}, and has been moved
 * into a separate class to support instantiation of multiple SMSDispatchers on
 * dual-mode devices that require support for both 3GPP and 3GPP2 format messages.
 */
public class SmsStorageMonitor extends Handler {
    private static const String TAG = "SmsStorageMonitor";

    /** SIM/RUIM storage is full */
    private static const Int32 EVENT_ICC_FULL = 1;

    /** Memory status reporting is acknowledged by RIL */
    private static const Int32 EVENT_REPORT_MEMORY_STATUS_DONE = 2;

    /** Radio is ON */
    private static const Int32 EVENT_RADIO_ON = 3;

    /** Context from phone object passed to constructor. */
    private final Context mContext;

    /** Wake lock to ensure device stays awake while dispatching the SMS intent. */
    private PowerManager.WakeLock mWakeLock;

    private Boolean mReportMemoryStatusPending;

    /** it is use to put in to extra value for SIM_FULL_ACTION and SMS_REJECTED_ACTION */
    PhoneBase mPhone;

    final CommandsInterface mCi;                            // accessed from inner class
    Boolean mStorageAvailable = TRUE;                       // accessed from inner class

    /**
     * Hold the wake lock for 5 seconds, which should be enough time for
     * any Receiver(s) to grab its own wake lock.
     */
    private static const Int32 WAKE_LOCK_TIMEOUT = 5000;

    /**
     * Creates an SmsStorageMonitor and registers for events.
     * @param phone the Phone to use
     */
    public SmsStorageMonitor(PhoneBase phone) {
        mPhone = phone;
        mContext = phone->GetContext();
        mCi = phone.mCi;

        CreateWakelock();

        mCi->SetOnIccSmsFull(this, EVENT_ICC_FULL, NULL);
        mCi->RegisterForOn(this, EVENT_RADIO_ON, NULL);

        // Register for device storage intents.  Use these to notify the RIL
        // that storage for SMS is or is not available.
        IntentFilter filter = new IntentFilter();
        filter->AddAction(IIntent::ACTION_DEVICE_STORAGE_FULL);
        filter->AddAction(IIntent::ACTION_DEVICE_STORAGE_NOT_FULL);
        mContext->RegisterReceiver(mResultReceiver, filter);
    }

    CARAPI Dispose() {
        mCi->UnSetOnIccSmsFull(this);
        mCi->UnregisterForOn(this);
        mContext->UnregisterReceiver(mResultReceiver);
    }

    /**
     * Handles events coming from the phone stack. Overridden from handler.
     * @param msg the message to handle
     */
    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;

        Switch (msg.what) {
            case EVENT_ICC_FULL:
                HandleIccFull();
                break;

            case EVENT_REPORT_MEMORY_STATUS_DONE:
                ar = (AsyncResult) msg.obj;
                If (ar.exception != NULL) {
                    mReportMemoryStatusPending = TRUE;
                    Rlog->V(TAG, "Memory status report to modem pending : mStorageAvailable = "
                            + mStorageAvailable);
                } else {
                    mReportMemoryStatusPending = FALSE;
                }
                break;

            case EVENT_RADIO_ON:
                If (mReportMemoryStatusPending) {
                    Rlog->V(TAG, "Sending pending memory status report : mStorageAvailable = "
                            + mStorageAvailable);
                    mCi->ReportSmsMemoryStatus(mStorageAvailable,
                            ObtainMessage(EVENT_REPORT_MEMORY_STATUS_DONE));
                }
                break;
        }
    }

    private void CreateWakelock() {
        PowerManager pm = (PowerManager)mContext->GetSystemService(Context.POWER_SERVICE);
        mWakeLock = pm->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "SmsStorageMonitor");
        mWakeLock->SetReferenceCounted(TRUE);
    }

    /**
     * Called when SIM_FULL message is received from the RIL.  Notifies interested
     * parties that SIM storage for SMS messages is full.
     */
    private void HandleIccFull() {
        // broadcast SIM_FULL intent
        Intent intent = new Intent(Intents.SIM_FULL_ACTION);
        mWakeLock->Acquire(WAKE_LOCK_TIMEOUT);
        SubscriptionManager->PutPhoneIdAndSubIdExtra(intent, mPhone->GetPhoneId());
        mContext->SendBroadcast(intent, Manifest::permission::RECEIVE_SMS);
    }

    /** Returns whether or not there is storage available for an incoming SMS. */
    public Boolean IsStorageAvailable() {
        return mStorageAvailable;
    }

    private final BroadcastReceiver mResultReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            If (intent->GetAction()->Equals(IIntent::ACTION_DEVICE_STORAGE_FULL)) {
                mStorageAvailable = FALSE;
                mCi->ReportSmsMemoryStatus(FALSE, ObtainMessage(EVENT_REPORT_MEMORY_STATUS_DONE));
            } else If (intent->GetAction()->Equals(IIntent::ACTION_DEVICE_STORAGE_NOT_FULL)) {
                mStorageAvailable = TRUE;
                mCi->ReportSmsMemoryStatus(TRUE, ObtainMessage(EVENT_REPORT_MEMORY_STATUS_DONE));
            }
        }
    };
}
