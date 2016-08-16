
#include "elastos/droid/internal/telephony/uicc/UiccController.h"
#include "elastos/droid/internal/telephony/uicc/CIccRefreshResponse.h"
#include "elastos/droid/internal/telephony/uicc/CUiccCard.h"
#include "elastos/droid/internal/telephony/uicc/CUiccController.h"
#include "elastos/droid/internal/telephony/SubscriptionController.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/os/RegistrantList.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/telephony/CServiceState.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/R.h"

#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Telephony::CServiceState;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Core::CInteger32;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::IO::ByteOrder;
using Elastos::IO::CByteOrderHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::IFlushable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                            UiccController
//=====================================================================
CAR_INTERFACE_IMPL(UiccController, Handler, IUiccController);

const Boolean UiccController::DBG = TRUE;
const String UiccController::LOGTAG("UiccController");
const Int32 UiccController::EVENT_ICC_STATUS_CHANGED;
const Int32 UiccController::EVENT_GET_ICC_STATUS_DONE;
const Int32 UiccController::EVENT_RADIO_UNAVAILABLE;
const Int32 UiccController::EVENT_REFRESH;
const Int32 UiccController::EVENT_REFRESH_OEM;
Object UiccController::mLock;
Object UiccController::THIS;
AutoPtr<IUiccController> UiccController::mInstance;

UiccController::UiccController()
    : mOEMHookSimRefresh(FALSE)
{
    mIccChangedRegistrants = new RegistrantList();
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    Int32 count = 0;
    tm->GetPhoneCount(&count);
    mUiccCards = ArrayOf<IUiccCard*>::Alloc(count);
}

ECode UiccController::constructor(
    /* [in] */ IContext* c,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci)
{
    Handler::constructor();

    if (DBG) Log(String("Creating UiccController"));
    mContext = c;
    mCis = ci;

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetBoolean(Elastos::Droid::R::bool_::config_sim_refresh_for_dual_mode_card, &mOEMHookSimRefresh);
    Boolean tmp = FALSE;
    for (Int32 i = 0; i < mCis->GetLength(); i++) {
        if ((*mCis)[i] == NULL) continue;

        AutoPtr<IInteger32> index;
        CInteger32::New(i, (IInteger32**)&index);
        if (SystemProperties::GetBoolean(String("persist.radio.apm_sim_not_pwdn"), FALSE, &tmp), tmp) {
            // Reading ICC status in airplane mode is only supported in QCOM
            // RILs when this property is set to true
            (*mCis)[i]->RegisterForAvailable(this, EVENT_ICC_STATUS_CHANGED, index);
        }
        else {
            (*mCis)[i]->RegisterForOn(this, EVENT_ICC_STATUS_CHANGED, index);
        }

        (*mCis)[i]->RegisterForIccStatusChanged(this, EVENT_ICC_STATUS_CHANGED, index);
        // TODO remove this once modem correctly notifies the unsols
        (*mCis)[i]->RegisterForNotAvailable(this, EVENT_RADIO_UNAVAILABLE, index);
        if (mOEMHookSimRefresh) {
            (*mCis)[i]->RegisterForSimRefreshEvent(this, EVENT_REFRESH_OEM, index);
        }
        else {
            (*mCis)[i]->RegisterForIccRefresh(this, EVENT_REFRESH, index);
        }
    }
    return NOERROR;
}

AutoPtr<IUiccController> UiccController::Make(
    /* [in] */ IContext* c,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci)
{
    AutoLock lock(mLock);
    if (mInstance != NULL) {
        // throw new RuntimeException("MSimUiccController.make() should only be called once");
        assert(0 && "MSimUiccController.make() should only be called once");
    }
    CUiccController::New(c, ci, (IUiccController**)&mInstance);
    return mInstance;
}

AutoPtr<IUiccController> UiccController::GetInstance()
{
    AutoLock lock(mLock);
    if (mInstance == NULL) {
        // throw new RuntimeException(
        //         "UiccController.getInstance can't be called before make()");
        assert(0 && "UiccController.getInstance can't be called before make()");
    }
    return mInstance;
}

ECode UiccController::GetUiccCard(
    /* [out] */ IUiccCard** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISubscriptionController> sc = SubscriptionController::GetInstance();
    Int64 subId = 0;
    IISub::Probe(sc)->GetDefaultSubId(&subId);
    Int32 id = 0;
    IISub::Probe(sc)->GetPhoneId(subId, &id);
    return GetUiccCard(id, result);
}

ECode UiccController::GetUiccCard(
    /* [in] */ Int32 slotId,
    /* [out] */ IUiccCard** result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    if (IsValidCardIndex(slotId)) {
        *result = (*mUiccCards)[slotId];
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

ECode UiccController::GetUiccCards(
    /* [out] */ ArrayOf<IUiccCard*>** result)
{
    VALIDATE_NOT_NULL(result);
    // Return cloned array since we don't want to give out reference
    // to internal data structure.
    AutoLock lock(mLock);
    *result = mUiccCards->Clone();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UiccController::GetUiccCardApplication(
    /* [in] */ Int32 family,
    /* [out] */ IUiccCardApplication** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISubscriptionController> sc = SubscriptionController::GetInstance();
    Int64 subId = 0;
    IISub::Probe(sc)->GetDefaultSubId(&subId);
    Int32 id = 0;
    IISub::Probe(sc)->GetPhoneId(subId, &id);
    return GetUiccCardApplication(id, family, result);
}

ECode UiccController::GetIccRecords(
    /* [in] */ Int32 slotId,
    /* [in] */ Int32 family,
    /* [out] */ IIccRecords** result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    AutoPtr<IUiccCardApplication> app;
    GetUiccCardApplication(slotId, family, (IUiccCardApplication**)&app);
    if (app != NULL) {
        return app->GetIccRecords(result);
    }
    *result = NULL;
    return NOERROR;
}

ECode UiccController::GetIccFileHandler(
    /* [in] */ Int32 slotId,
    /* [in] */ Int32 family,
    /* [out] */ IIccFileHandler** result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    AutoPtr<IUiccCardApplication> app;
    GetUiccCardApplication(slotId, family, (IUiccCardApplication**)&app);
    if (app != NULL) {
        return app->GetIccFileHandler(result);
    }
    *result = NULL;
    return NOERROR;
}

Int32 UiccController::GetFamilyFromRadioTechnology(
    /* [in] */ Int32 radioTechnology)
{
    Boolean tmp = FALSE;
    if ((CServiceState::IsGsm(radioTechnology, &tmp), tmp) ||
            radioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_EHRPD) {
        return  IUiccController::APP_FAM_3GPP;
    }
    else if (CServiceState::IsCdma(radioTechnology, &tmp), tmp) {
        return IUiccController::APP_FAM_3GPP2;
    }
    else {
        // If it is UNKNOWN rat
        return IUiccController::APP_FAM_UNKNOWN;
    }
    return 0;
}

ECode UiccController::RegisterForIccChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    {
        AutoLock lock(mLock);
        AutoPtr<IRegistrant> r;
        CRegistrant::New(h, what, obj, (IRegistrant**)&r);
        mIccChangedRegistrants->Add(r);
        //Notify registrant right after registering, so that it will get the latest ICC status,
        //otherwise which may not happen until there is an actual change in ICC status.
        r->NotifyRegistrant();
    }
    return NOERROR;
}

ECode UiccController::UnregisterForIccChanged(
    /* [in] */ IHandler* h)
{
    AutoLock lock(mLock);
    mIccChangedRegistrants->Remove(h);
    return NOERROR;
}

ECode UiccController::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoLock lock(mLock);
    AutoPtr<IInteger32> _index = GetCiIndex(msg);
    Int32 index = 0;
    _index->GetValue(&index);

    Int32 what = 0;
    msg->GetWhat(&what);
    if (index < 0 || index >= mCis->GetLength()) {
        Logger::E(LOGTAG, "Invalid index : %d received with event %d", index, what);
        return NOERROR;
    }

    switch (what) {
        case EVENT_ICC_STATUS_CHANGED: {
            if (DBG) Log(String("Received EVENT_ICC_STATUS_CHANGED, calling getIccCardStatus"));
            AutoPtr<IMessage> m;
            ObtainMessage(EVENT_GET_ICC_STATUS_DONE, _index, (IMessage**)&m);
            (*mCis)[index]->GetIccCardStatus(m);
            break;
        }
        case EVENT_GET_ICC_STATUS_DONE: {
            if (DBG) Log(String("Received EVENT_GET_ICC_STATUS_DONE"));
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AsyncResult* ar = (AsyncResult*)IAsyncResult::Probe(obj);
            OnGetIccCardStatusDone(ar, _index);
            break;
        }
        case EVENT_RADIO_UNAVAILABLE: {
            if (DBG) Log(String("EVENT_RADIO_UNAVAILABLE, dispose card"));
            if ((*mUiccCards)[index] != NULL) {
                (*mUiccCards)[index]->Dispose();
            }
            (*mUiccCards)[index] = NULL;
            AutoPtr<AsyncResult> a = new AsyncResult(NULL, _index, NULL);
            mIccChangedRegistrants->NotifyRegistrants(a);
            break;
        }
        case EVENT_REFRESH: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);

            AsyncResult* ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (DBG) Log(String("Sim REFRESH received"));
            if (ar->mException == NULL) {
                HandleRefresh(IIccRefreshResponse::Probe(ar->mResult), index);
            }
            else {
                Log(String("Exception on refresh ") + TO_CSTR(ar->mException));
            }
            break;
        }
        case EVENT_REFRESH_OEM: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);

            AsyncResult* ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (DBG) Log(String("Sim REFRESH OEM received"));
            if (ar->mException == NULL) {
                AutoPtr<IByteBufferHelper> helper;
                CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
                Int32 len = 0;
                AutoPtr<IArrayOf> ao = IArrayOf::Probe(ar->mResult);
                assert(ao);
                ao->GetLength(&len);
                AutoPtr<ArrayOf<Byte> > bs = ArrayOf<Byte>::Alloc(len);
                for(Int32 i = 0; i < len; i++) {
                    AutoPtr<IInterface> b;
                    ao->Get(i, (IInterface**)&b);
                    Byte v;
                    IByte::Probe(b)->GetValue(&v);
                    (*bs)[i] = v;
                }
                AutoPtr<IByteBuffer> payload;
                helper->Wrap(bs, (IByteBuffer**)&payload);
                HandleRefresh(ParseOemSimRefresh(payload), index);
            }
            else {
                Log(String("Exception on refresh ") + TO_CSTR(ar->mException));
            }
            break;
        }
        default: {
            Logger::E(LOGTAG, " Unknown Event %d", what);
        }
    }
    return NOERROR;
}

AutoPtr<IIccRefreshResponse> UiccController::ParseOemSimRefresh(
    /* [in] */ IByteBuffer* payload)
{
    AutoPtr<CIccRefreshResponse> response;
    CIccRefreshResponse::NewByFriend((CIccRefreshResponse**)&response);
    /* AID maximum size */
    const Int32 QHOOK_MAX_AID_SIZE = 20*2+1+3;

    /* parse from payload */
    AutoPtr<IByteOrderHelper> helper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&helper);
    ByteOrder bo;
    helper->GetNativeOrder(&bo);
    payload->SetOrder(bo);

    payload->GetInt32(&response->mRefreshResult);
    payload->GetInt32(&response->mEfId);
    Int32 aidLen = 0;
    payload->GetInt32(&aidLen);
    AutoPtr<ArrayOf<Byte> > aid = ArrayOf<Byte>::Alloc(QHOOK_MAX_AID_SIZE);
    payload->Get(aid, 0, QHOOK_MAX_AID_SIZE);
    //Read the aid string with QHOOK_MAX_AID_SIZE from payload at first because need
    //corresponding to the aid array length sent from qcril and need parse the payload
    //to get app type and sub id in IccRecords.java after called this method.
    response->mAid = (aidLen == 0) ? String(NULL) : String(*aid).Substring(0, aidLen);

    if (DBG) {
        Logger::D(LOGTAG, "refresh SIM card , refresh result:%d, ef Id:%d, aid:%s"
                , response->mRefreshResult, response->mEfId, response->mAid.string());
    }
    return response;
}

ECode UiccController::GetUiccCardApplication(
    /* [in] */ Int32 slotId,
    /* [in] */ Int32 family,
    /* [out] */ IUiccCardApplication** result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    if (IsValidCardIndex(slotId)) {
        AutoPtr<IUiccCard> c = (*mUiccCards)[slotId];
        if (c != NULL) {
            return (*mUiccCards)[slotId]->GetApplication(family, result);
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode UiccController::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("UiccController: ") + TO_CSTR(this));
    pw->Println(String(" mContext=") + TO_CSTR(mContext));
    pw->Println(String(" mInstance=") + TO_CSTR(mInstance));
    // pw->Println(" mCi=" + TO_CSTR(mCi));
    // pw->Println(" mUiccCard=" + TO_CSTR(mUiccCard));
    pw->Println(String(" mIccChangedRegistrants: size=") +
                StringUtils::ToString(mIccChangedRegistrants->GetSize()));
    for (Int32 i = 0; i < mIccChangedRegistrants->GetSize(); i++) {
        AutoPtr<IHandler> hdl;
        IRegistrant::Probe(mIccChangedRegistrants->Get(i))->GetHandler((IHandler**)&hdl);
        pw->Println(String("  mIccChangedRegistrants[") + StringUtils::ToString(i)
                + String("]=")/* + hdl*/);
    }
    pw->Println();
    IFlushable::Probe(pw)->Flush();
//        for (int i = 0; i < mUiccCards.length; i++) {
//            mUiccCards[i].dump(fd, pw, args);
//        }
    return NOERROR;
}

AutoPtr<ArrayOf<IUiccCard*> > UiccController::MiddleInitMuicccards()
{
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    Int32 count = 0;
    tm->GetPhoneCount(&count);
    AutoPtr<ArrayOf<IUiccCard*> > result = ArrayOf<IUiccCard*>::Alloc(count);
    return result;
}

AutoPtr<IInteger32> UiccController::GetCiIndex(
    /* [in] */ IMessage* msg)
{
    AsyncResult* ar = NULL;
    AutoPtr<IInteger32> index;
    CInteger32::New(IPhoneConstants::DEFAULT_CARD_INDEX, (IInteger32**)&index);

    /*
     * The events can be come in two ways. By explicitly sending it using
     * sendMessage, in this case the user object passed is msg.obj and from
     * the CommandsInterface, in this case the user object is msg.obj.userObj
     */
    if (msg != NULL) {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        if (obj != NULL && IInteger32::Probe(obj)) {
            index = IInteger32::Probe(obj);
        }
        else if(obj != NULL && IAsyncResult::Probe(obj)) {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mUserObj != NULL && IInteger32::Probe(ar->mUserObj)) {
                index = IInteger32::Probe(ar->mUserObj);
            }
        }
    }
    return index;
}

void UiccController::HandleRefresh(
    /* [in] */ IIccRefreshResponse* refreshResponse,
    /* [in] */ Int32 index)
{
    if (refreshResponse == NULL) {
        Log(String("handleRefresh received without input"));
        return;
    }

    // Let the card know right away that a refresh has occurred
    if ((*mUiccCards)[index] != NULL) {
        ((CUiccCard*)(*mUiccCards)[index])->OnRefresh(refreshResponse);
    }
    // The card status could have changed. Get the latest state
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_GET_ICC_STATUS_DONE, (IMessage**)&msg);
    (*mCis)[index]->GetIccCardStatus(msg);
}

// synchronized
void UiccController::OnGetIccCardStatusDone(
    /* [in] */ AsyncResult* ar,
    /* [in] */ IInteger32* _index)
{
    AutoLock lock(THIS);
    Int32 index = 0;
    _index->GetValue(&index);
    if (ar->mException != NULL) {
        Logger::E(LOGTAG,"Error getting ICC status. RIL_REQUEST_GET_ICC_STATUS should never return an error%s"
                , TO_CSTR(ar->mException));
        return;
    }
    if (!IsValidCardIndex(index)) {
        Logger::E(LOGTAG,"onGetIccCardStatusDone: invalid index : %d", index);
        return;
    }

    AutoPtr<IIccCardStatus> status = IIccCardStatus::Probe(ar->mResult);

    if ((*mUiccCards)[index] == NULL) {
        //Create new card
        AutoPtr<IUiccCard> uc;
        CUiccCard::New(mContext, (*mCis)[index], status, index, (IUiccCard**)&uc);
        mUiccCards->Set(index, uc);

/*
        // Update the UiccCard in base class, so that if someone calls
        // UiccManager.getUiccCard(), it will return the default card.
        if (index == PhoneConstants.DEFAULT_CARD_INDEX) {
            mUiccCard = mUiccCards[index];
        }
*/
    }
    else {
        //Update already existing card
        (*mUiccCards)[index]->Update(mContext, (*mCis)[index] , status);
    }

    if (DBG) Log(String("Notifying IccChangedRegistrants"));
    AutoPtr<AsyncResult> tmp = new AsyncResult(NULL, _index, NULL);
    mIccChangedRegistrants->NotifyRegistrants(tmp);
}

Boolean UiccController::IsValidCardIndex(
    /* [in] */ Int32 index)
{
    return (index >= 0 && index < mUiccCards->GetLength());
}

void UiccController::Log(
    /* [in] */ const String& string)
{
    Logger::D(LOGTAG, string);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
