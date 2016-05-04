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

package com.android.internal.telephony.uicc;

using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseArray;

using Elastos::Droid::Internal::Telephony::Gsm::IUsimPhoneBookManager;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;

/**
 * {@hide}
 */
public class AdnRecordCache extends Handler implements IccConstants {
    //***** Instance Variables

    private IccFileHandler mFh;
    private UsimPhoneBookManager mUsimPhoneBookManager;

    private Int32 mAdncountofIcc = 0;

    // Indexed by EF ID
    SparseArray<ArrayList<AdnRecord>> mAdnLikeFiles
        = new SparseArray<ArrayList<AdnRecord>>();

    // People waiting for ADN-like files to be loaded
    SparseArray<ArrayList<Message>> mAdnLikeWaiters
        = new SparseArray<ArrayList<Message>>();

    // People waiting for adn record to be updated
    SparseArray<Message> mUserWriteResponse = new SparseArray<Message>();

    //***** Event Constants

    static const Int32 EVENT_LOAD_ALL_ADN_LIKE_DONE = 1;
    static const Int32 EVENT_UPDATE_ADN_DONE = 2;

    // *****USIM TAG Constants
    private static const Int32 USIM_EFANR_TAG   = 0xC4;
    private static const Int32 USIM_EFEMAIL_TAG = 0xCA;
    //***** Constructor



    AdnRecordCache(IccFileHandler fh) {
        mFh = fh;
        mUsimPhoneBookManager = new UsimPhoneBookManager(mFh, this);
    }

    //***** Called from SIMRecords

    /**
     * Called from SIMRecords.onRadioNotAvailable and SIMRecords.handleSimRefresh.
     */
    CARAPI Reset() {
        mAdnLikeFiles->Clear();
        mUsimPhoneBookManager->Reset();

        ClearWaiters();
        ClearUserWriters();

    }

    private void ClearWaiters() {
        Int32 size = mAdnLikeWaiters->Size();
        For (Int32 i = 0; i < size; i++) {
            ArrayList<Message> waiters = mAdnLikeWaiters->ValueAt(i);
            AsyncResult ar = new AsyncResult(NULL, NULL, new RuntimeException("AdnCache reset"));
            NotifyWaiters(waiters, ar);
        }
        mAdnLikeWaiters->Clear();
    }

    private void ClearUserWriters() {
        Int32 size = mUserWriteResponse->Size();
        For (Int32 i = 0; i < size; i++) {
            SendErrorResponse(mUserWriteResponse->ValueAt(i), "AdnCace reset");
        }
        mUserWriteResponse->Clear();
    }

    /**
     * @return List of AdnRecords for efid if we've already loaded them this
     * radio session, or NULL if we haven't
     */
    public ArrayList<AdnRecord>
    GetRecordsIfLoaded(Int32 efid) {
        return mAdnLikeFiles->Get(efid);
    }

    /**
     * Returns extension ef associated with ADN-like EF or -1 if
     * we don't know.
     *
     * See 3GPP TS 51.011 for this mapping
     */
    public Int32 ExtensionEfForEf(Int32 efid) {
        Switch (efid) {
            case EF_MBDN: return EF_EXT6;
            case EF_ADN: return EF_EXT1;
            case EF_SDN: return EF_EXT3;
            case EF_FDN: return EF_EXT2;
            case EF_MSISDN: return EF_EXT1;
            case EF_PBR: return 0; // The EF PBR doesn't have an extension record
            default: return -1;
        }
    }

    private void SendErrorResponse(Message response, String errString) {
        If (response != NULL) {
            Exception e = new RuntimeException(errString);
            AsyncResult->ForMessage(response).exception = e;
            response->SendToTarget();
        }
    }

    /**
     * Update an ADN-like record in EF by record index
     *
     * @param efid must be one among EF_ADN, EF_FDN, and EF_SDN
     * @param adn is the new adn to be stored
     * @param recordIndex is the 1-based adn record index
     * @param pin2 is required to update EF_FDN, otherwise must be NULL
     * @param response message to be posted when done
     *        response.exception hold the exception in error
     */
    CARAPI UpdateAdnByIndex(Int32 efid, AdnRecord adn, Int32 recordIndex, String pin2,
            Message response) {

        Int32 extensionEF = ExtensionEfForEf(efid);
        If (extensionEF < 0) {
            SendErrorResponse(response, "EF is not known ADN-like EF:" + efid);
            return;
        }

        Message pendingResponse = mUserWriteResponse->Get(efid);
        If (pendingResponse != NULL) {
            SendErrorResponse(response, "Have pending update for EF:" + efid);
            return;
        }

        mUserWriteResponse->Put(efid, response);

        new AdnRecordLoader(mFh).UpdateEF(adn, efid, extensionEF,
                recordIndex, pin2,
                ObtainMessage(EVENT_UPDATE_ADN_DONE, efid, recordIndex, adn));
    }

    /**
     * Replace oldAdn with newAdn in ADN-like record in EF
     *
     * The ADN-like records must be read through RequestLoadAllAdnLike() before
     *
     * @param efid must be one of EF_ADN, EF_FDN, and EF_SDN
     * @param oldAdn is the adn to be replaced
     *        If oldAdn->IsEmpty() is ture, it insert the newAdn
     * @param newAdn is the adn to be stored
     *        If newAdn->IsEmpty() is TRUE, it delete the oldAdn
     * @param pin2 is required to update EF_FDN, otherwise must be NULL
     * @param response message to be posted when done
     *        response.exception hold the exception in error
     */
    CARAPI UpdateAdnBySearch(Int32 efid, AdnRecord oldAdn, AdnRecord newAdn,
            String pin2, Message response) {

        Int32 extensionEF;
        extensionEF = ExtensionEfForEf(efid);

        If (extensionEF < 0) {
            SendErrorResponse(response, "EF is not known ADN-like EF:" + efid);
            return;
        }

        ArrayList<AdnRecord> oldAdnList = NULL;
        try {
            If (efid == EF_PBR) {
                oldAdnList = mUsimPhoneBookManager->LoadEfFilesFromUsim();
            } else {
                oldAdnList = GetRecordsIfLoaded(efid);
            }
        } Catch (NullPointerException e) {
            // NullPointerException will be thrown occasionally when we call this method just
            // during phone changed to airplane mode.
            // Some Object used in this method will be reset, so we add protect code here to avoid
            // phone force close.
            oldAdnList = NULL;
        }

        If (oldAdnList == NULL) {
            SendErrorResponse(response, "Adn list not exist for EF:" + efid);
            return;
        }

        Int32 index = -1;
        Int32 count = 1;
        Int32 prePbrIndex = -2;
        Int32 anrNum = 0;
        Int32 emailNum = 0;
        For (Iterator<AdnRecord> it = oldAdnList->Iterator(); it->HasNext();) {
            AdnRecord nextAdnRecord = it->Next();
            Boolean isEmailOrAnrIsFull = FALSE;
            If (efid == EF_PBR) {
                // There may more than one PBR files in the USIM card, if the current PBR file can
                // not save the new AdnRecord which contain anr or email, try save it into next PBR
                // file.
                Int32 pbrIndex = mUsimPhoneBookManager->GetPbrIndexBy(count - 1);
                If (pbrIndex != prePbrIndex) {
                    // For a specific pbrIndex, the anrNum and emailNum is fixed.
                    anrNum = mUsimPhoneBookManager->GetEmptyAnrNum_Pbrindex(pbrIndex);
                    emailNum = mUsimPhoneBookManager->GetEmptyEmailNum_Pbrindex(pbrIndex);
                    prePbrIndex = pbrIndex;
                    Logger::D("AdnRecordCache", "updateAdnBySearch, pbrIndex: " + pbrIndex +
                            " anrNum:" + anrNum + " emailNum:" + emailNum);
                }
                If ((anrNum == 0 &&
                        (oldAdn->GetAdditionalNumbers() == NULL &&
                         newAdn->GetAdditionalNumbers() != NULL)) ||
                    (emailNum == 0 &&
                        (oldAdn->GetEmails() == NULL &&
                         newAdn->GetEmails() != NULL))) {
                    isEmailOrAnrIsFull = TRUE;
                }
            }

            If (!isEmailOrAnrIsFull && oldAdn->IsEqual(nextAdnRecord)) {
                index = count;
                break;
            }
            count++;
        }

        Logger::D("AdnRecordCache", "updateAdnBySearch, update oldADN:" + oldAdn->ToString() +
                ", newAdn:" + newAdn->ToString() + ",index :" + index);

        If (index == -1) {
            SendErrorResponse(response, "Adn record don't exist for " + oldAdn);
            return;
        }

        If (efid == EF_PBR) {
            AdnRecord foundAdn = oldAdnList->Get(index-1);
            newAdn.mEfid = foundAdn.mEfid;
            newAdn.mExtRecord = foundAdn.mExtRecord;
            newAdn.mRecordNumber = foundAdn.mRecordNumber;
            // make sure the sequence is same with foundAdn
            oldAdn->SetAdditionalNumbers(foundAdn->GetAdditionalNumbers());
            oldAdn->SetEmails(foundAdn->GetEmails());
            newAdn->UpdateAnrEmailArray(oldAdn,
                    mUsimPhoneBookManager->GetEmailFilesCountEachAdn(),
                    mUsimPhoneBookManager->GetAnrFilesCountEachAdn());
        }

        Message pendingResponse = mUserWriteResponse->Get(efid);

        If (pendingResponse != NULL) {
            SendErrorResponse(response, "Have pending update for EF:" + efid);
            return;
        }

        If (efid == EF_PBR) {
            UpdateEmailAndAnr(efid, oldAdn, newAdn, index, pin2, response);
        } else {
            mUserWriteResponse->Put(efid, response);
            new AdnRecordLoader(mFh).UpdateEF(newAdn, efid, extensionEF,
                    index, pin2,
                    ObtainMessage(EVENT_UPDATE_ADN_DONE, efid, index, newAdn));
        }
    }


    /**
     * Responds with Exception (in response) if efid is not a known ADN-like
     * record
     */
    CARAPI
    RequestLoadAllAdnLike (Int32 efid, Int32 extensionEf, Message response) {
        ArrayList<Message> waiters;
        ArrayList<AdnRecord> result;

        If (efid == EF_PBR) {
            result = mUsimPhoneBookManager->LoadEfFilesFromUsim();
        } else {
            result = GetRecordsIfLoaded(efid);
        }

        // Have we already loaded this efid?
        If (result != NULL) {
            If (response != NULL) {
                AsyncResult->ForMessage(response).result = result;
                response->SendToTarget();
            }

            return;
        }

        // Have we already *started* loading this efid?

        waiters = mAdnLikeWaiters->Get(efid);

        If (waiters != NULL) {
            // There's a pending request for this EF already
            // just add ourselves to it

            waiters->Add(response);
            return;
        }

        // Start loading efid

        waiters = new ArrayList<Message>();
        waiters->Add(response);

        mAdnLikeWaiters->Put(efid, waiters);


        If (extensionEf < 0) {
            // respond with error if not known ADN-like record

            If (response != NULL) {
                AsyncResult->ForMessage(response).exception
                    = new RuntimeException("EF is not known ADN-like EF:" + efid);
                response->SendToTarget();
            }

            return;
        }

        new AdnRecordLoader(mFh).LoadAllFromEF(efid, extensionEf,
            ObtainMessage(EVENT_LOAD_ALL_ADN_LIKE_DONE, efid, 0));
    }

    //***** Private methods

    private void
    NotifyWaiters(ArrayList<Message> waiters, AsyncResult ar) {

        If (waiters == NULL) {
            return;
        }

        For (Int32 i = 0, s = waiters->Size() ; i < s ; i++) {
            Message waiter = waiters->Get(i);

            AsyncResult->ForMessage(waiter, ar.result, ar.exception);
            waiter->SendToTarget();
        }
    }

    //***** Overridden from Handler

    //@Override
    CARAPI
    HandleMessage(Message msg) {
        AsyncResult ar;
        Int32 efid;

        Switch(msg.what) {
            case EVENT_LOAD_ALL_ADN_LIKE_DONE:
                /* arg1 is efid, obj.result is ArrayList<AdnRecord>*/
                ar = (AsyncResult) msg.obj;
                efid = msg.arg1;
                ArrayList<Message> waiters;

                waiters = mAdnLikeWaiters->Get(efid);
                mAdnLikeWaiters->Delete(efid);

                If (ar.exception == NULL) {
                    mAdnLikeFiles->Put(efid, (ArrayList<AdnRecord>) ar.result);
                }
                NotifyWaiters(waiters, ar);
                If (mAdnLikeFiles->Get(EF_ADN) != NULL) {
                    SetAdnCount(mAdnLikeFiles->Get(EF_ADN).Size());
                }
                break;
            case EVENT_UPDATE_ADN_DONE:
                ar = (AsyncResult)msg.obj;
                efid = msg.arg1;
                Int32 index = msg.arg2;
                AdnRecord adn = (AdnRecord) (ar.userObj);

                If (ar.exception == NULL) {
                    If (mAdnLikeFiles->Get(efid) != NULL) {
                        mAdnLikeFiles->Get(efid).Set(index - 1, adn);
                    }
                    If (efid == EF_PBR) {
                        mUsimPhoneBookManager->LoadEfFilesFromUsim()->Set(index - 1, adn);
                    }
                }

                Message response = mUserWriteResponse->Get(efid);
                mUserWriteResponse->Delete(efid);

                // response may be cleared when simrecord is reset,
                // so we should check if it is NULL.
                If (response != NULL) {
                    AsyncResult->ForMessage(response, NULL, ar.exception);
                    response->SendToTarget();
                }
                break;
        }

    }

    private void UpdateEmailAndAnr(Int32 efid, AdnRecord oldAdn, AdnRecord newAdn, Int32 index,
            String pin2, Message response) {
        Int32 extensionEF;
        extensionEF = ExtensionEfForEf(newAdn.mEfid);
        Boolean success = FALSE;
        success = UpdateUsimRecord(oldAdn, newAdn, index, USIM_EFEMAIL_TAG);

        If (success) {
            success = UpdateUsimRecord(oldAdn, newAdn, index, USIM_EFANR_TAG);
        } else {
            SendErrorResponse(response, "update email failed");
            return;
        }
        If (success) {
            mUserWriteResponse->Put(efid, response);
            new AdnRecordLoader(mFh).UpdateEF(newAdn, newAdn.mEfid, extensionEF,
                    newAdn.mRecordNumber, pin2,
                    ObtainMessage(EVENT_UPDATE_ADN_DONE, efid, index, newAdn));
        } else {
            SendErrorResponse(response, "update anr failed");
            return;
        }
    }

    private Boolean UpdateAnrEmailFile(String oldRecord,
                String newRecord, Int32 index, Int32 tag, Int32 efidIndex) {
        Boolean success = TRUE;
        try {
            Switch (tag) {
                case USIM_EFEMAIL_TAG:
                    success = mUsimPhoneBookManager
                            .UpdateEmailFile(index, oldRecord, newRecord, efidIndex);
                    break;
                case USIM_EFANR_TAG:
                    success = mUsimPhoneBookManager
                            .UpdateAnrFile(index, oldRecord, newRecord, efidIndex);
                    break;
                default:
                    success = FALSE;
            }
        } Catch (RuntimeException e) {
            success = FALSE;
            Logger::E("AdnRecordCache", "update usim record failed", e);
        }

        return success;
    }

    private Boolean UpdateUsimRecord(AdnRecord oldAdn, AdnRecord newAdn, Int32 index, Int32 tag) {
        String[] oldRecords = NULL;
        String[] newRecords = NULL;
        String oldRecord = NULL;
        String newRecord = NULL;
        Boolean success = TRUE;
        // currently we only support one email records
        Switch (tag) {
            case USIM_EFEMAIL_TAG:
                oldRecords = oldAdn->GetEmails();
                newRecords = newAdn->GetEmails();
                break;
            case USIM_EFANR_TAG:
                oldRecords = oldAdn->GetAdditionalNumbers();
                newRecords = newAdn->GetAdditionalNumbers();
                break;
            default:
                return FALSE;
        }

        If (NULL == oldRecords && NULL == newRecords) {
            // UI send empty string, no need to update
            Logger::E("AdnRecordCache", "Both old and new EMAIL/ANR are NULL");
            return TRUE;
        }

        // insert scenario
        If (NULL == oldRecords && NULL != newRecords) {
            For (Int32 i = 0; i < newRecords.length; i++) {
                If (!TextUtils->IsEmpty(newRecords[i])) {
                    success &= UpdateAnrEmailFile(NULL, newRecords[i], index, tag, i);
                }
            }
        // delete scenario
        } else If (NULL != oldRecords && NULL == newRecords) {
            For (Int32 i = 0; i < oldRecords.length; i++) {
                If (!TextUtils->IsEmpty(oldRecords[i])) {
                    success &= UpdateAnrEmailFile(oldRecords[i], NULL, index, tag, i);
                }
            }
        // update scenario
        } else {
            Int32 maxLen = (oldRecords.length > newRecords.length) ?
                            oldRecords.length : newRecords.length;
            For (Int32 i = 0; i < maxLen; i++) {
                oldRecord = (i >= oldRecords.length) ? NULL : oldRecords[i];
                newRecord = (i >= newRecords.length) ? NULL : newRecords[i];

                If ((TextUtils->IsEmpty(oldRecord) && TextUtils->IsEmpty(newRecord)) ||
                    (oldRecord != NULL && newRecord != NULL && oldRecord->Equals(newRecord))) {
                    continue;
                } else {
                    success &= UpdateAnrEmailFile(oldRecord, newRecord, index, tag, i);
                }
            }
        }

        return success;
    }

    CARAPI UpdateUsimAdnByIndex(Int32 efid, AdnRecord newAdn, Int32 recordIndex, String pin2,
            Message response) {

        Int32 extensionEF;
        extensionEF = ExtensionEfForEf(efid);
        If (extensionEF < 0) {
            SendErrorResponse(response, "EF is not known ADN-like EF:" + efid);
            return;
        }

        ArrayList<AdnRecord> oldAdnList = NULL;
        try {
            If (efid == EF_PBR) {
                oldAdnList = mUsimPhoneBookManager->LoadEfFilesFromUsim();
            } else {
                oldAdnList = GetRecordsIfLoaded(efid);
            }
        } Catch (NullPointerException e) {
            // NullPointerException will be thrown occasionally when we call this method just
            // during phone changed to airplane mode.
            // Some Object used in this method will be reset, so we add protect code here to avoid
            // phone force close.
            oldAdnList = NULL;
        }

        If (oldAdnList == NULL) {
            SendErrorResponse(response, "Adn list not exist for EF:" + efid);
            return;
        }

        Int32 index = recordIndex;

        If (efid == EF_PBR) {
            AdnRecord foundAdn = oldAdnList->Get(index - 1);
            newAdn.mEfid = foundAdn.mEfid;
            newAdn.mExtRecord = foundAdn.mExtRecord;
            newAdn.mRecordNumber = foundAdn.mRecordNumber;
        }

        Message pendingResponse = mUserWriteResponse->Get(efid);

        If (pendingResponse != NULL) {
            SendErrorResponse(response, "Have pending update for EF:" + efid);
            return;
        }

        If (efid == EF_PBR) {
            UpdateEmailAndAnr(efid, oldAdnList->Get(index - 1), newAdn, index, pin2, response);
        } else {
            mUserWriteResponse->Put(efid, response);
            new AdnRecordLoader(mFh).UpdateEF(newAdn, efid, extensionEF, index, pin2,
                    ObtainMessage(EVENT_UPDATE_ADN_DONE, efid, index, newAdn));
        }
    }

    public Int32 GetAnrCount() {
        return mUsimPhoneBookManager->GetAnrCount();
    }

    public Int32 GetEmailCount() {
        return mUsimPhoneBookManager->GetEmailCount();
    }
    public Int32 GetSpareAnrCount() {
        return mUsimPhoneBookManager->GetSpareAnrCount();
    }

    public Int32 GetSpareEmailCount() {
        return mUsimPhoneBookManager->GetSpareEmailCount();
    }

    public Int32 GetAdnCount() {
        return mAdncountofIcc;
    }

    CARAPI SetAdnCount(Int32 count) {
        mAdncountofIcc = count;
    }

    public Int32 GetUsimAdnCount() {
        return mUsimPhoneBookManager->GetUsimAdnCount();
    }
}
