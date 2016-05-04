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

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Ims::IImsCall;
using Elastos::Droid::Ims::IImsConferenceState;
using Elastos::Droid::Internal::Telephony::Imsphone::IImsPhone;
using Elastos::Droid::Internal::Telephony::Imsphone::IImsPhoneCall;
using Elastos::Droid::Internal::Telephony::Test::ITestConferenceEventPackageParser;

using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;

/**
 * Telephony tester receives the following intents where {name} is the phone name
 *
 * adb shell am broadcast -a com.android.internal.telephony.{name}.action_detached
 * adb shell am broadcast -a com.android.internal.telephony.{name}.action_attached
 * adb shell am broadcast -a com.android.internal.telephony.TestConferenceEventPackage -e filename
 *      test_filename.xml
 */
public class TelephonyTester {
    private static const String LOG_TAG = "TelephonyTester";
    private static const Boolean DBG = TRUE;

    /**
     * Test-only intent used to send a test conference event package to the IMS framework.
     */
    private static const String ACTION_TEST_CONFERENCE_EVENT_PACKAGE =
            "com.android.internal.telephony.TestConferenceEventPackage";
    private static const String EXTRA_FILENAME = "filename";

    private PhoneBase mPhone;

    // The static intent receiver one for all instances and we assume this
    // is running on the same thread as Dcc.
    protected BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
            //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            If (DBG) Log("sIntentReceiver.onReceive: action=" + action);
            If (action->Equals(mPhone->GetActionDetached())) {
                Log("simulate detaching");
                mPhone->GetServiceStateTracker().mDetachedRegistrants->NotifyRegistrants();
            } else If (action->Equals(mPhone->GetActionAttached())) {
                Log("simulate attaching");
                mPhone->GetServiceStateTracker().mAttachedRegistrants->NotifyRegistrants();
            } else If (action->Equals(ACTION_TEST_CONFERENCE_EVENT_PACKAGE)) {
                Log("inject simulated conference event package");
                HandleTestConferenceEventPackage(context, intent->GetStringExtra(EXTRA_FILENAME));
            } else {
                If (DBG) Log("onReceive: unknown action=" + action);
            }
        }
    };

    TelephonyTester(PhoneBase phone) {
        mPhone = phone;

        If (Build.IS_DEBUGGABLE) {
            IntentFilter filter = new IntentFilter();

            filter->AddAction(mPhone->GetActionDetached());
            Log("register for intent action=" + mPhone->GetActionDetached());

            filter->AddAction(mPhone->GetActionAttached());
            Log("register for intent action=" + mPhone->GetActionAttached());

            If (mPhone->GetPhoneType() == PhoneConstants.PHONE_TYPE_IMS) {
                Log("register for intent action=" + ACTION_TEST_CONFERENCE_EVENT_PACKAGE);
                filter->AddAction(ACTION_TEST_CONFERENCE_EVENT_PACKAGE);
            }

            phone->GetContext()->RegisterReceiver(mIntentReceiver, filter, NULL, mPhone->GetHandler());
        }
    }

    void Dispose() {
        If (Build.IS_DEBUGGABLE) {
            mPhone->GetContext()->UnregisterReceiver(mIntentReceiver);
        }
    }

    private static void Log(String s) {
        Rlog->D(LOG_TAG, s);
    }

    /**
     * Handles request to send a test conference event package to the active Ims call.
     *
     * @see com.android.internal.telephony.test.TestConferenceEventPackageParser
     * @param context The context.
     * @param fileName The name of the test conference event package file to read.
     */
    private void HandleTestConferenceEventPackage(Context context, String fileName) {
        // Attempt to get the active IMS call before parsing the test XML file.
        ImsPhone imsPhone = (ImsPhone) mPhone;
        If (imsPhone == NULL) {
            return;
        }

        ImsPhoneCall imsPhoneCall = imsPhone->GetForegroundCall();
        If (imsPhoneCall == NULL) {
            return;
        }

        ImsCall imsCall = imsPhoneCall->GetImsCall();
        If (imsCall == NULL) {
            return;
        }

        File packageFile = new File(context->GetFilesDir(), fileName);
        final FileInputStream is;
        try {
            is = new FileInputStream(packageFile);
        } Catch (FileNotFoundException ex) {
            Log("Test conference event package file not found: " + packageFile->GetAbsolutePath());
            return;
        }

        TestConferenceEventPackageParser parser = new TestConferenceEventPackageParser(is);
        ImsConferenceState imsConferenceState = parser->Parse();
        If (imsConferenceState == NULL) {
            return;
        }

        imsCall->ConferenceStateUpdated(imsConferenceState);
    }
}
