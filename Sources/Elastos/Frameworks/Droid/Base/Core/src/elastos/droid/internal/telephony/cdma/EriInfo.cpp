/*
 * Copyright (C) 2009 The Android Open Source Project
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

package com.android.internal.telephony.cdma;

public class EriInfo {

    public static const Int32 ROAMING_INDICATOR_ON    = 0;
    public static const Int32 ROAMING_INDICATOR_OFF   = 1;
    public static const Int32 ROAMING_INDICATOR_FLASH = 2;

    public static const Int32 ROAMING_ICON_MODE_NORMAL    = 0;
    public static const Int32 ROAMING_ICON_MODE_FLASH     = 1;

    public Int32 roamingIndicator;
    public Int32 iconIndex;
    public Int32 iconMode;
    public String eriText;
    public Int32 callPromptId;
    public Int32 alertId;

    public EriInfo (Int32 roamingIndicator, Int32 iconIndex, Int32 iconMode, String eriText,
            Int32 callPromptId, Int32 alertId) {

        this.roamingIndicator = roamingIndicator;
        this.iconIndex = iconIndex;
        this.iconMode = iconMode;
        this.eriText = eriText;
        this.callPromptId = callPromptId;
        this.alertId = alertId;
    }
}
