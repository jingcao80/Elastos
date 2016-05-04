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

package com.android.internal.telephony.gsm;

/**
 * Call fail causes from TS 24.008 .
 * These are mostly the cause codes we need to distinguish for the UI.
 * See 22.001 Annex F.4 for mapping of cause codes to local tones.
 *
 * {@hide}
 *
 */
public interface CallFailCause {
    // Unassigned/Unobtainable number
    static const Int32 UNOBTAINABLE_NUMBER = 1;

    static const Int32 NORMAL_CLEARING     = 16;
    // Busy Tone
    static const Int32 USER_BUSY           = 17;

    // No Tone
    static const Int32 NUMBER_CHANGED      = 22;
    static const Int32 STATUS_ENQUIRY      = 30;
    static const Int32 NORMAL_UNSPECIFIED  = 31;

    // Congestion Tone
    static const Int32 NO_CIRCUIT_AVAIL    = 34;
    static const Int32 TEMPORARY_FAILURE   = 41;
    static const Int32 SWITCHING_CONGESTION    = 42;
    static const Int32 CHANNEL_NOT_AVAIL   = 44;
    static const Int32 QOS_NOT_AVAIL       = 49;
    static const Int32 BEARER_NOT_AVAIL    = 58;

    // others
    static const Int32 ACM_LIMIT_EXCEEDED = 68;
    static const Int32 CALL_BARRED        = 240;
    static const Int32 FDN_BLOCKED        = 241;

    // Stk Call Control
    static const Int32 DIAL_MODIFIED_TO_USSD = 244;
    static const Int32 DIAL_MODIFIED_TO_SS = 245;
    static const Int32 DIAL_MODIFIED_TO_DIAL = 246;

    static const Int32 ERROR_UNSPECIFIED = 0xffff;
}
