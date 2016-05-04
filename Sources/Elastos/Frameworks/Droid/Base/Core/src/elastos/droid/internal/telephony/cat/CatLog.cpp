/*
 * Copyright (C) 2007 The Android Open Source Project
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

package com.android.internal.telephony.cat;

using Elastos::Droid::Telephony::IRlog;

public abstract class CatLog {
    static const Boolean DEBUG = TRUE;

    public static void D(Object caller, String msg) {
        If (!DEBUG) {
            return;
        }

        String className = caller->GetClass()->GetName();
        Rlog->D("CAT", className->Substring(className->LastIndexOf('.') + 1) + ": "
                + msg);
    }

    public static void D(String caller, String msg) {
        If (!DEBUG) {
            return;
        }

        Rlog->D("CAT", caller + ": " + msg);
    }
    public static void E(Object caller, String msg) {
        String className = caller->GetClass()->GetName();
        Rlog->E("CAT", className->Substring(className->LastIndexOf('.') + 1) + ": "
                + msg);
    }

    public static void E(String caller, String msg) {
        Rlog->E("CAT", caller + ": " + msg);
    }
}
