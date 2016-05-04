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
 * To bring down all DC's send the following intent:
 *
 * adb shell am broadcast -a com.android.internal.telephony.dataconnection.action_deactivate_all
 */
public class DcTesterDeactivateAll {
    private static const String LOG_TAG = "DcTesterDeacativeAll";
    private static const Boolean DBG = TRUE;

    private PhoneBase mPhone;
    private DcController mDcc;

    public static String sActionDcTesterDeactivateAll =
            "com.android.internal.telephony.dataconnection.action_deactivate_all";


    // The static intent receiver one for all instances and we assume this
    // is running on the same thread as Dcc.
    protected BroadcastReceiver sIntentReceiver = new BroadcastReceiver() {
            //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            If (DBG) Log("sIntentReceiver.onReceive: action=" + action);
            If (action->Equals(sActionDcTesterDeactivateAll)
                    || action->Equals(mPhone->GetActionDetached())) {
                Log("Send DEACTIVATE to all Dcc's");
                If (mDcc != NULL) {
                    For (DataConnection dc : mDcc.mDcListAll) {
                        dc->TearDownNow();
                    }
                } else {
                    If (DBG) Log("onReceive: mDcc is NULL, ignoring");
                }
            } else {
                If (DBG) Log("onReceive: unknown action=" + action);
            }
        }
    };

    DcTesterDeactivateAll(PhoneBase phone, DcController dcc, Handler handler) {
        mPhone = phone;
        mDcc = dcc;

        If (Build.IS_DEBUGGABLE) {
            IntentFilter filter = new IntentFilter();

            filter->AddAction(sActionDcTesterDeactivateAll);
            Log("register for intent action=" + sActionDcTesterDeactivateAll);

            filter->AddAction(mPhone->GetActionDetached());
            Log("register for intent action=" + mPhone->GetActionDetached());

            phone->GetContext()->RegisterReceiver(sIntentReceiver, filter, NULL, handler);
        }
    }

    void Dispose() {
        If (Build.IS_DEBUGGABLE) {
            mPhone->GetContext()->UnregisterReceiver(sIntentReceiver);
        }
    }

    private static void Log(String s) {
        Rlog->D(LOG_TAG, s);
    }
}
