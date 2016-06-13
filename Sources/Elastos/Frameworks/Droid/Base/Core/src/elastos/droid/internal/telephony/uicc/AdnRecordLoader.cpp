
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/uicc/AdnRecordLoader.h"

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
    // ==================before translated======================
    // // The telephony unit-test cases may create AdnRecords
    // // in secondary threads
    // super(Looper.getMainLooper());
    // mFh = fh;
    return NOERROR;
}

ECode AdnRecordLoader::LoadFromEF(
    /* [in] */ Int32 ef,
    /* [in] */ Int32 extensionEF,
    /* [in] */ Int32 recordNumber,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // mEf = ef;
    // mExtensionEF = extensionEF;
    // mRecordNumber = recordNumber;
    // mUserResponse = response;
    //
    // if (ef == IccConstants.EF_ADN) {
    //     mFh.loadEFLinearFixed(
    //                 ef, getEFPath(ef), recordNumber,
    //                 obtainMessage(EVENT_ADN_LOAD_DONE));
    // } else {
    //     mFh.loadEFLinearFixed(
    //             ef, recordNumber,
    //             obtainMessage(EVENT_ADN_LOAD_DONE));
    // }
    assert(0);
    return NOERROR;
}

ECode AdnRecordLoader::LoadAllFromEF(
    /* [in] */ Int32 ef,
    /* [in] */ Int32 extensionEF,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // mEf = ef;
    // mExtensionEF = extensionEF;
    // mUserResponse = response;
    //
    // /* If we are loading from EF_ADN, specifically
    //  * specify the path as well, since, on some cards,
    //  * the fileid is not unique.
    //  */
    // if (ef == IccConstants.EF_ADN) {
    //
    //     mFh.loadEFLinearFixedAll(
    //             ef, getEFPath(ef),
    //             obtainMessage(EVENT_ADN_LOAD_ALL_DONE));
    // } else {
    //     mFh.loadEFLinearFixedAll(
    //                 ef,
    //                 obtainMessage(EVENT_ADN_LOAD_ALL_DONE));
    // }
    assert(0);
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
    // ==================before translated======================
    // mEf = ef;
    // mExtensionEF = extensionEF;
    // mRecordNumber = recordNumber;
    // mUserResponse = response;
    // mPin2 = pin2;
    //
    // if (ef == IccConstants.EF_ADN) {
    //     mFh.getEFLinearRecordSize( ef, getEFPath(ef),
    //         obtainMessage(EVENT_EF_LINEAR_RECORD_SIZE_DONE, adn));
    // } else {
    //     mFh.getEFLinearRecordSize( ef,
    //             obtainMessage(EVENT_EF_LINEAR_RECORD_SIZE_DONE, adn));
    // }
    assert(0);
    return NOERROR;
}

ECode AdnRecordLoader::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // AsyncResult ar;
    // byte data[];
    // AdnRecord adn;
    //
    // try {
    //     switch (msg.what) {
    //         case EVENT_EF_LINEAR_RECORD_SIZE_DONE:
    //             ar = (AsyncResult)(msg.obj);
    //             adn = (AdnRecord)(ar.userObj);
    //
    //             if (ar.exception != null) {
    //                 throw new RuntimeException("get EF record size failed",
    //                         ar.exception);
    //             }
    //
    //             int[] recordSize = (int[])ar.result;
    //             // recordSize is int[3] array
    //             // int[0]  is the record length
    //             // int[1]  is the total length of the EF file
    //             // int[2]  is the number of records in the EF file
    //             // So int[0] * int[2] = int[1]
    //            if (recordSize.length != 3 || mRecordNumber > recordSize[2]) {
    //                 throw new RuntimeException("get wrong EF record size format",
    //                         ar.exception);
    //             }
    //
    //             data = adn.buildAdnString(recordSize[0]);
    //
    //             if(data == null) {
    //                 throw new RuntimeException("wrong ADN format",
    //                         ar.exception);
    //             }
    //
    //             if (mEf == IccConstants.EF_ADN) {
    //                 mFh.updateEFLinearFixed(mEf, getEFPath(mEf), mRecordNumber,
    //                         data, mPin2, obtainMessage(EVENT_UPDATE_RECORD_DONE));
    //             } else {
    //                 mFh.updateEFLinearFixed(mEf, mRecordNumber,
    //                         data, mPin2, obtainMessage(EVENT_UPDATE_RECORD_DONE));
    //             }
    //
    //             mPendingExtLoads = 1;
    //
    //             break;
    //         case EVENT_UPDATE_RECORD_DONE:
    //             ar = (AsyncResult)(msg.obj);
    //             if (ar.exception != null) {
    //                 throw new RuntimeException("update EF adn record failed",
    //                         ar.exception);
    //             }
    //             mPendingExtLoads = 0;
    //             mResult = null;
    //             break;
    //         case EVENT_ADN_LOAD_DONE:
    //             ar = (AsyncResult)(msg.obj);
    //             data = (byte[])(ar.result);
    //
    //             if (ar.exception != null) {
    //                 throw new RuntimeException("load failed", ar.exception);
    //             }
    //
    //             if (VDBG) {
    //                 Rlog.d(LOGTAG,"ADN EF: 0x"
    //                     + Integer.toHexString(mEf)
    //                     + ":" + mRecordNumber
    //                     + "\n" + IccUtils.bytesToHexString(data));
    //             }
    //
    //             adn = new AdnRecord(mEf, mRecordNumber, data);
    //             mResult = adn;
    //
    //             if (adn.hasExtendedRecord()) {
    //                 // If we have a valid value in the ext record field,
    //                 // we're not done yet: we need to read the corresponding
    //                 // ext record and append it
    //
    //                 mPendingExtLoads = 1;
    //
    //                 mFh.loadEFLinearFixed(
    //                     mExtensionEF, adn.mExtRecord,
    //                     obtainMessage(EVENT_EXT_RECORD_LOAD_DONE, adn));
    //             }
    //         break;
    //
    //         case EVENT_EXT_RECORD_LOAD_DONE:
    //             ar = (AsyncResult)(msg.obj);
    //             data = (byte[])(ar.result);
    //             adn = (AdnRecord)(ar.userObj);
    //
    //             if (ar.exception != null) {
    //                 throw new RuntimeException("load failed", ar.exception);
    //             }
    //
    //             Rlog.d(LOGTAG,"ADN extension EF: 0x"
    //                 + Integer.toHexString(mExtensionEF)
    //                 + ":" + adn.mExtRecord
    //                 + "\n" + IccUtils.bytesToHexString(data));
    //
    //             adn.appendExtRecord(data);
    //
    //             mPendingExtLoads--;
    //             // result should have been set in
    //             // EVENT_ADN_LOAD_DONE or EVENT_ADN_LOAD_ALL_DONE
    //         break;
    //
    //         case EVENT_ADN_LOAD_ALL_DONE:
    //             ar = (AsyncResult)(msg.obj);
    //             ArrayList<byte[]> datas = (ArrayList<byte[]>)(ar.result);
    //
    //             if (ar.exception != null) {
    //                 throw new RuntimeException("load failed", ar.exception);
    //             }
    //
    //             mAdns = new ArrayList<AdnRecord>(datas.size());
    //             mResult = mAdns;
    //             mPendingExtLoads = 0;
    //
    //             for(int i = 0, s = datas.size() ; i < s ; i++) {
    //                 adn = new AdnRecord(mEf, 1 + i, datas.get(i));
    //                 mAdns.add(adn);
    //
    //                 if (adn.hasExtendedRecord()) {
    //                     // If we have a valid value in the ext record field,
    //                     // we're not done yet: we need to read the corresponding
    //                     // ext record and append it
    //
    //                     mPendingExtLoads++;
    //
    //                     mFh.loadEFLinearFixed(
    //                         mExtensionEF, adn.mExtRecord,
    //                         obtainMessage(EVENT_EXT_RECORD_LOAD_DONE, adn));
    //                 }
    //             }
    //         break;
    //     }
    // } catch (RuntimeException exc) {
    //     if (mUserResponse != null) {
    //         AsyncResult.forMessage(mUserResponse)
    //                         .exception = exc;
    //         mUserResponse.sendToTarget();
    //         // Loading is all or nothing--either every load succeeds
    //         // or we fail the whole thing.
    //         mUserResponse = null;
    //     }
    //     return;
    // }
    //
    // if (mUserResponse != null && mPendingExtLoads == 0) {
    //     AsyncResult.forMessage(mUserResponse).result
    //         = mResult;
    //
    //     mUserResponse.sendToTarget();
    //     mUserResponse = null;
    // }
    assert(0);
    return NOERROR;
}

String AdnRecordLoader::GetEFPath(
    /* [in] */ Int32 efid)
{
    // ==================before translated======================
    // if (efid == IccConstants.EF_ADN) {
    //     return IccConstants.MF_SIM + IccConstants.DF_TELECOM;
    // }
    //
    // return null;
    assert(0);
    return String("");
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
