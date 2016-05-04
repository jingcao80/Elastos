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

package com.android.internal.telephony;

using Elastos::Droid::Telephony::IPhoneNumberUtils;

/**
 * See also RIL_CallForwardInfo in include/telephony/ril.h
 *
 * {@hide}
 */
public class CallForwardInfo {
    public Int32             status;      /*1 = active, 0 = not active */
    public Int32             reason;      /* from TS 27.007 7.11 "reason" */
    public Int32             serviceClass; /* Sum of CommandsInterface.SERVICE_CLASS */
    public Int32             toa;         /* "type" from TS 27.007 7.11 */
    public String          number;      /* "number" from TS 27.007 7.11 */
    public Int32             timeSeconds; /* for CF no reply only */
    public Int32             startHour;
    public Int32             startMinute;
    public Int32             endHour;
    public Int32             endMinute;

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return super->ToString() + (status == 0 ? " not active " : " active ")
            + " reason: " + reason
            + " serviceClass: " + serviceClass
            + " \"" + PhoneNumberUtils->StringFromStringAndTOA(number, toa) + "\" "
            + timeSeconds + " seconds" + " startHour:" + startHour
            + " startMinute: " + startMinute + " endHour: " + endHour
            + " endMinute: " + endMinute ;

    }
}
