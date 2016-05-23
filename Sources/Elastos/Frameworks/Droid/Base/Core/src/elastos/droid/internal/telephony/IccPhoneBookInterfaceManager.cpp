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

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Droid::Internal::Telephony::Uicc::IAdnRecord;
using Elastos::Droid::Internal::Telephony::Uicc::IAdnRecordCache;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppType;
using Elastos::Droid::Internal::Telephony::Uicc::IIccConstants;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;

using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

/**
 * SimPhoneBookInterfaceManager to provide an inter-process communication to
 * access ADN-like SIM records.
 */
public abstract class IccPhoneBookInterfaceManager {
    protected static const Boolean DBG = TRUE;

    protected PhoneBase mPhone;
    private   UiccCardApplication mCurrentApp = NULL;
    protected AdnRecordCache mAdnCache;
    protected final Object mLock = new Object();
    protected Int32 mRecordSize[];
    protected Boolean mSuccess;
    private   Boolean mIs3gCard = FALSE;  // flag to determine if card is 3G or 2G
    protected List<AdnRecord> mRecords;


    protected static const Boolean ALLOW_SIM_OP_IN_UI_THREAD = FALSE;

    protected static const Int32 EVENT_GET_SIZE_DONE = 1;
    protected static const Int32 EVENT_LOAD_DONE = 2;
    protected static const Int32 EVENT_UPDATE_DONE = 3;

    protected Handler mBaseHandler = new Handler() {
        //@Override
        CARAPI HandleMessage(Message msg) {
            AsyncResult ar;

            Switch (msg.what) {
                case EVENT_GET_SIZE_DONE:
                    ar = (AsyncResult) msg.obj;
                    {    AutoLock syncLock(mLock);
                        If (ar.exception == NULL) {
                            mRecordSize = (Int32[])ar.result;
                            // recordSize[0]  is the record length
                            // recordSize[1]  is the total length of the EF file
                            // recordSize[2]  is the number of records in the EF file
                            Logd("GET_RECORD_SIZE Size " + mRecordSize[0] +
                                    " total " + mRecordSize[1] +
                                    " #record " + mRecordSize[2]);
                        }
                        NotifyPending(ar);
                    }
                    break;
                case EVENT_UPDATE_DONE:
                    ar = (AsyncResult) msg.obj;
                    {    AutoLock syncLock(mLock);
                        mSuccess = (ar.exception == NULL);
                        NotifyPending(ar);
                    }
                    break;
                case EVENT_LOAD_DONE:
                    ar = (AsyncResult)msg.obj;
                    {    AutoLock syncLock(mLock);
                        If (ar.exception == NULL) {
                            mRecords = (List<AdnRecord>) ar.result;
                        } else {
                            If(DBG) Logd("Cannot load ADN records");
                            If (mRecords != NULL) {
                                mRecords->Clear();
                            }
                        }
                        NotifyPending(ar);
                    }
                    break;
            }
        }

        private void NotifyPending(AsyncResult ar) {
            If (ar.userObj != NULL) {
                AtomicBoolean status = (AtomicBoolean) ar.userObj;
                status->Set(TRUE);
            }
            mLock->NotifyAll();
        }
    };

    public IccPhoneBookInterfaceManager(PhoneBase phone) {
        this.mPhone = phone;
    }

    private void CleanUp() {
        If (mAdnCache != NULL) {
            mAdnCache->Reset();
            mAdnCache = NULL;
        }
        mIs3gCard = FALSE;
        mCurrentApp = NULL;
    }

    CARAPI Dispose() {
        If (mRecords != NULL) {
            mRecords->Clear();
        }
        CleanUp();
    }

    CARAPI SetIccCard(UiccCard card) {
        Logd("Card update received: " + card);

        If (card == NULL) {
            Logd("Card is NULL. Cleanup");
            CleanUp();
            return;
        }

        UiccCardApplication validApp = NULL;
        Int32 numApps = card->GetNumApplications();
        Boolean isCurrentAppFound = FALSE;
        mIs3gCard = FALSE;

        For (Int32 i = 0; i < numApps; i++) {
            UiccCardApplication app = card->GetApplicationIndex(i);
            If (app != NULL) {
                // Determine if the card is a 3G card by looking
                // for a CSIM/USIM/ISIM app on the card
                AppType type = app->GetType();
                If (type == AppType.APPTYPE_CSIM || type == AppType.APPTYPE_USIM
                        || type == AppType.APPTYPE_ISIM) {
                    Logd("Card is 3G");
                    mIs3gCard = TRUE;
                }
                // Check if the app we have is present.
                // If yes, then continue using that.
                If (!isCurrentAppFound) {
                    // if not, then find a valid app.
                    // It does not matter which app, since we are
                    // accessing non-app specific files
                    If (validApp == NULL && type != AppType.APPTYPE_UNKNOWN) {
                        validApp = app;
                    }

                    If (mCurrentApp == app) {
                        Logd("Existing app found");
                        isCurrentAppFound = TRUE;
                    }
                }

                // We have determined that this is 3g card
                // and we also found the current app
                // We are done
                If (mIs3gCard && isCurrentAppFound) {
                    break;
                }
            }
        }

        //Set a new currentApp if
        // - one was not set before
        // OR
        // - the previously set app no longer exists
        If (mCurrentApp == NULL || !isCurrentAppFound) {
            If (validApp != NULL) {
                Logd("Setting currentApp: " + validApp);
                mCurrentApp = validApp;
                mAdnCache = mCurrentApp->GetIccRecords()->GetAdnCache();
            }
        }
    }

    protected abstract void Logd(String msg);

    protected abstract void Loge(String msg);

    /**
     * Replace oldAdn with newAdn in ADN-like record in EF
     *
     * getAdnRecordsInEf must be called at least once before this function,
     * otherwise an error will be returned. Currently the email field
     * if set in the ADN record is ignored.
     * throws SecurityException if no WRITE_CONTACTS permission
     *
     * @param efid must be one among EF_ADN, EF_FDN, and EF_SDN
     * @param oldTag adn tag to be replaced
     * @param oldPhoneNumber adn number to be replaced
     *        Set both oldTag and oldPhoneNubmer to "" means to replace an
     *        empty record, aka, insert new record
     * @param newTag adn tag to be stored
     * @param newPhoneNumber adn number ot be stored
     *        Set both newTag and newPhoneNubmer to "" means to replace the old
     *        record with empty one, aka, delete old record
     * @param pin2 required to update EF_FDN, otherwise must be NULL
     * @return TRUE for success
     */
    public Boolean
    UpdateAdnRecordsInEfBySearch (Int32 efid,
            String oldTag, String oldPhoneNumber,
            String newTag, String newPhoneNumber, String pin2) {


        If (mPhone->GetContext()->CheckCallingOrSelfPermission(
                Manifest::permission::WRITE_CONTACTS)
            != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException(
                    "Requires android.permission.WRITE_CONTACTS permission");
        }


        If (DBG) Logd("updateAdnRecordsInEfBySearch: efid=" + efid +
                " ("+ oldTag + "," + oldPhoneNumber + ")"+ "==>" +
                " ("+ newTag + "," + newPhoneNumber + ")"+ " pin2=" + pin2);

        efid = UpdateEfForIccType(efid);

        {    AutoLock syncLock(mLock);
            CheckThread();
            mSuccess = FALSE;
            AtomicBoolean status = new AtomicBoolean(FALSE);
            Message response = mBaseHandler->ObtainMessage(EVENT_UPDATE_DONE, status);
            AdnRecord oldAdn = new AdnRecord(oldTag, oldPhoneNumber);
            AdnRecord newAdn = new AdnRecord(newTag, newPhoneNumber);
            If (mAdnCache != NULL) {
                mAdnCache->UpdateAdnBySearch(efid, oldAdn, newAdn, pin2, response);
                WaitForResult(status);
            } else {
                Loge("Failure while trying to update by search due to uninitialised adncache");
            }
        }
        return mSuccess;
    }

    public Boolean UpdateAdnRecordsWithContentValuesInEfBySearch(Int32 efid, ContentValues values,
            String pin2) {

        If (mPhone->GetContext()->CheckCallingOrSelfPermission(
                Manifest::permission::WRITE_CONTACTS) != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException("Requires android.permission.WRITE_CONTACTS permission");
        }

        String oldTag = values->GetAsString(IccProvider.STR_TAG);
        String newTag = values->GetAsString(IccProvider.STR_NEW_TAG);
        String oldPhoneNumber = values->GetAsString(IccProvider.STR_NUMBER);
        String newPhoneNumber = values->GetAsString(IccProvider.STR_NEW_NUMBER);
        String oldEmail = values->GetAsString(IccProvider.STR_EMAILS);
        String newEmail = values->GetAsString(IccProvider.STR_NEW_EMAILS);
        String oldAnr = values->GetAsString(IccProvider.STR_ANRS);
        String newAnr = values->GetAsString(IccProvider.STR_NEW_ANRS);
        String[] oldEmailArray = TextUtils->IsEmpty(oldEmail) ? NULL : GetStringArray(oldEmail);
        String[] newEmailArray = TextUtils->IsEmpty(newEmail) ? NULL : GetStringArray(newEmail);
        String[] oldAnrArray = TextUtils->IsEmpty(oldAnr) ? NULL : GetStringArray(oldAnr);
        String[] newAnrArray = TextUtils->IsEmpty(newAnr) ? NULL : GetStringArray(newAnr);
        efid = UpdateEfForIccType(efid);

        If (DBG)
            Logd("updateAdnRecordsInEfBySearch: efid=" + efid + ", values = " + values + ", pin2="
                    + pin2);
        {    AutoLock syncLock(mLock);
            CheckThread();
            mSuccess = FALSE;
            AtomicBoolean status = new AtomicBoolean(FALSE);
            Message response = mBaseHandler->ObtainMessage(EVENT_UPDATE_DONE, status);
            AdnRecord oldAdn = new AdnRecord(oldTag, oldPhoneNumber, oldEmailArray, oldAnrArray);
            AdnRecord newAdn = new AdnRecord(newTag, newPhoneNumber, newEmailArray, newAnrArray);
            If (mAdnCache != NULL) {
                mAdnCache->UpdateAdnBySearch(efid, oldAdn, newAdn, pin2, response);
                WaitForResult(status);
            } else {
                Loge("Failure while trying to update by search due to uninitialised adncache");
            }
        }
        return mSuccess;
    }

    /**
     * Update an ADN-like EF record by record index
     *
     * This is useful for iteration the whole ADN file, such as write the whole
     * phone book or erase/format the whole phonebook. Currently the email field
     * if set in the ADN record is ignored.
     * throws SecurityException if no WRITE_CONTACTS permission
     *
     * @param efid must be one among EF_ADN, EF_FDN, and EF_SDN
     * @param newTag adn tag to be stored
     * @param newPhoneNumber adn number to be stored
     *        Set both newTag and newPhoneNubmer to "" means to replace the old
     *        record with empty one, aka, delete old record
     * @param index is 1-based adn record index to be updated
     * @param pin2 required to update EF_FDN, otherwise must be NULL
     * @return TRUE for success
     */
    public Boolean
    UpdateAdnRecordsInEfByIndex(Int32 efid, String newTag,
            String newPhoneNumber, Int32 index, String pin2) {

        If (mPhone->GetContext()->CheckCallingOrSelfPermission(
                Manifest::permission::WRITE_CONTACTS)
                != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException(
                    "Requires android.permission.WRITE_CONTACTS permission");
        }

        If (DBG) Logd("updateAdnRecordsInEfByIndex: efid=" + efid +
                " Index=" + index + " ==> " +
                "("+ newTag + "," + newPhoneNumber + ")"+ " pin2=" + pin2);
        {    AutoLock syncLock(mLock);
            CheckThread();
            mSuccess = FALSE;
            AtomicBoolean status = new AtomicBoolean(FALSE);
            Message response = mBaseHandler->ObtainMessage(EVENT_UPDATE_DONE, status);
            AdnRecord newAdn = new AdnRecord(newTag, newPhoneNumber);
            If (mAdnCache != NULL) {
                mAdnCache->UpdateAdnByIndex(efid, newAdn, index, pin2, response);
                WaitForResult(status);
            } else {
                Loge("Failure while trying to update by index due to uninitialised adncache");
            }
        }
        return mSuccess;
    }

    /**
     * Get the capacity of records in efid
     *
     * @param efid the EF id of a ADN-like ICC
     * @return  Int32[3] array
     *            recordSizes[0]  is the single record length
     *            recordSizes[1]  is the total length of the EF file
     *            recordSizes[2]  is the number of records in the EF file
     */
    public abstract Int32[] GetAdnRecordsSize(Int32 efid);

    /**
     * Loads the AdnRecords in efid and returns them as a
     * List of AdnRecords
     *
     * throws SecurityException if no READ_CONTACTS permission
     *
     * @param efid the EF id of a ADN-like ICC
     * @return List of AdnRecord
     */
    public List<AdnRecord> GetAdnRecordsInEf(Int32 efid) {

        If (mPhone->GetContext()->CheckCallingOrSelfPermission(
                Manifest::permission::READ_CONTACTS)
                != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException(
                    "Requires android.permission.READ_CONTACTS permission");
        }

        efid = UpdateEfForIccType(efid);
        If (DBG) Logd("getAdnRecordsInEF: efid=" + efid);

        {    AutoLock syncLock(mLock);
            CheckThread();
            AtomicBoolean status = new AtomicBoolean(FALSE);
            Message response = mBaseHandler->ObtainMessage(EVENT_LOAD_DONE, status);
            If (mAdnCache != NULL) {
                mAdnCache->RequestLoadAllAdnLike(efid, mAdnCache->ExtensionEfForEf(efid), response);
                WaitForResult(status);
            } else {
                Loge("Failure while trying to load from SIM due to uninitialised adncache");
            }
        }
        return mRecords;
    }

    protected void CheckThread() {
        If (!ALLOW_SIM_OP_IN_UI_THREAD) {
            // Make sure this isn't the UI thread, since it will block
            If (mBaseHandler->GetLooper()->Equals(Looper->MyLooper())) {
                Loge("Query() called on the main UI thread!");
                throw new IllegalStateException(
                        "You cannot call query on this provder from the main UI thread.");
            }
        }
    }

    private String[] GetStringArray(String str) {
        If (str != NULL) {
            return str->Split(",");
        }
        return NULL;
    }

    protected void WaitForResult(AtomicBoolean status) {
        While (!status->Get()) {
            try {
                mLock->Wait();
            } Catch (InterruptedException e) {
                Logd("interrupted while trying to update by search");
            }
        }
    }

    private Int32 UpdateEfForIccType(Int32 efid) {
        // If we are trying to read ADN records on a 3G card
        // use EF_PBR
        If (efid == IccConstants.EF_ADN && mIs3gCard) {
            Logd("Translate EF_ADN to EF_PBR");
            return IccConstants.EF_PBR;
        }
        return efid;
    }

    public Int32 GetAdnCount() {
        Int32 adnCount = 0;
        If (mAdnCache != NULL) {
            If (mPhone->GetCurrentUiccAppType() == AppType.APPTYPE_USIM) {
                adnCount = mAdnCache->GetUsimAdnCount();
            } else {
                adnCount = mAdnCache->GetAdnCount();
            }
        } else {
            Loge("mAdnCache is NULL when getAdnCount.");
        }
        return adnCount;
    }

    public Int32 GetAnrCount() {
        Int32 anrCount = 0;
        If (mAdnCache != NULL) {
            anrCount = mAdnCache->GetAnrCount();
        } else {
            Loge("mAdnCache is NULL when getAnrCount.");
        }
        return anrCount;
    }

    public Int32 GetEmailCount() {
        Int32 emailCount = 0;
        If (mAdnCache != NULL) {
            emailCount = mAdnCache->GetEmailCount();
        } else {
            Loge("mAdnCache is NULL when getEmailCount.");
        }
        return emailCount;
    }

    public Int32 GetSpareAnrCount() {
        Int32 spareAnrCount = 0;
        If (mAdnCache != NULL) {
            spareAnrCount = mAdnCache->GetSpareAnrCount();
        } else {
            Loge("mAdnCache is NULL when getSpareAnrCount.");
        }
        return spareAnrCount;
    }

    public Int32 GetSpareEmailCount() {
        Int32 spareEmailCount = 0;
        If (mAdnCache != NULL) {
            spareEmailCount = mAdnCache->GetSpareEmailCount();
        } else {
            Loge("mAdnCache is NULL when getSpareEmailCount.");
        }
        return spareEmailCount;
    }
}

