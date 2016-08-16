#include "Elastos.CoreLibrary.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IsimUiccRecords.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/internal/telephony/uicc/CIccUtils.h"
#include "elastos/droid/internal/telephony/uicc/CAdnRecordCache.h"
#include "elastos/droid/internal/telephony/gsm/CSimTlv.h"

#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Telephony::Gsm::ISimTlv;
using Elastos::Droid::Internal::Telephony::Gsm::CSimTlv;

using Elastos::Core::AutoLock;
using Elastos::Core::IByte;
using Elastos::Core::StringUtils;
using Elastos::IO::IFlushable;
using Elastos::IO::Charset::ICharset;
using Elastos::Utility::Arrays;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                  IsimUiccRecords::EfIsimImpiLoaded
//=====================================================================
CAR_INTERFACE_IMPL(IsimUiccRecords::EfIsimImpiLoaded, Object, IIccRecordLoaded);

IsimUiccRecords::EfIsimImpiLoaded::EfIsimImpiLoaded(
    /* [in] */ IsimUiccRecords* host)
    : mHost(host)
{
}

ECode IsimUiccRecords::EfIsimImpiLoaded::GetEfName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = String("EF_ISIM_IMPI");
    return NOERROR;
}

ECode IsimUiccRecords::EfIsimImpiLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IArrayList> pArr = IArrayList::Probe(((AsyncResult*)ar)->mResult);
    Int32 size = 0;
    pArr->GetSize(&size);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> p;
        pArr->Get(i, (IInterface**)&p);
        IByte::Probe(p)->GetValue(&((*data)[i]));
    }
    mHost->mIsimImpi = mHost->IsimTlvToString(data);
    if (DUMP_RECORDS) mHost->Log(String("EF_IMPI=") + mHost->mIsimImpi);
    return NOERROR;
}

//=====================================================================
//                  IsimUiccRecords::EfIsimImpuLoaded
//=====================================================================
CAR_INTERFACE_IMPL(IsimUiccRecords::EfIsimImpuLoaded, Object, IIccRecordLoaded);

IsimUiccRecords::EfIsimImpuLoaded::EfIsimImpuLoaded(
    /* [in] */ IsimUiccRecords* host)
    : mHost(host)
{
}

ECode IsimUiccRecords::EfIsimImpuLoaded::GetEfName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = String("EF_ISIM_IMPU");
    return NOERROR;
}

ECode IsimUiccRecords::EfIsimImpuLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IArrayList> impuList = IArrayList::Probe(((AsyncResult*)ar)->mResult);
    Int32 size = 0;
    impuList->GetSize(&size);
    if (DBG) {
        mHost->Log(String("EF_IMPU record count: ") + StringUtils::ToString(size));
    }
    mHost->mIsimImpu = ArrayOf<String>::Alloc(size);
    Int32 i = 0;
    AutoPtr<IIterator> it;
    impuList->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IArrayList> pArr = IArrayList::Probe(p);
        Int32 size = 0;
        pArr->GetSize(&size);
        AutoPtr<ArrayOf<Byte> > identity = ArrayOf<Byte>::Alloc(size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> v;
            pArr->Get(i, (IInterface**)&v);
            IByte::Probe(v)->GetValue(&((*identity)[i]));
        }
        String impu = IsimTlvToString(identity);
        if (DUMP_RECORDS) {
            mHost->Log(String("EF_IMPU[") + StringUtils::ToString(i) + String("]=") + impu);
        }
        (*(mHost->mIsimImpu))[i++] = impu;
    }
    return NOERROR;
}

//=====================================================================
//                 IsimUiccRecords::EfIsimDomainLoaded
//=====================================================================
CAR_INTERFACE_IMPL(IsimUiccRecords::EfIsimDomainLoaded, Object, IIccRecordLoaded);

IsimUiccRecords::EfIsimDomainLoaded::EfIsimDomainLoaded(
    /* [in] */ IsimUiccRecords* host)
    : mHost(host)
{
}

ECode IsimUiccRecords::EfIsimDomainLoaded::GetEfName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = String("EF_ISIM_DOMAIN");
    return NOERROR;
}

ECode IsimUiccRecords::EfIsimDomainLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IArrayList> pArr = IArrayList::Probe(((AsyncResult*)ar)->mResult);
    Int32 size = 0;
    pArr->GetSize(&size);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> v;
        pArr->Get(i, (IInterface**)&v);
        IByte::Probe(v)->GetValue(&((*data)[i]));
    }
    mHost->mIsimDomain = IsimTlvToString(data);
    if (DUMP_RECORDS) {
        mHost->Log(String("EF_DOMAIN=") + mHost->mIsimDomain);
    }
    return NOERROR;
}

//=====================================================================
//                   IsimUiccRecords::EfIsimIstLoaded
//=====================================================================
CAR_INTERFACE_IMPL(IsimUiccRecords::EfIsimIstLoaded, Object, IIccRecordLoaded);

IsimUiccRecords::EfIsimIstLoaded::EfIsimIstLoaded(
    /* [in] */ IsimUiccRecords* host)
    : mHost(host)
{
}

ECode IsimUiccRecords::EfIsimIstLoaded::GetEfName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = String("EF_ISIM_IST");
    return NOERROR;
}

ECode IsimUiccRecords::EfIsimIstLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IArrayList> pArr = IArrayList::Probe(((AsyncResult*)ar)->mResult);
    Int32 size = 0;
    pArr->GetSize(&size);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> v;
        pArr->Get(i, (IInterface**)&v);
        IByte::Probe(v)->GetValue(&((*data)[i]));
    }
    AutoPtr<IIccUtils> iccu;
    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    iccu->BytesToHexString(data, &(mHost->mIsimIst));
    if (DUMP_RECORDS) mHost->Log(String("EF_IST=") + mHost->mIsimIst);
    return NOERROR;
}

//=====================================================================
//                  IsimUiccRecords::EfIsimPcscfLoaded
//=====================================================================
CAR_INTERFACE_IMPL(IsimUiccRecords::EfIsimPcscfLoaded, Object, IIccRecordLoaded);

IsimUiccRecords::EfIsimPcscfLoaded::EfIsimPcscfLoaded(
    /* [in] */ IsimUiccRecords* host)
    : mHost(host)
{
}

ECode IsimUiccRecords::EfIsimPcscfLoaded::GetEfName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = String("EF_ISIM_PCSCF");
    return NOERROR;
}

ECode IsimUiccRecords::EfIsimPcscfLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IArrayList> pcscflist = IArrayList::Probe(((AsyncResult*)ar)->mResult);
    Int32 size = 0;
    pcscflist->GetSize(&size);
    if (DBG) mHost->Log(String("EF_PCSCF record count: ") + StringUtils::ToString(size));
    mHost->mIsimPcscf = ArrayOf<String>::Alloc(size);
    Int32 i = 0;
    AutoPtr<IIterator> it;
    pcscflist->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IArrayList> pArr = IArrayList::Probe(p);
        Int32 size = 0;
        pArr->GetSize(&size);
        AutoPtr<ArrayOf<Byte> > identity = ArrayOf<Byte>::Alloc(size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> v;
            pArr->Get(i, (IInterface**)&v);
            IByte::Probe(v)->GetValue(&((*identity)[i]));
        }
        String pcscf = IsimTlvToString(identity);
        if (DUMP_RECORDS) mHost->Log(String("EF_PCSCF[") + StringUtils::ToString(i) + String("]=") + pcscf);
        (*(mHost->mIsimPcscf))[i++] = pcscf;
    }
    return NOERROR;
}

//=====================================================================
//                           IsimUiccRecords
//=====================================================================
CAR_INTERFACE_IMPL(IsimUiccRecords, IccRecords, IIsimRecords);

const String IsimUiccRecords::LOGTAG("IsimUiccRecords");
const Boolean IsimUiccRecords::DBG = TRUE;
const Boolean IsimUiccRecords::DUMP_RECORDS = TRUE;
const Int32 IsimUiccRecords::EVENT_APP_READY;
const Int32 IsimUiccRecords::EVENT_AKA_AUTHENTICATE_DONE;
const Int32 IsimUiccRecords::TAG_ISIM_VALUE;

IsimUiccRecords::IsimUiccRecords()
{
}

ECode IsimUiccRecords::constructor(
    /* [in] */ IUiccCardApplication* app,
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci)
{
    IccRecords::constructor(app, c, ci);

    CAdnRecordCache::New(mFh, (IAdnRecordCache**)&mAdnCache);

    mRecordsRequested = FALSE;  // No load request is made till SIM ready

    // recordsToLoad is set to 0 because no requests are made yet
    mRecordsToLoad = 0;
    // Start off by setting empty state
    ResetRecords();

    mParentApp->RegisterForReady(this, EVENT_APP_READY, NULL);
    if (DBG) {
        Log(String("IsimUiccRecords X ctor this=") + TO_CSTR(this));
    }
    return NOERROR;
}

ECode IsimUiccRecords::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String str;
    IccRecords::ToString(&str);
    assert(0 && "TODO");
    // *result = String("IsimUiccRecords: ") + str
    //         + String(" mIsimImpi=") + mIsimImpi
    //         + String(" mIsimDomain=") + mIsimDomain
    //         + String(" mIsimImpu=") + mIsimImpu
    //         + String(" mIsimIst=") + mIsimIst
    //         + String(" mIsimPcscf=") + mIsimPcscf;
    return NOERROR;
}

ECode IsimUiccRecords::Dispose()
{
    Log(String("Disposing ") + TO_CSTR(this));
    //Unregister for all events
    mParentApp->UnregisterForReady(this);
    ResetRecords();
    IccRecords::Dispose();
    return NOERROR;
}

ECode IsimUiccRecords::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;

    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    Boolean b = FALSE;
    mDestroyed->Get(&b);
    if (b) {
        Logger::E(LOGTAG, String("Received message ") + TO_CSTR(msg) +
                String("[") + StringUtils::ToString(what) +
                String("] while being destroyed. Ignoring."));
        return NOERROR;
    }
    Loge(String("IsimUiccRecords: handleMessage ") + TO_CSTR(msg)
        + String("[") + StringUtils::ToString(what) + String("] "));

    // try {
        switch (what) {
            case EVENT_APP_READY:
                OnReady();
                break;

            case EVENT_AKA_AUTHENTICATE_DONE:
                ar = (AsyncResult*)(IObject*)obj.Get();
                Log(String("EVENT_AKA_AUTHENTICATE_DONE"));
                if (ar->mException != NULL) {
                    Log(String("Exception ISIM AKA: ") + TO_CSTR(ar->mException));
                }
                else {
                    // try {
                        ICharSequence::Probe(ar->mResult)->ToString(&mAuth_rsp);
                        Log(String("ISIM AKA: auth_rsp = ") + mAuth_rsp);
                    // } catch (Exception e) {
                    //     log("Failed to parse ISIM AKA contents: " + e);
                    // }
                }
                {
                    AutoLock lock(mLock);
                    mLock.NotifyAll();
                }
                break;

            default:
                IccRecords::HandleMessage(msg);   // IccRecords handles generic record load responses
        }
    // } catch (RuntimeException exc) {
    //     // I don't want these exceptions to be fatal
    //     Logger.w(LOGTAG, "Exception parsing SIM record", exc);
    // }
    return NOERROR;
}

ECode IsimUiccRecords::GetIsimImpi(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsimImpi;
    return NOERROR;
}

ECode IsimUiccRecords::GetIsimDomain(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsimDomain;
    return NOERROR;
}

ECode IsimUiccRecords::GetIsimImpu(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mIsimImpu != NULL) ? mIsimImpu->Clone() : NULL;
    return NOERROR;
}

ECode IsimUiccRecords::GetIsimIst(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsimIst;
    return NOERROR;
}

ECode IsimUiccRecords::GetIsimPcscf(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mIsimPcscf != NULL) ? mIsimPcscf->Clone() : NULL;
    return NOERROR;
}

ECode IsimUiccRecords::GetIsimChallengeResponse(
    /* [in] */ const String& nonce,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) {
        Log(String("getIsimChallengeResponse-nonce:") + nonce);
    }
    // try {
        {
            AutoLock lock(mLock);
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_AKA_AUTHENTICATE_DONE, (IMessage**)&msg);
            mCi->RequestIsimAuthentication(nonce, msg);
            // try {
                mLock.Wait();
            // } catch (InterruptedException e) {
            //     log("interrupted while trying to request Isim Auth");
            // }
        }
    // } catch(Exception e) {
    //     if (DBG) log( "Fail while trying to request Isim Auth");
    //     return null;
    // }

    if (DBG) Log(String("getIsimChallengeResponse-auth_rsp") + mAuth_rsp);

    *result = mAuth_rsp;
    return NOERROR;
}

ECode IsimUiccRecords::GetDisplayRule(
    /* [in] */ const String& plmn,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // Not applicable to Isim
    *result = 0;
    return NOERROR;
}

ECode IsimUiccRecords::OnReady()
{
    FetchIsimRecords();
    return NOERROR;
}

ECode IsimUiccRecords::OnRefresh(
    /* [in] */ Boolean fileChanged,
    /* [in] */ ArrayOf<Int32>* fileList)
{
    if (fileChanged) {
        // A future optimization would be to inspect fileList and
        // only reload those files that we care about.  For now,
        // just re-fetch all SIM records that we cache.
        FetchIsimRecords();
    }
    return NOERROR;
}

ECode IsimUiccRecords::SetVoiceMailNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& voiceNumber,
    /* [in] */ IMessage* onComplete)
{
    // Not applicable to Isim
    return NOERROR;
}

ECode IsimUiccRecords::SetVoiceMessageWaiting(
    /* [in] */ Int32 line,
    /* [in] */ Int32 countWaiting)
{
    // Not applicable to Isim
    return NOERROR;
}

ECode IsimUiccRecords::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("IsimRecords: ") + TO_CSTR(this));
    pw->Println(String(" extends:"));
    IccRecords::Dump(fd, pw, args);
    pw->Println(String(" mIsimImpi=") + mIsimImpi);
    pw->Println(String(" mIsimDomain=") + mIsimDomain);
    pw->Println(String(" mIsimImpu[]=") + Arrays::ToString(mIsimImpu));
    pw->Println(String(" mIsimIst") + mIsimIst);
    pw->Println(String(" mIsimPcscf") + Arrays::ToString(mIsimPcscf));
    IFlushable::Probe(pw)->Flush();
    return NOERROR;
}

ECode IsimUiccRecords::GetVoiceMessageCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0; // Not applicable to Isim
    return NOERROR;
}

void IsimUiccRecords::FetchIsimRecords()
{
    mRecordsRequested = TRUE;

    AutoPtr<EfIsimImpiLoaded> pImpi = new EfIsimImpiLoaded(this);
    AutoPtr<IMessage> msg1;
    ObtainMessage(
            IIccRecords::EVENT_GET_ICC_RECORD_DONE, IIccRecordLoaded::Probe(pImpi), (IMessage**)&msg1);
    mFh->LoadEFTransparent(EF_IMPI, msg1);
    mRecordsToLoad++;

    AutoPtr<EfIsimImpuLoaded> pImpu = new EfIsimImpuLoaded(this);
    AutoPtr<IMessage> msg2;
    ObtainMessage(
            IIccRecords::EVENT_GET_ICC_RECORD_DONE, IIccRecordLoaded::Probe(pImpu), (IMessage**)&msg2);
    mFh->LoadEFLinearFixedAll(EF_IMPU, msg2);
    mRecordsToLoad++;

    AutoPtr<EfIsimDomainLoaded> pDomain = new EfIsimDomainLoaded(this);
    AutoPtr<IMessage> msg3;
    ObtainMessage(
            IIccRecords::EVENT_GET_ICC_RECORD_DONE, IIccRecordLoaded::Probe(pDomain), (IMessage**)&msg3);
    mFh->LoadEFTransparent(EF_DOMAIN, msg3);
    mRecordsToLoad++;

    AutoPtr<EfIsimIstLoaded> pIst = new EfIsimIstLoaded(this);
    AutoPtr<IMessage> msg4;
    ObtainMessage(
            IIccRecords::EVENT_GET_ICC_RECORD_DONE, IIccRecordLoaded::Probe(pIst), (IMessage**)&msg4);
    mFh->LoadEFTransparent(EF_IST, msg4);
    mRecordsToLoad++;

    AutoPtr<EfIsimPcscfLoaded> pPcscf = new EfIsimPcscfLoaded(this);
    AutoPtr<IMessage> msg5;
    ObtainMessage(
            IIccRecords::EVENT_GET_ICC_RECORD_DONE, IIccRecordLoaded::Probe(pPcscf), (IMessage**)&msg5);
    mFh->LoadEFLinearFixedAll(EF_PCSCF, msg5);
    mRecordsToLoad++;

    if (DBG) Log(String("fetchIsimRecords ") + StringUtils::ToString(mRecordsToLoad)
                + String(" requested: ") + StringUtils::ToString(mRecordsRequested));
}

void IsimUiccRecords::ResetRecords()
{
    // recordsRequested is set to false indicating that the SIM
    // read requests made so far are not valid. This is set to
    // true only when fresh set of read requests are made.
    mIsimImpi = NULL;
    mIsimDomain = NULL;
    mIsimImpu = NULL;
    mIsimIst = NULL;
    mIsimPcscf = NULL;
    auth_rsp = NULL;

    mRecordsRequested = FALSE;
}

ECode IsimUiccRecords::OnRecordLoaded()
{
    // One record loaded successfully or failed, In either case
    // we need to update the recordsToLoad count
    mRecordsToLoad -= 1;
    if (DBG) Log(String("onRecordLoaded ") + StringUtils::ToString(mRecordsToLoad)
                + String(" requested: ") + StringUtils::ToString(mRecordsRequested));

    if (mRecordsToLoad == 0 && mRecordsRequested == TRUE) {
        OnAllRecordsLoaded();
    }
    else if (mRecordsToLoad < 0) {
        Loge(String("recordsToLoad <0, programmer error suspected"));
        mRecordsToLoad = 0;
    }
    return NOERROR;
}

ECode IsimUiccRecords::OnAllRecordsLoaded()
{
    if (DBG) Log(String("record load complete"));
    AutoPtr<AsyncResult> p = new AsyncResult(NULL, NULL, NULL);
    mRecordsLoadedRegistrants->NotifyRegistrants(p);
    return NOERROR;
}

ECode IsimUiccRecords::HandleFileUpdate(
    /* [in] */ Int32 efid)
{
    switch (efid) {
        case EF_IMPI: {
            AutoPtr<EfIsimImpiLoaded> p = new EfIsimImpiLoaded(this);
            AutoPtr<IMessage> msg;
            ObtainMessage(
                    IIccRecords::EVENT_GET_ICC_RECORD_DONE, IIccRecordLoaded::Probe(p), (IMessage**)&msg);
            mFh->LoadEFTransparent(EF_IMPI, msg);
            mRecordsToLoad++;
            break;
        }
        case EF_IMPU: {
            AutoPtr<EfIsimImpuLoaded> p = new EfIsimImpuLoaded(this);
            AutoPtr<IMessage> msg;
            ObtainMessage(
                    IIccRecords::EVENT_GET_ICC_RECORD_DONE, IIccRecordLoaded::Probe(p), (IMessage**)&msg);
            mFh->LoadEFLinearFixedAll(EF_IMPU, msg);
            mRecordsToLoad++;
        break;
        }
        case EF_DOMAIN: {
            AutoPtr<EfIsimDomainLoaded> p = new EfIsimDomainLoaded(this);
            AutoPtr<IMessage> msg;
            ObtainMessage(
                    IIccRecords::EVENT_GET_ICC_RECORD_DONE, IIccRecordLoaded::Probe(p), (IMessage**)&msg);
            mFh->LoadEFTransparent(EF_DOMAIN, msg);
            mRecordsToLoad++;
        break;
        }
        case EF_IST: {
            AutoPtr<EfIsimIstLoaded> p = new EfIsimIstLoaded(this);
            AutoPtr<IMessage> msg;
            ObtainMessage(
                        IIccRecords::EVENT_GET_ICC_RECORD_DONE, IIccRecordLoaded::Probe(p), (IMessage**)&msg);
            mFh->LoadEFTransparent(EF_IST, msg);
            mRecordsToLoad++;
        break;
        }
        case EF_PCSCF: {
            AutoPtr<EfIsimPcscfLoaded> p = new EfIsimPcscfLoaded(this);
            AutoPtr<IMessage> msg;
            ObtainMessage(
                        IIccRecords::EVENT_GET_ICC_RECORD_DONE, IIccRecordLoaded::Probe(p), (IMessage**)&msg);
            mFh->LoadEFLinearFixedAll(EF_PCSCF, msg);
            mRecordsToLoad++;
        }
        default:
            FetchIsimRecords();
            break;
    }
    return NOERROR;
}

ECode IsimUiccRecords::BroadcastRefresh()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIsimUiccRecords::INTENT_ISIM_REFRESH, (IIntent**)&intent);
    Log(String("send ISim REFRESH: ") + IIsimUiccRecords::INTENT_ISIM_REFRESH);
    mContext->SendBroadcast(intent);
    return NOERROR;
}

ECode IsimUiccRecords::Log(
    /* [in] */ const String& s)
{
    if (DBG) Logger::D(LOGTAG, String("[ISIM] ") + s);
    return NOERROR;
}

ECode IsimUiccRecords::Loge(
    /* [in] */ const String& s)
{
    if (DBG) Logger::E(LOGTAG, String("[ISIM] ") + s);
    return NOERROR;
}

String IsimUiccRecords::IsimTlvToString(
    /* [in] */ ArrayOf<Byte>* record)
{
    AutoPtr<ISimTlv> tlv;
    CSimTlv::New(record, 0, record->GetLength(), (ISimTlv**)&tlv);
    Boolean bHasNext = FALSE;
    do {
        Int32 tag = 0;
        tlv->GetTag(&tag);
        if (tag == TAG_ISIM_VALUE) {
            AutoPtr<ArrayOf<Byte> > data;
            tlv->GetData((ArrayOf<Byte>**)&data);
            AutoPtr<ICharset> cs;
            assert(0 && "TODO");
            // Elastos::IO::Charset::Charset::ForName(String("UTF-8"), (ICharset**)&cs);
            // return String(data, cs);
            return String(*data);
        }
    } while ((tlv->NextObject(&bHasNext), bHasNext));

    Logger::E(LOGTAG, String("[ISIM] can't find TLV tag in ISIM record, returning null"));
    return String(NULL);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
