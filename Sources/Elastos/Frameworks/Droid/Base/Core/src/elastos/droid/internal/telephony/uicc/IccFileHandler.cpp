
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccFileHandler.h"
#include "elastos/droid/internal/telephony/uicc/CIccUtils.h"
#include "elastos/droid/internal/telephony/uicc/IccIoResult.h"
#include "elastos/droid/internal/telephony/uicc/UiccTlvData.h"
#include "elastos/droid/internal/telephony/uicc/CUiccTlvDataHelper.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Os::IAsyncResult;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;

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
    mEfid = efid;
    mRecordNum = recordNum;
    mOnLoaded = onLoaded;
    mLoadAll = FALSE;
    mLoadPart = FALSE;
    mPath = NULL;
}

IccFileHandler::LoadLinearFixedContext::LoadLinearFixedContext(
    /* [in] */ Int32 efid,
    /* [in] */ Int32 recordNum,
    /* [in] */ const String& path,
    /* [in] */ IMessage* onLoaded)
{
    mEfid = efid;
    mRecordNum = recordNum;
    mOnLoaded = onLoaded;
    mLoadAll = FALSE;
    mLoadPart = FALSE;
    mPath = path;
}

IccFileHandler::LoadLinearFixedContext::LoadLinearFixedContext(
    /* [in] */ Int32 efid,
    /* [in] */ const String& path,
    /* [in] */ IMessage* onLoaded)
{
    mEfid = efid;
    mRecordNum = 1;
    mLoadAll = TRUE;
    mLoadPart = FALSE;
    mOnLoaded = onLoaded;
    mPath = path;
}

IccFileHandler::LoadLinearFixedContext::LoadLinearFixedContext(
    /* [in] */ Int32 efid,
    /* [in] */ IMessage* onLoaded)
{
    mEfid = efid;
    mRecordNum = 1;
    mLoadAll = TRUE;
    mLoadPart = FALSE;
    mOnLoaded = onLoaded;
    mPath = NULL;
}

IccFileHandler::LoadLinearFixedContext::LoadLinearFixedContext(
    /* [in] */ Int32 efid,
    /* [in] */ IArrayList/*<Integer*>*/* recordNums,
    /* [in] */ const String& path,
    /* [in] */ IMessage* onLoaded)
{
    mEfid = efid;
    AutoPtr<IInterface> p;
    recordNums->Get(0, (IInterface**)&p);
    IInteger32::Probe(p)->GetValue(&mRecordNum);
    mLoadAll = FALSE;
    mLoadPart = TRUE;
    CArrayList::New((IArrayList**)&mRecordNums);
    mRecordNums->AddAll(ICollection::Probe(recordNums));
    mCount = 0;
    recordNums->GetSize(&mCountLoadrecords);
    mOnLoaded = onLoaded;
    mPath = path;
}

void IccFileHandler::LoadLinearFixedContext::InitLCResults(
    /* [in] */ Int32 size)
{
    CArrayList::New(size, (IArrayList**)&mResults);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(mRecordSize);
    for (Int32 i = 0; i < mRecordSize; i++) {
        (*data)[i] = (Byte) 0xff;
    }
    for (Int32 i = 0; i < size; i++) {
        mResults->Add(CoreUtils::ConvertByteArray(data));
    }
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
    return NOERROR;
}

ECode IccFileHandler::LoadEFLinearFixed(
    /* [in] */ Int32 fileid,
    /* [in] */ const String& path,
    /* [in] */ Int32 recordNum,
    /* [in] */ IMessage* onLoaded)
{
    AutoPtr<LoadLinearFixedContext> p = new LoadLinearFixedContext(fileid, recordNum, path, onLoaded);
    AutoPtr<IMessage> response;
    ObtainMessage(EVENT_GET_RECORD_SIZE_DONE,
                (IObject*)p.Get(), (IMessage**)&response);

    mCi->IccIOForApp(COMMAND_GET_RESPONSE, fileid, path,
                    0, 0, GET_RESPONSE_EF_SIZE_BYTES, String(NULL), String(NULL), mAid, response);
    return NOERROR;
}

ECode IccFileHandler::LoadEFLinearFixed(
    /* [in] */ Int32 fileid,
    /* [in] */ Int32 recordNum,
    /* [in] */ IMessage* onLoaded)
{
    LoadEFLinearFixed(fileid, GetEFPath(fileid), recordNum, onLoaded);
    return NOERROR;
}

ECode IccFileHandler::LoadEFImgLinearFixed(
    /* [in] */ Int32 recordNum,
    /* [in] */ IMessage* onLoaded)
{
    AutoPtr<LoadLinearFixedContext> p = new LoadLinearFixedContext(
                    IIccConstants::EF_IMG, recordNum,
                    onLoaded);
    AutoPtr<IMessage> response;
    ObtainMessage(EVENT_GET_RECORD_SIZE_IMG_DONE,
            (IObject*)p.Get(), (IMessage**)&response);

    mCi->IccIOForApp(COMMAND_GET_RESPONSE, IIccConstants::EF_IMG,
                GetEFPath(IIccConstants::EF_IMG), recordNum,
                READ_RECORD_MODE_ABSOLUTE, GET_RESPONSE_EF_IMG_SIZE_BYTES,
                String(NULL), String(NULL), mAid, response);
    return NOERROR;
}

ECode IccFileHandler::GetEFLinearRecordSize(
    /* [in] */ Int32 fileid,
    /* [in] */ const String& path,
    /* [in] */ IMessage* onLoaded)
{
    AutoPtr<LoadLinearFixedContext> p = new LoadLinearFixedContext(fileid, path, onLoaded);
    AutoPtr<IMessage> response;
    ObtainMessage(EVENT_GET_EF_LINEAR_RECORD_SIZE_DONE,
                (IObject*)p.Get(), (IMessage**)&response);
    mCi->IccIOForApp(COMMAND_GET_RESPONSE, fileid, path,
                0, 0, GET_RESPONSE_EF_SIZE_BYTES, String(NULL), String(NULL), mAid, response);
    return NOERROR;
}

ECode IccFileHandler::GetEFLinearRecordSize(
    /* [in] */ Int32 fileid,
    /* [in] */ IMessage* onLoaded)
{
    GetEFLinearRecordSize(fileid, GetEFPath(fileid), onLoaded);
    return NOERROR;
}

ECode IccFileHandler::LoadEFLinearFixedAll(
    /* [in] */ Int32 fileid,
    /* [in] */ IMessage* onLoaded)
{
    LoadEFLinearFixedAll(fileid, GetEFPath(fileid), onLoaded);
    return NOERROR;
}

ECode IccFileHandler::LoadEFLinearFixedAll(
    /* [in] */ Int32 fileid,
    /* [in] */ const String& path,
    /* [in] */ IMessage* onLoaded)
{
    AutoPtr<LoadLinearFixedContext> p = new LoadLinearFixedContext(fileid, path, onLoaded);
    AutoPtr<IMessage> response;
    ObtainMessage(EVENT_GET_RECORD_SIZE_DONE,
                (IObject*)p.Get(), (IMessage**)&response);

    mCi->IccIOForApp(COMMAND_GET_RESPONSE, fileid, path,
                    0, 0, GET_RESPONSE_EF_SIZE_BYTES, String(NULL), String(NULL), mAid, response);
    return NOERROR;
}

ECode IccFileHandler::LoadEFLinearFixedPart(
    /* [in] */ Int32 fileid,
    /* [in] */ IArrayList/*<Integer*>*/* recordNums,
    /* [in] */ IMessage* onLoaded)
{
    AutoPtr<LoadLinearFixedContext> p = new LoadLinearFixedContext(
            fileid, recordNums, GetEFPath(fileid), onLoaded);
    AutoPtr<IMessage> response;
    ObtainMessage(EVENT_GET_RECORD_SIZE_DONE, (IObject*)p.Get(), (IMessage**)&response);

    mCi->IccIOForApp(COMMAND_GET_RESPONSE, fileid, GetEFPath(fileid), 0, 0,
            GET_RESPONSE_EF_SIZE_BYTES, String(NULL), String(NULL), mAid, response);
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
    AutoPtr<IMessage> response;
    ObtainMessage(EVENT_READ_BINARY_DONE,
                    fileid, 0, onLoaded,
                    (IMessage**)&response);

    mCi->IccIOForApp(COMMAND_READ_BINARY, fileid, GetEFPath(fileid),
                    0, 0, size, String(NULL), String(NULL), mAid, response);
    return NOERROR;
}

ECode IccFileHandler::LoadEFImgTransparent(
    /* [in] */ Int32 fileid,
    /* [in] */ Int32 highOffset,
    /* [in] */ Int32 lowOffset,
    /* [in] */ Int32 length,
    /* [in] */ IMessage* onLoaded)
{
    AutoPtr<IMessage> response;
    ObtainMessage(EVENT_READ_ICON_DONE, fileid, 0,
            onLoaded, (IMessage**)&response);

    Logd(String("IccFileHandler: loadEFImgTransparent fileid = ") + StringUtils::ToString(fileid)
        + String(" filePath = ") + GetEFPath(EF_IMG)
        + String(" highOffset = ") + StringUtils::ToString(highOffset)
        + String(" lowOffset = ") + StringUtils::ToString(lowOffset)
        + String(" length = ") + StringUtils::ToString(length));

    /* Per TS 31.102, for displaying of Icon, under
     * DF Telecom and DF Graphics , EF instance(s) (4FXX,transparent files)
     * are present. The possible image file identifiers (EF instance) for
     * EF img ( 4F20, linear fixed file) are : 4F01 ... 4F05.
     * It should be MF_SIM + DF_TELECOM + DF_GRAPHICS, same path as EF IMG
     */
    mCi->IccIOForApp(COMMAND_READ_BINARY, fileid, GetEFPath(EF_IMG),
            highOffset, lowOffset, length, String(NULL), String(NULL), mAid, response);
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
    AutoPtr<IIccUtils> iccu;
    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    String strData;
    iccu->BytesToHexString(data, &strData);
    mCi->IccIOForApp(COMMAND_UPDATE_RECORD, fileid, path,
                    recordNum, READ_RECORD_MODE_ABSOLUTE, data->GetLength(),
                    strData, pin2, mAid, onComplete);
    return NOERROR;
}

ECode IccFileHandler::UpdateEFLinearFixed(
    /* [in] */ Int32 fileid,
    /* [in] */ Int32 recordNum,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* onComplete)
{
    AutoPtr<IIccUtils> iccu;
    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    String strData;
    iccu->BytesToHexString(data, &strData);
    mCi->IccIOForApp(COMMAND_UPDATE_RECORD, fileid, GetEFPath(fileid),
                    recordNum, READ_RECORD_MODE_ABSOLUTE, data->GetLength(),
                    strData, pin2, mAid, onComplete);
    return NOERROR;
}

ECode IccFileHandler::UpdateEFTransparent(
    /* [in] */ Int32 fileid,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IMessage* onComplete)
{
    AutoPtr<IIccUtils> iccu;
    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    String strData;
    iccu->BytesToHexString(data, &strData);
    mCi->IccIOForApp(COMMAND_UPDATE_BINARY, fileid, GetEFPath(fileid),
                    0, 0, data->GetLength(),
                    strData, String(NULL), mAid, onComplete);
    return NOERROR;
}

ECode IccFileHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;
    AutoPtr<IccIoResult> result;
    AutoPtr<IMessage> response;
    String str;
    AutoPtr<LoadLinearFixedContext> lc;

    AutoPtr<ArrayOf<Byte> > data;
    Int32 size;
    Int32 fileid;
    AutoPtr<ArrayOf<Int32> > recordSize;
    String path(NULL);
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    Int32 arg1 = 0;
    msg->GetArg1(&arg1);

    AutoPtr<IUiccTlvDataHelper> hlp;
    CUiccTlvDataHelper::AcquireSingleton((IUiccTlvDataHelper**)&hlp);
    Boolean bTlvData = FALSE;
    // try {
    switch (what) {
        case EVENT_GET_EF_LINEAR_RECORD_SIZE_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            lc = (LoadLinearFixedContext*)(IObject*)((ar->mUserObj).Get());
            result = (IccIoResult*)IIccIoResult::Probe(ar->mResult);
            response = lc->mOnLoaded;

            if (ProcessException(response, (AsyncResult*)IAsyncResult::Probe(obj))) {
                break;
            }

            data = result->mPayload;

            hlp->IsUiccTlvData(data, &bTlvData);
            if (bTlvData) {

                AutoPtr<IUiccTlvData> tlvData;
                hlp->Parse(data, (IUiccTlvData**)&tlvData);

                Boolean bComplete = FALSE;
                tlvData->IsIncomplete(&bComplete);
                if (bComplete) {
                    // throw new IccFileTypeMismatch();
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }

                recordSize = ArrayOf<Int32>::Alloc(3);
                AutoPtr<UiccTlvData> _tlvData = (UiccTlvData*)tlvData.Get();
                (*recordSize)[0] = _tlvData->mRecordSize;
                (*recordSize)[1] = _tlvData->mFileSize;
                (*recordSize)[2] = _tlvData->mNumRecords;

            }
            else if (TYPE_EF == (*data)[RESPONSE_DATA_FILE_TYPE] &&
                EF_TYPE_LINEAR_FIXED == (*data)[RESPONSE_DATA_STRUCTURE]) {

                recordSize = ArrayOf<Int32>::Alloc(3);
                (*recordSize)[0] = (*data)[RESPONSE_DATA_RECORD_LENGTH] & 0xFF;
                (*recordSize)[1] = (((*data)[RESPONSE_DATA_FILE_SIZE_1] & 0xff) << 8)
                        + ((*data)[RESPONSE_DATA_FILE_SIZE_2] & 0xff);
                (*recordSize)[2] = (*recordSize)[1] / (*recordSize)[0];

            }
            else {
                // throw new IccFileTypeMismatch();
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            SendResult(response, CoreUtils::Convert(recordSize));
            break;
        }
        case EVENT_GET_RECORD_SIZE_IMG_DONE:
        case EVENT_GET_RECORD_SIZE_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            lc = (LoadLinearFixedContext*)(IObject*)((ar->mUserObj).Get());
            result = (IccIoResult*)IIccIoResult::Probe(ar->mResult);
            response = lc->mOnLoaded;

            if (ProcessException(response, (AsyncResult*)IAsyncResult::Probe(obj))) {
                break;
            }

            data = result->mPayload;
            path = lc->mPath;

            hlp->IsUiccTlvData(data, &bTlvData);
            if (bTlvData) {

                AutoPtr<IUiccTlvData> tlvData;
                hlp->Parse(data, (IUiccTlvData**)&tlvData);

                Boolean bComplete = FALSE;
                tlvData->IsIncomplete(&bComplete);
                if (bComplete) {
                    // throw new IccFileTypeMismatch();
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }

                AutoPtr<UiccTlvData> _tlvData = (UiccTlvData*)tlvData.Get();
                lc->mRecordSize = _tlvData->mRecordSize;
                lc->mCountRecords = _tlvData->mNumRecords;
                size = _tlvData->mFileSize;

            }
            else if (TYPE_EF == (*data)[RESPONSE_DATA_FILE_TYPE]) {

                if (EF_TYPE_LINEAR_FIXED != (*data)[RESPONSE_DATA_STRUCTURE]) {
                    // throw new IccFileTypeMismatch();
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }

                lc->mRecordSize = (*data)[RESPONSE_DATA_RECORD_LENGTH] & 0xFF;

                size = (((*data)[RESPONSE_DATA_FILE_SIZE_1] & 0xff) << 8)
                        + ((*data)[RESPONSE_DATA_FILE_SIZE_2] & 0xff);

                lc->mCountRecords = size / lc->mRecordSize;
            }
            else {
                // throw new IccFileTypeMismatch();
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            if (lc->mLoadAll) {
                CArrayList::New(lc->mCountRecords, (IArrayList**)&(lc->mResults));
            }
            else if (lc->mLoadPart) {
                lc->InitLCResults(lc->mCountRecords);
            }

            if (path == NULL) {
                path = GetEFPath(lc->mEfid);
            }
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_READ_RECORD_DONE, (IObject*)lc.Get(), (IMessage**)&msg);
            mCi->IccIOForApp(COMMAND_READ_RECORD, lc->mEfid, path,
                    lc->mRecordNum,
                    READ_RECORD_MODE_ABSOLUTE,
                    lc->mRecordSize, String(NULL), String(NULL), mAid,
                    msg);
            break;
        }
        case EVENT_GET_BINARY_SIZE_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            response = IMessage::Probe(ar->mUserObj);
            result = (IccIoResult*)IIccIoResult::Probe(ar->mResult);

            if (ProcessException(response, ar)) {
                break;
            }

            data = result->mPayload;

            fileid = arg1;

            hlp->IsUiccTlvData(data, &bTlvData);
            if (bTlvData) {
                AutoPtr<IUiccTlvData> tlvData;
                hlp->Parse(data, (IUiccTlvData**)&tlvData);

                AutoPtr<UiccTlvData> _tlvData = (UiccTlvData*)tlvData.Get();
                if (_tlvData->mFileSize < 0) {
                    // throw new IccFileTypeMismatch();
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }

                size = _tlvData->mFileSize;

            }
            else if (TYPE_EF == (*data)[RESPONSE_DATA_FILE_TYPE]) {

                if (EF_TYPE_TRANSPARENT != (*data)[RESPONSE_DATA_STRUCTURE]) {
                    // throw new IccFileTypeMismatch();
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }

                size = (((*data)[RESPONSE_DATA_FILE_SIZE_1] & 0xff) << 8)
                        + ((*data)[RESPONSE_DATA_FILE_SIZE_2] & 0xff);
            }
            else {
                // throw new IccFileTypeMismatch();
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_READ_BINARY_DONE,
                        fileid, 0, response,
                        (IMessage**)&msg);
            mCi->IccIOForApp(COMMAND_READ_BINARY, fileid, GetEFPath(fileid),
                            0, 0, size, String(NULL), String(NULL), mAid,
                            msg);
        break;
        }
        case EVENT_READ_IMG_DONE:
        case EVENT_READ_RECORD_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            lc = (LoadLinearFixedContext*)IObject::Probe(ar->mUserObj);
            result = (IccIoResult*)IIccIoResult::Probe(ar->mResult);
            response = lc->mOnLoaded;
            path = lc->mPath;

            if (ProcessException(response, ar)) {
                break;
            }

            if (lc->mLoadAll) {
                lc->mResults->Add(CoreUtils::ConvertByteArray(result->mPayload));

                lc->mRecordNum++;

                if (lc->mRecordNum > lc->mCountRecords) {
                    SendResult(response, lc->mResults);
                }
                else {
                    if (path == NULL) {
                        path = GetEFPath(lc->mEfid);
                    }

                    AutoPtr<IMessage> msg;
                    ObtainMessage(EVENT_READ_RECORD_DONE, (IObject*)lc.Get(), (IMessage**)&msg);
                    mCi->IccIOForApp(COMMAND_READ_RECORD, lc->mEfid, path,
                                lc->mRecordNum,
                                READ_RECORD_MODE_ABSOLUTE,
                                lc->mRecordSize, String(NULL), String(NULL), mAid,
                                msg);
                }
            }
            else if (lc->mLoadPart) {
                lc->mResults->Set(lc->mRecordNum - 1, CoreUtils::ConvertByteArray(result->mPayload));
                lc->mCount++;
                if (lc->mCount < lc->mCountLoadrecords) {
                    AutoPtr<IInterface> p;
                    lc->mRecordNums->Get(lc->mCount, (IInterface**)&p);
                    IInteger32::Probe(p)->GetValue(&(lc->mRecordNum));
                    if (lc->mRecordNum <= lc->mCountRecords) {
                        if (path == NULL) {
                            path = GetEFPath(lc->mEfid);
                        }
                        AutoPtr<IMessage> msg;
                        ObtainMessage(EVENT_READ_RECORD_DONE, (IObject*)lc.Get(), (IMessage**)&msg);
                        mCi->IccIOForApp(COMMAND_READ_RECORD, lc->mEfid, path, lc->mRecordNum,
                                READ_RECORD_MODE_ABSOLUTE, lc->mRecordSize, String(NULL), String(NULL),
                                mAid, msg);
                    }
                    else {
                        SendResult(response, lc->mResults);
                    }
                }
                else {
                    SendResult(response, lc->mResults);
                }
            }
            else {
                SendResult(response, CoreUtils::ConvertByteArray(result->mPayload));
            }
        }
        break;
        case EVENT_READ_BINARY_DONE:
        case EVENT_READ_ICON_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            response = IMessage::Probe(ar->mUserObj);
            result = (IccIoResult*)IIccIoResult::Probe(ar->mResult);

            if (ProcessException(response, ar)) {
                break;
            }

            SendResult(response, CoreUtils::ConvertByteArray(result->mPayload));
        break;
        }
    }
    // }} catch (Exception exc) {
    //     if (response != NULL) {
    //         sendResult(response, NULL, exc);
    //     } else {
    //         loge("uncaught exception" + exc);
    //     }
    // }
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
{
    if (response == NULL) {
        return;
    }

    AsyncResult::ForMessage(response, result, NULL/*ex*/);

    response->SendToTarget();
}

Boolean IccFileHandler::ProcessException(
    /* [in] */ IMessage* response,
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<IIccException> iccException;
    Boolean flag = FALSE;
    AutoPtr<IIccIoResult> result = IIccIoResult::Probe(ar->mResult);
    if (ar->mException != NULL) {
        // SendResult(response, NULL, ar->mException);
        SendResult(response, NULL);
        flag = TRUE;
    }
    else {
        result->GetException((IIccException**)&iccException);
        if (iccException != NULL) {
            // SendResult(response, NULL, iccException);
            SendResult(response, NULL);
            flag = TRUE;
        }
    }
    return flag;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
