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

package com.android.internal.telephony.gsm;

using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::Uicc::IAdnRecord;
using Elastos::Droid::Internal::Telephony::Uicc::IAdnRecordCache;
using Elastos::Droid::Internal::Telephony::Uicc::IIccConstants;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IMap;

/**
 * This class implements reading and parsing USIM records.
 * Refer to Spec 3GPP TS 31.102 for more details.
 *
 * {@hide}
 */
public class UsimPhoneBookManager extends Handler implements IccConstants {
    private static const String LOG_TAG = "UsimPhoneBookManager";
    private static const Boolean DBG = TRUE;
    private PbrFile mPbrFile;
    private Boolean mIsPbrPresent;
    private IccFileHandler mFh;
    private AdnRecordCache mAdnCache;
    private Object mLock = new Object();
    private ArrayList<AdnRecord> mPhoneBookRecords;
    private Boolean mEmailPresentInIap = FALSE;
    private Int32 mEmailTagNumberInIap = 0;
    private Boolean mAnrPresentInIap = FALSE;
    private Int32 mAnrTagNumberInIap = 0;
    private Boolean mIapPresent = FALSE;
    private Map<Integer, ArrayList<Byte[]>> mIapFileRecord;
    private Map<Integer, ArrayList<Byte[]>> mEmailFileRecord;
    private Map<Integer, ArrayList<Byte[]>> mAnrFileRecord;
    private ArrayList<Integer> mAdnLengthList = NULL;
    private Int32 mPendingExtLoads;
    private Boolean mSuccess = FALSE;
    private Boolean mRefreshCache = FALSE;

    private Map<Integer, ArrayList<Integer>> mAnrFlags;
    private Map<Integer, ArrayList<Integer>> mEmailFlags;
    private ArrayList<Integer>[] mAnrFlagsRecord;
    private ArrayList<Integer>[] mEmailFlagsRecord;

    // Variable used to save valid records' recordnum
    private Map<Integer, ArrayList<Integer>> mRecordNums;

    private static const Int32 EVENT_PBR_LOAD_DONE = 1;
    private static const Int32 EVENT_USIM_ADN_LOAD_DONE = 2;
    private static const Int32 EVENT_IAP_LOAD_DONE = 3;
    private static const Int32 EVENT_EMAIL_LOAD_DONE = 4;
    private static const Int32 EVENT_ANR_LOAD_DONE = 5;
    private static const Int32 EVENT_EF_EMAIL_RECORD_SIZE_DONE = 6;
    private static const Int32 EVENT_EF_ANR_RECORD_SIZE_DONE = 7;
    private static const Int32 EVENT_UPDATE_EMAIL_RECORD_DONE = 8;
    private static const Int32 EVENT_UPDATE_ANR_RECORD_DONE = 9;
    private static const Int32 EVENT_EF_IAP_RECORD_SIZE_DONE = 10;
    private static const Int32 EVENT_UPDATE_IAP_RECORD_DONE = 11;

    private static const Int32 USIM_TYPE1_TAG   = 0xA8;
    private static const Int32 USIM_TYPE2_TAG   = 0xA9;
    private static const Int32 USIM_TYPE3_TAG   = 0xAA;
    private static const Int32 USIM_EFADN_TAG   = 0xC0;
    private static const Int32 USIM_EFIAP_TAG   = 0xC1;
    private static const Int32 USIM_EFEXT1_TAG  = 0xC2;
    private static const Int32 USIM_EFSNE_TAG   = 0xC3;
    private static const Int32 USIM_EFANR_TAG   = 0xC4;
    private static const Int32 USIM_EFPBC_TAG   = 0xC5;
    private static const Int32 USIM_EFGRP_TAG   = 0xC6;
    private static const Int32 USIM_EFAAS_TAG   = 0xC7;
    private static const Int32 USIM_EFGSD_TAG   = 0xC8;
    private static const Int32 USIM_EFUID_TAG   = 0xC9;
    private static const Int32 USIM_EFEMAIL_TAG = 0xCA;
    private static const Int32 USIM_EFCCP1_TAG  = 0xCB;

    private static const Int32 MAX_NUMBER_SIZE_BYTES = 11;
    private static const Int32 ANR_DESCRIPTION_ID = 0;
    private static const Int32 ANR_BCD_NUMBER_LENGTH = 1;
    private static const Int32 ANR_TON_NPI_ID = 2;
    private static const Int32 ANR_ADDITIONAL_NUMBER_START_ID = 3;
    private static const Int32 ANR_ADDITIONAL_NUMBER_END_ID = 12;
    private static const Int32 ANR_CAPABILITY_ID = 13;
    private static const Int32 ANR_EXTENSION_ID = 14;
    private static const Int32 ANR_ADN_SFI_ID = 15;
    private static const Int32 ANR_ADN_RECORD_IDENTIFIER_ID = 16;

    public UsimPhoneBookManager(IccFileHandler fh, AdnRecordCache cache) {
        mFh = fh;
        mPhoneBookRecords = new ArrayList<AdnRecord>();
        mAdnLengthList = new ArrayList<Integer>();
        mIapFileRecord = new HashMap<Integer, ArrayList<Byte[]>>();
        mEmailFileRecord = new HashMap<Integer, ArrayList<Byte[]>>();
        mAnrFileRecord = new HashMap<Integer, ArrayList<Byte[]>>();
        mRecordNums = new HashMap<Integer, ArrayList<Integer>>();
        mPbrFile = NULL;

        mAnrFlags = new HashMap<Integer, ArrayList<Integer>>();
        mEmailFlags = new HashMap<Integer, ArrayList<Integer>>();

        // We assume its present, after the first read this is updated.
        // So we don't have to read from UICC if its not present on subsequent reads.
        mIsPbrPresent = TRUE;
        mAdnCache = cache;
    }

    CARAPI Reset() {
        If ((mAnrFlagsRecord != NULL) && (mEmailFlagsRecord != NULL) && mPbrFile != NULL) {
            For (Int32 i = 0; i < mPbrFile.mFileIds->Size(); i++) {
                mAnrFlagsRecord[i].Clear();
                mEmailFlagsRecord[i].Clear();
            }
        }
        mAnrFlags->Clear();
        mEmailFlags->Clear();

        mPhoneBookRecords->Clear();
        mIapFileRecord->Clear();
        mEmailFileRecord->Clear();
        mAnrFileRecord->Clear();
        mRecordNums->Clear();
        mPbrFile = NULL;
        mAdnLengthList->Clear();
        mIsPbrPresent = TRUE;
        mRefreshCache = FALSE;
    }

    public ArrayList<AdnRecord> LoadEfFilesFromUsim() {
        Synchronized (mLock) {
            If (!mPhoneBookRecords->IsEmpty()) {
                If (mRefreshCache) {
                    mRefreshCache = FALSE;
                    RefreshCache();
                }
                return mPhoneBookRecords;
            }

            If (!mIsPbrPresent) return NULL;

            // Check if the PBR file is present in the cache, if not read it
            // from the USIM.
            If (mPbrFile == NULL) {
                ReadPbrFileAndWait();
            }

            If (mPbrFile == NULL) return NULL;

            Int32 numRecs = mPbrFile.mFileIds->Size();

            If ((mAnrFlagsRecord == NULL) && (mEmailFlagsRecord == NULL)) {
                mAnrFlagsRecord = new ArrayList[numRecs];
                mEmailFlagsRecord = new ArrayList[numRecs];
                For (Int32 i = 0; i < numRecs; i++) {
                    mAnrFlagsRecord[i] = new ArrayList<Integer>();
                    mEmailFlagsRecord[i] = new ArrayList<Integer>();
                }
            }

            For (Int32 i = 0; i < numRecs; i++) {
                ReadAdnFileAndWait(i);
                ReadEmailFileAndWait(i);
                ReadAnrFileAndWait(i);
            }
            // All EF files are loaded, post the response.
        }
        return mPhoneBookRecords;
    }

    private void RefreshCache() {
        If (mPbrFile == NULL) return;
        mPhoneBookRecords->Clear();

        Int32 numRecs = mPbrFile.mFileIds->Size();
        For (Int32 i = 0; i < numRecs; i++) {
            ReadAdnFileAndWait(i);
        }
    }

    CARAPI InvalidateCache() {
        mRefreshCache = TRUE;
    }

    private void ReadPbrFileAndWait() {
        mFh->LoadEFLinearFixedAll(EF_PBR, ObtainMessage(EVENT_PBR_LOAD_DONE));
        try {
            mLock->Wait();
        } Catch (InterruptedException e) {
            Rlog->E(LOG_TAG, "Interrupted Exception in readAdnFileAndWait");
        }
    }

    private void ReadEmailFileAndWait(Int32 recNum) {
        Map <Integer,Integer> fileIds;
        fileIds = mPbrFile.mFileIds->Get(recNum);
        If (fileIds == NULL) return;

        If (fileIds->ContainsKey(USIM_EFEMAIL_TAG)) {
            // Check if the EFEmail is a Type 1 file or a type 2 file.
            // If mEmailPresentInIap is TRUE, its a type 2 file.
            // So we read the IAP file and then read the email records.
            // instead of reading directly.
            If (mEmailPresentInIap) {
                ReadIapFileAndWait(fileIds->Get(USIM_EFIAP_TAG), recNum);
                If (!HasRecordIn(mIapFileRecord, recNum)) {
                    Rlog->E(LOG_TAG, "Error: IAP file is empty");
                    return;
                }
                mFh->LoadEFLinearFixedAll(fileIds->Get(USIM_EFEMAIL_TAG),
                        ObtainMessage(EVENT_EMAIL_LOAD_DONE, recNum));

                Log("readEmailFileAndWait email efid is : " + fileIds->Get(USIM_EFEMAIL_TAG));
                try {
                    mLock->Wait();
                } Catch (InterruptedException e) {
                    Rlog->E(LOG_TAG, "Interrupted Exception in readEmailFileAndWait");
                }

            } else {
                // Read all Email files per Record
                For (Int32 efid: mPbrFile.mEmailFileIds->Get(recNum)) {
                    mFh->LoadEFLinearFixedPart(efid, GetValidRecordNums(recNum),
                        ObtainMessage(EVENT_EMAIL_LOAD_DONE, recNum));

                    Log("readEmailFileAndWait email efid is : " + efid + " recNum:" + recNum);
                    try {
                        mLock->Wait();
                    } Catch (InterruptedException e) {
                        Rlog->E(LOG_TAG, "Interrupted Exception in readEmailFileAndWait");
                    }
                }
            }

            For (Int32 m = 0; m < mEmailFileRecord->Get(recNum).Size(); m++) {
                mEmailFlagsRecord[recNum].Add(0);
            }
            mEmailFlags->Put(recNum, mEmailFlagsRecord[recNum]);

            If (!HasRecordIn(mEmailFileRecord, recNum)) {
                Rlog->E(LOG_TAG, "Error: Email file is empty");
                return;
            }
            UpdatePhoneAdnRecordWithEmail(recNum);
        }

    }

    private void ReadAnrFileAndWait(Int32 recNum) {
        Map<Integer, Integer> fileIds;
        If (mPbrFile == NULL) {
            Rlog->E(LOG_TAG, "mPbrFile is NULL, exiting from readAnrFileAndWait");
            return;
        } else {
            fileIds = mPbrFile.mFileIds->Get(recNum);
        }
        If (fileIds == NULL || fileIds->IsEmpty())
            return;
        If (fileIds->ContainsKey(USIM_EFANR_TAG)) {
            If (mAnrPresentInIap) {
                ReadIapFileAndWait(fileIds->Get(USIM_EFIAP_TAG), recNum);
                If (!HasRecordIn(mIapFileRecord, recNum)) {
                    Rlog->E(LOG_TAG, "Error: IAP file is empty");
                    return;
                }
                mFh->LoadEFLinearFixedAll(fileIds->Get(USIM_EFANR_TAG),
                        ObtainMessage(EVENT_ANR_LOAD_DONE, recNum));
                Log("readAnrFileAndWait anr efid is : " + fileIds->Get(USIM_EFANR_TAG));
                try {
                    mLock->Wait();
                } Catch (InterruptedException e) {
                    Rlog->E(LOG_TAG, "Interrupted Exception in readEmailFileAndWait");
                }
            } else {
                // Read all Anr files for each Adn Record
                For (Int32 efid: mPbrFile.mAnrFileIds->Get(recNum)) {
                    mFh->LoadEFLinearFixedPart(efid, GetValidRecordNums(recNum),
                        ObtainMessage(EVENT_ANR_LOAD_DONE, recNum));
                    Log("readAnrFileAndWait anr efid is : " + efid + " recNum:" + recNum);
                    try {
                        mLock->Wait();
                    } Catch (InterruptedException e) {
                        Rlog->E(LOG_TAG, "Interrupted Exception in readEmailFileAndWait");
                    }
                }
            }

            For (Int32 m = 0; m < mAnrFileRecord->Get(recNum).Size(); m++) {
                mAnrFlagsRecord[recNum].Add(0);
            }
            mAnrFlags->Put(recNum, mAnrFlagsRecord[recNum]);

            If (!HasRecordIn(mAnrFileRecord, recNum)) {
                Rlog->E(LOG_TAG, "Error: Anr file is empty");
                return;
            }
            UpdatePhoneAdnRecordWithAnr(recNum);
        }
    }

    private void ReadIapFileAndWait(Int32 efid, Int32 recNum) {
        Log("pbrIndex is " + recNum + ",iap efid is : " + efid);
        mFh->LoadEFLinearFixedPart(efid, GetValidRecordNums(recNum),
                ObtainMessage(EVENT_IAP_LOAD_DONE, recNum));
        try {
            mLock->Wait();
        } Catch (InterruptedException e) {
            Rlog->E(LOG_TAG, "Interrupted Exception in readIapFileAndWait");
        }
    }

    public Boolean UpdateEmailFile(Int32 adnRecNum, String oldEmail, String newEmail, Int32 efidIndex) {
        Int32 pbrIndex = GetPbrIndexBy(adnRecNum - 1);
        Int32 efid = GetEfidByTag(pbrIndex, USIM_EFEMAIL_TAG, efidIndex);
        If (oldEmail == NULL)
            oldEmail = "";
        If (newEmail == NULL)
            newEmail = "";
        String emails = oldEmail + "," + newEmail;
        mSuccess = FALSE;

        Log("updateEmailFile oldEmail : " + oldEmail + " newEmail:" + newEmail + " emails:"
                    + emails + " efid" + efid + " adnRecNum: " + adnRecNum);

        If (efid == -1)
            return mSuccess;
        If (mEmailPresentInIap && (TextUtils->IsEmpty(oldEmail) && !TextUtils->IsEmpty(newEmail))) {
            If (GetEmptyEmailNum_Pbrindex(pbrIndex) == 0) {
                Log("updateEmailFile getEmptyEmailNum_Pbrindex=0, pbrIndex is " + pbrIndex);
                mSuccess = TRUE;
                return mSuccess;
            }

            mSuccess = UpdateIapFile(adnRecNum, oldEmail, newEmail, USIM_EFEMAIL_TAG);
        } else {
            mSuccess = TRUE;
        }
        If (mSuccess) {
            Synchronized (mLock) {
                mFh->GetEFLinearRecordSize(efid,
                        ObtainMessage(EVENT_EF_EMAIL_RECORD_SIZE_DONE, adnRecNum, efid, emails));
                try {
                    mLock->Wait();
                } Catch (InterruptedException e) {
                    Rlog->E(LOG_TAG, "interrupted while trying to update by search");
                }
            }
        }
        If (mEmailPresentInIap && mSuccess
                && (!TextUtils->IsEmpty(oldEmail) && TextUtils->IsEmpty(newEmail))) {
            mSuccess = UpdateIapFile(adnRecNum, oldEmail, newEmail, USIM_EFEMAIL_TAG);
        }
        return mSuccess;
    }

    public Boolean UpdateAnrFile(Int32 adnRecNum, String oldAnr, String newAnr, Int32 efidIndex) {
        Int32 pbrIndex = GetPbrIndexBy(adnRecNum - 1);
        Int32 efid = GetEfidByTag(pbrIndex, USIM_EFANR_TAG, efidIndex);
        If (oldAnr == NULL)
            oldAnr = "";
        If (newAnr == NULL)
            newAnr = "";
        String anrs = oldAnr + "," + newAnr;
        mSuccess = FALSE;
        Log("updateAnrFile oldAnr : " + oldAnr + ", newAnr:" + newAnr + " anrs:" + anrs + ", efid"
                + efid + ", adnRecNum: " + adnRecNum);
        If (efid == -1)
            return mSuccess;
        If (mAnrPresentInIap && (TextUtils->IsEmpty(oldAnr) && !TextUtils->IsEmpty(newAnr))) {
            If (GetEmptyAnrNum_Pbrindex(pbrIndex) == 0) {
                Log("updateAnrFile getEmptyAnrNum_Pbrindex=0, pbrIndex is " + pbrIndex);
                mSuccess = TRUE;
                return mSuccess;
            }

            mSuccess = UpdateIapFile(adnRecNum, oldAnr, newAnr, USIM_EFANR_TAG);
        } else {
            mSuccess = TRUE;
        }
        Synchronized (mLock) {
            mFh->GetEFLinearRecordSize(efid,
                    ObtainMessage(EVENT_EF_ANR_RECORD_SIZE_DONE, adnRecNum, efid, anrs));
            try {
                mLock->Wait();
            } Catch (InterruptedException e) {
                Rlog->E(LOG_TAG, "interrupted while trying to update by search");
            }
        }
        If (mAnrPresentInIap && mSuccess
                && (!TextUtils->IsEmpty(oldAnr) && TextUtils->IsEmpty(newAnr))) {
            mSuccess = UpdateIapFile(adnRecNum, oldAnr, newAnr, USIM_EFANR_TAG);
        }
        return mSuccess;
    }

    private Boolean UpdateIapFile(Int32 adnRecNum, String oldValue, String newValue, Int32 tag) {
        Int32 pbrIndex = GetPbrIndexBy(adnRecNum - 1);
        Int32 efid = GetEfidByTag(pbrIndex, USIM_EFIAP_TAG, 0);
        mSuccess = FALSE;
        Int32 recordNumber = -1;
        If (efid == -1)
            return mSuccess;
        Switch (tag) {
            case USIM_EFEMAIL_TAG:
                recordNumber = GetEmailRecNumber(adnRecNum - 1,
                        mPhoneBookRecords->Size(), oldValue);
                break;
            case USIM_EFANR_TAG:
                recordNumber = GetAnrRecNumber(adnRecNum - 1, mPhoneBookRecords->Size(), oldValue);
                break;
        }
        If (TextUtils->IsEmpty(newValue)) {
            recordNumber = -1;
        }
        Log("updateIapFile  efid=" + efid + ", recordNumber= " + recordNumber + ", adnRecNum="
                + adnRecNum);
        Synchronized (mLock) {
            mFh->GetEFLinearRecordSize(efid,
                    ObtainMessage(EVENT_EF_IAP_RECORD_SIZE_DONE, adnRecNum, recordNumber, tag));
            try {
                mLock->Wait();
            } Catch (InterruptedException e) {
                Rlog->E(LOG_TAG, "interrupted while trying to update by search");
            }
        }
        return mSuccess;
    }

    private Int32 GetEfidByTag(Int32 recNum, Int32 tag, Int32 efidIndex) {
        Map<Integer, Integer> fileIds;
        Int32 efid = -1;
        Int32 numRecs = mPbrFile.mFileIds->Size();

        fileIds = mPbrFile.mFileIds->Get(recNum);
        If (fileIds == NULL)
            return -1;

        If (!fileIds->ContainsKey(tag)) {
            return -1;
        }

        If (!mEmailPresentInIap && USIM_EFEMAIL_TAG == tag) {
            efid = mPbrFile.mEmailFileIds->Get(recNum).Get(efidIndex);
        } else If (!mAnrPresentInIap && USIM_EFANR_TAG == tag) {
            efid = mPbrFile.mAnrFileIds->Get(recNum).Get(efidIndex);
        } else {
            efid = fileIds->Get(tag);
        }
        return efid;
    }

    public Int32 GetPbrIndexBy(Int32 adnIndex) {
        Int32 len = mAdnLengthList->Size();
        Int32 size = 0;
        For (Int32 i = 0; i < len; i++) {
            size += mAdnLengthList->Get(i);
            If (adnIndex < size) {
                return i;
            }
        }
        return -1;
    }

    private Int32 GetInitIndexBy(Int32 pbrIndex) {
        Int32 index = 0;
        While (pbrIndex > 0) {
            index += mAdnLengthList->Get(pbrIndex - 1);
            pbrIndex--;
        }
        return index;
    }

    private Boolean HasRecordIn(Map<Integer, ArrayList<Byte[]>> record, Int32 pbrIndex) {
        If (record == NULL)
            return FALSE;
        try {
            record->Get(pbrIndex);
        } Catch (IndexOutOfBoundsException e) {
            Rlog->E(LOG_TAG, "record is empty in pbrIndex" + pbrIndex);
            return FALSE;
        }
        return TRUE;
    }

    private void UpdatePhoneAdnRecordWithEmail(Int32 pbrIndex) {
        If (!HasRecordIn(mEmailFileRecord, pbrIndex))
            return;
        Int32 numAdnRecs = mAdnLengthList->Get(pbrIndex);
        Int32 adnRecIndex;
        If (mEmailPresentInIap && HasRecordIn(mIapFileRecord, pbrIndex)) {
            // The number of records in the IAP file is same as the number of records in ADN file.
            // The order of the pointers in an EFIAP shall be the same as the order of file IDs
            // that appear in the TLV object indicated by Tag 'A9' in the reference file record.
            // i.e value of mEmailTagNumberInIap

            For (Int32 i = 0; i < numAdnRecs; i++) {
                Byte[] record = NULL;
                try {
                    record = mIapFileRecord->Get(pbrIndex).Get(i);
                } Catch (IndexOutOfBoundsException e) {
                    Rlog->E(LOG_TAG, "Error: Improper ICC card: No IAP record for ADN, continuing");
                    break;
                }
                Int32 recNum = record[mEmailTagNumberInIap];

                If (recNum > 0) {
                    String[] emails = new String[1];
                    // SIM record numbers are 1 based
                    emails[0] = ReadEmailRecord(recNum - 1, pbrIndex, 0);
                    adnRecIndex = i + GetInitIndexBy(pbrIndex);
                    AdnRecord rec = mPhoneBookRecords->Get(adnRecIndex);
                    If (rec != NULL && (!TextUtils->IsEmpty(emails[0]))) {
                        rec->SetEmails(emails);
                        mPhoneBookRecords->Set(adnRecIndex, rec);
                        mEmailFlags->Get(pbrIndex).Set(recNum - 1, 1);
                    }
                }
            }

            Log("updatePhoneAdnRecordWithEmail: no need to parse type1 EMAIL file");
            return;
        }

        // ICC cards can be made such that they have an IAP file but all
        // records are empty. So we read both type 1 and type 2 file
        // email records, just to be sure.

        Int32 len = mAdnLengthList->Get(pbrIndex);
        // Type 1 file, the number of records is the same as the number of
        // records in the ADN file.
        If (!mEmailPresentInIap) {
            ParseType1EmailFile(len, pbrIndex);
        }
    }

    private void UpdatePhoneAdnRecordWithAnr(Int32 pbrIndex) {
        If (!HasRecordIn(mAnrFileRecord, pbrIndex))
            return;
        Int32 numAdnRecs = mAdnLengthList->Get(pbrIndex);
        Int32 adnRecIndex;
        If (mAnrPresentInIap && HasRecordIn(mIapFileRecord, pbrIndex)) {
            // The number of records in the IAP file is same as the number of records in ADN file.
            // The order of the pointers in an EFIAP shall be the same as the order of file IDs
            // that appear in the TLV object indicated by Tag 'A9' in the reference file record.
            // i.e value of mAnrTagNumberInIap

            For (Int32 i = 0; i < numAdnRecs; i++) {
                Byte[] record = NULL;
                try {
                    record = mIapFileRecord->Get(pbrIndex).Get(i);
                } Catch (IndexOutOfBoundsException e) {
                    Rlog->E(LOG_TAG, "Error: Improper ICC card: No IAP record for ADN, continuing");
                    break;
                }
                Int32 recNum = record[mAnrTagNumberInIap];
                If (recNum > 0) {
                    String[] anrs = new String[1];
                    // SIM record numbers are 1 based
                    anrs[0] = ReadAnrRecord(recNum - 1, pbrIndex, 0);
                    adnRecIndex = i + GetInitIndexBy(pbrIndex);
                    AdnRecord rec = mPhoneBookRecords->Get(adnRecIndex);
                    If (rec != NULL && (!TextUtils->IsEmpty(anrs[0]))) {
                        rec->SetAdditionalNumbers(anrs);
                        mPhoneBookRecords->Set(adnRecIndex, rec);
                    }

                }
            }

            Log("updatePhoneAdnRecordWithAnr: no need to parse type1 ANR file");
            return;
        }

        // ICC cards can be made such that they have an IAP file but all
        // records are empty. So we read both type 1 and type 2 file
        // anr records, just to be sure.

        // Type 1 file, the number of records is the same as the number of
        // records in the ADN file.
        If (!mAnrPresentInIap) {
            ParseType1AnrFile(numAdnRecs, pbrIndex);
        }
    }

    void ParseType1EmailFile(Int32 numRecs, Int32 pbrIndex) {
        Byte[] emailRec = NULL;
        Int32 count;
        Int32 numEmailFiles = mPbrFile.mEmailFileIds->Get(pbrIndex).Size();
        ArrayList<String> emailList = new ArrayList<String>();
        Int32 adnInitIndex = GetInitIndexBy(pbrIndex);

        If (!HasRecordIn(mEmailFileRecord, pbrIndex))
            return;

        Log("parseType1EmailFile: pbrIndex is: " + pbrIndex + ", numRecs is: " + numRecs);
        For (Int32 i = 0; i < numRecs; i++) {
            count = 0;
            emailList->Clear();

            For (Int32 j = 0; j < numEmailFiles; j++) {
                String email = ReadEmailRecord(i, pbrIndex, j*numRecs);
                emailList->Add(email);
                If (TextUtils->IsEmpty(email)) {
                    email = "";
                    continue;
                }

                count++;
                //Type1 Email file, no need for mEmailFlags
                mEmailFlags->Get(pbrIndex).Set(i+j*numRecs, 1);
            }

            // All Email files is NULL
            If (count == 0) {
                continue;
            }

            AdnRecord rec = mPhoneBookRecords->Get(i+adnInitIndex);
            If (rec != NULL) {
                String[] emails = new String[emailList->Size()];
                System->Arraycopy(emailList->ToArray(), 0, emails, 0, emailList->Size());
                rec->SetAdditionalNumbers(emails);
                mPhoneBookRecords->Set(i, rec);
            }
        }
    }

    void ParseType1AnrFile(Int32 numRecs, Int32 pbrIndex) {
        Int32 count;
        Int32 numAnrFiles = mPbrFile.mAnrFileIds->Get(pbrIndex).Size();
        ArrayList<String> anrList = new ArrayList<String>();
        Int32 adnInitIndex = GetInitIndexBy(pbrIndex);

        If (!HasRecordIn(mAnrFileRecord, pbrIndex))
            return;
        Log("parseType1AnrFile: pbrIndex is: " + pbrIndex + ", numRecs is: " + numRecs +
                ", numAnrFiles " + numAnrFiles);
        For (Int32 i = 0; i < numRecs; i++) {
            count = 0;
            anrList->Clear();
            For (Int32 j = 0; j < numAnrFiles; j++) {
                String anr = ReadAnrRecord(i, pbrIndex, j*numRecs);
                anrList->Add(anr);
                If (TextUtils->IsEmpty(anr)) {
                    anr = "";
                    continue;
                }

                count++;
                //Fix Me: For type1 this is not necessary
                mAnrFlags->Get(pbrIndex).Set(i+j*numRecs, 1);
            }

            // All anr files is NULL
            If (count == 0) {
                continue;
            }

            AdnRecord rec = mPhoneBookRecords->Get(i+adnInitIndex);
            If (rec != NULL) {
                String[] anrs = new String[anrList->Size()];
                System->Arraycopy(anrList->ToArray(), 0, anrs, 0, anrList->Size());
                rec->SetAdditionalNumbers(anrs);
                mPhoneBookRecords->Set(i, rec);
            }
        }
    }

    private String ReadEmailRecord(Int32 recNum, Int32 pbrIndex, Int32 offSet) {
        Byte[] emailRec = NULL;
        If (!HasRecordIn(mEmailFileRecord, pbrIndex))
            return NULL;
        try {
            emailRec = mEmailFileRecord->Get(pbrIndex).Get(recNum + offSet);
        } Catch (IndexOutOfBoundsException e) {
            return NULL;
        }

        // The length of the record is X+2 Byte, where X bytes is the email address
        String email = IccUtils->AdnStringFieldToString(emailRec, 0, emailRec.length - 2);
        return email;
    }

    private String ReadAnrRecord(Int32 recNum, Int32 pbrIndex, Int32 offSet) {
        Byte[] anrRec = NULL;
        If (!HasRecordIn(mAnrFileRecord, pbrIndex))
            return NULL;
        try {
            anrRec = mAnrFileRecord->Get(pbrIndex).Get(recNum + offSet);
        } Catch (IndexOutOfBoundsException e) {
            Rlog->E(LOG_TAG, "Error: Improper ICC card: No anr record for ADN, continuing");
            return NULL;
        }
        Int32 numberLength = 0xff & anrRec[1];
        If (numberLength > MAX_NUMBER_SIZE_BYTES) {
            //Log("Invalid number length[" + numberLength + "] in anr record: " + recNum +
            //        " pbrIndex:" + pbrIndex + " offSet:" + offSet);
            return "";
        }
        return PhoneNumberUtils->CalledPartyBCDToString(anrRec, 2, numberLength);
    }

    private void ReadAdnFileAndWait(Int32 recNum) {
        Map <Integer,Integer> fileIds;
        fileIds = mPbrFile.mFileIds->Get(recNum);
        If (fileIds == NULL || fileIds->IsEmpty()) return;


        Int32 extEf = 0;
        // Only call fileIds.get while EFEXT1_TAG is available
        If (fileIds->ContainsKey(USIM_EFEXT1_TAG)) {
            extEf = fileIds->Get(USIM_EFEXT1_TAG);
        }
        Log("readAdnFileAndWait adn efid is : " + fileIds->Get(USIM_EFADN_TAG));
        mAdnCache->RequestLoadAllAdnLike(fileIds->Get(USIM_EFADN_TAG), extEf,
                ObtainMessage(EVENT_USIM_ADN_LOAD_DONE, recNum));
        try {
            mLock->Wait();
        } Catch (InterruptedException e) {
            Rlog->E(LOG_TAG, "Interrupted Exception in readAdnFileAndWait");
        }
    }

    private Int32 GetEmailRecNumber(Int32 adnRecIndex, Int32 numRecs, String oldEmail) {
        Int32 pbrIndex = GetPbrIndexBy(adnRecIndex);
        Int32 recordIndex = adnRecIndex - GetInitIndexBy(pbrIndex);
        Int32 recordNumber = -1;
        Log("getEmailRecNumber adnRecIndex is: " + adnRecIndex + ", recordIndex is :"
                + recordIndex);
        If (!HasRecordIn(mEmailFileRecord, pbrIndex)) {
            Log("getEmailRecNumber recordNumber is: " + recordNumber);
            return recordNumber;
        }
        If (mEmailPresentInIap && HasRecordIn(mIapFileRecord, pbrIndex)) {
            Byte[] record = NULL;
            try {
                record = mIapFileRecord->Get(pbrIndex).Get(recordIndex);
            } Catch (IndexOutOfBoundsException e) {
                Rlog->E(LOG_TAG, "IndexOutOfBoundsException in getEmailRecNumber");
            }
            If (record != NULL && record[mEmailTagNumberInIap] > 0) {
                recordNumber = record[mEmailTagNumberInIap];
                Log(" getEmailRecNumber: record is " + IccUtils->BytesToHexString(record)
                        + ", the email recordNumber is :" + recordNumber);
                return recordNumber;
            } else {
                Int32 recsSize = mEmailFileRecord->Get(pbrIndex).Size();
                Log("getEmailRecNumber recsSize is: " + recsSize);
                If (TextUtils->IsEmpty(oldEmail)) {
                    For (Int32 i = 0; i < recsSize; i++) {
                        String emailRecord = ReadEmailRecord(i, pbrIndex, 0);
                        If (TextUtils->IsEmpty(emailRecord)) {
                            Log("getEmailRecNumber: Got empty record.Email record num is :" +
                                     (i + 1));
                            return i + 1;
                        }
                    }
                }
            }
        } else {
            recordNumber = recordIndex + 1;
            return recordNumber;
        }
        Log("getEmailRecNumber: no email record index found");
        return recordNumber;
    }

    private Int32 GetAnrRecNumber(Int32 adnRecIndex, Int32 numRecs, String oldAnr) {
        Int32 pbrIndex = GetPbrIndexBy(adnRecIndex);
        Int32 recordIndex = adnRecIndex - GetInitIndexBy(pbrIndex);
        Int32 recordNumber = -1;
        If (!HasRecordIn(mAnrFileRecord, pbrIndex)) {
            return recordNumber;
        }
        If (mAnrPresentInIap && HasRecordIn(mIapFileRecord, pbrIndex)) {
            Byte[] record = NULL;
            try {
                record = mIapFileRecord->Get(pbrIndex).Get(recordIndex);
            } Catch (IndexOutOfBoundsException e) {
                Rlog->E(LOG_TAG, "IndexOutOfBoundsException in getAnrRecNumber");
            }
            If (record != NULL && record[mAnrTagNumberInIap] > 0) {
                recordNumber = record[mAnrTagNumberInIap];
                Log("getAnrRecNumber: recnum from iap is :" + recordNumber);
                return recordNumber;
            } else {
                Int32 recsSize = mAnrFileRecord->Get(pbrIndex).Size();
                Log("getAnrRecNumber: anr record size is :" + recsSize);
                If (TextUtils->IsEmpty(oldAnr)) {
                    For (Int32 i = 0; i < recsSize; i++) {
                        String anrRecord = ReadAnrRecord(i, pbrIndex, 0);
                        If (TextUtils->IsEmpty(anrRecord)) {
                            Log("getAnrRecNumber: Empty anr record. Anr record num is :" + (i + 1));
                            return i + 1;
                        }
                    }
                }
            }
        } else {
            recordNumber = recordIndex + 1;
            return recordNumber;
        }
        Log("getAnrRecNumber: no anr record index found");
        return recordNumber;
    }

    private Byte[] BuildEmailData(Int32 length, Int32 adnRecIndex, String email) {
        Byte[] data = new Byte[length];
        For (Int32 i=0; i<length; i++ ) {
            data[i]= (Byte)0xff;
        }
        If (TextUtils->IsEmpty(email)) {
            Log("[buildEmailData] Empty email record");
            return data; // return the empty Record (for delete)
        }
        Byte[] byteEmail = GsmAlphabet->StringToGsm8BitPacked(email);
        System->Arraycopy(byteEmail, 0, data, 0, byteEmail.length);
        Int32 pbrIndex = GetPbrIndexBy(adnRecIndex);
        Int32 recordIndex = adnRecIndex - GetInitIndexBy(pbrIndex);
        If (mEmailPresentInIap) {
            data[length - 1] = (Byte) (recordIndex + 1);
        }
        Log("buildEmailData: data is" + IccUtils->BytesToHexString(data));
        return data;
    }

    private Byte[] BuildAnrData(Int32 length, Int32 adnRecIndex, String anr) {
        Byte[] data = new Byte[length];
        For (Int32 i = 0; i < length; i++) {
            data[i] = (Byte) 0xff;
        }
        If (TextUtils->IsEmpty(anr)) {
            Log("[buildAnrData] Empty anr record");
            return data; // return the empty Record (for delete)
        }
        data[ANR_DESCRIPTION_ID] = (Byte) (0x0);
        Byte[] byteAnr = PhoneNumberUtils->NumberToCalledPartyBCD(anr);

        // If the phone number does not matching format, like "+" return NULL.
        If (byteAnr == NULL) {
            return NULL;
        }

        // numberToCalledPartyBCD has considered TOA Byte
        Int32 maxlength = ANR_ADDITIONAL_NUMBER_END_ID - ANR_ADDITIONAL_NUMBER_START_ID + 1 + 1;
        If (byteAnr.length > maxlength) {
            System->Arraycopy(byteAnr, 0, data, ANR_TON_NPI_ID, maxlength);
            data[ANR_BCD_NUMBER_LENGTH] = (Byte) (maxlength);
        } else {
            System->Arraycopy(byteAnr, 0, data, ANR_TON_NPI_ID, byteAnr.length);
            data[ANR_BCD_NUMBER_LENGTH] = (Byte) (byteAnr.length);
        }
        data[ANR_CAPABILITY_ID] = (Byte) 0xFF;
        data[ANR_EXTENSION_ID] = (Byte) 0xFF;
        If (length == 17) {
            Int32 pbrIndex = GetPbrIndexBy(adnRecIndex);
            Int32 recordIndex = adnRecIndex - GetInitIndexBy(pbrIndex);
            data[ANR_ADN_RECORD_IDENTIFIER_ID] = (Byte) (recordIndex + 1);
        }
        Log("buildAnrData: data is" + IccUtils->BytesToHexString(data));
        return data;
    }

    private void CreatePbrFile(ArrayList<Byte[]> records) {
        If (records == NULL) {
            mPbrFile = NULL;
            mIsPbrPresent = FALSE;
            return;
        }
        mPbrFile = new PbrFile(records);
    }

    private void PutValidRecNums(Int32 pbrIndex) {
        ArrayList<Integer> recordNums = new ArrayList<Integer>();
        Int32 initAdnIndex = GetInitIndexBy(pbrIndex);
        Log("pbr index is " + pbrIndex + ", initAdnIndex is " + initAdnIndex);
        For (Int32 i = 0; i < mAdnLengthList->Get(pbrIndex); i++) {
            recordNums->Add(i + 1);
        }
        // Need to read at least one record to inint
        // variable mIapFileRecord, mEmailFileRecord,mAnrFileRecord
        If (recordNums->Size() == 0) {
            recordNums->Add(1);
        }
        mRecordNums->Put(pbrIndex, recordNums);
    }

    private ArrayList<Integer> GetValidRecordNums(Int32 pbrIndex) {
        return mRecordNums->Get(pbrIndex);
    }

    private Boolean HasValidRecords(Int32 pbrIndex) {
        return mRecordNums->Get(pbrIndex).Size() > 0;
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;
        Byte data[];
        Int32 efid;
        Int32 adnRecIndex;
        Int32 recordIndex;
        Int32[] recordSize;
        Int32 recordNumber;
        Int32 efidIndex;
        Int32 actualRecNumber;
        String oldAnr = NULL;
        String newAnr = NULL;
        String oldEmail = NULL;
        String newEmail = NULL;
        Message response = NULL;
        Int32 pbrIndex;
        Switch (msg.what) {
            case EVENT_PBR_LOAD_DONE:
                Log("Loading PBR done");
                ar = (AsyncResult) msg.obj;
                If (ar.exception == NULL) {
                    CreatePbrFile((ArrayList<Byte[]>) ar.result);
                }
                Synchronized (mLock) {
                    mLock->Notify();
                }
                break;
            case EVENT_USIM_ADN_LOAD_DONE:
                Log("Loading USIM ADN records done");
                ar = (AsyncResult) msg.obj;
                pbrIndex = (Integer) ar.userObj;
                If (ar.exception == NULL) {
                    mPhoneBookRecords->AddAll((ArrayList<AdnRecord>) ar.result);
                    mAdnLengthList->Add(pbrIndex, ((ArrayList<AdnRecord>) ar.result).Size());
                    PutValidRecNums(pbrIndex);
                } else {
                    Log("can't load USIM ADN records");
                }
                Synchronized (mLock) {
                    mLock->Notify();
                }
                break;
            case EVENT_IAP_LOAD_DONE:
                Log("Loading USIM IAP records done");
                ar = (AsyncResult) msg.obj;
                pbrIndex = (Integer) ar.userObj;
                If (ar.exception == NULL) {
                    mIapFileRecord->Put(pbrIndex, (ArrayList<Byte[]>) ar.result);
                }
                Synchronized (mLock) {
                    mLock->Notify();
                }
                break;
            case EVENT_EMAIL_LOAD_DONE:
                Log("Loading USIM Email records done");
                ar = (AsyncResult) msg.obj;
                pbrIndex = (Integer) ar.userObj;
                If (ar.exception == NULL && mPbrFile != NULL) {
                    ArrayList<Byte[]> tmpList = mEmailFileRecord->Get(pbrIndex);
                    If (tmpList == NULL) {
                        mEmailFileRecord->Put(pbrIndex, (ArrayList<Byte[]>) ar.result);
                    } else {
                        tmpList->AddAll((ArrayList<Byte[]>) ar.result);
                        mEmailFileRecord->Put(pbrIndex, tmpList);
                    }

                    Log("handlemessage EVENT_EMAIL_LOAD_DONE size is: "
                            + mEmailFileRecord->Get(pbrIndex).Size());
                }
                Synchronized (mLock) {
                    mLock->Notify();
                }
                break;
            case EVENT_ANR_LOAD_DONE:
                Log("Loading USIM Anr records done");
                ar = (AsyncResult) msg.obj;
                pbrIndex = (Integer) ar.userObj;
                If (ar.exception == NULL && mPbrFile != NULL) {
                    ArrayList<Byte[]> tmp = mAnrFileRecord->Get(pbrIndex);
                    If (tmp == NULL) {
                        mAnrFileRecord->Put(pbrIndex, (ArrayList<Byte[]>) ar.result);
                    } else {
                        tmp->AddAll((ArrayList<Byte[]>) ar.result);
                        mAnrFileRecord->Put(pbrIndex, tmp);
                    }

                    Log("handlemessage EVENT_ANR_LOAD_DONE size is: "
                            + mAnrFileRecord->Get(pbrIndex).Size());
                }
                Synchronized (mLock) {
                    mLock->Notify();
                }
                break;
            case EVENT_EF_EMAIL_RECORD_SIZE_DONE:
                Log("Loading EF_EMAIL_RECORD_SIZE_DONE");
                ar = (AsyncResult) (msg.obj);
                String emails = (String) (ar.userObj);
                adnRecIndex = ((Int32) msg.arg1) - 1;
                efid = (Int32) msg.arg2;
                String email[] = emails->Split(",");
                If (email.length == 1) {
                    oldEmail = email[0];
                    newEmail = "";
                } else If (email.length > 1) {
                    oldEmail = email[0];
                    newEmail = email[1];
                }
                If (ar.exception != NULL) {
                    mSuccess = FALSE;
                    Synchronized (mLock) {
                        mLock->Notify();
                    }
                    return;
                }
                recordSize = (Int32[]) ar.result;
                recordNumber = GetEmailRecNumber(adnRecIndex, mPhoneBookRecords->Size(), oldEmail);
                If (recordSize.length != 3 || recordNumber > recordSize[2] || recordNumber <= 0) {
                    mSuccess = FALSE;
                    Synchronized (mLock) {
                        mLock->Notify();
                    }
                    return;
                }
                data = BuildEmailData(recordSize[0], adnRecIndex, newEmail);

                actualRecNumber = recordNumber;
                If (!mEmailPresentInIap) {
                    efidIndex = mPbrFile.mEmailFileIds->Get(GetPbrIndexBy(
                                adnRecIndex)).IndexOf(efid);
                    If (efidIndex == -1) {
                        Log("wrong efid index:" + efid );
                        return;
                    }

                    actualRecNumber = recordNumber +
                            efidIndex * mAdnLengthList->Get(GetPbrIndexBy(adnRecIndex));
                    Log("EMAIL index:" + efidIndex + " efid:" + efid +
                        " actual RecNumber:" + actualRecNumber);
                }

                mFh->UpdateEFLinearFixed(
                        efid,
                        recordNumber,
                        data,
                        NULL,
                        ObtainMessage(EVENT_UPDATE_EMAIL_RECORD_DONE, actualRecNumber, adnRecIndex,
                                data));
                mPendingExtLoads = 1;
                break;
            case EVENT_EF_ANR_RECORD_SIZE_DONE:
                Log("Loading EF_ANR_RECORD_SIZE_DONE");
                ar = (AsyncResult) (msg.obj);
                String anrs = (String) (ar.userObj);
                adnRecIndex = ((Int32) msg.arg1) - 1;
                efid = (Int32) msg.arg2;
                String[] anr = anrs->Split(",");
                If (anr.length == 1) {
                    oldAnr = anr[0];
                    newAnr = "";
                } else If (anr.length > 1) {
                    oldAnr = anr[0];
                    newAnr = anr[1];
                }
                If (ar.exception != NULL) {
                    mSuccess = FALSE;
                    Synchronized (mLock) {
                        mLock->Notify();
                    }
                    return;
                }
                recordSize = (Int32[]) ar.result;
                recordNumber = GetAnrRecNumber(adnRecIndex, mPhoneBookRecords->Size(), oldAnr);
                If (recordSize.length != 3 || recordNumber > recordSize[2] || recordNumber <= 0) {
                    mSuccess = FALSE;
                    Synchronized (mLock) {
                        mLock->Notify();
                    }
                    return;
                }
                data = BuildAnrData(recordSize[0], adnRecIndex, newAnr);
                If (data == NULL) {
                    mSuccess = FALSE;
                    Synchronized (mLock) {
                        mLock->Notify();
                    }
                    return;
                }

                actualRecNumber = recordNumber;
                If (!mAnrPresentInIap) {
                    efidIndex = mPbrFile.mAnrFileIds->Get(GetPbrIndexBy(adnRecIndex)).IndexOf(efid);
                    If (efidIndex == -1) {
                        Log("wrong efid index:" + efid );
                        return;
                    }
                    actualRecNumber = recordNumber +
                            efidIndex * mAdnLengthList->Get(GetPbrIndexBy(adnRecIndex));

                    Log("ANR index:" + efidIndex + " efid:" + efid +
                        " actual RecNumber:" + actualRecNumber);
                }

                mFh->UpdateEFLinearFixed(
                        efid,
                        recordNumber,
                        data,
                        NULL,
                        ObtainMessage(EVENT_UPDATE_ANR_RECORD_DONE, actualRecNumber,
                                adnRecIndex, data));
                mPendingExtLoads = 1;
                break;
            case EVENT_UPDATE_EMAIL_RECORD_DONE:
                Log("Loading UPDATE_EMAIL_RECORD_DONE");
                ar = (AsyncResult) (msg.obj);
                If (ar.exception != NULL) {
                    mSuccess = FALSE;
                }
                data = (Byte[]) (ar.userObj);
                recordNumber = (Int32) msg.arg1;
                adnRecIndex = (Int32) msg.arg2;
                pbrIndex = GetPbrIndexBy(adnRecIndex);
                Log("EVENT_UPDATE_EMAIL_RECORD_DONE");
                mPendingExtLoads = 0;
                mSuccess = TRUE;
                mEmailFileRecord->Get(pbrIndex).Set(recordNumber - 1, data);

                For (Int32 i = 0; i < data.length; i++) {
                    Log("EVENT_UPDATE_EMAIL_RECORD_DONE data = " + data[i] + ",i is " + i);
                    If (data[i] != (Byte) 0xff) {
                        Log("EVENT_UPDATE_EMAIL_RECORD_DONE data !=0xff");
                        mEmailFlags->Get(pbrIndex).Set(recordNumber - 1, 1);
                        break;
                    }
                    mEmailFlags->Get(pbrIndex).Set(recordNumber - 1, 0);
                }
                Synchronized (mLock) {
                    mLock->Notify();
                }
                break;
            case EVENT_UPDATE_ANR_RECORD_DONE:
                Log("Loading UPDATE_ANR_RECORD_DONE");
                ar = (AsyncResult) (msg.obj);
                data = (Byte[]) (ar.userObj);
                recordNumber = (Int32) msg.arg1;
                adnRecIndex = (Int32) msg.arg2;
                pbrIndex = GetPbrIndexBy(adnRecIndex);
                If (ar.exception != NULL) {
                    mSuccess = FALSE;
                }
                Log("EVENT_UPDATE_ANR_RECORD_DONE");
                mPendingExtLoads = 0;
                mSuccess = TRUE;
                mAnrFileRecord->Get(pbrIndex).Set(recordNumber - 1, data);

                For (Int32 i = 0; i < data.length; i++) {
                    If (data[i] != (Byte) 0xff) {
                        mAnrFlags->Get(pbrIndex).Set(recordNumber - 1, 1);
                        break;
                    }
                    mAnrFlags->Get(pbrIndex).Set(recordNumber - 1, 0);
                }
                Synchronized (mLock) {
                    mLock->Notify();
                }
                break;
            case EVENT_EF_IAP_RECORD_SIZE_DONE:
                Log("EVENT_EF_IAP_RECORD_SIZE_DONE");
                ar = (AsyncResult) (msg.obj);
                recordNumber = (Int32) msg.arg2;
                adnRecIndex = ((Int32) msg.arg1) - 1;
                pbrIndex = GetPbrIndexBy(adnRecIndex);
                efid = GetEfidByTag(pbrIndex, USIM_EFIAP_TAG, 0);
                Int32 tag = (Integer) ar.userObj;
                If (ar.exception != NULL) {
                    mSuccess = FALSE;
                    Synchronized (mLock) {
                        mLock->Notify();
                    }
                    return;
                }
                pbrIndex = GetPbrIndexBy(adnRecIndex);
                efid = GetEfidByTag(pbrIndex, USIM_EFIAP_TAG, 0);
                recordSize = (Int32[]) ar.result;
                data = NULL;

                recordIndex = adnRecIndex - GetInitIndexBy(pbrIndex);
                Log("handleIAP_RECORD_SIZE_DONE adnRecIndex is: "
                        + adnRecIndex + ", recordNumber is: " + recordNumber
                        + ", recordIndex is: " + recordIndex);
                If (recordSize.length != 3 || recordIndex + 1 > recordSize[2]
                        || recordNumber == 0) {
                    mSuccess = FALSE;
                    Synchronized (mLock) {
                        mLock->Notify();
                    }
                    return;
                }
                If (HasRecordIn(mIapFileRecord, pbrIndex)) {
                    data = mIapFileRecord->Get(pbrIndex).Get(recordIndex);
                    Byte[] record_data = new Byte[data.length];
                    System->Arraycopy(data, 0, record_data, 0, record_data.length);
                    Switch (tag) {
                        case USIM_EFEMAIL_TAG:
                            record_data[mEmailTagNumberInIap] = (Byte) recordNumber;
                            break;
                        case USIM_EFANR_TAG:
                            record_data[mAnrTagNumberInIap] = (Byte) recordNumber;
                            break;
                    }
                    mPendingExtLoads = 1;
                    Log(" IAP  efid= " + efid + ", update IAP index= " + (recordIndex)
                            + " with value= " + IccUtils->BytesToHexString(record_data));
                    mFh->UpdateEFLinearFixed(
                            efid,
                            recordIndex + 1,
                            record_data,
                            NULL,
                            ObtainMessage(EVENT_UPDATE_IAP_RECORD_DONE, adnRecIndex, recordNumber,
                                    record_data));
                }
                break;
            case EVENT_UPDATE_IAP_RECORD_DONE:
                Log("EVENT_UPDATE_IAP_RECORD_DONE");
                ar = (AsyncResult) (msg.obj);
                If (ar.exception != NULL) {
                    mSuccess = FALSE;
                }
                data = (Byte[]) (ar.userObj);
                adnRecIndex = (Int32) msg.arg1;
                pbrIndex = GetPbrIndexBy(adnRecIndex);
                recordIndex = adnRecIndex - GetInitIndexBy(pbrIndex);
                Log("handleMessage EVENT_UPDATE_IAP_RECORD_DONE recordIndex is: "
                        + recordIndex + ", adnRecIndex is: " + adnRecIndex);
                mPendingExtLoads = 0;
                mSuccess = TRUE;

                mIapFileRecord->Get(pbrIndex).Set(recordIndex, data);
                Log("the iap email recordNumber is :" + data[mEmailTagNumberInIap]);
                Synchronized (mLock) {
                    mLock->Notify();
                }
                break;
        }
    }

    private class PbrFile {
        // RecNum <EF Tag, efid>
        HashMap<Integer,Map<Integer,Integer>> mFileIds;
        // All Type1 ANR/EMAIL file will be recorded below ArrayList
        HashMap<Integer,ArrayList<Integer>> mAnrFileIds;
        HashMap<Integer,ArrayList<Integer>> mEmailFileIds;

        PbrFile(ArrayList<Byte[]> records) {
            mFileIds = new HashMap<Integer, Map<Integer, Integer>>();
            mAnrFileIds = new HashMap<Integer, ArrayList<Integer>>();
            mEmailFileIds = new HashMap<Integer, ArrayList<Integer>>();
            SimTlv recTlv;
            Int32 recNum = 0;
            For (Byte[] record: records) {
                recTlv = new SimTlv(record, 0, record.length);
                ParseTag(recTlv, recNum);
                recNum ++;
            }
        }

        void ParseTag(SimTlv tlv, Int32 recNum) {
            Rlog->D(LOG_TAG, "parseTag: recNum=" + recNum);
            SimTlv tlvEf;
            Int32 tag;
            Byte[] data;
            Map<Integer, Integer> val = new HashMap<Integer, Integer>();
            ArrayList<Integer> anrList = new ArrayList<Integer>();
            ArrayList<Integer> emailList= new ArrayList<Integer>();
            do {
                tag = tlv->GetTag();
                Switch(tag) {
                case USIM_TYPE1_TAG: // A8
                case USIM_TYPE3_TAG: // AA
                case USIM_TYPE2_TAG: // A9
                    data = tlv->GetData();
                    tlvEf = new SimTlv(data, 0, data.length);
                    ParseEf(tlvEf, val, tag, anrList, emailList);
                    break;
                }
            } While (tlv->NextObject());

            If (0 != anrList->Size()) {
                mAnrFileIds->Put(recNum, anrList);
                Rlog->D(LOG_TAG, "parseTag: recNum=" + recNum + " ANR file list:" + anrList);
            }
            If (0 != emailList->Size()) {
                Rlog->D(LOG_TAG, "parseTag: recNum=" + recNum + " EMAIL file list:" + emailList);
                mEmailFileIds->Put(recNum, emailList);
            }
            mFileIds->Put(recNum, val);
        }

        void ParseEf(SimTlv tlv, Map<Integer, Integer> val, Int32 parentTag,
                ArrayList<Integer> anrList, ArrayList<Integer> emailList) {
            Int32 tag;
            Byte[] data;
            Int32 tagNumberWithinParentTag = 0;
            do {
                tag = tlv->GetTag();
                // Check if EFIAP is present. EFIAP must be under TYPE1 tag.
                If (parentTag == USIM_TYPE1_TAG && tag == USIM_EFIAP_TAG) {
                    mIapPresent = TRUE;
                }
                If (parentTag == USIM_TYPE2_TAG && mIapPresent && tag == USIM_EFEMAIL_TAG) {
                    mEmailPresentInIap = TRUE;
                    mEmailTagNumberInIap = tagNumberWithinParentTag;
                    Log("parseEf: EmailPresentInIap tag = " + mEmailTagNumberInIap);
                }
                If (parentTag == USIM_TYPE2_TAG && mIapPresent && tag == USIM_EFANR_TAG) {
                    mAnrPresentInIap = TRUE;
                    mAnrTagNumberInIap = tagNumberWithinParentTag;
                    Log("parseEf: AnrPresentInIap tag = " + mAnrTagNumberInIap);
                }
                Switch(tag) {
                    case USIM_EFEMAIL_TAG:
                    case USIM_EFADN_TAG:
                    case USIM_EFEXT1_TAG:
                    case USIM_EFANR_TAG:
                    case USIM_EFPBC_TAG:
                    case USIM_EFGRP_TAG:
                    case USIM_EFAAS_TAG:
                    case USIM_EFGSD_TAG:
                    case USIM_EFUID_TAG:
                    case USIM_EFCCP1_TAG:
                    case USIM_EFIAP_TAG:
                    case USIM_EFSNE_TAG:
                        data = tlv->GetData();
                        Int32 efid = ((data[0] & 0xFF) << 8) | (data[1] & 0xFF);
                        val->Put(tag, efid);

                        If (parentTag == USIM_TYPE1_TAG) {
                            If (tag == USIM_EFANR_TAG) {
                                anrList->Add(efid);
                            } else If (tag == USIM_EFEMAIL_TAG) {
                                emailList->Add(efid);
                            }
                        }
                        Rlog->D(LOG_TAG, "parseEf->Put(" + tag + "," + efid + ") parent tag:"
                                + parentTag);
                        break;
                }
                tagNumberWithinParentTag ++;
            } While(tlv->NextObject());
        }
    }

    private void Log(String msg) {
        If(DBG) Rlog->D(LOG_TAG, msg);
    }

    public Int32 GetAnrCount() {
        Int32 count = 0;
        Int32 pbrIndex = mAnrFlags->Size();
        For (Int32 j = 0; j < pbrIndex; j++) {
            count += mAnrFlags->Get(j).Size();
        }
        Log("getAnrCount count is: " + count);
        return count;
    }

    public Int32 GetEmailCount() {
        Int32 count = 0;
        Int32 pbrIndex = mEmailFlags->Size();
        For (Int32 j = 0; j < pbrIndex; j++) {
            count += mEmailFlags->Get(j).Size();
        }
        Log("getEmailCount count is: " + count);
        return count;
    }

    public Int32 GetSpareAnrCount() {
        Int32 count = 0;
        Int32 pbrIndex = mAnrFlags->Size();
        For (Int32 j = 0; j < pbrIndex; j++) {
            For (Int32 i = 0; i < mAnrFlags->Get(j).Size(); i++) {
                If (0 == mAnrFlags->Get(j).Get(i))
                    count++;
            }
        }
        Log("getSpareAnrCount count is" + count);
        return count;
    }

    public Int32 GetSpareEmailCount() {
        Int32 count = 0;
        Int32 pbrIndex = mEmailFlags->Size();
        For (Int32 j = 0; j < pbrIndex; j++) {
            For (Int32 i = 0; i < mEmailFlags->Get(j).Size(); i++) {
                If (0 == mEmailFlags->Get(j).Get(i))
                    count++;
            }
        }
        Log("getSpareEmailCount count is: " + count);
        return count;
    }

    public Int32 GetUsimAdnCount() {
        If ((mPhoneBookRecords != NULL) && (!mPhoneBookRecords->IsEmpty())) {
            Log("getUsimAdnCount count is" + mPhoneBookRecords->Size());
            return mPhoneBookRecords->Size();
        } else {
            return 0;
        }
    }

    public Int32 GetEmptyEmailNum_Pbrindex(Int32 pbrindex) {
        Int32 count = 0;
        Int32 size = 0;

        If (!mEmailPresentInIap) {
            //for Type1 Email, the number is always equal to ADN
            //Log("getEmptyEmailNum_Pbrindex pbrIndex:" + pbrindex + " default to 1");
            return 1;
        }

        If (mEmailFlags->ContainsKey(pbrindex)) {
            size = mEmailFlags->Get(pbrindex).Size();
            For (Int32 i = 0; i < size; i++) {
                If (0 == mEmailFlags->Get(pbrindex).Get(i)) count++;
            }
        }
        //Log("getEmptyEmailNum_Pbrindex pbrIndex is: " + pbrindex + " size is: "
        //        + size + ", count is " + count);
        return count;
    }

    public Int32 GetEmptyAnrNum_Pbrindex(Int32 pbrindex) {
        Int32 count = 0;
        Int32 size = 0;

        If (!mAnrPresentInIap) {
            //for Type1 Anr, the number is always equals to ADN
            //Log("getEmptyAnrNum_Pbrindex pbrIndex:" + pbrindex + " default to 1");
            return 1;
        }

        If (mAnrFlags->ContainsKey(pbrindex)) {
            size = mAnrFlags->Get(pbrindex).Size();
            For (Int32 i = 0; i < size; i++) {
                If (0 == mAnrFlags->Get(pbrindex).Get(i)) count++;
            }
        }
        //Log("getEmptyAnrNum_Pbrindex pbrIndex is: " + pbrindex + " size is: "
        //        + size + ", count is " + count);
        return count;
    }

    public Int32 GetEmailFilesCountEachAdn() {
        Map <Integer,Integer> fileIds;
        If (mPbrFile == NULL) {
            Rlog->E(LOG_TAG, "mPbrFile is NULL, exiting from getEmailFilesCountEachAdn");
            return 0;
        } else {
            fileIds = mPbrFile.mFileIds->Get(0);
        }
        If (fileIds == NULL) return 0;

        If (fileIds->ContainsKey(USIM_EFEMAIL_TAG)) {
            If (!mEmailPresentInIap) {
                return mPbrFile.mEmailFileIds->Get(0).Size();
            } else {
                return 1;
            }

        } else {
            return 0;
        }
    }

    public Int32 GetAnrFilesCountEachAdn() {
        Map <Integer,Integer> fileIds;
        If (mPbrFile == NULL) {
            Rlog->E(LOG_TAG, "mPbrFile is NULL, exiting from getAnrFilesCountEachAdn");
            return 0;
        } else {
            fileIds = mPbrFile.mFileIds->Get(0);
        }
        If (fileIds == NULL) return 0;

        If (fileIds->ContainsKey(USIM_EFANR_TAG)) {
            If (!mAnrPresentInIap) {
                return mPbrFile.mAnrFileIds->Get(0).Size();
            } else {
                return 1;
            }

        } else {
            return 0;
        }
    }
}
