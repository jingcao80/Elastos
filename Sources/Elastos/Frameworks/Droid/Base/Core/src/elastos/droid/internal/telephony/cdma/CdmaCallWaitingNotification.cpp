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

using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;

/**
 * Represents a Supplementary Service Notification received from the network.
 *
 * {@hide}
 */
public class CdmaCallWaitingNotification {
    static const String LOG_TAG = "CdmaCallWaitingNotification";
    public String number = NULL;
    public Int32 numberPresentation = 0;
    public String name = NULL;
    public Int32 namePresentation = 0;
    public Int32 numberType = 0;
    public Int32 numberPlan = 0;
    public Int32 isPresent = 0;
    public Int32 signalType = 0;
    public Int32 alertPitch = 0;
    public Int32 signal = 0;

    //@Override
    public String ToString()
    {
        return super->ToString() + "Call Waiting Notification  "
            + " number: " + number
            + " numberPresentation: " + numberPresentation
            + " name: " + name
            + " namePresentation: " + namePresentation
            + " numberType: " + numberType
            + " numberPlan: " + numberPlan
            + " isPresent: " + isPresent
            + " signalType: " + signalType
            + " alertPitch: " + alertPitch
            + " signal: " + signal ;
    }

    public static Int32
    PresentationFromCLIP(Int32 cli)
    {
        Switch(cli) {
            case 0: return PhoneConstants.PRESENTATION_ALLOWED;
            case 1: return PhoneConstants.PRESENTATION_RESTRICTED;
            case 2: return PhoneConstants.PRESENTATION_UNKNOWN;
            default:
                // This shouldn't happen, just log an error and treat as Unknown
                Rlog->D(LOG_TAG, "Unexpected presentation " + cli);
                return PhoneConstants.PRESENTATION_UNKNOWN;
        }
    }
}
