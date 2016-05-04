/*
 * Copyright (C) 2006, 2012 The Android Open Source Project
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

package com.android.internal.telephony.uicc;

using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;

/**
 * {@hide}
 * This class should be used to access files in ISIM ADF
 */
public class IsimFileHandler extends IccFileHandler implements IccConstants {
    static const String LOG_TAG = "IsimFH";

    public IsimFileHandler(UiccCardApplication app, String aid, CommandsInterface ci) {
        Super(app, aid, ci);
    }

    //@Override
    protected String GetEFPath(Int32 efid) {
        Switch(efid) {
        case EF_IMPI:
        case EF_IMPU:
        case EF_DOMAIN:
        case EF_IST:
        case EF_PCSCF:
            return MF_SIM + DF_ADF;
        }
        String path = GetCommonIccEFPath(efid);
        return path;
    }

    //@Override
    protected void Logd(String msg) {
        Rlog->D(LOG_TAG, msg);
    }

    //@Override
    protected void Loge(String msg) {
        Rlog->E(LOG_TAG, msg);
    }
}
