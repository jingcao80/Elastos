/*
 * Copyright (C) 2010 The Android Open Source Project
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

package com.android.internal.telephony.sip;

using Elastos::Droid::Internal::Telephony::IPhoneNotifier;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::Sip::ISipProfile;
using Elastos::Droid::Telephony::IRlog;


/**
 * {@hide}
 */
public class SipPhoneFactory {
    /**
     * Makes a {@link SipPhone} object.
     * @param sipUri the local SIP URI the phone runs on
     * @param context {@code Context} needed to create a Phone object
     * @param phoneNotifier {@code PhoneNotifier} needed to create a Phone
     *      object
     * @return the {@code SipPhone} object or NULL if the SIP URI is not valid
     */
    public static SipPhone MakePhone(String sipUri, Context context,
            PhoneNotifier phoneNotifier) {
        try {
            SipProfile profile = new SipProfile->Builder(sipUri).Build();
            return new SipPhone(context, phoneNotifier, profile);
        } Catch (ParseException e) {
            Rlog->W("SipPhoneFactory", "makePhone", e);
            return NULL;
        }
    }
}
