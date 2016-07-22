
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/uicc/AdnRecordLoader.h"
#include "elastos/droid/internal/telephony/uicc/AdnRecord.h"
#include "elastos/droid/internal/telephony/uicc/CIccUtils.h"
#include "elastos/droid/internal/telephony/uicc/CAdnRecord.h"
#include "elastos/droid/os/CLooperHelper.h"
#include "elastos/droid/os/AsyncResult.h"

#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;

using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                           AdnRecordLoader
//=====================================================================
CAR_INTERFACE_IMPL(AdnRecordLoader, Handler, IAdnRecordLoader);

const String AdnRecordLoader::LOGTAG("AdnRecordLoader");
const Boolean AdnRecordLoader::VDBG = FALSE;
const Int32 AdnRecordLoader::EVENT_ADN_LOAD_DONE;
const Int32 AdnRecordLoader::EVENT_EXT_RECORD_LOAD_DONE;
const Int32 AdnRecordLoader::EVENT_ADN_LOAD_ALL_DONE;
const Int32 AdnRecordLoader::EVENT_EF_LINEAR_RECORD_SIZE_DONE;
const Int32 AdnRecordLoader::EVENT_UPDATE_RECORD_DONE;

AdnRecordLoader::AdnRecordLoader()
{
}

ECode AdnRecordLoader::constructor(
    /* [in] */ IIccFileHandler* fh)
{
    // The telephony unit-test cases may create AdnRecords
    // in secondary threads
    AutoPtr<ILooperHelper> hlp;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&hlp);
    AutoPtr<ILooper> lp;
    hlp->GetMainLooper((ILooper**)&lp);
    Handler::constructor(lp);
    mFh = fh;
    return NOERROR;
}

ECode AdnRecordLoader::LoadFromEF(
    /* [in] */ Int32 ef,
    /* [in] */ Int32 extensionEF,
    /* [in] */ Int32 recordNumber,
    /* [in] */ IMessage* response)
{
    mEf = ef;
    mExtensionEF = extensionEF;
    mRecordNumber = recordNumber;
    mUserResponse = response;
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_ADN_LOAD_DONE, (IMessage**)&msg);
    if (ef == IIccConstants::EF_ADN) {
        mFh->LoadEFLinearFixed(
                    ef, GetEFPath(ef), recordNumber,
                    msg);
    }
    else {
        mFh->LoadEFLinearFixed(
                ef, recordNumber,
                msg);
    }
    return NOERROR;
}

ECode AdnRecordLoader::LoadAllFromEF(
    /* [in] */ Int32 ef,
    /* [in] */ Int32 extensionEF,
    /* [in] */ IMessage* response)
{
    mEf = ef;
    mExtensionEF = extensionEF;
    mUserResponse = response;

    /* If we are loading from EF_ADN, specifically
     * specify the path as well, since, on some cards,
     * the fileid is not unique.
     */
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_ADN_LOAD_ALL_DONE, (IMessage**)&msg);
    if (ef == IIccConstants::EF_ADN) {
        mFh->LoadEFLinearFixedAll(
                ef, GetEFPath(ef),
                msg);
    }
    else {
        mFh->LoadEFLinearFixedAll(
                ef,
                msg);
    }
    return NOERROR;
}

ECode AdnRecordLoader::UpdateEF(
    /* [in] */ IAdnRecord* adn,
    /* [in] */ Int32 ef,
    /* [in] */ Int32 extensionEF,
    /* [in] */ Int32 recordNumber,
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* response)
{
    mEf = ef;
    mExtensionEF = extensionEF;
    mRecordNumber = recordNumber;
    mUserResponse = response;
    mPin2 = pin2;

    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_EF_LINEAR_RECORD_SIZE_DONE, adn, (IMessage**)&msg);
    if (ef == IIccConstants::EF_ADN) {
        mFh->GetEFLinearRecordSize(ef, GetEFPath(ef),
            msg);
    }
    else {
        mFh->GetEFLinearRecordSize( ef,
            msg);
    }
    return NOERROR;
}

ECode AdnRecordLoader::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;
    AutoPtr<ArrayOf<Byte> > data;
    AutoPtr<AdnRecord> adn;
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    // try {
        switch (what) {
            case EVENT_EF_LINEAR_RECORD_SIZE_DONE: {
                ar = (AsyncResult*)(IObject*)(obj.Get());
                adn = (AdnRecord*)(IObject*)(ar->mUserObj).Get();

                if (ar->mException != NULL) {
                    // throw new RuntimeException("get EF record size failed",
                    //         ar.exception);
                    return E_RUNTIME_EXCEPTION;
                }

                assert(0 && "TODO");
                AutoPtr<ArrayOf<Int32> > recordSize;// = (int[])ar.result;
                // recordSize is int[3] array
                // int[0]  is the record length
                // int[1]  is the total length of the EF file
                // int[2]  is the number of records in the EF file
                // So int[0] * int[2] = int[1]
                if (recordSize->GetLength() != 3 || mRecordNumber > (*recordSize)[2]) {
                    // throw new RuntimeException("get wrong EF record size format",
                    //         ar.exception);
                    return E_RUNTIME_EXCEPTION;
                }

                adn->BuildAdnString((*recordSize)[0], (ArrayOf<Byte>**)&data);

                if (data == NULL) {
                    // throw new RuntimeException("wrong ADN format",
                    //         ar.exception);
                    return E_RUNTIME_EXCEPTION;
                }

                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_UPDATE_RECORD_DONE, (IMessage**)&msg);
                if (mEf == IIccConstants::EF_ADN) {
                    mFh->UpdateEFLinearFixed(mEf, GetEFPath(mEf), mRecordNumber,
                            data, mPin2, msg);
                }
                else {
                    mFh->UpdateEFLinearFixed(mEf, mRecordNumber,
                            data, mPin2, msg);
                }

                mPendingExtLoads = 1;

                break;
            }
            case EVENT_UPDATE_RECORD_DONE: {
                ar = (AsyncResult*)(IObject*)(obj.Get());
                if (ar->mException != NULL) {
                    // throw new RuntimeException("update EF adn record failed",
                    //         ar.exception);
                    return E_RUNTIME_EXCEPTION;
                }
                mPendingExtLoads = 0;
                mResult = NULL;
                break;
            }
            case EVENT_ADN_LOAD_DONE: {
                ar = (AsyncResult*)(IObject*)(obj.Get());
                assert(0 && "TODO");
                // data = (ArrayOf<Byte>*)(ar->mResult);

                if (ar->mException != NULL) {
                    // throw new RuntimeException("load failed", ar.exception);
                    return E_RUNTIME_EXCEPTION;
                }

                if (VDBG) {
                    AutoPtr<IIccUtils> iccu;
                    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
                    String str;
                    iccu->BytesToHexString(data, &str);
                    Logger::D(LOGTAG, String("ADN EF: 0x")
                        + StringUtils::ToString(mEf)
                        + String(":") + StringUtils::ToString(mRecordNumber)
                        + String("\n") + str);
                }

                CAdnRecord::New(mEf, mRecordNumber, data, (IAdnRecord**)&adn);
                mResult = (IObject*)adn.Get();

                Boolean bHasExtendedRecord = FALSE;
                adn->HasExtendedRecord(&bHasExtendedRecord);
                if (bHasExtendedRecord) {
                    // If we have a valid value in the ext record field,
                    // we're not done yet: we need to read the corresponding
                    // ext record and append it

                    mPendingExtLoads = 1;

                    AutoPtr<IMessage> msg;
                    ObtainMessage(EVENT_EXT_RECORD_LOAD_DONE, (IObject*)adn.Get(), (IMessage**)&msg);
                    mFh->LoadEFLinearFixed(
                        mExtensionEF, adn->mExtRecord,
                        msg);
                }
                break;
            }
            case EVENT_EXT_RECORD_LOAD_DONE: {
                ar = (AsyncResult*)(IObject*)(obj.Get());
                assert(0 && "TODO");
                // data = (ArrayOf<Byte>*)(ar->mResult);
                adn = (AdnRecord*)(IObject*)(ar->mUserObj).Get();

                if (ar->mException != NULL) {
                    // throw new RuntimeException("load failed", ar.exception);
                    return E_RUNTIME_EXCEPTION;
                }

                AutoPtr<IIccUtils> iccu;
                CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
                String str;
                iccu->BytesToHexString(data, &str);
                Logger::D(LOGTAG, String("ADN extension EF: 0x")
                    + StringUtils::ToString(mExtensionEF)
                    + String(":") + StringUtils::ToString(adn->mExtRecord)
                    + String("\n") + str);

                adn->AppendExtRecord(data);

                mPendingExtLoads--;
                // result should have been set in
                // EVENT_ADN_LOAD_DONE or EVENT_ADN_LOAD_ALL_DONE
                break;
            }
            case EVENT_ADN_LOAD_ALL_DONE: {
                ar = (AsyncResult*)(IObject*)(obj.Get());
                AutoPtr<IArrayList> datas = IArrayList::Probe(ar->mResult);

                if (ar->mException != NULL) {
                    // throw new RuntimeException("load failed", ar.exception);
                    return E_RUNTIME_EXCEPTION;
                }

                Int32 size = 0;
                datas->GetSize(&size);
                CArrayList::New(size, (IArrayList**)&mAdns);
                mResult = mAdns;
                mPendingExtLoads = 0;

                for (Int32 i = 0, s = size; i < s; i++) {
                    AutoPtr<IInterface> p;
                    datas->Get(i, (IInterface**)&p);
                    assert(0 && "TODO");
                    // CAdnRecord::New(mEf, 1 + i, p, (IAdnRecord**)&adn);
                    mAdns->Add((IObject*)adn.Get());
                    Boolean bHasExtendedRecord = FALSE;
                    adn->HasExtendedRecord(&bHasExtendedRecord);
                    if (bHasExtendedRecord) {
                        // If we have a valid value in the ext record field,
                        // we're not done yet: we need to read the corresponding
                        // ext record and append it

                        mPendingExtLoads++;
                        AutoPtr<IMessage> msg;
                        ObtainMessage(EVENT_EXT_RECORD_LOAD_DONE, (IObject*)adn.Get(), (IMessage**)&msg);
                        mFh->LoadEFLinearFixed(
                            mExtensionEF, adn->mExtRecord,
                            msg);
                    }
                }
            break;
            }
        }
    // } catch (RuntimeException exc) {
    //     if (mUserResponse != NULL) {
    //         AsyncResult.forMessage(mUserResponse)
    //                         .exception = exc;
    //         mUserResponse.sendToTarget();
    //         // Loading is all or nothing--either every load succeeds
    //         // or we fail the whole thing.
    //         mUserResponse = NULL;
    //     }
    //     return;
    // }

    if (mUserResponse != NULL && mPendingExtLoads == 0) {
        AsyncResult::ForMessage(mUserResponse)->mResult
            = mResult;

        mUserResponse->SendToTarget();
        mUserResponse = NULL;
    }
    return NOERROR;
}

String AdnRecordLoader::GetEFPath(
    /* [in] */ Int32 efid)
{
    if (efid == IIccConstants::EF_ADN) {
        return IIccConstants::MF_SIM + IIccConstants::DF_TELECOM;
    }

    return String(NULL);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
