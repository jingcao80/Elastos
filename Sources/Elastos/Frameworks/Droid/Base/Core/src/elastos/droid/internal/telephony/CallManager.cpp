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

#include "elastos/droid/internal/telephony/CallManager.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Registrant.h"
#include "Elastos.Droid.Telecom.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::ImsPhone::IImsPhone;
using Elastos::Droid::Internal::Telephony::Sip::ISipPhone;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::Registrant;
using Elastos::Droid::Telecom::IVideoProfileVideoState;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Core::CInteger64;
using Elastos::Core::IInteger64;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String CallManager::TAG("CallManager");
const Boolean CallManager::DBG = TRUE;
const Boolean CallManager::VDBG = FALSE;
const Int32 CallManager::EVENT_DISCONNECT = 100;
const Int32 CallManager::EVENT_PRECISE_CALL_STATE_CHANGED = 101;
const Int32 CallManager::EVENT_NEW_RINGING_CONNECTION = 102;
const Int32 CallManager::EVENT_UNKNOWN_CONNECTION = 103;
const Int32 CallManager::EVENT_INCOMING_RING = 104;
const Int32 CallManager::EVENT_RINGBACK_TONE = 105;
const Int32 CallManager::EVENT_IN_CALL_VOICE_PRIVACY_ON = 106;
const Int32 CallManager::EVENT_IN_CALL_VOICE_PRIVACY_OFF = 107;
const Int32 CallManager::EVENT_CALL_WAITING = 108;
const Int32 CallManager::EVENT_DISPLAY_INFO = 109;
const Int32 CallManager::EVENT_SIGNAL_INFO = 110;
const Int32 CallManager::EVENT_CDMA_OTA_STATUS_CHANGE = 111;
const Int32 CallManager::EVENT_RESEND_INCALL_MUTE = 112;
const Int32 CallManager::EVENT_MMI_INITIATE = 113;
const Int32 CallManager::EVENT_MMI_COMPLETE = 114;
const Int32 CallManager::EVENT_ECM_TIMER_RESET = 115;
const Int32 CallManager::EVENT_SUBSCRIPTION_INFO_READY = 116;
const Int32 CallManager::EVENT_SUPP_SERVICE_FAILED = 117;
const Int32 CallManager::EVENT_SERVICE_STATE_CHANGED = 118;
const Int32 CallManager::EVENT_POST_DIAL_CHARACTER = 119;
const Int32 CallManager::EVENT_ONHOLD_TONE = 120;
const Int32 CallManager::EVENT_SUPP_SERVICE_NOTIFY = 121;

CallManager::CallManagerHandler::CallManagerHandler(
    /* [in] */ CallManager* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CallManager::CallManagerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case EVENT_DISCONNECT:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_DISCONNECT)");
            mHost->mDisconnectRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_PRECISE_CALL_STATE_CHANGED:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_PRECISE_CALL_STATE_CHANGED)");
            mHost->mPreciseCallStateRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_NEW_RINGING_CONNECTION: {
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_NEW_RINGING_CONNECTION)");
            AutoPtr<IConnection> c = IConnection::Probe(((AsyncResult*) IAsyncResult::Probe(obj))->mResult);
            AutoPtr<ICall> call;
            c->GetCall((ICall**)&call);
            AutoPtr<IPhone> phone;
            call->GetPhone((IPhone**)&phone);
            Int64 subId = 0;
            phone->GetSubId(&subId);
            ICallState state;
            mHost->GetActiveFgCallState(subId, &state);
            Boolean isDialing = state == ICallState_DIALING || state == ICallState_ALERTING;
            if (isDialing || mHost->HasMoreThanOneRingingCall(subId)) {
                Logger::D(TAG, "silently drop incoming call: %s", TO_CSTR(call));
                if (FAILED(call->Hangup())) {
                    Logger::W(TAG, "new ringing connection");
                }
            }
            else {
                mHost->mNewRingingConnectionRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            }
            break;
        }
        case EVENT_UNKNOWN_CONNECTION:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_UNKNOWN_CONNECTION)");
            mHost->mUnknownConnectionRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_INCOMING_RING: {
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_INCOMING_RING)");
            // The event may come from RIL who's not aware of an ongoing fg call
            Boolean has = FALSE;
            mHost->HasActiveFgCall(&has);
            if (!has) {
                mHost->mIncomingRingRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            }
            break;
        }
        case EVENT_RINGBACK_TONE:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_RINGBACK_TONE)");
            mHost->mRingbackToneRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_IN_CALL_VOICE_PRIVACY_ON:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_IN_CALL_VOICE_PRIVACY_ON)");
            mHost->mInCallVoicePrivacyOnRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_IN_CALL_VOICE_PRIVACY_OFF:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_IN_CALL_VOICE_PRIVACY_OFF)");
            mHost->mInCallVoicePrivacyOffRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_CALL_WAITING:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_CALL_WAITING)");
            mHost->mCallWaitingRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_DISPLAY_INFO:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_DISPLAY_INFO)");
            mHost->mDisplayInfoRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_SIGNAL_INFO:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_SIGNAL_INFO)");
            mHost->mSignalInfoRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_CDMA_OTA_STATUS_CHANGE:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_CDMA_OTA_STATUS_CHANGE)");
            mHost->mCdmaOtaStatusChangeRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_RESEND_INCALL_MUTE:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_RESEND_INCALL_MUTE)");
            mHost->mResendIncallMuteRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_MMI_INITIATE:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_MMI_INITIATE)");
            mHost->mMmiInitiateRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_MMI_COMPLETE:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_MMI_COMPLETE)");
            mHost->mMmiCompleteRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_ECM_TIMER_RESET:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_ECM_TIMER_RESET)");
            mHost->mEcmTimerResetRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_SUBSCRIPTION_INFO_READY:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_SUBSCRIPTION_INFO_READY)");
            mHost->mSubscriptionInfoReadyRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_SUPP_SERVICE_NOTIFY:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_SUPP_SERVICE_NOTIFY)");
            mHost->mSuppServiceNotifyRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_SUPP_SERVICE_FAILED:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_SUPP_SERVICE_FAILED)");
            mHost->mSuppServiceFailedRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_SERVICE_STATE_CHANGED:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_SERVICE_STATE_CHANGED)");
            mHost->mServiceStateChangedRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
        case EVENT_POST_DIAL_CHARACTER:
            // we need send the character that is being processed in msg.arg1
            // so can't use notifyRegistrants()
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_POST_DIAL_CHARACTER)");
            for(Int32 i = 0; i < mHost->mPostDialCharacterRegistrants->GetSize(); i++) {
                AutoPtr<IMessage> notifyMsg;
                ((Registrant*)IObject::Probe(mHost->mPostDialCharacterRegistrants->Get(i)))->MessageForRegistrant((IMessage**)&notifyMsg);
                notifyMsg->SetObj(obj);
                Int32 arg1 = 0;
                msg->GetArg1(&arg1);
                notifyMsg->SetArg1(arg1);
                notifyMsg->SendToTarget();
            }
            break;
        case EVENT_ONHOLD_TONE:
            if (VDBG) Logger::D(TAG, " handleMessage (EVENT_ONHOLD_TONE)");
            mHost->mOnHoldToneRegistrants->NotifyRegistrants((AsyncResult*) IAsyncResult::Probe(obj));
            break;
    }
    return NOERROR;
}

AutoPtr<CallManager> CallManager::INSTANCE = new CallManager();
Int64 CallManager::mActiveSub = ISubscriptionManager::INVALID_SUB_ID;
CAR_INTERFACE_IMPL(CallManager, Object, ICallManager)
CallManager::CallManager()
    : mSpeedUpAudioForMtCall(FALSE)
{
    mPreciseCallStateRegistrants = new RegistrantList();
    mNewRingingConnectionRegistrants = new RegistrantList();
    mIncomingRingRegistrants = new RegistrantList();
    mDisconnectRegistrants = new RegistrantList();
    mMmiRegistrants = new RegistrantList();
    mUnknownConnectionRegistrants = new RegistrantList();
    mRingbackToneRegistrants = new RegistrantList();
    mOnHoldToneRegistrants = new RegistrantList();
    mInCallVoicePrivacyOnRegistrants = new RegistrantList();
    mInCallVoicePrivacyOffRegistrants = new RegistrantList();
    mCallWaitingRegistrants = new RegistrantList();
    mDisplayInfoRegistrants = new RegistrantList();
    mSignalInfoRegistrants = new RegistrantList();
    mCdmaOtaStatusChangeRegistrants = new RegistrantList();
    mResendIncallMuteRegistrants = new RegistrantList();
    mMmiInitiateRegistrants = new RegistrantList();
    mMmiCompleteRegistrants = new RegistrantList();
    mEcmTimerResetRegistrants = new RegistrantList();
    mSubscriptionInfoReadyRegistrants = new RegistrantList();
    mSuppServiceNotifyRegistrants = new RegistrantList();
    mSuppServiceFailedRegistrants = new RegistrantList();
    mServiceStateChangedRegistrants = new RegistrantList();
    mPostDialCharacterRegistrants = new RegistrantList();
    mActiveSubChangeRegistrants = new RegistrantList();

    CArrayList::New((IArrayList**)&mEmptyConnections);
    CArrayList::New((IArrayList**)&mPhones);
    CArrayList::New((IArrayList**)&mRingingCalls);
    CArrayList::New((IArrayList**)&mBackgroundCalls);
    CArrayList::New((IArrayList**)&mForegroundCalls);
    mDefaultPhone = NULL;
}

/**
 * get singleton instance of CallManager
 * @return CallManager
 */
AutoPtr<CallManager> CallManager::GetInstance()
{
    return INSTANCE;
}

/**
 * Get the corresponding PhoneBase obj
 *
 * @param phone a Phone object
 * @return the corresponding PhoneBase obj in Phone if Phone
 * is a PhoneProxy obj
 * or the Phone itself if Phone is not a PhoneProxy obj
 */
AutoPtr<IPhone> CallManager::GetPhoneBase(
    /* [in] */ IPhone* phone)
{
    if (IPhoneProxy::Probe(phone)) {
        AutoPtr<ICall> call;
        phone->GetForegroundCall((ICall**)&call);
        AutoPtr<IPhone> p;
        call->GetPhone((IPhone**)&p);
        return p;
    }
    return phone;
}

/**
 * Check if two phones refer to the same PhoneBase obj
 *
 * Note: PhoneBase, not PhoneProxy, is to be used inside of CallManager
 *
 * Both PhoneBase and PhoneProxy implement Phone interface, so
 * they have same phone APIs, such as dial(). The real implementation, for
 * example in GSM,  is in GSMPhone as extend from PhoneBase, so that
 * foregroundCall.GetPhone() returns GSMPhone obj. On the other hand,
 * PhoneFactory.getDefaultPhone() returns PhoneProxy obj, which has a class
 * member of GSMPhone.
 *
 * So for phone returned by PhoneFacotry, which is used by PhoneApp,
 *        phone.getForegroundCall().GetPhone() != phone
 * but
 *        isSamePhone(phone, phone.getForegroundCall().GetPhone()) == TRUE
 *
 * @param p1 is the first Phone obj
 * @param p2 is the second Phone obj
 * @return TRUE if p1 and p2 refer to the same phone
 */
Boolean CallManager::IsSamePhone(
    /* [in] */ IPhone* p1,
    /* [in] */ IPhone* p2)
{
    AutoPtr<IPhone> b1 = GetPhoneBase(p1);
    AutoPtr<IPhone> b2 = GetPhoneBase(p2);
    return (b2 == b2);
}

/**
 * Returns all the registered phone objects.
 * @return all the registered phone objects.
 */
ECode CallManager::GetAllPhones(
    /* [out] */ IList** phones)
{
    VALIDATE_NOT_NULL(phones);
    AutoPtr<ICollections> c;
    CCollections::AcquireSingleton((ICollections**)&c);
    return c->UnmodifiableList(IList::Probe(mPhones), phones);
}

/**
 * get Phone object corresponds to subId
 * @return Phone
 */
AutoPtr<IPhone> CallManager::GetPhone(
    /* [in] */ Int64 subId)
{
    AutoPtr<IPhone> p;
    AutoPtr<IIterator> ator;
    mPhones->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IPhone> phone = IPhone::Probe(obj);

        Int64 id = 0;
        phone->GetSubId(&id);
        if (id == subId && IImsPhone::Probe(phone) == NULL) {
            p = phone;
            break;
        }
    }

    return p;
}

/**
 * Get current coarse-grained voice call state.
 * If the Call Manager has an active call and call waiting occurs,
 * then the phone state is RINGING not OFFHOOK
 *
 */
ECode CallManager::GetState(
    /* [out] */ PhoneConstantsState* state)
{
    VALIDATE_NOT_NULL(state);
    PhoneConstantsState s = PhoneConstantsState_IDLE;

    AutoPtr<IIterator> ator;
    mPhones->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IPhone> phone = IPhone::Probe(obj);

        ICallState s;
        phone->GetState(&s);
        if (s == PhoneConstantsState_RINGING) {
            s = PhoneConstantsState_RINGING;
        }
        else if (s == PhoneConstantsState_OFFHOOK) {
            if (s == PhoneConstantsState_IDLE) s = PhoneConstantsState_OFFHOOK;
        }
    }

    *state = s;
    return NOERROR;
}

/**
 * Get current coarse-grained voice call state on a subId.
 * If the Call Manager has an active call and call waiting occurs,
 * then the phone state is RINGING not OFFHOOK
 *
 */
ECode CallManager::GetState(
    /* [in] */ Int64 subId,
    /* [out] */ PhoneConstantsState* state)
{
    VALIDATE_NOT_NULL(state);
    PhoneConstantsState s = PhoneConstantsState_IDLE;

    AutoPtr<IIterator> ator;
    mPhones->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IPhone> phone = IPhone::Probe(obj);

        Int64 id = 0;
        if ((phone->GetSubId(&id), id) == subId) {
            ICallState s;
            phone->GetState(&s);
            if (s == PhoneConstantsState_RINGING) {
                s = PhoneConstantsState_RINGING;
            }
            else if (s == PhoneConstantsState_OFFHOOK) {
                if (s == PhoneConstantsState_IDLE) s = PhoneConstantsState_OFFHOOK;
            }
        }
    }

    *state = s;
    return NOERROR;
}

/**
 * @return the service state of CallManager, which represents the
 * highest priority state of all the service states of phones
 *
 * The priority is defined as
 *
 * STATE_IN_SERIVCE > STATE_OUT_OF_SERIVCE > STATE_EMERGENCY > STATE_POWER_OFF
 *
 */
ECode CallManager::GetServiceState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 resultState = IServiceState::STATE_OUT_OF_SERVICE;

    AutoPtr<IIterator> ator;
    mPhones->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IPhone> phone = IPhone::Probe(obj);

        AutoPtr<IServiceState> ss;
        phone->GetServiceState((IServiceState**)&ss);
        Int32 serviceState = 0;
        ss->GetState(&serviceState);
        if (serviceState == IServiceState::STATE_IN_SERVICE) {
            // IN_SERVICE has the highest priority
            resultState = serviceState;
            break;
        }
        else if (serviceState == IServiceState::STATE_OUT_OF_SERVICE) {
            // OUT_OF_SERVICE replaces EMERGENCY_ONLY and POWER_OFF
            // Note: EMERGENCY_ONLY is not in use at this moment
            if ( resultState == IServiceState::STATE_EMERGENCY_ONLY ||
                    resultState == IServiceState::STATE_POWER_OFF) {
                resultState = serviceState;
            }
        }
        else if (serviceState == IServiceState::STATE_EMERGENCY_ONLY) {
            if (resultState == IServiceState::STATE_POWER_OFF) {
                resultState = serviceState;
            }
        }
    }

    *result = resultState;
    return NOERROR;
}

/**
 * @return the Phone service state corresponds to subId
 */
ECode CallManager::GetServiceState(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 resultState = IServiceState::STATE_OUT_OF_SERVICE;

    AutoPtr<IIterator> ator;
    mPhones->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IPhone> phone = IPhone::Probe(obj);

        Int64 id = 0;
        if ((phone->GetSubId(&id), id) == subId) {
            AutoPtr<IServiceState> ss;
            phone->GetServiceState((IServiceState**)&ss);
            Int32 serviceState = 0;
            ss->GetState(&serviceState);
            if (serviceState == IServiceState::STATE_IN_SERVICE) {
                // IN_SERVICE has the highest priority
                resultState = serviceState;
                break;
            }
            else if (serviceState == IServiceState::STATE_OUT_OF_SERVICE) {
                // OUT_OF_SERVICE replaces EMERGENCY_ONLY and POWER_OFF
                // Note: EMERGENCY_ONLY is not in use at this moment
                if ( resultState == IServiceState::STATE_EMERGENCY_ONLY ||
                        resultState == IServiceState::STATE_POWER_OFF) {
                    resultState = serviceState;
                }
            }
            else if (serviceState == IServiceState::STATE_EMERGENCY_ONLY) {
                if (resultState == IServiceState::STATE_POWER_OFF) {
                    resultState = serviceState;
                }
            }
        }
    }

    *result = resultState;
    return NOERROR;
}

/**
 * @return the phone associated with any call
 */
ECode CallManager::GetPhoneInCall(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IPhone> phone;
    AutoPtr<ICall> call;
    GetFirstActiveRingingCall((ICall**)&call);
    Boolean isIdle = FALSE;
    call->IsIdle(&isIdle);
    if (!isIdle) {
        call->GetPhone((IPhone**)&phone);
    }
    else {
        call = NULL;
        GetActiveFgCall((ICall**)&call);
        call->IsIdle(&isIdle);
        if (!isIdle) {
            call = NULL;
            GetActiveFgCall((ICall**)&call);
            call->GetPhone((IPhone**)&phone);
        }
        else {
            // If BG call is idle, we return default phone
            call = NULL;
            GetFirstActiveBgCall((ICall**)&call);
            call->GetPhone((IPhone**)&phone);
        }
    }

    *result = phone;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CallManager::GetPhoneInCall(
    /* [in] */ Int64 subId,
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IPhone> phone;
    AutoPtr<ICall> call;
    GetFirstActiveRingingCall(subId, (ICall**)&call);
    Boolean isIdle = FALSE;
    call->IsIdle(&isIdle);
    if (!isIdle) {
        call->GetPhone((IPhone**)&phone);
    }
    else {
        call = NULL;
        GetActiveFgCall(subId, (ICall**)&call);
        call->IsIdle(&isIdle);

        if (!isIdle) {
            call->GetPhone((IPhone**)&phone);
        }
        else {
            // If BG call is idle, we return default phone
            call = NULL;
            GetFirstActiveBgCall(subId, (ICall**)&call);
            call->GetPhone((IPhone**)&phone);
        }
    }

    *result = phone;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Register phone to CallManager
 * @param phone to be registered
 * @return TRUE if register successfully
 */
ECode CallManager::RegisterPhone(
    /* [in] */ IPhone* phone,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IPhone> basePhone = GetPhoneBase(phone);

    Boolean tmp = FALSE;
    if (basePhone != NULL && (mPhones->Contains(basePhone, &tmp), !tmp)) {

        if (DBG) {
            String pn;
            phone->GetPhoneName(&pn);
            Logger::D(TAG, "registerPhone(%s %s)", pn.string(), TO_CSTR(phone));
        }

        if (mPhones->IsEmpty(&tmp), tmp) {
            mDefaultPhone = basePhone;
        }
        mPhones->Add(basePhone);
        AutoPtr<ICall> call;
        basePhone->GetRingingCall((ICall**)&call);
        mRingingCalls->Add(call);
        call = NULL;
        basePhone->GetBackgroundCall((ICall**)&call);
        mBackgroundCalls->Add(call);
        call = NULL;
        basePhone->GetForegroundCall((ICall**)&call);
        mForegroundCalls->Add(call);
        RegisterForPhoneStates(basePhone);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

/**
 * unregister phone from CallManager
 * @param phone to be unregistered
 */
ECode CallManager::UnregisterPhone(
    /* [in] */ IPhone* phone)
{
    AutoPtr<IPhone> basePhone = GetPhoneBase(phone);

    Boolean tmp = FALSE;
    if (basePhone != NULL && (mPhones->Contains(basePhone, &tmp), tmp)) {
        if (DBG) {
            String pn;
            phone->GetPhoneName(&pn);
            Logger::D(TAG, "unregisterPhone(%s %s)", pn.string(), TO_CSTR(phone));
        }

        AutoPtr<IPhone> vPhone;
        basePhone->GetImsPhone((IPhone**)&vPhone);
        if (vPhone != NULL) {
           UnregisterPhone(vPhone);
        }

        mPhones->Remove(basePhone);
        AutoPtr<ICall> call;
        basePhone->GetRingingCall((ICall**)&call);
        mRingingCalls->Remove(call);
        call = NULL;
        basePhone->GetBackgroundCall((ICall**)&call);
        mBackgroundCalls->Remove(call);
        call = NULL;
        basePhone->GetForegroundCall((ICall**)&call);
        mForegroundCalls->Remove(call);
        UnregisterForPhoneStates(basePhone);
        if (basePhone == mDefaultPhone) {
            if (mPhones->IsEmpty(&tmp), tmp) {
                mDefaultPhone = NULL;
            }
            else {
                AutoPtr<IInterface> o;
                mPhones->Get(0, (IInterface**)&o);
                mDefaultPhone = IPhone::Probe(o);
            }
        }
    }
    return NOERROR;
}

/**
 * return the default phone or NULL if no phone available
 */
ECode CallManager::GetDefaultPhone(
    /* [out] */ IPhone** phone)
{
    VALIDATE_NOT_NULL(phone);
    *phone = mDefaultPhone;
    REFCOUNT_ADD(*phone);
    return NOERROR;
}

/**
 * @return the phone associated with the foreground call
 */
ECode CallManager::GetFgPhone(
    /* [out] */ IPhone** phone)
{
    VALIDATE_NOT_NULL(phone);
    AutoPtr<ICall> c;
    GetActiveFgCall((ICall**)&c);
    return c->GetPhone(phone);
}

/**
 * @return the phone associated with the foreground call
 * of a particular subId
 */
ECode CallManager::GetFgPhone(
    /* [in] */ Int64 subId,
    /* [out] */ IPhone** phone)
{
    VALIDATE_NOT_NULL(phone);
    AutoPtr<ICall> c;
    GetActiveFgCall(subId, (ICall**)&c);
    return c->GetPhone(phone);
}

/**
 * @return the phone associated with the background call
 */
ECode CallManager::GetBgPhone(
    /* [out] */ IPhone** phone)
{
    VALIDATE_NOT_NULL(phone);
    AutoPtr<ICall> c;
    GetFirstActiveBgCall((ICall**)&c);
    return c->GetPhone(phone);
}

/**
 * @return the phone associated with the background call
 * of a particular subId
 */
ECode CallManager::GetBgPhone(
    /* [in] */ Int64 subId,
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> c;
    GetFirstActiveBgCall(subId, (ICall**)&c);
    return c->GetPhone(result);
}

/**
 * @return the phone associated with the ringing call
 */
ECode CallManager::GetRingingPhone(
    /* [out] */ IPhone** phone)
{
    VALIDATE_NOT_NULL(phone);
    AutoPtr<ICall> c;
    GetFirstActiveRingingCall((ICall**)&c);
    return c->GetPhone(phone);
}

/**
 * @return the phone associated with the ringing call
 * of a particular subId
 */
ECode CallManager::GetRingingPhone(
    /* [in] */ Int64 subId,
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> c;
    GetFirstActiveRingingCall(subId, (ICall**)&c);
    return c->GetPhone(result);
}

AutoPtr<IContext> CallManager::GetContext()
{
    AutoPtr<IPhone> defaultPhone;
    GetDefaultPhone((IPhone**)&defaultPhone);
    if (defaultPhone == NULL) {
        return NULL;
    }

    AutoPtr<IContext> ctx;
    defaultPhone->GetContext((IContext**)&ctx);
    return ctx;
}

void CallManager::RegisterForPhoneStates(
    /* [in] */ IPhone* phone)
{
    // We need to keep a mapping of handler to Phone for proper unregistration.
    // TODO: Clean up this solution as it is just a work around for each Phone instance
    // using the same Handler to register with the RIL. When time permits, we should consider
    // moving the handler (or the reference ot the handler) into the Phone object.
    // See b/17414427.
    AutoPtr<CallManagerHandler> handler = mHandlerMap[phone];
    if (handler != NULL) {
        Logger::D(TAG, "This phone has already been registered.");
        return;
    }

    // New registration, create a new handler instance and register the phone.
    handler = new CallManagerHandler(this);
    mHandlerMap[phone] = handler;

    // for common events supported by all phones
    phone->RegisterForPreciseCallStateChanged(handler, EVENT_PRECISE_CALL_STATE_CHANGED, NULL);
    phone->RegisterForDisconnect(handler, EVENT_DISCONNECT, NULL);
    phone->RegisterForNewRingingConnection(handler, EVENT_NEW_RINGING_CONNECTION, NULL);
    phone->RegisterForUnknownConnection(handler, EVENT_UNKNOWN_CONNECTION, NULL);
    phone->RegisterForIncomingRing(handler, EVENT_INCOMING_RING, NULL);
    phone->RegisterForRingbackTone(handler, EVENT_RINGBACK_TONE, NULL);
    phone->RegisterForInCallVoicePrivacyOn(handler, EVENT_IN_CALL_VOICE_PRIVACY_ON, NULL);
    phone->RegisterForInCallVoicePrivacyOff(handler, EVENT_IN_CALL_VOICE_PRIVACY_OFF, NULL);
    phone->RegisterForDisplayInfo(handler, EVENT_DISPLAY_INFO, NULL);
    phone->RegisterForSignalInfo(handler, EVENT_SIGNAL_INFO, NULL);
    phone->RegisterForResendIncallMute(handler, EVENT_RESEND_INCALL_MUTE, NULL);
    phone->RegisterForMmiInitiate(handler, EVENT_MMI_INITIATE, NULL);
    phone->RegisterForMmiComplete(handler, EVENT_MMI_COMPLETE, NULL);
    phone->RegisterForSuppServiceFailed(handler, EVENT_SUPP_SERVICE_FAILED, NULL);
    phone->RegisterForServiceStateChanged(handler, EVENT_SERVICE_STATE_CHANGED, NULL);

    // for events supported only by GSM phone
    Int32 type = 0;
    phone->GetPhoneType(&type);
    if (type == IPhoneConstants::PHONE_TYPE_GSM) {
        phone->RegisterForSuppServiceNotification(handler, EVENT_SUPP_SERVICE_NOTIFY, NULL);
    }

    // for events supported only by GSM, CDMA and IMS phone
    if (type == IPhoneConstants::PHONE_TYPE_GSM ||
            type == IPhoneConstants::PHONE_TYPE_CDMA ||
            type == IPhoneConstants::PHONE_TYPE_IMS) {
        phone->SetOnPostDialCharacter(handler, EVENT_POST_DIAL_CHARACTER, NULL);
    }

    // for events supported only by CDMA phone
    if (type == IPhoneConstants::PHONE_TYPE_CDMA ){
        phone->RegisterForCdmaOtaStatusChange(handler, EVENT_CDMA_OTA_STATUS_CHANGE, NULL);
        phone->RegisterForSubscriptionInfoReady(handler, EVENT_SUBSCRIPTION_INFO_READY, NULL);
        phone->RegisterForCallWaiting(handler, EVENT_CALL_WAITING, NULL);
        phone->RegisterForEcmTimerReset(handler, EVENT_ECM_TIMER_RESET, NULL);
    }

    // for events supported only by IMS phone
    if (type == IPhoneConstants::PHONE_TYPE_IMS) {
        phone->RegisterForOnHoldTone(handler, EVENT_ONHOLD_TONE, NULL);
    }
}

void CallManager::UnregisterForPhoneStates(
    /* [in] */ IPhone* phone)
{
    // Make sure that we clean up our map of handlers to Phones.
    AutoPtr<CallManagerHandler> handler = mHandlerMap[phone];
    if (handler != NULL) {
        Logger::E(TAG, "Could not find Phone handler for unregistration");
        return;
    }
    mHandlerMap.Erase(phone);

    //  for common events supported by all phones
    phone->UnregisterForPreciseCallStateChanged(handler);
    phone->UnregisterForDisconnect(handler);
    phone->UnregisterForNewRingingConnection(handler);
    phone->UnregisterForUnknownConnection(handler);
    phone->UnregisterForIncomingRing(handler);
    phone->UnregisterForRingbackTone(handler);
    phone->UnregisterForInCallVoicePrivacyOn(handler);
    phone->UnregisterForInCallVoicePrivacyOff(handler);
    phone->UnregisterForDisplayInfo(handler);
    phone->UnregisterForSignalInfo(handler);
    phone->UnregisterForResendIncallMute(handler);
    phone->UnregisterForMmiInitiate(handler);
    phone->UnregisterForMmiComplete(handler);
    phone->UnregisterForSuppServiceFailed(handler);
    phone->UnregisterForServiceStateChanged(handler);

    // for events supported only by GSM phone
    Int32 type = 0;
    phone->GetPhoneType(&type);
    if (type == IPhoneConstants::PHONE_TYPE_GSM) {
        phone->UnregisterForSuppServiceNotification(handler);
    }

    // for events supported only by GSM, CDMA and IMS phone
    if (type == IPhoneConstants::PHONE_TYPE_GSM ||
            type == IPhoneConstants::PHONE_TYPE_CDMA ||
            type == IPhoneConstants::PHONE_TYPE_IMS) {
        phone->SetOnPostDialCharacter(NULL, EVENT_POST_DIAL_CHARACTER, NULL);
    }

    // for events supported only by CDMA phone
    if (type == IPhoneConstants::PHONE_TYPE_CDMA ){
        phone->UnregisterForCdmaOtaStatusChange(handler);
        phone->UnregisterForSubscriptionInfoReady(handler);
        phone->UnregisterForCallWaiting(handler);
        phone->UnregisterForEcmTimerReset(handler);
    }

    // for events supported only by IMS phone
    if (type == IPhoneConstants::PHONE_TYPE_IMS) {
        phone->UnregisterForOnHoldTone(handler);
    }
}

/**
 * Answers a ringing or waiting call.
 *
 * Active call, if any, go on hold.
 * If active call can't be held, i.e., a background call of the same channel exists,
 * the active call will be hang up.
 *
 * Answering occurs asynchronously, and final notification occurs via
 * {@link #registerForPreciseCallStateChanged(android.os.Handler, Int32,
 * java.lang.Object) registerForPreciseCallStateChanged()}.
 *
 * @exception CallStateException when call is not ringing or waiting
 */
ECode CallManager::AcceptCall(
    /* [in] */ ICall* ringingCall) /*throws CallStateException*/
{
    AutoPtr<IPhone> ringingPhone;
    ringingCall->GetPhone((IPhone**)&ringingPhone);

    if (VDBG) {
        Logger::D(TAG, "acceptCall(%s from %s)", TO_CSTR(ringingCall), TO_CSTR(ringingPhone));
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }

    Boolean tmp = FALSE;
    if (HasActiveFgCall(&tmp), tmp) {
        AutoPtr<ICall> call;
        GetActiveFgCall((ICall**)&call);
        AutoPtr<IPhone> activePhone;
        call->GetPhone((IPhone**)&activePhone);
        AutoPtr<ICall> bc;
        activePhone->GetBackgroundCall((ICall**)&bc);
        Boolean isIdle = FALSE;
        bc->IsIdle(&isIdle);
        Boolean hasBgCall = !isIdle;
        Boolean sameChannel = (activePhone == ringingPhone);

        if (VDBG) {
            Logger::D(TAG, "hasBgCall: %dsameChannel:%d", hasBgCall, sameChannel);
        }

        if (sameChannel && hasBgCall) {
            call->Hangup();
        }
        else if (!sameChannel && !hasBgCall) {
            activePhone->SwitchHoldingAndActive();
        }
        else if (!sameChannel && hasBgCall) {
            call->Hangup();
        }
    }

    // We only support the AUDIO_ONLY video state in this scenario.
    ringingPhone->AcceptCall(IVideoProfileVideoState::AUDIO_ONLY);

    if (VDBG) {
        Logger::D(TAG, "End acceptCall(%s)", TO_CSTR(ringingCall));
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }
    return NOERROR;
}

/**
 * Reject (ignore) a ringing call. In GSM, this means UDUB
 * (User Determined User Busy). Reject occurs asynchronously,
 * and final notification occurs via
 * {@link #registerForPreciseCallStateChanged(android.os.Handler, Int32,
 * java.lang.Object) registerForPreciseCallStateChanged()}.
 *
 * @exception CallStateException when no call is ringing or waiting
 */
ECode CallManager::RejectCall(
    /* [in] */ ICall* ringingCall) /*throws CallStateException*/
{
    if (VDBG) {
        Logger::D(TAG, "rejectCall(%s)", TO_CSTR(ringingCall));
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }

    AutoPtr<IPhone> ringingPhone;
    ringingCall->GetPhone((IPhone**)&ringingPhone);

    ringingPhone->RejectCall();

    if (VDBG) {
        Logger::D(TAG, "End rejectCall(%s)", TO_CSTR(ringingCall));
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }
    return NOERROR;
}

/**
 * Places active call on hold, and makes held call active.
 * Switch occurs asynchronously and may fail.
 *
 * There are 4 scenarios
 * 1. only active call but no held call, aka, hold
 * 2. no active call but only held call, aka, unhold
 * 3. both active and held calls from same phone, aka, swap
 * 4. active and held calls from different phones, aka, phone swap
 *
 * Final notification occurs via
 * {@link #registerForPreciseCallStateChanged(android.os.Handler, Int32,
 * java.lang.Object) registerForPreciseCallStateChanged()}.
 *
 * @exception CallStateException if active call is ringing, waiting, or
 * dialing/alerting, or heldCall can't be active.
 * In these cases, this operation may not be performed.
 */
ECode CallManager::SwitchHoldingAndActive(
    /* [in] */ ICall* heldCall) /*throws CallStateException*/
{
    AutoPtr<IPhone> activePhone;
    AutoPtr<IPhone> heldPhone;

    if (VDBG) {
        Logger::D(TAG, "SwitchHoldingAndActive(%s)", TO_CSTR(heldCall));
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }

    Boolean tmp = FALSE;
    if (HasActiveFgCall(&tmp), tmp) {
        AutoPtr<ICall> call;
        GetActiveFgCall((ICall**)&call);
        call->GetPhone((IPhone**)&activePhone);
    }

    if (heldCall != NULL) {
        heldCall->GetPhone((IPhone**)&heldPhone);
    }

    if (activePhone != NULL) {
        activePhone->SwitchHoldingAndActive();
    }

    if (heldPhone != NULL && heldPhone != activePhone) {
        heldPhone->SwitchHoldingAndActive();
    }

    if (VDBG) {
        Logger::D(TAG, "End SwitchHoldingAndActive(%s)", TO_CSTR(heldCall));
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }
    return NOERROR;
}

/**
 * Hangup foreground call and resume the specific background call
 *
 * Note: this is noop if there is no foreground call or the heldCall is NULL
 *
 * @param heldCall to become foreground
 * @throws CallStateException
 */
ECode CallManager::HangupForegroundResumeBackground(
    /* [in] */ ICall* heldCall) /*throws CallStateException*/
{
    AutoPtr<IPhone> foregroundPhone;
    AutoPtr<IPhone> backgroundPhone;

    if (VDBG) {
        Logger::D(TAG, "hangupForegroundResumeBackground(%s)", TO_CSTR(heldCall));
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }

    Boolean tmp = FALSE;
    if (HasActiveFgCall(&tmp), tmp) {
        GetFgPhone((IPhone**)&foregroundPhone);
        if (heldCall != NULL) {
            heldCall->GetPhone((IPhone**)&backgroundPhone);

            AutoPtr<ICall> call;
            GetActiveFgCall((ICall**)&call);
            if (foregroundPhone == backgroundPhone) {
                call->Hangup();
            }
            else {
            // the call to be hangup and resumed belongs to different phones
                call->Hangup();
                SwitchHoldingAndActive(heldCall);
            }
        }
    }

    if (VDBG) {
        Logger::D(TAG, "End hangupForegroundResumeBackground(%s)", TO_CSTR(heldCall));
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }
    return NOERROR;
}

/**
 * Whether or not the phone can conference in the current phone
 * state--that is, one call holding and one call active.
 * @return TRUE if the phone can conference; FALSE otherwise.
 */
ECode CallManager::CanConference(
    /* [in] */ ICall* heldCall,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IPhone> activePhone;
    AutoPtr<IPhone> heldPhone;

    Boolean tmp = FALSE;
    if (HasActiveFgCall(&tmp), tmp) {
        AutoPtr<ICall> call;
        GetActiveFgCall((ICall**)&call);
        call->GetPhone((IPhone**)&activePhone);
    }

    if (heldCall != NULL) {
        heldCall->GetPhone((IPhone**)&heldPhone);
    }

    ClassID id1, id2;
    ECode e1 = IObject::Probe(heldPhone)->GetClassID(&id1);
    ECode e2 = IObject::Probe(activePhone)->GetClassID(&id2);
    if (FAILED(e1) || FAILED(e2)) {
        assert(0 && "Need car class or implement the getClass method.");
    }

    *result = id1 == id2;
    return NOERROR;
}

/**
 * Whether or not the phone can conference in the current phone
 * state--that is, one call holding and one call active.
 * This method consider the phone object which is specific
 * to the provided subId.
 * @return TRUE if the phone can conference; FALSE otherwise.
 */
ECode CallManager::CanConference(
    /* [in] */ ICall* heldCall,
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IPhone> activePhone;
    AutoPtr<IPhone> heldPhone;

    Boolean tmp = FALSE;
    if (HasActiveFgCall(subId, &tmp), tmp) {
        AutoPtr<ICall> call;
        GetActiveFgCall(subId, (ICall**)&call);
        call->GetPhone((IPhone**)&activePhone);
    }

    if (heldCall != NULL) {
        heldCall->GetPhone((IPhone**)&heldPhone);
    }

    ClassID id1, id2;
    ECode e1 = IObject::Probe(heldPhone)->GetClassID(&id1);
    ECode e2 = IObject::Probe(activePhone)->GetClassID(&id2);
    if (FAILED(e1) || FAILED(e2)) {
        assert(0 && "Need car class or implement the getClass method.");
    }

    *result = id1 == id2;
    return NOERROR;
}

/**
 * Conferences holding and active. Conference occurs asynchronously
 * and may fail. Final notification occurs via
 * {@link #registerForPreciseCallStateChanged(android.os.Handler, Int32,
 * java.lang.Object) registerForPreciseCallStateChanged()}.
 *
 * @exception CallStateException if canConference() would return FALSE.
 * In these cases, this operation may not be performed.
 */
ECode CallManager::Conference(
    /* [in] */ ICall* heldCall) /*throws CallStateException*/
{
    AutoPtr<IPhone> phone;
    heldCall->GetPhone((IPhone**)&phone);
    Int64 subId = 0;
    phone->GetSubId(&subId);

    if (VDBG) {
        Logger::D(TAG, "conference(%s)", TO_CSTR(heldCall));
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }

    AutoPtr<IPhone> fgPhone;
    GetFgPhone(subId, (IPhone**)&fgPhone);
    if (fgPhone != NULL) {
        Boolean tmp = FALSE;
        if (ISipPhone::Probe(fgPhone)) {
            ISipPhone::Probe(fgPhone)->Conference(heldCall);
        }
        else if (CanConference(heldCall, &tmp), tmp) {
            fgPhone->Conference();
        }
        else {
            // throw(new CallStateException("Can't conference foreground and selected background call"));
            Logger::D(TAG, "Can't conference foreground and selected background call");
            return E_TELEPHONEY_CALL_STATE_EXCEPTION;
        }
    }
    else {
        Logger::D(TAG, "conference: fgPhone=NULL");
    }

    if (VDBG) {
        Logger::D(TAG, "End conference(%s)", TO_CSTR(heldCall));
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }
    return NOERROR;
}

/**
 * Initiate a new voice connection. This happens asynchronously, so you
 * cannot assume the audio path is connected (or a call index has been
 * assigned) until PhoneStateChanged notification has occurred.
 *
 * @exception CallStateException if a new outgoing call is not currently
 * possible because no more call slots exist or a call exists that is
 * dialing, alerting, ringing, or waiting.  Other errors are
 * handled asynchronously.
 */
ECode CallManager::Dial(
    /* [in] */ IPhone* phone,
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** retValue) /*throws CallStateException*/
{
    VALIDATE_NOT_NULL(retValue);
    *retValue = NULL;
    AutoPtr<IPhone> basePhone = GetPhoneBase(phone);
    Int64 subId = 0;
    phone->GetSubId(&subId);
    AutoPtr<IConnection> result;

    if (VDBG) {
        Logger::D(TAG, " dial(%s, %s) subId = %lld", TO_CSTR(basePhone), dialString.string(), subId);
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }

    if (!CanDial(phone)) {
        /*
         * CanDial function only checks whether the phone can make a new call.
         * InCall MMI commmands are basically supplementary services
         * within a call eg: call hold, call deflection, explicit call transfer etc.
         */
        String newDialString;
        PhoneNumberUtils::StripSeparators(dialString, &newDialString);
        Boolean tmp = FALSE;
        if (basePhone->HandleInCallMmiCommands(newDialString, &tmp), tmp) {
            return NOERROR;
        }
        else {
            // throw new CallStateException("cannot dial in current state");
            Logger::D(TAG, "cannot dial in current state");
            return E_TELEPHONEY_CALL_STATE_EXCEPTION;
        }
    }

    Boolean tmp = FALSE;
    if (HasActiveFgCall(subId, &tmp), tmp) {
        AutoPtr<ICall> call;
        GetActiveFgCall(subId, (ICall**)&call);
        AutoPtr<IPhone> activePhone;
        call->GetPhone((IPhone**)&activePhone);
        AutoPtr<ICall> bc;
        activePhone->GetBackgroundCall((ICall**)&bc);
        bc->IsIdle(&tmp);
        Boolean hasBgCall = !tmp;

        if (DBG) {
            Logger::D(TAG, "hasBgCall: %d sameChannel:%d", hasBgCall, (activePhone == basePhone));
        }

        // Manipulation between IMS phone and its owner
        // will be treated in GSM/CDMA phone.
        AutoPtr<IPhone> vPhone;
        basePhone->GetImsPhone((IPhone**)&vPhone);
        if (activePhone != basePhone
                && (vPhone == NULL || vPhone != activePhone)) {
            if (hasBgCall) {
                Logger::D(TAG, "Hangup");
                call->Hangup();
            }
            else {
                Logger::D(TAG, "Switch");
                activePhone->SwitchHoldingAndActive();
            }
        }
    }

    basePhone->Dial(dialString, videoState, (IConnection**)&result);

    if (VDBG) {
        Logger::D(TAG, "End dial(%s, %s)", TO_CSTR(basePhone), dialString.string());
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }

    *retValue = result;
    REFCOUNT_ADD(*retValue);
    return NOERROR;
}

/**
 * Initiate a new voice connection. This happens asynchronously, so you
 * cannot assume the audio path is connected (or a call index has been
 * assigned) until PhoneStateChanged notification has occurred.
 *
 * @exception CallStateException if a new outgoing call is not currently
 * possible because no more call slots exist or a call exists that is
 * dialing, alerting, ringing, or waiting.  Other errors are
 * handled asynchronously.
 */
ECode CallManager::Dial(
    /* [in] */ IPhone* phone,
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** retValue) /*throws CallStateException*/
{
    VALIDATE_NOT_NULL(retValue);
    return phone->Dial(dialString, uusInfo, videoState, retValue);
}

/**
 * clear disconnect connection for each phone
 */
ECode CallManager::ClearDisconnected()
{
    AutoPtr<IIterator> ator;
    mPhones->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IPhone> phone = IPhone::Probe(obj);
        phone->ClearDisconnected();
    }
    return NOERROR;
}

/**
 * clear disconnect connection for a phone specific
 * to the provided subId
 */
ECode CallManager::ClearDisconnected(
    /* [in] */ Int64 subId)
{
    AutoPtr<IIterator> ator;
    mPhones->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IPhone> phone = IPhone::Probe(obj);

        Int64 id = 0;
        if ((phone->GetSubId(&id), id) == subId) {
            phone->ClearDisconnected();
        }
    }
    return NOERROR;
}

/**
 * Phone can make a call only if ALL of the following are TRUE:
 *        - Phone is not powered off
 *        - There's no incoming or waiting call
 *        - The foreground call is ACTIVE or IDLE or DISCONNECTED.
 *          (We mainly need to make sure it *isn't* DIALING or ALERTING.)
 * @param phone
 * @return TRUE if the phone can make a new call
 */
Boolean CallManager::CanDial(
    /* [in] */ IPhone* phone)
{
    AutoPtr<IServiceState> ss;
    phone->GetServiceState((IServiceState**)&ss);
    Int32 serviceState = 0;
    ss->GetState(&serviceState);
    Int64 subId = 0;
    phone->GetSubId(&subId);
    Boolean hasRingingCall = FALSE;
    HasActiveRingingCall(&hasRingingCall);
    ICallState fgCallState;
    GetActiveFgCallState(subId, &fgCallState);

    Boolean result = (serviceState != IServiceState::STATE_POWER_OFF
            && !hasRingingCall
            && ((fgCallState == ICallState_ACTIVE)
                || (fgCallState == ICallState_IDLE)
                || (fgCallState == ICallState_DISCONNECTED)
                /*As per 3GPP TS 51.010-1 section 31.13.1.4
                call should be alowed when the foreground
                call is in ALERTING state*/
                || (fgCallState == ICallState_ALERTING)));

    if (result == FALSE) {
        Logger::D(TAG, "CanDial serviceState=%d hasRingingCall=%d fgCallState=%d"
            , serviceState, hasRingingCall, fgCallState);
    }
    return result;
}

/**
 * Whether or not the phone can do explicit call transfer in the current
 * phone state--that is, one call holding and one call active.
 * @return TRUE if the phone can do explicit call transfer; FALSE otherwise.
 */
ECode CallManager::CanTransfer(
    /* [in] */ ICall* heldCall,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IPhone> activePhone;
    AutoPtr<IPhone> heldPhone;

    Boolean tmp = FALSE;
    if (HasActiveFgCall(&tmp), tmp) {
        AutoPtr<ICall> call;
        GetActiveFgCall((ICall**)&call);
        call->GetPhone((IPhone**)&activePhone);
    }

    if (heldCall != NULL) {
        heldCall->GetPhone((IPhone**)&heldPhone);
    }

    *result = (heldPhone == activePhone && (activePhone->CanTransfer(&tmp), tmp));
    return NOERROR;
}

/**
 * Whether or not the phone specific to subId can do explicit call transfer
 * in the current phone state--that is, one call holding and one call active.
 * @return TRUE if the phone can do explicit call transfer; FALSE otherwise.
 */
ECode CallManager::CanTransfer(
    /* [in] */ ICall* heldCall,
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IPhone> activePhone;
    AutoPtr<IPhone> heldPhone;

    Boolean tmp = FALSE;
    if (HasActiveFgCall(subId, &tmp), tmp) {
        AutoPtr<ICall> call;
        GetActiveFgCall(subId, (ICall**)&call);
        call->GetPhone((IPhone**)&activePhone);
    }

    if (heldCall != NULL) {
        heldCall->GetPhone((IPhone**)&heldPhone);
    }

    *result = (heldPhone == activePhone && (activePhone->CanTransfer(&tmp), tmp));
    return NOERROR;
}

/**
 * Connects the held call and active call
 * Disconnects the subscriber from both calls
 *
 * Explicit Call Transfer occurs asynchronously
 * and may fail. Final notification occurs via
 * {@link #registerForPreciseCallStateChanged(android.os.Handler, Int32,
 * java.lang.Object) registerForPreciseCallStateChanged()}.
 *
 * @exception CallStateException if canTransfer() would return FALSE.
 * In these cases, this operation may not be performed.
 */
ECode CallManager::ExplicitCallTransfer(
    /* [in] */ ICall* heldCall) /*throws CallStateException*/
{
    if (VDBG) {
        Logger::D(TAG, " explicitCallTransfer(%s", TO_CSTR(heldCall));
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }

    Boolean tmp = FALSE;
    if (CanTransfer(heldCall, &tmp), tmp) {
        AutoPtr<IPhone> phone;
        heldCall->GetPhone((IPhone**)&phone);
        phone->ExplicitCallTransfer();
    }

    if (VDBG) {
        Logger::D(TAG, "End explicitCallTransfer(%s", TO_CSTR(heldCall));
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }
    return NOERROR;
}

/**
 * Returns a list of MMI codes that are pending for a phone. (They have initiated
 * but have not yet completed).
 * Presently there is only ever one.
 *
 * Use <code>registerForMmiInitiate</code>
 * and <code>registerForMmiComplete</code> for change notification.
 * @return NULL if phone doesn't have or support mmi code
 */
ECode CallManager::GetPendingMmiCodes(
    /* [in] */ IPhone* phone,
    /* [out] */ IList** codes)
{
    VALIDATE_NOT_NULL(codes);
    Logger::E(TAG, "getPendingMmiCodes not implemented");
    *codes = NULL;
    return NOERROR;
}

/**
 * Sends user response to a USSD REQUEST message.  An MmiCode instance
 * representing this response is sent to handlers registered with
 * registerForMmiInitiate.
 *
 * @param ussdMessge    Message to send in the response.
 * @return FALSE if phone doesn't support ussd service
 */
ECode CallManager::SendUssdResponse(
    /* [in] */ IPhone* phone,
    /* [in] */ const String& ussdMessge,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::E(TAG, "sendUssdResponse not implemented");
    *result = FALSE;
    return NOERROR;
}

/**
 * Mutes or unmutes the microphone for the active call. The microphone
 * is automatically unmuted if a call is answered, dialed, or resumed
 * from a holding state.
 *
 * @param muted TRUE to mute the microphone,
 * FALSE to activate the microphone.
 */
ECode CallManager::SetMute(
    /* [in] */ Boolean muted)
{
    if (VDBG) {
        Logger::D(TAG, " setMute(%d)", muted);
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }

    Boolean tmp = FALSE;
    if (HasActiveFgCall(&tmp), tmp) {
        AutoPtr<ICall> call;
        GetActiveFgCall((ICall**)&call);
        AutoPtr<IPhone> phone;
        call->GetPhone((IPhone**)&phone);
        phone->SetMute(muted);
    }

    if (VDBG) {
        Logger::D(TAG, "End setMute(%d)", muted);
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }
    return NOERROR;
}

/**
 * Gets current mute status. Use
 * {@link #registerForPreciseCallStateChanged(android.os.Handler, Int32,
 * java.lang.Object) registerForPreciseCallStateChanged()}
 * as a change notifcation, although presently phone state changed is not
 * fired when setMute() is called.
 *
 * @return TRUE is muting, FALSE is unmuting
 */
ECode CallManager::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    if (HasActiveFgCall(&tmp), tmp) {
        AutoPtr<ICall> call;
        GetActiveFgCall((ICall**)&call);
        AutoPtr<IPhone> phone;
        call->GetPhone((IPhone**)&phone);
        return phone->GetMute(result);
    }
    else if (HasActiveBgCall(&tmp), tmp) {
        AutoPtr<ICall> call;
        GetFirstActiveBgCall((ICall**)&call);
        AutoPtr<IPhone> phone;
        call->GetPhone((IPhone**)&phone);
        return phone->GetMute(result);
    }
    *result = FALSE;
    return NOERROR;
}

/**
 * Enables or disables echo suppression.
 */
ECode CallManager::SetEchoSuppressionEnabled()
{
    if (VDBG) {
        Logger::D(TAG, " setEchoSuppression()");
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }

    Boolean tmp = FALSE;
    if (HasActiveFgCall(&tmp), tmp) {
        AutoPtr<ICall> call;
        GetActiveFgCall((ICall**)&call);
        AutoPtr<IPhone> phone;
        call->GetPhone((IPhone**)&phone);
        phone->SetEchoSuppressionEnabled();
    }

    if (VDBG) {
        Logger::D(TAG, "End setEchoSuppression()");
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }
    return NOERROR;
}

/**
 * Play a DTMF tone on the active call.
 *
 * @param c should be one of 0-9, '*' or '#'. Other values will be
 * silently ignored.
 * @return FALSE if no active call or the active call doesn't support
 *         dtmf tone
 */
ECode CallManager::SendDtmf(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* retValue)
{
    VALIDATE_NOT_NULL(retValue);
    Boolean result = FALSE;

    if (VDBG) {
        Logger::D(TAG, " sendDtmf(%c)", c);
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }

    Boolean tmp = FALSE;
    if (HasActiveFgCall(&tmp), tmp) {
        AutoPtr<ICall> call;
        GetActiveFgCall((ICall**)&call);
        AutoPtr<IPhone> phone;
        call->GetPhone((IPhone**)&phone);
        phone->SendDtmf(c);
        result = TRUE;
    }

    if (VDBG) {
        Logger::D(TAG, "End sendDtmf(%c)", c);
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }
    *retValue = result;
    return NOERROR;
}

/**
 * Start to paly a DTMF tone on the active call.
 * or there is a playing DTMF tone.
 * @param c should be one of 0-9, '*' or '#'. Other values will be
 * silently ignored.
 *
 * @return FALSE if no active call or the active call doesn't support
 *         dtmf tone
 */
ECode CallManager::StartDtmf(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* retValue)
{
    VALIDATE_NOT_NULL(retValue);
    Boolean result = FALSE;

    if (VDBG) {
        Logger::D(TAG, " startDtmf(%c)", c);
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }

    Boolean tmp = FALSE;
    if (HasActiveFgCall(&tmp), tmp) {
        AutoPtr<ICall> call;
        GetActiveFgCall((ICall**)&call);
        AutoPtr<IPhone> phone;
        call->GetPhone((IPhone**)&phone);
        phone->StartDtmf(c);
        result = TRUE;
    }

    if (VDBG) {
        Logger::D(TAG, "End startDtmf(%c)", c);
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }

    *retValue = result;
    return NOERROR;
}

/**
 * Stop the playing DTMF tone. Ignored if there is no playing DTMF
 * tone or no active call.
 */
ECode CallManager::StopDtmf()
{
    if (VDBG) {
        Logger::D(TAG, " stopDtmf()" );
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }

    Boolean tmp = FALSE;
    if (HasActiveFgCall(&tmp), tmp) {
        AutoPtr<IPhone> phone;
        GetFgPhone((IPhone**)&phone);
        phone->StopDtmf();
    }

    if (VDBG) {
        Logger::D(TAG, "End stopDtmf()");
        String s;
        ToString(&s);
        Logger::D(TAG, s);
    }
    return NOERROR;
}

/**
 * send burst DTMF tone, it can send the string as single character or multiple character
 * ignore if there is no active call or not valid digits string.
 * Valid digit means only includes characters ISO-LATIN characters 0-9, *, #
 * The difference between sendDtmf and sendBurstDtmf is sendDtmf only sends one character,
 * this api can send single character and multiple character, also, this api has response
 * back to caller.
 *
 * @param dtmfString is string representing the dialing digit(s) in the active call
 * @param on the DTMF ON length in milliseconds, or 0 for default
 * @param off the DTMF OFF length in milliseconds, or 0 for default
 * @param onComplete is the callback message when the action is processed by BP
 *
 */
ECode CallManager::SendBurstDtmf(
    /* [in] */ const String& dtmfString,
    /* [in] */ Int32 on,
    /* [in] */ Int32 off,
    /* [in] */ IMessage* onComplete,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    if (HasActiveFgCall(&tmp), tmp) {
        AutoPtr<ICall> call;
        GetActiveFgCall((ICall**)&call);
        AutoPtr<IPhone> phone;
        call->GetPhone((IPhone**)&phone);
        phone->SendBurstDtmf(dtmfString, on, off, onComplete);
        *result =  TRUE;
        return NOERROR;
    }
    *result =  FALSE;
    return NOERROR;
}

/**
 * Notifies when a voice connection has disconnected, either due to local
 * or remote hangup or error.
 *
 *  Messages received from this will have the following members:<p>
 *  <ul><li>Message.obj will be an AsyncResult</li>
 *  <li>AsyncResult.userObj = obj</li>
 *  <li>AsyncResult.result = a Connection object that is
 *  no longer connected.</li></ul>
 */
ECode CallManager::RegisterForDisconnect(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mDisconnectRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregisters for voice disconnection notification.
 * Extraneous calls are tolerated silently
 */
ECode CallManager::UnregisterForDisconnect(
    /* [in] */ IHandler* h)
{
    mDisconnectRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Register for getting notifications for change in the Call State {@link Call.State}
 * This is called PreciseCallState because the call state is more precise than what
 * can be obtained using the {@link PhoneStateListener}
 *
 * Resulting events will have an AsyncResult in <code>Message.obj</code>.
 * AsyncResult.userData will be set to the obj argument here.
 * The <em>h</em> parameter is held only by a weak reference.
 */
ECode CallManager::RegisterForPreciseCallStateChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mPreciseCallStateRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregisters for voice call state change notifications.
 * Extraneous calls are tolerated silently.
 */
ECode CallManager::UnregisterForPreciseCallStateChanged(
    /* [in] */ IHandler* h)
{
    mPreciseCallStateRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Notifies when a previously untracked non-ringing/waiting connection has appeared.
 * This is likely due to some other entity (eg, SIM card application) initiating a call.
 */
ECode CallManager::RegisterForUnknownConnection(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mUnknownConnectionRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregisters for unknown connection notifications.
 */
ECode CallManager::UnregisterForUnknownConnection(
    /* [in] */ IHandler* h)
{
    mUnknownConnectionRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Notifies when a new ringing or waiting connection has appeared.<p>
 *
 *  Messages received from this:
 *  Message.obj will be an AsyncResult
 *  AsyncResult.userObj = obj
 *  AsyncResult.result = a Connection. <p>
 *  Please check Connection.isRinging() to make sure the Connection
 *  has not dropped since this message was posted.
 *  If Connection.isRinging() is TRUE, then
 *   Connection.getCall() == Phone.getRingingCall()
 */
ECode CallManager::RegisterForNewRingingConnection(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mNewRingingConnectionRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregisters for new ringing connection notification.
 * Extraneous calls are tolerated silently
 */

ECode CallManager::UnregisterForNewRingingConnection(
    /* [in] */ IHandler* h)
{
    mNewRingingConnectionRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Notifies when an incoming call rings.<p>
 *
 *  Messages received from this:
 *  Message.obj will be an AsyncResult
 *  AsyncResult.userObj = obj
 *  AsyncResult.result = a Connection. <p>
 */
ECode CallManager::RegisterForIncomingRing(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mIncomingRingRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregisters for ring notification.
 * Extraneous calls are tolerated silently
 */

ECode CallManager::UnregisterForIncomingRing(
    /* [in] */ IHandler* h)
{
    mIncomingRingRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Notifies when out-band ringback tone is needed.<p>
 *
 *  Messages received from this:
 *  Message.obj will be an AsyncResult
 *  AsyncResult.userObj = obj
 *  AsyncResult.result = Boolean, TRUE to start play ringback tone
 *                       and FALSE to stop. <p>
 */
ECode CallManager::RegisterForRingbackTone(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mRingbackToneRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregisters for ringback tone notification.
 */

ECode CallManager::UnregisterForRingbackTone(
    /* [in] */ IHandler* h)
{
    mRingbackToneRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Notifies when out-band on-hold tone is needed.<p>
 *
 *  Messages received from this:
 *  Message.obj will be an AsyncResult
 *  AsyncResult.userObj = obj
 *  AsyncResult.result = Boolean, TRUE to start play on-hold tone
 *                       and FALSE to stop. <p>
 */
ECode CallManager::RegisterForOnHoldTone(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mOnHoldToneRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregisters for on-hold tone notification.
 */

ECode CallManager::UnregisterForOnHoldTone(
    /* [in] */ IHandler* h)
{
    mOnHoldToneRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Registers the handler to reset the uplink mute state to get
 * uplink audio.
 */
ECode CallManager::RegisterForResendIncallMute(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mResendIncallMuteRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregisters for resend incall mute notifications.
 */
ECode CallManager::UnregisterForResendIncallMute(
    /* [in] */ IHandler* h)
{
    mResendIncallMuteRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Register for notifications of initiation of a new MMI code request.
 * MMI codes for GSM are discussed in 3GPP TS 22.030.<p>
 *
 * Example: If Phone.dial is called with "*#31#", then the app will
 * be notified here.<p>
 *
 * The returned <code>Message.obj</code> will contain an AsyncResult.
 *
 * <code>obj.result</code> will be an "MmiCode" object.
 */
ECode CallManager::RegisterForMmiInitiate(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mMmiInitiateRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregisters for new MMI initiate notification.
 * Extraneous calls are tolerated silently
 */
ECode CallManager::UnregisterForMmiInitiate(
    /* [in] */ IHandler* h)
{
    mMmiInitiateRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Register for notifications that an MMI request has completed
 * its network activity and is in its final state. This may mean a state
 * of COMPLETE, FAILED, or CANCELLED.
 *
 * <code>Message.obj</code> will contain an AsyncResult.
 * <code>obj.result</code> will be an "MmiCode" object
 */
ECode CallManager::RegisterForMmiComplete(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mMmiCompleteRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregisters for MMI complete notification.
 * Extraneous calls are tolerated silently
 */
ECode CallManager::UnregisterForMmiComplete(
    /* [in] */ IHandler* h)
{
    mMmiCompleteRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Registration point for Ecm timer reset
 * @param h handler to notify
 * @param what user-defined message code
 * @param obj placed in Message.obj
 */
ECode CallManager::RegisterForEcmTimerReset(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mEcmTimerResetRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregister for notification for Ecm timer reset
 * @param h Handler to be removed from the registrant list.
 */
ECode CallManager::UnregisterForEcmTimerReset(
    /* [in] */ IHandler* h)
{
    mEcmTimerResetRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Register for ServiceState changed.
 * Message.obj will contain an AsyncResult.
 * AsyncResult.result will be a ServiceState instance
 */
ECode CallManager::RegisterForServiceStateChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mServiceStateChangedRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregisters for ServiceStateChange notification.
 * Extraneous calls are tolerated silently
 */
ECode CallManager::UnregisterForServiceStateChanged(
    /* [in] */ IHandler* h)
{
    mServiceStateChangedRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Register for supplementary service notifications.
 * Message.obj will contain an AsyncResult.
 *
 * @param h Handler that receives the notification message.
 * @param what User-defined message code.
 * @param obj User object.
 */
ECode CallManager::RegisterForSuppServiceNotification(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mSuppServiceNotifyRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregister for supplementary service notifications.
 * Extraneous calls are tolerated silently
 *
 * @param h Handler to be removed from the registrant list.
 */
ECode CallManager::UnregisterForSuppServiceNotification(
    /* [in] */ IHandler* h)
{
    mSuppServiceNotifyRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Register for notifications when a supplementary service attempt fails.
 * Message.obj will contain an AsyncResult.
 *
 * @param h Handler that receives the notification message.
 * @param what User-defined message code.
 * @param obj User object.
 */
ECode CallManager::RegisterForSuppServiceFailed(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mSuppServiceFailedRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregister for notifications when a supplementary service attempt fails.
 * Extraneous calls are tolerated silently
 *
 * @param h Handler to be removed from the registrant list.
 */
ECode CallManager::UnregisterForSuppServiceFailed(
    /* [in] */ IHandler* h)
{
    mSuppServiceFailedRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Register for notifications when a sInCall VoicePrivacy is enabled
 *
 * @param h Handler that receives the notification message.
 * @param what User-defined message code.
 * @param obj User object.
 */
ECode CallManager::RegisterForInCallVoicePrivacyOn(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mInCallVoicePrivacyOnRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregister for notifications when a sInCall VoicePrivacy is enabled
 *
 * @param h Handler to be removed from the registrant list.
 */
ECode CallManager::UnregisterForInCallVoicePrivacyOn(
    /* [in] */ IHandler* h)
{
    mInCallVoicePrivacyOnRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Register for notifications when a sInCall VoicePrivacy is disabled
 *
 * @param h Handler that receives the notification message.
 * @param what User-defined message code.
 * @param obj User object.
 */
ECode CallManager::RegisterForInCallVoicePrivacyOff(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mInCallVoicePrivacyOffRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregister for notifications when a sInCall VoicePrivacy is disabled
 *
 * @param h Handler to be removed from the registrant list.
 */
ECode CallManager::UnregisterForInCallVoicePrivacyOff(
    /* [in] */ IHandler* h)
{
    mInCallVoicePrivacyOffRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Register for notifications when CDMA call waiting comes
 *
 * @param h Handler that receives the notification message.
 * @param what User-defined message code.
 * @param obj User object.
 */
ECode CallManager::RegisterForCallWaiting(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mCallWaitingRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregister for notifications when CDMA Call waiting comes
 * @param h Handler to be removed from the registrant list.
 */
ECode CallManager::UnregisterForCallWaiting(
    /* [in] */ IHandler* h)
{
    mCallWaitingRegistrants->Remove(h);
    return NOERROR;
}


/**
 * Register for signal information notifications from the network.
 * Message.obj will contain an AsyncResult.
 * AsyncResult.result will be a SuppServiceNotification instance.
 *
 * @param h Handler that receives the notification message.
 * @param what User-defined message code.
 * @param obj User object.
 */
ECode CallManager::RegisterForSignalInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mSignalInfoRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregisters for signal information notifications.
 * Extraneous calls are tolerated silently
 *
 * @param h Handler to be removed from the registrant list.
 */
ECode CallManager::UnregisterForSignalInfo(
    /* [in] */ IHandler* h)
{
    mSignalInfoRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Register for display information notifications from the network.
 * Message.obj will contain an AsyncResult.
 * AsyncResult.result will be a SuppServiceNotification instance.
 *
 * @param h Handler that receives the notification message.
 * @param what User-defined message code.
 * @param obj User object.
 */
ECode CallManager::RegisterForDisplayInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mDisplayInfoRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregisters for display information notifications.
 * Extraneous calls are tolerated silently
 *
 * @param h Handler to be removed from the registrant list.
 */
ECode CallManager::UnregisterForDisplayInfo(
    /* [in] */ IHandler* h)
{
    mDisplayInfoRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Register for notifications when CDMA OTA Provision status change
 *
 * @param h Handler that receives the notification message.
 * @param what User-defined message code.
 * @param obj User object.
 */
ECode CallManager::RegisterForCdmaOtaStatusChange(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mCdmaOtaStatusChangeRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregister for notifications when CDMA OTA Provision status change
 * @param h Handler to be removed from the registrant list.
 */
ECode CallManager::UnregisterForCdmaOtaStatusChange(
    /* [in] */ IHandler* h)
{
    mCdmaOtaStatusChangeRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Registration point for subcription info ready
 * @param h handler to notify
 * @param what what code of message when delivered
 * @param obj placed in Message.obj
 */
ECode CallManager::RegisterForSubscriptionInfoReady(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mSubscriptionInfoReadyRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

/**
 * Unregister for notifications for subscription info
 * @param h Handler to be removed from the registrant list.
 */
ECode CallManager::UnregisterForSubscriptionInfoReady(
    /* [in] */ IHandler* h)
{
    mSubscriptionInfoReadyRegistrants->Remove(h);
    return NOERROR;
}

ECode CallManager::RegisterForSubscriptionChange(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mActiveSubChangeRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

ECode CallManager::UnregisterForSubscriptionChange(
    /* [in] */ IHandler* h)
{
    mActiveSubChangeRegistrants->Remove(h);
    return NOERROR;
}

ECode CallManager::SetActiveSubscription(
    /* [in] */ Int64 subscription)
{
    Logger::D(TAG, "setActiveSubscription existing:%lld new = %lld", mActiveSub, subscription);
    mActiveSub = subscription;
    AutoPtr<IInteger64> i64;
    CInteger64::New(mActiveSub, (IInteger64**)&i64);
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, i64, NULL);
    mActiveSubChangeRegistrants->NotifyRegistrants(ar);
    return NOERROR;
}

/**
 * Sets an event to be fired when the telephony system processes
 * a post-dial character on an outgoing call.<p>
 *
 * Messages of type <code>what</code> will be sent to <code>h</code>.
 * The <code>obj</code> field of these Message's will be instances of
 * <code>AsyncResult</code>. <code>Message.obj.result</code> will be
 * a Connection object.<p>
 *
 * Message.arg1 will be the post dial character being processed,
 * or 0 ('\0') if end of string.<p>
 *
 * If Connection.getPostDialState() == WAIT,
 * the application must call
 * {@link com.android.internal.telephony.Connection#proceedAfterWaitChar()
 * Connection.proceedAfterWaitChar()} or
 * {@link com.android.internal.telephony.Connection#cancelPostDial()
 * Connection.cancelPostDial()}
 * for the telephony system to continue playing the post-dial
 * DTMF sequence.<p>
 *
 * If Connection.getPostDialState() == WILD,
 * the application must call
 * {@link com.android.internal.telephony.Connection#proceedAfterWildChar
 * Connection.proceedAfterWildChar()}
 * or
 * {@link com.android.internal.telephony.Connection#cancelPostDial()
 * Connection.cancelPostDial()}
 * for the telephony system to continue playing the
 * post-dial DTMF sequence.<p>
 *
 */
ECode CallManager::RegisterForPostDialCharacter(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mPostDialCharacterRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

ECode CallManager::UnregisterForPostDialCharacter(
    /* [in] */ IHandler* h)
{
    mPostDialCharacterRegistrants->Remove(h);
    return NOERROR;
}

/* APIs to access foregroudCalls, backgroudCalls, and ringingCalls
 * 1. APIs to access list of calls
 * 2. APIs to check if any active call, which has connection other than
 * disconnected ones, pleaser refer to Call.isIdle()
 * 3. APIs to return first active call
 * 4. APIs to return the connections of first active call
 * 5. APIs to return other property of first active call
 */

/**
 * @return list of all ringing calls
 */
ECode CallManager::GetRingingCalls(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICollections> c;
    CCollections::AcquireSingleton((ICollections**)&c);
    return c->UnmodifiableList(IList::Probe(mRingingCalls), result);
}

/**
 * @return list of all foreground calls
 */
ECode CallManager::GetForegroundCalls(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICollections> c;
    CCollections::AcquireSingleton((ICollections**)&c);
    return c->UnmodifiableList(IList::Probe(mForegroundCalls), result);
}

/**
 * @return list of all background calls
 */
ECode CallManager::GetBackgroundCalls(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICollections> c;
    CCollections::AcquireSingleton((ICollections**)&c);
    return c->UnmodifiableList(IList::Probe(mBackgroundCalls), result);
}

/**
 * Return TRUE if there is at least one active foreground call
 */
ECode CallManager::HasActiveFgCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (GetFirstActiveCall(mForegroundCalls) != NULL);
    return NOERROR;
}

/**
 * Return TRUE if there is at least one active foreground call
 * on a particular subId or an active sip call
 */
ECode CallManager::HasActiveFgCall(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (GetFirstActiveCall(mForegroundCalls, subId) != NULL);
    return NOERROR;
}

/**
 * Return TRUE if there is at least one active background call
 */
ECode CallManager::HasActiveBgCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // TODO since HasActiveBgCall may get called often
    // better to cache it to improve performance
    *result = (GetFirstActiveCall(mBackgroundCalls) != NULL);
    return NOERROR;
}

/**
 * Return TRUE if there is at least one active background call
 * on a particular subId or an active sip call
 */
ECode CallManager::HasActiveBgCall(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // TODO since HasActiveBgCall may get called often
    // better to cache it to improve performance
    *result = (GetFirstActiveCall(mBackgroundCalls, subId) != NULL);
    return NOERROR;
}

/**
 * Return TRUE if there is at least one active ringing call
 *
 */
ECode CallManager::HasActiveRingingCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (GetFirstActiveCall(mRingingCalls) != NULL);
    return NOERROR;
}

/**
 * Return TRUE if there is at least one active ringing call
 */
ECode CallManager::HasActiveRingingCall(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (GetFirstActiveCall(mRingingCalls, subId) != NULL);
    return NOERROR;
}

/**
 * return the active foreground call from foreground calls
 *
 * Active call means the call is NOT in ICallState_IDLE
 *
 * 1. If there is active foreground call, return it
 * 2. If there is no active foreground call, return the
 *    foreground call associated with default phone, which state is IDLE.
 * 3. If there is no phone registered at all, return NULL.
 *
 */
ECode CallManager::GetActiveFgCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> call = GetFirstNonIdleCall(IList::Probe(mForegroundCalls));
    if (call == NULL) {
        if (mDefaultPhone != NULL) {
            mDefaultPhone->GetForegroundCall((ICall**)&call);
        }
    }
    *result = call;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CallManager::GetActiveFgCall(
    /* [in] */ Int64 subId,
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> call = GetFirstNonIdleCall(IList::Probe(mForegroundCalls), subId);
    if (call == NULL) {
        AutoPtr<IPhone> phone = GetPhone(subId);
        if (phone != NULL) {
            phone->GetForegroundCall((ICall**)&call);
        }
    }
    *result = call;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

// Returns the first call that is not in IDLE state. If both active calls
// and disconnecting/disconnected calls exist, return the first active call.
AutoPtr<ICall> CallManager::GetFirstNonIdleCall(
    /* [in] */ IList* calls)
{
    AutoPtr<ICall> result;
    AutoPtr<IIterator> ator;
    calls->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE, tmp = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<ICall> call = ICall::Probe(obj);

        if (call->IsIdle(&tmp), !tmp) {
            return call;
        }
        else {
            ICallState cs;
            call->GetState(&cs);
            if (cs != ICallState_IDLE) {
                if (result == NULL) result = call;
            }
        }
    }

    return result;
}

// Returns the first call that is not in IDLE state. If both active calls
// and disconnecting/disconnected calls exist, return the first active call.
AutoPtr<ICall> CallManager::GetFirstNonIdleCall(
    /* [in] */ IList* calls,
    /* [in] */ Int64 subId)
{
    AutoPtr<ICall> result;
    AutoPtr<IIterator> ator;
    calls->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<ICall> call = ICall::Probe(obj);

        AutoPtr<IPhone> phone;
        call->GetPhone((IPhone**)&phone);
        Int64 id = 0;
        if (((phone->GetSubId(&id), id) == subId) || (ISipPhone::Probe(phone))) {
            Boolean tmp = FALSE;
            ICallState cs;
            if (call->IsIdle(&tmp), !tmp) {
                return call;
            }
            else if ((call->GetState(&cs), cs) != ICallState_IDLE) {
                if (result == NULL) result = call;
            }
        }
    }

    return result;
}

/**
 * return one active background call from background calls
 *
 * Active call means the call is NOT idle defined by Call.isIdle()
 *
 * 1. If there is only one active background call, return it
 * 2. If there is more than one active background call, return the first one
 * 3. If there is no active background call, return the background call
 *    associated with default phone, which state is IDLE.
 * 4. If there is no background call at all, return NULL.
 *
 * Complete background calls list can be get by getBackgroundCalls()
 */
ECode CallManager::GetFirstActiveBgCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> call = GetFirstNonIdleCall(IList::Probe(mBackgroundCalls));
    if (call == NULL) {
        if (mDefaultPhone != NULL) {
            mDefaultPhone->GetBackgroundCall((ICall**)&call);
        }
    }
    *result = call;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * return one active background call from background calls of the
 * requested subId.
 *
 * Active call means the call is NOT idle defined by Call.isIdle()
 *
 * 1. If there is only one active background call on given sub or
 *    on SIP Phone, return it
 * 2. If there is more than one active background call, return the background call
 *    associated with the active sub.
 * 3. If there is no background call at all, return NULL.
 *
 * Complete background calls list can be get by getBackgroundCalls()
 */
ECode CallManager::GetFirstActiveBgCall(
    /* [in] */ Int64 subId,
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IPhone> phone = GetPhone(subId);
    if (HasMoreThanOneHoldingCall(subId)) {
        return phone->GetBackgroundCall(result);
    }

    AutoPtr<ICall> call = GetFirstNonIdleCall(IList::Probe(mBackgroundCalls), subId);
    if (call == NULL) {
        if (phone != NULL) {
            phone->GetBackgroundCall((ICall**)&call);
        }
    }
    *result = call;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * return one active ringing call from ringing calls
 *
 * Active call means the call is NOT idle defined by Call.isIdle()
 *
 * 1. If there is only one active ringing call, return it
 * 2. If there is more than one active ringing call, return the first one
 * 3. If there is no active ringing call, return the ringing call
 *    associated with default phone, which state is IDLE.
 * 4. If there is no ringing call at all, return NULL.
 *
 * Complete ringing calls list can be get by getRingingCalls()
 */
ECode CallManager::GetFirstActiveRingingCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> call = GetFirstNonIdleCall(IList::Probe(mRingingCalls));
    if (call == NULL) {
        if (mDefaultPhone != NULL) {
            mDefaultPhone->GetRingingCall((ICall**)&call);
        }
    }
    *result = call;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CallManager::GetFirstActiveRingingCall(
    /* [in] */ Int64 subId,
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IPhone> phone = GetPhone(subId);
    AutoPtr<ICall> call = GetFirstNonIdleCall(IList::Probe(mRingingCalls), subId);
    if (call == NULL) {
        if (phone != NULL) {
            phone->GetRingingCall((ICall**)&call);
        }
    }
    *result = call;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * @return the state of active foreground call
 * return IDLE if there is no active foreground call
 */
ECode CallManager::GetActiveFgCallState(
    /* [out] */ ICallState* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> fgCall;
    GetActiveFgCall((ICall**)&fgCall);

    if (fgCall != NULL) {
        return fgCall->GetState(result);
    }

    *result = ICallState_IDLE;
    return NOERROR;
}

ECode CallManager::GetActiveFgCallState(
    /* [in] */ Int64 subId,
    /* [out] */ ICallState* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> fgCall;
    GetActiveFgCall(subId, (ICall**)&fgCall);

    if (fgCall != NULL) {
        return fgCall->GetState(result);
    }

    *result = ICallState_IDLE;
    return NOERROR;
}

/**
 * @return the connections of active foreground call
 * return empty list if there is no active foreground call
 */
ECode CallManager::GetFgCallConnections(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> fgCall;
    GetActiveFgCall((ICall**)&fgCall);
    if (fgCall != NULL) {
        return fgCall->GetConnections(result);
    }
    *result = IList::Probe(mEmptyConnections);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * @return the connections of active foreground call
 * return empty list if there is no active foreground call
 */
ECode CallManager::GetFgCallConnections(
    /* [in] */ Int64 subId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> fgCall;
    GetActiveFgCall(subId, (ICall**)&fgCall);
    if (fgCall != NULL) {
        return fgCall->GetConnections(result);
    }
    *result = IList::Probe(mEmptyConnections);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * @return the connections of active background call
 * return empty list if there is no active background call
 */
ECode CallManager::GetBgCallConnections(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> bgCall;
    GetFirstActiveBgCall((ICall**)&bgCall);
    if (bgCall != NULL) {
        return bgCall->GetConnections(result);
    }
    *result = IList::Probe(mEmptyConnections);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * @return the connections of active background call
 * return empty list if there is no active background call
 */
ECode CallManager::GetBgCallConnections(
    /* [in] */ Int64 subId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> bgCall;
    GetFirstActiveBgCall(subId, (ICall**)&bgCall);
    if (bgCall != NULL) {
        return bgCall->GetConnections(result);
    }
    *result = IList::Probe(mEmptyConnections);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * @return the latest connection of active foreground call
 * return NULL if there is no active foreground call
 */
ECode CallManager::GetFgCallLatestConnection(
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> fgCall;
    GetActiveFgCall((ICall**)&fgCall);
    if (fgCall != NULL) {
        return fgCall->GetLatestConnection(result);
    }
    *result = NULL;
    return NOERROR;
}

/**
 * @return the latest connection of active foreground call
 * return NULL if there is no active foreground call
 */
ECode CallManager::GetFgCallLatestConnection(
    /* [in] */ Int64 subId,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> fgCall;
    GetActiveFgCall(subId, (ICall**)&fgCall);
    if (fgCall != NULL) {
        return fgCall->GetLatestConnection(result);
    }
    *result = NULL;
    return NOERROR;
}

/**
 * @return TRUE if there is at least one Foreground call in disconnected state
 */
ECode CallManager::HasDisconnectedFgCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (GetFirstCallOfState(mForegroundCalls, ICallState_DISCONNECTED) != NULL);
    return NOERROR;
}

/**
 * @return TRUE if there is at least one Foreground call in disconnected state
 */
ECode CallManager::HasDisconnectedFgCall(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (GetFirstCallOfState(mForegroundCalls, ICallState_DISCONNECTED,
            subId) != NULL);
    return NOERROR;
}

/**
 * @return TRUE if there is at least one background call in disconnected state
 */
ECode CallManager::HasDisconnectedBgCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (GetFirstCallOfState(mBackgroundCalls, ICallState_DISCONNECTED) != NULL);
    return NOERROR;
}

/**
 * @return TRUE if there is at least one background call in disconnected state
 */
ECode CallManager::HasDisconnectedBgCall(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (GetFirstCallOfState(mBackgroundCalls, ICallState_DISCONNECTED,
            subId) != NULL);
    return NOERROR;
}

/**
 * @return the first active call from a call list
 */
AutoPtr<ICall> CallManager::GetFirstActiveCall(
    /* [in] */ IArrayList* calls)
{
    AutoPtr<IIterator> ator;
    calls->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE, tmp = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<ICall> call = ICall::Probe(obj);

        if (call->IsIdle(&tmp), !tmp) {
            return call;
        }
    }

    return NULL;
}

/**
 * @return the first active call from a call list
 */
AutoPtr<ICall> CallManager::GetFirstActiveCall(
    /* [in] */ IArrayList* calls,
    /* [in] */ Int64 subId)
{
    AutoPtr<IIterator> ator;
    calls->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE, tmp = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<ICall> call = ICall::Probe(obj);

        AutoPtr<IPhone> phone;
        call->GetPhone((IPhone**)&phone);
        Int64 id = 0;
        if ((call->IsIdle(&tmp), !tmp) && (((phone->GetSubId(&id), id) == subId) ||
                (ISipPhone::Probe(phone)))) {
            return call;
        }
    }

    return NULL;
}

/**
 * @return the first call in a the Call.state from a call list
 */
AutoPtr<ICall> CallManager::GetFirstCallOfState(
    /* [in] */ IArrayList* calls,
    /* [in] */ ICallState state)
{
    AutoPtr<IIterator> ator;
    calls->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<ICall> call = ICall::Probe(obj);
        ICallState cs;
        if ((call->GetState(&cs), cs) == state) {
            return call;
        }
    }

    return NULL;
}

/**
 * @return the first call in a the Call.state from a call list
 */
AutoPtr<ICall> CallManager::GetFirstCallOfState(
    /* [in] */ IArrayList* calls,
    /* [in] */ ICallState state,
    /* [in] */ Int64 subId)
{
    AutoPtr<IIterator> ator;
    calls->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<ICall> call = ICall::Probe(obj);

        ICallState cs;
        call->GetState(&cs);
        AutoPtr<IPhone> phone;
        call->GetPhone((IPhone**)&phone);
        Int64 id = 0;
        if ((cs == state) ||
            (((phone->GetSubId(&id), id) == subId) ||
            (ISipPhone::Probe(phone)))) {
            return call;
        }
    }

    return NULL;
}

Boolean CallManager::HasMoreThanOneRingingCall()
{
    Int32 count = 0;

    AutoPtr<IIterator> ator;
    mRingingCalls->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<ICall> call = ICall::Probe(obj);

        ICallState cs;
        call->GetState(&cs);
        Boolean isRinging = cs == ICallState_INCOMING || cs == ICallState_WAITING;
        if (isRinging) {
            if (++count > 1) return TRUE;
        }
    }

    return FALSE;
}

/**
 * @return TRUE if more than one active ringing call exists on
 * the active subId.
 * This checks for the active calls on provided
 * subId and also active calls on SIP Phone.
 *
 */
Boolean CallManager::HasMoreThanOneRingingCall(
    /* [in] */ Int64 subId)
{
    Int32 count = 0;
    AutoPtr<IIterator> ator;
    mRingingCalls->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<ICall> call = ICall::Probe(obj);

        ICallState cs;
        call->GetState(&cs);
        Boolean isRinging = cs == ICallState_INCOMING || cs == ICallState_WAITING;
        AutoPtr<IPhone> phone;
        call->GetPhone((IPhone**)&phone);
        Int64 id = 0;
        if (isRinging &&
            (((phone->GetSubId(&id), id) == subId) ||
            (ISipPhone::Probe(phone)))) {
            if (++count > 1) return TRUE;
        }
    }

    return FALSE;
}

/**
 * @return TRUE if more than one active background call exists on
 * the provided subId.
 * This checks for the background calls on provided
 * subId and also background calls on SIP Phone.
 *
 */
Boolean CallManager::HasMoreThanOneHoldingCall(
    /* [in] */ Int64 subId)
{
    Int32 count = 0;
    AutoPtr<IIterator> ator;
    mBackgroundCalls->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<ICall> call = ICall::Probe(obj);

        ICallState cs;
        call->GetState(&cs);
        AutoPtr<IPhone> phone;
        call->GetPhone((IPhone**)&phone);
        Int64 id = 0;
        if ((cs == ICallState_HOLDING) &&
            (((phone->GetSubId(&id), id) == subId) ||
            (ISipPhone::Probe(phone)))) {
            if (++count > 1) return TRUE;
        }
    }

    return FALSE;
}

ECode CallManager::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> call;
    StringBuilder b;
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    Int32 count = 0;
    tm->GetPhoneCount(&count);
    for (Int32 i = 0; i < count; i++) {
        b.Append("CallManager {");
        PhoneConstantsState ps;
        GetState(i, &ps);
        b.Append(String("\nstate = ") + StringUtils::ToString(ps));
        call = NULL;
        GetActiveFgCall(i, (ICall**)&call);

        ICallState cs;
        GetActiveFgCallState(i, &cs);
        b.Append(String("\n- Foreground: ") + StringUtils::ToString(cs));

        AutoPtr<IPhone> p;
        call->GetPhone((IPhone**)&p);
        b.Append(String(" from ") + TO_CSTR(p));
        b.Append("\n  Conn: ");

        AutoPtr<IList> l;
        GetFgCallConnections(i, (IList**)&l);
        b.Append(TO_CSTR(l));
        call = NULL;
        GetFirstActiveBgCall(i, (ICall**)&call);
        call->GetState(&cs);
        b.Append(String("\n- Background: ") + StringUtils::ToString(cs));
        p = NULL;
        call->GetPhone((IPhone**)&p);
        b.Append(String(" from ") + TO_CSTR(p));
        b.Append("\n  Conn: ");

        l = NULL;
        GetBgCallConnections(i, (IList**)&l);
        b.Append(TO_CSTR(l));
        call = NULL;
        GetFirstActiveRingingCall(i, (ICall**)&call);
        call->GetState(&cs);
        b.Append(String("\n- Ringing: ") + StringUtils::ToString(cs));
        p = NULL;
        call->GetPhone((IPhone**)&p);
        b.Append(String(" from ") + TO_CSTR(p));
    }

    AutoPtr<IList> phones;
    GetAllPhones((IList**)&phones);
    AutoPtr<IIterator> ator;
    phones->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IPhone> phone = IPhone::Probe(obj);

        if (phone != NULL) {
            String pn;
            phone->GetPhoneName(&pn);
            PhoneConstantsState ps;
            phone->GetState(&ps);
            b.Append(String("\nPhone: ") + TO_CSTR(phone) + String(", name = ") + pn
                    + String(", state = ") + StringUtils::ToString(ps));
            call = NULL;
            phone->GetForegroundCall((ICall**)&call);
            b.Append("\n- Foreground: ");
            b.Append(TO_CSTR(call));
            call = NULL;
            phone->GetBackgroundCall((ICall**)&call);
            b.Append(" Background: ");\
            b.Append(TO_CSTR(call));
            call = NULL;
            phone->GetRingingCall((ICall**)&call);
            b.Append(" Ringing: ");
            b.Append(TO_CSTR(call));
        }
    }

    b.Append(String("\n}"));
    return b.ToString(result);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
