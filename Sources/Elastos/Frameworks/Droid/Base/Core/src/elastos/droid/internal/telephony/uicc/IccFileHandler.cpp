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

package com.android.internal.telephony.uicc;

using Elastos::Droid::Os::I*;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;

using Elastos::Utility::IArrayList;

/**
 * {@hide}
 */
public abstract class IccFileHandler extends Handler implements IccConstants {

    //from TS 11.11 9.1 or elsewhere
    static protected final Int32 COMMAND_READ_BINARY = 0xb0;
    static protected final Int32 COMMAND_UPDATE_BINARY = 0xd6;
    static protected final Int32 COMMAND_READ_RECORD = 0xb2;
    static protected final Int32 COMMAND_UPDATE_RECORD = 0xdc;
    static protected final Int32 COMMAND_SEEK = 0xa2;
    static protected final Int32 COMMAND_GET_RESPONSE = 0xc0;

    // from TS 11.11 9.2.5
    static protected final Int32 READ_RECORD_MODE_ABSOLUTE = 4;

    //***** types of files  TS 11.11 9.3
    static protected final Int32 EF_TYPE_TRANSPARENT = 0;
    static protected final Int32 EF_TYPE_LINEAR_FIXED = 1;
    static protected final Int32 EF_TYPE_CYCLIC = 3;

    //***** types of files  TS 11.11 9.3
    static protected final Int32 TYPE_RFU = 0;
    static protected final Int32 TYPE_MF  = 1;
    static protected final Int32 TYPE_DF  = 2;
    static protected final Int32 TYPE_EF  = 4;

    // size of GET_RESPONSE for EF's
    static protected final Int32 GET_RESPONSE_EF_SIZE_BYTES = 15;
    static protected final Int32 GET_RESPONSE_EF_IMG_SIZE_BYTES = 10;

    // Byte order received in response to COMMAND_GET_RESPONSE
    // Refer TS 51.011 Section 9.2.1
    static protected final Int32 RESPONSE_DATA_RFU_1 = 0;
    static protected final Int32 RESPONSE_DATA_RFU_2 = 1;

    static protected final Int32 RESPONSE_DATA_FILE_SIZE_1 = 2;
    static protected final Int32 RESPONSE_DATA_FILE_SIZE_2 = 3;

    static protected final Int32 RESPONSE_DATA_FILE_ID_1 = 4;
    static protected final Int32 RESPONSE_DATA_FILE_ID_2 = 5;
    static protected final Int32 RESPONSE_DATA_FILE_TYPE = 6;
    static protected final Int32 RESPONSE_DATA_RFU_3 = 7;
    static protected final Int32 RESPONSE_DATA_ACCESS_CONDITION_1 = 8;
    static protected final Int32 RESPONSE_DATA_ACCESS_CONDITION_2 = 9;
    static protected final Int32 RESPONSE_DATA_ACCESS_CONDITION_3 = 10;
    static protected final Int32 RESPONSE_DATA_FILE_STATUS = 11;
    static protected final Int32 RESPONSE_DATA_LENGTH = 12;
    static protected final Int32 RESPONSE_DATA_STRUCTURE = 13;
    static protected final Int32 RESPONSE_DATA_RECORD_LENGTH = 14;


    //***** Events

    /** Finished retrieving size of transparent EF; start loading. */
    static protected final Int32 EVENT_GET_BINARY_SIZE_DONE = 4;
    /** Finished loading contents of transparent EF; post result. */
    static protected final Int32 EVENT_READ_BINARY_DONE = 5;
    /** Finished retrieving size of records for linear-fixed EF; now load. */
    static protected final Int32 EVENT_GET_RECORD_SIZE_DONE = 6;
    /** Finished loading single record from a linear-fixed EF; post result. */
    static protected final Int32 EVENT_READ_RECORD_DONE = 7;
    /** Finished retrieving record size; post result. */
    static protected final Int32 EVENT_GET_EF_LINEAR_RECORD_SIZE_DONE = 8;
    /** Finished retrieving image instance record; post result. */
    static protected final Int32 EVENT_READ_IMG_DONE = 9;
    /** Finished retrieving icon data; post result. */
    static protected final Int32 EVENT_READ_ICON_DONE = 10;
    /** Finished retrieving size of record for EFimg now. */
    static protected final Int32 EVENT_GET_RECORD_SIZE_IMG_DONE = 11;

     // member variables
    protected final CommandsInterface mCi;
    protected final UiccCardApplication mParentApp;
    protected final String mAid;

    static class LoadLinearFixedContext {

        Int32 mEfid;
        Int32 mRecordNum, mRecordSize, mCountRecords;
        Boolean mLoadAll;
        String mPath;

        // Variables used to load part records
        Boolean mLoadPart;
        ArrayList<Integer> mRecordNums;
        Int32 mCountLoadrecords;
        Int32 mCount;
        Message mOnLoaded;

        ArrayList<Byte[]> results;

        LoadLinearFixedContext(Int32 efid, Int32 recordNum, Message onLoaded) {
            mEfid = efid;
            mRecordNum = recordNum;
            mOnLoaded = onLoaded;
            mLoadAll = FALSE;
            mLoadPart = FALSE;
            mPath = NULL;
        }

        LoadLinearFixedContext(Int32 efid, Int32 recordNum, String path, Message onLoaded) {
            mEfid = efid;
            mRecordNum = recordNum;
            mOnLoaded = onLoaded;
            mLoadAll = FALSE;
            mLoadPart = FALSE;
            mPath = path;
        }

        LoadLinearFixedContext(Int32 efid, String path, Message onLoaded) {
            mEfid = efid;
            mRecordNum = 1;
            mLoadAll = TRUE;
            mLoadPart = FALSE;
            mOnLoaded = onLoaded;
            mPath = path;
        }

        LoadLinearFixedContext(Int32 efid, Message onLoaded) {
            mEfid = efid;
            mRecordNum = 1;
            mLoadAll = TRUE;
            mLoadPart = FALSE;
            mOnLoaded = onLoaded;
            mPath = NULL;
        }

        LoadLinearFixedContext(Int32 efid, ArrayList<Integer> recordNums, String path,
                Message onLoaded) {
            mEfid = efid;
            mRecordNum = recordNums->Get(0);
            mLoadAll = FALSE;
            mLoadPart = TRUE;
            mRecordNums = new ArrayList<Integer>();
            mRecordNums->AddAll(recordNums);
            mCount = 0;
            mCountLoadrecords = recordNums->Size();
            mOnLoaded = onLoaded;
            mPath = path;
        }

        private void InitLCResults(Int32 size) {
            this.results = new ArrayList<Byte[]>(size);
            Byte[] data = new Byte[this.mRecordSize];
            For (Int32 i = 0; i < this.mRecordSize; i++) {
                data[i] = (Byte) 0xff;
            }
            For (Int32 i = 0; i < size; i++) {
                this.results->Add(data);
            }
        }
    }

    /**
     * Default constructor
     */
    protected IccFileHandler(UiccCardApplication app, String aid, CommandsInterface ci) {
        mParentApp = app;
        mAid = aid;
        mCi = ci;
    }

    CARAPI Dispose() {
    }

    //***** Public Methods

    /**
     * Load a record from a SIM Linear Fixed EF
     *
     * @param fileid EF id
     * @param path Path of the EF on the card
     * @param recordNum 1-Based (not 0-based) record number
     * @param onLoaded
     *
     * ((AsyncResult)(onLoaded.obj)).result is the Byte[]
     *
     */
    CARAPI LoadEFLinearFixed(Int32 fileid, String path, Int32 recordNum, Message onLoaded) {
        Message response
                = ObtainMessage(EVENT_GET_RECORD_SIZE_DONE,
                        new LoadLinearFixedContext(fileid, recordNum, path, onLoaded));

        mCi->IccIOForApp(COMMAND_GET_RESPONSE, fileid, path,
                        0, 0, GET_RESPONSE_EF_SIZE_BYTES, NULL, NULL, mAid, response);
    }

    /**
     * Load a record from a SIM Linear Fixed EF
     *
     * @param fileid EF id
     * @param recordNum 1-Based (not 0-based) record number
     * @param onLoaded
     *
     * ((AsyncResult)(onLoaded.obj)).result is the Byte[]
     *
     */
    CARAPI LoadEFLinearFixed(Int32 fileid, Int32 recordNum, Message onLoaded) {
        LoadEFLinearFixed(fileid, GetEFPath(fileid), recordNum, onLoaded);
    }

    /**
     * Load a image instance record from a SIM Linear Fixed EF-IMG
     *
     * @param recordNum 1-Based (not 0-based) record number
     * @param onLoaded
     *
     * ((AsyncResult)(onLoaded.obj)).result is the Byte[]
     *
     */
    CARAPI LoadEFImgLinearFixed(Int32 recordNum, Message onLoaded) {
        Message response = ObtainMessage(EVENT_GET_RECORD_SIZE_IMG_DONE,
                new LoadLinearFixedContext(IccConstants.EF_IMG, recordNum,
                        onLoaded));

        mCi->IccIOForApp(COMMAND_GET_RESPONSE, IccConstants.EF_IMG,
                    GetEFPath(IccConstants.EF_IMG), recordNum,
                    READ_RECORD_MODE_ABSOLUTE, GET_RESPONSE_EF_IMG_SIZE_BYTES,
                    NULL, NULL, mAid, response);
    }

    /**
     * get record size for a linear fixed EF
     *
     * @param fileid EF id
     * @param path Path of the EF on the card
     * @param OnLoaded ((AsnyncResult)(onLoaded.obj)).result is the recordSize[]
     *        Int32[0] is the record length Int32[1] is the total length of the EF
     *        file Int32[3] is the number of records in the EF file So Int32[0] *
     *        Int32[3] = Int32[1]
     */
    CARAPI GetEFLinearRecordSize(Int32 fileid, String path, Message onLoaded) {
        Message response
                = ObtainMessage(EVENT_GET_EF_LINEAR_RECORD_SIZE_DONE,
                        new LoadLinearFixedContext(fileid, path, onLoaded));
        mCi->IccIOForApp(COMMAND_GET_RESPONSE, fileid, path,
                    0, 0, GET_RESPONSE_EF_SIZE_BYTES, NULL, NULL, mAid, response);
    }

    /**
     * get record size for a linear fixed EF
     *
     * @param fileid EF id
     * @param OnLoaded ((AsnyncResult)(onLoaded.obj)).result is the recordSize[]
     *        Int32[0] is the record length Int32[1] is the total length of the EF
     *        file Int32[3] is the number of records in the EF file So Int32[0] *
     *        Int32[3] = Int32[1]
     */
    CARAPI GetEFLinearRecordSize(Int32 fileid, Message onLoaded) {
        GetEFLinearRecordSize(fileid, GetEFPath(fileid), onLoaded);
    }

    /**
     * Load all records from a SIM Linear Fixed EF
     *
     * @param fileid EF id
     * @param onLoaded
     *
     * ((AsyncResult)(onLoaded.obj)).result is an ArrayList<Byte[]>
     *
     */
    CARAPI LoadEFLinearFixedAll(Int32 fileid, Message onLoaded) {
        LoadEFLinearFixedAll(fileid, GetEFPath(fileid), onLoaded);
    }

    /**
     * Load all records from a SIM Linear Fixed EF
     *
     * @param fileid EF id
     * @param path Path of the EF on the card
     * @param onLoaded
     *
     * ((AsyncResult)(onLoaded.obj)).result is an ArrayList<Byte[]>
     *
     */
    CARAPI LoadEFLinearFixedAll(Int32 fileid, String path, Message onLoaded) {
        Message response = ObtainMessage(EVENT_GET_RECORD_SIZE_DONE,
                        new LoadLinearFixedContext(fileid, path, onLoaded));

        mCi->IccIOForApp(COMMAND_GET_RESPONSE, fileid, path,
                        0, 0, GET_RESPONSE_EF_SIZE_BYTES, NULL, NULL, mAid, response);
    }


    /**
     * Load several records from a SIM Linear Fixed EF
     *
     * @param fileid EF id
     * @param onLoaded
     *
     * ((AsyncResult)(onLoaded.obj)).result is an ArrayList<Byte[]>
     *
     */
    CARAPI LoadEFLinearFixedPart(Int32 fileid,
            ArrayList<Integer> recordNums, Message onLoaded) {
        Message response = ObtainMessage(EVENT_GET_RECORD_SIZE_DONE, new LoadLinearFixedContext(
                fileid, recordNums, GetEFPath(fileid), onLoaded));

        mCi->IccIOForApp(COMMAND_GET_RESPONSE, fileid, GetEFPath(fileid), 0, 0,
                GET_RESPONSE_EF_SIZE_BYTES, NULL, NULL, mAid, response);
    }

    /**
     * Load a SIM Transparent EF
     *
     * @param fileid EF id
     * @param onLoaded
     *
     * ((AsyncResult)(onLoaded.obj)).result is the Byte[]
     *
     */

    CARAPI LoadEFTransparent(Int32 fileid, Message onLoaded) {
        Message response = ObtainMessage(EVENT_GET_BINARY_SIZE_DONE,
                        fileid, 0, onLoaded);

        mCi->IccIOForApp(COMMAND_GET_RESPONSE, fileid, GetEFPath(fileid),
                        0, 0, 0, NULL, NULL, mAid, response);
    }

    /**
     * Load first @size bytes from SIM Transparent EF
     *
     * @param fileid EF id
     * @param size
     * @param onLoaded
     *
     * ((AsyncResult)(onLoaded.obj)).result is the Byte[]
     *
     */
    CARAPI LoadEFTransparent(Int32 fileid, Int32 size, Message onLoaded) {
        Message response = ObtainMessage(EVENT_READ_BINARY_DONE,
                        fileid, 0, onLoaded);

        mCi->IccIOForApp(COMMAND_READ_BINARY, fileid, GetEFPath(fileid),
                        0, 0, size, NULL, NULL, mAid, response);
    }

    /**
     * Load a SIM Transparent EF-IMG. Used right after loadEFImgLinearFixed to
     * retrive STK's icon data.
     *
     * @param fileid EF id
     * @param onLoaded
     *
     * ((AsyncResult)(onLoaded.obj)).result is the Byte[]
     *
     */
    CARAPI LoadEFImgTransparent(Int32 fileid, Int32 highOffset, Int32 lowOffset,
            Int32 length, Message onLoaded) {
        Message response = ObtainMessage(EVENT_READ_ICON_DONE, fileid, 0,
                onLoaded);

        Logd("IccFileHandler: loadEFImgTransparent fileid = " + fileid
                + " filePath = " + GetEFPath(EF_IMG) + " highOffset = " + highOffset
                + " lowOffset = " + lowOffset + " length = " + length);

        /* Per TS 31.102, for displaying of Icon, under
         * DF Telecom and DF Graphics , EF Instance(s) (4FXX,transparent files)
         * are present. The possible image file Identifiers (EF instance) for
         * EF Img ( 4F20, linear fixed file) are : 4F01 ... 4F05.
         * It should be MF_SIM + DF_TELECOM + DF_GRAPHICS, same path as EF IMG
         */
        mCi->IccIOForApp(COMMAND_READ_BINARY, fileid, GetEFPath(EF_IMG),
                highOffset, lowOffset, length, NULL, NULL, mAid, response);
    }

    /**
     * Update a record in a linear fixed EF
     * @param fileid EF id
     * @param path Path of the EF on the card
     * @param recordNum 1-Based (not 0-based) record number
     * @param data must be exactly as Int64 as the record in the EF
     * @param pin2 for CHV2 operations, otherwist must be NULL
     * @param onComplete onComplete.obj will be an AsyncResult
     *                   onComplete.obj.userObj will be a IccIoResult on success
     */
    CARAPI UpdateEFLinearFixed(Int32 fileid, String path, Int32 recordNum, Byte[] data,
            String pin2, Message onComplete) {
        mCi->IccIOForApp(COMMAND_UPDATE_RECORD, fileid, path,
                        recordNum, READ_RECORD_MODE_ABSOLUTE, data.length,
                        IccUtils->BytesToHexString(data), pin2, mAid, onComplete);
    }

    /**
     * Update a record in a linear fixed EF
     * @param fileid EF id
     * @param recordNum 1-Based (not 0-based) record number
     * @param data must be exactly as Int64 as the record in the EF
     * @param pin2 for CHV2 operations, otherwist must be NULL
     * @param onComplete onComplete.obj will be an AsyncResult
     *                   onComplete.obj.userObj will be a IccIoResult on success
     */
    CARAPI UpdateEFLinearFixed(Int32 fileid, Int32 recordNum, Byte[] data,
            String pin2, Message onComplete) {
        mCi->IccIOForApp(COMMAND_UPDATE_RECORD, fileid, GetEFPath(fileid),
                        recordNum, READ_RECORD_MODE_ABSOLUTE, data.length,
                        IccUtils->BytesToHexString(data), pin2, mAid, onComplete);
    }

    /**
     * Update a transparent EF
     * @param fileid EF id
     * @param data must be exactly as Int64 as the EF
     */
    CARAPI UpdateEFTransparent(Int32 fileid, Byte[] data, Message onComplete) {
        mCi->IccIOForApp(COMMAND_UPDATE_BINARY, fileid, GetEFPath(fileid),
                        0, 0, data.length,
                        IccUtils->BytesToHexString(data), NULL, mAid, onComplete);
    }


    //***** Abstract Methods


    //***** Private Methods

    private void SendResult(Message response, Object result, Throwable ex) {
        If (response == NULL) {
            return;
        }

        AsyncResult->ForMessage(response, result, ex);

        response->SendToTarget();
    }

    private Boolean ProcessException(Message response, AsyncResult ar) {
        IccException iccException;
        Boolean flag = FALSE;
        IccIoResult result = (IccIoResult) ar.result;
        If (ar.exception != NULL) {
            SendResult(response, NULL, ar.exception);
            flag = TRUE;
        } else {
            iccException = result->GetException();
            If (iccException != NULL) {
                SendResult(response, NULL, iccException);
                flag = TRUE;
            }
        }
        return flag;
    }

    //***** Overridden from Handler

    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;
        IccIoResult result;
        Message response = NULL;
        String str;
        LoadLinearFixedContext lc;

        Byte data[];
        Int32 size;
        Int32 fileid;
        Int32 recordSize[];
        String path = NULL;

        try {
            Switch (msg.what) {
            case EVENT_GET_EF_LINEAR_RECORD_SIZE_DONE:
                ar = (AsyncResult)msg.obj;
                lc = (LoadLinearFixedContext) ar.userObj;
                result = (IccIoResult) ar.result;
                response = lc.mOnLoaded;

                If (ProcessException(response, (AsyncResult) msg.obj)) {
                    break;
                }

                data = result.payload;

                If (UiccTlvData->IsUiccTlvData(data)) {

                    UiccTlvData tlvData = UiccTlvData->Parse(data);

                    If (tlvData->IsIncomplete()) {
                        throw new IccFileTypeMismatch();
                    }

                    recordSize = new Int32[3];
                    recordSize[0] = tlvData.mRecordSize;
                    recordSize[1] = tlvData.mFileSize;
                    recordSize[2] = tlvData.mNumRecords;

                } else If (TYPE_EF == data[RESPONSE_DATA_FILE_TYPE] &&
                    EF_TYPE_LINEAR_FIXED == data[RESPONSE_DATA_STRUCTURE]) {

                    recordSize = new Int32[3];
                    recordSize[0] = data[RESPONSE_DATA_RECORD_LENGTH] & 0xFF;
                    recordSize[1] = ((data[RESPONSE_DATA_FILE_SIZE_1] & 0xff) << 8)
                            + (data[RESPONSE_DATA_FILE_SIZE_2] & 0xff);
                    recordSize[2] = recordSize[1] / recordSize[0];

                } else {
                    throw new IccFileTypeMismatch();
                }

                SendResult(response, recordSize, NULL);
                break;

             case EVENT_GET_RECORD_SIZE_IMG_DONE:
             case EVENT_GET_RECORD_SIZE_DONE:
                ar = (AsyncResult)msg.obj;
                lc = (LoadLinearFixedContext) ar.userObj;
                result = (IccIoResult) ar.result;
                response = lc.mOnLoaded;

                If (ProcessException(response, (AsyncResult) msg.obj)) {
                    break;
                }

                data = result.payload;
                path = lc.mPath;

                If (UiccTlvData->IsUiccTlvData(data)) {

                    UiccTlvData tlvData = UiccTlvData->Parse(data);

                    If (tlvData->IsIncomplete()) {
                        throw new IccFileTypeMismatch();
                    }

                    lc.mRecordSize = tlvData.mRecordSize;
                    lc.mCountRecords = tlvData.mNumRecords;
                    size = tlvData.mFileSize;

                } else If (TYPE_EF == data[RESPONSE_DATA_FILE_TYPE]) {

                    If (EF_TYPE_LINEAR_FIXED != data[RESPONSE_DATA_STRUCTURE]) {
                        throw new IccFileTypeMismatch();
                    }

                    lc.mRecordSize = data[RESPONSE_DATA_RECORD_LENGTH] & 0xFF;

                    size = ((data[RESPONSE_DATA_FILE_SIZE_1] & 0xff) << 8)
                            + (data[RESPONSE_DATA_FILE_SIZE_2] & 0xff);

                    lc.mCountRecords = size / lc.mRecordSize;
                } else {
                    throw new IccFileTypeMismatch();
                }

                 If (lc.mLoadAll) {
                     lc.results = new ArrayList<Byte[]>(lc.mCountRecords);
                 } else If (lc.mLoadPart) {
                     lc->InitLCResults(lc.mCountRecords);
                 }

                 If (path == NULL) {
                     path = GetEFPath(lc.mEfid);
                 }
                 mCi->IccIOForApp(COMMAND_READ_RECORD, lc.mEfid, path,
                         lc.mRecordNum,
                         READ_RECORD_MODE_ABSOLUTE,
                         lc.mRecordSize, NULL, NULL, mAid,
                         ObtainMessage(EVENT_READ_RECORD_DONE, lc));
                 break;
            case EVENT_GET_BINARY_SIZE_DONE:
                ar = (AsyncResult)msg.obj;
                response = (Message) ar.userObj;
                result = (IccIoResult) ar.result;

                If (ProcessException(response, (AsyncResult) msg.obj)) {
                    break;
                }

                data = result.payload;

                fileid = msg.arg1;

                If (UiccTlvData->IsUiccTlvData(data)) {


                    UiccTlvData tlvData = UiccTlvData->Parse(data);

                    If (tlvData.mFileSize < 0) {
                        throw new IccFileTypeMismatch();
                    }

                    size = tlvData.mFileSize;


                } else If (TYPE_EF == data[RESPONSE_DATA_FILE_TYPE]) {

                    If (EF_TYPE_TRANSPARENT != data[RESPONSE_DATA_STRUCTURE]) {
                        throw new IccFileTypeMismatch();
                    }

                    size = ((data[RESPONSE_DATA_FILE_SIZE_1] & 0xff) << 8)
                            + (data[RESPONSE_DATA_FILE_SIZE_2] & 0xff);
                } else {
                    throw new IccFileTypeMismatch();
                }

                mCi->IccIOForApp(COMMAND_READ_BINARY, fileid, GetEFPath(fileid),
                                0, 0, size, NULL, NULL, mAid,
                                ObtainMessage(EVENT_READ_BINARY_DONE,
                                              fileid, 0, response));
            break;

            case EVENT_READ_IMG_DONE:
            case EVENT_READ_RECORD_DONE:

                ar = (AsyncResult)msg.obj;
                lc = (LoadLinearFixedContext) ar.userObj;
                result = (IccIoResult) ar.result;
                response = lc.mOnLoaded;
                path = lc.mPath;

                If (ProcessException(response, (AsyncResult) msg.obj)) {
                    break;
                }

                If (lc.mLoadAll) {
                    lc.results->Add(result.payload);

                    lc.mRecordNum++;

                    If (lc.mRecordNum > lc.mCountRecords) {
                        SendResult(response, lc.results, NULL);
                    } else {
                        If (path == NULL) {
                            path = GetEFPath(lc.mEfid);
                        }

                        mCi->IccIOForApp(COMMAND_READ_RECORD, lc.mEfid, path,
                                    lc.mRecordNum,
                                    READ_RECORD_MODE_ABSOLUTE,
                                    lc.mRecordSize, NULL, NULL, mAid,
                                    ObtainMessage(EVENT_READ_RECORD_DONE, lc));
                    }
                } else If (lc.mLoadPart) {
                    lc.results->Set(lc.mRecordNum - 1, result.payload);
                    lc.mCount++;
                    If (lc.mCount < lc.mCountLoadrecords) {
                        lc.mRecordNum = lc.mRecordNums->Get(lc.mCount);
                        If (lc.mRecordNum <= lc.mCountRecords) {
                            If (path == NULL) {
                                path = GetEFPath(lc.mEfid);
                            }
                            mCi->IccIOForApp(COMMAND_READ_RECORD, lc.mEfid, path, lc.mRecordNum,
                                    READ_RECORD_MODE_ABSOLUTE, lc.mRecordSize, NULL, NULL, mAid,
                                    ObtainMessage(EVENT_READ_RECORD_DONE, lc));
                        } else {
                            SendResult(response, lc.results, NULL);
                        }
                    } else {
                        SendResult(response, lc.results, NULL);
                    }
                }
            else{
                SendResult(response, result.payload, NULL);
                }

            break;

            case EVENT_READ_BINARY_DONE:
            case EVENT_READ_ICON_DONE:
                ar = (AsyncResult)msg.obj;
                response = (Message) ar.userObj;
                result = (IccIoResult) ar.result;

                If (ProcessException(response, (AsyncResult) msg.obj)) {
                    break;
                }

                SendResult(response, result.payload, NULL);
            break;

        }} Catch (Exception exc) {
            If (response != NULL) {
                SendResult(response, NULL, exc);
            } else {
                Loge("uncaught exception" + exc);
            }
        }
    }

    /**
     * Returns the root path of the EF file.
     * i.e returns MasterFile + DFfile as a string.
     * Ex: For EF_ADN on a SIM, it will return "3F007F10"
     * This function handles only EFids that are common to
     * RUIM, SIM, USIM and other types of Icc cards.
     *
     * @param efid of path to retrieve
     * @return root path of the file.
     */
    protected String GetCommonIccEFPath(Int32 efid) {
        Switch(efid) {
        case EF_ADN:
        case EF_FDN:
        case EF_MSISDN:
        case EF_SDN:
        case EF_EXT1:
        case EF_EXT2:
        case EF_EXT3:
        case EF_PSI:
            return MF_SIM + DF_TELECOM;

        case EF_ICCID:
        case EF_PL:
            return MF_SIM;
        case EF_PBR:
            // we only support global phonebook.
            return MF_SIM + DF_TELECOM + DF_PHONEBOOK;
        case EF_IMG:
            return MF_SIM + DF_TELECOM + DF_GRAPHICS;
        }
        return NULL;
    }

    protected abstract String GetEFPath(Int32 efid);
    protected abstract void Logd(String s);

    protected abstract void Loge(String s);

}
