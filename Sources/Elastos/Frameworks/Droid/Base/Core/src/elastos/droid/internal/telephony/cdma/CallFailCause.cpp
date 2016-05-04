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

package com.android.internal.telephony.cdma;

/**
 * CDMA Call fail causes covering all the possible failures that are
 * needed to be distinguished by the UI. CDMA call failure reasons
 * are derived from the possible call failure scenarios described
 * in "CDMA IS2000 - Release A (C.S0005-A v6.0)" standard.
 *
 * {@hide}
 *
 */
public interface CallFailCause {
    static const Int32 NORMAL_CLEARING                = 16;
    // Busy Tone
    static const Int32 USER_BUSY                      = 17;

    static const Int32 NORMAL_UNSPECIFIED             = 31;

    // Congestion Tone
    static const Int32 NO_CIRCUIT_AVAIL               = 34;

    // others
    static const Int32 ACM_LIMIT_EXCEEDED             = 68;
    static const Int32 CALL_BARRED                    = 240;
    static const Int32 FDN_BLOCKED                    = 241;

    // Stk Call Control
    static const Int32 DIAL_MODIFIED_TO_USSD          = 244;
    static const Int32 DIAL_MODIFIED_TO_SS            = 245;
    static const Int32 DIAL_MODIFIED_TO_DIAL          = 246;

    static const Int32 CDMA_LOCKED_UNTIL_POWER_CYCLE  = 1000;
    static const Int32 CDMA_DROP                      = 1001;
    static const Int32 CDMA_INTERCEPT                 = 1002;
    static const Int32 CDMA_REORDER                   = 1003;
    static const Int32 CDMA_SO_REJECT                 = 1004;
    static const Int32 CDMA_RETRY_ORDER               = 1005;
    static const Int32 CDMA_ACCESS_FAILURE            = 1006;
    static const Int32 CDMA_PREEMPTED                 = 1007;

    // For non-emergency number dialed while in emergency callback mode.
    static const Int32 CDMA_NOT_EMERGENCY             = 1008;

    // Access Blocked by CDMA Network.
    static const Int32 CDMA_ACCESS_BLOCKED            = 1009;

    static const Int32 ERROR_UNSPECIFIED = 0xffff;
}
