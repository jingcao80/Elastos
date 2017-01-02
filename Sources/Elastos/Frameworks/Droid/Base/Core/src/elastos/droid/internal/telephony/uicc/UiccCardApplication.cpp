//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/os/RegistrantList.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/internal/telephony/uicc/UiccCardApplication.h"
#include "elastos/droid/internal/telephony/uicc/CSIMFileHandler.h"
#include "elastos/droid/internal/telephony/uicc/CRuimFileHandler.h"
#include "elastos/droid/internal/telephony/uicc/CUsimFileHandler.h"
#include "elastos/droid/internal/telephony/uicc/CCsimFileHandler.h"
#include "elastos/droid/internal/telephony/uicc/CIsimFileHandler.h"
#include "elastos/droid/internal/telephony/uicc/CSIMRecords.h"
#include "elastos/droid/internal/telephony/uicc/CRuimRecords.h"
#include "elastos/droid/internal/telephony/uicc/CIsimUiccRecords.h"
#include "elastos/droid/internal/telephony/uicc/IccCardApplicationStatus.h"
#include "elastos/droid/internal/telephony/uicc/IccRefreshResponse.h"

#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::Uicc::AppState;
using Elastos::Droid::Os::Registrant;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::RegistrantList;

using Elastos::Core::AutoLock;
using Elastos::Core::IArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::IO::IFlushable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                  UiccCardApplication::InnerHandler
//=====================================================================
UiccCardApplication::InnerHandler::InnerHandler(
    /* [in] */ UiccCardApplication* owner)
    : mOwner(owner)
{
    Handler::constructor();
}

ECode UiccCardApplication::InnerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;

    Int32 what = 0;
    msg->GetWhat(&what);
    if (mOwner->mDestroyed) {
        Logger::E(String("UiccCardApplication"), String("Received message %p")
            + String("[") + StringUtils::ToString(what)
            + String("] while being destroyed. Ignoring."), msg);
        return NOERROR;
    }

    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case EVENT_PIN1_PUK1_DONE:
        case EVENT_PIN2_PUK2_DONE:
        case EVENT_CHANGE_PIN1_DONE:
        case EVENT_CHANGE_PIN2_DONE: {
            // a PIN/PUK/PIN2/PUK2 complete
            // request has completed. ar.userObj is the response Message
            Int32 attemptsRemaining = -1;
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mResult != NULL) {
                attemptsRemaining = mOwner->ParsePinPukErrorResult(ar);
            }
            AutoPtr<IMessage> response = IMessage::Probe(ar->mUserObj);
            AsyncResult::ForMessage(response)->mException = ar->mException;
            response->SetArg1(attemptsRemaining);
            response->SendToTarget();
            break;
        }
        case EVENT_QUERY_FACILITY_FDN_DONE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            mOwner->OnQueryFdnEnabled(ar);
            break;
        case EVENT_CHANGE_FACILITY_FDN_DONE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            mOwner->OnChangeFdnDone(ar);
            break;
        case EVENT_QUERY_FACILITY_LOCK_DONE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            mOwner->OnQueryFacilityLock(ar);
            break;
        case EVENT_CHANGE_FACILITY_LOCK_DONE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            mOwner->OnChangeFacilityLock(ar);
            break;
        default: {
            mOwner->Loge(String("Unknown Event ") + StringUtils::ToString(what));
        }
    }
    return NOERROR;
}

//=====================================================================
//                         UiccCardApplication
//=====================================================================
CAR_INTERFACE_IMPL(UiccCardApplication, Object, IUiccCardApplication);

const String UiccCardApplication::LOGTAG("UiccCardApplication");
const Boolean UiccCardApplication::DBG = TRUE;
const Int32 UiccCardApplication::EVENT_PIN1_PUK1_DONE;
const Int32 UiccCardApplication::EVENT_CHANGE_PIN1_DONE;
const Int32 UiccCardApplication::EVENT_CHANGE_PIN2_DONE;
const Int32 UiccCardApplication::EVENT_QUERY_FACILITY_FDN_DONE;
const Int32 UiccCardApplication::EVENT_CHANGE_FACILITY_FDN_DONE;
const Int32 UiccCardApplication::EVENT_QUERY_FACILITY_LOCK_DONE;
const Int32 UiccCardApplication::EVENT_CHANGE_FACILITY_LOCK_DONE;
const Int32 UiccCardApplication::EVENT_PIN2_PUK2_DONE;

UiccCardApplication::UiccCardApplication()
    : mAppState(-1)
    , mAppType(-1)
    , mAuthContext(0)
    , mPersoSubState(-1)
    , mPin1Replaced(FALSE)
    , mPin1State(-1)
    , mPin2State(-1)
    , mIccFdnEnabled(FALSE)
    , mDesiredFdnEnabled(FALSE)
    , mIccLockEnabled(FALSE)
    , mDesiredPinLocked(FALSE)
    , mIccFdnAvailable(TRUE)
    , mDestroyed(FALSE)
{
    mReadyRegistrants = new RegistrantList();
    mPinLockedRegistrants = new RegistrantList();
    mPersoLockedRegistrants = new RegistrantList();

    mHandler = new InnerHandler(this);
}

ECode UiccCardApplication::constructor(
    /* [in] */ IUiccCard* uiccCard,
    /* [in] */ IIccCardApplicationStatus* as,
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci)
{
    if (DBG) Log(String("Creating UiccCardApplication: "));// + as);
    IccCardApplicationStatus* cas = (IccCardApplicationStatus*)as;
    mUiccCard = uiccCard;
    mAppState = cas->mApp_state;
    mAppType = cas->mApp_type;
    mAuthContext = GetAuthContext(mAppType);
    mPersoSubState = cas->mPerso_substate;
    mAid = cas->mAid;
    mAppLabel = cas->mApp_label;
    mPin1Replaced = (cas->mPin1_replaced != 0);
    mPin1State = cas->mPin1;
    mPin2State = cas->mPin2;

    mContext = c;
    mCi = ci;

    //Logger::E("leliang", "line:%d, func:%s, type:%d, state:%d\n", __LINE__, __func__, cas->mApp_type, mAppState);
    //test leliang type:2, state: 5
    mIccFh = CreateIccFileHandler(cas->mApp_type);
    mIccRecords = CreateIccRecords(cas->mApp_type, mContext, mCi);

    if (mAppState == APPSTATE_READY) {
        QueryFdn();
        QueryPin1State();
    }
    return NOERROR;
}

ECode UiccCardApplication::Update(
    /* [in] */ IIccCardApplicationStatus* _as,
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci)
{
    AutoLock lock(mLock);
    if (mDestroyed) {
        Loge(String("Application updated after destroyed! Fix me!"));
        return NOERROR;
    }

    if (DBG) Log(StringUtils::ToString(mAppType) + " update. New " + TO_CSTR(_as));
    mContext = c;
    mCi = ci;
    AppType oldAppType = mAppType;
    AppState oldAppState = mAppState;
    PersoSubState oldPersoSubState = mPersoSubState;
    IccCardApplicationStatus* as = (IccCardApplicationStatus*)_as;
    mAppType = as->mApp_type;
    mAuthContext = GetAuthContext(mAppType);
    mAppState = as->mApp_state;
    mPersoSubState = as->mPerso_substate;
    mAid = as->mAid;
    mAppLabel = as->mApp_label;
    mPin1Replaced = (as->mPin1_replaced != 0);
    mPin1State = as->mPin1;
    mPin2State = as->mPin2;

    if (mAppType != oldAppType) {
        if (mIccFh != NULL) {
            mIccFh->Dispose();
        }
        if (mIccRecords != NULL) {
            mIccRecords->Dispose();
        }
        mIccFh = CreateIccFileHandler(as->mApp_type);
        mIccRecords = CreateIccRecords(as->mApp_type, c, ci);
    }

    Boolean tmp = FALSE;
    if (mPersoSubState != oldPersoSubState &&
            (IsPersoLocked(&tmp), tmp)) {
        NotifyPersoLockedRegistrantsIfNeeded(NULL);
    }

    if (mAppState != oldAppState) {
        if (DBG) Log(StringUtils::ToString(oldAppType) + " changed state: "
            + StringUtils::ToString(oldAppState) + " -> " + StringUtils::ToString(mAppState));
        // If the app state turns to APPSTATE_READY, then query FDN status,
        //as it might have failed in earlier attempt.
        if (mAppState == APPSTATE_READY) {
            QueryFdn();
            QueryPin1State();
        }
        NotifyPinLockedRegistrantsIfNeeded(NULL);
        NotifyReadyRegistrantsIfNeeded(NULL);
    }
    return NOERROR;
}

ECode UiccCardApplication::Dispose()
{
    {
        AutoLock lock(mLock);
        if (DBG) {
            Log(StringUtils::ToString(mAppType) + String(" being Disposed"));
        }
        mDestroyed = TRUE;
        if (mIccRecords != NULL) {
            mIccRecords->Dispose();
        }
        if (mIccFh != NULL) {
            mIccFh->Dispose();
        }
        mIccRecords = NULL;
        mIccFh = NULL;
    }
    return NOERROR;
}

ECode UiccCardApplication::QueryFdn()
{
    //This shouldn't change run-time. So needs to be called only once.
    Int32 serviceClassX = 0;

    serviceClassX = ICommandsInterface::SERVICE_CLASS_VOICE +
                    ICommandsInterface::SERVICE_CLASS_DATA +
                    ICommandsInterface::SERVICE_CLASS_FAX;
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_QUERY_FACILITY_FDN_DONE, (IMessage**)&msg);
    mCi->QueryFacilityLockForApp(ICommandsInterface::CB_FACILITY_BA_FD, String(""), serviceClassX,
            mAid, msg);
    return NOERROR;
}

ECode UiccCardApplication::OnRefresh(
    /* [in] */ IIccRefreshResponse* refreshResponse)
{
    if (refreshResponse == NULL) {
        Loge(String("onRefresh received without input"));
        return NOERROR;
    }

    AutoPtr<IccRefreshResponse> _refreshResponse = (IccRefreshResponse*)refreshResponse;
    if (_refreshResponse->mAid == NULL ||
            _refreshResponse->mAid.Equals(mAid)) {
        Log(String("refresh for app ") + _refreshResponse->mAid);
    }
    else {
        // This is for a different app. Ignore.
        return NOERROR;
    }

    switch (_refreshResponse->mRefreshResult) {
        case IIccRefreshResponse::REFRESH_RESULT_INIT:
        case IIccRefreshResponse::REFRESH_RESULT_RESET:
            Log(String("onRefresh: Setting app state to unknown"));
            // Move our state to Unknown as soon as we know about a refresh
            // so that anyone interested does not get a stale state.
            mAppState = APPSTATE_UNKNOWN;
            break;
    }
    return NOERROR;
}

ECode UiccCardApplication::RegisterForReady(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    {
        AutoLock lock(mLock);
        for (Int32 i = mReadyRegistrants->GetSize() - 1; i >= 0 ; i--) {
            AutoPtr<IInterface> obj = mReadyRegistrants->Get(i);
            Registrant* r = (Registrant*)IRegistrant::Probe(obj);
            AutoPtr<IHandler> rH;
            r->GetHandler((IHandler**)&rH);

            if (rH != NULL && rH.Get() == h) { //TODO  is == this right??
                return NOERROR;
            }
        }
        AutoPtr<IRegistrant> r;
        CRegistrant::New(h, what, obj, (IRegistrant**)&r);

        mReadyRegistrants->Add(r);
        NotifyReadyRegistrantsIfNeeded((Registrant*)r.Get());
    }
    return NOERROR;
}

ECode UiccCardApplication::UnregisterForReady(
    /* [in] */ IHandler* h)
{
    {
        AutoLock lock(mLock);
        mReadyRegistrants->Remove(h);
    }
    return NOERROR;
}

ECode UiccCardApplication::RegisterForLocked(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    {
        AutoLock lock(mLock);
        AutoPtr<IRegistrant> r;
        CRegistrant::New(h, what, obj, (IRegistrant**)&r);
        mPinLockedRegistrants->Add(r);
        NotifyPinLockedRegistrantsIfNeeded((Registrant*)r.Get());
    }
    return NOERROR;
}

ECode UiccCardApplication::UnregisterForLocked(
    /* [in] */ IHandler* h)
{
    AutoLock lock(mLock);
    mPinLockedRegistrants->Remove(h);
    return NOERROR;
}

ECode UiccCardApplication::RegisterForPersoLocked(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoLock lock(mLock);
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mPersoLockedRegistrants->Add(r);
    NotifyPersoLockedRegistrantsIfNeeded((Registrant*)r.Get());
    return NOERROR;
}

ECode UiccCardApplication::UnregisterForPersoLocked(
    /* [in] */ IHandler* h)
{
    AutoLock lock(mLock);
    mPersoLockedRegistrants->Remove(h);
    return NOERROR;
}

ECode UiccCardApplication::GetState(
    /* [out] */ AppState* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        *result = mAppState;
    }
    return NOERROR;
}

ECode UiccCardApplication::GetType(
    /* [out] */ AppType* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        *result = mAppType;
    }
    return NOERROR;
}

ECode UiccCardApplication::GetAuthContext(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    *result = mAuthContext;
    return NOERROR;
}

ECode UiccCardApplication::GetPersoSubState(
    /* [out] */ PersoSubState* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    *result = mPersoSubState;
    return NOERROR;
}

ECode UiccCardApplication::GetAid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    *result = mAid;
    return NOERROR;
}

ECode UiccCardApplication::GetAppLabel(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAppLabel;
    return NOERROR;
}

ECode UiccCardApplication::GetPin1State(
    /* [out] */ PinState* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    if (mPin1Replaced) {
        return mUiccCard->GetUniversalPinState(result);
    }
    *result = mPin1State;
    return NOERROR;
}

ECode UiccCardApplication::GetIccFileHandler(
    /* [out] */ IIccFileHandler** result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        *result = mIccFh;
        REFCOUNT_ADD(*result);
    }
    return NOERROR;
}

ECode UiccCardApplication::GetIccRecords(
    /* [out] */ IIccRecords** result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        *result = mIccRecords;
        REFCOUNT_ADD(*result);
    }
    return NOERROR;
}

ECode UiccCardApplication::IsPersoLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    switch (mPersoSubState) {
        case PERSOSUBSTATE_UNKNOWN:
        case PERSOSUBSTATE_IN_PROGRESS:
        case PERSOSUBSTATE_READY: {
            *result = FALSE;
            return NOERROR;
        }
        default:{
            *result = TRUE;
            return NOERROR;
        }
    }
    assert(0 && "Has error.");
    return NOERROR;
}

ECode UiccCardApplication::SupplyPin(
    /* [in] */ const String& pin,
    /* [in] */ IMessage* onComplete)
{
    AutoLock lock(mLock);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_PIN1_PUK1_DONE, onComplete, (IMessage**)&msg);
    mCi->SupplyIccPinForApp(pin, mAid, msg);
    return NOERROR;
}

ECode UiccCardApplication::SupplyPuk(
    /* [in] */ const String& puk,
    /* [in] */ const String& newPin,
    /* [in] */ IMessage* onComplete)
{
    AutoLock lock(mLock);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_PIN1_PUK1_DONE, onComplete, (IMessage**)&msg);
    mCi->SupplyIccPukForApp(puk, newPin, mAid, msg);

    return NOERROR;
}

ECode UiccCardApplication::SupplyPin2(
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* onComplete)
{
    AutoLock lock(mLock);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_PIN2_PUK2_DONE, onComplete, (IMessage**)&msg);
    mCi->SupplyIccPin2ForApp(pin2, mAid, msg);
    return NOERROR;
}

ECode UiccCardApplication::SupplyPuk2(
    /* [in] */ const String& puk2,
    /* [in] */ const String& newPin2,
    /* [in] */ IMessage* onComplete)
{
    AutoLock lock(mLock);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_PIN2_PUK2_DONE, onComplete, (IMessage**)&msg);
    mCi->SupplyIccPuk2ForApp(puk2, newPin2, mAid, msg);
    return NOERROR;
}

ECode UiccCardApplication::SupplyDepersonalization(
    /* [in] */ const String& pin,
    /* [in] */ const String& type,
    /* [in] */ IMessage* onComplete)
{
    AutoLock lock(mLock);
    if (DBG) Log(String("Network Despersonalization: pin = **** , type = ") + type);
    mCi->SupplyDepersonalization(pin, type, onComplete);
    return NOERROR;
}

ECode UiccCardApplication::GetIccLockEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIccLockEnabled;
    /* STOPSHIP: Remove line above and all code associated with setting
       mIccLockEanbled once all RIL correctly sends the pin1 state.
    // Use getPin1State to take into account pin1Replaced flag
    PinState pinState = getPin1State();
    return pinState == PinState.PINSTATE_ENABLED_NOT_VERIFIED ||
           pinState == PinState.PINSTATE_ENABLED_VERIFIED ||
           pinState == PinState.PINSTATE_ENABLED_BLOCKED ||
           pinState == PinState.PINSTATE_ENABLED_PERM_BLOCKED;*/
    return NOERROR;
}

ECode UiccCardApplication::GetIccFdnEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    *result =  mIccFdnEnabled;
    return NOERROR;
}

ECode UiccCardApplication::GetIccFdnAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIccFdnAvailable;
    return NOERROR;
}

ECode UiccCardApplication::SetIccLockEnabled(
    /* [in] */ Boolean enabled,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    AutoLock lock(mLock);
    Int32 serviceClassX;
    serviceClassX = ICommandsInterface::SERVICE_CLASS_VOICE +
            ICommandsInterface::SERVICE_CLASS_DATA +
            ICommandsInterface::SERVICE_CLASS_FAX;

    mDesiredPinLocked = enabled;

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_CHANGE_FACILITY_LOCK_DONE, onComplete, (IMessage**)&msg);
    mCi->SetFacilityLockForApp(ICommandsInterface::CB_FACILITY_BA_SIM,
            enabled, password, serviceClassX, mAid, msg);
    return NOERROR;
}

ECode UiccCardApplication::SetIccFdnEnabled(
    /* [in] */ Boolean enabled,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    AutoLock lock(mLock);

    Int32 serviceClassX;
    serviceClassX = ICommandsInterface::SERVICE_CLASS_VOICE +
            ICommandsInterface::SERVICE_CLASS_DATA +
            ICommandsInterface::SERVICE_CLASS_FAX +
            ICommandsInterface::SERVICE_CLASS_SMS;

    mDesiredFdnEnabled = enabled;

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_CHANGE_FACILITY_FDN_DONE, onComplete, (IMessage**)&msg);
    mCi->SetFacilityLockForApp(ICommandsInterface::CB_FACILITY_BA_FD,
            enabled, password, serviceClassX, mAid, msg);
    return NOERROR;
}

ECode UiccCardApplication::ChangeIccLockPassword(
    /* [in] */ const String& oldPassword,
    /* [in] */ const String& newPassword,
    /* [in] */ IMessage* onComplete)
{
    AutoLock lock(mLock);
    if (DBG) Log(String("changeIccLockPassword"));
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_CHANGE_PIN1_DONE, onComplete, (IMessage**)&msg);
    mCi->ChangeIccPinForApp(oldPassword, newPassword, mAid, msg);
    return NOERROR;
}

ECode UiccCardApplication::ChangeIccFdnPassword(
    /* [in] */ const String& oldPassword,
    /* [in] */ const String& newPassword,
    /* [in] */ IMessage* onComplete)
{
    AutoLock lock(mLock);
    if (DBG) Log(String("changeIccFdnPassword"));
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_CHANGE_PIN2_DONE, onComplete, (IMessage**)&msg);
    mCi->ChangeIccPin2ForApp(oldPassword, newPassword, mAid, msg);
    return NOERROR;
}

ECode UiccCardApplication::GetIccPin2Blocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    *result = mPin2State == PINSTATE_ENABLED_BLOCKED;
    return NOERROR;
}

ECode UiccCardApplication::GetIccPuk2Blocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    *result = mPin2State == PINSTATE_ENABLED_PERM_BLOCKED;
    return NOERROR;
}

ECode UiccCardApplication::GetUICCConfig(
    /* [out] */ IUICCConfig** result)
{
    VALIDATE_NOT_NULL(result);
    return mUiccCard->GetUICCConfig(result);
}

ECode UiccCardApplication::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("UiccCardApplication: ") + TO_CSTR(this));
    pw->Println(String(" mUiccCard=") + TO_CSTR(mUiccCard));
    pw->Println(String(" mAppState=") + StringUtils::ToString(mAppState));
    pw->Println(String(" mAppType=") + StringUtils::ToString(mAppType));
    pw->Println(String(" mPersoSubState=") + StringUtils::ToString(mPersoSubState));
    pw->Println(String(" mAid=") + mAid);
    pw->Println(String(" mAppLabel=") + mAppLabel);
    pw->Println(String(" mPin1Replaced=") + StringUtils::ToString(mPin1Replaced));
    pw->Println(String(" mPin1State=") + StringUtils::ToString(mPin1State));
    pw->Println(String(" mPin2State=") + StringUtils::ToString(mPin2State));
    pw->Println(String(" mIccFdnEnabled=") + StringUtils::ToString(mIccFdnEnabled));
    pw->Println(String(" mDesiredFdnEnabled=") + StringUtils::ToString(mDesiredFdnEnabled));
    pw->Println(String(" mIccLockEnabled=") + StringUtils::ToString(mIccLockEnabled));
    pw->Println(String(" mDesiredPinLocked=") + StringUtils::ToString(mDesiredPinLocked));
    pw->Println(String(" mCi=") + TO_CSTR(mCi));
    pw->Println(String(" mIccRecords=") + TO_CSTR(mIccRecords));
    pw->Println(String(" mIccFh=") + TO_CSTR(mIccFh));
    pw->Println(String(" mDestroyed=") + StringUtils::ToString(mDestroyed));
    pw->Println(String(" mReadyRegistrants: size=") + StringUtils::ToString(mReadyRegistrants->GetSize()));
    for (Int32 i = 0; i < mReadyRegistrants->GetSize(); i++) {
        AutoPtr<IHandler> hdl;
        IRegistrant::Probe(mReadyRegistrants->Get(i))->GetHandler((IHandler**)&hdl);
        pw->Println(String("  mReadyRegistrants[") + StringUtils::ToString(i) +
                String("]=")/* + hdl*/);
    }
    pw->Println(String(" mPinLockedRegistrants: size=") + StringUtils::ToString(mPinLockedRegistrants->GetSize()));
    for (Int32 i = 0; i < mPinLockedRegistrants->GetSize(); i++) {
        AutoPtr<IHandler> hdl;
        IRegistrant::Probe(mPinLockedRegistrants->Get(i))->GetHandler((IHandler**)&hdl);
        pw->Println(String("  mPinLockedRegistrants[") + StringUtils::ToString(i) +
                    String("]=")/* + hdl*/);
    }
    pw->Println(String(" mPersoLockedRegistrants: size=") + StringUtils::ToString(mPersoLockedRegistrants->GetSize()));
    for (Int32 i = 0; i < mPersoLockedRegistrants->GetSize(); i++) {
        AutoPtr<IHandler> hdl;
        IRegistrant::Probe(mPersoLockedRegistrants->Get(i))->GetHandler((IHandler**)&hdl);
        pw->Println(String("  mPersoLockedRegistrants[") + StringUtils::ToString(i) +
                    String("]=")/* + hdl*/);
    }
    IFlushable::Probe(pw)->Flush();
    return NOERROR;
}

ECode UiccCardApplication::GetUiccCard(
    /* [out] */ IUiccCard** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUiccCard;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IIccRecords> UiccCardApplication::CreateIccRecords(
    /* [in] */ AppType type,
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci)
{
    AutoPtr<IIccRecords> ir;
    if (type == APPTYPE_USIM || type == APPTYPE_SIM) {
        CSIMRecords::New(this, c, ci, (IIccRecords**)&ir);
    }
    else if (type == APPTYPE_RUIM || type == APPTYPE_CSIM){
        CRuimRecords::New(this, c, ci, (IIccRecords**)&ir);
    }
    else if (type == APPTYPE_ISIM) {
        CIsimUiccRecords::New(this, c, ci, (IIccRecords**)&ir);
    }
    else {
        // Unknown app type (maybe detection is still in progress)
        ir = NULL;
    }
    return ir;
}

AutoPtr<IIccFileHandler> UiccCardApplication::CreateIccFileHandler(
    /* [in] */ AppType type)
{
    AutoPtr<IIccFileHandler> fh;
    switch (type) {
        case APPTYPE_SIM:
            CSIMFileHandler::New(this, mAid, mCi, (IIccFileHandler**)&fh);
            break;
        case APPTYPE_RUIM:
            CRuimFileHandler::New(this, mAid, mCi, (IIccFileHandler**)&fh);
            break;
        case APPTYPE_USIM: {// leliang this type
            CUsimFileHandler::New(this, mAid, mCi, (IIccFileHandler**)&fh);
            break;
        }
        case APPTYPE_CSIM:
            CCsimFileHandler::New(this, mAid, mCi, (IIccFileHandler**)&fh);
            break;
        case APPTYPE_ISIM:
            CIsimFileHandler::New(this, mAid, mCi, (IIccFileHandler**)&fh);
            break;
        default:
            fh = NULL;
    }
    return fh;
}

void UiccCardApplication::OnQueryFdnEnabled(
    /* [in] */ AsyncResult* ar)
{
    AutoLock lock(mLock);
    if (ar->mException != NULL) {
        if (DBG) Log(String("Error in querying facility lock:") + TO_CSTR(ar->mException));
        return;
    }

    AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
    Int32 len = 0;
    array->GetLength(&len);
    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        array->Get(i, (IInterface**)&obj);
        IInteger32::Probe(obj)->GetValue(&(*result)[i]);
    }
    if(result->GetLength() != 0) {
        //0 - Available & Disabled, 1-Available & Enabled, 2-Unavailable.
        if ((*result)[0] == 2) {
            mIccFdnEnabled = FALSE;
            mIccFdnAvailable = FALSE;
        }
        else {
            mIccFdnEnabled = ((*result)[0] == 1) ? TRUE : FALSE;
            mIccFdnAvailable = TRUE;
        }
        Log(String("Query facility FDN : FDN service available: ") + StringUtils::ToString(mIccFdnAvailable)
                +" enabled: "  + StringUtils::ToString(mIccFdnEnabled));
    }
    else {
        Loge(String("Bogus facility lock response"));
    }
}

void UiccCardApplication::OnChangeFdnDone(
    /* [in] */ AsyncResult* ar)
{
    AutoLock lock(mLock);
    Int32 attemptsRemaining = -1;

    if (ar->mException == NULL) {
        mIccFdnEnabled = mDesiredFdnEnabled;
        if (DBG) Log(String("EVENT_CHANGE_FACILITY_FDN_DONE: mIccFdnEnabled=") + StringUtils::ToString(mIccFdnEnabled));
    }
    else {
        attemptsRemaining = ParsePinPukErrorResult(ar);
        Loge(String("Error change facility fdn with exception ") + TO_CSTR(ar->mException));
    }
    AutoPtr<IMessage> response = IMessage::Probe(ar->mUserObj);
    response->SetArg1(attemptsRemaining);
    AsyncResult::ForMessage(response)->mException = ar->mException;
    response->SendToTarget();
}

void UiccCardApplication::QueryPin1State()
{
    Int32 serviceClassX = ICommandsInterface::SERVICE_CLASS_VOICE +
            ICommandsInterface::SERVICE_CLASS_DATA +
            ICommandsInterface::SERVICE_CLASS_FAX;
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_QUERY_FACILITY_LOCK_DONE, (IMessage**)&msg);
    mCi->QueryFacilityLockForApp(ICommandsInterface::CB_FACILITY_BA_SIM, String(""), serviceClassX,
        mAid, msg);
}

void UiccCardApplication::OnQueryFacilityLock(
    /* [in] */ AsyncResult* ar)
{
    AutoLock lock(mLock);
    if(ar->mException != NULL) {
        if (DBG) Log(String("Error in querying facility lock:") + TO_CSTR(ar->mException));
        return;
    }

    AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
    Int32 len = 0;
    array->GetLength(&len);
    AutoPtr<ArrayOf<Int32> > ints = ArrayOf<Int32>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        array->Get(i, (IInterface**)&obj);
        IInteger32::Probe(obj)->GetValue(&(*ints)[i]);
    }
    if(ints->GetLength() != 0) {
        if (DBG) Log(String("Query facility lock : ") + StringUtils::ToString((*ints)[0]));

        mIccLockEnabled = ((*ints)[0] != 0);

        if (mIccLockEnabled) {
            mPinLockedRegistrants->NotifyRegistrants();
        }

        // Sanity check: we expect mPin1State to match mIccLockEnabled.
        // When mPin1State is DISABLED mIccLockEanbled should be FALSE.
        // When mPin1State is ENABLED mIccLockEnabled should be TRUE.
        //
        // Here we validate these assumptions to assist in identifying which ril/radio's
        // have not correctly implemented GET_SIM_STATUS
        switch (mPin1State) {
            case PINSTATE_DISABLED:
                if (mIccLockEnabled) {
                    Loge(String("QUERY_FACILITY_LOCK:enabled GET_SIM_STATUS.Pin1:disabled. Fixme"));
                }
                break;
            case PINSTATE_ENABLED_NOT_VERIFIED:
            case PINSTATE_ENABLED_VERIFIED:
            case PINSTATE_ENABLED_BLOCKED:
            case PINSTATE_ENABLED_PERM_BLOCKED:
                if (!mIccLockEnabled) {
                    Loge(String("QUERY_FACILITY_LOCK:disabled GET_SIM_STATUS.Pin1:enabled. Fixme"));
                }
            case PINSTATE_UNKNOWN:
            default:
                if (DBG) Log(String("Ignoring: pin1state=") + StringUtils::ToString(mPin1State));
                break;
        }
    }
    else {
        Loge(String("Bogus facility lock response"));
    }
}

void UiccCardApplication::OnChangeFacilityLock(
    /* [in] */ AsyncResult* ar)
{
    AutoLock lock(mLock);
    Int32 attemptsRemaining = -1;

    if (ar->mException == NULL) {
        mIccLockEnabled = mDesiredPinLocked;
        if (DBG) Log(String("EVENT_CHANGE_FACILITY_LOCK_DONE: mIccLockEnabled= ")
                + StringUtils::ToString(mIccLockEnabled));
    }
    else {
        attemptsRemaining = ParsePinPukErrorResult(ar);
        Loge(String("Error change facility lock with exception ") + TO_CSTR(ar->mException));
    }
    AutoPtr<IMessage> response = IMessage::Probe(ar->mUserObj);
    AsyncResult::ForMessage(response)->mException = ar->mException;
    response->SetArg1(attemptsRemaining);
    response->SendToTarget();
}

Int32 UiccCardApplication::ParsePinPukErrorResult(
    /* [in] */ AsyncResult* ar)
{
    if (ar->mResult == NULL) {
        return -1;
    }

    AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
    Int32 len = 0;
    array->GetLength(&len);
    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        array->Get(i, (IInterface**)&obj);
        IInteger32::Probe(obj)->GetValue(&(*result)[i]);
    }
    if (result == NULL) {
        return -1;
    }

    Int32 length = len;
    Int32 attemptsRemaining = -1;
    if (length > 0) {
        attemptsRemaining = (*result)[0];
    }
    Log(String("parsePinPukErrorResult: attemptsRemaining=") + StringUtils::ToString(attemptsRemaining));
    return attemptsRemaining;
}

void UiccCardApplication::NotifyReadyRegistrantsIfNeeded(
    /* [in] */ Registrant* r)
{
    if (mDestroyed) {
        return;
    }
    if (mAppState == APPSTATE_READY) {
        if (mPin1State == PINSTATE_ENABLED_NOT_VERIFIED ||
                mPin1State == PINSTATE_ENABLED_BLOCKED ||
                mPin1State == PINSTATE_ENABLED_PERM_BLOCKED) {
            Loge(String("Sanity check failed! APPSTATE is ready while PIN1 is not verified!!!"));
            // Don't notify if application is in insane state
            return;
        }
        if (r == NULL) {
            if (DBG) Log(String("Notifying registrants: READY"));
            mReadyRegistrants->NotifyRegistrants();
        }
        else {
            if (DBG) Log(String("Notifying 1 registrant: READY"));
            AutoPtr<AsyncResult> ar = new AsyncResult(NULL, NULL, NULL);
            r->NotifyRegistrant(ar);
        }
    }
}

void UiccCardApplication::NotifyPinLockedRegistrantsIfNeeded(
    /* [in] */ Registrant* r)
{
    if (mDestroyed) {
        return;
    }

    if (mAppState == APPSTATE_PIN ||
            mAppState == APPSTATE_PUK) {
        if (mPin1State == PINSTATE_ENABLED_VERIFIED ||
                mPin1State == PINSTATE_DISABLED) {
            Loge(String("Sanity check failed! APPSTATE is locked while PIN1 is not!!!"));
            //Don't notify if application is in insane state
            return;
        }
        if (r == NULL) {
            if (DBG) Log(String("Notifying registrants: LOCKED"));
            mPinLockedRegistrants->NotifyRegistrants();
        }
        else {
            if (DBG) Log(String("Notifying 1 registrant: LOCKED"));
            AutoPtr<AsyncResult> p = new AsyncResult(NULL, NULL, NULL);
            r->NotifyRegistrant(p);
        }
    }
}

void UiccCardApplication::NotifyPersoLockedRegistrantsIfNeeded(
    /* [in] */ Registrant* r)
{
    if (mDestroyed) {
        return;
    }

    Boolean b = FALSE;
    IsPersoLocked(&b);
    if (mAppState == APPSTATE_SUBSCRIPTION_PERSO && b) {
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, CoreUtils::Convert(mPersoSubState), NULL);
        if (r == NULL) {
            if (DBG) Log(String("Notifying registrants: PERSO_LOCKED"));
            mPersoLockedRegistrants->NotifyRegistrants(ar);
        }
        else {
            if (DBG) Log(String("Notifying 1 registrant: PERSO_LOCKED"));
            r->NotifyRegistrant(ar);
        }
    }
}

Int32 UiccCardApplication::GetAuthContext(
    /* [in] */ AppType appType)
{
    Int32 authContext = 0;

    switch (appType) {
        case APPTYPE_SIM:
            authContext = AUTH_CONTEXT_EAP_SIM;
            break;

        case APPTYPE_USIM:
            authContext = AUTH_CONTEXT_EAP_AKA;
            break;

        default:
            authContext = AUTH_CONTEXT_UNDEFINED;
            break;
    }

    return authContext;
}

void UiccCardApplication::Log(
    /* [in] */ const String& msg)
{
    // Rlog.d(LOGTAG, msg);
    Logger::E("UiccCardApplication", "[Log] msg:%s", msg.string());
}

void UiccCardApplication::Loge(
    /* [in] */ const String& msg)
{
    // Rlog.e(LOGTAG, msg);
    Logger::E("UiccCardApplication", "[Loge] msg:%s", msg.string());
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
