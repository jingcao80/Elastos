/*
 * Copyright (C) 2006 The Android Open Source Project
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

package com.android.internal.telephony.test;

public interface SimulatedRadioControl
{
    CARAPI TriggerRing(String number);

    CARAPI ProgressConnectingCallState();

    CARAPI ProgressConnectingToActive();

    CARAPI SetAutoProgressConnectingCall(Boolean b);

    CARAPI SetNextDialFailImmediately(Boolean b);

    CARAPI SetNextCallFailCause(Int32 gsmCause);

    CARAPI TriggerHangupForeground();

    CARAPI TriggerHangupBackground();

    CARAPI TriggerHangupAll();

    CARAPI TriggerIncomingSMS(String message);

    CARAPI Shutdown();

    /** Pause responses to async requests Until (ref-counted) ResumeResponses() */
    CARAPI PauseResponses();

    /** see pauseResponses */
    CARAPI ResumeResponses();

    CARAPI TriggerSsn(Int32 type, Int32 code);

    /** Generates an incoming USSD message. */
    CARAPI TriggerIncomingUssd(String statusCode, String message);
}
