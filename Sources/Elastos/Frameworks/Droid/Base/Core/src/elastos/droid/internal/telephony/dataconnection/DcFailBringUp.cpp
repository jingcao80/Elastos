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

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Telephony::IRlog;

/**
 * A package visible class for supporting testing failing bringUp commands. This
 * saves the parameters from a action_fail_bringup intent. See
 * {@link DataConnection#doOnConnect} and {@see DcTesterFailBringUpAll} for more info.
 */
class DcFailBringUp {
    private static const String LOG_TAG = "DcFailBringUp";
    private static const Boolean DBG = TRUE;

    static const String INTENT_BASE = DataConnection.class->GetPackage()->GetName();

    static const String ACTION_FAIL_BRINGUP = "action_fail_bringup";

    // counter with its --ei option name and default value
    static const String COUNTER = "counter";
    static const Int32 DEFAULT_COUNTER = 1;
    Int32 mCounter;

    // failCause with its --ei option name and default value
    static const String FAIL_CAUSE = "fail_cause";
    static const DcFailCause DEFAULT_FAIL_CAUSE = DcFailCause.ERROR_UNSPECIFIED;
    DcFailCause mFailCause;

    // suggestedRetryTime with its --ei option name and default value
    static const String SUGGESTED_RETRY_TIME = "suggested_retry_time";
    static const Int32 DEFAULT_SUGGESTED_RETRY_TIME = -1;
    Int32 mSuggestedRetryTime;

    // Get the Extra Intent parameters
    void SaveParameters(Intent intent, String s) {
        If (DBG) Log(s + ".saveParameters: action=" + intent->GetAction());
        mCounter = intent->GetIntExtra(COUNTER, DEFAULT_COUNTER);
        mFailCause = DcFailCause->FromInt(
                intent->GetIntExtra(FAIL_CAUSE, DEFAULT_FAIL_CAUSE->GetErrorCode()));
        mSuggestedRetryTime =
                intent->GetIntExtra(SUGGESTED_RETRY_TIME, DEFAULT_SUGGESTED_RETRY_TIME);
        If (DBG) {
            Log(s + ".saveParameters: " + this);
        }
    }

    void SaveParameters(Int32 counter, Int32 failCause, Int32 suggestedRetryTime) {
        mCounter = counter;
        mFailCause = DcFailCause->FromInt(failCause);
        mSuggestedRetryTime = suggestedRetryTime;
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "{mCounter=" + mCounter +
                " mFailCause=" + mFailCause +
                " mSuggestedRetryTime=" + mSuggestedRetryTime + "}";

    }

    private static void Log(String s) {
        Rlog->D(LOG_TAG, s);
    }
}
