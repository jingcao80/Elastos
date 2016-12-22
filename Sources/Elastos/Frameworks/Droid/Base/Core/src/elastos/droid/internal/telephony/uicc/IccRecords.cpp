#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/os/RegistrantList.h"
#include "elastos/droid/internal/telephony/uicc/IccRecords.h"
#include "elastos/droid/internal/telephony/uicc/IccIoResult.h"
#include "elastos/droid/internal/telephony/uicc/CAdnRecord.h"
#include "elastos/droid/internal/telephony/uicc/CAdnRecordLoader.h"
#include "elastos/droid/internal/telephony/uicc/CIccRefreshResponse.h"
#include "elastos/droid/internal/telephony/uicc/CIccCardApplicationStatus.h"
#include "elastos/droid/internal/telephony/uicc/CUiccControllerHelper.h"
#include "elastos/droid/internal/telephony/uicc/CIccRefreshResponse.h"
#include "elastos/droid/internal/telephony/CSubscriptionControllerHelper.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"
#include "elastos/droid/utility/CBase64.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::CSubscriptionControllerHelper;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Utility::CBase64;
using Elastos::Droid::Utility::IBase64;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::AutoLock;
using Elastos::IO::IFlushable;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                              IccRecords
//=====================================================================
CAR_INTERFACE_IMPL_2(IccRecords, Handler, IIccConstants, IIccRecords);

const Boolean IccRecords::DBG = TRUE;
const Int32 IccRecords::UNINITIALIZED;
const Int32 IccRecords::UNKNOWN;
const Int32 IccRecords::EVENT_SET_MSISDN_DONE;
const Int32 IccRecords::EVENT_GET_SMS_RECORD_SIZE_DONE;
const Int32 IccRecords::EVENT_APP_READY;
const Int32 IccRecords::EVENT_AKA_AUTHENTICATE_DONE;

IccRecords::IccRecords()
{
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mDestroyed);

    mRecordsLoadedRegistrants = new RegistrantList();
    mImsiReadyRegistrants = new RegistrantList();
    mRecordsEventsRegistrants = new RegistrantList();
    mNewSmsRegistrants = new RegistrantList();
    mNetworkSelectionModeAutomaticRegistrants = new RegistrantList();

    mRecordsToLoad = 0;

    mRecordsRequested = FALSE;
    mIsVoiceMailFixed = FALSE;

    mMncLength = UNINITIALIZED;
    mMailboxIndex = 0; // 0 is no mailbox dailing number associated
    mSmsCountOnIcc = -1;

    mOEMHookSimRefresh = FALSE;
}

ECode IccRecords::constructor(
    /* [in] */ IUiccCardApplication* app,
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci)
{
    Handler::constructor();
    mContext = c;
    mCi = ci;
    app->GetIccFileHandler((IIccFileHandler**)&mFh);
    mParentApp = app;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetBoolean(
            R::bool_::config_sim_refresh_for_dual_mode_card, &mOEMHookSimRefresh);
    if (mOEMHookSimRefresh) {
        mCi->RegisterForSimRefreshEvent(this, EVENT_REFRESH_OEM, NULL);
    }
    else {
        mCi->RegisterForIccRefresh(this, EVENT_REFRESH, NULL);
    }
    return NOERROR;
}

ECode IccRecords::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder sb("mDestroyed=");
    sb.Append(mDestroyed);
    *result = String("mDestroyed=") + TO_CSTR(mDestroyed);
    sb += String(" mContext=");
    sb += TO_CSTR(mContext);
    sb += String(" mCi=");
    sb += TO_CSTR(mCi);
    sb += String(" mFh=");
    sb += TO_CSTR(mFh);
    sb += String(" mParentApp=");
    sb += TO_CSTR(mParentApp);
    sb += String(" recordsLoadedRegistrants=");
    sb += TO_CSTR(mRecordsLoadedRegistrants);
    sb += String(" mImsiReadyRegistrants=");
    sb += TO_CSTR(mImsiReadyRegistrants);
    sb += String(" mRecordsEventsRegistrants=");
    sb += TO_CSTR(mRecordsEventsRegistrants);
    sb += String(" mNewSmsRegistrants=");
    sb += TO_CSTR(mNewSmsRegistrants);
    sb += String(" mNetworkSelectionModeAutomaticRegistrants=");
    sb += TO_CSTR(mNetworkSelectionModeAutomaticRegistrants);
    sb += String(" recordsToLoad=") + StringUtils::ToString(mRecordsToLoad);
    sb += String(" adnCache=");
    sb += TO_CSTR(mAdnCache);
    sb += String(" recordsRequested=") + StringUtils::ToString(mRecordsRequested);
    sb += String(" iccid=") + mIccId;
    sb += String(" msisdn=") + mMsisdn;
    sb += String(" msisdnTag=") + mMsisdnTag;
    sb += String(" voiceMailNum=") + mVoiceMailNum;
    sb += String(" voiceMailTag=") + mVoiceMailTag;
    sb += String(" newVoiceMailNum=") + mNewVoiceMailNum;
    sb += String(" newVoiceMailTag=") + mNewVoiceMailTag;
    sb += String(" isVoiceMailFixed=") + StringUtils::ToString(mIsVoiceMailFixed);
    sb += String(" mImsi=") + mImsi;
    sb += String(" mncLength=") + StringUtils::ToString(mMncLength);
    sb += String(" mailboxIndex=") + StringUtils::ToString(mMailboxIndex);
    sb += String(" spn=") + mSpn;
    return sb.ToString(result);
}

ECode IccRecords::Dispose()
{
    mDestroyed->Set(TRUE);
    if (mOEMHookSimRefresh) {
        mCi->UnregisterForSimRefreshEvent(this);
    }
    else {
        mCi->UnregisterForIccRefresh(this);
    }
    mParentApp = NULL;
    mFh = NULL;
    mCi = NULL;
    mContext = NULL;
    return NOERROR;
}

ECode IccRecords::RecordsRequired()
{
    return NOERROR;
}

ECode IccRecords::GetAdnCache(
    /* [out] */ IAdnRecordCache** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAdnCache;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode IccRecords::GetIccId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIccId;
    return NOERROR;
}

ECode IccRecords::RegisterForRecordsLoaded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    Boolean bDestroyed = FALSE;
    mDestroyed->Get(&bDestroyed);
    if (bDestroyed) {
        return NOERROR;
    }

    for (Int32 i = mRecordsLoadedRegistrants->GetSize() - 1; i >= 0 ; i--) {
        AutoPtr<IRegistrant> r = IRegistrant::Probe(mRecordsLoadedRegistrants->Get(i));
        AutoPtr<IHandler> rH;
        r->GetHandler((IHandler**)&rH);

        if (rH != NULL && Object::Equals(rH, h)) {
            return NOERROR;
        }
    }
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mRecordsLoadedRegistrants->Add(r);

    if (mRecordsToLoad == 0 && mRecordsRequested == TRUE) {
        AutoPtr<AsyncResult> p = new AsyncResult(NULL, NULL, NULL);
        r->NotifyRegistrant(p);
    }
    return NOERROR;
}

ECode IccRecords::UnregisterForRecordsLoaded(
    /* [in] */ IHandler* h)
{
    mRecordsLoadedRegistrants->Remove(h);
    return NOERROR;
}

ECode IccRecords::RegisterForImsiReady(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    Boolean bDestroyed = FALSE;
    mDestroyed->Get(&bDestroyed);
    if (bDestroyed) {
        return NOERROR;
    }

    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mImsiReadyRegistrants->Add(r);

    if (mImsi != NULL) {
        AutoPtr<AsyncResult> p = new AsyncResult(NULL, NULL, NULL);
        r->NotifyRegistrant(p);
    }
    return NOERROR;
}

ECode IccRecords::UnregisterForImsiReady(
    /* [in] */ IHandler* h)
{
    mImsiReadyRegistrants->Remove(h);
    return NOERROR;
}

ECode IccRecords::RegisterForRecordsEvents(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mRecordsEventsRegistrants->Add(r);

    /* Notify registrant of all the possible events. This is to make sure registrant is
    notified even if event occurred in the past. */
    r->NotifyResult(CoreUtils::Convert(EVENT_MWI));
    r->NotifyResult(CoreUtils::Convert(EVENT_CFI));
    return NOERROR;
}

ECode IccRecords::UnregisterForRecordsEvents(
    /* [in] */ IHandler* h)
{
    mRecordsEventsRegistrants->Remove(h);
    return NOERROR;
}

ECode IccRecords::RegisterForNewSms(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mNewSmsRegistrants->Add(r);
    return NOERROR;
}

ECode IccRecords::UnregisterForNewSms(
    /* [in] */ IHandler* h)
{
    mNewSmsRegistrants->Remove(h);
    return NOERROR;
}

ECode IccRecords::RegisterForNetworkSelectionModeAutomatic(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mNetworkSelectionModeAutomaticRegistrants->Add(r);
    return NOERROR;
}

ECode IccRecords::UnregisterForNetworkSelectionModeAutomatic(
    /* [in] */ IHandler* h)
{
    mNetworkSelectionModeAutomaticRegistrants->Remove(h);
    return NOERROR;
}

ECode IccRecords::GetIMSI(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode IccRecords::SetImsi(
    /* [in] */ const String& imsi)
{
    mImsi = imsi;
    mImsiReadyRegistrants->NotifyRegistrants();
    return NOERROR;
}

ECode IccRecords::GetMsisdnNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMsisdn;
    return NOERROR;
}

ECode IccRecords::GetGid1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode IccRecords::SetMsisdnNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ IMessage* onComplete)
{
    mMsisdn = number;
    mMsisdnTag = alphaTag;

    if (DBG) Log(String("Set MSISDN: ") + mMsisdnTag + String(" ") + mMsisdn);

    AutoPtr<IAdnRecord> adn;
    CAdnRecord::New(mMsisdnTag, mMsisdn, (IAdnRecord**)&adn);

    AutoPtr<IAdnRecordLoader> p;
    CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&p);
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_SET_MSISDN_DONE, onComplete, (IMessage**)&msg);
    p->UpdateEF(adn, EF_MSISDN, EF_EXT1, 1, String(NULL), msg);
    return NOERROR;
}

ECode IccRecords::GetMsisdnAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMsisdnTag;
    return NOERROR;
}

ECode IccRecords::GetVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVoiceMailNum;
    return NOERROR;
}

ECode IccRecords::GetServiceProviderName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String providerName = mSpn;

    // Check for NULL pointers, mParentApp can be NULL after dispose,
    // which did occur after removing a SIM.
    AutoPtr<IUiccCardApplication> parentApp = mParentApp;
    if (parentApp != NULL) {
        AutoPtr<IUiccCard> card;
        parentApp->GetUiccCard((IUiccCard**)&card);
        if (card != NULL) {
            String brandOverride;
            card->GetOperatorBrandOverride(&brandOverride);
            if (brandOverride != NULL) {
                Log(String("getServiceProviderName: override"));
                providerName = brandOverride;
            }
            else {
                Log(String("getServiceProviderName: no brandOverride"));
            }
        }
        else {
            Log(String("getServiceProviderName: card is NULL"));
        }
    }
    else {
        Log(String("getServiceProviderName: mParentApp is NULL"));
    }
    Log(String("getServiceProviderName: providerName=") + providerName);
    *result = providerName;
    return NOERROR;
}

ECode IccRecords::GetVoiceMailAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVoiceMailTag;
    return NOERROR;
}

ECode IccRecords::GetRecordsLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mRecordsToLoad == 0 && mRecordsRequested == TRUE) {
        *result = TRUE;
    }
    else {
        *result = FALSE;
    }
    return NOERROR;
}

ECode IccRecords::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    AutoPtr<AsyncResult> ar;
    switch (what) {
        case EVENT_GET_ICC_RECORD_DONE: {
            // try {
                ar = (AsyncResult*)IAsyncResult::Probe(obj);
                AutoPtr<IIccRecordLoaded> recordLoaded = IIccRecordLoaded::Probe(ar->mUserObj);
                if (DBG) {
                    String name;
                    recordLoaded->GetEfName(&name);
                    Log(name + String(" LOADED"));
                }

                if (ar->mException != NULL) {
                    Loge(String("Record Load Exception: ") + TO_CSTR(ar->mException));
                }
                else {
                    recordLoaded->OnRecordLoaded(ar);
                }
            // }catch (RuntimeException exc) {
            //     // I don't want these exceptions to be fatal
            //     loge("Exception parsing SIM record: " + exc);
            // } finally {
                // Count up record load responses even if they are fails
                OnRecordLoaded();
            // }
            break;
        }
        case EVENT_REFRESH: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (DBG) Log(String("Card REFRESH occurred: "));
            if (ar->mException == NULL) {
                BroadcastRefresh();
                HandleRefresh(IIccRefreshResponse::Probe(ar->mResult));
            }
            else {
                Loge(String("Icc refresh Exception: ") + TO_CSTR(ar->mException));
            }
            break;
        }
        case EVENT_REFRESH_OEM: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (DBG) Log(String("Card REFRESH OEM occurred: "));
            if (ar->mException == NULL) {
                AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
                assert(array != NULL);
                Int32 len = 0;
                array->GetLength(&len);
                AutoPtr<ArrayOf<Byte> > records = ArrayOf<Byte>::Alloc(len);
                for (Int32 i = 0; i < len; i++) {
                    AutoPtr<IInterface> o;
                    array->Get(i, (IInterface**)&o);
                    IByte::Probe(o)->GetValue(&(*records)[i]);
                }
                HandleRefreshOem(records);
            }
            else {
                Loge(String("Icc refresh OEM Exception: ") + TO_CSTR(ar->mException));
            }
            break;
        }
        case EVENT_AKA_AUTHENTICATE_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            auth_rsp = NULL;
            if (DBG) Log(String("EVENT_AKA_AUTHENTICATE_DONE"));
            if (ar->mException != NULL) {
                Loge(String("Exception ICC SIM AKA: ") + TO_CSTR(ar->mException));
            }
            else {
                // try {
                    auth_rsp = IIccIoResult::Probe(ar->mResult);
                    // if (DBG) Log(String("ICC SIM AKA: auth_rsp = ") + auth_rsp);
                // } catch (Exception e) {
                //     loge("Failed to parse ICC SIM AKA contents: " + e);
                // }
            }
            {
                AutoLock lock(mLock);
                mLock.NotifyAll();
            }

            break;
        }
        case EVENT_GET_SMS_RECORD_SIZE_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException != NULL) {
                Loge(String("Exception in EVENT_GET_SMS_RECORD_SIZE_DONE ") + TO_CSTR(ar->mException));
                break;
            }

            AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
            assert(array != NULL);
            Int32 len = 0;
            array->GetLength(&len);
            AutoPtr<ArrayOf<Int32> > recordSize = ArrayOf<Int32>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> o;
                array->Get(i, (IInterface**)&o);
                IInteger32::Probe(o)->GetValue(&(*recordSize)[i]);
            }
            // try {
                // recordSize[0]  is the record length
                // recordSize[1]  is the total length of the EF file
                // recordSize[2]  is the number of records in the EF file
                mSmsCountOnIcc = (*recordSize)[2];
                Log(String("EVENT_GET_SMS_RECORD_SIZE_DONE Size ") + StringUtils::ToString((*recordSize)[0])
                        + String(" total ") + StringUtils::ToString((*recordSize)[1])
                        + String(" record ") + StringUtils::ToString((*recordSize)[2]));
            // } catch (ArrayIndexOutOfBoundsException exc) {
            //     loge("ArrayIndexOutOfBoundsException in EVENT_GET_SMS_RECORD_SIZE_DONE: "
            //             + exc.toString());
            // }
            break;
        }
        default:
            Handler::HandleMessage(msg);
    }
    return NOERROR;
}

ECode IccRecords::IsCspPlmnEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode IccRecords::GetOperatorNumeric(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode IccRecords::IsCallForwardStatusStored(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode IccRecords::GetVoiceCallForwardingFlag(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode IccRecords::SetVoiceCallForwardingFlag(
    /* [in] */ Int32 line,
    /* [in] */ Boolean enable,
    /* [in] */ const String& number)
{
    return NOERROR;
}

ECode IccRecords::IsProvisioned(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode IccRecords::GetIsimRecords(
    /* [out] */ IIsimRecords** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode IccRecords::GetUsimServiceTable(
    /* [out] */ IUsimServiceTable** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode IccRecords::GetIccSimChallengeResponse(
    /* [in] */ Int32 authContext,
    /* [in] */ const String& data,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Log(String("getIccSimChallengeResponse:"));

    // try {
        {
            AutoLock lock(mLock);
            AutoPtr<ICommandsInterface> ci = mCi;
            AutoPtr<IUiccCardApplication> parentApp = mParentApp;
            if (ci != NULL && parentApp != NULL) {
                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_AKA_AUTHENTICATE_DONE, (IMessage**)&msg);
                String aid;
                parentApp->GetAid(&aid);
                ci->RequestIccSimAuthentication(authContext, data,
                        aid,
                        msg);
                // try {
                mLock.Wait();
                // } catch (InterruptedException e) {
                //     loge("getIccSimChallengeResponse: Fail, interrupted"
                //             + " while trying to request Icc Sim Auth");
                //     return NULL;
                // }
            }
            else {
                Loge(String("getIccSimChallengeResponse: ")
                        + String("Fail, ci or parentApp is NULL"));
                *result = String(NULL);
                return NOERROR;
            }
        }
    // } catch(Exception e) {
    //     loge( "getIccSimChallengeResponse: "
    //             + "Fail while trying to request Icc Sim Auth");
    //     return NULL;
    // }

    if (DBG) Log(String("getIccSimChallengeResponse: return auth_rsp"));

    AutoPtr<IBase64> base64;
    CBase64::AcquireSingleton((IBase64**)&base64);
    return base64->EncodeToString(((IccIoResult*)auth_rsp.Get())->mPayload, IBase64::NO_WRAP, result);
}

ECode IccRecords::GetSmsCapacityOnIcc(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Log(String("getSmsCapacityOnIcc: ") + StringUtils::ToString(mSmsCountOnIcc));
    *result = mSmsCountOnIcc;
    return NOERROR;
}

ECode IccRecords::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("IccRecords: ") + TO_CSTR(this));
    pw->Println(String(" mDestroyed=") + TO_CSTR(mDestroyed));
    pw->Println(String(" mCi=") + TO_CSTR(mCi));
    pw->Println(String(" mFh=") + TO_CSTR(mFh));
    pw->Println(String(" mParentApp=") + TO_CSTR(mParentApp));
    pw->Println(String(" recordsLoadedRegistrants: size=")
                + StringUtils::ToString(mRecordsLoadedRegistrants->GetSize()));
    for (Int32 i = 0; i < mRecordsLoadedRegistrants->GetSize(); i++) {
        AutoPtr<IInterface> p = mRecordsLoadedRegistrants->Get(i);
        AutoPtr<IRegistrant> _p = IRegistrant::Probe(p);
        AutoPtr<IHandler> hdl;
        _p->GetHandler((IHandler**)&hdl);
        pw->Println(String("  recordsLoadedRegistrants[") + StringUtils::ToString(i) + String("]=")
                + TO_CSTR(hdl));
    }
    pw->Println(String(" mImsiReadyRegistrants: size=")
                + StringUtils::ToString(mImsiReadyRegistrants->GetSize()));
    for (Int32 i = 0; i < mImsiReadyRegistrants->GetSize(); i++) {
        AutoPtr<IInterface> p = mImsiReadyRegistrants->Get(i);
        AutoPtr<IRegistrant> _p = IRegistrant::Probe(p);
        AutoPtr<IHandler> hdl;
        _p->GetHandler((IHandler**)&hdl);
        pw->Println(String("  mImsiReadyRegistrants[") + StringUtils::ToString(i) + String("]=")
                + TO_CSTR(hdl));
    }
    pw->Println(String(" mRecordsEventsRegistrants: size=")
                + StringUtils::ToString(mRecordsEventsRegistrants->GetSize()));
    for (Int32 i = 0; i < mRecordsEventsRegistrants->GetSize(); i++) {
        AutoPtr<IInterface> p = mNewSmsRegistrants->Get(i);
        AutoPtr<IRegistrant> _p = IRegistrant::Probe(p);
        AutoPtr<IHandler> hdl;
        _p->GetHandler((IHandler**)&hdl);
        pw->Println(String("  mRecordsEventsRegistrants[") + StringUtils::ToString(i) + String("]=")
                + TO_CSTR(hdl));
    }
    pw->Println(String(" mNewSmsRegistrants: size=")
                + StringUtils::ToString(mNewSmsRegistrants->GetSize()));
    for (Int32 i = 0; i < mNewSmsRegistrants->GetSize(); i++) {
        AutoPtr<IInterface> p = mNewSmsRegistrants->Get(i);
        AutoPtr<IRegistrant> _p = IRegistrant::Probe(p);
        AutoPtr<IHandler> hdl;
        _p->GetHandler((IHandler**)&hdl);
        pw->Println(String("  mNewSmsRegistrants[") + StringUtils::ToString(i) + String("]=")
                + TO_CSTR(hdl));
    }
    pw->Println(String(" mNetworkSelectionModeAutomaticRegistrants: size=")
            + StringUtils::ToString(mNetworkSelectionModeAutomaticRegistrants->GetSize()));
    for (Int32 i = 0; i < mNetworkSelectionModeAutomaticRegistrants->GetSize(); i++) {
        AutoPtr<IInterface> p = mNetworkSelectionModeAutomaticRegistrants->Get(i);
        AutoPtr<IRegistrant> _p = IRegistrant::Probe(p);
        AutoPtr<IHandler> hdl;
        _p->GetHandler((IHandler**)&hdl);
        pw->Println(String("  mNetworkSelectionModeAutomaticRegistrants[")
                + StringUtils::ToString(i) + String("]=") + TO_CSTR(hdl));
    }
    pw->Println(String(" mRecordsRequested=") + StringUtils::ToString(mRecordsRequested));
    pw->Println(String(" mRecordsToLoad=") + StringUtils::ToString(mRecordsToLoad));
    pw->Println(String(" mRdnCache=") + TO_CSTR(mAdnCache));
    pw->Println(String(" iccid=") + mIccId);
    pw->Println(String(" mMsisdn=") + mMsisdn);
    pw->Println(String(" mMsisdnTag=") + mMsisdnTag);
    pw->Println(String(" mVoiceMailNum=") + mVoiceMailNum);
    pw->Println(String(" mVoiceMailTag=") + mVoiceMailTag);
    pw->Println(String(" mNewVoiceMailNum=") + mNewVoiceMailNum);
    pw->Println(String(" mNewVoiceMailTag=") + mNewVoiceMailTag);
    pw->Println(String(" mIsVoiceMailFixed=") + StringUtils::ToString(mIsVoiceMailFixed));
    pw->Println(String(" mImsi=") + mImsi);
    pw->Println(String(" mMncLength=") + StringUtils::ToString(mMncLength));
    pw->Println(String(" mMailboxIndex=") + StringUtils::ToString(mMailboxIndex));
    pw->Println(String(" mSpn=") + mSpn);
    IFlushable::Probe(pw)->Flush();
    return NOERROR;
}

void IccRecords::SetServiceProviderName(
    /* [in] */ const String& spn)
{
    mSpn = spn;
}

void IccRecords::OnIccRefreshInit()
{
    mAdnCache->Reset();
    AutoPtr<IUiccCardApplication> parentApp = mParentApp;

    if (parentApp != NULL) {
        AppState state;
        parentApp->GetState(&state);
        if (state == APPSTATE_READY) {
            // This will cause files to be reread
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_APP_READY, (IMessage**)&msg);
            Boolean b = FALSE;
            SendMessage(msg, &b);
        }
    }
}

ECode IccRecords::BroadcastRefresh()
{
    return NOERROR;
}

void IccRecords::HandleRefresh(
    /* [in] */ IIccRefreshResponse* refreshResponse)
{
    if (refreshResponse == NULL) {
        if (DBG) Log(String("handleRefresh received without input"));
        return;
    }

    String aid;
    mParentApp->GetAid(&aid);
    AutoPtr<CIccRefreshResponse> _refreshResponse = (CIccRefreshResponse*)refreshResponse;
    if (_refreshResponse->mAid != NULL &&
            !_refreshResponse->mAid.Equals(aid)) {
        // This is for different app. Ignore.
        return;
    }

    switch (_refreshResponse->mRefreshResult) {
        case IIccRefreshResponse::REFRESH_RESULT_FILE_UPDATE:
            if (DBG) Log(String("handleRefresh with SIM_FILE_UPDATED"));
            HandleFileUpdate(_refreshResponse->mEfId);
            break;
        case IIccRefreshResponse::REFRESH_RESULT_INIT:
            if (DBG) Log(String("handleRefresh with SIM_REFRESH_INIT"));
            // need to reload all files (that we care about)
            if (mAdnCache != NULL) {
                mAdnCache->Reset();
                //We will re-fetch the records when the app
                // goes back to the ready state. Nothing to do here.
            }
            break;
        case IIccRefreshResponse::REFRESH_RESULT_RESET:
            if (DBG) Log(String("handleRefresh with SIM_REFRESH_RESET"));
            if (PowerOffOnSimReset()) {
                mCi->SetRadioPower(FALSE, NULL);
                /* Note: no need to call setRadioPower(true).  Assuming the desired
                * radio power state is still ON (as tracked by ServiceStateTracker),
                * ServiceStateTracker will call setRadioPower when it receives the
                * RADIO_STATE_CHANGED notification for the power off.  And if the
                * desired power state has changed in the interim, we don't want to
                * override it with an unconditional power on.
                */
            }
            else {
                if (mAdnCache != NULL) {
                    mAdnCache->Reset();
                }
                mRecordsRequested = FALSE;
                mImsi = NULL;
            }
            //We will re-fetch the records when the app
            // goes back to the ready state. Nothing to do here.
            break;
        default:
            // unknown refresh operation
            if (DBG) Log(String("handleRefresh with unknown operation"));
            break;
    }
}

Boolean IccRecords::RequirePowerOffOnSimRefreshReset()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Boolean b = FALSE;
    res->GetBoolean(R::bool_::config_requireRadioPowerOffOnSimRefreshReset, &b);
    return b;
}

Boolean IccRecords::PowerOffOnSimReset()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Boolean b = FALSE;
    res->GetBoolean(
            R::bool_::skip_radio_power_off_on_sim_refresh_reset, &b);
    return !b;
}

void IccRecords::SetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& value)
{
    if (mParentApp == NULL) return;
    AutoPtr<IUiccCard> uicc;
    mParentApp->GetUiccCard((IUiccCard**)&uicc);
    Int32 slotId = 0;
    uicc->GetSlotId(&slotId);

    AutoPtr<ISubscriptionControllerHelper> hlp;
    CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlp);
    AutoPtr<ISubscriptionController> subController;
    hlp->GetInstance((ISubscriptionController**)&subController);
    AutoPtr<ArrayOf<Int64> > arr;
    subController->GetSubIdUsingSlotId(slotId, (ArrayOf<Int64>**)&arr);
    Int64 subId = (*arr)[0];

    AutoPtr<ITelephonyManagerHelper> tmhlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmhlp);
    tmhlp->SetTelephonyProperty(property, subId, value);
}

void IccRecords::HandleRefreshOem(
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<IByteBufferHelper> bbhlp;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbhlp);
    AutoPtr<IByteBuffer> payload;
    bbhlp->Wrap(data, (IByteBuffer**)&payload);
    AutoPtr<IUiccControllerHelper> uchlp;
    CUiccControllerHelper::AcquireSingleton((IUiccControllerHelper**)&uchlp);
    AutoPtr<IIccRefreshResponse> response;
    uchlp->ParseOemSimRefresh(payload, (IIccRefreshResponse**)&response);

    AutoPtr<IIccCardApplicationStatus> appStatus;
    CIccCardApplicationStatus::New((IIccCardApplicationStatus**)&appStatus);
    Int32 num = 0;
    payload->GetInt32(&num);
    AppType appType;
    appStatus->AppTypeFromRILInt(num, &appType);
    Int32 slotId = 0;
    payload->GetInt32(&slotId);
    AppType type;
    mParentApp->GetType(&type);
    if ((appType != APPTYPE_UNKNOWN)
        && (appType != type)) {
        // This is for different app. Ignore.
        return;
    }

    BroadcastRefresh();
    HandleRefresh(response);

    AutoPtr<CIccRefreshResponse> cresponse = (CIccRefreshResponse*)response.Get();
    if (cresponse->mRefreshResult == IIccRefreshResponse::REFRESH_RESULT_FILE_UPDATE ||
        cresponse->mRefreshResult == IIccRefreshResponse::REFRESH_RESULT_INIT) {
        Log(String("send broadcast org.codeaurora.intent.action.ACTION_SIM_REFRESH_UPDATE"));
        AutoPtr<IIntent> sendIntent;
        CIntent::New(
                String("org.codeaurora.intent.action.ACTION_SIM_REFRESH_UPDATE"), (IIntent**)&sendIntent);
        AutoPtr<ITelephonyManagerHelper> hlp;
        CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&hlp);
        AutoPtr<ITelephonyManager> tm;
        hlp->GetDefault((ITelephonyManager**)&tm);
        Boolean bEnabled = FALSE;
        tm->IsMultiSimEnabled(&bEnabled);
        if (bEnabled){
            sendIntent->PutExtra(IPhoneConstants::SLOT_KEY, slotId);
        }
        mContext->SendBroadcast(sendIntent, String(NULL));
    }
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
