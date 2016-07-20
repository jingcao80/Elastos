
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccFileHandler.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                IccFileHandler::LoadLinearFixedContext
//=====================================================================
IccFileHandler::LoadLinearFixedContext::LoadLinearFixedContext(
    /* [in] */ Int32 efid,
    /* [in] */ Int32 recordNum,
    /* [in] */ IMessage* onLoaded)
{
    // ==================before translated======================
    // mEfid = efid;
    // mRecordNum = recordNum;
    // mOnLoaded = onLoaded;
    // mLoadAll = false;
    // mLoadPart = false;
    // mPath = null;
}

IccFileHandler::LoadLinearFixedContext::LoadLinearFixedContext(
    /* [in] */ Int32 efid,
    /* [in] */ Int32 recordNum,
    /* [in] */ const String& path,
    /* [in] */ IMessage* onLoaded)
{
    // ==================before translated======================
    // mEfid = efid;
    // mRecordNum = recordNum;
    // mOnLoaded = onLoaded;
    // mLoadAll = false;
    // mLoadPart = false;
    // mPath = path;
}

IccFileHandler::LoadLinearFixedContext::LoadLinearFixedContext(
    /* [in] */ Int32 efid,
    /* [in] */ const String& path,
    /* [in] */ IMessage* onLoaded)
{
    // ==================before translated======================
    // mEfid = efid;
    // mRecordNum = 1;
    // mLoadAll = true;
    // mLoadPart = false;
    // mOnLoaded = onLoaded;
    // mPath = path;
}

IccFileHandler::LoadLinearFixedContext::LoadLinearFixedContext(
    /* [in] */ Int32 efid,
    /* [in] */ IMessage* onLoaded)
{
    // ==================before translated======================
    // mEfid = efid;
    // mRecordNum = 1;
    // mLoadAll = true;
    // mLoadPart = false;
    // mOnLoaded = onLoaded;
    // mPath = null;
}

IccFileHandler::LoadLinearFixedContext::LoadLinearFixedContext(
    /* [in] */ Int32 efid,
    /* [in] */ IArrayList/*<Integer*>*/* recordNums,
    /* [in] */ const String& path,
    /* [in] */ IMessage* onLoaded)
{
    // ==================before translated======================
    // mEfid = efid;
    // mRecordNum = recordNums.get(0);
    // mLoadAll = false;
    // mLoadPart = true;
    // mRecordNums = new ArrayList<Integer>();
    // mRecordNums.addAll(recordNums);
    // mCount = 0;
    // mCountLoadrecords = recordNums.size();
    // mOnLoaded = onLoaded;
    // mPath = path;
}

void IccFileHandler::LoadLinearFixedContext::InitLCResults(
    /* [in] */ Int32 size)
{
    // ==================before translated======================
    // this.results = new ArrayList<byte[]>(size);
    // byte[] data = new byte[this.mRecordSize];
    // for (int i = 0; i < this.mRecordSize; i++) {
    //     data[i] = (byte) 0xff;
    // }
    // for (int i = 0; i < size; i++) {
    //     this.results.add(data);
    // }
    assert(0);
}

//=====================================================================
//                            IccFileHandler
//=====================================================================
CAR_INTERFACE_IMPL_2(IccFileHandler, Handler, IIccFileHandler, IIccConstants);

const Int32 IccFileHandler::COMMAND_READ_BINARY;
const Int32 IccFileHandler::COMMAND_UPDATE_BINARY;
const Int32 IccFileHandler::COMMAND_READ_RECORD;
const Int32 IccFileHandler::COMMAND_UPDATE_RECORD;
const Int32 IccFileHandler::COMMAND_SEEK;
const Int32 IccFileHandler::COMMAND_GET_RESPONSE;
const Int32 IccFileHandler::READ_RECORD_MODE_ABSOLUTE;
const Int32 IccFileHandler::EF_TYPE_TRANSPARENT;
const Int32 IccFileHandler::EF_TYPE_LINEAR_FIXED;
const Int32 IccFileHandler::EF_TYPE_CYCLIC;
const Int32 IccFileHandler::TYPE_RFU;
const Int32 IccFileHandler::TYPE_MF;
const Int32 IccFileHandler::TYPE_DF;
const Int32 IccFileHandler::TYPE_EF;
const Int32 IccFileHandler::GET_RESPONSE_EF_SIZE_BYTES;
const Int32 IccFileHandler::GET_RESPONSE_EF_IMG_SIZE_BYTES;
const Int32 IccFileHandler::RESPONSE_DATA_RFU_1;
const Int32 IccFileHandler::RESPONSE_DATA_RFU_2;
const Int32 IccFileHandler::RESPONSE_DATA_FILE_SIZE_1;
const Int32 IccFileHandler::RESPONSE_DATA_FILE_SIZE_2;
const Int32 IccFileHandler::RESPONSE_DATA_FILE_ID_1;
const Int32 IccFileHandler::RESPONSE_DATA_FILE_ID_2;
const Int32 IccFileHandler::RESPONSE_DATA_FILE_TYPE;
const Int32 IccFileHandler::RESPONSE_DATA_RFU_3;
const Int32 IccFileHandler::RESPONSE_DATA_ACCESS_CONDITION_1;
const Int32 IccFileHandler::RESPONSE_DATA_ACCESS_CONDITION_2;
const Int32 IccFileHandler::RESPONSE_DATA_ACCESS_CONDITION_3;
const Int32 IccFileHandler::RESPONSE_DATA_FILE_STATUS;
const Int32 IccFileHandler::RESPONSE_DATA_LENGTH;
const Int32 IccFileHandler::RESPONSE_DATA_STRUCTURE;
const Int32 IccFileHandler::RESPONSE_DATA_RECORD_LENGTH;
const Int32 IccFileHandler::EVENT_GET_BINARY_SIZE_DONE;
const Int32 IccFileHandler::EVENT_READ_BINARY_DONE;
const Int32 IccFileHandler::EVENT_GET_RECORD_SIZE_DONE;
const Int32 IccFileHandler::EVENT_READ_RECORD_DONE;
const Int32 IccFileHandler::EVENT_GET_EF_LINEAR_RECORD_SIZE_DONE;
const Int32 IccFileHandler::EVENT_READ_IMG_DONE;
const Int32 IccFileHandler::EVENT_READ_ICON_DONE;
const Int32 IccFileHandler::EVENT_GET_RECORD_SIZE_IMG_DONE;

ECode IccFileHandler::Dispose()
{
    assert(0);
    return NOERROR;
}

ECode IccFileHandler::LoadEFLinearFixed(
    /* [in] */ Int32 fileid,
    /* [in] */ const String& path,
    /* [in] */ Int32 recordNum,
    /* [in] */ IMessage* onLoaded)
{
    // ==================before translated======================
    // Message response
    //         = obtainMessage(EVENT_GET_RECORD_SIZE_DONE,
    //                 new LoadLinearFixedContext(fileid, recordNum, path, onLoaded));
    //
    // mCi.iccIOForApp(COMMAND_GET_RESPONSE, fileid, path,
    //                 0, 0, GET_RESPONSE_EF_SIZE_BYTES, null, null, mAid, response);
    assert(0);
    return NOERROR;
}

ECode IccFileHandler::LoadEFLinearFixed(
    /* [in] */ Int32 fileid,
    /* [in] */ Int32 recordNum,
    /* [in] */ IMessage* onLoaded)
{
    // ==================before translated======================
    // loadEFLinearFixed(fileid, getEFPath(fileid), recordNum, onLoaded);
    assert(0);
    return NOERROR;
}

ECode IccFileHandler::LoadEFImgLinearFixed(
    /* [in] */ Int32 recordNum,
    /* [in] */ IMessage* onLoaded)
{
    // ==================before translated======================
    // Message response = obtainMessage(EVENT_GET_RECORD_SIZE_IMG_DONE,
    //         new LoadLinearFixedContext(IccConstants.EF_IMG, recordNum,
    //                 onLoaded));
    //
    // mCi.iccIOForApp(COMMAND_GET_RESPONSE, IccConstants.EF_IMG,
    //             getEFPath(IccConstants.EF_IMG), recordNum,
    //             READ_RECORD_MODE_ABSOLUTE, GET_RESPONSE_EF_IMG_SIZE_BYTES,
    //             null, null, mAid, response);
    assert(0);
    return NOERROR;
}

ECode IccFileHandler::GetEFLinearRecordSize(
    /* [in] */ Int32 fileid,
    /* [in] */ const String& path,
    /* [in] */ IMessage* onLoaded)
{
    // ==================before translated======================
    // Message response
    //         = obtainMessage(EVENT_GET_EF_LINEAR_RECORD_SIZE_DONE,
    //                 new LoadLinearFixedContext(fileid, path, onLoaded));
    // mCi.iccIOForApp(COMMAND_GET_RESPONSE, fileid, path,
    //             0, 0, GET_RESPONSE_EF_SIZE_BYTES, null, null, mAid, response);
    assert(0);
    return NOERROR;
}

ECode IccFileHandler::GetEFLinearRecordSize(
    /* [in] */ Int32 fileid,
    /* [in] */ IMessage* onLoaded)
{
    // ==================before translated======================
    // getEFLinearRecordSize(fileid, getEFPath(fileid), onLoaded);
    assert(0);
    return NOERROR;
}

ECode IccFileHandler::LoadEFLinearFixedAll(
    /* [in] */ Int32 fileid,
    /* [in] */ IMessage* onLoaded)
{
    // ==================before translated======================
    // loadEFLinearFixedAll(fileid, getEFPath(fileid), onLoaded);
    assert(0);
    return NOERROR;
}

ECode IccFileHandler::LoadEFLinearFixedAll(
    /* [in] */ Int32 fileid,
    /* [in] */ const String& path,
    /* [in] */ IMessage* onLoaded)
{
    // ==================before translated======================
    // Message response = obtainMessage(EVENT_GET_RECORD_SIZE_DONE,
    //                 new LoadLinearFixedContext(fileid, path, onLoaded));
    //
    // mCi.iccIOForApp(COMMAND_GET_RESPONSE, fileid, path,
    //                 0, 0, GET_RESPONSE_EF_SIZE_BYTES, null, null, mAid, response);
    assert(0);
    return NOERROR;
}

ECode IccFileHandler::LoadEFLinearFixedPart(
    /* [in] */ Int32 fileid,
    /* [in] */ IArrayList/*<Integer*>*/* recordNums,
    /* [in] */ IMessage* onLoaded)
{
    // ==================before translated======================
    // Message response = obtainMessage(EVENT_GET_RECORD_SIZE_DONE, new LoadLinearFixedContext(
    //         fileid, recordNums, getEFPath(fileid), onLoaded));
    //
    // mCi.iccIOForApp(COMMAND_GET_RESPONSE, fileid, getEFPath(fileid), 0, 0,
    //         GET_RESPONSE_EF_SIZE_BYTES, null, null, mAid, response);
    assert(0);
    return NOERROR;
}

ECode IccFileHandler::LoadEFTransparent(
    /* [in] */ Int32 fileid,
    /* [in] */ IMessage* onLoaded)
{
    AutoPtr<IMessage> response;
    ObtainMessage(EVENT_GET_BINARY_SIZE_DONE,
                    fileid, 0, onLoaded, (IMessage**)&response);

    mCi->IccIOForApp(COMMAND_GET_RESPONSE, fileid, GetEFPath(fileid),
                    0, 0, 0, String(NULL), String(NULL), mAid, response);
    return NOERROR;
}

ECode IccFileHandler::LoadEFTransparent(
    /* [in] */ Int32 fileid,
    /* [in] */ Int32 size,
    /* [in] */ IMessage* onLoaded)
{
    Logger::E("IccFileHandler", "TODO LoadEFTransparent 3 no impl");
    // ==================before translated======================
    // Message response = obtainMessage(EVENT_READ_BINARY_DONE,
    //                 fileid, 0, onLoaded);
    //
    // mCi.iccIOForApp(COMMAND_READ_BINARY, fileid, getEFPath(fileid),
    //                 0, 0, size, null, null, mAid, response);
    return NOERROR;
}

ECode IccFileHandler::LoadEFImgTransparent(
    /* [in] */ Int32 fileid,
    /* [in] */ Int32 highOffset,
    /* [in] */ Int32 lowOffset,
    /* [in] */ Int32 length,
    /* [in] */ IMessage* onLoaded)
{
    // ==================before translated======================
    // Message response = obtainMessage(EVENT_READ_ICON_DONE, fileid, 0,
    //         onLoaded);
    //
    // logd("IccFileHandler: loadEFImgTransparent fileid = " + fileid
    //         + " filePath = " + getEFPath(EF_IMG) + " highOffset = " + highOffset
    //         + " lowOffset = " + lowOffset + " length = " + length);
    //
    // /* Per TS 31.102, for displaying of Icon, under
    //  * DF Telecom and DF Graphics , EF instance(s) (4FXX,transparent files)
    //  * are present. The possible image file identifiers (EF instance) for
    //  * EF img ( 4F20, linear fixed file) are : 4F01 ... 4F05.
    //  * It should be MF_SIM + DF_TELECOM + DF_GRAPHICS, same path as EF IMG
    //  */
    // mCi.iccIOForApp(COMMAND_READ_BINARY, fileid, getEFPath(EF_IMG),
    //         highOffset, lowOffset, length, null, null, mAid, response);
    assert(0);
    return NOERROR;
}

ECode IccFileHandler::UpdateEFLinearFixed(
    /* [in] */ Int32 fileid,
    /* [in] */ const String& path,
    /* [in] */ Int32 recordNum,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // mCi.iccIOForApp(COMMAND_UPDATE_RECORD, fileid, path,
    //                 recordNum, READ_RECORD_MODE_ABSOLUTE, data.length,
    //                 IccUtils.bytesToHexString(data), pin2, mAid, onComplete);
    assert(0);
    return NOERROR;
}

ECode IccFileHandler::UpdateEFLinearFixed(
    /* [in] */ Int32 fileid,
    /* [in] */ Int32 recordNum,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // mCi.iccIOForApp(COMMAND_UPDATE_RECORD, fileid, getEFPath(fileid),
    //                 recordNum, READ_RECORD_MODE_ABSOLUTE, data.length,
    //                 IccUtils.bytesToHexString(data), pin2, mAid, onComplete);
    assert(0);
    return NOERROR;
}

ECode IccFileHandler::UpdateEFTransparent(
    /* [in] */ Int32 fileid,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // mCi.iccIOForApp(COMMAND_UPDATE_BINARY, fileid, getEFPath(fileid),
    //                 0, 0, data.length,
    //                 IccUtils.bytesToHexString(data), null, mAid, onComplete);
    assert(0);
    return NOERROR;
}

ECode IccFileHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    Logger::D("IccFileHandler", "[TODO] HandleMessage ====================what=[%d]", what);
    // ==================before translated======================
    // AsyncResult ar;
    // IccIoResult result;
    // Message response = null;
    // String str;
    // LoadLinearFixedContext lc;
    //
    // byte data[];
    // int size;
    // int fileid;
    // int recordSize[];
    // String path = null;
    //
    // try {
    //     switch (msg.what) {
    //     case EVENT_GET_EF_LINEAR_RECORD_SIZE_DONE:
    //         ar = (AsyncResult)msg.obj;
    //         lc = (LoadLinearFixedContext) ar.userObj;
    //         result = (IccIoResult) ar.result;
    //         response = lc.mOnLoaded;
    //
    //         if (processException(response, (AsyncResult) msg.obj)) {
    //             break;
    //         }
    //
    //         data = result.payload;
    //
    //         if (UiccTlvData.isUiccTlvData(data)) {
    //
    //             UiccTlvData tlvData = UiccTlvData.parse(data);
    //
    //             if (tlvData.isIncomplete()) {
    //                 throw new IccFileTypeMismatch();
    //             }
    //
    //             recordSize = new int[3];
    //             recordSize[0] = tlvData.mRecordSize;
    //             recordSize[1] = tlvData.mFileSize;
    //             recordSize[2] = tlvData.mNumRecords;
    //
    //         } else if (TYPE_EF == data[RESPONSE_DATA_FILE_TYPE] &&
    //             EF_TYPE_LINEAR_FIXED == data[RESPONSE_DATA_STRUCTURE]) {
    //
    //             recordSize = new int[3];
    //             recordSize[0] = data[RESPONSE_DATA_RECORD_LENGTH] & 0xFF;
    //             recordSize[1] = ((data[RESPONSE_DATA_FILE_SIZE_1] & 0xff) << 8)
    //                     + (data[RESPONSE_DATA_FILE_SIZE_2] & 0xff);
    //             recordSize[2] = recordSize[1] / recordSize[0];
    //
    //         } else {
    //             throw new IccFileTypeMismatch();
    //         }
    //
    //         sendResult(response, recordSize, null);
    //         break;
    //
    //      case EVENT_GET_RECORD_SIZE_IMG_DONE:
    //      case EVENT_GET_RECORD_SIZE_DONE:
    //         ar = (AsyncResult)msg.obj;
    //         lc = (LoadLinearFixedContext) ar.userObj;
    //         result = (IccIoResult) ar.result;
    //         response = lc.mOnLoaded;
    //
    //         if (processException(response, (AsyncResult) msg.obj)) {
    //             break;
    //         }
    //
    //         data = result.payload;
    //         path = lc.mPath;
    //
    //         if (UiccTlvData.isUiccTlvData(data)) {
    //
    //             UiccTlvData tlvData = UiccTlvData.parse(data);
    //
    //             if (tlvData.isIncomplete()) {
    //                 throw new IccFileTypeMismatch();
    //             }
    //
    //             lc.mRecordSize = tlvData.mRecordSize;
    //             lc.mCountRecords = tlvData.mNumRecords;
    //             size = tlvData.mFileSize;
    //
    //         } else if (TYPE_EF == data[RESPONSE_DATA_FILE_TYPE]) {
    //
    //             if (EF_TYPE_LINEAR_FIXED != data[RESPONSE_DATA_STRUCTURE]) {
    //                 throw new IccFileTypeMismatch();
    //             }
    //
    //             lc.mRecordSize = data[RESPONSE_DATA_RECORD_LENGTH] & 0xFF;
    //
    //             size = ((data[RESPONSE_DATA_FILE_SIZE_1] & 0xff) << 8)
    //                     + (data[RESPONSE_DATA_FILE_SIZE_2] & 0xff);
    //
    //             lc.mCountRecords = size / lc.mRecordSize;
    //         } else {
    //             throw new IccFileTypeMismatch();
    //         }
    //
    //          if (lc.mLoadAll) {
    //              lc.results = new ArrayList<byte[]>(lc.mCountRecords);
    //          } else if (lc.mLoadPart) {
    //              lc.initLCResults(lc.mCountRecords);
    //          }
    //
    //          if (path == null) {
    //              path = getEFPath(lc.mEfid);
    //          }
    //          mCi.iccIOForApp(COMMAND_READ_RECORD, lc.mEfid, path,
    //                  lc.mRecordNum,
    //                  READ_RECORD_MODE_ABSOLUTE,
    //                  lc.mRecordSize, null, null, mAid,
    //                  obtainMessage(EVENT_READ_RECORD_DONE, lc));
    //          break;
    //     case EVENT_GET_BINARY_SIZE_DONE:
    //         ar = (AsyncResult)msg.obj;
    //         response = (Message) ar.userObj;
    //         result = (IccIoResult) ar.result;
    //
    //         if (processException(response, (AsyncResult) msg.obj)) {
    //             break;
    //         }
    //
    //         data = result.payload;
    //
    //         fileid = msg.arg1;
    //
    //         if (UiccTlvData.isUiccTlvData(data)) {
    //
    //
    //             UiccTlvData tlvData = UiccTlvData.parse(data);
    //
    //             if (tlvData.mFileSize < 0) {
    //                 throw new IccFileTypeMismatch();
    //             }
    //
    //             size = tlvData.mFileSize;
    //
    //
    //         } else if (TYPE_EF == data[RESPONSE_DATA_FILE_TYPE]) {
    //
    //             if (EF_TYPE_TRANSPARENT != data[RESPONSE_DATA_STRUCTURE]) {
    //                 throw new IccFileTypeMismatch();
    //             }
    //
    //             size = ((data[RESPONSE_DATA_FILE_SIZE_1] & 0xff) << 8)
    //                     + (data[RESPONSE_DATA_FILE_SIZE_2] & 0xff);
    //         } else {
    //             throw new IccFileTypeMismatch();
    //         }
    //
    //         mCi.iccIOForApp(COMMAND_READ_BINARY, fileid, getEFPath(fileid),
    //                         0, 0, size, null, null, mAid,
    //                         obtainMessage(EVENT_READ_BINARY_DONE,
    //                                       fileid, 0, response));
    //     break;
    //
    //     case EVENT_READ_IMG_DONE:
    //     case EVENT_READ_RECORD_DONE:
    //
    //         ar = (AsyncResult)msg.obj;
    //         lc = (LoadLinearFixedContext) ar.userObj;
    //         result = (IccIoResult) ar.result;
    //         response = lc.mOnLoaded;
    //         path = lc.mPath;
    //
    //         if (processException(response, (AsyncResult) msg.obj)) {
    //             break;
    //         }
    //
    //         if (lc.mLoadAll) {
    //             lc.results.add(result.payload);
    //
    //             lc.mRecordNum++;
    //
    //             if (lc.mRecordNum > lc.mCountRecords) {
    //                 sendResult(response, lc.results, null);
    //             } else {
    //                 if (path == null) {
    //                     path = getEFPath(lc.mEfid);
    //                 }
    //
    //                 mCi.iccIOForApp(COMMAND_READ_RECORD, lc.mEfid, path,
    //                             lc.mRecordNum,
    //                             READ_RECORD_MODE_ABSOLUTE,
    //                             lc.mRecordSize, null, null, mAid,
    //                             obtainMessage(EVENT_READ_RECORD_DONE, lc));
    //             }
    //         } else if (lc.mLoadPart) {
    //             lc.results.set(lc.mRecordNum - 1, result.payload);
    //             lc.mCount++;
    //             if (lc.mCount < lc.mCountLoadrecords) {
    //                 lc.mRecordNum = lc.mRecordNums.get(lc.mCount);
    //                 if (lc.mRecordNum <= lc.mCountRecords) {
    //                     if (path == null) {
    //                         path = getEFPath(lc.mEfid);
    //                     }
    //                     mCi.iccIOForApp(COMMAND_READ_RECORD, lc.mEfid, path, lc.mRecordNum,
    //                             READ_RECORD_MODE_ABSOLUTE, lc.mRecordSize, null, null, mAid,
    //                             obtainMessage(EVENT_READ_RECORD_DONE, lc));
    //                 } else {
    //                     sendResult(response, lc.results, null);
    //                 }
    //             } else {
    //                 sendResult(response, lc.results, null);
    //             }
    //         }
    //     else{
    //         sendResult(response, result.payload, null);
    //         }
    //
    //     break;
    //
    //     case EVENT_READ_BINARY_DONE:
    //     case EVENT_READ_ICON_DONE:
    //         ar = (AsyncResult)msg.obj;
    //         response = (Message) ar.userObj;
    //         result = (IccIoResult) ar.result;
    //
    //         if (processException(response, (AsyncResult) msg.obj)) {
    //             break;
    //         }
    //
    //         sendResult(response, result.payload, null);
    //     break;
    //
    // }} catch (Exception exc) {
    //     if (response != null) {
    //         sendResult(response, null, exc);
    //     } else {
    //         loge("uncaught exception" + exc);
    //     }
    // }
    // assert(0);
    return NOERROR;
}

ECode IccFileHandler::constructor(
    /* [in] */ IUiccCardApplication* app,
    /* [in] */ const String& aid,
    /* [in] */ ICommandsInterface* ci)
{
    mParentApp = app;
    mAid = aid;
    mCi = ci;
    return Handler::constructor();
}

String IccFileHandler::GetCommonIccEFPath(
    /* [in] */ Int32 efid)
{
    switch(efid) {
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
    return String(NULL);
}

void IccFileHandler::SendResult(
    /* [in] */ IMessage* response,
    /* [in] */ IInterface* result)
    ///* [in] */ Throwable* ex)
{
    // ==================before translated======================
    // if (response == null) {
    //     return;
    // }
    //
    // AsyncResult.forMessage(response, result, ex);
    //
    // response.sendToTarget();
    assert(0);
}

Boolean IccFileHandler::ProcessException(
    /* [in] */ IMessage* response,
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // IccException iccException;
    // boolean flag = false;
    // IccIoResult result = (IccIoResult) ar.result;
    // if (ar.exception != null) {
    //     sendResult(response, null, ar.exception);
    //     flag = true;
    // } else {
    //     iccException = result.getException();
    //     if (iccException != null) {
    //         sendResult(response, null, iccException);
    //         flag = true;
    //     }
    // }
    // return flag;
    assert(0);
    return FALSE;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
