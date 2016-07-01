#include "elastos/droid/internal/telephony/gsm/CUsimPhoneBookManager.h"
#include "elastos/droid/internal/telephony/gsm/CSimTlv.h"
#include "elastos/droid/internal/telephony/GsmAlphabet.h"
#include "elastos/droid/internal/telephony/IccUtils.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/os/AsyncResult.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::GsmAlphabet;
using Elastos::Droid::Internal::Telephony::Uicc::EIID_IIccConstants;
using Elastos::Droid::Internal::Telephony::Uicc::IAdnRecord;
using Elastos::Droid::Internal::Telephony::IccUtils;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IByte;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

const String CUsimPhoneBookManager::TAG("UsimPhoneBookManager");
const Boolean CUsimPhoneBookManager::DBG;
const Int32 CUsimPhoneBookManager::EVENT_PBR_LOAD_DONE;
const Int32 CUsimPhoneBookManager::EVENT_USIM_ADN_LOAD_DONE;
const Int32 CUsimPhoneBookManager::EVENT_IAP_LOAD_DONE;
const Int32 CUsimPhoneBookManager::EVENT_EMAIL_LOAD_DONE;
const Int32 CUsimPhoneBookManager::EVENT_ANR_LOAD_DONE;
const Int32 CUsimPhoneBookManager::EVENT_EF_EMAIL_RECORD_SIZE_DONE;
const Int32 CUsimPhoneBookManager::EVENT_EF_ANR_RECORD_SIZE_DONE;
const Int32 CUsimPhoneBookManager::EVENT_UPDATE_EMAIL_RECORD_DONE;
const Int32 CUsimPhoneBookManager::EVENT_UPDATE_ANR_RECORD_DONE;
const Int32 CUsimPhoneBookManager::EVENT_EF_IAP_RECORD_SIZE_DONE;
const Int32 CUsimPhoneBookManager::EVENT_UPDATE_IAP_RECORD_DONE;

const Int32 CUsimPhoneBookManager::USIM_TYPE1_TAG;
const Int32 CUsimPhoneBookManager::USIM_TYPE2_TAG;
const Int32 CUsimPhoneBookManager::USIM_TYPE3_TAG;
const Int32 CUsimPhoneBookManager::USIM_EFADN_TAG;
const Int32 CUsimPhoneBookManager::USIM_EFIAP_TAG;
const Int32 CUsimPhoneBookManager::USIM_EFEXT1_TAG;
const Int32 CUsimPhoneBookManager::USIM_EFSNE_TAG;
const Int32 CUsimPhoneBookManager::USIM_EFANR_TAG;
const Int32 CUsimPhoneBookManager::USIM_EFPBC_TAG;
const Int32 CUsimPhoneBookManager::USIM_EFGRP_TAG;
const Int32 CUsimPhoneBookManager::USIM_EFAAS_TAG;
const Int32 CUsimPhoneBookManager::USIM_EFGSD_TAG;
const Int32 CUsimPhoneBookManager::USIM_EFUID_TAG;
const Int32 CUsimPhoneBookManager::USIM_EFEMAIL_TAG;
const Int32 CUsimPhoneBookManager::USIM_EFCCP1_TAG;

const Int32 CUsimPhoneBookManager::MAX_NUMBER_SIZE_BYTES;
const Int32 CUsimPhoneBookManager::ANR_DESCRIPTION_ID;
const Int32 CUsimPhoneBookManager::ANR_BCD_NUMBER_LENGTH;
const Int32 CUsimPhoneBookManager::ANR_TON_NPI_ID;
const Int32 CUsimPhoneBookManager::ANR_ADDITIONAL_NUMBER_START_ID;
const Int32 CUsimPhoneBookManager::ANR_ADDITIONAL_NUMBER_END_ID;
const Int32 CUsimPhoneBookManager::ANR_CAPABILITY_ID;
const Int32 CUsimPhoneBookManager::ANR_EXTENSION_ID;
const Int32 CUsimPhoneBookManager::ANR_ADN_SFI_ID;
const Int32 CUsimPhoneBookManager::ANR_ADN_RECORD_IDENTIFIER_ID;

CAR_INTERFACE_IMPL_2(CUsimPhoneBookManager, Handler, IUsimPhoneBookManager, IIccConstants)

CAR_OBJECT_IMPL(CUsimPhoneBookManager)

CUsimPhoneBookManager::PbrFile::PbrFile(
    /* [in] */ IArrayList* records,
    /* [in] */ CUsimPhoneBookManager* host)
    : mHost(host)
{
    CHashMap::New((IHashMap**)&mFileIds);
    CHashMap::New((IHashMap**)&mAnrFileIds);
    CHashMap::New((IHashMap**)&mEmailFileIds);
    AutoPtr<ISimTlv> recTlv;
    Int32 recNum = 0;
    Int32 size;
    records->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        records->Get(i, (IInterface**)&obj);
        AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);
        Int32 len;
        array->GetLength(&len);
        AutoPtr<ArrayOf<Byte> > record = ArrayOf<Byte>::Alloc(len);
        for (Int32 j = 0; j < len; j++) {
            AutoPtr<IInterface> o;
            array->Get(j, (IInterface**)&o);
            Byte b;
            IByte::Probe(o)->GetValue(&b);
            (*record)[j] = b;
        }

        CSimTlv::New(record, 0, record->GetLength(), (ISimTlv**)&recTlv);
        ParseTag(recTlv, recNum);
        recNum ++;
    }
}

ECode CUsimPhoneBookManager::PbrFile::ParseTag(
    /* [in] */ ISimTlv* tlv,
    /* [in] */ Int32 recNum)
{
    Logger::D(TAG, "parseTag: recNum=%d", recNum);
    AutoPtr<ISimTlv> tlvEf;
    Int32 tag;
    AutoPtr<ArrayOf<Byte> > data;
    AutoPtr<IHashMap> val;
    CHashMap::New((IHashMap**)&val);
    AutoPtr<IArrayList> anrList;
    CArrayList::New((IArrayList**)&anrList);
    AutoPtr<IArrayList> emailList;
    CArrayList::New((IArrayList**)&emailList);

    Boolean b;
    do {
        tlv->GetTag(&tag);
        switch(tag) {
        case USIM_TYPE1_TAG: // A8
        case USIM_TYPE3_TAG: // AA
        case USIM_TYPE2_TAG: // A9
            tlv->GetData((ArrayOf<Byte>**)&data);
            CSimTlv::New(data, 0, data->GetLength(), (ISimTlv**)&tlvEf);
            ParseEf(tlvEf, IMap::Probe(val), tag, anrList, emailList);
            break;
        }
    } while (tlv->NextObject(&b), b);

    AutoPtr<IInteger32> i32 = CoreUtils::Convert(recNum);
    Int32 size;
    anrList->GetSize(&size);
    if (0 != size) {
        mAnrFileIds->Put(i32, anrList);
        Logger::D(TAG, "parseTag: recNum=%d ANR file list:%p", recNum, anrList.Get());
    }
    emailList->GetSize(&size);
    if (0 != size) {
        Logger::D(TAG, "parseTag: recNum=%d EMAIL file list:%p", recNum, emailList.Get());
        mEmailFileIds->Put(i32, emailList);
    }
    mFileIds->Put(i32, val);
    return NOERROR;
}

ECode CUsimPhoneBookManager::PbrFile::ParseEf(
    /* [in] */ ISimTlv* tlv,
    /* [in] */ IMap* val,
    /* [in] */ Int32 parentTag,
    /* [in] */ IArrayList* anrList,
    /* [in] */ IArrayList* emailList)
{
    Int32 tag;
    AutoPtr<ArrayOf<Byte> > data;
    Int32 tagNumberWithinParentTag = 0;
    Boolean b;
    do {
        tlv->GetTag(&tag);
        // Check if EFIAP is present. EFIAP must be under TYPE1 tag.
        if (parentTag == USIM_TYPE1_TAG && tag == USIM_EFIAP_TAG) {
            mHost->mIapPresent = TRUE;
        }
        if (parentTag == USIM_TYPE2_TAG && mHost->mIapPresent && tag == USIM_EFEMAIL_TAG) {
            mHost->mEmailPresentInIap = TRUE;
            mHost->mEmailTagNumberInIap = tagNumberWithinParentTag;
            mHost->Log(String("parseEf: EmailPresentInIap tag = ") + mHost->mEmailTagNumberInIap);
        }
        if (parentTag == USIM_TYPE2_TAG && mHost->mIapPresent && tag == USIM_EFANR_TAG) {
            mHost->mAnrPresentInIap = TRUE;
            mHost->mAnrTagNumberInIap = tagNumberWithinParentTag;
            mHost->Log(String("parseEf: AnrPresentInIap tag = ") + mHost->mAnrTagNumberInIap);
        }
        switch(tag) {
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
                tlv->GetData((ArrayOf<Byte>**)&data);
                Int32 efid = (((*data)[0] & 0xFF) << 8) | ((*data)[1] & 0xFF);
                val->Put(CoreUtils::Convert(tag), CoreUtils::Convert(efid));

                if (parentTag == USIM_TYPE1_TAG) {
                    if (tag == USIM_EFANR_TAG) {
                        anrList->Add(CoreUtils::Convert(efid));
                    }
                    else if (tag == USIM_EFEMAIL_TAG) {
                        emailList->Add(CoreUtils::Convert(efid));
                    }
                }
                Logger::D(TAG, "parseEf.put(%d,%d) parent tag: %d"
                        , tag, efid, parentTag);
                break;
        }
        tagNumberWithinParentTag++;
    } while(tlv->NextObject(&b), b);
    return NOERROR;
}

CUsimPhoneBookManager::CUsimPhoneBookManager()
    : mIsPbrPresent(FALSE)
    , mEmailPresentInIap(FALSE)
    , mEmailTagNumberInIap(0)
    , mAnrPresentInIap(FALSE)
    , mAnrTagNumberInIap(0)
    , mIapPresent(FALSE)
    , mPendingExtLoads(0)
    , mSuccess(FALSE)
    , mRefreshCache(FALSE)
{
}

ECode CUsimPhoneBookManager::constructor()
{
    return NOERROR;
}

ECode CUsimPhoneBookManager::constructor(
    /* [in] */ IIccFileHandler* fh,
    /* [in] */ IAdnRecordCache* cache)
{
    mFh = fh;
    CArrayList::New((IArrayList**)&mPhoneBookRecords);
    CArrayList::New((IArrayList**)&mAdnLengthList);
    CHashMap::New((IHashMap**)&mIapFileRecord);
    CHashMap::New((IHashMap**)&mEmailFileRecord);
    CHashMap::New((IHashMap**)&mAnrFileRecord);
    CHashMap::New((IHashMap**)&mRecordNums);
    mPbrFile = NULL;

    CHashMap::New((IHashMap**)&mAnrFlags);
    CHashMap::New((IHashMap**)&mEmailFlags);

    // We assume its present, after the first read this is updated.
    // So we don't have to read from UICC if its not present on subsequent reads.
    mIsPbrPresent = TRUE;
    mAdnCache = cache;
    return NOERROR;
}

ECode CUsimPhoneBookManager::Reset()
{
    if ((mAnrFlagsRecord != NULL) && (mEmailFlagsRecord != NULL) && mPbrFile != NULL) {
        Int32 size;
        mPbrFile->mFileIds->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            (*mAnrFlagsRecord)[i]->Clear();
            (*mEmailFlagsRecord)[i]->Clear();
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
    return NOERROR;
}

ECode CUsimPhoneBookManager::LoadEfFilesFromUsim(
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(mLock);
    Boolean b;
    if (mPhoneBookRecords->IsEmpty(&b), !b) {
        if (mRefreshCache) {
            mRefreshCache = FALSE;
            RefreshCache();
        }
        *result = mPhoneBookRecords;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    if (!mIsPbrPresent) {
        *result = NULL;
        return NOERROR;
    }

    // Check if the PBR file is present in the cache, if not read it
    // from the USIM.
    if (mPbrFile == NULL) {
        ReadPbrFileAndWait();
    }

    if (mPbrFile == NULL) {
        *result = NULL;
        return NOERROR;
    }

    Int32 numRecs;
    mPbrFile->mFileIds->GetSize(&numRecs);

    if ((mAnrFlagsRecord == NULL) && (mEmailFlagsRecord == NULL)) {
        mAnrFlagsRecord = ArrayOf<IArrayList*>::Alloc(numRecs);
        mEmailFlagsRecord = ArrayOf<IArrayList*>::Alloc(numRecs);
        for (Int32 i = 0; i < numRecs; i++) {
            CArrayList::New((IArrayList**)&(*mAnrFlagsRecord)[i]);
            CArrayList::New((IArrayList**)&(*mEmailFlagsRecord)[i]);
        }
    }

    for (Int32 i = 0; i < numRecs; i++) {
        ReadAdnFileAndWait(i);
        ReadEmailFileAndWait(i);
        ReadAnrFileAndWait(i);
    }
    // All EF files are loaded, post the response.
    *result = mPhoneBookRecords;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CUsimPhoneBookManager::InvalidateCache()
{
    mRefreshCache = TRUE;
    return NOERROR;
}

ECode CUsimPhoneBookManager::UpdateEmailFile(
    /* [in] */ Int32 adnRecNum,
    /* [in] */ const String& _oldEmail,
    /* [in] */ const String& _newEmail,
    /* [in] */ Int32 efidIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String oldEmail = _oldEmail;
    String newEmail = _newEmail;
    Int32 pbrIndex;
    GetPbrIndexBy(adnRecNum - 1, &pbrIndex);
    Int32 efid = GetEfidByTag(pbrIndex, USIM_EFEMAIL_TAG, efidIndex);
    if (oldEmail.IsNull())
        oldEmail = String("");
    if (newEmail.IsNull())
        newEmail = String("");
    String emails = oldEmail + "," + newEmail;
    mSuccess = FALSE;

    Log(String("updateEmailFile oldEmail : ") + oldEmail +
            " newEmail:" + newEmail + " emails:" + emails +
            " efid" + StringUtils::ToString(efid) +
            " adnRecNum: " + StringUtils::ToString(adnRecNum));

    if (efid == -1) {
        *result = mSuccess;
        return NOERROR;
    }
    if (mEmailPresentInIap && (oldEmail.IsEmpty() && !newEmail.IsEmpty())) {
        Int32 index;
        GetEmptyEmailNum_Pbrindex(pbrIndex, &index);
        if (index == 0) {
            Log(String("updateEmailFile getEmptyEmailNum_Pbrindex=0, pbrIndex is ") +
                    StringUtils::ToString(pbrIndex));
            mSuccess = TRUE;
            *result = mSuccess;
            return NOERROR;
        }

        mSuccess = UpdateIapFile(adnRecNum, oldEmail, newEmail, USIM_EFEMAIL_TAG);
    }
    else {
        mSuccess = TRUE;
    }
    if (mSuccess) {
        AutoLock lock(mLock);
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_EF_EMAIL_RECORD_SIZE_DONE, adnRecNum, efid,
                CoreUtils::Convert(emails), (IMessage**)&msg);
        mFh->GetEFLinearRecordSize(efid, msg);
        // try {
        mLock.Wait();
        // } catch (InterruptedException e) {
        //     Rlog.e(TAG, "interrupted while trying to update by search");
        // }
    }
    if (mEmailPresentInIap && mSuccess
            && (!oldEmail.IsEmpty() && newEmail.IsEmpty())) {
        mSuccess = UpdateIapFile(adnRecNum, oldEmail, newEmail, USIM_EFEMAIL_TAG);
    }
    *result = mSuccess;
    return NOERROR;
}

ECode CUsimPhoneBookManager::UpdateAnrFile(
    /* [in] */ Int32 adnRecNum,
    /* [in] */ const String& _oldAnr,
    /* [in] */ const String& _newAnr,
    /* [in] */ Int32 efidIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String oldAnr = _oldAnr;
    String newAnr = _newAnr;
    Int32 pbrIndex;
    GetPbrIndexBy(adnRecNum - 1, &pbrIndex);
    Int32 efid = GetEfidByTag(pbrIndex, USIM_EFANR_TAG, efidIndex);
    if (oldAnr.IsNull())
        oldAnr = String("");
    if (newAnr.IsNull())
        newAnr = String("");
    String anrs = oldAnr + "," + newAnr;
    mSuccess = FALSE;
    Log(String("updateAnrFile oldAnr : ") + oldAnr + ", newAnr:" + newAnr + " anrs:" + anrs + ", efid"
            + efid + ", adnRecNum: " + adnRecNum);
    if (efid == -1) {
        *result = mSuccess;
        return NOERROR;
    }
    if (mAnrPresentInIap && (oldAnr.IsEmpty() && !newAnr.IsEmpty())) {
        Int32 index;
        GetEmptyAnrNum_Pbrindex(pbrIndex, &index);
        if (index == 0) {
            Log(String("updateAnrFile getEmptyAnrNum_Pbrindex=0, pbrIndex is ") +
                StringUtils::ToString(pbrIndex));
            mSuccess = TRUE;
            *result = mSuccess;
            return NOERROR;
        }

        mSuccess = UpdateIapFile(adnRecNum, oldAnr, newAnr, USIM_EFANR_TAG);
    }
    else {
        mSuccess = TRUE;
    }
    {
        AutoLock lock(mLock);
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_EF_ANR_RECORD_SIZE_DONE, adnRecNum, efid, CoreUtils::Convert(anrs), (IMessage**)&msg);
        mFh->GetEFLinearRecordSize(efid, msg);
        // try {
        mLock.Wait();
        // } catch (InterruptedException e) {
        //     Rlog.e(TAG, "interrupted while trying to update by search");
        // }
    }
    if (mAnrPresentInIap && mSuccess
            && (!oldAnr.IsEmpty() && newAnr.IsEmpty())) {
        mSuccess = UpdateIapFile(adnRecNum, oldAnr, newAnr, USIM_EFANR_TAG);
    }
    *result = mSuccess;
    return NOERROR;
}

ECode CUsimPhoneBookManager::GetPbrIndexBy(
    /* [in] */ Int32 adnIndex,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 len;
    mAdnLengthList->GetSize(&len);
    Int32 size = 0;
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        mAdnLengthList->Get(i, (IInterface**)&obj);
        Int32 val;
        IInteger32::Probe(obj)->GetValue(&val);
        size += val;
        if (adnIndex < size) {
            *result = i;
            return NOERROR;
        }
    }
    *result = -1;
    return NOERROR;
}

ECode CUsimPhoneBookManager::ParseType1EmailFile(
    /* [in] */ Int32 numRecs,
    /* [in] */ Int32 pbrIndex)
{
    AutoPtr<ArrayOf<Byte> > emailRec;
    Int32 count;
    AutoPtr<IInterface> obj;
    mPbrFile->mEmailFileIds->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
    Int32 numEmailFiles;
    IArrayList::Probe(obj)->GetSize(&numEmailFiles);
    AutoPtr<IArrayList> emailList;
    CArrayList::New((IArrayList**)&emailList);
    Int32 adnInitIndex = GetInitIndexBy(pbrIndex);

    if (!HasRecordIn(mEmailFileRecord, pbrIndex))
        return NOERROR;

    Log(String("parseType1EmailFile: pbrIndex is: ") +
            StringUtils::ToString(pbrIndex) +
            ", numRecs is: " + StringUtils::ToString(numRecs));
    for (Int32 i = 0; i < numRecs; i++) {
        count = 0;
        emailList->Clear();

        for (Int32 j = 0; j < numEmailFiles; j++) {
            String email = ReadEmailRecord(i, pbrIndex, j*numRecs);
            emailList->Add(CoreUtils::Convert(email));
            if (email.IsEmpty()) {
                email = String("");
                continue;
            }

            count++;
            //Type1 Email file, no need for mEmailFlags
            obj = NULL;
            mEmailFlags->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
            IArrayList::Probe(obj)->Set(i+j*numRecs, CoreUtils::Convert(1));
        }

        // All Email files is NULL
        if (count == 0) {
            continue;
        }

        obj = NULL;
        mPhoneBookRecords->Get(i+adnInitIndex, (IInterface**)&obj);
        AutoPtr<IAdnRecord> rec = IAdnRecord::Probe(obj);
        if (rec != NULL) {
            Int32 size;
            emailList->GetSize(&size);
            AutoPtr<ArrayOf<String> > emails = ArrayOf<String>::Alloc(size);

            AutoPtr<ArrayOf<IInterface *> > array;
            emailList->ToArray((ArrayOf<IInterface *>**)&array);
            for (Int32 j = 0; j < size; j++) {
                String str;
                ICharSequence::Probe((*array)[j])->ToString(&str);
                (*emails)[j] = str;
            }
            rec->SetAdditionalNumbers(emails);
            mPhoneBookRecords->Set(i, rec);
        }
    }
    return NOERROR;
}

ECode CUsimPhoneBookManager::ParseType1AnrFile(
    /* [in] */ Int32 numRecs,
    /* [in] */ Int32 pbrIndex)
{
    Int32 count;
    Int32 numAnrFiles;
    AutoPtr<IInterface> obj;
    mPbrFile->mAnrFileIds->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
    IArrayList::Probe(obj)->GetSize(&numAnrFiles);

    AutoPtr<IArrayList> anrList;
    CArrayList::New((IArrayList**)&anrList);
    Int32 adnInitIndex = GetInitIndexBy(pbrIndex);

    if (!HasRecordIn(mAnrFileRecord, pbrIndex))
        return NOERROR;

    Log(String("parseType1AnrFile: pbrIndex is: ") + StringUtils::ToString(pbrIndex) +
        ", numRecs is: " + StringUtils::ToString(numRecs) +
        ", numAnrFiles " + StringUtils::ToString(numAnrFiles));
    for (Int32 i = 0; i < numRecs; i++) {
        count = 0;
        anrList->Clear();
        for (Int32 j = 0; j < numAnrFiles; j++) {
            String anr = ReadAnrRecord(i, pbrIndex, j*numRecs);
            anrList->Add(CoreUtils::Convert(anr));
            if (anr.IsEmpty()) {
                anr = String("");
                continue;
            }

            count++;
            //Fix Me: For type1 this is not necessary
            obj = NULL;
            mAnrFlags->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
            IArrayList::Probe(obj)->Set(i+j*numRecs, CoreUtils::Convert(1));
        }

        // All anr files is NULL
        if (count == 0) {
            continue;
        }

        obj = NULL;
        mPhoneBookRecords->Get(i+adnInitIndex, (IInterface**)&obj);
        AutoPtr<IAdnRecord> rec = IAdnRecord::Probe(obj);
        if (rec != NULL) {
            Int32 size;
            anrList->GetSize(&size);
            AutoPtr<ArrayOf<String> > anrs = ArrayOf<String>::Alloc(size);

            AutoPtr<ArrayOf<IInterface *> > array;
            anrList->ToArray((ArrayOf<IInterface *>**)&array);
            for (Int32 j = 0; j < size; j++) {
                String str;
                ICharSequence::Probe((*array)[j])->ToString(&str);
                (*anrs)[j] = str;
            }

            rec->SetAdditionalNumbers(anrs);
            mPhoneBookRecords->Set(i, rec);
        }
    }
    return NOERROR;
}

ECode CUsimPhoneBookManager::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;
    AutoPtr<ArrayOf<Byte> > data;
    Int32 efid;
    Int32 adnRecIndex;
    Int32 recordIndex;
    AutoPtr<ArrayOf<Int32> > recordSize;
    Int32 recordNumber;
    Int32 efidIndex;
    Int32 actualRecNumber;
    String oldAnr;
    String newAnr;
    String oldEmail;
    String newEmail;
    AutoPtr<IMessage> response;
    Int32 pbrIndex;

    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case EVENT_PBR_LOAD_DONE:
            Log(String("Loading PBR done"));
            ar = (AsyncResult*)(IObject*)obj.Get();
            if (ar->mException == NULL) {
                CreatePbrFile(IArrayList::Probe(ar->mResult));
            }
            {
                AutoLock lock(mLock);
                mLock.Notify();
            }
            break;
        case EVENT_USIM_ADN_LOAD_DONE:
            Log(String("Loading USIM ADN records done"));
            ar = (AsyncResult*)(IObject*)obj.Get();
            IInteger32::Probe(ar->mUserObj)->GetValue(&pbrIndex);
            if (ar->mException == NULL) {
                mPhoneBookRecords->AddAll(ICollection::Probe(ar->mResult));
                Int32 size;
                IArrayList::Probe(ar->mResult)->GetSize(&size);
                mAdnLengthList->Add(pbrIndex, CoreUtils::Convert(size));
                PutValidRecNums(pbrIndex);
            }
            else {
                Log(String("can't load USIM ADN records"));
            }
            {
                AutoLock lock(mLock);
                mLock.Notify();
            }
            break;
        case EVENT_IAP_LOAD_DONE:
            Log(String("Loading USIM IAP records done"));
            ar = (AsyncResult*)(IObject*)obj.Get();
            IInteger32::Probe(ar->mUserObj)->GetValue(&pbrIndex);
            if (ar->mException == NULL) {
                mIapFileRecord->Put(CoreUtils::Convert(pbrIndex), IArrayList::Probe(ar->mResult));
            }
            {
                AutoLock lock(mLock);
                mLock.Notify();
            }
            break;
        case EVENT_EMAIL_LOAD_DONE:
            Log(String("Loading USIM Email records done"));
            ar = (AsyncResult*)(IObject*)obj.Get();
            IInteger32::Probe(ar->mUserObj)->GetValue(&pbrIndex);
            if (ar->mException == NULL && mPbrFile != NULL) {
                AutoPtr<IInterface> o;
                mEmailFileRecord->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&o);
                AutoPtr<IArrayList> tmpList = IArrayList::Probe(o);
                if (tmpList == NULL) {
                    mEmailFileRecord->Put(CoreUtils::Convert(pbrIndex), IArrayList::Probe(ar->mResult));
                }
                else {
                    tmpList->AddAll(ICollection::Probe(ar->mResult));
                    mEmailFileRecord->Put(CoreUtils::Convert(pbrIndex), tmpList);
                }

                Int32 size;
                tmpList->GetSize(&size);
                Log(String("handlemessage EVENT_EMAIL_LOAD_DONE size is: ")
                        + StringUtils::ToString(size));
            }
            {
                AutoLock lock(mLock);
                mLock.Notify();
            }
            break;
        case EVENT_ANR_LOAD_DONE:
            Log(String("Loading USIM Anr records done"));
            ar = (AsyncResult*)(IObject*)obj.Get();
            IInteger32::Probe(ar->mUserObj)->GetValue(&pbrIndex);
            if (ar->mException == NULL && mPbrFile != NULL) {
                AutoPtr<IInterface> o;
                mAnrFileRecord->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&o);
                AutoPtr<IArrayList> tmp = IArrayList::Probe(o);
                if (tmp == NULL) {
                    mAnrFileRecord->Put(CoreUtils::Convert(pbrIndex), IArrayList::Probe(ar->mResult));
                }
                else {
                    tmp->AddAll(ICollection::Probe(ar->mResult));
                    mAnrFileRecord->Put(CoreUtils::Convert(pbrIndex), tmp);
                }

                Int32 size;
                tmp->GetSize(&size);
                Log(String("handlemessage EVENT_ANR_LOAD_DONE size is: ")
                        + StringUtils::ToString(size));
            }
            {
                AutoLock lock(mLock);
                mLock.Notify();
            }
            break;
        case EVENT_EF_EMAIL_RECORD_SIZE_DONE: {
            Log(String("Loading EF_EMAIL_RECORD_SIZE_DONE"));
            ar = (AsyncResult*)(IObject*)obj.Get();
            String emails;
            ICharSequence::Probe(ar->mUserObj)->ToString(&emails);
            Int32 arg1, arg2;
            msg->GetArg1(&arg1);
            msg->GetArg2(&arg2);
            adnRecIndex = arg1 - 1;
            efid = arg2;
            AutoPtr<ArrayOf<String> > email;
            StringUtils::Split(emails, String(","), (ArrayOf<String>**)&email);
            if (email->GetLength() == 1) {
                oldEmail = (*email)[0];
                newEmail = String("");
            }
            else if (email->GetLength() > 1) {
                oldEmail = (*email)[0];
                newEmail = (*email)[1];
            }
            if (ar->mException != NULL) {
                mSuccess = FALSE;
                {
                    AutoLock lock(mLock);
                    mLock.Notify();
                }
                return NOERROR;
            }
            Int32 size;
            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
            iArray->GetLength(&size);
            recordSize = ArrayOf<Int32>::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> o;
                iArray->Get(i, (IInterface**)&o);
                Int32 value;
                IInteger32::Probe(o)->GetValue(&value);
                (*recordSize)[i] = value;
            }

            mPhoneBookRecords->GetSize(&size);
            recordNumber = GetEmailRecNumber(adnRecIndex, size, oldEmail);
            if (recordSize->GetLength() != 3 || recordNumber > (*recordSize)[2] || recordNumber <= 0) {
                mSuccess = FALSE;
                {
                    AutoLock lock(mLock);
                    mLock.Notify();
                }
                return NOERROR;
            }
            data = BuildEmailData((*recordSize)[0], adnRecIndex, newEmail);

            actualRecNumber = recordNumber;
            if (!mEmailPresentInIap) {
                Int32 val;
                GetPbrIndexBy(adnRecIndex, &val);
                AutoPtr<IInterface> o;
                mPbrFile->mEmailFileIds->Get(CoreUtils::Convert(val), (IInterface**)&o);
                IArrayList::Probe(o)->IndexOf(CoreUtils::Convert(efid), &efidIndex);
                if (efidIndex == -1) {
                    Log(String("wrong efid index:") + StringUtils::ToString(efid) );
                    return NOERROR;
                }

                GetPbrIndexBy(adnRecIndex, &val);
                o = NULL;
                mAdnLengthList->Get(val, (IInterface**)&o);
                IInteger32::Probe(o)->GetValue(&val);
                actualRecNumber = recordNumber + efidIndex * val;
                Log(String("EMAIL index:") + StringUtils::ToString(efidIndex) +
                        " efid:" + StringUtils::ToString(efid) +
                        " actual RecNumber:" + StringUtils::ToString(actualRecNumber));
            }

            AutoPtr<IArrayOf> array;
            CArrayOf::New(EIID_IByte, data->GetLength(), (IArrayOf**)&array);
            for (Int32 i = 0; i < data->GetLength(); i++) {
                array->Set(i, CoreUtils::ConvertByte((*data)[i]));
            }

            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_UPDATE_EMAIL_RECORD_DONE, actualRecNumber, adnRecIndex,
                    array, (IMessage**)&msg);

            mFh->UpdateEFLinearFixed(
                    efid,
                    recordNumber,
                    data,
                    String(NULL),
                    msg);
            mPendingExtLoads = 1;
            break;
        }
        case EVENT_EF_ANR_RECORD_SIZE_DONE: {
            Log(String("Loading EF_ANR_RECORD_SIZE_DONE"));
            ar = (AsyncResult*)(IObject*)obj.Get();
            String anrs;
            ICharSequence::Probe(ar->mUserObj)->ToString(&anrs);
            Int32 arg1, arg2;
            msg->GetArg1(&arg1);
            msg->GetArg2(&arg2);
            adnRecIndex = arg1 - 1;
            efid = arg2;
            AutoPtr<ArrayOf<String> > anr;
            StringUtils::Split(anrs, String(","), (ArrayOf<String>**)&anr);

            if (anr->GetLength() == 1) {
                oldAnr = (*anr)[0];
                newAnr = String("");
            }
            else if (anr->GetLength() > 1) {
                oldAnr = (*anr)[0];
                newAnr = (*anr)[1];
            }
            if (ar->mException != NULL) {
                mSuccess = FALSE;
                {
                    AutoLock lock(mLock);
                    mLock.Notify();
                }
                return NOERROR;
            }
            Int32 size;
            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
            iArray->GetLength(&size);
            recordSize = ArrayOf<Int32>::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> o;
                iArray->Get(i, (IInterface**)&o);
                Int32 value;
                IInteger32::Probe(o)->GetValue(&value);
                (*recordSize)[i] = value;
            }

            mPhoneBookRecords->GetSize(&size);
            recordNumber = GetAnrRecNumber(adnRecIndex, size, oldAnr);
            if (recordSize->GetLength() != 3 || recordNumber > (*recordSize)[2] || recordNumber <= 0) {
                mSuccess = FALSE;
                {
                    AutoLock lock(mLock);
                    mLock.Notify();
                }
                return NOERROR;
            }
            data = BuildAnrData((*recordSize)[0], adnRecIndex, newAnr);
            if (data == NULL) {
                mSuccess = FALSE;
                {
                    AutoLock lock(mLock);
                    mLock.Notify();
                }
                return NOERROR;
            }

            actualRecNumber = recordNumber;
            if (!mAnrPresentInIap) {
                Int32 val;
                GetPbrIndexBy(adnRecIndex, &val);
                AutoPtr<IInterface> o;
                mPbrFile->mAnrFileIds->Get(CoreUtils::Convert(val), (IInterface**)&o);
                IArrayList::Probe(o)->IndexOf(CoreUtils::Convert(efid), &efidIndex);

                if (efidIndex == -1) {
                    Log(String("wrong efid index:") + efid );
                    return NOERROR;
                }

                GetPbrIndexBy(adnRecIndex, &val);
                o = NULL;
                mAdnLengthList->Get(val, (IInterface**)&o);
                IInteger32::Probe(o)->GetValue(&val);
                actualRecNumber = recordNumber + efidIndex * val;

                Log(String("ANR index:") + StringUtils::ToString(efidIndex) +
                    " efid:" + StringUtils::ToString(efid) +
                    " actual RecNumber:" + StringUtils::ToString(actualRecNumber));
            }

            AutoPtr<IArrayOf> array;
            CArrayOf::New(EIID_IByte, data->GetLength(), (IArrayOf**)&array);
            for (Int32 i = 0; i < data->GetLength(); i++) {
                array->Set(i, CoreUtils::ConvertByte((*data)[i]));
            }

            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_UPDATE_ANR_RECORD_DONE, actualRecNumber,
                    adnRecIndex, array, (IMessage**)&msg);

            mFh->UpdateEFLinearFixed(
                    efid,
                    recordNumber,
                    data,
                    String(NULL),
                    msg);
            mPendingExtLoads = 1;
            break;
        }
        case EVENT_UPDATE_EMAIL_RECORD_DONE: {
            Log(String("Loading UPDATE_EMAIL_RECORD_DONE"));
            ar = (AsyncResult*)(IObject*)obj.Get();
            if (ar->mException != NULL) {
                mSuccess = FALSE;
            }

            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mUserObj);
            Int32 size;
            iArray->GetLength(&size);
            data = ArrayOf<Byte>::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> o;
                iArray->Get(i, (IInterface**)&o);
                Byte b;
                IByte::Probe(o)->GetValue(&b);
                (*data)[i] = b;
            }

            Int32 arg1, arg2;
            msg->GetArg1(&arg1);
            msg->GetArg2(&arg2);
            recordNumber = arg1;
            adnRecIndex = arg2;
            GetPbrIndexBy(adnRecIndex, &pbrIndex);
            Log(String("EVENT_UPDATE_EMAIL_RECORD_DONE"));
            mPendingExtLoads = 0;
            mSuccess = TRUE;
            AutoPtr<IInterface> o;
            mEmailFileRecord->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&o);
            IArrayList::Probe(o)->Set(recordNumber - 1, iArray);

            for (Int32 i = 0; i < data->GetLength(); i++) {
                Log(String("EVENT_UPDATE_EMAIL_RECORD_DONE data = ") + (*data)[i] + ",i is " + i);
                if ((*data)[i] != (Byte) 0xff) {
                    Log(String("EVENT_UPDATE_EMAIL_RECORD_DONE data !=0xff"));
                    AutoPtr<IInterface> o;
                    mEmailFlags->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&o);
                    IArrayList::Probe(o)->Set(recordNumber - 1, CoreUtils::Convert(1));
                    break;
                }
                AutoPtr<IInterface> o;
                mEmailFlags->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&o);
                IArrayList::Probe(o)->Set(recordNumber - 1, 0);
            }
            {
                AutoLock lock(mLock);
                mLock.Notify();
            }
            break;
        }
        case EVENT_UPDATE_ANR_RECORD_DONE: {
            Log(String("Loading UPDATE_ANR_RECORD_DONE"));
            ar = (AsyncResult*)(IObject*)obj.Get();

            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mUserObj);
            Int32 size;
            iArray->GetLength(&size);
            data = ArrayOf<Byte>::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> o;
                iArray->Get(i, (IInterface**)&o);
                Byte b;
                IByte::Probe(o)->GetValue(&b);
                (*data)[i] = b;
            }

            Int32 arg1, arg2;
            msg->GetArg1(&arg1);
            msg->GetArg2(&arg2);
            recordNumber = arg1;
            adnRecIndex = arg2;
            GetPbrIndexBy(adnRecIndex, &pbrIndex);
            if (ar->mException != NULL) {
                mSuccess = FALSE;
            }
            Log(String("EVENT_UPDATE_ANR_RECORD_DONE"));
            mPendingExtLoads = 0;
            mSuccess = TRUE;
            AutoPtr<IInterface> o;
            mAnrFileRecord->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&o);
            IArrayList::Probe(o)->Set(recordNumber - 1, iArray);

            for (Int32 i = 0; i < data->GetLength(); i++) {
                if ((*data)[i] != (byte) 0xff) {
                    AutoPtr<IInterface> o;
                    mAnrFlags->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&o);
                    IArrayList::Probe(o)->Set(recordNumber - 1, CoreUtils::Convert(1));
                    break;
                }
                AutoPtr<IInterface> o;
                mAnrFlags->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&o);
                IArrayList::Probe(o)->Set(recordNumber - 1, 0);
            }
            {
                AutoLock lock(mLock);
                mLock.Notify();
            }
            break;
        }
        case EVENT_EF_IAP_RECORD_SIZE_DONE: {
            Log(String("EVENT_EF_IAP_RECORD_SIZE_DONE"));
            ar = (AsyncResult*)(IObject*)obj.Get();
            Int32 arg1, arg2;
            msg->GetArg1(&arg1);
            msg->GetArg2(&arg2);
            recordNumber = arg2;
            adnRecIndex = arg1 - 1;
            GetPbrIndexBy(adnRecIndex, &pbrIndex);
            efid = GetEfidByTag(pbrIndex, USIM_EFIAP_TAG, 0);
            Int32 tag;
            IInteger32::Probe(ar->mUserObj)->GetValue(&tag);
            if (ar->mException != NULL) {
                mSuccess = FALSE;
                {
                    AutoLock lock(mLock);
                    mLock.Notify();
                }
                return NOERROR;
            }
            GetPbrIndexBy(adnRecIndex, &pbrIndex);
            efid = GetEfidByTag(pbrIndex, USIM_EFIAP_TAG, 0);
            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
            Int32 size;
            iArray->GetLength(&size);
            recordSize = ArrayOf<Int32>::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> o;
                iArray->Get(i, (IInterface**)&o);
                Int32 val;
                IInteger32::Probe(o)->GetValue(&val);
                (*recordSize)[i] = val;
            }
            data = NULL;

            recordIndex = adnRecIndex - GetInitIndexBy(pbrIndex);
            Log(String("handleIAP_RECORD_SIZE_DONE adnRecIndex is: ")
                    + StringUtils::ToString(adnRecIndex) +
                    ", recordNumber is: " + StringUtils::ToString(recordNumber)
                    + ", recordIndex is: " + StringUtils::ToString(recordIndex));
            if (recordSize->GetLength() != 3 || recordIndex + 1 > (*recordSize)[2]
                    || recordNumber == 0) {
                mSuccess = FALSE;
                {
                    AutoLock lock(mLock);
                    mLock.Notify();
                }
                return NOERROR;
            }
            if (HasRecordIn(mIapFileRecord, pbrIndex)) {
                AutoPtr<IInterface> o;
                mIapFileRecord->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&o);
                o = NULL;
                IArrayList::Probe(o)->Get(recordIndex, (IInterface**)&o);
                AutoPtr<IArrayOf> array = IArrayOf::Probe(o);
                Int32 size;
                array->GetLength(&size);
                data = ArrayOf<Byte>::Alloc(size);
                for (Int32 i = 0; i < size; i++) {
                    o = NULL;
                    array->Get(i, (IInterface**)&o);
                    Byte b;
                    IByte::Probe(o)->GetValue(&b);
                    (*data)[i] = b;
                }

                AutoPtr<ArrayOf<Byte> > record_data = ArrayOf<Byte>::Alloc(data->GetLength());
                record_data->Copy(0, data, 0, record_data->GetLength());
                switch (tag) {
                    case USIM_EFEMAIL_TAG:
                        (*record_data)[mEmailTagNumberInIap] = (Byte) recordNumber;
                        break;
                    case USIM_EFANR_TAG:
                        (*record_data)[mAnrTagNumberInIap] = (Byte) recordNumber;
                        break;
                }
                mPendingExtLoads = 1;
                Log(String(" IAP  efid= ") + efid + ", update IAP index= " + (recordIndex)
                        + " with value= " + IccUtils::BytesToHexString(record_data));

                array = NULL;
                CArrayOf::New(EIID_IByte, record_data->GetLength(), (IArrayOf**)&array);
                for (Int32 i = 0; i < record_data->GetLength(); i++) {
                    array->Set(i, CoreUtils::ConvertByte((*record_data)[i]));
                }

                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_UPDATE_IAP_RECORD_DONE, adnRecIndex, recordNumber,
                        array, (IMessage**)&msg);
                mFh->UpdateEFLinearFixed(
                        efid,
                        recordIndex + 1,
                        record_data,
                        String(NULL),
                        msg);
            }
            break;
        }
        case EVENT_UPDATE_IAP_RECORD_DONE: {
            Log(String("EVENT_UPDATE_IAP_RECORD_DONE"));
            ar = (AsyncResult*)(IObject*)obj.Get();
            if (ar->mException != NULL) {
                mSuccess = FALSE;
            }

            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mUserObj);
            Int32 size;
            iArray->GetLength(&size);
            data = ArrayOf<Byte>::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> o;
                iArray->Get(i, (IInterface**)&o);
                Byte b;
                IByte::Probe(o)->GetValue(&b);
                (*data)[i] = b;
            }

            Int32 arg1;
            msg->GetArg1(&arg1);
            adnRecIndex = arg1;
            GetPbrIndexBy(adnRecIndex, &pbrIndex);
            recordIndex = adnRecIndex - GetInitIndexBy(pbrIndex);
            Log(String("handleMessage EVENT_UPDATE_IAP_RECORD_DONE recordIndex is: ")
                    + StringUtils::ToString(recordIndex) +
                    ", adnRecIndex is: " + StringUtils::ToString(adnRecIndex));
            mPendingExtLoads = 0;
            mSuccess = TRUE;

            AutoPtr<IInterface> o;
            mIapFileRecord->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&o);
            IArrayList::Probe(o)->Set(recordIndex, iArray);
            Log(String("the iap email recordNumber is :") + (*data)[mEmailTagNumberInIap]);
            {
                AutoLock lock(mLock);
                mLock.Notify();
            }
            break;
        }
    }
    return NOERROR;
}

ECode CUsimPhoneBookManager::GetAnrCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 count = 0;
    Int32 pbrIndex;
    mAnrFlags->GetSize(&pbrIndex);
    for (Int32 j = 0; j < pbrIndex; j++) {
        AutoPtr<IInterface> obj;
        mAnrFlags->Get(CoreUtils::Convert(j), (IInterface**)&obj);
        Int32 size;
        IArrayList::Probe(obj)->GetSize(&size);
        count += size;
    }
    Log(String("getAnrCount count is: ") + StringUtils::ToString(count));
    *result = count;
    return NOERROR;
}

ECode CUsimPhoneBookManager::GetEmailCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 count = 0;
    Int32 pbrIndex;
    mEmailFlags->GetSize(&pbrIndex);
    for (Int32 j = 0; j < pbrIndex; j++) {
        AutoPtr<IInterface> obj;
        mEmailFlags->Get(CoreUtils::Convert(j), (IInterface**)&obj);
        Int32 size;
        IArrayList::Probe(obj)->GetSize(&size);
        count += size;
    }
    Log(String("getEmailCount count is: ") + StringUtils::ToString(count));
    *result = count;
    return NOERROR;
}

ECode CUsimPhoneBookManager::GetSpareAnrCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 count = 0;
    Int32 pbrIndex;
    mAnrFlags->GetSize(&pbrIndex);
    for (Int32 j = 0; j < pbrIndex; j++) {
        AutoPtr<IInterface> obj;
        mAnrFlags->Get(CoreUtils::Convert(j), (IInterface**)&obj);
        AutoPtr<IArrayList> list = IArrayList::Probe(obj);
        Int32 size;
        list->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> o;
            list->Get(i, (IInterface**)&o);
            Int32 val;
            IInteger32::Probe(o)->GetValue(&val);
            if (0 == val)
                count++;
        }
    }
    Log(String("getSpareAnrCount count is") + StringUtils::ToString(count));
    *result = count;
    return NOERROR;
}

ECode CUsimPhoneBookManager::GetSpareEmailCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 count = 0;
    Int32 pbrIndex;
    mEmailFlags->GetSize(&pbrIndex);
    for (Int32 j = 0; j < pbrIndex; j++) {
        AutoPtr<IInterface> obj;
        mEmailFlags->Get(CoreUtils::Convert(j), (IInterface**)&obj);
        AutoPtr<IArrayList> list = IArrayList::Probe(obj);
        Int32 size;
        list->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> o;
            list->Get(i, (IInterface**)&o);
            Int32 val;
            IInteger32::Probe(o)->GetValue(&val);
            if (0 == val)
                count++;
        }
    }
    Log(String("getSpareEmailCount count is: ") + StringUtils::ToString(count));
    *result = count;
    return NOERROR;
}

ECode CUsimPhoneBookManager::GetUsimAdnCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean b;
    if ((mPhoneBookRecords != NULL) && (mPhoneBookRecords->IsEmpty(&b), !b)) {
        Int32 size;
        mPhoneBookRecords->GetSize(&size);
        Log(String("getUsimAdnCount count is") + StringUtils::ToString(size));
        *result = size;
        return NOERROR;
    }
    else {
        *result = 0;
        return NOERROR;
    }
    return NOERROR;
}

ECode CUsimPhoneBookManager::GetEmptyEmailNum_Pbrindex(
    /* [in] */ Int32 pbrindex,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 count = 0;
    Int32 size = 0;

    if (!mEmailPresentInIap) {
        //for Type1 Email, the number is always equal to ADN
        //Log("getEmptyEmailNum_Pbrindex pbrIndex:" + pbrindex + " default to 1");
        *result = 1;
        return NOERROR;
    }

    Boolean b;
    if (mEmailFlags->ContainsKey(CoreUtils::Convert(pbrindex), &b), b) {
        AutoPtr<IInterface> obj;
        mEmailFlags->Get(CoreUtils::Convert(pbrindex), (IInterface**)&obj);
        AutoPtr<IArrayList> list = IArrayList::Probe(obj);
        list->GetSize(&size);

        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> o;
            list->Get(i, (IInterface**)&o);
            Int32 val;
            IInteger32::Probe(o)->GetValue(&val);
            if (0 == val) count++;
        }
    }
    //Log("getEmptyEmailNum_Pbrindex pbrIndex is: " + pbrindex + " size is: "
    //        + size + ", count is " + count);
    *result = count;
    return NOERROR;
}

ECode CUsimPhoneBookManager::GetEmptyAnrNum_Pbrindex(
    /* [in] */ Int32 pbrindex,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 count = 0;
    Int32 size = 0;

    if (!mAnrPresentInIap) {
        //for Type1 Anr, the number is always equals to ADN
        //Log("getEmptyAnrNum_Pbrindex pbrIndex:" + pbrindex + " default to 1");
        *result = 1;
        return NOERROR;
    }

    Boolean b;
    if (mAnrFlags->ContainsKey(CoreUtils::Convert(pbrindex), &b), b) {
        AutoPtr<IInterface> obj;
        mAnrFlags->Get(CoreUtils::Convert(pbrindex), (IInterface**)&obj);
        AutoPtr<IArrayList> list = IArrayList::Probe(obj);
        list->GetSize(&size);

        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> o;
            list->Get(i, (IInterface**)&o);
            Int32 val;
            IInteger32::Probe(o)->GetValue(&val);
            if (0 == val) count++;
        }
    }
    //Log("getEmptyAnrNum_Pbrindex pbrIndex is: " + pbrindex + " size is: "
    //        + size + ", count is " + count);
    *result = count;
    return NOERROR;
}

ECode CUsimPhoneBookManager::GetEmailFilesCountEachAdn(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IMap> fileIds;
    if (mPbrFile == NULL) {
        Logger::E(TAG, "mPbrFile is NULL, exiting from getEmailFilesCountEachAdn");
        *result = 0;
        return NOERROR;
    }
    else {
        AutoPtr<IInterface> obj;
        mPbrFile->mFileIds->Get(0, (IInterface**)&obj);
        fileIds = IMap::Probe(obj);
    }
    if (fileIds == NULL) {
        *result = 0;
        return NOERROR;
    }

    Boolean b;
    if (fileIds->ContainsKey(CoreUtils::Convert(USIM_EFEMAIL_TAG), &b), b) {
        if (!mEmailPresentInIap) {
            AutoPtr<IInterface> obj;
            mPbrFile->mEmailFileIds->Get(0, (IInterface**)&obj);
            Int32 size;
            IArrayList::Probe(obj)->GetSize(&size);
            *result = size;
            return NOERROR;
        }
        else {
            *result = 1;
            return NOERROR;
        }
    }
    else {
        *result = 0;
        return NOERROR;
    }
    return NOERROR;
}

ECode CUsimPhoneBookManager::GetAnrFilesCountEachAdn(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IMap> fileIds;
    if (mPbrFile == NULL) {
        Logger::E(TAG, "mPbrFile is NULL, exiting from getAnrFilesCountEachAdn");
        *result = 0;
        return NOERROR;
    }
    else {
        AutoPtr<IInterface> obj;
        mPbrFile->mFileIds->Get(0, (IInterface**)&obj);
        fileIds = IMap::Probe(obj);
    }
    if (fileIds == NULL) {
        *result = 0;
        return NOERROR;
    }

    Boolean b;
    if (fileIds->ContainsKey(CoreUtils::Convert(USIM_EFANR_TAG), &b), b) {
        if (!mAnrPresentInIap) {
            AutoPtr<IInterface> obj;
            mPbrFile->mAnrFileIds->Get(0, (IInterface**)&obj);
            Int32 size;
            IArrayList::Probe(obj)->GetSize(&size);
            *result = size;
            return NOERROR;
        } else {
            *result = 1;
            return NOERROR;
        }
    }
    else {
        *result = 0;
        return NOERROR;
    }
    return NOERROR;
}

void CUsimPhoneBookManager::RefreshCache()
{
    if (mPbrFile == NULL) return;
    mPhoneBookRecords->Clear();

    Int32 numRecs;
    mPbrFile->mFileIds->GetSize(&numRecs);
    for (Int32 i = 0; i < numRecs; i++) {
        ReadAdnFileAndWait(i);
    }
}

void CUsimPhoneBookManager::ReadPbrFileAndWait()
{
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_PBR_LOAD_DONE, (IMessage**)&msg);
    mFh->LoadEFLinearFixedAll(EF_PBR, msg);
    // try {
    mLock.Wait();
    // } catch (InterruptedException e) {
    //     Rlog.e(TAG, "Interrupted Exception in readAdnFileAndWait");
    // }
}

void CUsimPhoneBookManager::ReadEmailFileAndWait(
    /* [in] */ Int32 recNum)
{
    AutoPtr<IMap> fileIds;
    AutoPtr<IInterface> obj;
    mPbrFile->mFileIds->Get(CoreUtils::Convert(recNum), (IInterface**)&obj);
    fileIds = IMap::Probe(obj);
    if (fileIds == NULL) return;

    Boolean b;
    if (fileIds->ContainsKey(CoreUtils::Convert(USIM_EFEMAIL_TAG), &b), b) {
        // Check if the EFEmail is a Type 1 file or a type 2 file.
        // If mEmailPresentInIap is TRUE, its a type 2 file.
        // So we read the IAP file and then read the email records.
        // instead of reading directly.
        if (mEmailPresentInIap) {
            AutoPtr<IInterface> o;
            fileIds->Get(CoreUtils::Convert(USIM_EFIAP_TAG), (IInterface**)&o);
            Int32 val;
            IInteger32::Probe(o)->GetValue(&val);
            ReadIapFileAndWait(val, recNum);
            if (!HasRecordIn(mIapFileRecord, recNum)) {
                Logger::E(TAG, "Error: IAP file is empty");
                return;
            }
            o = NULL;
            fileIds->Get(CoreUtils::Convert(USIM_EFEMAIL_TAG), (IInterface**)&o);
            IInteger32::Probe(o)->GetValue(&val);

            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_EMAIL_LOAD_DONE, CoreUtils::Convert(recNum), (IMessage**)&msg);
            mFh->LoadEFLinearFixedAll(val, msg);

            Log(String("readEmailFileAndWait email efid is : ") + StringUtils::ToString(val));
            // try {
            mLock.Wait();
            // } catch (InterruptedException e) {
            //     Rlog.e(TAG, "Interrupted Exception in readEmailFileAndWait");
            // }

        }
        else {
            // Read all Email files per Record
            AutoPtr<IInterface> o;
            mPbrFile->mEmailFileIds->Get(CoreUtils::Convert(recNum), (IInterface**)&o);
            AutoPtr<IArrayList> list = IArrayList::Probe(o);
            Int32 size;
            list->GetSize(&size);

            for (Int32 i = 0; i < size; i++) {
                obj = NULL;
                list->Get(i, (IInterface**)&obj);
                Int32 efid;
                IInteger32::Probe(obj)->GetValue(&efid);

                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_EMAIL_LOAD_DONE, CoreUtils::Convert(recNum), (IMessage**)&msg);
                mFh->LoadEFLinearFixedPart(efid, GetValidRecordNums(recNum), msg);

                Log(String("readEmailFileAndWait email efid is : ") +
                        StringUtils::ToString(efid) +
                        " recNum:" + StringUtils::ToString(recNum));
                // try {
                mLock.Wait();
                // } catch (InterruptedException e) {
                //     Rlog.e(TAG, "Interrupted Exception in readEmailFileAndWait");
                // }
            }
        }

        obj = NULL;
        mEmailFileRecord->Get(CoreUtils::Convert(recNum), (IInterface**)&obj);
        AutoPtr<IArrayList> list = IArrayList::Probe(obj);
        Int32 size;
        list->GetSize(&size);
        for (Int32 m = 0; m < size; m++) {
            (*mEmailFlagsRecord)[recNum]->Add(0);
        }
        mEmailFlags->Put(CoreUtils::Convert(recNum), (*mEmailFlagsRecord)[recNum]);

        if (!HasRecordIn(mEmailFileRecord, recNum)) {
            Logger::E(TAG, "Error: Email file is empty");
            return;
        }
        UpdatePhoneAdnRecordWithEmail(recNum);
    }
}

void CUsimPhoneBookManager::ReadAnrFileAndWait(
    /* [in] */ Int32 recNum)
{
    AutoPtr<IMap> fileIds;
    if (mPbrFile == NULL) {
        Logger::E(TAG, "mPbrFile is NULL, exiting from readAnrFileAndWait");
        return;
    }
    else {
        AutoPtr<IInterface> obj;
        mPbrFile->mFileIds->Get(CoreUtils::Convert(recNum), (IInterface**)&obj);
        fileIds = IMap::Probe(obj);
    }
    Boolean b;
    if (fileIds == NULL || (fileIds->IsEmpty(&b), b))
        return;
    if (fileIds->ContainsKey(CoreUtils::Convert(USIM_EFANR_TAG), &b), b) {
        if (mAnrPresentInIap) {
            AutoPtr<IInterface> obj;
            fileIds->Get(CoreUtils::Convert(USIM_EFIAP_TAG), (IInterface**)&obj);
            Int32 val;
            IInteger32::Probe(obj)->GetValue(&val);
            ReadIapFileAndWait(val, recNum);
            if (!HasRecordIn(mIapFileRecord, recNum)) {
                Logger::E(TAG, "Error: IAP file is empty");
                return;
            }
            obj = NULL;
            fileIds->Get(CoreUtils::Convert(USIM_EFANR_TAG), (IInterface**)&obj);
            IInteger32::Probe(obj)->GetValue(&val);
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_ANR_LOAD_DONE, CoreUtils::Convert(recNum), (IMessage**)&msg);
            mFh->LoadEFLinearFixedAll(val, msg);
            Log(String("readAnrFileAndWait anr efid is : ") + StringUtils::ToString(val));
            // try {
                mLock.Wait();
            // } catch (InterruptedException e) {
            //     Logger::E(TAG, "Interrupted Exception in readEmailFileAndWait");
            // }
        }
        else {
            // Read all Anr files for each Adn Record
            AutoPtr<IInterface> obj;
            mPbrFile->mAnrFileIds->Get(CoreUtils::Convert(recNum), (IInterface**)&obj);
            AutoPtr<IArrayList> list = IArrayList::Probe(obj);
            Int32 size;
            list->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                obj = NULL;
                list->Get(i, (IInterface**)&obj);
                Int32 efid;
                IInteger32::Probe(obj)->GetValue(&efid);

                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_ANR_LOAD_DONE, CoreUtils::Convert(recNum), (IMessage**)&msg);
                mFh->LoadEFLinearFixedPart(efid, GetValidRecordNums(recNum), msg);
                Log(String("readAnrFileAndWait anr efid is : ") + StringUtils::ToString(efid) +
                        " recNum:" + StringUtils::ToString(recNum));
                // try {
                mLock.Wait();
                // } catch (InterruptedException e) {
                //     Logger::E(TAG, "Interrupted Exception in readEmailFileAndWait");
                // }
            }
        }

        AutoPtr<IInterface> obj;
        mAnrFileRecord->Get(CoreUtils::Convert(recNum), (IInterface**)&obj);
        AutoPtr<IArrayList> list = IArrayList::Probe(obj);
        Int32 size;
        list->GetSize(&size);
        for (Int32 m = 0; m < size; m++) {
            (*mAnrFlagsRecord)[recNum]->Add(0);
        }
        mAnrFlags->Put(CoreUtils::Convert(recNum), (*mAnrFlagsRecord)[recNum]);

        if (!HasRecordIn(mAnrFileRecord, recNum)) {
            Logger::E(TAG, "Error: Anr file is empty");
            return;
        }
        UpdatePhoneAdnRecordWithAnr(recNum);
    }
}

void CUsimPhoneBookManager::ReadIapFileAndWait(
    /* [in] */ Int32 efid,
    /* [in] */ Int32 recNum)
{
    Log(String("pbrIndex is ") + StringUtils::ToString(recNum) +
            ",iap efid is : " + StringUtils::ToString(efid));
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_IAP_LOAD_DONE, CoreUtils::Convert(recNum), (IMessage**)&msg);
    mFh->LoadEFLinearFixedPart(efid, GetValidRecordNums(recNum), msg);
    // try {
    mLock.Wait();
    // } catch (InterruptedException e) {
    //     Rlog.e(TAG, "Interrupted Exception in readIapFileAndWait");
    // }
}

Boolean CUsimPhoneBookManager::UpdateIapFile(
    /* [in] */ Int32 adnRecNum,
    /* [in] */ const String& oldValue,
    /* [in] */ const String& newValue,
    /* [in] */ Int32 tag)
{
    Int32 pbrIndex;
    GetPbrIndexBy(adnRecNum - 1, &pbrIndex);
    Int32 efid = GetEfidByTag(pbrIndex, USIM_EFIAP_TAG, 0);
    mSuccess = FALSE;
    Int32 recordNumber = -1;
    if (efid == -1)
        return mSuccess;

    Int32 size;
    mPhoneBookRecords->GetSize(&size);
    switch (tag) {
        case USIM_EFEMAIL_TAG:
            recordNumber = GetEmailRecNumber(adnRecNum - 1,
                    size, oldValue);
            break;
        case USIM_EFANR_TAG:
            recordNumber = GetAnrRecNumber(adnRecNum - 1, size, oldValue);
            break;
    }
    if (newValue.IsEmpty()) {
        recordNumber = -1;
    }
    Log(String("updateIapFile  efid=") + StringUtils::ToString(efid) +
            ", recordNumber= " + StringUtils::ToString(recordNumber) +
            ", adnRecNum=" + StringUtils::ToString(adnRecNum));
    {
        AutoLock lock(mLock);
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_EF_IAP_RECORD_SIZE_DONE, adnRecNum, recordNumber, CoreUtils::Convert(tag), (IMessage**)&msg);
        mFh->GetEFLinearRecordSize(efid, msg);
        // try {
        mLock.Wait();
        // } catch (InterruptedException e) {
        //     Rlog.e(TAG, "interrupted while trying to update by search");
        // }
    }
    return mSuccess;
}

Int32 CUsimPhoneBookManager::GetEfidByTag(
    /* [in] */ Int32 recNum,
    /* [in] */ Int32 tag,
    /* [in] */ Int32 efidIndex)
{
    AutoPtr<IMap> fileIds;
    Int32 efid = -1;
    Int32 numRecs;
    mPbrFile->mFileIds->GetSize(&numRecs);

    AutoPtr<IInterface> obj;
    mPbrFile->mFileIds->Get(CoreUtils::Convert(recNum), (IInterface**)&obj);
    fileIds = IMap::Probe(obj);
    if (fileIds == NULL)
        return -1;

    Boolean b;
    if (fileIds->ContainsKey(CoreUtils::Convert(tag), &b), b) {
        return -1;
    }

    if (!mEmailPresentInIap && USIM_EFEMAIL_TAG == tag) {
        obj = NULL;
        mPbrFile->mEmailFileIds->Get(CoreUtils::Convert(recNum), (IInterface**)&obj);
        AutoPtr<IArrayList> list = IArrayList::Probe(obj);
        obj = NULL;
        list->Get(efidIndex, (IInterface**)&obj);
        IInteger32::Probe(obj)->GetValue(&efid);
    }
    else if (!mAnrPresentInIap && USIM_EFANR_TAG == tag) {
        obj = NULL;
        mPbrFile->mAnrFileIds->Get(CoreUtils::Convert(recNum), (IInterface**)&obj);
        AutoPtr<IArrayList> list = IArrayList::Probe(obj);
        obj = NULL;
        list->Get(efidIndex, (IInterface**)&obj);
        IInteger32::Probe(obj)->GetValue(&efid);
    }
    else {
        obj = NULL;
        fileIds->Get(CoreUtils::Convert(tag), (IInterface**)&obj);
        IInteger32::Probe(obj)->GetValue(&efid);
    }
    return efid;
}

Int32 CUsimPhoneBookManager::GetInitIndexBy(
    /* [in] */ Int32 pbrIndex)
{
    Int32 index = 0;
    while (pbrIndex > 0) {
        AutoPtr<IInterface> obj;
        mAdnLengthList->Get(pbrIndex - 1, (IInterface**)&obj);
        Int32 val;
        IInteger32::Probe(obj)->GetValue(&val);
        index += val;
        pbrIndex--;
    }
    return index;
}

Boolean CUsimPhoneBookManager::HasRecordIn(
    /* [in] */ IMap* record,
    /* [in] */ Int32 pbrIndex)
{
    if (record == NULL)
        return FALSE;
    // try {
    AutoPtr<IInterface> obj;
    record->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
    // } catch (IndexOutOfBoundsException e) {
    //     Rlog.e(TAG, "record is empty in pbrIndex" + pbrIndex);
    //     return FALSE;
    // }
    return TRUE;
}

void CUsimPhoneBookManager::UpdatePhoneAdnRecordWithEmail(
    /* [in] */ Int32 pbrIndex)
{
    if (!HasRecordIn(mEmailFileRecord, pbrIndex))
        return;

    AutoPtr<IInterface> obj;
    mAdnLengthList->Get(pbrIndex, (IInterface**)&obj);
    Int32 numAdnRecs;
    IInteger32::Probe(obj)->GetValue(&numAdnRecs);

    Int32 adnRecIndex;
    if (mEmailPresentInIap && HasRecordIn(mIapFileRecord, pbrIndex)) {
        // The number of records in the IAP file is same as the number of records in ADN file.
        // The order of the pointers in an EFIAP shall be the same as the order of file IDs
        // that appear in the TLV object indicated by Tag 'A9' in the reference file record.
        // i.e value of mEmailTagNumberInIap

        for (Int32 i = 0; i < numAdnRecs; i++) {
            AutoPtr<ArrayOf<Byte> > record;
            // try {
            obj = NULL;
            mIapFileRecord->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
            AutoPtr<IArrayList> list = IArrayList::Probe(obj);
            obj = NULL;
            list->Get(i, (IInterface**)&obj);
            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(obj);

            Int32 size;
            iArray->GetLength(&size);
            record = ArrayOf<Byte>::Alloc(size);
            for (Int32 j = 0; j < size; j++) {
                AutoPtr<IInterface> o;
                iArray->Get(j, (IInterface**)&o);
                Byte b;
                IByte::Probe(o)->GetValue(&b);
                (*record)[j] = b;
            }

            // } catch (IndexOutOfBoundsException e) {
            //     Rlog.e(TAG, "Error: Improper ICC card: No IAP record for ADN, continuing");
            //     break;
            // }
            Int32 recNum = (*record)[mEmailTagNumberInIap];

            if (recNum > 0) {
                AutoPtr<ArrayOf<String> > emails = ArrayOf<String>::Alloc(1);
                // SIM record numbers are 1 based
                (*emails)[0] = ReadEmailRecord(recNum - 1, pbrIndex, 0);
                adnRecIndex = i + GetInitIndexBy(pbrIndex);
                obj = NULL;
                mPhoneBookRecords->Get(adnRecIndex, (IInterface**)&obj);
                AutoPtr<IAdnRecord> rec = IAdnRecord::Probe(obj);
                if (rec != NULL && (!(*emails)[0].IsEmpty())) {
                    rec->SetEmails(emails);
                    mPhoneBookRecords->Set(adnRecIndex, rec);

                    obj = NULL;
                    mEmailFlags->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
                    AutoPtr<IArrayList> list = IArrayList::Probe(obj);
                    list->Set(recNum - 1, CoreUtils::Convert(1));
                }
            }
        }

        Log(String("updatePhoneAdnRecordWithEmail: no need to parse type1 EMAIL file"));
        return;
    }

    // ICC cards can be made such that they have an IAP file but all
    // records are empty. So we read both type 1 and type 2 file
    // email records, just to be sure.

    obj = NULL;
    mAdnLengthList->Get(pbrIndex, (IInterface**)&obj);
    Int32 len;
    IInteger32::Probe(obj)->GetValue(&len);
    // Type 1 file, the number of records is the same as the number of
    // records in the ADN file.
    if (!mEmailPresentInIap) {
        ParseType1EmailFile(len, pbrIndex);
    }
}

void CUsimPhoneBookManager::UpdatePhoneAdnRecordWithAnr(
    /* [in] */ Int32 pbrIndex)
{
    if (!HasRecordIn(mAnrFileRecord, pbrIndex))
        return;

    AutoPtr<IInterface> obj;
    mAdnLengthList->Get(pbrIndex, (IInterface**)&obj);
    Int32 numAdnRecs;
    IInteger32::Probe(obj)->GetValue(&numAdnRecs);
    Int32 adnRecIndex;
    if (mAnrPresentInIap && HasRecordIn(mIapFileRecord, pbrIndex)) {
        // The number of records in the IAP file is same as the number of records in ADN file.
        // The order of the pointers in an EFIAP shall be the same as the order of file IDs
        // that appear in the TLV object indicated by Tag 'A9' in the reference file record.
        // i.e value of mAnrTagNumberInIap

        for (Int32 i = 0; i < numAdnRecs; i++) {
            AutoPtr<ArrayOf<Byte> > record;
            // try {
            AutoPtr<IInterface> obj;
            mIapFileRecord->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
            AutoPtr<IArrayList> list = IArrayList::Probe(obj);
            obj = NULL;
            list->Get(i, (IInterface**)&obj);
            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(obj);

            Int32 size;
            iArray->GetLength(&size);
            record = ArrayOf<Byte>::Alloc(size);
            for (Int32 j = 0; j < size; j++) {
                AutoPtr<IInterface> o;
                iArray->Get(j, (IInterface**)&o);
                Byte b;
                IByte::Probe(o)->GetValue(&b);
                (*record)[j] = b;
            }

            // } catch (IndexOutOfBoundsException e) {
            //     Rlog.e(TAG, "Error: Improper ICC card: No IAP record for ADN, continuing");
            //     break;
            // }
            Int32 recNum = (*record)[mAnrTagNumberInIap];
            if (recNum > 0) {
                AutoPtr<ArrayOf<String> > anrs = ArrayOf<String>::Alloc(1);
                // SIM record numbers are 1 based
                (*anrs)[0] = ReadAnrRecord(recNum - 1, pbrIndex, 0);
                adnRecIndex = i + GetInitIndexBy(pbrIndex);
                obj = NULL;
                mPhoneBookRecords->Get(adnRecIndex, (IInterface**)&obj);
                AutoPtr<IAdnRecord> rec = IAdnRecord::Probe(obj);
                if (rec != NULL && (!(*anrs)[0].IsEmpty())) {
                    rec->SetAdditionalNumbers(anrs);
                    mPhoneBookRecords->Set(adnRecIndex, rec);
                }

            }
        }

        Log(String("updatePhoneAdnRecordWithAnr: no need to parse type1 ANR file"));
        return;
    }

    // ICC cards can be made such that they have an IAP file but all
    // records are empty. So we read both type 1 and type 2 file
    // anr records, just to be sure.

    // Type 1 file, the number of records is the same as the number of
    // records in the ADN file.
    if (!mAnrPresentInIap) {
        ParseType1AnrFile(numAdnRecs, pbrIndex);
    }
}

String CUsimPhoneBookManager::ReadEmailRecord(
    /* [in] */ Int32 recNum,
    /* [in] */ Int32 pbrIndex,
    /* [in] */ Int32 offSet)
{
    AutoPtr<ArrayOf<Byte> > emailRec;
    if (!HasRecordIn(mEmailFileRecord, pbrIndex))
        return String(NULL);
    // try {
    AutoPtr<IInterface> obj;
    mEmailFileRecord->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
    AutoPtr<IArrayList> list = IArrayList::Probe(obj);
    obj = NULL;
    list->Get(recNum + offSet, (IInterface**)&obj);
    AutoPtr<IArrayOf> iArray = IArrayOf::Probe(obj);

    Int32 size;
    iArray->GetLength(&size);
    emailRec = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> o;
        iArray->Get(i, (IInterface**)&o);
        Byte b;
        IByte::Probe(o)->GetValue(&b);
        (*emailRec)[i] = b;
    }

    // } catch (IndexOutOfBoundsException e) {
    //     return NULL;
    // }

    // The length of the record is X+2 byte, where X bytes is the email address
    String email = IccUtils::AdnStringFieldToString(emailRec, 0, emailRec->GetLength() - 2);
    return email;
}

String CUsimPhoneBookManager::ReadAnrRecord(
    /* [in] */ Int32 recNum,
    /* [in] */ Int32 pbrIndex,
    /* [in] */ Int32 offSet)
{
    AutoPtr<ArrayOf<Byte> > anrRec;
    if (!HasRecordIn(mAnrFileRecord, pbrIndex))
        return String(NULL);
    // try {
    AutoPtr<IInterface> obj;
    mAnrFileRecord->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
    AutoPtr<IArrayList> list = IArrayList::Probe(obj);
    obj = NULL;
    list->Get(recNum + offSet, (IInterface**)&obj);
    AutoPtr<IArrayOf> iArray = IArrayOf::Probe(obj);

    Int32 size;
    iArray->GetLength(&size);
    anrRec = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> o;
        iArray->Get(i, (IInterface**)&o);
        Byte b;
        IByte::Probe(o)->GetValue(&b);
        (*anrRec)[i] = b;
    }

    // } catch (IndexOutOfBoundsException e) {
    //     Rlog.e(TAG, "Error: Improper ICC card: No anr record for ADN, continuing");
    //     return NULL;
    // }
    Int32 numberLength = 0xff & (*anrRec)[1];
    if (numberLength > MAX_NUMBER_SIZE_BYTES) {
        //Log("Invalid number length[" + numberLength + "] in anr record: " + recNum +
        //        " pbrIndex:" + pbrIndex + " offSet:" + offSet);
        return String("");
    }

    String str;
    PhoneNumberUtils::CalledPartyBCDToString(anrRec, 2, numberLength, &str);
    return str;
}

void CUsimPhoneBookManager::ReadAdnFileAndWait(
    /* [in] */ Int32 recNum)
{
    AutoPtr<IInterface> obj;
    mPbrFile->mFileIds->Get(CoreUtils::Convert(recNum), (IInterface**)&obj);
    AutoPtr<IMap> fileIds = IMap::Probe(obj);

    Boolean b;
    if (fileIds == NULL || (fileIds->IsEmpty(&b), b)) return;

    Int32 extEf = 0;
    // Only call fileIds.get while EFEXT1_TAG is available
    if (fileIds->ContainsKey(CoreUtils::Convert(USIM_EFEXT1_TAG), &b), b) {
        AutoPtr<IInterface> o;
        fileIds->Get(CoreUtils::Convert(USIM_EFEXT1_TAG), (IInterface**)&o);
        IInteger32::Probe(o)->GetValue(&extEf);
    }

    obj = NULL;
    fileIds->Get(CoreUtils::Convert(USIM_EFADN_TAG), (IInterface**)&obj);
    Int32 val;
    IInteger32::Probe(obj)->GetValue(&val);
    Log(String("readAdnFileAndWait adn efid is : ") + StringUtils::ToString(val));
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_USIM_ADN_LOAD_DONE, CoreUtils::Convert(recNum), (IMessage**)&msg);
    mAdnCache->RequestLoadAllAdnLike(val, extEf, msg);
    // try {
    mLock.Wait();
    // } catch (InterruptedException e) {
    //     Rlog.e(TAG, "Interrupted Exception in readAdnFileAndWait");
    // }
}

Int32 CUsimPhoneBookManager::GetEmailRecNumber(
    /* [in] */ Int32 adnRecIndex,
    /* [in] */ Int32 numRecs,
    /* [in] */ const String& oldEmail)
{
    Int32 pbrIndex;
    GetPbrIndexBy(adnRecIndex, &pbrIndex);
    Int32 recordIndex = adnRecIndex - GetInitIndexBy(pbrIndex);
    Int32 recordNumber = -1;
    Log(String("getEmailRecNumber adnRecIndex is: ") + StringUtils::ToString(adnRecIndex) +
            ", recordIndex is :" + StringUtils::ToString(recordIndex));
    if (!HasRecordIn(mEmailFileRecord, pbrIndex)) {
        Log(String("getEmailRecNumber recordNumber is: ") + StringUtils::ToString(recordNumber));
        return recordNumber;
    }
    if (mEmailPresentInIap && HasRecordIn(mIapFileRecord, pbrIndex)) {
        AutoPtr<ArrayOf<Byte> > record;
        // try {
        AutoPtr<IInterface> obj;
        mIapFileRecord->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
        AutoPtr<IArrayList> list = IArrayList::Probe(obj);
        obj = NULL;
        list->Get(recordIndex, (IInterface**)&obj);
        AutoPtr<IArrayOf> iArray = IArrayOf::Probe(obj);

        Int32 size;
        iArray->GetLength(&size);
        record = ArrayOf<Byte>::Alloc(size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> o;
            iArray->Get(i, (IInterface**)&o);
            Byte b;
            IByte::Probe(o)->GetValue(&b);
            (*record)[i] = b;
        }
        // } catch (IndexOutOfBoundsException e) {
        //     Rlog.e(TAG, "IndexOutOfBoundsException in getEmailRecNumber");
        // }
        if (record != NULL && (*record)[mEmailTagNumberInIap] > 0) {
            recordNumber = (*record)[mEmailTagNumberInIap];
            Log(String(" getEmailRecNumber: record is ") + IccUtils::BytesToHexString(record)
                    + ", the email recordNumber is :" + StringUtils::ToString(recordNumber));
            return recordNumber;
        }
        else {
            AutoPtr<IInterface> obj;
            mEmailFileRecord->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
            Int32 recsSize;
            IArrayList::Probe(obj)->GetSize(&recsSize);

            Log(String("getEmailRecNumber recsSize is: ") + StringUtils::ToString(recsSize));
            if (oldEmail.IsEmpty()) {
                for (Int32 i = 0; i < recsSize; i++) {
                    String emailRecord = ReadEmailRecord(i, pbrIndex, 0);
                    if (emailRecord.IsEmpty()) {
                        Log(String("getEmailRecNumber: Got empty record.Email record num is :") +
                                 StringUtils::ToString(i + 1));
                        return i + 1;
                    }
                }
            }
        }
    }
    else {
        recordNumber = recordIndex + 1;
        return recordNumber;
    }
    Log(String("getEmailRecNumber: no email record index found"));
    return recordNumber;
}

Int32 CUsimPhoneBookManager::GetAnrRecNumber(
    /* [in] */ Int32 adnRecIndex,
    /* [in] */ Int32 numRecs,
    /* [in] */ const String& oldAnr)
{
    Int32 pbrIndex;
    GetPbrIndexBy(adnRecIndex, &pbrIndex);
    Int32 recordIndex = adnRecIndex - GetInitIndexBy(pbrIndex);
    Int32 recordNumber = -1;
    if (!HasRecordIn(mAnrFileRecord, pbrIndex)) {
        return recordNumber;
    }
    if (mAnrPresentInIap && HasRecordIn(mIapFileRecord, pbrIndex)) {
        AutoPtr<IArrayOf> record;
        // try {
        AutoPtr<IInterface> obj;
        mAnrFileRecord->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
        AutoPtr<IArrayList> list = IArrayList::Probe(obj);
        obj = NULL;
        list->Get(recordIndex, (IInterface**)&obj);
        record = IArrayOf::Probe(obj);

        // } catch (IndexOutOfBoundsException e) {
        //     Rlog.e(TAG, "IndexOutOfBoundsException in getAnrRecNumber");
        // }
        if (record != NULL) {
            obj = NULL;
            record->Get(mAnrTagNumberInIap, (IInterface**)&obj);
            Byte bValue;
            IByte::Probe(obj)->GetValue(&bValue);
            if (bValue > 0) {
                recordNumber = bValue;
                Log(String("getAnrRecNumber: recnum from iap is :") + StringUtils::ToString(recordNumber));
                return recordNumber;
            }
            else {
                AutoPtr<IInterface> obj;
                mAnrFileRecord->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
                Int32 recsSize;
                IArrayList::Probe(obj)->GetSize(&recsSize);
                Log(String("getAnrRecNumber: anr record size is :") + StringUtils::ToString(recsSize));
                if (oldAnr.IsEmpty()) {
                    for (Int32 i = 0; i < recsSize; i++) {
                        String anrRecord = ReadAnrRecord(i, pbrIndex, 0);
                        if (anrRecord.IsEmpty()) {
                            Log(String("getAnrRecNumber: Empty anr record. Anr record num is :") +
                                    StringUtils::ToString(i + 1));
                            return i + 1;
                        }
                    }
                }
            }
        }
    }
    else {
        recordNumber = recordIndex + 1;
        return recordNumber;
    }
    Log(String("getAnrRecNumber: no anr record index found"));
    return recordNumber;
}

AutoPtr<ArrayOf<Byte> > CUsimPhoneBookManager::BuildEmailData(
    /* [in] */ Int32 length,
    /* [in] */ Int32 adnRecIndex,
    /* [in] */ const String& email)
{
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        (*data)[i] = (Byte) 0xff;
    }
    if (email.IsEmpty()) {
        Log(String("[buildEmailData] Empty email record"));
        return data; // return the empty record (for delete)
    }
    AutoPtr<ArrayOf<Byte> > byteEmail;
    GsmAlphabet::StringToGsm8BitPacked(email, (ArrayOf<Byte>**)&byteEmail);
    data->Copy(0, byteEmail, 0, byteEmail->GetLength());
    Int32 pbrIndex;
    GetPbrIndexBy(adnRecIndex, &pbrIndex);
    Int32 recordIndex = adnRecIndex - GetInitIndexBy(pbrIndex);
    if (mEmailPresentInIap) {
        (*data)[length - 1] = (Byte) (recordIndex + 1);
    }
    Log(String("buildEmailData: data is") + IccUtils::BytesToHexString(data));
    return data;
}

AutoPtr<ArrayOf<Byte> > CUsimPhoneBookManager::BuildAnrData(
    /* [in] */ Int32 length,
    /* [in] */ Int32 adnRecIndex,
    /* [in] */ const String& anr)
{
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        (*data)[i] = (Byte) 0xff;
    }
    if (anr.IsEmpty()) {
        Log(String("[buildAnrData] Empty anr record"));
        return data; // return the empty record (for delete)
    }
    (*data)[ANR_DESCRIPTION_ID] = (Byte) (0x0);
    AutoPtr<ArrayOf<Byte> > byteAnr;
    PhoneNumberUtils::NumberToCalledPartyBCD(anr, (ArrayOf<Byte>**)&byteAnr);

    // If the phone number does not matching format, like "+" return NULL.
    if (byteAnr == NULL) {
        return NULL;
    }

    // numberToCalledPartyBCD has considered TOA byte
    Int32 maxlength = ANR_ADDITIONAL_NUMBER_END_ID - ANR_ADDITIONAL_NUMBER_START_ID + 1 + 1;
    if (byteAnr->GetLength() > maxlength) {
        data->Copy(ANR_TON_NPI_ID, byteAnr, 0, maxlength);
        (*data)[ANR_BCD_NUMBER_LENGTH] = (Byte) (maxlength);
    }
    else {
        data->Copy(ANR_TON_NPI_ID, byteAnr, 0, byteAnr->GetLength());
        (*data)[ANR_BCD_NUMBER_LENGTH] = (Byte) (byteAnr->GetLength());
    }
    (*data)[ANR_CAPABILITY_ID] = (Byte) 0xFF;
    (*data)[ANR_EXTENSION_ID] = (Byte) 0xFF;
    if (length == 17) {
        Int32 pbrIndex;
        GetPbrIndexBy(adnRecIndex, &pbrIndex);
        Int32 recordIndex = adnRecIndex - GetInitIndexBy(pbrIndex);
        (*data)[ANR_ADN_RECORD_IDENTIFIER_ID] = (Byte) (recordIndex + 1);
    }
    Log(String("buildAnrData: data is") + IccUtils::BytesToHexString(data));
    return data;
}

void CUsimPhoneBookManager::CreatePbrFile(
    /* [in] */ IArrayList* records)
{
    if (records == NULL) {
        mPbrFile = NULL;
        mIsPbrPresent = FALSE;
        return;
    }
    mPbrFile = new PbrFile(records, this);
}

void CUsimPhoneBookManager::PutValidRecNums(
    /* [in] */ Int32 pbrIndex)
{
    AutoPtr<IArrayList> recordNums;
    CArrayList::New((IArrayList**)&recordNums);
    Int32 initAdnIndex = GetInitIndexBy(pbrIndex);
    Log(String("pbr index is ") + StringUtils::ToString(pbrIndex) +
            ", initAdnIndex is " + StringUtils::ToString(initAdnIndex));

    AutoPtr<IInterface> obj;
    mAdnLengthList->Get(pbrIndex, (IInterface**)&obj);
    Int32 val;
    IInteger32::Probe(obj)->GetValue(&val);
    for (Int32 i = 0; i < val; i++) {
        recordNums->Add(CoreUtils::Convert(i + 1));
    }
    // Need to read at least one record to inint
    // variable mIapFileRecord, mEmailFileRecord,mAnrFileRecord
    Int32 size;
    recordNums->GetSize(&size);
    if (size == 0) {
        recordNums->Add(CoreUtils::Convert(1));
    }
    mRecordNums->Put(CoreUtils::Convert(pbrIndex), recordNums);
}

AutoPtr<IArrayList> CUsimPhoneBookManager::GetValidRecordNums(
    /* [in] */ Int32 pbrIndex)
{
    AutoPtr<IInterface> obj;
    mRecordNums->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
    return IArrayList::Probe(obj);
}

Boolean CUsimPhoneBookManager::HasValidRecords(
    /* [in] */ Int32 pbrIndex)
{
    AutoPtr<IInterface> obj;
    mRecordNums->Get(CoreUtils::Convert(pbrIndex), (IInterface**)&obj);
    AutoPtr<IArrayList> list = IArrayList::Probe(obj);
    Int32 size;
    list->GetSize(&size);
    return size > 0;
}

void CUsimPhoneBookManager::Log(
    /* [in] */ const String& msg)
{
    if(DBG) Logger::D(TAG, msg);
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos