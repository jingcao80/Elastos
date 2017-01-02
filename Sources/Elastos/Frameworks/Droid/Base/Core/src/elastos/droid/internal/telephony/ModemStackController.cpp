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

#include "elastos/droid/internal/telephony/ModemStackController.h"
#include "elastos/droid/internal/telephony/IccUtils.h"
#include "elastos/droid/internal/telephony/RIL.h"
#include "elastos/droid/telephony/CSubInfoRecord.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/os/Registrant.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::Registrant;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Telephony::CSubInfoRecord;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::ISubInfoRecord;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Core::AutoLock;
using Elastos::Core::CInteger32;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::IO::ByteOrder;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::CByteOrderHelper;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::IByteOrderHelper;
using Elastos::Utility::CCollections;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String ModemStackController::TAG("ModemStackController");
const Int32 ModemStackController::CMD_DEACTIVATE_ALL_SUBS = 1;
const Int32 ModemStackController::EVENT_GET_MODEM_CAPS_DONE = 2;
const Int32 ModemStackController::CMD_TRIGGER_UNBIND = 3;
const Int32 ModemStackController::EVENT_UNBIND_DONE = 4;
const Int32 ModemStackController::CMD_TRIGGER_BIND = 5;
const Int32 ModemStackController::EVENT_BIND_DONE = 6;
const Int32 ModemStackController::EVENT_SET_PREF_MODE_DONE = 7;
const Int32 ModemStackController::EVENT_SUB_DEACTIVATED = 8;
const Int32 ModemStackController::EVENT_RADIO_AVAILABLE = 9;
const Int32 ModemStackController::EVENT_MODEM_CAPABILITY_CHANGED = 10;
const Int32 ModemStackController::STATE_UNKNOWN = 1;
const Int32 ModemStackController::STATE_GOT_MODEM_CAPS = 2;
const Int32 ModemStackController::STATE_SUB_DEACT = 3;
const Int32 ModemStackController::STATE_UNBIND = 4;
const Int32 ModemStackController::STATE_BIND = 5;
const Int32 ModemStackController::STATE_SUB_ACT = 6;
const Int32 ModemStackController::STATE_SET_PREF_MODE = 7;
const Int32 ModemStackController::BIND_TO_STACK = 1;
const Int32 ModemStackController::UNBIND_TO_STACK = 0;
const Int32 ModemStackController::GET_MODEM_CAPS_BUFFER_LEN = 7;
const Int32 ModemStackController::SUCCESS = 1;
const Int32 ModemStackController::FAILURE = 0;
const Int32 ModemStackController::PRIMARY_STACK_ID = 0;
const Int32 ModemStackController::DEFAULT_MAX_DATA_ALLOWED = 1;
AutoPtr<ModemStackController> ModemStackController::sModemStackController;

ModemStackController::ModemCapabilityInfo::ModemCapabilityInfo(
    /* [in] */ Int32 stackId,
    /* [in] */ Int32 supportedRatBitMask,
    /* [in] */ Int32 voiceCap,
    /* [in] */ Int32 dataCap)
{
    mStackId = stackId;
    mSupportedRatBitMask = supportedRatBitMask;
    mVoiceDataCap = voiceCap;
    mMaxDataCap = dataCap;
}

Int32 ModemStackController::ModemCapabilityInfo::GetSupportedRatBitMask()
{
    return mSupportedRatBitMask;
}

Int32 ModemStackController::ModemCapabilityInfo::GetStackId()
{
    return mStackId;
}

Int32 ModemStackController::ModemCapabilityInfo::GetMaxDataCap()
{
    return mMaxDataCap;
}

ECode ModemStackController::ModemCapabilityInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("[stack = ") + StringUtils::ToString(mStackId) + ", SuppRatBitMask = "
        + StringUtils::ToString(mSupportedRatBitMask) + ", voiceDataCap = "
        + StringUtils::ToString(mVoiceDataCap) + ", maxDataCap = "
        + StringUtils::ToString(mMaxDataCap) +"]";
    return NOERROR;
}

ModemStackController::Receiver::Receiver(
    /* [in] */ ModemStackController* host)
    : mHost(host)
{
    BroadcastReceiver::constructor();
}

ECode ModemStackController::Receiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED.Equals(action)) {
        Boolean tmp = FALSE;
        if (intent->GetBooleanExtra(IPhoneConstants::PHONE_IN_ECM_STATE, FALSE, &tmp), tmp) {
            mHost->Logd(String("Device is in ECBM Mode"));
            mHost->mIsPhoneInEcbmMode = TRUE;
        }
        else {
            mHost->Logd(String("Device is out of ECBM Mode"));
            mHost->mIsPhoneInEcbmMode = FALSE;
        }
    }
    else if (ITelephonyIntents::ACTION_SUBINFO_CONTENT_CHANGE.Equals(action)) {
        Int64 subId = 0;
        intent->GetInt64Extra(IBaseColumns::ID,
                ISubscriptionManager::INVALID_SUB_ID, &subId);
        String column;
        intent->GetStringExtra(ITelephonyIntents::EXTRA_COLUMN_NAME, &column);
        Int32 intValue;
        intent->GetInt32Extra(ITelephonyIntents::EXTRA_INT_CONTENT, 0, &intValue);
        mHost->Logd(String("Received ACTION_SUBINFO_CONTENT_CHANGE on subId: ") + StringUtils::ToString(subId)
                + "for " + column + " intValue: " + StringUtils::ToString(intValue));

        if (mHost->mDeactivationInProgress && column != NULL
                && column.Equals(ISubscriptionManager::SUB_STATE)) {
            Int32 phoneId = 0;
            assert(0 && "TODO");
            // phoneId = SubscriptionController::GetInstance().getPhoneId(subId);
            Int32 v = 0;
            HashMap<Int32, Int32>::Iterator ator = mHost->mSubcriptionStatus.Find(phoneId);
            if (ator != mHost->mSubcriptionStatus.End()) {
                v = ator->mSecond;
            }
            if (intValue == ISubscriptionManager::INACTIVE &&
                    v == ISubscriptionManager::ACTIVE) {

                //deactivated the activated sub
                AutoPtr<IMessage> msg;
                AutoPtr<IInteger32> obj;
                CInteger32::New(phoneId, (IInteger32**)&obj);
                mHost->ObtainMessage(EVENT_SUB_DEACTIVATED, obj, (IMessage**)&msg);
                assert(0 && "TODO");
                // AsyncResult::ForMessage(msg, ISubscriptionStatus::SUB_DEACTIVATED, NULL);
                Boolean tmp = FALSE;
                mHost->SendMessage(msg, &tmp);
            }
        }
    }
    else if (ITelephonyIntents::ACTION_SUBSCRIPTION_SET_UICC_RESULT.Equals(action)) {
        Int64 subId = 0;
        intent->GetInt64Extra(IPhoneConstants::SUBSCRIPTION_KEY,
                ISubscriptionManager::INVALID_SUB_ID, &subId);
        Int32 phoneId = 0;
        intent->GetInt32Extra(IPhoneConstants::PHONE_KEY,
                IPhoneConstants::PHONE_ID1, &phoneId);
        Int32 status = 0;
        intent->GetInt32Extra(ITelephonyIntents::EXTRA_RESULT,
                IPhoneConstants::FAILURE, &status);
        mHost->Logd(String("Received ACTION_SUBSCRIPTION_SET_UICC_RESULT on subId: ")
                + StringUtils::ToString(subId)
                + "phoneId " + StringUtils::ToString(phoneId)
                + " status: " + StringUtils::ToString(status));
        if (mHost->mDeactivationInProgress && (status == IPhoneConstants::FAILURE)) {
            // Sub deactivation failed
            AutoPtr<IMessage> msg;
            AutoPtr<IInteger32> obj;
            CInteger32::New(phoneId, (IInteger32**)&obj);
            mHost->ObtainMessage(EVENT_SUB_DEACTIVATED, obj, (IMessage**)&msg);
            assert(0 && "TODO");
            // AsyncResult::ForMessage(msg, ISubscriptionStatus::SUB_ACTIVATED, NULL);
            Boolean tmp = FALSE;
            mHost->SendMessage(msg, &tmp);
        }
    }
    return NOERROR;
}

AutoPtr<ModemStackController> ModemStackController::Make(
    /* [in] */ IContext* context,
    /* [in] */ IUiccController* uiccMgr,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci)
{
    Logger::D(TAG, "getInstance");
    if (sModemStackController == NULL) {
        sModemStackController = new ModemStackController(context, uiccMgr, ci);
    }
    else {
        // throw new RuntimeException("ModemStackController.make() should only be called once");
        assert(0 && "ModemStackController.make() should only be called once");
    }
    return sModemStackController;
}

AutoPtr<ModemStackController> ModemStackController::GetInstance()
{
    if (sModemStackController == NULL) {
        // throw new RuntimeException("ModemStackController.getInstance called before make()");
        assert(0 && "ModemStackController.getInstance called before make()");
    }
    return sModemStackController;
}

//***** Constructor
ModemStackController::ModemStackController(
    /* [in] */ IContext* context,
    /* [in] */ IUiccController* uiccManager,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci)
    : mActiveSubCount(0)
    , mDeactivedSubCount(0)
    , mIsStackReady(FALSE)
    , mIsRecoveryInProgress(FALSE)
    , mIsPhoneInEcbmMode(FALSE)
    , mModemRatCapabilitiesAvailable(FALSE)
    , mDeactivationInProgress(FALSE)
{
    Handler::constructor();

    Logd(String("Constructor - Enter"));

    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    tm->GetPhoneCount(&mNumPhones);
    mPreferredStackId = ArrayOf<Int32>::Alloc(mNumPhones);
    mCurrentStackId = ArrayOf<Int32>::Alloc(mNumPhones);
    mPrefNwMode = ArrayOf<Int32>::Alloc(mNumPhones);
    mSubState = ArrayOf<Int32>::Alloc(mNumPhones);
    mCmdFailed = ArrayOf<Boolean>::Alloc(mNumPhones);
    mStackReadyRegistrants = new RegistrantList();
    mModemRatCapsAvailableRegistrants = new RegistrantList();
    mModemDataCapsAvailableRegistrants = new RegistrantList();
    mReceiver = new Receiver(this);

    mCi = ci;
    mContext = context;
    mModemCapInfo = ArrayOf<ModemCapabilityInfo*>::Alloc(mNumPhones);

    for (Int32 i = 0; i < mCi->GetLength(); i++) {
        if ((*mCi)[i] == NULL) continue;

        AutoPtr<IInteger32> obj;
        CInteger32::New(i, (IInteger32**)&obj);
        (*mCi)[i]->RegisterForAvailable(this, EVENT_RADIO_AVAILABLE, obj);
        (*mCi)[i]->RegisterForModemCapEvent(this, EVENT_MODEM_CAPABILITY_CHANGED, NULL);
    }

    for (Int32 i = 0; i < mNumPhones; i++) {
        (*mPreferredStackId)[i] = i;
        (*mCurrentStackId)[i] = i;
        (*mSubState)[i] = STATE_UNKNOWN;
        (*mCmdFailed)[i] = FALSE;
    }

    // In case of Single Sim, Stack is by default ready
    if (mNumPhones == 1) mIsStackReady = TRUE;

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED, (IIntentFilter**)&filter);
    filter->AddAction(ITelephonyIntents::ACTION_SUBINFO_CONTENT_CHANGE);
    filter->AddAction(ITelephonyIntents::ACTION_SUBSCRIPTION_SET_UICC_RESULT);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mReceiver, filter, (IIntent**)&intent);
    Logd(String("Constructor - Exit"));
}

ECode ModemStackController::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AsyncResult* ar = NULL;
    AutoPtr<IInteger32> phoneId;

    Int32 what = 0, v = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch(what) {
        case EVENT_RADIO_AVAILABLE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            phoneId = IInteger32::Probe(ar->mUserObj);
            Logd(String("EVENT_RADIO_AVAILABLE"));
            phoneId->GetValue(&v);
            ProcessRadioAvailable(ar, v);
            break;

        case EVENT_GET_MODEM_CAPS_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            phoneId = IInteger32::Probe(ar->mUserObj);
            Logd(String("EVENT_GET_MODEM_CAPS_DONE"));
            phoneId->GetValue(&v);
            AutoPtr<IArrayOf> result = IArrayOf::Probe(ar->mResult);
            AutoPtr<ArrayOf<Byte> > bs;
            if (result != NULL) {
                Int32 len = 0;
                result->GetLength(&len);
                bs = ArrayOf<Byte>::Alloc(len);
                for (Int32 i = 0; i < len; i++) {
                    AutoPtr<IInterface> o;
                    result->Get(i, (IInterface**)&o);
                    Byte b;
                    IByte::Probe(o)->GetValue(&b);
                    (*bs)[i] = b;
                }
            }

            OnGetModemCapabilityDone(ar, bs, v);
            break;
        }

        case EVENT_MODEM_CAPABILITY_CHANGED:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            Logd(String("EVENT_MODEM_CAPABILITY_CHANGED ar =") + TO_CSTR(ar));
            OnUnsolModemCapabilityChanged(ar);
            break;

        case CMD_DEACTIVATE_ALL_SUBS:
            Logd(String("CMD_DEACTIVATE_ALL_SUBS"));
            DeactivateAllSubscriptions();
            break;

        case EVENT_SUB_DEACTIVATED:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            phoneId = IInteger32::Probe(ar->mUserObj);
            Logd(String("EVENT_SUB_DEACTIVATED"));
            phoneId->GetValue(&v);
            OnSubDeactivated(ar, v);
            break;

        case CMD_TRIGGER_UNBIND:
            phoneId = IInteger32::Probe(obj);
            Logd(String("CMD_TRIGGER_UNBIND"));
            phoneId->GetValue(&v);
            UnbindStackOnSub(v);
            break;

        case EVENT_UNBIND_DONE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            phoneId = IInteger32::Probe(ar->mUserObj);
            Logd(String("EVENT_UNBIND_DONE"));
            phoneId->GetValue(&v);
            OnUnbindComplete(ar, v);
            break;

        case CMD_TRIGGER_BIND:
            phoneId = IInteger32::Probe(obj);
            Logd(String("CMD_TRIGGER_BIND"));
            phoneId->GetValue(&v);
            BindStackOnSub(v);
            break;

        case EVENT_BIND_DONE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            phoneId = IInteger32::Probe(ar->mUserObj);
            Logd(String("EVENT_BIND_DONE"));
            phoneId->GetValue(&v);
            OnBindComplete(ar, v);
            break;

        case EVENT_SET_PREF_MODE_DONE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            phoneId = IInteger32::Probe(ar->mUserObj);
            Logd(String("EVENT_SET_PREF_MODE_DONE"));
            phoneId->GetValue(&v);
            OnSetPrefNwModeDone(ar, v);
            break;

        default:
            break;
    }
    return NOERROR;
}

void ModemStackController::ProcessRadioAvailable(
    /* [in] */ AsyncResult* ar,
    /* [in] */ Int32 phoneId)
{
    Logd(String("ProcessRadioAvailable on phoneId = ") + StringUtils::ToString(phoneId));

    if (phoneId >= 0 && phoneId < mNumPhones) {
        AutoPtr<IInteger32> obj;
        CInteger32::New(phoneId, (IInteger32**)&obj);
        AutoPtr<IMessage> getModemCapsMsg = CMessage::Obtain(this, EVENT_GET_MODEM_CAPS_DONE, obj);

        (*mCi)[phoneId]->GetModemCapability(getModemCapsMsg);
    }
    else {
        Loge(String("Invalid Index!!!"));
    }
}

void ModemStackController::OnGetModemCapabilityDone(
    /* [in] */ AsyncResult* ar,
    /* [in] */ ArrayOf<Byte>* result,
    /* [in] */ Int32 phoneId)
{
    if (result == NULL/* && ar.exception instanceof CommandException*/) {
        Loge(String("onGetModemCapabilityDone: EXIT!, result NULL or Exception =") + TO_CSTR(ar->mException));
        //On Modem Packages which do not support GetModemCaps RIl will return exception
        //On such Modem packages notify stack is ready so that SUB Activation can continue.
        NotifyStackReady(FALSE);
        return;
    }

    // Logd(String("onGetModemCapabilityDone on phoneId[")
    //         + StringUtils::ToString(phoneId)
    //         + "] result = " + result);

    if (phoneId >= 0 && phoneId < mNumPhones) {
        //Parse response and store it.
        (*mSubState)[phoneId] = STATE_GOT_MODEM_CAPS;
        ParseGetModemCapabilityResponse(result, phoneId);

        //Wait till we get Modem Capabilities on all subs
        if (AreAllSubsinSameState(STATE_GOT_MODEM_CAPS)) {
            NotifyModemRatCapabilitiesAvailable();
        }
    }
    else {
        Loge(String("Invalid Index!!!"));
    }
}

void ModemStackController::OnUnsolModemCapabilityChanged(
    /* [in] */ AsyncResult* ar)
{
    Logd(String("onUnsolModemCapabilityChanged"));
    AutoPtr<RIL::UnsolOemHookBuffer> unsolOemHookBuffer = (RIL::UnsolOemHookBuffer*)IObject::Probe(ar->mResult);

    assert(0 && "TODO");
    // if(unsolOemHookBuffer == NULL && ar.exception instanceof CommandException) {
    //     Loge(String("onUnsolModemCapabilityChanged: EXIT!, result NULL or Exception =") + TO_CSTR(ar->mException));
    //     return;
    // }
    AutoPtr<ArrayOf<Byte> > data;
    unsolOemHookBuffer->GetUnsolOemHookBuffer((ArrayOf<Byte>**)&data);
    Int32 phoneId = 0;
    unsolOemHookBuffer->GetRilInstance(&phoneId);

    Logd(String("onUnsolModemCapabilityChanged on phoneId = ") + StringUtils::ToString(phoneId));

    ParseGetModemCapabilityResponse(data, phoneId);
    NotifyModemDataCapabilitiesAvailable();
}

void ModemStackController::OnSubDeactivated(
    /* [in] */ AsyncResult* ar,
    /* [in] */ Int32 phoneId)
{
    assert(0 && "TODO");
    // SubscriptionStatus subStatus = (SubscriptionStatus)ar.result;
    // if (subStatus == NULL ||
    //         (subStatus != NULL && SubscriptionStatus.SUB_DEACTIVATED != subStatus)) {
    //     Loge("onSubDeactivated on phoneId[" + phoneId + "] Failed!!!");
    //     mCmdFailed[phoneId] = TRUE;
    // }

    // Logd("onSubDeactivated on phoneId[" + phoneId + "] subStatus = " + subStatus);

    //avoid duplicate entries
    if ((*mSubState)[phoneId] == STATE_SUB_DEACT) return;

    (*mSubState)[phoneId] = STATE_SUB_DEACT;
    mDeactivedSubCount++;

    //Wait till we get Sub Deact response on all active subs
    if (mDeactivedSubCount == mActiveSubCount) {
        //if any deact failed notify registrants to activate any deactivated subs
        //and stop binding process. No need to recover here.
        if (IsAnyCmdFailed()) {
            NotifyStackReady(FALSE);
        }
        else {
            mDeactivationInProgress = FALSE;
            TriggerUnBindingOnAllSubs();
        }
    }
}

void ModemStackController::BindStackOnSub(
    /* [in] */ Int32 phoneId)
{
    Logd(String("bindStack ") + StringUtils::ToString((*mPreferredStackId)[phoneId])
            + " On phoneId[" + StringUtils::ToString(phoneId) + "]");
    AutoPtr<IInteger32> obj;
    CInteger32::New(phoneId, (IInteger32**)&obj);
    AutoPtr<IMessage> msg = CMessage::Obtain(this, EVENT_BIND_DONE, obj);
    (*mCi)[phoneId]->UpdateStackBinding((*mPreferredStackId)[phoneId], BIND_TO_STACK, msg);
}

void ModemStackController::UnbindStackOnSub(
    /* [in] */ Int32 phoneId)
{
    Logd(String("unbindStack ") + StringUtils::ToString((*mCurrentStackId)[phoneId])
            + " On phoneId[" + StringUtils::ToString(phoneId) + "]");

    AutoPtr<IInteger32> obj;
    CInteger32::New(phoneId, (IInteger32**)&obj);
    AutoPtr<IMessage> msg = CMessage::Obtain(this, EVENT_UNBIND_DONE, obj);
    (*mCi)[phoneId]->UpdateStackBinding((*mCurrentStackId)[phoneId], UNBIND_TO_STACK, msg);
}

void ModemStackController::OnUnbindComplete(
    /* [in] */ AsyncResult* ar,
    /* [in] */ Int32 phoneId)
{
    assert(0 && "TODO");
    // if (ar.exception instanceof CommandException) {
    //     mCmdFailed[phoneId] = TRUE;
    //     Loge("onUnbindComplete(" + phoneId + "): got Exception =" + ar.exception);
    // }

    (*mSubState)[phoneId] = STATE_UNBIND;

    //Wait till we get UNBIND response on all subs
    if (AreAllSubsinSameState(STATE_UNBIND)) {
        if (IsAnyCmdFailed()) {
            RecoverToPrevState();
            return;
        }
        TriggerBindingOnAllSubs();
    }
}

void ModemStackController::OnBindComplete(
    /* [in] */ AsyncResult* ar,
    /* [in] */ Int32 phoneId)
{
    assert(0 && "TODO");
    // if (ar.exception instanceof CommandException) {
    //     mCmdFailed[phoneId] = TRUE;
    //     Loge("onBindComplete(" + phoneId + "): got Exception =" + ar.exception);
    // }

    (*mSubState)[phoneId] = STATE_BIND;

    //Wait till we get BIND response on all subs
    if (AreAllSubsinSameState(STATE_BIND)) {
        if (IsAnyCmdFailed()) {
            RecoverToPrevState();
            return;
        }
        SetPrefNwTypeOnAllSubs();
    }
}

void ModemStackController::OnSetPrefNwModeDone(
    /* [in] */ AsyncResult* ar,
    /* [in] */ Int32 phoneId)
{
    assert(0 && "TODO");
    // if (ar.exception instanceof CommandException) {
    //     mCmdFailed[phoneId] = TRUE;
    //     Loge("onSetPrefNwModeDone(SUB:" + phoneId + "): got Exception =" + ar.exception);
    // }

    (*mSubState)[phoneId] = STATE_SET_PREF_MODE;

    //Wait till we get Set Pref NwMode response on all subs
    if (AreAllSubsinSameState(STATE_SET_PREF_MODE)) {
        if (IsAnyCmdFailed()) {
            RecoverToPrevState();
            return;
        }

        if (mUpdateStackMsg != NULL) {
            SendResponseToTarget(mUpdateStackMsg, IRILConstants::SUCCESS);
            mUpdateStackMsg = NULL;
        }
        UpdateNetworkSelectionMode();
        NotifyStackReady(TRUE);
    }
}

void ModemStackController::UpdateNetworkSelectionMode()
{
    for (Int32 i = 0; i < mNumPhones; i++) {
        (*mCi)[i]->SetNetworkSelectionModeAutomatic(NULL);
    }
}

void ModemStackController::TriggerUnBindingOnAllSubs()
{
    ResetSubStates();
    Boolean tmp = FALSE;
    for (Int32 i = 0; i < mNumPhones; i++) {
        AutoPtr<IInteger32> obj;
        CInteger32::New(i, (IInteger32**)&obj);
        AutoPtr<IMessage> msg;
        ObtainMessage(CMD_TRIGGER_UNBIND, obj, (IMessage**)&msg);
        SendMessage(msg, &tmp);
    }
}

void ModemStackController::TriggerBindingOnAllSubs()
{
    ResetSubStates();
    Boolean tmp = FALSE;
    for (Int32 i = 0; i < mNumPhones; i++) {
        AutoPtr<IInteger32> obj;
        CInteger32::New(i, (IInteger32**)&obj);
        AutoPtr<IMessage> msg;
        ObtainMessage(CMD_TRIGGER_BIND, obj, (IMessage**)&msg);
        SendMessage(msg, &tmp);
    }
}

void ModemStackController::TriggerDeactivationOnAllSubs()
{
    ResetSubStates();
    AutoPtr<IMessage> msg;
    ObtainMessage(CMD_DEACTIVATE_ALL_SUBS, (IMessage**)&msg);
    Boolean tmp = FALSE;
    SendMessage(msg, &tmp);
}

void ModemStackController::SetPrefNwTypeOnAllSubs()
{
    ResetSubStates();
    for (Int32 i = 0; i < mNumPhones; i++) {
        AutoPtr<IInteger32> obj;
        CInteger32::New(i, (IInteger32**)&obj);
        AutoPtr<IMessage> resp;
        ObtainMessage(EVENT_SET_PREF_MODE_DONE, obj, (IMessage**)&resp);
        (*mCi)[i]->SetPreferredNetworkType((*mPrefNwMode)[i], resp);
    }
}

Boolean ModemStackController::AreAllSubsinSameState(
    /* [in] */ Int32 state)
{
    for (Int32 i = 0; i < mSubState->GetLength(); i++) {
        Int32 subState = (*mSubState)[i];
        Logd(String("AreAllSubsinSameState state= ") + StringUtils::ToString(state)
                + " substate=" + StringUtils::ToString(subState));
        if (subState != state) return FALSE;
    }
    return TRUE;
}

void ModemStackController::ResetSubStates()
{
    for (Int32 i = 0; i < mNumPhones; i++) {
        (*mSubState)[i] = STATE_UNKNOWN;
        (*mCmdFailed)[i] = FALSE;
    }
}

Boolean ModemStackController::IsAnyCmdFailed()
{
    Boolean result = FALSE;
    for (Int32 i = 0; i < mNumPhones; i++) {
        if ((*mCmdFailed)[i] != FALSE) {
            result = TRUE;
        }
    }
    return result;
}

void ModemStackController::UpdateModemCapInfo(
    /* [in] */ Int32 phoneId,
    /* [in] */ Int32 stackId,
    /* [in] */ Int32 supportedRatBitMask,
    /* [in] */ Int32 voiceDataCap,
    /* [in] */ Int32 maxDataCap)
{
    (*mCurrentStackId)[phoneId] = stackId;
    //Modem Capabilities are stored per StackId.
    mModemCapInfo->Set((*mCurrentStackId)[phoneId]
            , new ModemCapabilityInfo((*mCurrentStackId)[phoneId], supportedRatBitMask, voiceDataCap , maxDataCap));
    String s;
    (*mModemCapInfo)[(*mCurrentStackId)[phoneId]]->ToString(&s);
    Logd(String("UpdateModemCapInfo: ModemCaps[") + StringUtils::ToString(phoneId) + "]" + s);
}

void ModemStackController::ParseGetModemCapabilityResponse(
    /* [in] */ ArrayOf<Byte>* result,
    /* [in] */ Int32 phoneId)
{
    Int32 supportedRatBitMask = 0;
    Int32 stackId = 0;
    Int32 voiceDataCap = 0;
    Int32 maxDataCap = 0;

    Int32 len = result->GetLength();
    if (len != GET_MODEM_CAPS_BUFFER_LEN) {
        Loge(String("parseGetModemCapabilityResponse: EXIT!, result length(") + StringUtils::ToString(len)
                + ") and Expected length("
                + StringUtils::ToString(GET_MODEM_CAPS_BUFFER_LEN) + ") not matching.");
        return;
    }
    Logd(String("parseGetModemCapabilityResponse: buffer = ") + IccUtils::BytesToHexString(result));
    AutoPtr<IByteBuffer> respBuffer;
    AutoPtr<IByteBufferHelper> helper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    helper->Wrap(result, (IByteBuffer**)&respBuffer);

    AutoPtr<IByteOrderHelper> orderHelper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&orderHelper);
    ByteOrder order;
    orderHelper->GetNativeOrder(&order);
    respBuffer->SetOrder(order);

    Byte bv;
    respBuffer->Get(&bv);
    stackId = bv;

    if ( !(stackId >= 0 && stackId < mNumPhones) ) {
        Loge(String("Invalid Index!!!"));
        return;
    }
    respBuffer->GetInt32(&supportedRatBitMask);
    respBuffer->Get(&bv);
    voiceDataCap = bv;
    respBuffer->Get(&bv);
    maxDataCap = bv;

    UpdateModemCapInfo(phoneId, stackId, supportedRatBitMask, voiceDataCap, maxDataCap);
}

void ModemStackController::SyncPreferredNwModeFromDB()
{
    for (Int32 i = 0; i < mNumPhones; i++) {
        // try {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        if (FAILED(CTelephonyManager::GetInt32AtIndex(cr,
                ISettingsGlobal::PREFERRED_NETWORK_MODE, i, &(*mPrefNwMode)[i]))) {
            Loge(String("getPreferredNetworkMode: Could not find PREFERRED_NETWORK_MODE!!!"));
            (*mPrefNwMode)[i] = IPhone::PREFERRED_NT_MODE;
        }
        // } catch (SettingNotFoundException snfe) {
        //     Loge("getPreferredNetworkMode: Could not find PREFERRED_NETWORK_MODE!!!");
        //     mPrefNwMode[i] = Phone.PREFERRED_NT_MODE;
        // }
    }
}

Boolean ModemStackController::IsAnyCallsInProgress()
{
    Boolean isCallInProgress = FALSE;
    Int32 v = 0;
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    for (Int32 i = 0; i < mNumPhones; i++) {
        tm->GetCallState(i, &v);
        if (v != ITelephonyManager::CALL_STATE_IDLE) {
            isCallInProgress = TRUE;
            break;
        }
    }
    return isCallInProgress;
}

Boolean ModemStackController::IsStackReady()
{
    return mIsStackReady;
}

Int32 ModemStackController::GetMaxDataAllowed()
{
    Logd(String("getMaxDataAllowed"));
    Int32 ret = DEFAULT_MAX_DATA_ALLOWED;
    AutoPtr<IList> unsortedList;
    CArrayList::New((IList**)&unsortedList);

    for (Int32 i = 0; i < mNumPhones; i++) {
        if ((*mModemCapInfo)[i] != NULL) {
            AutoPtr<IInteger32> obj;
            CInteger32::New((*mModemCapInfo)[i]->GetMaxDataCap(), (IInteger32**)&obj);
            unsortedList->Add(obj);
        }
    }

    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    helper->Sort(unsortedList);
    Int32 listSize = 0;
    unsortedList->GetSize(&listSize);
    if (listSize > 0) {
        AutoPtr<IInterface> obj;
        unsortedList->Get(listSize - 1, (IInterface**)&obj);
        IInteger32::Probe(obj)->GetValue(&ret);
    }
    return ret;
}

Int32 ModemStackController::GetCurrentStackIdForPhoneId(
    /* [in] */ Int32 phoneId)
{
    return (*mCurrentStackId)[phoneId];
}

Int32 ModemStackController::GetPrimarySub()
{
    for (Int32 i = 0; i < mNumPhones; i++) {
        if (GetCurrentStackIdForPhoneId(i) == PRIMARY_STACK_ID) {
            return i;
        }
    }
    return 0;
}

AutoPtr<ModemStackController::ModemCapabilityInfo> ModemStackController::GetModemRatCapsForPhoneId(
    /* [in] */ Int32 phoneId)
{
    return (*mModemCapInfo)[(*mCurrentStackId)[phoneId]];
}

Int32 ModemStackController::UpdateStackBinding(
    /* [in] */ ArrayOf<Int32>* prefStackIds,
    /* [in] */ Boolean isBootUp,
    /* [in] */ IMessage* msg)
{
    Boolean isUpdateRequired = FALSE;
    Boolean callInProgress = IsAnyCallsInProgress();

    if (mNumPhones == 1) {
        Loge(String("No need to update Stack Binding in case of Single Sim."));
        return FAILURE;
    }

    if (callInProgress || mIsPhoneInEcbmMode || (!mIsStackReady && !isBootUp)) {
        Loge(String("updateStackBinding: Calls is progress = ")
                + StringUtils::ToString(callInProgress) +
                ", mIsPhoneInEcbmMode = " + StringUtils::ToString(mIsPhoneInEcbmMode)
                + ", mIsStackReady = "
                + StringUtils::ToString(mIsStackReady) + ". So EXITING!!!");
        return FAILURE;
    }
    for (Int32 i = 0; i < mNumPhones; i++) {
        (*mPreferredStackId)[i] = (*prefStackIds)[i];
    }

    for (Int32 i = 0; i < mNumPhones; i++) {
        if ((*mPreferredStackId)[i] != (*mCurrentStackId)[i]) {
            //if preferred stackId is different from current, bindupdate is required.
            isUpdateRequired = TRUE;
            break;
        }
    }

    if (isUpdateRequired) {
        mIsStackReady = FALSE;
        //Store the msg object , so that result of updateStackbinding can be sent later.
        mUpdateStackMsg = msg;
        //Get Stored prefNwMode for all the subs and send request to RIL after update binding.
        SyncPreferredNwModeFromDB();
        if (isBootUp) {
            TriggerUnBindingOnAllSubs();
        }
        else {
            TriggerDeactivationOnAllSubs();
        }
    }
    else {
        //incase of bootup if cross binding is not required send stack ready notification.
        if (isBootUp) NotifyStackReady(FALSE);
        return FAILURE;
    }
    return SUCCESS;
}

void ModemStackController::DeactivateAllSubscriptions()
{
    AutoPtr<ISubscriptionController> subCtrlr;
    assert(0 && "TODO");
    // subCtrlr = SubscriptionController.getInstance();
    AutoPtr<IList> subInfoList;
    IISub::Probe(subCtrlr)->GetActiveSubInfoList((IList**)&subInfoList);
    mActiveSubCount = 0;
    if (subInfoList == NULL) {
        //if getting sub info list is failed, abort cross mapping process.
        NotifyStackReady(FALSE);
        return;
    }

    AutoPtr<IIterator> iterator;
    subInfoList->GetIterator((IIterator**)&iterator);
    Boolean hasNext;
    while (iterator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);

        CSubInfoRecord* subInfo = (CSubInfoRecord*)ISubInfoRecord::Probe(obj);
        Int32 subStatus = 0;
        IISub::Probe(subCtrlr)->GetSubState(subInfo->mSubId, &subStatus);
        assert(0 && "TODO");
        // if (subStatus == ISubscriptionManager::ACTIVE) {
        //     mActiveSubCount++;
        //     subCtrlr->DeactivateSubId(subInfo->mSubId);
        // }
        mSubcriptionStatus[subInfo->mSlotId] = subStatus;
    }

    if (mActiveSubCount > 0) {
        mDeactivedSubCount = 0;
        mDeactivationInProgress = TRUE;
    }
    else {
        mDeactivationInProgress = FALSE;
        TriggerUnBindingOnAllSubs();
    }
}

void ModemStackController::NotifyStackReady(
    /* [in] */ Boolean isCrossMapDone)
{
    Logd(String("NotifyStackReady: Stack is READY!!!"));
    mIsRecoveryInProgress = FALSE;
    mIsStackReady = TRUE;
    ResetSubStates();

    if (isCrossMapDone) {
        for (Int32 i = 0; i < mNumPhones; i++) {
            //update the current stackIds
            (*mCurrentStackId)[i] = (*mPreferredStackId)[i];
        }
    }
    //notify binding completed to all StackReady registrants.
    //including subscriptionManager which activates available subs on binding complete.
    mStackReadyRegistrants->NotifyRegistrants();
}

void ModemStackController::RegisterForStackReady(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    if (mIsStackReady) {
        r->NotifyRegistrant();
    }
    {
        AutoLock lock(mStackReadyRegistrants);
        mStackReadyRegistrants->Add(r);
    }
}

void ModemStackController::NotifyModemRatCapabilitiesAvailable()
{
    Logd(String("notifyGetRatCapabilitiesDone: Got RAT capabilities for all Stacks!!!"));
    mModemRatCapabilitiesAvailable = TRUE;
    mModemRatCapsAvailableRegistrants->NotifyRegistrants();
}

void ModemStackController::NotifyModemDataCapabilitiesAvailable()
{
    Logd(String("notifyGetDataCapabilitiesDone"));
    mModemDataCapsAvailableRegistrants->NotifyRegistrants();
}

void ModemStackController::RegisterForModemRatCapsAvailable(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    if (mModemRatCapabilitiesAvailable) {
        r->NotifyRegistrant();
    }
    {
        AutoLock lock(mModemRatCapsAvailableRegistrants);
        mModemRatCapsAvailableRegistrants->Add(r);
    }
}

void ModemStackController::RegisterForModemDataCapsUpdate(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);

    {
        AutoLock lock(mModemDataCapsAvailableRegistrants);
        mModemDataCapsAvailableRegistrants->Add(r);
    }
}

void ModemStackController::RecoverToPrevState()
{
    if (mIsRecoveryInProgress) {
        //Already in recovery process and exception occured again.
        //Get stored msg if available and send exception and stop recovery.
        if (mUpdateStackMsg != NULL) {
            SendResponseToTarget(mUpdateStackMsg, IRILConstants::GENERIC_FAILURE);
            mUpdateStackMsg = NULL;
        }
        mIsRecoveryInProgress = FALSE;
        if(STATE_SET_PREF_MODE == (*mSubState)[0]) {
            //Already recovery in progress, got failure in SetPrefNwmode. We are bailing out.
            //As Set Pref is failed, Binding is completed. so update and notify same.
            NotifyStackReady(TRUE);
        }
        return;
    }

    mIsRecoveryInProgress = TRUE;
    //Binding to Preferred Stack failed, recovery mode: fallback to Current stackIds
    for (Int32 i = 0; i < mNumPhones; i++) {
        (*mPreferredStackId)[i] = (*mCurrentStackId)[i];
    }
    TriggerUnBindingOnAllSubs();
}

void ModemStackController::SendResponseToTarget(
    /* [in] */ IMessage* response,
    /* [in] */ Int32 responseCode)
{
    assert(0 && "TODO");
    // Exception e = CommandException.fromRilErrno(responseCode);
    // AsyncResult::ForMessage(response, NULL, e);
    response->SendToTarget();
}

void ModemStackController::Logd(
    /* [in] */ const String& string)
{
    Logger::D(TAG, string);
}

void ModemStackController::Loge(
    /* [in] */ const String& string)
{
    Logger::E(TAG, string);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
