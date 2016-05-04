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
package com.android.internal.telephony.dataconnection;

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IRILConstants;

/**
 * The Data Connection Retry Alarm Controller.
 */
public class DcRetryAlarmController {
    private String mLogTag = "DcRac";
    private static const Boolean DBG = TRUE;

    private PhoneBase mPhone;
    private DataConnection mDc;
    private AlarmManager mAlarmManager;

    // The Intent action for retrying and its two extra's
    private String mActionRetry;
    private static const String INTENT_RETRY_ALARM_WHAT = "what";
    private static const String INTENT_RETRY_ALARM_TAG = "tag";

    private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
            //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            If (TextUtils->IsEmpty(action)) {
                // Our mActionXxxx's could be NULL when disposed this could match an empty action.
                Log("onReceive: ignore empty action='" + action + "'");
                return;
            }
            If (TextUtils->Equals(action, mActionRetry)) {
                If (!intent->HasExtra(INTENT_RETRY_ALARM_WHAT)) {
                    throw new RuntimeException(mActionRetry + " has no INTENT_RETRY_ALRAM_WHAT");
                }
                If (!intent->HasExtra(INTENT_RETRY_ALARM_TAG)) {
                    throw new RuntimeException(mActionRetry + " has no INTENT_RETRY_ALRAM_TAG");
                }
                Int32 what = intent->GetIntExtra(INTENT_RETRY_ALARM_WHAT, Integer.MAX_VALUE);
                Int32 tag = intent->GetIntExtra(INTENT_RETRY_ALARM_TAG, Integer.MAX_VALUE);
                If (DBG) {
                    Log("onReceive: action=" + action
                            + " SendMessage(what:" + mDc->GetWhatToString(what)
                            + ", tag:" + tag + ")");
                }
                mDc->SendMessage(mDc->ObtainMessage(what, tag, 0));
            } else {
                If (DBG) Log("onReceive: unknown action=" + action);
            }
        }
    };

    DcRetryAlarmController(PhoneBase phone, DataConnection dc) {
        mLogTag = dc->GetName();
        mPhone = phone;
        mDc = dc;
        mAlarmManager = (AlarmManager) mPhone->GetContext()->GetSystemService(Context.ALARM_SERVICE);
        mActionRetry = mDc->GetClass()->GetCanonicalName() + "." + mDc->GetName() + ".action_retry";

        IntentFilter filter = new IntentFilter();
        filter->AddAction(mActionRetry);
        Log("DcRetryAlarmController: register for intent action=" + mActionRetry);

        mPhone->GetContext()->RegisterReceiver(mIntentReceiver, filter, NULL, mDc->GetHandler());
    }

    /**
     * Dispose of resources when shutting down
     */
    void Dispose() {
        If (DBG) Log("dispose");
        mPhone->GetContext()->UnregisterReceiver(mIntentReceiver);
        mPhone = NULL;
        mDc = NULL;
        mAlarmManager = NULL;
        mActionRetry = NULL;
    }

    /**
     * Using dc.mRetryManager and the result of the SETUP_DATA_CALL determine
     * the retry delay.
     *
     * @param dc is the DataConnection
     * @param ar is the result from SETUP_DATA_CALL
     * @return < 0 if no retry is needed otherwise the delay to the next SETUP_DATA_CALL
     */
    Int32 GetSuggestedRetryTime(DataConnection dc, AsyncResult ar) {
        Int32 retryDelay;

        DataCallResponse response = (DataCallResponse) ar.result;
        retryDelay = response.suggestedRetryTime;
        If (retryDelay == RILConstants.MAX_INT) {
            If (DBG) Log("getSuggestedRetryTime: suggestedRetryTime is MAX_INT, retry NOT needed");
            retryDelay = -1;
        } else If (retryDelay >= 0) {
            If (DBG) Log("getSuggestedRetryTime: suggestedRetryTime is >= 0 use it");
        } else If (dc.mRetryManager->IsRetryNeeded()) {
            retryDelay = dc.mRetryManager->GetRetryTimer();
            If (retryDelay < 0) {
                retryDelay = 0;
            }
            If (DBG) Log("getSuggestedRetryTime: retry is needed");
        } else {
            If (DBG) Log("getSuggestedRetryTime: retry is NOT needed");
            retryDelay = -1;
        }

        If (DBG) {
            Log("getSuggestedRetryTime: " + retryDelay + " response=" + response + " dc=" + dc);
        }
        return retryDelay;
    }

    CARAPI StartRetryAlarm(Int32 what, Int32 tag, Int32 delay) {
        Intent intent = new Intent(mActionRetry);
        intent->PutExtra(INTENT_RETRY_ALARM_WHAT, what);
        intent->PutExtra(INTENT_RETRY_ALARM_TAG, tag);

        If (DBG) {
            Log("startRetryAlarm: next attempt in " + (delay / 1000) + "s" +
                    " what=" + what + " tag=" + tag);
        }

        PendingIntent retryIntent = PendingIntent.GetBroadcast (mPhone->GetContext(), 0,
                                        intent, PendingIIntent::FLAG_UPDATE_CURRENT);
        mAlarmManager->Set(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                SystemClock->ElapsedRealtime() + delay, retryIntent);
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        StringBuilder sb = new StringBuilder();
        sb->Append(mLogTag).Append(" [dcRac] ");
        sb->Append(" mPhone=").Append(mPhone);
        sb->Append(" mDc=").Append(mDc);
        sb->Append(" mAlaramManager=").Append(mAlarmManager);
        sb->Append(" mActionRetry=").Append(mActionRetry);
        return sb->ToString();
    }

    private void Log(String s) {
        Rlog->D(mLogTag, "[dcRac] " + s);
    }
}
