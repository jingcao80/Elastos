/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
 * Not a Contribution.
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

using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ITelephonyManager;


using Elastos::Droid::Internal::Telephony::IIPhoneSubInfo;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneSubInfoProxy;

public class PhoneSubInfoController extends IPhoneSubInfo.Stub {
    private static const String TAG = "PhoneSubInfoController";
    private Phone[] mPhone;

    public PhoneSubInfoController(Phone[] phone) {
        mPhone = phone;
        If (ServiceManager->GetService("iphonesubinfo") == NULL) {
            ServiceManager->AddService("iphonesubinfo", this);
        }
    }

    // Use first phoneId to return unique value always
    public String GetDeviceId() {
        return GetDeviceIdForSubscriber(GetFirstPhoneSubId());
    }

    public String GetDeviceIdForSubscriber(Int64 subId) {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
        If (phoneSubInfoProxy != NULL) {
            return phoneSubInfoProxy->GetDeviceId();
        } else {
            Rlog->E(TAG,"getDeviceId phoneSubInfoProxy is NULL" +
                      " for Subscription:" + subId);
            return NULL;
        }
    }

    public String GetImeiForSubscriber(Int64 subId) {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
        If (phoneSubInfoProxy != NULL) {
            return phoneSubInfoProxy->GetImei();
        } else {
            Rlog->E(TAG,"getDeviceId phoneSubInfoProxy is NULL" +
                    " for Subscription:" + subId);
            return NULL;
        }
    }

    public String GetDeviceSvn() {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(GetFirstPhoneSubId());
        If (phoneSubInfoProxy != NULL) {
            return phoneSubInfoProxy->GetDeviceSvn();
        } else {
            Rlog->E(TAG,"getDeviceSvn phoneSubInfoProxy is NULL");
            return NULL;
        }
    }

    public String GetSubscriberId() {
        return GetSubscriberIdForSubscriber(GetDefaultSubId());
    }

    public String GetSubscriberIdForSubscriber(Int64 subId) {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
        If (phoneSubInfoProxy != NULL) {
            return phoneSubInfoProxy->GetSubscriberId();
        } else {
            Rlog->E(TAG,"getSubscriberId phoneSubInfoProxy is" +
                      " NULL for Subscription:" + subId);
            return NULL;
        }
    }

    /**
     * Retrieves the serial number of the ICC, if applicable.
     */
    public String GetIccSerialNumber() {
        return GetIccSerialNumberForSubscriber(GetDefaultSubId());
    }

    public String GetIccSerialNumberForSubscriber(Int64 subId) {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
        If (phoneSubInfoProxy != NULL) {
            return phoneSubInfoProxy->GetIccSerialNumber();
        } else {
            Rlog->E(TAG,"getIccSerialNumber phoneSubInfoProxy is" +
                      " NULL for Subscription:" + subId);
            return NULL;
        }
    }

    public String GetLine1Number() {
        return GetLine1NumberForSubscriber(GetDefaultSubId());
    }

    public String GetLine1NumberForSubscriber(Int64 subId) {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
        If (phoneSubInfoProxy != NULL) {
            return phoneSubInfoProxy->GetLine1Number();
        } else {
            Rlog->E(TAG,"getLine1Number phoneSubInfoProxy is" +
                      " NULL for Subscription:" + subId);
            return NULL;
        }
    }

    public String GetLine1AlphaTag() {
        return GetLine1AlphaTagForSubscriber(GetDefaultSubId());
    }

    public String GetLine1AlphaTagForSubscriber(Int64 subId) {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
        If (phoneSubInfoProxy != NULL) {
            return phoneSubInfoProxy->GetLine1AlphaTag();
        } else {
            Rlog->E(TAG,"getLine1AlphaTag phoneSubInfoProxy is" +
                      " NULL for Subscription:" + subId);
            return NULL;
        }
    }

    public String GetMsisdn() {
        return GetMsisdnForSubscriber(GetDefaultSubId());
    }

    public String GetMsisdnForSubscriber(Int64 subId) {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
        If (phoneSubInfoProxy != NULL) {
            return phoneSubInfoProxy->GetMsisdn();
        } else {
            Rlog->E(TAG,"getMsisdn phoneSubInfoProxy is" +
                      " NULL for Subscription:" + subId);
            return NULL;
        }
    }

    public String GetVoiceMailNumber() {
        return GetVoiceMailNumberForSubscriber(GetDefaultVoiceSubId());
    }

    public String GetVoiceMailNumberForSubscriber(Int64 subId) {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
        If (phoneSubInfoProxy != NULL) {
            return phoneSubInfoProxy->GetVoiceMailNumber();
        } else {
            Rlog->E(TAG,"getVoiceMailNumber phoneSubInfoProxy is" +
                      " NULL for Subscription:" + subId);
            return NULL;
        }
    }

    public String GetCompleteVoiceMailNumber() {
        return GetCompleteVoiceMailNumberForSubscriber(GetDefaultVoiceSubId());
    }

    public String GetCompleteVoiceMailNumberForSubscriber(Int64 subId) {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
        If (phoneSubInfoProxy != NULL) {
            return phoneSubInfoProxy->GetCompleteVoiceMailNumber();
        } else {
            Rlog->E(TAG,"getCompleteVoiceMailNumber phoneSubInfoProxy" +
                      " is NULL for Subscription:" + subId);
            return NULL;
        }
    }

    public String GetVoiceMailAlphaTag() {
        return GetVoiceMailAlphaTagForSubscriber(GetDefaultVoiceSubId());
    }

    public String GetVoiceMailAlphaTagForSubscriber(Int64 subId) {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
        If (phoneSubInfoProxy != NULL) {
            return phoneSubInfoProxy->GetVoiceMailAlphaTag();
        } else {
            Rlog->E(TAG,"getVoiceMailAlphaTag phoneSubInfoProxy is" +
                      " NULL for Subscription:" + subId);
            return NULL;
        }
    }

    /**
     * get Phone sub info proxy object based on subId.
     **/
    private PhoneSubInfoProxy GetPhoneSubInfoProxy(Int64 subId) {
        Int32 phoneId = SubscriptionController->GetInstance()->GetPhoneId(subId);
        If (phoneId < 0 || phoneId >= TelephonyManager->GetDefault()->GetPhoneCount()) {
            phoneId = 0;
        }

        try {
            Return ((PhoneProxy)mPhone[phoneId]).GetPhoneSubInfoProxy();
        } Catch (NullPointerException e) {
            Rlog->E(TAG, "Exception is :" + e->ToString() + " For subId :" + subId);
            e->PrintStackTrace();
            return NULL;
        } Catch (ArrayIndexOutOfBoundsException e) {
            Rlog->E(TAG, "Exception is :" + e->ToString() + " For subId :" + subId);
            e->PrintStackTrace();
            return NULL;
        }
    }

    private Int64 GetDefaultSubId() {
        return  SubscriptionController->GetInstance()->GetDefaultSubId();
    }

    private Int64 GetDefaultVoiceSubId() {
        return  SubscriptionController->GetInstance()->GetDefaultVoiceSubId();
    }

    private Int64 GetFirstPhoneSubId() {
        // get subId from first Phone/slot Id(i.e 0)
        Int64[] subId = SubscriptionController->GetInstance()->GetSubId(PhoneConstants.PHONE_ID1);
        return  subId[0];
    }

    public String GetIsimImpi() {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(GetDefaultSubId());
        return phoneSubInfoProxy->GetIsimImpi();
    }

    public String GetIsimDomain() {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(GetDefaultSubId());
        return phoneSubInfoProxy->GetIsimDomain();
    }

    public String[] GetIsimImpu() {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(GetDefaultSubId());
        return phoneSubInfoProxy->GetIsimImpu();
    }

    public String GetIsimIst() throws RemoteException {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(GetDefaultSubId());
        return phoneSubInfoProxy->GetIsimIst();
    }

    public String[] GetIsimPcscf() throws RemoteException {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(GetDefaultSubId());
        return phoneSubInfoProxy->GetIsimPcscf();
    }

    public String GetIsimChallengeResponse(String nonce) throws RemoteException {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(GetDefaultSubId());
        return phoneSubInfoProxy->GetIsimChallengeResponse(nonce);
    }

    public String GetIccSimChallengeResponse(Int64 subId, Int32 appType, String data)
            throws RemoteException {
        PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
        return phoneSubInfoProxy->GetIccSimChallengeResponse(subId, appType, data);
    }

     public String GetGroupIdLevel1() {
         return GetGroupIdLevel1ForSubscriber(GetDefaultSubId());
     }

     public String GetGroupIdLevel1ForSubscriber(Int64 subId) {
         PhoneSubInfoProxy phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
         If (phoneSubInfoProxy != NULL) {
             return phoneSubInfoProxy->GetGroupIdLevel1();
         } else {
             Rlog->E(TAG,"getGroupIdLevel1 phoneSubInfoProxy is" +
                       " NULL for Subscription:" + subId);
             return NULL;
         }
     }
}
