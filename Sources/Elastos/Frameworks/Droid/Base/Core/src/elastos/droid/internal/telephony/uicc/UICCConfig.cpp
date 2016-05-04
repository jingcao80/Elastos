/*
 * Copyright (C) 2014 The CyanogenMod Project
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

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Telephony::IRlog;

/**
 * A class that stores various UICC Settings/values.
 * @hide
 */
public class UICCConfig
{
    private final String PREFERENCE_NAME = "UICCConfig";
    private final String TAG = "UICCConfig";
    private final Boolean LOG_DEBUG = FALSE;

    private String mImsi;
    private Int32 mMncLength;

    /**
     * A method to get the stored Imsi.
     * @hide
     */
    public String GetImsi() {
        If (mImsi == NULL) {
            Logd("Getting IMSI: NULL");
        } else {
            Logd("Getting IMSI: " + mImsi);
        }
        return mImsi;
    }

    /**
     * A method to set the stored Imsi.
     * @hide
     */
    CARAPI SetImsi(String lImsi) {
        Logd("Setting IMSI: " + lImsi);
        mImsi = lImsi;
    }

    /**
     * A method to get the stored MncLength.
     * @hide
     */
    public Int32 GetMncLength() {
        Logd("Getting MncLength: " + Integer->ToString(mMncLength));
        return mMncLength;
    }

    /**
     * A method to set the stored MncLength.
     * @hide
     */
    CARAPI SetMncLength(Int32 lMncLength) {
        Logd("Setting MncLength: " + Integer->ToString(lMncLength));
        mMncLength = lMncLength;
    }

    private void Logd(String sLog) {
        If (LOG_DEBUG) {
            Rlog->D(TAG, sLog);
        }
    }

    private void Loge(String sLog)
    {
        Rlog->E(TAG, sLog);
    }

}