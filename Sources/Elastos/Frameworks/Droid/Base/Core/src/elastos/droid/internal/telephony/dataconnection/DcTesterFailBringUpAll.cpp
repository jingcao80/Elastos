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

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::IPhoneBase;

/**
 * A package level call that causes all DataConnection bringUp calls to fail a specific
 * number of times. Here is an example that sets counter to 2 and cause to -3 for all instances:
 *    adb shell am broadcast -a com.android.internal.telephony.dataconnection.action_fail_bringup \
 *     --ei counter 2 --ei fail_cause -3
 *
 * Also you can add a suggested retry time if desired:
 *     --ei suggested_retry_time 5000
 *
 * The fail_cause is one of {@link DcFailCause}
 */
public class DcTesterFailBringUpAll {
    private static const String LOG_TAG = "DcTesterFailBrinupAll";
    private static const Boolean DBG = TRUE;

    private PhoneBase mPhone;

    private String mActionFailBringUp = DcFailBringUp.INTENT_BASE + "."
            + DcFailBringUp.ACTION_FAIL_BRINGUP;

    // The saved FailBringUp data from the intent
    private DcFailBringUp mFailBringUp = new DcFailBringUp();

    // The static intent receiver one for all instances.
    private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
            //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            If (DBG) Log("sIntentReceiver.onReceive: action=" + action);
            If (action->Equals(mActionFailBringUp)) {
                mFailBringUp->SaveParameters(intent, "sFailBringUp");
            } else If (action->Equals(mPhone->GetActionDetached())) {
                // Counter is MAX, bringUp/retry will always fail
                Log("simulate detaching");
                mFailBringUp->SaveParameters(Integer.MAX_VALUE,
                        DcFailCause.LOST_CONNECTION->GetErrorCode(),
                        DcFailBringUp.DEFAULT_SUGGESTED_RETRY_TIME);
            } else If (action->Equals(mPhone->GetActionAttached())) {
                // Counter is 0 next bringUp/retry will succeed
                Log("simulate attaching");
                mFailBringUp->SaveParameters(0, DcFailCause.NONE->GetErrorCode(),
                        DcFailBringUp.DEFAULT_SUGGESTED_RETRY_TIME);
            } else {
                If (DBG) Log("onReceive: unknown action=" + action);
            }
        }
    };

    DcTesterFailBringUpAll(PhoneBase phone, Handler handler) {
        mPhone = phone;
        If (Build.IS_DEBUGGABLE) {
            IntentFilter filter = new IntentFilter();

            filter->AddAction(mActionFailBringUp);
            Log("register for intent action=" + mActionFailBringUp);

            filter->AddAction(mPhone->GetActionDetached());
            Log("register for intent action=" + mPhone->GetActionDetached());

            filter->AddAction(mPhone->GetActionAttached());
            Log("register for intent action=" + mPhone->GetActionAttached());

            phone->GetContext()->RegisterReceiver(mIntentReceiver, filter, NULL, handler);
        }
    }

    void Dispose() {
        If (Build.IS_DEBUGGABLE) {
            mPhone->GetContext()->UnregisterReceiver(mIntentReceiver);
        }
    }

    DcFailBringUp GetDcFailBringUp() {
        return mFailBringUp;
    }

    private void Log(String s) {
        Rlog->D(LOG_TAG, s);
    }
}
