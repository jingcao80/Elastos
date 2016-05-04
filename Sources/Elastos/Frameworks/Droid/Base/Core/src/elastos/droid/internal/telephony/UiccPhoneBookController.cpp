/*
 * Copyright (C) 2008 The Android Open Source Project
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

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::IIccPhoneBookInterfaceManagerProxy;
using Elastos::Droid::Internal::Telephony::IIIccPhoneBook;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::Uicc::IAdnRecord;

using Elastos::Utility::IList;

public class UiccPhoneBookController extends IIccPhoneBook.Stub {
    private static const String TAG = "UiccPhoneBookController";
    private Phone[] mPhone;

    /* only one UiccPhoneBookController exists */
    public UiccPhoneBookController(Phone[] phone) {
        If (ServiceManager->GetService("simphonebook") == NULL) {
               ServiceManager->AddService("simphonebook", this);
        }
        mPhone = phone;
    }

    public Boolean
    UpdateAdnRecordsInEfBySearch (Int32 efid, String oldTag, String oldPhoneNumber,
            String newTag, String newPhoneNumber, String pin2) throws android.os.RemoteException {
        return UpdateAdnRecordsInEfBySearchForSubscriber(GetDefaultSubId(), efid, oldTag,
                oldPhoneNumber, newTag, newPhoneNumber, pin2);
    }

    public Boolean
    UpdateAdnRecordsInEfBySearchForSubscriber(Int64 subId, Int32 efid, String oldTag,
            String oldPhoneNumber, String newTag, String newPhoneNumber,
            String pin2) throws android.os.RemoteException {
        IccPhoneBookInterfaceManagerProxy iccPbkIntMgrProxy =
                             GetIccPhoneBookInterfaceManagerProxy(subId);
        If (iccPbkIntMgrProxy != NULL) {
            return iccPbkIntMgrProxy->UpdateAdnRecordsInEfBySearch(efid, oldTag,
                    oldPhoneNumber, newTag, newPhoneNumber, pin2);
        } else {
            Rlog->E(TAG,"updateAdnRecordsInEfBySearch iccPbkIntMgrProxy is" +
                      " NULL for Subscription:"+subId);
            return FALSE;
        }
    }

    public Boolean
    UpdateAdnRecordsInEfByIndex(Int32 efid, String newTag,
            String newPhoneNumber, Int32 index, String pin2) throws android.os.RemoteException {
        return UpdateAdnRecordsInEfByIndexForSubscriber(GetDefaultSubId(), efid, newTag,
                newPhoneNumber, index, pin2);
    }

    public Boolean
    UpdateAdnRecordsInEfByIndexForSubscriber(Int64 subId, Int32 efid, String newTag,
            String newPhoneNumber, Int32 index, String pin2) throws android.os.RemoteException {
        IccPhoneBookInterfaceManagerProxy iccPbkIntMgrProxy =
                             GetIccPhoneBookInterfaceManagerProxy(subId);
        If (iccPbkIntMgrProxy != NULL) {
            return iccPbkIntMgrProxy->UpdateAdnRecordsInEfByIndex(efid, newTag,
                    newPhoneNumber, index, pin2);
        } else {
            Rlog->E(TAG,"updateAdnRecordsInEfByIndex iccPbkIntMgrProxy is" +
                      " NULL for Subscription:"+subId);
            return FALSE;
        }
    }

    public Int32[] GetAdnRecordsSize(Int32 efid) throws android.os.RemoteException {
        return GetAdnRecordsSizeForSubscriber(GetDefaultSubId(), efid);
    }

    public Int32[]
    GetAdnRecordsSizeForSubscriber(Int64 subId, Int32 efid) throws android.os.RemoteException {
        IccPhoneBookInterfaceManagerProxy iccPbkIntMgrProxy =
                             GetIccPhoneBookInterfaceManagerProxy(subId);
        If (iccPbkIntMgrProxy != NULL) {
            return iccPbkIntMgrProxy->GetAdnRecordsSize(efid);
        } else {
            Rlog->E(TAG,"getAdnRecordsSize iccPbkIntMgrProxy is" +
                      " NULL for Subscription:"+subId);
            return NULL;
        }
    }

    public List<AdnRecord> GetAdnRecordsInEf(Int32 efid) throws android.os.RemoteException {
        return GetAdnRecordsInEfForSubscriber(GetDefaultSubId(), efid);
    }

    public List<AdnRecord> GetAdnRecordsInEfForSubscriber(Int64 subId, Int32 efid)
           throws android.os.RemoteException {
        IccPhoneBookInterfaceManagerProxy iccPbkIntMgrProxy =
                             GetIccPhoneBookInterfaceManagerProxy(subId);
        If (iccPbkIntMgrProxy != NULL) {
            return iccPbkIntMgrProxy->GetAdnRecordsInEf(efid);
        } else {
            Rlog->E(TAG,"getAdnRecordsInEf iccPbkIntMgrProxy is" +
                      "NULL for Subscription:"+subId);
            return NULL;
        }
    }

    public Boolean
    UpdateAdnRecordsWithContentValuesInEfBySearch(Int32 efid, ContentValues values,
        String pin2) throws android.os.RemoteException {
            return UpdateAdnRecordsWithContentValuesInEfBySearchUsingSubId(
                GetDefaultSubId(), efid, values, pin2);
    }

    public Boolean
    UpdateAdnRecordsWithContentValuesInEfBySearchUsingSubId(Int64 subId, Int32 efid,
        ContentValues values, String pin2)
        throws android.os.RemoteException {

        IccPhoneBookInterfaceManagerProxy iccPbkIntMgrProxy =
                             GetIccPhoneBookInterfaceManagerProxy(subId);
        If (iccPbkIntMgrProxy != NULL) {
            return iccPbkIntMgrProxy->UpdateAdnRecordsWithContentValuesInEfBySearch(
                efid, values, pin2);
        } else {
            Rlog->E(TAG,"updateAdnRecordsWithContentValuesInEfBySearchUsingSubId " +
                "iccPbkIntMgrProxy is NULL for Subscription:"+subId);
            return FALSE;
        }
    }

    public Int32 GetAdnCount() throws android.os.RemoteException {
        return GetAdnCountUsingSubId(GetDefaultSubId());
    }

    public Int32 GetAdnCountUsingSubId(Int64 subId) throws android.os.RemoteException {
        IccPhoneBookInterfaceManagerProxy iccPbkIntMgrProxy =
                             GetIccPhoneBookInterfaceManagerProxy(subId);
        If (iccPbkIntMgrProxy != NULL) {
            return iccPbkIntMgrProxy->GetAdnCount();
        } else {
            Rlog->E(TAG,"getAdnCount iccPbkIntMgrProxy is" +
                      "NULL for Subscription:"+subId);
            return 0;
        }
    }

    public Int32 GetAnrCount() throws android.os.RemoteException {
        return GetAnrCountUsingSubId(GetDefaultSubId());
    }

    public Int32 GetAnrCountUsingSubId(Int64 subId) throws android.os.RemoteException {
        IccPhoneBookInterfaceManagerProxy iccPbkIntMgrProxy =
                             GetIccPhoneBookInterfaceManagerProxy(subId);
        If (iccPbkIntMgrProxy != NULL) {
            return iccPbkIntMgrProxy->GetAnrCount();
        } else {
            Rlog->E(TAG,"getAnrCount iccPbkIntMgrProxy is" +
                      "NULL for Subscription:"+subId);
            return 0;
        }
    }

    public Int32 GetEmailCount() throws android.os.RemoteException {
        return GetEmailCountUsingSubId(GetDefaultSubId());
    }

    public Int32 GetEmailCountUsingSubId(Int64 subId) throws android.os.RemoteException {
        IccPhoneBookInterfaceManagerProxy iccPbkIntMgrProxy =
                             GetIccPhoneBookInterfaceManagerProxy(subId);
        If (iccPbkIntMgrProxy != NULL) {
            return iccPbkIntMgrProxy->GetEmailCount();
        } else {
            Rlog->E(TAG,"getEmailCount iccPbkIntMgrProxy is" +
                      "NULL for Subscription:"+subId);
            return 0;
        }
    }

    public Int32 GetSpareAnrCount() throws android.os.RemoteException {
        return GetSpareAnrCountUsingSubId(GetDefaultSubId());
    }

    public Int32 GetSpareAnrCountUsingSubId(Int64 subId) throws android.os.RemoteException {
        IccPhoneBookInterfaceManagerProxy iccPbkIntMgrProxy =
                             GetIccPhoneBookInterfaceManagerProxy(subId);
        If (iccPbkIntMgrProxy != NULL) {
            return iccPbkIntMgrProxy->GetSpareAnrCount();
        } else {
            Rlog->E(TAG,"getSpareAnrCount iccPbkIntMgrProxy is" +
                      "NULL for Subscription:"+subId);
            return 0;
        }
    }

    public Int32 GetSpareEmailCount() throws android.os.RemoteException {
        return GetSpareEmailCountUsingSubId(GetDefaultSubId());
    }

    public Int32 GetSpareEmailCountUsingSubId(Int64 subId) throws android.os.RemoteException {
        IccPhoneBookInterfaceManagerProxy iccPbkIntMgrProxy =
                             GetIccPhoneBookInterfaceManagerProxy(subId);
        If (iccPbkIntMgrProxy != NULL) {
            return iccPbkIntMgrProxy->GetSpareEmailCount();
        } else {
            Rlog->E(TAG,"getSpareEmailCount iccPbkIntMgrProxy is" +
                      "NULL for Subscription:"+subId);
            return 0;
        }
    }

    /**
     * get phone book interface manager proxy object based on subscription.
     **/
    private IccPhoneBookInterfaceManagerProxy
            GetIccPhoneBookInterfaceManagerProxy(Int64 subId) {
        Int32 phoneId = SubscriptionController->GetInstance()->GetPhoneId(subId);
        try {
            Return ((PhoneProxy)mPhone[phoneId]).GetIccPhoneBookInterfaceManagerProxy();
        } Catch (NullPointerException e) {
            Rlog->E(TAG, "Exception is :"+e->ToString()+" For subscription :"+subId );
            e->PrintStackTrace(); //To print stack trace
            return NULL;
        } Catch (ArrayIndexOutOfBoundsException e) {
            Rlog->E(TAG, "Exception is :"+e->ToString()+" For subscription :"+subId );
            e->PrintStackTrace();
            return NULL;
        }
    }

    private Int64 GetDefaultSubId() {
        return SubscriptionController->GetInstance()->GetDefaultSubId();
    }
}
