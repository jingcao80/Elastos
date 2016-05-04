/*
 * Copyright (C) 2012 The Android Open Source Project
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

using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IIccCardProxy;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

/**
 * A debug service that will dump telephony's state
 *
 * Currently this "Service" has a proxy in the phone app
 * com.android.phone.TelephonyDebugService which actually
 * invokes the dump method.
 */
public class DebugService {
    private static String TAG = "DebugService";

    /** Constructor */
    public DebugService() {
        Log("DebugService:");
    }

    /**
     * Dump the state of various objects, add calls to other objects as desired.
     */
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        Log("dump: +");
        PhoneProxy phoneProxy = NULL;
        PhoneBase phoneBase = NULL;

        try {
            phoneProxy = (PhoneProxy) PhoneFactory->GetDefaultPhone();
        } Catch (Exception e) {
            pw->Println("Telephony DebugService: Could not getDefaultPhone e=" + e);
            return;
        }
        try {
            phoneBase = (PhoneBase)phoneProxy->GetActivePhone();
        } Catch (Exception e) {
            pw->Println("Telephony DebugService: Could not PhoneBase e=" + e);
            return;
        }

        /**
         * Surround each of the sub dump's with try/catch so even
         * if one fails we'll be able to dump the next ones.
         */
        pw->Println();
        pw->Println("++++++++++++++++++++++++++++++++");
        pw->Flush();
        try {
            phoneBase->Dump(fd, pw, args);
        } Catch (Exception e) {
            e->PrintStackTrace();
        }
        pw->Flush();
        pw->Println("++++++++++++++++++++++++++++++++");
        try {
            phoneBase.mDcTracker->Dump(fd, pw, args);
        } Catch (Exception e) {
            e->PrintStackTrace();
        }
        pw->Flush();
        pw->Println("++++++++++++++++++++++++++++++++");
        try {
            phoneBase->GetServiceStateTracker()->Dump(fd, pw, args);
        } Catch (Exception e) {
            e->PrintStackTrace();
        }
        pw->Flush();
        pw->Println("++++++++++++++++++++++++++++++++");
        try {
            phoneBase->GetCallTracker()->Dump(fd, pw, args);
        } Catch (Exception e) {
            e->PrintStackTrace();
        }
        pw->Flush();
        pw->Println("++++++++++++++++++++++++++++++++");
        try {
            ((RIL)phoneBase.mCi).Dump(fd, pw, args);
        } Catch (Exception e) {
            e->PrintStackTrace();
        }
        pw->Flush();
        pw->Println("++++++++++++++++++++++++++++++++");
        try {
            UiccController->GetInstance()->Dump(fd, pw, args);
        } Catch (Exception e) {
            e->PrintStackTrace();
        }
        pw->Flush();
        pw->Println("++++++++++++++++++++++++++++++++");
        try {
            ((IccCardProxy)phoneProxy->GetIccCard()).Dump(fd, pw, args);
        } Catch (Exception e) {
            e->PrintStackTrace();
        }
        pw->Flush();
        pw->Println("++++++++++++++++++++++++++++++++");

        try {
            SubscriptionController->GetInstance()->Dump(fd, pw, args);
        } Catch (Exception e) {
            e->PrintStackTrace();
        }
        pw->Flush();
        Log("dump: -");
    }

    private static void Log(String s) {
        Rlog->D(TAG, "DebugService " + s);
    }
}
