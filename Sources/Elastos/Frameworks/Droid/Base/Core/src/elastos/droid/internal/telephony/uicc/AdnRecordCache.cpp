#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/uicc/AdnRecordCache.h"
#include "elastos/droid/internal/telephony/uicc/CAdnRecordLoader.h"
#include "elastos/droid/internal/telephony/uicc/AdnRecord.h"
#include "elastos/droid/internal/telephony/gsm/CUsimPhoneBookManager.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/text/TextUtils.h"

#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::Gsm::CUsimPhoneBookManager;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::StringUtils;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

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
    : mAdncountofIcc(0)
{
    CSparseArray::New((ISparseArray**)&mAdnLikeFiles);

    CSparseArray::New((ISparseArray**)&mAdnLikeWaiters);

    CSparseArray::New((ISparseArray**)&mUserWriteResponse);
}

ECode AdnRecordCache::constructor(
    /* [in] */ IIccFileHandler* fh)
{
    Handler::constructor();
    mFh = fh;
    CUsimPhoneBookManager::New(mFh, this, (IUsimPhoneBookManager**)&mUsimPhoneBookManager);
    return NOERROR;
}

ECode AdnRecordCache::Reset()
{
    mAdnLikeFiles->Clear();
    mUsimPhoneBookManager->Reset();

    ClearWaiters();
    ClearUserWriters();
    return NOERROR;
}

ECode AdnRecordCache::GetRecordsIfLoaded(
    /* [in] */ Int32 efid,
    /* [out] */ IArrayList/*<AdnRecord>*/** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mAdnLikeFiles->Get(efid, (IInterface**)&p);
    *result = IArrayList::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode AdnRecordCache::ExtensionEfForEf(
    /* [in] */ Int32 efid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    switch (efid) {
        case EF_MBDN: {
            *result = EF_EXT6;
            return NOERROR;
        }
        case EF_ADN: {
            *result = EF_EXT1;
            return NOERROR;
        }
        case EF_SDN: {
            *result = EF_EXT3;
            return NOERROR;
        }
        case EF_FDN: {
            *result = EF_EXT2;
            return NOERROR;
        }
        case EF_MSISDN: {
            *result = EF_EXT1;
            return NOERROR;
        }
        case EF_PBR: {
            *result = 0; // The EF PBR doesn't have an extension record
            return NOERROR;
        }
        default: {
            *result = -1;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode AdnRecordCache::UpdateAdnByIndex(
    /* [in] */ Int32 efid,
    /* [in] */ IAdnRecord* adn,
    /* [in] */ Int32 recordIndex,
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* response)
{
    Int32 extensionEF = 0;
    ExtensionEfForEf(efid, &extensionEF);
    if (extensionEF < 0) {
        SendErrorResponse(response, String("EF is not known ADN-like EF:") + StringUtils::ToString(efid));
        return NOERROR;
    }

    AutoPtr<IInterface> p;
    mUserWriteResponse->Get(efid, (IInterface**)&p);
    AutoPtr<IMessage> pendingResponse = IMessage::Probe(p);
    if (pendingResponse != NULL) {
        SendErrorResponse(response, String("Have pending update for EF:") + StringUtils::ToString(efid));
        return NOERROR;
    }

    mUserWriteResponse->Put(efid, response);

    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_UPDATE_ADN_DONE, efid, recordIndex, adn, (IMessage**)&msg);
    AutoPtr<IAdnRecordLoader> loader;
    CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
    loader->UpdateEF(adn, efid, extensionEF,
            recordIndex, pin2,
            msg);
    return NOERROR;
}

ECode AdnRecordCache::UpdateAdnBySearch(
    /* [in] */ Int32 efid,
    /* [in] */ IAdnRecord* oldAdn,
    /* [in] */ IAdnRecord* newAdn,
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* response)
{
    Int32 extensionEF = 0;
    ExtensionEfForEf(efid, &extensionEF);

    if (extensionEF < 0) {
        SendErrorResponse(response, String("EF is not known ADN-like EF:") + StringUtils::ToString(efid));
        return NOERROR;
    }

    AutoPtr<IArrayList> oldAdnList;
    // try {
        if (efid == EF_PBR) {
            mUsimPhoneBookManager->LoadEfFilesFromUsim((IArrayList**)&oldAdnList);
        }
        else {
            GetRecordsIfLoaded(efid, (IArrayList**)&oldAdnList);
        }
    // } catch (NullPointerException e) {
    //     // NullPointerException will be thrown occasionally when we call this method just
    //     // during phone changed to airplane mode.
    //     // Some Object used in this method will be reset, so we add protect code here to avoid
    //     // phone force close.
    //     oldAdnList = NULL;
    // }

    if (oldAdnList == NULL) {
        SendErrorResponse(response, String("Adn list not exist for EF:") + StringUtils::ToString(efid));
        return NOERROR;
    }

    Int32 index = -1;
    Int32 count = 1;
    Int32 prePbrIndex = -2;
    Int32 anrNum = 0;
    Int32 emailNum = 0;
    AutoPtr<IIterator> it;
    oldAdnList->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    for (;(it->HasNext(&bHasNext), bHasNext);) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IAdnRecord> nextAdnRecord = IAdnRecord::Probe(p);
        Boolean isEmailOrAnrIsFull = FALSE;
        if (efid == EF_PBR) {
            // There may more than one PBR files in the USIM card, if the current PBR file can
            // not save the new AdnRecord which contain anr or email, try save it into next PBR
            // file.
            Int32 pbrIndex = 0;
            mUsimPhoneBookManager->GetPbrIndexBy(count - 1, &pbrIndex);
            if (pbrIndex != prePbrIndex) {
                // For a specific pbrIndex, the anrNum and emailNum is fixed.
                mUsimPhoneBookManager->GetEmptyAnrNum_Pbrindex(pbrIndex, &anrNum);
                mUsimPhoneBookManager->GetEmptyEmailNum_Pbrindex(pbrIndex, &emailNum);
                prePbrIndex = pbrIndex;
                Logger::D(String("AdnRecordCache"), String("updateAdnBySearch, pbrIndex: ")
                        + StringUtils::ToString(pbrIndex) + String(" anrNum:") + StringUtils::ToString(anrNum)
                        + String(" emailNum:") + StringUtils::ToString(emailNum));
            }
            AutoPtr<ArrayOf<String> > oldNumbers;
            oldAdn->GetAdditionalNumbers((ArrayOf<String>**)&oldNumbers);
            AutoPtr<ArrayOf<String> > newNumbers;
            newAdn->GetAdditionalNumbers((ArrayOf<String>**)&newNumbers);
            AutoPtr<ArrayOf<String> > oldMails;
            oldAdn->GetEmails((ArrayOf<String>**)&oldMails);
            AutoPtr<ArrayOf<String> > newMails;
            newAdn->GetEmails((ArrayOf<String>**)&newMails);
            if ((anrNum == 0 &&
                    (oldNumbers == NULL &&
                     newNumbers != NULL)) ||
                (emailNum == 0 &&
                    (oldMails == NULL &&
                     newMails != NULL))) {
                isEmailOrAnrIsFull = TRUE;
            }
        }

        if (!isEmailOrAnrIsFull && Object::Equals(oldAdn, nextAdnRecord)) {
            index = count;
            break;
        }
        count++;
    }

    Logger::D("AdnRecordCache", "updateAdnBySearch, update oldADN:%s, newAdn:%s,index :%d",
            TO_CSTR(oldAdn), TO_CSTR(newAdn), index);

    if (index == -1) {
        SendErrorResponse(response, String("Adn record don't exist for ") + TO_CSTR(oldAdn));
        return NOERROR;
    }

    if (efid == EF_PBR) {
        AutoPtr<IInterface> p;
        oldAdnList->Get(index - 1, (IInterface**)&p);
        AutoPtr<AdnRecord> foundAdn = (AdnRecord*)IAdnRecord::Probe(p);
        AutoPtr<AdnRecord> _newAdn = (AdnRecord*)newAdn;
        _newAdn->mEfid = foundAdn->mEfid;
        _newAdn->mExtRecord = foundAdn->mExtRecord;
        _newAdn->mRecordNumber = foundAdn->mRecordNumber;
        // make sure the sequence is same with foundAdn
        AutoPtr<ArrayOf<String> > numbers;
        foundAdn->GetAdditionalNumbers((ArrayOf<String>**)&numbers);
        oldAdn->SetAdditionalNumbers(numbers);
        AutoPtr<ArrayOf<String> > emails;
        foundAdn->GetEmails((ArrayOf<String>**)&emails);
        oldAdn->SetEmails(emails);
        Int32 emailCount = 0, anrCount = 0;
        mUsimPhoneBookManager->GetEmailFilesCountEachAdn(&emailCount);
        mUsimPhoneBookManager->GetAnrFilesCountEachAdn(&anrCount);
        newAdn->UpdateAnrEmailArray(oldAdn,
                emailCount,
                anrCount);
    }

    AutoPtr<IInterface> _pendingResponse;
    mUserWriteResponse->Get(efid, (IInterface**)&_pendingResponse);
    AutoPtr<IMessage> pendingResponse = IMessage::Probe(_pendingResponse);

    if (pendingResponse != NULL) {
        SendErrorResponse(response, String("Have pending update for EF:") + StringUtils::ToString(efid));
        return NOERROR;
    }

    if (efid == EF_PBR) {
        UpdateEmailAndAnr(efid, oldAdn, newAdn, index, pin2, response);
    }
    else {
        mUserWriteResponse->Put(efid, response);
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_UPDATE_ADN_DONE, efid, index, newAdn, (IMessage**)&msg);
        AutoPtr<IAdnRecordLoader> loader;
        CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
        loader->UpdateEF(newAdn, efid, extensionEF,
                index, pin2,
                msg);
    }
    return NOERROR;
}

ECode AdnRecordCache::RequestLoadAllAdnLike(
    /* [in] */ Int32 efid,
    /* [in] */ Int32 extensionEf,
    /* [in] */ IMessage* response)
{
    AutoPtr<IArrayList> waiters;
    AutoPtr<IArrayList> result;

    if (efid == EF_PBR) {
        mUsimPhoneBookManager->LoadEfFilesFromUsim((IArrayList**)&result);
    }
    else {
        GetRecordsIfLoaded(efid, (IArrayList**)&result);
    }

    // Have we already loaded this efid?
    if (result != NULL) {
        if (response != NULL) {
            AutoPtr<AsyncResult> p = AsyncResult::ForMessage(response);
            p->mResult = result;
            response->SendToTarget();
        }

        return NOERROR;
    }

    // Have we already *started* loading this efid?
    AutoPtr<IInterface> _waiters;
    mAdnLikeWaiters->Get(efid, (IInterface**)&_waiters);
    waiters = IArrayList::Probe(_waiters);

    if (waiters != NULL) {
        // There's a pending request for this EF already
        // just add ourselves to it

        waiters->Add(response);
        return NOERROR;
    }

    // Start loading efid

    CArrayList::New((IArrayList**)&waiters);
    waiters->Add(response);

    mAdnLikeWaiters->Put(efid, waiters);


    if (extensionEf < 0) {
        // respond with error if not known ADN-like record

        if (response != NULL) {
            AutoPtr<AsyncResult> p = AsyncResult::ForMessage(response);
            assert(0 && "TODO");
            // p->mException
            //     = new RuntimeException(String("EF is not known ADN-like EF:") + StringUtils::ToString(efid));
            response->SendToTarget();
        }

        return NOERROR;
    }

    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_LOAD_ALL_ADN_LIKE_DONE, efid, 0, (IMessage**)&msg);
    AutoPtr<IAdnRecordLoader> loader;
    CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
    loader->LoadAllFromEF(efid, extensionEf, msg);
    return NOERROR;
}

ECode AdnRecordCache::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;
    Int32 efid = 0;

    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    Int32 arg1 = 0;
    msg->GetArg1(&arg1);
    Int32 arg2 = 0;
    msg->GetArg2(&arg2);
    switch(what) {
        case EVENT_LOAD_ALL_ADN_LIKE_DONE: {
            /* arg1 is efid, obj.result is ArrayList<AdnRecord>*/
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            efid = arg1;
            AutoPtr<IInterface> _waiters;
            mAdnLikeWaiters->Get(efid, (IInterface**)&_waiters);
            AutoPtr<IArrayList> waiters = IArrayList::Probe(_waiters);
            mAdnLikeWaiters->Delete(efid);

            if (ar->mException == NULL) {
                mAdnLikeFiles->Put(efid, ar->mResult);
            }
            NotifyWaiters(waiters, ar);
            AutoPtr<IInterface> p;
            mAdnLikeFiles->Get(EF_ADN, (IInterface**)&p);
            if (p != NULL) {
                Int32 size = 0;
                IArrayList::Probe(p)->GetSize(&size);
                SetAdnCount(size);
            }
            break;
        }
        case EVENT_UPDATE_ADN_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            efid = arg1;
            Int32 index = arg2;
            AutoPtr<IAdnRecord> adn = IAdnRecord::Probe(ar->mUserObj);

            if (ar->mException == NULL) {
                AutoPtr<IInterface> p;
                mAdnLikeFiles->Get(efid, (IInterface**)&p);
                if (p != NULL) {
                    IArrayList::Probe(p)->Set(index - 1, adn);
                }
                if (efid == EF_PBR) {
                    AutoPtr<IArrayList> al;
                    mUsimPhoneBookManager->LoadEfFilesFromUsim((IArrayList**)&al);
                    al->Set(index - 1, adn);
                }
            }

            AutoPtr<IInterface> _response;
            mUserWriteResponse->Get(efid, (IInterface**)&_response);
            AutoPtr<IMessage> response = IMessage::Probe(_response);
            mUserWriteResponse->Delete(efid);

            // response may be cleared when simrecord is reset,
            // so we should check if it is NULL.
            if (response != NULL) {
                AsyncResult::ForMessage(response, NULL, ar->mException);
                response->SendToTarget();
            }
            break;
        }
    }
    return NOERROR;
}

ECode AdnRecordCache::UpdateUsimAdnByIndex(
    /* [in] */ Int32 efid,
    /* [in] */ IAdnRecord* newAdn,
    /* [in] */ Int32 recordIndex,
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* response)
{
    Int32 extensionEF = 0;
    ExtensionEfForEf(efid, &extensionEF);
    if (extensionEF < 0) {
        SendErrorResponse(response, String("EF is not known ADN-like EF:") + StringUtils::ToString(efid));
        return NOERROR;
    }

    AutoPtr<IArrayList> oldAdnList;
    // try {
        if (efid == EF_PBR) {
            mUsimPhoneBookManager->LoadEfFilesFromUsim((IArrayList**)&oldAdnList);
        }
        else {
            GetRecordsIfLoaded(efid, (IArrayList**)&oldAdnList);
        }
    // } catch (NullPointerException e) {
    //     // NullPointerException will be thrown occasionally when we call this method just
    //     // during phone changed to airplane mode.
    //     // Some Object used in this method will be reset, so we add protect code here to avoid
    //     // phone force close.
    //     oldAdnList = NULL;
    // }

    if (oldAdnList == NULL) {
        SendErrorResponse(response, String("Adn list not exist for EF:") + StringUtils::ToString(efid));
        return NOERROR;
    }

    Int32 index = recordIndex;

    if (efid == EF_PBR) {
        AutoPtr<IInterface> p;
        oldAdnList->Get(index - 1, (IInterface**)&p);
        AutoPtr<AdnRecord> foundAdn = (AdnRecord*)IAdnRecord::Probe(p);
        AutoPtr<AdnRecord> _newAdn = (AdnRecord*)newAdn;
        _newAdn->mEfid = foundAdn->mEfid;
        _newAdn->mExtRecord = foundAdn->mExtRecord;
        _newAdn->mRecordNumber = foundAdn->mRecordNumber;
    }

    AutoPtr<IInterface> _pendingResponse;
    mUserWriteResponse->Get(efid, (IInterface**)&_pendingResponse);
    AutoPtr<IMessage> pendingResponse = IMessage::Probe(_pendingResponse);

    if (pendingResponse != NULL) {
        SendErrorResponse(response, String("Have pending update for EF:") + StringUtils::ToString(efid));
        return NOERROR;
    }

    if (efid == EF_PBR) {
        AutoPtr<IInterface> p;
        oldAdnList->Get(index - 1, (IInterface**)&p);
        UpdateEmailAndAnr(efid, IAdnRecord::Probe(p), newAdn, index, pin2, response);
    }
    else {
        mUserWriteResponse->Put(efid, response);
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_UPDATE_ADN_DONE, efid, index, newAdn, (IMessage**)&msg);
        AutoPtr<IAdnRecordLoader> loader;
        CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
        loader->UpdateEF(newAdn, efid, extensionEF, index, pin2, msg);
    }
    return NOERROR;
}

ECode AdnRecordCache::GetAnrCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mUsimPhoneBookManager->GetAnrCount(result);
}

ECode AdnRecordCache::GetEmailCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mUsimPhoneBookManager->GetEmailCount(result);
}

ECode AdnRecordCache::GetSpareAnrCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mUsimPhoneBookManager->GetSpareAnrCount(result);
}

ECode AdnRecordCache::GetSpareEmailCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mUsimPhoneBookManager->GetSpareEmailCount(result);
}

ECode AdnRecordCache::GetAdnCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAdncountofIcc;
    return NOERROR;
}

ECode AdnRecordCache::SetAdnCount(
    /* [in] */ Int32 count)
{
    mAdncountofIcc = count;
    return NOERROR;
}

ECode AdnRecordCache::GetUsimAdnCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mUsimPhoneBookManager->GetUsimAdnCount(result);
}

void AdnRecordCache::ClearWaiters()
{
    Int32 size = 0;
    mAdnLikeWaiters->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> p;
        mAdnLikeWaiters->ValueAt(i, (IInterface**)&p);
        AutoPtr<IArrayList> waiters = IArrayList::Probe(p);
        assert(0 && "TODO");
        AutoPtr<AsyncResult> ar;// = new AsyncResult(NULL, NULL, new RuntimeException("AdnCache reset"));
        NotifyWaiters(waiters, ar);
    }
    mAdnLikeWaiters->Clear();
}

void AdnRecordCache::ClearUserWriters()
{
    Int32 size = 0;
    mUserWriteResponse->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> p;
        mUserWriteResponse->ValueAt(i, (IInterface**)&p);
        SendErrorResponse(IMessage::Probe(p), String("AdnCace reset"));
    }
    mUserWriteResponse->Clear();
}

void AdnRecordCache::SendErrorResponse(
    /* [in] */ IMessage* response,
    /* [in] */ const String& errString)
{
    if (response != NULL) {
        assert(0 && "TODO");
        // Exception e = new RuntimeException(errString);
        // AsyncResult::ForMessage(response).exception = e;
        response->SendToTarget();
    }
}

void AdnRecordCache::NotifyWaiters(
    /* [in] */ IArrayList/*<IMessage>*/* waiters,
    /* [in] */ AsyncResult* ar)
{
    if (waiters == NULL) {
        return;
    }

    Int32 s = 0;
    waiters->GetSize(&s);
    for (Int32 i = 0; i < s; i++) {
        AutoPtr<IInterface> p;
        waiters->Get(i, (IInterface**)&p);
        AutoPtr<IMessage> waiter = IMessage::Probe(p);

        AsyncResult::ForMessage(waiter, ar->mResult, ar->mException);
        waiter->SendToTarget();
    }
}

void AdnRecordCache::UpdateEmailAndAnr(
    /* [in] */ Int32 efid,
    /* [in] */ IAdnRecord* oldAdn,
    /* [in] */ IAdnRecord* newAdn,
    /* [in] */ Int32 index,
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* response)
{
    AutoPtr<AdnRecord> _newAdn = (AdnRecord*)newAdn;
    Int32 extensionEF = 0;
    ExtensionEfForEf(_newAdn->mEfid, &extensionEF);
    Boolean success = FALSE;
    success = UpdateUsimRecord(oldAdn, newAdn, index, USIM_EFEMAIL_TAG);

    if (success) {
        success = UpdateUsimRecord(oldAdn, newAdn, index, USIM_EFANR_TAG);
    }
    else {
        SendErrorResponse(response, String("update email failed"));
        return;
    }
    if (success) {
        mUserWriteResponse->Put(efid, response);
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_UPDATE_ADN_DONE, efid, index, newAdn, (IMessage**)&msg);
        AutoPtr<IAdnRecordLoader> loader;
        CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
        loader->UpdateEF(newAdn, _newAdn->mEfid, extensionEF,
                _newAdn->mRecordNumber, pin2,
                msg);
    }
    else {
        SendErrorResponse(response, String("update anr failed"));
        return;
    }
}

Boolean AdnRecordCache::UpdateAnrEmailFile(
    /* [in] */ const String& oldRecord,
    /* [in] */ const String& newRecord,
    /* [in] */ Int32 index,
    /* [in] */ Int32 tag,
    /* [in] */ Int32 efidIndex)
{
    Boolean success = TRUE;
    // try {
        switch (tag) {
            case USIM_EFEMAIL_TAG:
                mUsimPhoneBookManager
                        ->UpdateEmailFile(index, oldRecord, newRecord, efidIndex, &success);
                break;
            case USIM_EFANR_TAG:
                mUsimPhoneBookManager
                        ->UpdateAnrFile(index, oldRecord, newRecord, efidIndex, &success);
                break;
            default:
                success = FALSE;
        }
    // } catch (RuntimeException e) {
    //     success = false;
    //     Log.e("AdnRecordCache", "update usim record failed", e);
    // }

    return success;
}

Boolean AdnRecordCache::UpdateUsimRecord(
    /* [in] */ IAdnRecord* oldAdn,
    /* [in] */ IAdnRecord* newAdn,
    /* [in] */ Int32 index,
    /* [in] */ Int32 tag)
{
    AutoPtr<ArrayOf<String> > oldRecords;
    AutoPtr<ArrayOf<String> > newRecords;
    String oldRecord(NULL);
    String newRecord(NULL);
    Boolean success = TRUE;
    // currently we only support one email records
    switch (tag) {
        case USIM_EFEMAIL_TAG:
            oldAdn->GetEmails((ArrayOf<String>**)&oldRecords);
            newAdn->GetEmails((ArrayOf<String>**)&newRecords);
            break;
        case USIM_EFANR_TAG:
            oldAdn->GetAdditionalNumbers((ArrayOf<String>**)&oldRecords);
            newAdn->GetAdditionalNumbers((ArrayOf<String>**)&newRecords);
            break;
        default:
            return FALSE;
    }

    if (NULL == oldRecords && NULL == newRecords) {
        // UI send empty string, no need to update
        Logger::E(String("AdnRecordCache"), String("Both old and new EMAIL/ANR are NULL"));
        return TRUE;
    }

    // insert scenario
    if (NULL == oldRecords && NULL != newRecords) {
        for (Int32 i = 0; i < newRecords->GetLength(); i++) {
            if (!TextUtils::IsEmpty((*newRecords)[i])) {
                success &= UpdateAnrEmailFile(String(NULL), (*newRecords)[i], index, tag, i);
            }
        }
    // delete scenario
    }
    else if (NULL != oldRecords && NULL == newRecords) {
        for (Int32 i = 0; i < oldRecords->GetLength(); i++) {
            if (!TextUtils::IsEmpty((*oldRecords)[i])) {
                success &= UpdateAnrEmailFile((*oldRecords)[i], String(NULL), index, tag, i);
            }
        }
    // update scenario
    }
    else {
        Int32 maxLen = (oldRecords->GetLength() > newRecords->GetLength()) ?
                        oldRecords->GetLength() : newRecords->GetLength();
        for (Int32 i = 0; i < maxLen; i++) {
            oldRecord = (i >= oldRecords->GetLength()) ? String(NULL) : (*oldRecords)[i];
            newRecord = (i >= newRecords->GetLength()) ? String(NULL) : (*newRecords)[i];

            if ((TextUtils::IsEmpty(oldRecord) && TextUtils::IsEmpty(newRecord)) ||
                (oldRecord != NULL && newRecord != NULL && oldRecord.Equals(newRecord))) {
                continue;
            }
            else {
                success &= UpdateAnrEmailFile(oldRecord, newRecord, index, tag, i);
            }
        }
    }

    return success;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
