/*
 * Copyright (C) 2008 The Android Open Source Project
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
using Elastos::Droid::Internal::Telephony::Uicc::IAdnRecord;


using Elastos::Utility::IList;


/**
 * SimPhoneBookInterfaceManager to provide an inter-process communication to
 * access ADN-like SIM records.
 */
public class IccPhoneBookInterfaceManagerProxy {
    private IccPhoneBookInterfaceManager mIccPhoneBookInterfaceManager;

    public IccPhoneBookInterfaceManagerProxy(IccPhoneBookInterfaceManager
            iccPhoneBookInterfaceManager) {
        mIccPhoneBookInterfaceManager = iccPhoneBookInterfaceManager;
    }

    CARAPI SetmIccPhoneBookInterfaceManager(
            IccPhoneBookInterfaceManager iccPhoneBookInterfaceManager) {
        mIccPhoneBookInterfaceManager = iccPhoneBookInterfaceManager;
    }

    public Boolean
    UpdateAdnRecordsInEfBySearch (Int32 efid,
            String oldTag, String oldPhoneNumber,
            String newTag, String newPhoneNumber,
            String pin2) {
        return mIccPhoneBookInterfaceManager->UpdateAdnRecordsInEfBySearch(
                efid, oldTag, oldPhoneNumber, newTag, newPhoneNumber, pin2);
    }

    public Boolean UpdateAdnRecordsWithContentValuesInEfBySearch(Int32 efid, ContentValues values,
            String pin2) throws android.os.RemoteException {
        return mIccPhoneBookInterfaceManager->UpdateAdnRecordsWithContentValuesInEfBySearch(efid,
                values, pin2);
    }

    public Boolean
    UpdateAdnRecordsInEfByIndex(Int32 efid, String newTag,
            String newPhoneNumber, Int32 index, String pin2) {
        return mIccPhoneBookInterfaceManager->UpdateAdnRecordsInEfByIndex(efid,
                newTag, newPhoneNumber, index, pin2);
    }

    public Int32[] GetAdnRecordsSize(Int32 efid) {
        return mIccPhoneBookInterfaceManager->GetAdnRecordsSize(efid);
    }

    public List<AdnRecord> GetAdnRecordsInEf(Int32 efid) {
        return mIccPhoneBookInterfaceManager->GetAdnRecordsInEf(efid);
    }

    public Int32 GetAdnCount() {
        return mIccPhoneBookInterfaceManager->GetAdnCount();
    }

    public Int32 GetAnrCount() {
        return mIccPhoneBookInterfaceManager->GetAnrCount();
    }

    public Int32 GetEmailCount() {
        return mIccPhoneBookInterfaceManager->GetEmailCount();
    }

    public Int32 GetSpareAnrCount() {
        return mIccPhoneBookInterfaceManager->GetSpareAnrCount();
    }

    public Int32 GetSpareEmailCount() {
        return mIccPhoneBookInterfaceManager->GetSpareEmailCount();
    }
}
