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

using Elastos::Utility::IArrayList;

using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::Uicc::IIccConstants;

public class AdnRecordLoader extends Handler {
    static const String LOG_TAG = "AdnRecordLoader";
    static const Boolean VDBG = FALSE;

    //***** Instance Variables

    private IccFileHandler mFh;
    Int32 mEf;
    Int32 mExtensionEF;
    Int32 mPendingExtLoads;
    Message mUserResponse;
    String mPin2;

    // For "load one"
    Int32 mRecordNumber;

    // for "load all"
    ArrayList<AdnRecord> mAdns; // only valid after EVENT_ADN_LOAD_ALL_DONE

    // Either an AdnRecord or a reference to adns depending
    // if this is a load one or load all operation
    Object mResult;

    //***** Event Constants

    static const Int32 EVENT_ADN_LOAD_DONE = 1;
    static const Int32 EVENT_EXT_RECORD_LOAD_DONE = 2;
    static const Int32 EVENT_ADN_LOAD_ALL_DONE = 3;
    static const Int32 EVENT_EF_LINEAR_RECORD_SIZE_DONE = 4;
    static const Int32 EVENT_UPDATE_RECORD_DONE = 5;

    //***** Constructor

    AdnRecordLoader(IccFileHandler fh) {
        // The telephony unit-test cases may create AdnRecords
        // in secondary threads
        Super(Looper->GetMainLooper());
        mFh = fh;
    }

    private String GetEFPath(Int32 efid) {
        If (efid == IccConstants.EF_ADN) {
            return IccConstants.MF_SIM + IccConstants.DF_TELECOM;
        }

        return NULL;
    }

    /**
     * Resulting AdnRecord is placed in response.obj.result
     * or response.obj.exception is set
     */
    CARAPI
    LoadFromEF(Int32 ef, Int32 extensionEF, Int32 recordNumber,
                Message response) {
        mEf = ef;
        mExtensionEF = extensionEF;
        mRecordNumber = recordNumber;
        mUserResponse = response;

        If (ef == IccConstants.EF_ADN) {
            mFh->LoadEFLinearFixed(
                        ef, GetEFPath(ef), recordNumber,
                        ObtainMessage(EVENT_ADN_LOAD_DONE));
        } else {
            mFh->LoadEFLinearFixed(
                    ef, recordNumber,
                    ObtainMessage(EVENT_ADN_LOAD_DONE));
        }

    }


    /**
     * Resulting ArrayList&lt;adnRecord> is placed in response.obj.result
     * or response.obj.exception is set
     */
    CARAPI
    LoadAllFromEF(Int32 ef, Int32 extensionEF,
                Message response) {
        mEf = ef;
        mExtensionEF = extensionEF;
        mUserResponse = response;

        /* If we are loading from EF_ADN, specifically
         * specify the path as well, since, on some cards,
         * the fileid is not unique.
         */
        If (ef == IccConstants.EF_ADN) {

            mFh->LoadEFLinearFixedAll(
                    ef, GetEFPath(ef),
                    ObtainMessage(EVENT_ADN_LOAD_ALL_DONE));
        } else {
            mFh->LoadEFLinearFixedAll(
                        ef,
                        ObtainMessage(EVENT_ADN_LOAD_ALL_DONE));
        }
    }

    /**
     * Write adn to a EF SIM record
     * It will get the record size of EF record and compose hex adn array
     * then write the hex array to EF record
     *
     * @param adn is set with alphaTag and phone number
     * @param ef EF fileid
     * @param extensionEF extension EF fileid
     * @param recordNumber 1-based record index
     * @param pin2 for CHV2 operations, must be NULL if pin2 is not needed
     * @param response will be sent to its handler when completed
     */
    CARAPI
    UpdateEF(AdnRecord adn, Int32 ef, Int32 extensionEF, Int32 recordNumber,
            String pin2, Message response) {
        mEf = ef;
        mExtensionEF = extensionEF;
        mRecordNumber = recordNumber;
        mUserResponse = response;
        mPin2 = pin2;

        If (ef == IccConstants.EF_ADN) {
            mFh->GetEFLinearRecordSize( ef, GetEFPath(ef),
                ObtainMessage(EVENT_EF_LINEAR_RECORD_SIZE_DONE, adn));
        } else {
            mFh->GetEFLinearRecordSize( ef,
                    ObtainMessage(EVENT_EF_LINEAR_RECORD_SIZE_DONE, adn));
        }
    }

    //***** Overridden from Handler

    //@Override
    CARAPI
    HandleMessage(Message msg) {
        AsyncResult ar;
        Byte data[];
        AdnRecord adn;

        try {
            Switch (msg.what) {
                case EVENT_EF_LINEAR_RECORD_SIZE_DONE:
                    ar = (AsyncResult)(msg.obj);
                    adn = (AdnRecord)(ar.userObj);

                    If (ar.exception != NULL) {
                        throw new RuntimeException("get EF record size failed",
                                ar.exception);
                    }

                    Int32[] recordSize = (Int32[])ar.result;
                    // recordSize is Int32[3] array
                    // Int32[0]  is the record length
                    // Int32[1]  is the total length of the EF file
                    // Int32[2]  is the number of records in the EF file
                    // So Int32[0] * Int32[2] = Int32[1]
                   If (recordSize.length != 3 || mRecordNumber > recordSize[2]) {
                        throw new RuntimeException("get wrong EF record size format",
                                ar.exception);
                    }

                    data = adn->BuildAdnString(recordSize[0]);

                    If(data == NULL) {
                        throw new RuntimeException("wrong ADN format",
                                ar.exception);
                    }

                    If (mEf == IccConstants.EF_ADN) {
                        mFh->UpdateEFLinearFixed(mEf, GetEFPath(mEf), mRecordNumber,
                                data, mPin2, ObtainMessage(EVENT_UPDATE_RECORD_DONE));
                    } else {
                        mFh->UpdateEFLinearFixed(mEf, mRecordNumber,
                                data, mPin2, ObtainMessage(EVENT_UPDATE_RECORD_DONE));
                    }

                    mPendingExtLoads = 1;

                    break;
                case EVENT_UPDATE_RECORD_DONE:
                    ar = (AsyncResult)(msg.obj);
                    If (ar.exception != NULL) {
                        throw new RuntimeException("update EF adn record failed",
                                ar.exception);
                    }
                    mPendingExtLoads = 0;
                    mResult = NULL;
                    break;
                case EVENT_ADN_LOAD_DONE:
                    ar = (AsyncResult)(msg.obj);
                    data = (Byte[])(ar.result);

                    If (ar.exception != NULL) {
                        throw new RuntimeException("load failed", ar.exception);
                    }

                    If (VDBG) {
                        Rlog->D(LOG_TAG,"ADN EF: 0x"
                            + Integer->ToHexString(mEf)
                            + ":" + mRecordNumber
                            + "\n" + IccUtils->BytesToHexString(data));
                    }

                    adn = new AdnRecord(mEf, mRecordNumber, data);
                    mResult = adn;

                    If (adn->HasExtendedRecord()) {
                        // If we have a valid value in the ext record field,
                        // we're not done yet: we need to read the corresponding
                        // ext record and append it

                        mPendingExtLoads = 1;

                        mFh->LoadEFLinearFixed(
                            mExtensionEF, adn.mExtRecord,
                            ObtainMessage(EVENT_EXT_RECORD_LOAD_DONE, adn));
                    }
                break;

                case EVENT_EXT_RECORD_LOAD_DONE:
                    ar = (AsyncResult)(msg.obj);
                    data = (Byte[])(ar.result);
                    adn = (AdnRecord)(ar.userObj);

                    If (ar.exception != NULL) {
                        throw new RuntimeException("load failed", ar.exception);
                    }

                    Rlog->D(LOG_TAG,"ADN extension EF: 0x"
                        + Integer->ToHexString(mExtensionEF)
                        + ":" + adn.mExtRecord
                        + "\n" + IccUtils->BytesToHexString(data));

                    adn->AppendExtRecord(data);

                    mPendingExtLoads--;
                    // result should have been set in
                    // EVENT_ADN_LOAD_DONE or EVENT_ADN_LOAD_ALL_DONE
                break;

                case EVENT_ADN_LOAD_ALL_DONE:
                    ar = (AsyncResult)(msg.obj);
                    ArrayList<Byte[]> datas = (ArrayList<Byte[]>)(ar.result);

                    If (ar.exception != NULL) {
                        throw new RuntimeException("load failed", ar.exception);
                    }

                    mAdns = new ArrayList<AdnRecord>(datas->Size());
                    mResult = mAdns;
                    mPendingExtLoads = 0;

                    For(Int32 i = 0, s = datas->Size() ; i < s ; i++) {
                        adn = new AdnRecord(mEf, 1 + i, datas->Get(i));
                        mAdns->Add(adn);

                        If (adn->HasExtendedRecord()) {
                            // If we have a valid value in the ext record field,
                            // we're not done yet: we need to read the corresponding
                            // ext record and append it

                            mPendingExtLoads++;

                            mFh->LoadEFLinearFixed(
                                mExtensionEF, adn.mExtRecord,
                                ObtainMessage(EVENT_EXT_RECORD_LOAD_DONE, adn));
                        }
                    }
                break;
            }
        } Catch (RuntimeException exc) {
            If (mUserResponse != NULL) {
                AsyncResult->ForMessage(mUserResponse)
                                .exception = exc;
                mUserResponse->SendToTarget();
                // Loading is all or nothing--either every load succeeds
                // or we fail the whole thing.
                mUserResponse = NULL;
            }
            return;
        }

        If (mUserResponse != NULL && mPendingExtLoads == 0) {
            AsyncResult->ForMessage(mUserResponse).result
                = mResult;

            mUserResponse->SendToTarget();
            mUserResponse = NULL;
        }
    }


}
