#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/internal/telephony/uicc/AdnRecordCache.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                            AdnRecordCache
//=====================================================================
CAR_INTERFACE_IMPL_2(AdnRecordCache, Handler, IAdnRecordCache, IIccConstants);

const Int32 AdnRecordCache::EVENT_LOAD_ALL_ADN_LIKE_DONE;
const Int32 AdnRecordCache::EVENT_UPDATE_ADN_DONE;
const Int32 AdnRecordCache::USIM_EFANR_TAG;
const Int32 AdnRecordCache::USIM_EFEMAIL_TAG;

AdnRecordCache::AdnRecordCache()
{
}

ECode AdnRecordCache::constructor(
    /* [in] */ IIccFileHandler* fh)
{
    // ==================before translated======================
    // mFh = fh;
    // mUsimPhoneBookManager = new UsimPhoneBookManager(mFh, this);
    return NOERROR;
}

ECode AdnRecordCache::Reset()
{
    // ==================before translated======================
    // mAdnLikeFiles.clear();
    // mUsimPhoneBookManager.reset();
    //
    // clearWaiters();
    // clearUserWriters();
    assert(0);
    return NOERROR;
}

ECode AdnRecordCache::GetRecordsIfLoaded(
    /* [in] */ Int32 efid,
    /* [out] */ IArrayList/*<AdnRecord>*/** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAdnLikeFiles.get(efid);
    assert(0);
    return NOERROR;
}

ECode AdnRecordCache::ExtensionEfForEf(
    /* [in] */ Int32 efid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // switch (efid) {
    //     case EF_MBDN: return EF_EXT6;
    //     case EF_ADN: return EF_EXT1;
    //     case EF_SDN: return EF_EXT3;
    //     case EF_FDN: return EF_EXT2;
    //     case EF_MSISDN: return EF_EXT1;
    //     case EF_PBR: return 0; // The EF PBR doesn't have an extension record
    //     default: return -1;
    // }
    assert(0);
    return NOERROR;
}

ECode AdnRecordCache::UpdateAdnByIndex(
    /* [in] */ Int32 efid,
    /* [in] */ IAdnRecord* adn,
    /* [in] */ Int32 recordIndex,
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    //
    // int extensionEF = extensionEfForEf(efid);
    // if (extensionEF < 0) {
    //     sendErrorResponse(response, "EF is not known ADN-like EF:" + efid);
    //     return;
    // }
    //
    // Message pendingResponse = mUserWriteResponse.get(efid);
    // if (pendingResponse != null) {
    //     sendErrorResponse(response, "Have pending update for EF:" + efid);
    //     return;
    // }
    //
    // mUserWriteResponse.put(efid, response);
    //
    // new AdnRecordLoader(mFh).updateEF(adn, efid, extensionEF,
    //         recordIndex, pin2,
    //         obtainMessage(EVENT_UPDATE_ADN_DONE, efid, recordIndex, adn));
    assert(0);
    return NOERROR;
}

ECode AdnRecordCache::UpdateAdnBySearch(
    /* [in] */ Int32 efid,
    /* [in] */ IAdnRecord* oldAdn,
    /* [in] */ IAdnRecord* newAdn,
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    //
    // int extensionEF;
    // extensionEF = extensionEfForEf(efid);
    //
    // if (extensionEF < 0) {
    //     sendErrorResponse(response, "EF is not known ADN-like EF:" + efid);
    //     return;
    // }
    //
    // ArrayList<AdnRecord> oldAdnList = null;
    // try {
    //     if (efid == EF_PBR) {
    //         oldAdnList = mUsimPhoneBookManager.loadEfFilesFromUsim();
    //     } else {
    //         oldAdnList = getRecordsIfLoaded(efid);
    //     }
    // } catch (NullPointerException e) {
    //     // NullPointerException will be thrown occasionally when we call this method just
    //     // during phone changed to airplane mode.
    //     // Some Object used in this method will be reset, so we add protect code here to avoid
    //     // phone force close.
    //     oldAdnList = null;
    // }
    //
    // if (oldAdnList == null) {
    //     sendErrorResponse(response, "Adn list not exist for EF:" + efid);
    //     return;
    // }
    //
    // int index = -1;
    // int count = 1;
    // int prePbrIndex = -2;
    // int anrNum = 0;
    // int emailNum = 0;
    // for (Iterator<AdnRecord> it = oldAdnList.iterator(); it.hasNext();) {
    //     AdnRecord nextAdnRecord = it.next();
    //     boolean isEmailOrAnrIsFull = false;
    //     if (efid == EF_PBR) {
    //         // There may more than one PBR files in the USIM card, if the current PBR file can
    //         // not save the new AdnRecord which contain anr or email, try save it into next PBR
    //         // file.
    //         int pbrIndex = mUsimPhoneBookManager.getPbrIndexBy(count - 1);
    //         if (pbrIndex != prePbrIndex) {
    //             // For a specific pbrIndex, the anrNum and emailNum is fixed.
    //             anrNum = mUsimPhoneBookManager.getEmptyAnrNum_Pbrindex(pbrIndex);
    //             emailNum = mUsimPhoneBookManager.getEmptyEmailNum_Pbrindex(pbrIndex);
    //             prePbrIndex = pbrIndex;
    //             Log.d("AdnRecordCache", "updateAdnBySearch, pbrIndex: " + pbrIndex +
    //                     " anrNum:" + anrNum + " emailNum:" + emailNum);
    //         }
    //         if ((anrNum == 0 &&
    //                 (oldAdn.getAdditionalNumbers() == null &&
    //                  newAdn.getAdditionalNumbers() != null)) ||
    //             (emailNum == 0 &&
    //                 (oldAdn.getEmails() == null &&
    //                  newAdn.getEmails() != null))) {
    //             isEmailOrAnrIsFull = true;
    //         }
    //     }
    //
    //     if (!isEmailOrAnrIsFull && oldAdn.isEqual(nextAdnRecord)) {
    //         index = count;
    //         break;
    //     }
    //     count++;
    // }
    //
    // Log.d("AdnRecordCache", "updateAdnBySearch, update oldADN:" + oldAdn.toString() +
    //         ", newAdn:" + newAdn.toString() + ",index :" + index);
    //
    // if (index == -1) {
    //     sendErrorResponse(response, "Adn record don't exist for " + oldAdn);
    //     return;
    // }
    //
    // if (efid == EF_PBR) {
    //     AdnRecord foundAdn = oldAdnList.get(index-1);
    //     newAdn.mEfid = foundAdn.mEfid;
    //     newAdn.mExtRecord = foundAdn.mExtRecord;
    //     newAdn.mRecordNumber = foundAdn.mRecordNumber;
    //     // make sure the sequence is same with foundAdn
    //     oldAdn.setAdditionalNumbers(foundAdn.getAdditionalNumbers());
    //     oldAdn.setEmails(foundAdn.getEmails());
    //     newAdn.updateAnrEmailArray(oldAdn,
    //             mUsimPhoneBookManager.getEmailFilesCountEachAdn(),
    //             mUsimPhoneBookManager.getAnrFilesCountEachAdn());
    // }
    //
    // Message pendingResponse = mUserWriteResponse.get(efid);
    //
    // if (pendingResponse != null) {
    //     sendErrorResponse(response, "Have pending update for EF:" + efid);
    //     return;
    // }
    //
    // if (efid == EF_PBR) {
    //     updateEmailAndAnr(efid, oldAdn, newAdn, index, pin2, response);
    // } else {
    //     mUserWriteResponse.put(efid, response);
    //     new AdnRecordLoader(mFh).updateEF(newAdn, efid, extensionEF,
    //             index, pin2,
    //             obtainMessage(EVENT_UPDATE_ADN_DONE, efid, index, newAdn));
    // }
    assert(0);
    return NOERROR;
}

ECode AdnRecordCache::RequestLoadAllAdnLike(
    /* [in] */ Int32 efid,
    /* [in] */ Int32 extensionEf,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // ArrayList<Message> waiters;
    // ArrayList<AdnRecord> result;
    //
    // if (efid == EF_PBR) {
    //     result = mUsimPhoneBookManager.loadEfFilesFromUsim();
    // } else {
    //     result = getRecordsIfLoaded(efid);
    // }
    //
    // // Have we already loaded this efid?
    // if (result != null) {
    //     if (response != null) {
    //         AsyncResult.forMessage(response).result = result;
    //         response.sendToTarget();
    //     }
    //
    //     return;
    // }
    //
    // // Have we already *started* loading this efid?
    //
    // waiters = mAdnLikeWaiters.get(efid);
    //
    // if (waiters != null) {
    //     // There's a pending request for this EF already
    //     // just add ourselves to it
    //
    //     waiters.add(response);
    //     return;
    // }
    //
    // // Start loading efid
    //
    // waiters = new ArrayList<Message>();
    // waiters.add(response);
    //
    // mAdnLikeWaiters.put(efid, waiters);
    //
    //
    // if (extensionEf < 0) {
    //     // respond with error if not known ADN-like record
    //
    //     if (response != null) {
    //         AsyncResult.forMessage(response).exception
    //             = new RuntimeException("EF is not known ADN-like EF:" + efid);
    //         response.sendToTarget();
    //     }
    //
    //     return;
    // }
    //
    // new AdnRecordLoader(mFh).loadAllFromEF(efid, extensionEf,
    //     obtainMessage(EVENT_LOAD_ALL_ADN_LIKE_DONE, efid, 0));
    assert(0);
    return NOERROR;
}

ECode AdnRecordCache::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // AsyncResult ar;
    // int efid;
    //
    // switch(msg.what) {
    //     case EVENT_LOAD_ALL_ADN_LIKE_DONE:
    //         /* arg1 is efid, obj.result is ArrayList<AdnRecord>*/
    //         ar = (AsyncResult) msg.obj;
    //         efid = msg.arg1;
    //         ArrayList<Message> waiters;
    //
    //         waiters = mAdnLikeWaiters.get(efid);
    //         mAdnLikeWaiters.delete(efid);
    //
    //         if (ar.exception == null) {
    //             mAdnLikeFiles.put(efid, (ArrayList<AdnRecord>) ar.result);
    //         }
    //         notifyWaiters(waiters, ar);
    //         if (mAdnLikeFiles.get(EF_ADN) != null) {
    //             setAdnCount(mAdnLikeFiles.get(EF_ADN).size());
    //         }
    //         break;
    //     case EVENT_UPDATE_ADN_DONE:
    //         ar = (AsyncResult)msg.obj;
    //         efid = msg.arg1;
    //         int index = msg.arg2;
    //         AdnRecord adn = (AdnRecord) (ar.userObj);
    //
    //         if (ar.exception == null) {
    //             if (mAdnLikeFiles.get(efid) != null) {
    //                 mAdnLikeFiles.get(efid).set(index - 1, adn);
    //             }
    //             if (efid == EF_PBR) {
    //                 mUsimPhoneBookManager.loadEfFilesFromUsim().set(index - 1, adn);
    //             }
    //         }
    //
    //         Message response = mUserWriteResponse.get(efid);
    //         mUserWriteResponse.delete(efid);
    //
    //         // response may be cleared when simrecord is reset,
    //         // so we should check if it is null.
    //         if (response != null) {
    //             AsyncResult.forMessage(response, null, ar.exception);
    //             response.sendToTarget();
    //         }
    //         break;
    // }
    assert(0);
    return NOERROR;
}

ECode AdnRecordCache::UpdateUsimAdnByIndex(
    /* [in] */ Int32 efid,
    /* [in] */ IAdnRecord* newAdn,
    /* [in] */ Int32 recordIndex,
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    //
    // int extensionEF;
    // extensionEF = extensionEfForEf(efid);
    // if (extensionEF < 0) {
    //     sendErrorResponse(response, "EF is not known ADN-like EF:" + efid);
    //     return;
    // }
    //
    // ArrayList<AdnRecord> oldAdnList = null;
    // try {
    //     if (efid == EF_PBR) {
    //         oldAdnList = mUsimPhoneBookManager.loadEfFilesFromUsim();
    //     } else {
    //         oldAdnList = getRecordsIfLoaded(efid);
    //     }
    // } catch (NullPointerException e) {
    //     // NullPointerException will be thrown occasionally when we call this method just
    //     // during phone changed to airplane mode.
    //     // Some Object used in this method will be reset, so we add protect code here to avoid
    //     // phone force close.
    //     oldAdnList = null;
    // }
    //
    // if (oldAdnList == null) {
    //     sendErrorResponse(response, "Adn list not exist for EF:" + efid);
    //     return;
    // }
    //
    // int index = recordIndex;
    //
    // if (efid == EF_PBR) {
    //     AdnRecord foundAdn = oldAdnList.get(index - 1);
    //     newAdn.mEfid = foundAdn.mEfid;
    //     newAdn.mExtRecord = foundAdn.mExtRecord;
    //     newAdn.mRecordNumber = foundAdn.mRecordNumber;
    // }
    //
    // Message pendingResponse = mUserWriteResponse.get(efid);
    //
    // if (pendingResponse != null) {
    //     sendErrorResponse(response, "Have pending update for EF:" + efid);
    //     return;
    // }
    //
    // if (efid == EF_PBR) {
    //     updateEmailAndAnr(efid, oldAdnList.get(index - 1), newAdn, index, pin2, response);
    // } else {
    //     mUserWriteResponse.put(efid, response);
    //     new AdnRecordLoader(mFh).updateEF(newAdn, efid, extensionEF, index, pin2,
    //             obtainMessage(EVENT_UPDATE_ADN_DONE, efid, index, newAdn));
    // }
    assert(0);
    return NOERROR;
}

ECode AdnRecordCache::GetAnrCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUsimPhoneBookManager.getAnrCount();
    assert(0);
    return NOERROR;
}

ECode AdnRecordCache::GetEmailCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUsimPhoneBookManager.getEmailCount();
    assert(0);
    return NOERROR;
}

ECode AdnRecordCache::GetSpareAnrCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUsimPhoneBookManager.getSpareAnrCount();
    assert(0);
    return NOERROR;
}

ECode AdnRecordCache::GetSpareEmailCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUsimPhoneBookManager.getSpareEmailCount();
    assert(0);
    return NOERROR;
}

ECode AdnRecordCache::GetAdnCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAdncountofIcc;
    assert(0);
    return NOERROR;
}

ECode AdnRecordCache::SetAdnCount(
    /* [in] */ Int32 count)
{
    // ==================before translated======================
    // mAdncountofIcc = count;
    assert(0);
    return NOERROR;
}

ECode AdnRecordCache::GetUsimAdnCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUsimPhoneBookManager.getUsimAdnCount();
    assert(0);
    return NOERROR;
}

void AdnRecordCache::ClearWaiters()
{
    // ==================before translated======================
    // int size = mAdnLikeWaiters.size();
    // for (int i = 0; i < size; i++) {
    //     ArrayList<Message> waiters = mAdnLikeWaiters.valueAt(i);
    //     AsyncResult ar = new AsyncResult(null, null, new RuntimeException("AdnCache reset"));
    //     notifyWaiters(waiters, ar);
    // }
    // mAdnLikeWaiters.clear();
    assert(0);
}

void AdnRecordCache::ClearUserWriters()
{
    // ==================before translated======================
    // int size = mUserWriteResponse.size();
    // for (int i = 0; i < size; i++) {
    //     sendErrorResponse(mUserWriteResponse.valueAt(i), "AdnCace reset");
    // }
    // mUserWriteResponse.clear();
    assert(0);
}

void AdnRecordCache::SendErrorResponse(
    /* [in] */ IMessage* response,
    /* [in] */ const String& errString)
{
    // ==================before translated======================
    // if (response != null) {
    //     Exception e = new RuntimeException(errString);
    //     AsyncResult.forMessage(response).exception = e;
    //     response.sendToTarget();
    // }
    assert(0);
}

void AdnRecordCache::NotifyWaiters(
    /* [in] */ IArrayList/*<IMessage>*/* waiters,
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    //
    // if (waiters == null) {
    //     return;
    // }
    //
    // for (int i = 0, s = waiters.size() ; i < s ; i++) {
    //     Message waiter = waiters.get(i);
    //
    //     AsyncResult.forMessage(waiter, ar.result, ar.exception);
    //     waiter.sendToTarget();
    // }
    assert(0);
}

void AdnRecordCache::UpdateEmailAndAnr(
    /* [in] */ Int32 efid,
    /* [in] */ IAdnRecord* oldAdn,
    /* [in] */ IAdnRecord* newAdn,
    /* [in] */ Int32 index,
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // int extensionEF;
    // extensionEF = extensionEfForEf(newAdn.mEfid);
    // boolean success = false;
    // success = updateUsimRecord(oldAdn, newAdn, index, USIM_EFEMAIL_TAG);
    //
    // if (success) {
    //     success = updateUsimRecord(oldAdn, newAdn, index, USIM_EFANR_TAG);
    // } else {
    //     sendErrorResponse(response, "update email failed");
    //     return;
    // }
    // if (success) {
    //     mUserWriteResponse.put(efid, response);
    //     new AdnRecordLoader(mFh).updateEF(newAdn, newAdn.mEfid, extensionEF,
    //             newAdn.mRecordNumber, pin2,
    //             obtainMessage(EVENT_UPDATE_ADN_DONE, efid, index, newAdn));
    // } else {
    //     sendErrorResponse(response, "update anr failed");
    //     return;
    // }
    assert(0);
}

Boolean AdnRecordCache::UpdateAnrEmailFile(
    /* [in] */ const String& oldRecord,
    /* [in] */ const String& newRecord,
    /* [in] */ Int32 index,
    /* [in] */ Int32 tag,
    /* [in] */ Int32 efidIndex)
{
    // ==================before translated======================
    // boolean success = true;
    // try {
    //     switch (tag) {
    //         case USIM_EFEMAIL_TAG:
    //             success = mUsimPhoneBookManager
    //                     .updateEmailFile(index, oldRecord, newRecord, efidIndex);
    //             break;
    //         case USIM_EFANR_TAG:
    //             success = mUsimPhoneBookManager
    //                     .updateAnrFile(index, oldRecord, newRecord, efidIndex);
    //             break;
    //         default:
    //             success = false;
    //     }
    // } catch (RuntimeException e) {
    //     success = false;
    //     Log.e("AdnRecordCache", "update usim record failed", e);
    // }
    //
    // return success;
    assert(0);
    return FALSE;
}

Boolean AdnRecordCache::UpdateUsimRecord(
    /* [in] */ IAdnRecord* oldAdn,
    /* [in] */ IAdnRecord* newAdn,
    /* [in] */ Int32 index,
    /* [in] */ Int32 tag)
{
    // ==================before translated======================
    // String[] oldRecords = null;
    // String[] newRecords = null;
    // String oldRecord = null;
    // String newRecord = null;
    // boolean success = true;
    // // currently we only support one email records
    // switch (tag) {
    //     case USIM_EFEMAIL_TAG:
    //         oldRecords = oldAdn.getEmails();
    //         newRecords = newAdn.getEmails();
    //         break;
    //     case USIM_EFANR_TAG:
    //         oldRecords = oldAdn.getAdditionalNumbers();
    //         newRecords = newAdn.getAdditionalNumbers();
    //         break;
    //     default:
    //         return false;
    // }
    //
    // if (null == oldRecords && null == newRecords) {
    //     // UI send empty string, no need to update
    //     Log.e("AdnRecordCache", "Both old and new EMAIL/ANR are null");
    //     return true;
    // }
    //
    // // insert scenario
    // if (null == oldRecords && null != newRecords) {
    //     for (int i = 0; i < newRecords.length; i++) {
    //         if (!TextUtils.isEmpty(newRecords[i])) {
    //             success &= updateAnrEmailFile(null, newRecords[i], index, tag, i);
    //         }
    //     }
    // // delete scenario
    // } else if (null != oldRecords && null == newRecords) {
    //     for (int i = 0; i < oldRecords.length; i++) {
    //         if (!TextUtils.isEmpty(oldRecords[i])) {
    //             success &= updateAnrEmailFile(oldRecords[i], null, index, tag, i);
    //         }
    //     }
    // // update scenario
    // } else {
    //     int maxLen = (oldRecords.length > newRecords.length) ?
    //                     oldRecords.length : newRecords.length;
    //     for (int i = 0; i < maxLen; i++) {
    //         oldRecord = (i >= oldRecords.length) ? null : oldRecords[i];
    //         newRecord = (i >= newRecords.length) ? null : newRecords[i];
    //
    //         if ((TextUtils.isEmpty(oldRecord) && TextUtils.isEmpty(newRecord)) ||
    //             (oldRecord != null && newRecord != null && oldRecord.equals(newRecord))) {
    //             continue;
    //         } else {
    //             success &= updateAnrEmailFile(oldRecord, newRecord, index, tag, i);
    //         }
    //     }
    // }
    //
    // return success;
    assert(0);
    return FALSE;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
