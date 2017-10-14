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

#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/internal/telephony/gsm/CGsmCallTracker.h"
#include "elastos/droid/internal/telephony/gsm/CGSMPhone.h"
#include "elastos/droid/internal/telephony/gsm/CGsmMmiCode.h"
#include "elastos/droid/internal/telephony/gsm/CGsmServiceStateTracker.h"
#include "elastos/droid/internal/telephony/gsm/CSimPhoneBookInterfaceManager.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcTracker.h"
#include "elastos/droid/internal/telephony/CallForwardInfo.h"
#include "elastos/droid/internal/telephony/CPhoneFactory.h"
#include "elastos/droid/internal/telephony/CPhoneSubInfo.h"
#include "elastos/droid/internal/telephony/PhoneProxy.h"
#include "elastos/droid/internal/telephony/Subscription.h"
#include "elastos/droid/internal/telephony/SubscriptionController.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/preference/PreferenceManager.h"
#include "elastos/droid/provider/CTelephonyCarriers.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/os/CRegistrantList.h"
#include "elastos/droid/R.h"
#include "elastos/droid/telephony/CServiceState.h"
#include "elastos/droid/telephony/CSubscriptionManager.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Internal::Telephony::CallForwardInfo;
using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Droid::Internal::Telephony::CPhoneSubInfo;
using Elastos::Droid::Internal::Telephony::DataConnection::CDcTracker;
using Elastos::Droid::Internal::Telephony::DataConnection::IDcTracker;
using Elastos::Droid::Internal::Telephony::ImsPhone::IImsPhoneCall;
using Elastos::Droid::Internal::Telephony::ICommandException;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Internal::Telephony::PhoneProxy;
using Elastos::Droid::Internal::Telephony::Subscription;
using Elastos::Droid::Internal::Telephony::SubscriptionController;
// TODO:
// using Elastos::Droid::Internal::Telephony::Test::ISimulatedRadioControl;
using Elastos::Droid::Internal::Telephony::Uicc::IIccVmNotSupportedException;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::CRegistrantList;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Preference::PreferenceManager;
using Elastos::Droid::Provider::CTelephonyCarriers;
using Elastos::Droid::Provider::ITelephonyCarriers;
using Elastos::Droid::R;
using Elastos::Droid::Telecom::IVideoProfileVideoState;
using Elastos::Droid::Telephony::CServiceState;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::IThrowable;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

const String CGSMPhone::TAG("GSMPhone");
const Boolean CGSMPhone::LOCAL_DEBUG;
const Boolean CGSMPhone::VDBG;
const Int32 CGSMPhone::CHECK_CALLFORWARDING_STATUS;

CAR_INTERFACE_IMPL(CGSMPhone, PhoneBase, IGSMPhone)

CAR_OBJECT_IMPL(CGSMPhone)

CGSMPhone::Cfu::Cfu(
    /* [in] */ const String& cfNumber,
    /* [in] */ IMessage* onComplete)
    : mSetCfNumber(cfNumber)
    , mOnComplete(onComplete)
{
}

CGSMPhone::CGSMPhone()
{
    AutoPtr<IRegistrantList> list;
    CRegistrantList::New((IRegistrantList**)&list);
    mSsnRegistrants = (RegistrantList*)(list.Get());
    CArrayList::New((IArrayList**)&mPendingMMIs);
}

CGSMPhone::~CGSMPhone()
{
    if(LOCAL_DEBUG) Logger::D(TAG, "GSMPhone finalized");
}

ECode CGSMPhone::constructor()
{
    return NOERROR;
}

ECode CGSMPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier)
{
    return constructor(context,ci, notifier, FALSE);
}

ECode CGSMPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ Boolean unitTestMode)
{
    PhoneBase::constructor(String("GSM"), notifier, context, ci, unitTestMode);

// TODO: Need ISimulatedRadioControl
    // if (ISimulatedRadioControl::Probe(ci) != NULL) {
    //     mSimulatedRadioControl = ISimulatedRadioControl::Probe(ci);
    // }

    mCi->SetPhoneType(IPhoneConstants::PHONE_TYPE_GSM);
    CGsmCallTracker::New(this, (IGsmCallTracker**)&mCT);

    CGsmServiceStateTracker::New(this, (IGsmServiceStateTracker**)&mSST);
    AutoPtr<IDcTracker> dcTracker;
    CDcTracker::New(this, (IDcTracker**)&dcTracker);
    mDcTracker = IDcTrackerBase::Probe(dcTracker);

    if (!unitTestMode) {
        CSimPhoneBookInterfaceManager::New(this, (ISimPhoneBookInterfaceManager**)&mSimPhoneBookIntManager);
        CPhoneSubInfo::New(this, (IPhoneSubInfo**)&mSubInfo);
    }

    mCi->RegisterForAvailable(this, EVENT_RADIO_AVAILABLE, NULL);
    mCi->RegisterForOffOrNotAvailable(this, EVENT_RADIO_OFF_OR_NOT_AVAILABLE, NULL);
    mCi->RegisterForOn(this, EVENT_RADIO_ON, NULL);
    mCi->SetOnUSSD(this, EVENT_USSD, NULL);
    mCi->SetOnSuppServiceNotification(this, EVENT_SSN, NULL);
    IServiceStateTracker::Probe(mSST)->RegisterForNetworkAttached(this, EVENT_REGISTERED_TO_NETWORK, NULL);
    mCi->SetOnSs(this, EVENT_SS, NULL);
    SetProperties();
    return NOERROR;
}

ECode CGSMPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ Int32 phoneId)
{
    return constructor(context, ci, notifier, FALSE, phoneId);
}

ECode CGSMPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ Boolean unitTestMode,
    /* [in] */ Int32 phoneId)
{
    PhoneBase::constructor(String("GSM"), notifier, context, ci, unitTestMode, phoneId);

// TODO: Need ISimulatedRadioControl
    // if (ISimulatedRadioControl::Probe(ci) != NULL) {
    //     mSimulatedRadioControl = ISimulatedRadioControl::Probe(ci);
    // }

    mCi->SetPhoneType(IPhoneConstants::PHONE_TYPE_GSM);
    CGsmCallTracker::New(this, (IGsmCallTracker**)&mCT);

    CGsmServiceStateTracker::New(this, (IGsmServiceStateTracker**)&mSST);
    AutoPtr<IDcTracker> dcTracker;
    CDcTracker::New(this, (IDcTracker**)&dcTracker);
    mDcTracker = IDcTrackerBase::Probe(dcTracker);

    if (!unitTestMode) {
        CSimPhoneBookInterfaceManager::New(this, (ISimPhoneBookInterfaceManager**)&mSimPhoneBookIntManager);
        CPhoneSubInfo::New(this, (IPhoneSubInfo**)&mSubInfo);
    }

    mCi->RegisterForAvailable(this, EVENT_RADIO_AVAILABLE, NULL);
    mCi->RegisterForOffOrNotAvailable(this, EVENT_RADIO_OFF_OR_NOT_AVAILABLE, NULL);
    mCi->RegisterForOn(this, EVENT_RADIO_ON, NULL);
    mCi->SetOnUSSD(this, EVENT_USSD, NULL);
    mCi->SetOnSuppServiceNotification(this, EVENT_SSN, NULL);
    //((CGsmServiceStateTracker*)mSST.Get())->RegisterForNetworkAttached(this, EVENT_REGISTERED_TO_NETWORK, NULL);
    (IServiceStateTracker::Probe(mSST))->RegisterForNetworkAttached(this, EVENT_REGISTERED_TO_NETWORK, NULL);
    mCi->SetOnSs(this, EVENT_SS, NULL);
    SetProperties();

    Logger::I("CGSMPhone", "GSMPhone: constructor: sub = %d", mPhoneId);

    SetProperties();
    return NOERROR;
}

ECode CGSMPhone::Dispose()
{
// TODO:
    // AutoLock lock(PhoneProxy::mLockForRadioTechnologyChange);
    PhoneBase::Dispose();

    //Unregister from all former registered events
// TODO: Need CommandsInterface
    // mCi->UknregisterForAvailable(this); //EVENT_RADIO_AVAILABLE
    UnregisterForSimRecordEvents();
// TODO: Need CommandsInterface
    // mCi->UknregisterForOffOrNotAvailable(this); //EVENT_RADIO_OFF_OR_NOT_AVAILABLE
    // mCi->UknregisterForOn(this); //EVENT_RADIO_ON
    // mSST->UnregisterForNetworkAttached(this); //EVENT_REGISTERED_TO_NETWORK
    // mCi->UknSetOnUSSD(this);
    // mCi->UknSetOnSuppServiceNotification(this);
    // mCi->UknSetOnSs(this);

    mPendingMMIs->Clear();

    //Force all referenced classes to unregister their former registered events
    mCT->Dispose();
    mDcTracker->Dispose();
    IServiceStateTracker::Probe(mSST)->Dispose();
    IIccPhoneBookInterfaceManager::Probe(mSimPhoneBookIntManager)->Dispose();
    mSubInfo->Dispose();

    return NOERROR;
}

ECode CGSMPhone::RemoveReferences()
{
    Logger::D(TAG, "removeReferences");
// TODO: Need ISimulatedRadioControl
    // mSimulatedRadioControl = NULL;
    mSimPhoneBookIntManager = NULL;
    mSubInfo = NULL;
    mCT = NULL;
    mSST = NULL;

    PhoneBase::RemoveReferences();
    return NOERROR;
}

ECode CGSMPhone::GetServiceState(
    /* [out] */ IServiceState** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 state;
    ((CGsmServiceStateTracker*)mSST.Get())->mSS->GetState(&state);
    if (mSST == NULL || state != IServiceState::STATE_IN_SERVICE) {
        if (mImsPhone != NULL) {
            AutoPtr<IServiceState> ss;
            IPhone::Probe(mImsPhone)->GetServiceState((IServiceState**)&ss);
            Int32 state;
            ss->GetState(&state);
            if (state == IServiceState::STATE_IN_SERVICE) {
                return IPhone::Probe(mImsPhone)->GetServiceState(result);
            }
        }
    }

    if (mSST != NULL) {
        *result = ((CGsmServiceStateTracker*)mSST.Get())->mSS;
        REFCOUNT_ADD(*result)
    }
    else {
        // avoid potential NPE in EmergencyCallHelper during Phone switch
        CServiceState::New(result);
    }
    return NOERROR;
}

ECode CGSMPhone::GetCellLocation(
    /* [out] */ ICellLocation** result)
{
    VALIDATE_NOT_NULL(result)
    return mSST->GetCellLocation(result);
}

ECode CGSMPhone::GetState(
    /* [out] */ PhoneConstantsState* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ((CGsmCallTracker*)mCT.Get())->mState;
    return NOERROR;
}

ECode CGSMPhone::GetPhoneType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IPhoneConstants::PHONE_TYPE_GSM;
    return NOERROR;
}

ECode CGSMPhone::GetServiceStateTracker(
    /* [out] */ IServiceStateTracker** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IServiceStateTracker::Probe(mSST);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGSMPhone::GetCallTracker(
    /* [out] */ ICallTracker** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ICallTracker::Probe(mCT);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGSMPhone::GetCallForwardingIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean cf = FALSE;
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
    Boolean b;
    if (r != NULL && (r->IsCallForwardStatusStored(&b), b)) {
        r->GetVoiceCallForwardingFlag(&cf);
    }
    else {
        cf = GetCallForwardingPreference();
    }
    *result = cf;
    return NOERROR;
}

ECode CGSMPhone::GetPendingMmiCodes(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IList::Probe(mPendingMMIs);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGSMPhone::GetDataConnectionState(
    /* [in] */ const String& apnType,
    /* [out] */ PhoneConstantsDataState* result)
{
    VALIDATE_NOT_NULL(result)
    PhoneConstantsDataState ret = PhoneConstantsDataState_DISCONNECTED;

    Int32 state;
    Boolean b, b1, b2;
    if (mSST == NULL) {
        // Radio Technology Change is ongoning, dispose() and removeReferences() have
        // already been called

        ret = PhoneConstantsDataState_DISCONNECTED;
    }
    else if (!apnType.Equals(IPhoneConstants::APN_TYPE_EMERGENCY) &&
            (IServiceStateTracker::Probe(mSST)->GetCurrentDataConnectionState(&state), state) != IServiceState::STATE_IN_SERVICE
            && mOosIsDisconnect) {
        // If we're out of service, open TCP sockets may still work
        // but no data will flow

        // Emergency APN is available even in Out Of Service
        // Pass the actual State of EPDN

        ret = PhoneConstantsDataState_DISCONNECTED;
        Logger::D(TAG, "getDataConnectionState: Data is Out of Service. ret = %d", ret);
    }
    else if ((mDcTracker->IsApnTypeEnabled(apnType, &b1), b1) == FALSE ||
            (mDcTracker->IsApnTypeActive(apnType, &b2), b2) == FALSE) {
        //TODO: isApnTypeActive() is just checking whether ApnContext holds
        //      Dataconnection or not. Checking each ApnState below should
        //      provide the same state. Calling isApnTypeActive() can be removed.
        ret = PhoneConstantsDataState_DISCONNECTED;
    }
    else { /* mSST.gprsState == IServiceState::STATE_IN_SERVICE */
        DctConstantsState dcs;
        mDcTracker->GetState(apnType, &dcs);
        switch (dcs) {
            case Elastos::Droid::Internal::Telephony::DctConstantsState_RETRYING:
            case Elastos::Droid::Internal::Telephony::DctConstantsState_FAILED:
            case Elastos::Droid::Internal::Telephony::DctConstantsState_IDLE:
                ret = PhoneConstantsDataState_DISCONNECTED;
            break;

            case Elastos::Droid::Internal::Telephony::DctConstantsState_CONNECTED:
            case Elastos::Droid::Internal::Telephony::DctConstantsState_DISCONNECTING:
                if ( ((CGsmCallTracker*)mCT.Get())->mState != PhoneConstantsState_IDLE
                        && (IServiceStateTracker::Probe(mSST)->IsConcurrentVoiceAndDataAllowed(&b), b)) {
                    ret = PhoneConstantsDataState_SUSPENDED;
                }
                else {
                    ret = PhoneConstantsDataState_CONNECTED;
                }
            break;

            case Elastos::Droid::Internal::Telephony::DctConstantsState_CONNECTING:
            case Elastos::Droid::Internal::Telephony::DctConstantsState_SCANNING:
                ret = PhoneConstantsDataState_CONNECTING;
            break;
        }
    }

    *result = ret;
    return NOERROR;
}

ECode CGSMPhone::GetDataActivityState(
    /* [out] */ IPhoneDataActivityState* result)
{
    VALIDATE_NOT_NULL(result)
    IPhoneDataActivityState ret = IPhoneDataActivityState_NONE;

    Int32 state;
    IServiceStateTracker::Probe(mSST)->GetCurrentDataConnectionState(&state);
    if (state == IServiceState::STATE_IN_SERVICE) {
        DctConstantsActivity dca;
        mDcTracker->GetActivity(&dca);
        switch (dca) {
            case Elastos::Droid::Internal::Telephony::DctConstantsActivity_DATAIN:
                ret = IPhoneDataActivityState_DATAIN;
            break;

            case Elastos::Droid::Internal::Telephony::DctConstantsActivity_DATAOUT:
                ret = IPhoneDataActivityState_DATAOUT;
            break;

            case Elastos::Droid::Internal::Telephony::DctConstantsActivity_DATAINANDOUT:
                ret = IPhoneDataActivityState_DATAINANDOUT;
            break;

            case Elastos::Droid::Internal::Telephony::DctConstantsActivity_DORMANT:
                ret = IPhoneDataActivityState_DORMANT;
            break;

            default:
                ret = IPhoneDataActivityState_NONE;
            break;
        }
    }

    *result = ret;
    return NOERROR;
}

ECode CGSMPhone::NotifyPhoneStateChanged()
{
    return mNotifier->NotifyPhoneState(this);
}

ECode CGSMPhone::NotifyPreciseCallStateChanged()
{
    /* we'd love it if this was package-scoped*/
    PhoneBase::NotifyPreciseCallStateChangedP();
    return NOERROR;
}

ECode CGSMPhone::NotifyNewRingingConnection(
    /* [in] */ IConnection* c)
{
    return PhoneBase::NotifyNewRingingConnectionP(c);
}

ECode CGSMPhone::NotifyDisconnect(
    /* [in] */ IConnection* cn)
{
    mDisconnectRegistrants->NotifyResult(cn);

    Int32 val1, val2;
    cn->GetDisconnectCause(&val1);
    cn->GetPreciseDisconnectCause(&val2);
    mNotifier->NotifyDisconnectCause(val1, val2);
    return NOERROR;
}

ECode CGSMPhone::NotifyUnknownConnection(
    /* [in] */ IConnection* cn)
{
    mUnknownConnectionRegistrants->NotifyResult(cn);
    return NOERROR;
}

ECode CGSMPhone::NotifySuppServiceFailed(
    /* [in] */ IPhoneSuppService code)
{
// TODO: Need Interface IPhoneSuppService
    // return mSuppServiceFailedRegistrants->NotifyResult(code);
    return NOERROR;
}

ECode CGSMPhone::NotifyServiceStateChanged(
    /* [in] */ IServiceState* ss)
{
    PhoneBase::NotifyServiceStateChangedP(ss);
    return NOERROR;
}

ECode CGSMPhone::NotifyLocationChanged()
{
    return mNotifier->NotifyCellLocation(this);
}

ECode CGSMPhone::NotifyCallForwardingIndicator()
{
    return mNotifier->NotifyCallForwardingChanged(this);
}

ECode CGSMPhone::SetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& value)
{
    Int64 id;
    GetSubId(&id);
    return CTelephonyManager::SetTelephonyProperty(property, id, value);
}

ECode CGSMPhone::RegisterForSuppServiceNotification(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mSsnRegistrants->AddUnique(h, what, obj);
    Int32 size = mSsnRegistrants->GetSize();
    if (size == 1) mCi->SetSuppServiceNotifications(TRUE, NULL);
    return NOERROR;
}

ECode CGSMPhone::UnregisterForSuppServiceNotification(
    /* [in] */ IHandler* h)
{
    mSsnRegistrants->Remove(h);
    Int32 size = mSsnRegistrants->GetSize();
    if (size == 0) mCi->SetSuppServiceNotifications(FALSE, NULL);
    return NOERROR;
}

ECode CGSMPhone::RegisterForSimRecordsLoaded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mSimRecordsLoadedRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

ECode CGSMPhone::UnregisterForSimRecordsLoaded(
    /* [in] */ IHandler* h)
{
    mSimRecordsLoadedRegistrants->Remove(h);
    return NOERROR;
}

ECode CGSMPhone::AcceptCall(
    /* [in] */ Int32 videoState)
{
    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    if (imsPhone != NULL) {
        AutoPtr<ICall> call;
        IPhone::Probe(imsPhone)->GetRingingCall((ICall**)&call);
        Boolean ringing;
        if (call->IsRinging(&ringing), ringing) {
            IPhone::Probe(imsPhone)->AcceptCall(videoState);
            return NOERROR;
        }
    }

    ((CGsmCallTracker*)mCT.Get())->AcceptCall();
    return NOERROR;
}

ECode CGSMPhone::DeflectCall(
    /* [in] */ const String& number)
{
    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    if (imsPhone == NULL) {
        // throw new CallStateException("Deflect call NOT supported in GSM!");
        return E_CALL_STATE_EXCEPTION;
    }

    AutoPtr<ICall> call;
    IPhone::Probe(imsPhone)->GetRingingCall((ICall**)&call);
    Boolean ringing;
    if (call->IsRinging(&ringing), ringing) {
        IPhone::Probe(imsPhone)->DeflectCall(number);
    }
    return NOERROR;
}

ECode CGSMPhone::RejectCall()
{
    return ((CGsmCallTracker*)mCT.Get())->RejectCall();
}

ECode CGSMPhone::SwitchHoldingAndActive()
{
    return ((CGsmCallTracker*)mCT.Get())->SwitchWaitingOrHoldingAndActive();
}

ECode CGSMPhone::CanConference(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean canImsConference = FALSE;
    if (mImsPhone != NULL) {
        IPhone::Probe(mImsPhone)->CanConference(&canImsConference);
    }
    Boolean b;
    *result = (((CGsmCallTracker*)mCT.Get())->CanConference(&b), b) || canImsConference;
    return NOERROR;
}

ECode CGSMPhone::CanDial(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return ((CGsmCallTracker*)mCT.Get())->CanDial(result);
}

ECode CGSMPhone::Conference()
{
    Boolean b;
    if (mImsPhone != NULL && ( IPhone::Probe(mImsPhone)->CanConference(&b), b)) {
        Log(String("conference() - delegated to IMS phone"));
        IPhone::Probe(mImsPhone)->Conference();
        return NOERROR;
    }
    ((CGsmCallTracker*)mCT.Get())->Conference();
    return NOERROR;
}

ECode CGSMPhone::ClearDisconnected()
{
    return ((CGsmCallTracker*)mCT.Get())->ClearDisconnected();
}

ECode CGSMPhone::CanTransfer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return ((CGsmCallTracker*)mCT.Get())->CanTransfer(result);
}

ECode CGSMPhone::ExplicitCallTransfer()
{
    return ((CGsmCallTracker*)mCT.Get())->ExplicitCallTransfer();
}

ECode CGSMPhone::GetForegroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ICall::Probe(((CGsmCallTracker*)mCT.Get())->mForegroundCall);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGSMPhone::GetBackgroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ICall::Probe(((CGsmCallTracker*)mCT.Get())->mBackgroundCall);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGSMPhone::GetRingingCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    Boolean b;
    AutoPtr<CGsmCallTracker> ct = (CGsmCallTracker*)mCT.Get();
    if ( ct->mRingingCall != NULL && (ICall::Probe(ct->mRingingCall)->IsRinging(&b), b)) {
        *result = ICall::Probe(ct->mRingingCall);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if ( imsPhone != NULL ) {
        return IPhone::Probe(imsPhone)->GetRingingCall(result);
    }
    *result = ICall::Probe(ct->mRingingCall);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGSMPhone::HandleInCallMmiCommands(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    if (imsPhone != NULL) {
        AutoPtr<IServiceState> ss;
        IPhone::Probe(imsPhone)->GetServiceState((IServiceState**)&ss);
        Int32 state;
        ss->GetState(&state);
        if (state == IServiceState::STATE_IN_SERVICE) {
            return IPhone::Probe(imsPhone)->HandleInCallMmiCommands(dialString, result);
        }
    }

    Boolean b;
    if (IsInCall(&b), !b) {
        *result = FALSE;
        return NOERROR;
    }

    if (TextUtils::IsEmpty(dialString)) {
        *result = FALSE;
        return NOERROR;
    }

    b = FALSE;
    Char32 ch = dialString.GetChar(0);
    switch (ch) {
        case '0':
            b = HandleCallDeflectionIncallSupplementaryService(
                    dialString);
            break;
        case '1':
            b = HandleCallWaitingIncallSupplementaryService(
                    dialString);
            break;
        case '2':
            b = HandleCallHoldIncallSupplementaryService(dialString);
            break;
        case '3':
            b = HandleMultipartyIncallSupplementaryService(dialString);
            break;
        case '4':
            b = HandleEctIncallSupplementaryService(dialString);
            break;
        case '5':
            b = HandleCcbsIncallSupplementaryService(dialString);
            break;
        default:
            break;
    }

    *result = b;
    return NOERROR;
}

ECode CGSMPhone::IsInCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICall> fgc;
    GetForegroundCall((ICall**)&fgc);
    ICallState foregroundCallState;
    fgc->GetState(&foregroundCallState);

    AutoPtr<ICall> bgc;
    GetBackgroundCall((ICall**)&bgc);
    ICallState backgroundCallState;
    bgc->GetState(&backgroundCallState);

    AutoPtr<ICall> call;
    GetRingingCall((ICall**)&call);
    ICallState ringingCallState;
    call->GetState(&ringingCallState);

// TODO: Need ICallState::isAlive
   // *result = (foregroundCallState->IsAlive() ||
   //          backgroundCallState->IsAlive() ||
   //          ringingCallState->IsAlive());
    return NOERROR;
}

ECode CGSMPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    return Dial(dialString, NULL, videoState, extras, result);
}

ECode CGSMPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    return Dial(dialString, NULL, videoState, NULL, result);
}

ECode CGSMPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    return Dial(dialString, uusInfo, videoState, NULL, result);
}

ECode CGSMPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IImsPhone> imsPhone = mImsPhone;

    Boolean b1, b2;
    Boolean imsUseEnabled = FALSE;
// TODO: Need CImsManager
    Logger::E(TAG, "TODO Dial, CImsManager is not ready, imsUseEnable default to FALSE");
    // imsUseEnabled =
    //         (CImsManager::IsEnhanced4gLteModeSettingEnabledByPlatform(mContext, &b1), b1) &&
    //         (CImsManager::IsEnhanced4gLteModeSettingEnabledByUser(mContext, &b2), b2);
    if (!imsUseEnabled) {
        Logger::W(TAG, "IMS is disabled: forced to CS");
    }

    PhoneNumberUtils::IsEmergencyNumber(dialString, &b1);
    if (imsUseEnabled && imsPhone != NULL) {
        AutoPtr<IServiceState> ss;
        IPhone::Probe(imsPhone)->GetServiceState((IServiceState**)&ss);
        Int32 state;
        ss->GetState(&state);

        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        if ((state == IServiceState::STATE_IN_SERVICE && !b1) ||
            (b1 && (res->GetBoolean(Elastos::Droid::R::bool_::useImsAlwaysForEmergencyCall, &b2), b2))) {
            // try {
            if (LOCAL_DEBUG) Logger::D(TAG, "Trying IMS PS call");
            return IPhone::Probe(imsPhone)->Dial(dialString, videoState, extras, result);
            // } catch (CallStateException e) {
            //     if (LOCAL_DEBUG) Logger::D(TAG, "IMS PS call exception " + e);
            //     if (!ImsPhone.CS_FALLBACK.equals(e.getMessage())) {
            //         CallStateException ce = new CallStateException(e.getMessage());
            //         ce.setStackTrace(e.getStackTrace());
            //         throw ce;
            //     }
            // }

        }
    }

    if (LOCAL_DEBUG) Logger::D(TAG, "Trying (non-IMS) CS call");
    return DialInternal(dialString, NULL, IVideoProfileVideoState::AUDIO_ONLY, result);
}

ECode CGSMPhone::AddParticipant(
    /* [in] */ const String& dialString)
{
    AutoPtr<IImsPhone> imsPhone = mImsPhone;

    Boolean b1 = FALSE, b2 = FALSE;
    Boolean imsUseEnabled = FALSE;
// TODO: Need CImsManager
    // imsUseEnabled =
    //         (CImsManager->IsEnhanced4gLteModeSettingEnabledByPlatform(mContext, &b1), b1) &&
    //         (CImsManager->IsEnhanced4gLteModeSettingEnabledByUser(mContext, &b2), b2);

    if (imsUseEnabled && imsPhone != NULL) {
        AutoPtr<IServiceState> ss;
        IPhone::Probe(imsPhone)->GetServiceState((IServiceState**)&ss);
        Int32 state;
        ss->GetState(&state);
        if (state == IServiceState::STATE_IN_SERVICE) {
            // try {
            if (LOCAL_DEBUG) Logger::D(TAG, "Trying to add participant in IMS call");
            IPhone::Probe(imsPhone)->AddParticipant(dialString);
            // } catch (CallStateException e) {
            //     if (LOCAL_DEBUG) Logger::D(TAG, "IMS PS call exception " + e);
            // }
        }
    }
    else {
        Logger::E(TAG, "IMS is disabled so unable to add participant with IMS call");
    }
    return NOERROR;
}

ECode CGSMPhone::HandlePinMmi(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> obj;
    mUiccApplication->Get((IInterface**)&obj);
    AutoPtr<IGsmMmiCode> mmi;
    CGsmMmiCode::NewFromDialString(dialString, this, IUiccCardApplication::Probe(obj),
            (IGsmMmiCode**)&mmi);

    Boolean b;
    if (mmi != NULL && (((CGsmMmiCode*)mmi.Get())->IsPinPukCommand(&b), b)) {
        mPendingMMIs->Add(mmi);
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, mmi, NULL);
        mMmiRegistrants->NotifyRegistrants(ar);
        ((CGsmMmiCode*)mmi.Get())->ProcessCode();
        *result = TRUE;
    }

    *result = FALSE;
    return NOERROR;
}

ECode CGSMPhone::SendUssdResponse(
    /* [in] */ const String& ussdMessge)
{
    AutoPtr<IInterface> obj;
    mUiccApplication->Get((IInterface**)&obj);
    AutoPtr<IGsmMmiCode> mmi;
    CGsmMmiCode::NewFromUssdUserInput(ussdMessge, this,
            IUiccCardApplication::Probe(obj), (IGsmMmiCode**)&mmi);
    mPendingMMIs->Add(mmi);
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, mmi, NULL);
    mMmiRegistrants->NotifyRegistrants(ar);
    ((CGsmMmiCode*)mmi.Get())->SendUssd(ussdMessge);
    return NOERROR;
}

ECode CGSMPhone::SendDtmf(
    /* [in] */ Char32 c)
{
    Boolean b;
    if (PhoneNumberUtils::Is12Key(c, &b), b) {
        Logger::E(TAG,
                "sendDtmf called with invalid character '%d'", c);
    }
    else {
        if (((CGsmCallTracker*)mCT.Get())->mState ==  PhoneConstantsState_OFFHOOK) {
            mCi->SendDtmf(c, NULL);
        }
    }
    return NOERROR;
}

ECode CGSMPhone::StartDtmf(
    /* [in] */ Char32 c)
{
    Boolean b;
    if ((PhoneNumberUtils::Is12Key(c, &b), !b) || (c >= 'A' && c <= 'D')) {
        Logger::E(TAG,
            "startDtmf called with invalid character '%d'", c);
    }
    else {
        mCi->StartDtmf(c, NULL);
    }
    return NOERROR;
}

ECode CGSMPhone::StopDtmf()
{
    return mCi->StopDtmf(NULL);
}

ECode CGSMPhone::SendBurstDtmf(
    /* [in] */ const String& dtmfString)
{
    Logger::E(TAG, "[GSMPhone] sendBurstDtmf() is a CDMA method");
    return NOERROR;
}

ECode CGSMPhone::SetRadioPower(
    /* [in] */ Boolean power)
{
    return IServiceStateTracker::Probe(mSST)->SetRadioPower(power);
}

ECode CGSMPhone::GetVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // Read from the SIM. If its NULL, try reading from the shared preference area.
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
    String number;
    if (r != NULL) {
        r->GetVoiceMailNumber(&number);
    }
    else {
        number = String("");
    }
    if (TextUtils::IsEmpty(number)) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<ISharedPreferences> sp;
        PreferenceManager::GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&sp);
        Int64 id;
        GetSubId(&id);
        StringBuilder sb(VM_NUMBER); sb += id;
        sp->GetString(sb.ToString(), String(NULL), &number);
    }

    if (TextUtils::IsEmpty(number)) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IResources> res;
        ctx->GetResources((IResources**)&res);
        AutoPtr<ArrayOf<String> > listArray;
        res->GetStringArray(Elastos::Droid::R::array::config_default_vm_number, (ArrayOf<String>**)&listArray);

        if (listArray != NULL && listArray->GetLength() > 0) {
            for (Int32 i=0; i < listArray->GetLength(); i++) {
                if (!TextUtils::IsEmpty((*listArray)[i])) {
                    AutoPtr<ArrayOf<String> > defaultVMNumberArray;
                    StringUtils::Split((*listArray)[i], String(";"), (ArrayOf<String>**)&defaultVMNumberArray);
                    if (defaultVMNumberArray != NULL && defaultVMNumberArray->GetLength() > 0) {
                        String str;
                        if (defaultVMNumberArray->GetLength() == 1) {
                            number = (*defaultVMNumberArray)[0];
                        }
                        else if (defaultVMNumberArray->GetLength() == 2 &&
                                !TextUtils::IsEmpty((*defaultVMNumberArray)[1]) &&
                                (*defaultVMNumberArray)[1].EqualsIgnoreCase((GetGroupIdLevel1(&str), str))) {
                            number = (*defaultVMNumberArray)[0];
                            break;
                        }
                    }
                }
            }
        }
    }
    *result = number;
    return NOERROR;
}

ECode CGSMPhone::GetVoiceMailAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String ret;
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);


    if (r != NULL) {
        r->GetVoiceMailAlphaTag(&ret);
    }
    else {
        ret = String("");
    }

    if (!ret.IsNull() || ret.GetLength() == 0) {
        AutoPtr<ICharSequence> cs;
        mContext->GetText(Elastos::Droid::R::string::defaultVoiceMailAlphaTag, (ICharSequence**)&cs);
        cs->ToString(result);
    }

    *result = ret;
    return NOERROR;
}

ECode CGSMPhone::GetDeviceId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mImei;
    return NOERROR;
}

ECode CGSMPhone::GetDeviceSvn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mImeiSv;
    return NOERROR;
}

ECode CGSMPhone::GetIsimRecords(
    /* [out] */ IIsimRecords** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IIsimRecords::Probe(mIsimUiccRecords);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGSMPhone::GetImei(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mImei;
    return NOERROR;
}

ECode CGSMPhone::GetEsn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    Logger::E(TAG, "[GSMPhone] getEsn() is a CDMA method");
    *result = String("0");
    return NOERROR;
}

ECode CGSMPhone::GetMeid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    Logger::E(TAG, "[GSMPhone] getMeid() is a CDMA method");
    *result = String("0");
    return NOERROR;
}

ECode CGSMPhone::GetSubscriberId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

    if (r != NULL) {
        r->GetIMSI(result);
    }
    else {
        *result = String(NULL);
    }
    return NOERROR;
}

ECode CGSMPhone::GetGroupIdLevel1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

    if (r != NULL) {
        r->GetGid1(result);
    }
    else {
        *result = String(NULL);
    }
    return NOERROR;
}

ECode CGSMPhone::GetLine1Number(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

    if (r != NULL) {
        r->GetMsisdnNumber(result);
    }
    else {
        *result = String(NULL);
    }
    return NOERROR;
}

ECode CGSMPhone::GetMsisdn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

    if (r != NULL) {
        r->GetMsisdnNumber(result);
    }
    else {
        *result = String(NULL);
    }
    return NOERROR;
}

ECode CGSMPhone::GetLine1AlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

    if (r != NULL) {
        r->GetMsisdnAlphaTag(result);
    }
    else {
        *result = String(NULL);
    }
    return NOERROR;
}

ECode CGSMPhone::SetLine1Number(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ IMessage* onComplete)
{
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

    if (r != NULL) {
        r->SetMsisdnNumber(alphaTag, number, onComplete);
    }
    return NOERROR;
}

ECode CGSMPhone::SetVoiceMailNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& voiceMailNumber,
    /* [in] */ IMessage* onComplete)
{
    AutoPtr<IMessage> resp;
    mVmNumber = voiceMailNumber;
    ObtainMessage(EVENT_SET_VM_NUMBER_DONE, 0, 0, onComplete, (IMessage**)&resp);
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

    if (r != NULL) {
        r->SetVoiceMailNumber(alphaTag, mVmNumber, resp);
    }
    return NOERROR;
}

ECode CGSMPhone::GetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& defValue,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b;
    if(GetUnitTestMode(&b), b) {
        *result = String(NULL);
        return NOERROR;
    }
    Int64 id;
    GetSubId(&id);
    return CTelephonyManager::GetTelephonyProperty(property, id, defValue, result);
}

ECode CGSMPhone::UpdateDataConnectionTracker()
{
    return IDcTracker::Probe(mDcTracker)->Update();
}

ECode CGSMPhone::GetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ IMessage* onComplete)
{
    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    if (imsPhone != NULL) {
        AutoPtr<IServiceState> ss;
        IPhone::Probe(imsPhone)->GetServiceState((IServiceState**)&ss);
        Int32 state;
        ss->GetState(&state);
        if (state == IServiceState::STATE_IN_SERVICE) {
            IPhone::Probe(imsPhone)->GetCallForwardingOption(commandInterfaceCFReason, onComplete);
            return NOERROR;
        }
    }

    if (IsValidCommandInterfaceCFReason(commandInterfaceCFReason)) {
        if (LOCAL_DEBUG) Logger::D(TAG, "requesting call forwarding query.");
        AutoPtr<IMessage> resp;
        if (commandInterfaceCFReason == ICommandsInterface::CF_REASON_UNCONDITIONAL) {
            ObtainMessage(EVENT_GET_CALL_FORWARD_DONE, onComplete, (IMessage**)&resp);
        }
        else {
            resp = onComplete;
        }
        mCi->QueryCallForwardStatus(commandInterfaceCFReason, 0, String(NULL), resp);
    }
    return NOERROR;
}

ECode CGSMPhone::SetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ const String& dialingNumber,
    /* [in] */ Int32 timerSeconds,
    /* [in] */ IMessage* onComplete)
{
    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    if (imsPhone != NULL) {
        AutoPtr<IServiceState> ss;
        IPhone::Probe(imsPhone)->GetServiceState((IServiceState**)&ss);
        Int32 state;
        ss->GetState(&state);
        if (state == IServiceState::STATE_IN_SERVICE) {
            IPhone::Probe(imsPhone)->SetCallForwardingOption(commandInterfaceCFAction,
                    commandInterfaceCFReason, dialingNumber, timerSeconds, onComplete);
            return NOERROR;
        }
    }

    if ((IsValidCommandInterfaceCFAction(commandInterfaceCFAction)) &&
            (IsValidCommandInterfaceCFReason(commandInterfaceCFReason))) {

        AutoPtr<IMessage> resp;
        if (commandInterfaceCFReason == ICommandsInterface::CF_REASON_UNCONDITIONAL) {
            AutoPtr<Cfu> cfu = new Cfu(dialingNumber, onComplete);
            ObtainMessage(EVENT_SET_CALL_FORWARD_DONE,
                    IsCfEnable(commandInterfaceCFAction) ? 1 : 0, 0,
                    (IInterface*)(IObject*)cfu, (IMessage**)&resp);
        }
        else {
            resp = onComplete;
        }
        mCi->SetCallForward(commandInterfaceCFAction,
                commandInterfaceCFReason,
                ICommandsInterface::SERVICE_CLASS_VOICE,
                dialingNumber,
                timerSeconds,
                resp);
    }
    return NOERROR;
}

ECode CGSMPhone::SetCallForwardingUncondTimerOption(
    /* [in] */ Int32 startHour,
    /* [in] */ Int32 startMinute,
    /* [in] */ Int32 endHour,
    /* [in] */ Int32 endMinute,
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ const String& dialingNumber,
    /* [in] */ IMessage* onComplete)
{
    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    if (imsPhone != NULL) {
        IPhone::Probe(imsPhone)->SetCallForwardingUncondTimerOption(startHour, startMinute, endHour,
                endMinute, commandInterfaceCFAction, commandInterfaceCFReason,
                dialingNumber, onComplete);
    }
    else {
        if (onComplete != NULL) {
            AutoPtr<ICommandException> ce;
// TODO: Need CommandException
            // ce = new CommandException(CommandException.Error.GENERIC_FAILURE);
            AsyncResult::ForMessage(onComplete, NULL, IThrowable::Probe(ce));
            onComplete->SendToTarget();
       }
    }
    return NOERROR;
}

ECode CGSMPhone::GetCallForwardingUncondTimerOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ IMessage* onComplete)
{
    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    if (imsPhone != NULL) {
        IPhone::Probe(imsPhone)->GetCallForwardingOption(commandInterfaceCFReason, onComplete);
    }
    else {
        if (onComplete != NULL) {
            AutoPtr<ICommandException> ce;
// TODO: Need CommandException
            // ce = new CommandException(CommandException.Error.GENERIC_FAILURE);
            AsyncResult::ForMessage(onComplete, NULL, IThrowable::Probe(ce));
            onComplete->SendToTarget();
        }
    }
    return NOERROR;
}

ECode CGSMPhone::GetOutgoingCallerIdDisplay(
    /* [in] */ IMessage* onComplete)
{
    return mCi->GetCLIR(onComplete);
}

ECode CGSMPhone::SetOutgoingCallerIdDisplay(
    /* [in] */ Int32 commandInterfaceCLIRMode,
    /* [in] */ IMessage* onComplete)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_SET_CLIR_COMPLETE, commandInterfaceCLIRMode, 0, onComplete, (IMessage**)&msg);
    mCi->SetCLIR(commandInterfaceCLIRMode, msg);
    return NOERROR;
}

ECode CGSMPhone::GetCallWaiting(
    /* [in] */ IMessage* onComplete)
{
    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    if (imsPhone != NULL) {
        AutoPtr<IServiceState> ss;
        IPhone::Probe(imsPhone)->GetServiceState((IServiceState**)&ss);
        Int32 state;
        ss->GetState(&state);
        if (state == IServiceState::STATE_IN_SERVICE) {
            IPhone::Probe(imsPhone)->GetCallWaiting(onComplete);
            return NOERROR;
        }
    }

    //As per 3GPP TS 24.083, section 1.6 UE doesn't need to send service
    //class parameter in call waiting interrogation  to network
    mCi->QueryCallWaiting(ICommandsInterface::SERVICE_CLASS_NONE, onComplete);
    return NOERROR;
}

ECode CGSMPhone::SetCallWaiting(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    if (imsPhone != NULL) {
        AutoPtr<IServiceState> ss;
        IPhone::Probe(imsPhone)->GetServiceState((IServiceState**)&ss);
        Int32 state;
        ss->GetState(&state);
        if (state == IServiceState::STATE_IN_SERVICE) {
            IPhone::Probe(imsPhone)->SetCallWaiting(enable, onComplete);
            return NOERROR;
        }
    }

    mCi->SetCallWaiting(enable, ICommandsInterface::SERVICE_CLASS_VOICE, onComplete);
    return NOERROR;
}

ECode CGSMPhone::GetAvailableNetworks(
    /* [in] */ IMessage* response)
{
    return mCi->GetAvailableNetworks(response);
}

ECode CGSMPhone::GetNeighboringCids(
    /* [in] */ IMessage* response)
{
    return mCi->GetNeighboringCids(response);
}

ECode CGSMPhone::SetOnPostDialCharacter(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mPostDialHandler);
    return NOERROR;
}

ECode CGSMPhone::SetUiTTYMode(
    /* [in] */ Int32 uiTtyMode,
    /* [in] */ IMessage* onComplete)
{
    if (mImsPhone != NULL) {
        IPhone::Probe(mImsPhone)->SetUiTTYMode(uiTtyMode, onComplete);
    }
    return NOERROR;
}

ECode CGSMPhone::SetMute(
    /* [in] */ Boolean muted)
{
    return ((CGsmCallTracker*)mCT.Get())->SetMute(muted);
}

ECode CGSMPhone::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return ((CGsmCallTracker*)mCT.Get())->GetMute(result);
}

ECode CGSMPhone::GetDataCallList(
    /* [in] */ IMessage* response)
{
    return mCi->GetDataCallList(response);
}

ECode CGSMPhone::UpdateServiceLocation()
{
    return IServiceStateTracker::Probe(mSST)->EnableSingleLocationUpdate();
}

ECode CGSMPhone::EnableLocationUpdates()
{
    IServiceStateTracker::Probe(mSST)->EnableLocationUpdates();
    return NOERROR;
}

ECode CGSMPhone::DisableLocationUpdates()
{
    return IServiceStateTracker::Probe(mSST)->DisableLocationUpdates();
}

ECode CGSMPhone::GetDataRoamingEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mDcTracker->GetDataOnRoamingEnabled(result);
}

ECode CGSMPhone::SetDataRoamingEnabled(
    /* [in] */ Boolean enable)
{
    return mDcTracker->SetDataOnRoamingEnabled(enable);
}

ECode CGSMPhone::GetDataEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mDcTracker->GetDataEnabled(result);
}

ECode CGSMPhone::SetDataEnabled(
    /* [in] */ Boolean enable)
{
    return mDcTracker->SetDataEnabled(enable);
}

ECode CGSMPhone::OnMMIDone(
    /* [in] */ IGsmMmiCode* mmi)
{
    /* Only notify complete if it's on the pending list.
     * Otherwise, it's already been handled (eg, previously canceled).
     * The exception is cancellation of an incoming USSD-REQUEST, which is
     * not on the list.
     */
    Boolean b1, b2, b3;
    if ((IList::Probe(mPendingMMIs)->Remove(mmi, &b1), b1)
            || (IMmiCode::Probe(mmi)->IsUssdRequest(&b2), b2)
            || (mmi->IsSsInfo(&b3), b3)) {
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, mmi, NULL);
        mMmiCompleteRegistrants->NotifyRegistrants(ar);
    }
    return NOERROR;
}

ECode CGSMPhone::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;
    AutoPtr<IMessage> onComplete;

    if (!mIsTheCurrentActivePhone) {
        // Logger::E(TAG, "Received message " + msg +
        //         "[" + msg.what + "] while being destroyed. Ignoring.");
        return NOERROR;
    }

    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case EVENT_RADIO_AVAILABLE: {
            AutoPtr<IMessage> message;
            ObtainMessage(EVENT_GET_BASEBAND_VERSION_DONE, (IMessage**)&message);
            mCi->GetBasebandVersion(message);

            ObtainMessage(EVENT_GET_IMEI_DONE, (IMessage**)&message);
            mCi->GetIMEI(message);
            ObtainMessage(EVENT_GET_IMEISV_DONE, (IMessage**)&message);
            mCi->GetIMEISV(message);
        }
        break;

        case EVENT_RADIO_ON:
            // do-nothing
            break;

        case EVENT_REGISTERED_TO_NETWORK:
            SyncClirSetting();
            break;

        case EVENT_SIM_RECORDS_LOADED:{
            Boolean b;
            UpdateCurrentCarrierInProvider(&b);

            // Check if this is a different SIM than the previous one. If so unset the
            // voice mail number and the call forwarding flag.
            String imsi = GetSimImsi();
            String imsiFromSIM;
            GetSubscriberId(&imsiFromSIM);
            if (!imsi.IsNull() && !imsiFromSIM.IsNull() && !imsiFromSIM.Equals(imsi)) {
                StoreVoiceMailNumber(String(NULL));
                SetCallForwardingPreference(FALSE);
                SetSimImsi(String(NULL));
                AutoPtr<ISubscriptionController> controller = SubscriptionController::GetInstance();
                controller->RemoveStaleSubPreferences(VM_NUMBER);
                controller->RemoveStaleSubPreferences(SIM_IMSI);
                controller->RemoveStaleSubPreferences(CF_ENABLED);
            }

            mSimRecordsLoadedRegistrants->NotifyRegistrants();
            UpdateVoiceMail();
            UpdateCallForwardStatus();
        }
        break;

        case EVENT_GET_BASEBAND_VERSION_DONE:
            ar = (AsyncResult*)(IObject::Probe(obj));

            if (ar->mException != NULL) {
                Logger::D(TAG, "TODO remove this Baseband version get error");
                break;
            }

            // if (LOCAL_DEBUG) Logger::D(TAG, "Baseband version: " + ar->mResult);
            if (ICharSequence::Probe(ar->mResult) != NULL) {
                String str;
                ICharSequence::Probe(ar->mResult)->ToString(&str);
                SetSystemProperty(ITelephonyProperties::PROPERTY_BASEBAND_VERSION, str);
                Logger::D(TAG, "Baseband version: %s", str.string());//, ar->mResult);
            }
        break;

        case EVENT_GET_IMEI_DONE:
            ar = (AsyncResult*)(IObject::Probe(obj));

            if (ar->mException != NULL) {
                break;
            }

            ICharSequence::Probe(ar->mResult)->ToString(&mImei);
        break;

        case EVENT_GET_IMEISV_DONE:
            ar = (AsyncResult*)(IObject::Probe(obj));

            if (ar->mException != NULL) {
                break;
            }

            ICharSequence::Probe(ar->mResult)->ToString(&mImeiSv);
        break;

        case EVENT_USSD: {
            ar = (AsyncResult*)(IObject::Probe(obj));

            AutoPtr<IArrayOf> ussdResult = IArrayOf::Probe(ar->mResult);

            Int32 len;
            ussdResult->GetLength(&len);
            if (len > 1) {
                // try {
                AutoPtr<IInterface> obj0, obj1;
                ussdResult->Get(0, (IInterface**)&obj0);
                ussdResult->Get(1, (IInterface**)&obj1);
                String s0, s1;
                ICharSequence::Probe(obj0)->ToString(&s0);
                ICharSequence::Probe(obj1)->ToString(&s1);

                OnIncomingUSSD(StringUtils::ParseInt32(s0), s1);
                // } catch (NumberFormatException e) {
                //     Logger::W(TAG, "error parsing USSD");
                // }
            }
        break;
        }
        case EVENT_RADIO_OFF_OR_NOT_AVAILABLE: {
            // Some MMI requests (eg USSD) are not completed
            // within the course of a CommandsInterface request
            // If the radio shuts off or resets while one of these
            // is pending, we need to clean up.
            Int32 size;
            mPendingMMIs->GetSize(&size);
            for (Int32 i = size - 1; i >= 0; i--) {
                AutoPtr<IInterface> obj;
                mPendingMMIs->Get(i, (IInterface**)&obj);
                AutoPtr<IGsmMmiCode> gmc = IGsmMmiCode::Probe(obj);
                Boolean b;
                if (gmc->IsPendingUSSD(&b), b) {
                    ((CGsmMmiCode*)gmc.Get())->OnUssdFinishedError();
                }
            }
            break;
        }

        case EVENT_SSN: {
            ar = (AsyncResult*)(IObject::Probe(obj));
            AutoPtr<ISuppServiceNotification> no = ISuppServiceNotification::Probe(ar->mResult);
            mSsnRegistrants->NotifyRegistrants(ar);
        break;
        }
        case EVENT_SET_CALL_FORWARD_DONE: {
            ar = (AsyncResult*)(IObject::Probe(obj));
            AutoPtr<IInterface> obj;
            mIccRecords->Get((IInterface**)&obj);
            AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

            AutoPtr<Cfu> cfu = (Cfu*)(IObject::Probe(ar->mUserObj));
            if (ar->mException == NULL && r != NULL) {
                Int32 arg1;
                msg->GetArg1(&arg1);
                r->SetVoiceCallForwardingFlag(1, arg1 == 1, cfu->mSetCfNumber);
                SetCallForwardingPreference(arg1 == 1);
            }
            if (cfu->mOnComplete != NULL) {
                AsyncResult::ForMessage(cfu->mOnComplete, ar->mResult, ar->mException);
                cfu->mOnComplete->SendToTarget();
            }
            break;
        }
        case EVENT_SET_VM_NUMBER_DONE:
            ar = (AsyncResult*)(IObject::Probe(obj));
            if (IIccVmNotSupportedException::Probe(ar->mException) != NULL) {
                StoreVoiceMailNumber(mVmNumber);
                ar->mException = NULL;
            }
            onComplete = IMessage::Probe(ar->mUserObj);
            if (onComplete != NULL) {
                AsyncResult::ForMessage(onComplete, ar->mResult, ar->mException);
                onComplete->SendToTarget();
            }
            break;

        case EVENT_GET_CALL_FORWARD_DONE:
            ar = (AsyncResult*)(IObject::Probe(obj));
            if (ar->mException == NULL) {
                AutoPtr<IArrayOf> iarray = IArrayOf::Probe(ar->mResult);
                Int32 len;
                iarray->GetLength(&len);
                AutoPtr<ArrayOf<ICallForwardInfo*> > array = ArrayOf<ICallForwardInfo*>::Alloc(len);
                for (Int32 i = 0; i < len; i++) {
                    AutoPtr<IInterface> obj;
                    iarray->Get(i, (IInterface**)&obj);
                    array->Set(i, ICallForwardInfo::Probe(obj));
                }
                HandleCfuQueryResult(array);
            }
            onComplete = IMessage::Probe(ar->mUserObj);
            if (onComplete != NULL) {
                AsyncResult::ForMessage(onComplete, ar->mResult, ar->mException);
                onComplete->SendToTarget();
            }
            break;

        case EVENT_SET_NETWORK_AUTOMATIC: {
            // Automatic network selection from EF_CSP SIM record
            ar = (AsyncResult*)(IObject::Probe(obj));
            Boolean b;
            if (((CGsmServiceStateTracker*)mSST.Get())->mSS->GetIsManualSelection(&b), b) {
                SetNetworkSelectionModeAutomatic(IMessage::Probe(ar->mResult));
                Logger::D(TAG, "SET_NETWORK_SELECTION_AUTOMATIC: set to automatic");
            }
            else {
                // prevent duplicate request which will push current PLMN to low priority
                Logger::D(TAG, "SET_NETWORK_SELECTION_AUTOMATIC: already automatic, ignore");
            }
            break;
        }
        case EVENT_ICC_RECORD_EVENTS: {
            ar = (AsyncResult*)(IObject::Probe(obj));
            Int32 val;
            IInteger32::Probe(ar->mResult)->GetValue(&val);
            ProcessIccRecordEvents(val);
            break;
        }
        case EVENT_SET_CLIR_COMPLETE:
            ar = (AsyncResult*)(IObject::Probe(obj));
            if (ar->mException == NULL) {
                Int32 arg1;
                msg->GetArg1(&arg1);
                SaveClirSetting(arg1);
            }
            onComplete = IMessage::Probe(ar->mUserObj);
            if (onComplete != NULL) {
                AsyncResult::ForMessage(onComplete, ar->mResult, ar->mException);
                onComplete->SendToTarget();
            }
            break;

        case IPhoneConstants::EVENT_SUBSCRIPTION_ACTIVATED:
            Log(String("EVENT_SUBSCRIPTION_ACTIVATED"));
            OnSubscriptionActivated();
            break;

        case IPhoneConstants::EVENT_SUBSCRIPTION_DEACTIVATED:
            Log(String("EVENT_SUBSCRIPTION_DEACTIVATED"));
            OnSubscriptionDeactivated();
            break;

        case EVENT_SS: {
            ar = (AsyncResult*)(IObject::Probe(obj));
            Logger::D(TAG, "Event EVENT_SS received");
            // SS data is already being handled through MMI codes.
            // So, this result if processed as MMI response would help
            // in re-using the existing functionality.
            AutoPtr<IInterface> obj;
            mUiccApplication->Get((IInterface**)&obj);
            AutoPtr<IGsmMmiCode> mmi;
            CGsmMmiCode::New(this, IUiccCardApplication::Probe(obj), (IGsmMmiCode**)&mmi);
            ((CGsmMmiCode*)mmi.Get())->ProcessSsData(ar);
        }
        case CHECK_CALLFORWARDING_STATUS: {
            Boolean cfEnabled = GetCallForwardingPreference();
            if (LOCAL_DEBUG) Logger::D(TAG, "Callforwarding is %d", cfEnabled);
            if (cfEnabled) {
                NotifyCallForwardingIndicator();
            }
            break;
        }
         default:
             PhoneBase::HandleMessage(msg);
    }
    return NOERROR;
}

ECode CGSMPhone::UpdateCurrentCarrierInProvider(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    Int64 currentDds;
    sm->GetDefaultDataSubId(&currentDds);
    String operatorNumeric;
    GetOperatorNumeric(&operatorNumeric);

    Int64 id;
    GetSubId(&id);
    Logger::D(TAG, "updateCurrentCarrierInProvider: mSubId = %lld currentDds = %lld operatorNumeric = %s"
            , id, currentDds, operatorNumeric.string());

    if (!operatorNumeric.IsNullOrEmpty() && (id == currentDds)) {
        // try {
        AutoPtr<ITelephonyCarriers> tc;
        CTelephonyCarriers::AcquireSingleton((ITelephonyCarriers**)&tc);
        AutoPtr<IUri> contentUri;
        tc->GetCONTENT_URI((IUri**)&contentUri);
        AutoPtr<IUri> uri;
        Uri::WithAppendedPath(contentUri, String("current"), (IUri**)&uri);
        AutoPtr<IContentValues> map;
        CContentValues::New((IContentValues**)&map);
        map->Put(ITelephonyCarriers::NUMERIC, operatorNumeric);
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<IUri> insertedUri;
        cr->Insert(uri, map, (IUri**)&insertedUri);
        *result = TRUE;
        return NOERROR;
        // } catch (SQLException e) {
        //     Logger::E(TAG, "Can't store current operator", e);
        // }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CGSMPhone::SaveClirSetting(
    /* [in] */ Int32 commandInterfaceCLIRMode)
{
    // open the shared preferences editor, and write the value.
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&sp);
    AutoPtr<ISharedPreferencesEditor> editor;
    sp->Edit((ISharedPreferencesEditor**)&editor);
    Int32 pid;
    GetPhoneId(&pid);
    StringBuilder sb(CLIR_KEY); sb += pid;
    editor->PutInt32(sb.ToString(), commandInterfaceCLIRMode);

    // commit and log the result.
    Boolean b;
    if (editor->Commit(&b), !b) {
        Logger::E(TAG, "failed to commit CLIR preference");
    }
    return NOERROR;
}

ECode CGSMPhone::GetPhoneSubInfo(
    /* [out] */ IPhoneSubInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSubInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGSMPhone::GetIccPhoneBookInterfaceManager(
    /* [out] */ IIccPhoneBookInterfaceManager** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IIccPhoneBookInterfaceManager::Probe(mSimPhoneBookIntManager);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGSMPhone::ActivateCellBroadcastSms(
    /* [in] */ Int32 activate,
    /* [in] */ IMessage* response)
{
    Logger::E(TAG, "[GSMPhone] activateCellBroadcastSms() is obsolete; use SmsManager");
    return response->SendToTarget();
}

ECode CGSMPhone::GetCellBroadcastSmsConfig(
    /* [in] */ IMessage* response)
{
    Logger::E(TAG, "[GSMPhone] getCellBroadcastSmsConfig() is obsolete; use SmsManager");
    return response->SendToTarget();
}

ECode CGSMPhone::SetCellBroadcastSmsConfig(
    /* [in] */ ArrayOf<Int32>* configValuesArray,
    /* [in] */ IMessage* response)
{
    Logger::E(TAG, "[GSMPhone] setCellBroadcastSmsConfig() is obsolete; use SmsManager");
    return response->SendToTarget();
}

ECode CGSMPhone::IsCspPlmnEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

    if (r != NULL) {
        r->IsCspPlmnEnabled(result);
    }
    else {
        *result = FALSE;
    }
    return NOERROR;
}

ECode CGSMPhone::IsManualNetSelAllowed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 nwMode = IPhone::PREFERRED_NT_MODE;

    AutoPtr<IPhoneFactory> pf;
    CPhoneFactory::AcquireSingleton((IPhoneFactory**)&pf);
    pf->CalculatePreferredNetworkType(mContext, mPhoneId, &nwMode);

    Logger::D(TAG, "isManualNetSelAllowed in mode = %d", nwMode);
    /*
     *  For multimode targets in global mode manual network
     *  selection is disallowed
     */
    Boolean b;
    if ((SystemProperties::GetBoolean(IPhoneBase::PROPERTY_MULTIMODE_CDMA, FALSE, &b), b)
            && ((nwMode == IPhone::NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA)
                    || (nwMode == IPhone::NT_MODE_GLOBAL)) ){
        Logger::D(TAG, "Manual selection not supported in mode = %d", nwMode);
        *result = FALSE;
        return NOERROR;
    }

    /*
     *  Single mode phone with - GSM network modes/global mode
     *  LTE only for 3GPP
     *  LTE centric + 3GPP Legacy
     *  Note: the actual enabling/disabling manual selection for these
     *  cases will be controlled by csp
     */
    *result = TRUE;
    return NOERROR;
}

ECode CGSMPhone::ExitEmergencyCallbackMode()
{
    if (mImsPhone != NULL) {
        IPhone::Probe(mImsPhone)->ExitEmergencyCallbackMode();
    }
    return NOERROR;
}

ECode CGSMPhone::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("GSMPhone extends:"));
    PhoneBase::Dump(fd, pw, args);
    // pw->Println(" mCT=" + mCT);
    // pw->Println(" mSST=" + mSST);
    // pw->Println(" mPendingMMIs=" + mPendingMMIs);
    // pw->Println(" mSimPhoneBookIntManager=" + mSimPhoneBookIntManager);
    // pw->Println(" mSubInfo=" + mSubInfo);
    // if (VDBG) pw->Println(" mImei=" + mImei);
    // if (VDBG) pw->Println(" mImeiSv=" + mImeiSv);
    // pw->Println(" mVmNumber=" + mVmNumber);
    return NOERROR;
}

ECode CGSMPhone::SetOperatorBrandOverride(
    /* [in] */ const String& brand,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mUiccController == NULL) {
        return FALSE;
    }

    AutoPtr<IUiccCard> card;
    mUiccController->GetUiccCard((IUiccCard**)&card);
    if (card == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Boolean status;
    card->SetOperatorBrandOverride(brand, &status);

    // Refresh.
    if (status) {
        AutoPtr<IInterface> obj;
        mIccRecords->Get((IInterface**)&obj);
        AutoPtr<IIccRecords> iccRecords = IIccRecords::Probe(obj);

        if (iccRecords != NULL) {
            String name;
            iccRecords->GetServiceProviderName(&name);
            SystemProperties::Set(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, name);
        }
        if (mSST != NULL) {
            IServiceStateTracker::Probe(mSST)->PollState();
        }
    }
    *result = status;
    return NOERROR;
}

ECode CGSMPhone::GetOperatorNumeric(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String operatorNumeric;
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

    if (r != NULL) {
        r->GetOperatorNumeric(&operatorNumeric);
    }
    *result = operatorNumeric;
    return NOERROR;
}

ECode CGSMPhone::RegisterForAllDataDisconnected(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return IDcTracker::Probe(mDcTracker)->RegisterForAllDataDisconnected(h, what, obj);
}

ECode CGSMPhone::UnregisterForAllDataDisconnected(
    /* [in] */ IHandler* h)
{
    return IDcTracker::Probe(mDcTracker)->UnregisterForAllDataDisconnected(h);
}

ECode CGSMPhone::SetInternalDataEnabled(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onCompleteMsg)
{
    Boolean b;
    return IDcTracker::Probe(mDcTracker)->SetInternalDataEnabled(enable, onCompleteMsg, &b);
}

ECode CGSMPhone::SetInternalDataEnabledFlag(
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return IDcTracker::Probe(mDcTracker)->SetInternalDataEnabledFlag(enable, result);
}

ECode CGSMPhone::NotifyEcbmTimerReset(
    /* [in] */ Boolean flag)
{
    mEcmTimerResetRegistrants->NotifyResult(CoreUtils::Convert(flag));
    return NOERROR;
}

ECode CGSMPhone::RegisterForEcmTimerReset(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mEcmTimerResetRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

ECode CGSMPhone::UnregisterForEcmTimerReset(
    /* [in] */ IHandler* h)
{
    mEcmTimerResetRegistrants->Remove(h);
    return NOERROR;
}

ECode CGSMPhone::ResetSubSpecifics()
{
    return NOERROR;
}

ECode CGSMPhone::SetVoiceMessageWaiting(
    /* [in] */ Int32 line,
    /* [in] */ Int32 countWaiting)
{
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

    if (r != NULL) {
        r->SetVoiceMessageWaiting(line, countWaiting);
    }
    else {
        Log(String("SIM Records not found, MWI not updated"));
    }
    return NOERROR;
}

ECode CGSMPhone::GetCallBarringOption(
    /* [in] */ const String& facility,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    if (IsValidFacilityString(facility)) {
        mCi->QueryFacilityLock(facility, password, ICommandsInterface::SERVICE_CLASS_NONE,
                onComplete);
    }
    return NOERROR;
}

ECode CGSMPhone::SetCallBarringOption(
    /* [in] */ const String& facility,
    /* [in] */ Boolean lockState,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    if (IsValidFacilityString(facility)) {
        mCi->SetFacilityLock(facility, lockState, password,
                ICommandsInterface::SERVICE_CLASS_VOICE, onComplete);
    }
    return NOERROR;
}

ECode CGSMPhone::RequestChangeCbPsw(
    /* [in] */ const String& facility,
    /* [in] */ const String& oldPwd,
    /* [in] */ const String& newPwd,
    /* [in] */ IMessage* result)
{
    return mCi->ChangeBarringPassword(facility, oldPwd, newPwd, result);
}

void CGSMPhone::SetProperties()
{
    Int64 id;
    GetSubId(&id);
    String str = StringUtils::ToString(IPhoneConstants::PHONE_TYPE_GSM);
    CTelephonyManager::SetTelephonyProperty(ITelephonyProperties::CURRENT_ACTIVE_PHONE,
            id, str);
}

ECode CGSMPhone::DialInternal(
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** connection)
{
    VALIDATE_NOT_NULL(connection);
    // Need to make sure dialString gets parsed properly
    String newDialString;
    PhoneNumberUtils::StripSeparators(dialString, &newDialString);

    // handle in-call MMI first if applicable
    Boolean b;
    if (HandleInCallMmiCommands(newDialString, &b), b) {
        *connection = NULL;
        return NOERROR;
    }

    // Only look at the Network portion for mmi
    String networkPortion;
    PhoneNumberUtils::ExtractNetworkPortionAlt(newDialString, &networkPortion);
    AutoPtr<IInterface> obj;
    mUiccApplication->Get((IInterface**)&obj);
    AutoPtr<IGsmMmiCode> mmi;
    CGsmMmiCode::NewFromDialString(networkPortion, this,
            IUiccCardApplication::Probe(obj), (IGsmMmiCode**)&mmi);
    // if (LOCAL_DEBUG) Logger::D(TAG,
    //                        "dialing w/ mmi '" + mmi + "'...");

    if (mmi == NULL) {
        return ((CGsmCallTracker*)mCT.Get())->Dial(newDialString, uusInfo, connection);
    }
    else if (((CGsmMmiCode*)mmi.Get())->IsTemporaryModeCLIR(&b), b) {
        Int32 mode;
        ((CGsmMmiCode*)mmi.Get())->GetCLIRMode(&mode);
        return ((CGsmCallTracker*)mCT.Get())->Dial(((CGsmMmiCode*)mmi.Get())->mDialingNumber,
                mode, uusInfo, connection);
    }
    else {
        mPendingMMIs->Add(mmi);
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, mmi, NULL);
        mMmiRegistrants->NotifyRegistrants(ar);
        ((CGsmMmiCode*)mmi.Get())->ProcessCode();

        // FIXME should this return NULL or something else?
        *connection = NULL;
        return NOERROR;
    }
}

Boolean CGSMPhone::IsCfEnable(
    /* [in] */ Int32 action)
{
    return (action == ICommandsInterface::CF_ACTION_ENABLE)
            || (action == ICommandsInterface::CF_ACTION_REGISTRATION);
}

void CGSMPhone::SetCallForwardingPreference(
    /* [in] */ Boolean enabled)
{
    if (LOCAL_DEBUG) Logger::D(TAG, "Set callforwarding info to perferences");
    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(mContext, (ISharedPreferences**)&sp);

    AutoPtr<ISharedPreferencesEditor> edit;
    sp->Edit((ISharedPreferencesEditor**)&edit);
    Int64 id;
    GetSubId(&id);
    StringBuilder sb(CF_ENABLED); sb += id;
    edit->PutBoolean(sb.ToString(), enabled);
    Boolean b;
    edit->Commit(&b);

    // set the sim imsi to be able to track when the sim card is changed.
    String sid;
    GetSubscriberId(&sid);
    SetSimImsi(sid);
}

Boolean CGSMPhone::GetCallForwardingPreference()
{
    if (LOCAL_DEBUG) Logger::D(TAG, "Get callforwarding info from perferences");

    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(mContext, (ISharedPreferences**)&sp);

    Int64 id;
    GetSubId(&id);
    Boolean cf;
    StringBuilder sb(CF_ENABLED); sb += id;
    sp->GetBoolean(sb.ToString(), FALSE, &cf);
    return cf;
}

void CGSMPhone::SyncClirSetting()
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&sp);

    Int32 pid;
    GetPhoneId(&pid);
    Int32 clirSetting;
    StringBuilder sb(CLIR_KEY); sb += pid;
    sp->GetInt32(sb.ToString(), -1, &clirSetting);
    if (clirSetting >= 0) {
        mCi->SetCLIR(clirSetting, NULL);
    }
}

AutoPtr<IUiccCardApplication> CGSMPhone::GetUiccCardApplication()
{
    AutoPtr<IUiccCardApplication> app;
    if (mUiccController == NULL)
        return NULL;
    IUiccController::Probe(mUiccController)->GetUiccCardApplication(mPhoneId,
        IUiccController::APP_FAM_3GPP, (IUiccCardApplication**)&app);
    return app;
}

void CGSMPhone::SetCardInPhoneBook()
{
    if (mUiccController == NULL ) {
        return;
    }

    AutoPtr<IUiccCard> uc;
    mUiccController->GetUiccCard(mPhoneId, (IUiccCard**)&uc);
    IIccPhoneBookInterfaceManager::Probe(mSimPhoneBookIntManager)->SetIccCard(uc);
}

ECode CGSMPhone::OnUpdateIccAvailability()
{
    if (mUiccController == NULL ) {
        return NOERROR;
    }

    // Get the latest info on the card and
    // send this to Phone Book
    SetCardInPhoneBook();

    AutoPtr<IUiccCardApplication> newUiccApplication;
    mUiccController->GetUiccCardApplication(mPhoneId, IUiccController::APP_FAM_IMS,
            (IUiccCardApplication**)&newUiccApplication);
    AutoPtr<IIsimUiccRecords> newIsimUiccRecords;

    if (newUiccApplication != NULL) {
        AutoPtr<IIccRecords> rec;
        newUiccApplication->GetIccRecords((IIccRecords**)&rec);
        newIsimUiccRecords = IIsimUiccRecords::Probe(rec);
        if (LOCAL_DEBUG) Log(String("New ISIM application found"));
    }
    mIsimUiccRecords = newIsimUiccRecords;

    newUiccApplication = GetUiccCardApplication();

    AutoPtr<IInterface> obj;
    mUiccApplication->Get((IInterface**)&obj);
    AutoPtr<IUiccCardApplication> app = IUiccCardApplication::Probe(obj);
    if (app != newUiccApplication) {
        if (app != NULL) {
            if (LOCAL_DEBUG) Log(String("Removing stale icc objects."));
            AutoPtr<IInterface> o;
            mIccRecords->Get((IInterface**)&o);
            AutoPtr<IIccRecords> r = IIccRecords::Probe(o);
            if (r != NULL) {
                UnregisterForSimRecordEvents();
            }
            mIccRecords->Set(NULL);
            mUiccApplication->Set(NULL);
        }
        if (newUiccApplication != NULL) {
            if (LOCAL_DEBUG) Log(String("New Uicc application found"));
            mUiccApplication->Set(newUiccApplication);
            AutoPtr<IIccRecords> rec;
            newUiccApplication->GetIccRecords((IIccRecords**)&rec);
            mIccRecords->Set(rec);
            RegisterForSimRecordEvents();
        }
    }
    return NOERROR;
}

void CGSMPhone::Log(
    /* [in] */ const String& s)
{
    Logger::D(TAG, "[GSMPhone] %s", s.string());
}

void CGSMPhone::OnSubscriptionActivated()
{
    //mSubscriptionData = SubscriptionManager.getCurrentSubscription(mSubscription);

    AutoPtr<Subscription> sc = (Subscription*)mSubscriptionData.Get();
    Logger::I("CGSMPhone", "SUBSCRIPTION ACTIVATED : slotId : %d appid : %d subId : %d subStatus : %d",
        sc->mSlotId, sc->m3gppIndex, sc->mSubId, sc->mSubStatus);

    // Make sure properties are set for proper subscription.
    SetProperties();

    OnUpdateIccAvailability();
    AutoPtr<IMessage> msg;
    IHandler::Probe(mSST)->ObtainMessage(IDctConstants::EVENT_ICC_CHANGED, (IMessage**)&msg);
    Boolean b;
    IHandler::Probe(mSST)->SendMessage(msg, &b);
    IDcTracker::Probe(mDcTracker)->UpdateRecords();
}

void CGSMPhone::OnSubscriptionDeactivated()
{
    Log(String("SUBSCRIPTION DEACTIVATED"));
    mSubscriptionData = NULL;
    ResetSubSpecifics();
}

void CGSMPhone::UpdateVoiceMail()
{
    Int32 countVoiceMessages = 0;
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

    if (r != NULL) {
        // get voice mail count from SIM
        r->GetVoiceMessageCount(&countVoiceMessages);
    }
    if (countVoiceMessages == -1) {
        countVoiceMessages = GetStoredVoiceMessageCount();
    }
    Int64 id;
    GetSubId(&id);
    Logger::D(TAG, "updateVoiceMail countVoiceMessages = %d subId %d", countVoiceMessages, id);
    SetVoiceMessageCount(countVoiceMessages);
}

Boolean CGSMPhone::HandleCallDeflectionIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    if (dialString.GetLength() > 1) {
        return FALSE;
    }

    AutoPtr<ICall> ringCall;
    GetRingingCall((ICall**)&ringCall);
    ICallState state;
    ringCall->GetState(&state);

    AutoPtr<ICall> gsmCall;
    GetBackgroundCall((ICall**)&gsmCall);
    ICallState bState;
    gsmCall->GetState(&bState);

    if (state != ICallState_IDLE) {
        if (LOCAL_DEBUG) Logger::D(TAG, "MmiCode 0: rejectCall");
        // try {
        ((CGsmCallTracker*)mCT.Get())->RejectCall();
        // } catch (CallStateException e) {
        //     if (LOCAL_DEBUG) Logger::D(TAG,
        //         "reject failed", e);
        //     notifySuppServiceFailed(IPhoneSuppService_REJECT);
        // }
    }
    else if (bState != ICallState_IDLE) {
        if (LOCAL_DEBUG) Logger::D(TAG,
                "MmiCode 0: hangupWaitingOrBackground");
        ((CGsmCallTracker*)mCT.Get())->HangupWaitingOrBackground();
    }

    return TRUE;
}

Boolean CGSMPhone::HandleCallWaitingIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    Int32 len = dialString.GetLength();

    if (len > 2) {
        return FALSE;
    }

    AutoPtr<ICall> call;
    GetForegroundCall((ICall**)&call);

    // try {
    if (len > 1) {
        Char32 ch = dialString.GetChar(1);
        Int32 callIndex = ch - '0';

        if (callIndex >= 1 && callIndex <= CGsmCallTracker::MAX_CONNECTIONS) {
            if (LOCAL_DEBUG) Logger::D(TAG,
                    "MmiCode 1: hangupConnectionByIndex %d", callIndex);
            ((CGsmCallTracker*)mCT.Get())->HangupConnectionByIndex(IGsmCall::Probe(call), callIndex);
        }
    }
    else {
        ICallState state;
        call->GetState(&state);
        if (state != ICallState_IDLE) {
            if (LOCAL_DEBUG) Logger::D(TAG,
                    "MmiCode 1: hangup foreground");
            //mCT.hangupForegroundResumeBackground();
            ((CGsmCallTracker*)mCT.Get())->Hangup(IGsmCall::Probe(call));
        }
        else {
            if (LOCAL_DEBUG) Logger::D(TAG,
                    "MmiCode 1: switchWaitingOrHoldingAndActive");
            ((CGsmCallTracker*)mCT.Get())->SwitchWaitingOrHoldingAndActive();
        }
    }
    // } catch (CallStateException e) {
    //     if (LOCAL_DEBUG) Logger::D(TAG,
    //         "hangup failed", e);
    //     notifySuppServiceFailed(IPhoneSuppService_HANGUP);
    // }

    return TRUE;
}

Boolean CGSMPhone::HandleCallHoldIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    Int32 len = dialString.GetLength();

    if (len > 2) {
        return FALSE;
    }

    AutoPtr<ICall> call;
    GetForegroundCall((ICall**)&call);

    if (len > 1) {
        // try {
        Char32 ch = dialString.GetChar(1);
        Int32 callIndex = ch - '0';
        AutoPtr<IGsmConnection> conn;
        ((CGsmCallTracker*)mCT.Get())->GetConnectionByIndex(IGsmCall::Probe(call), callIndex, (IGsmConnection**)&conn);

        // gsm index starts at 1, up to 5 connections in a call,
        if (conn != NULL && callIndex >= 1 && callIndex <= CGsmCallTracker::MAX_CONNECTIONS) {
            if (LOCAL_DEBUG) Logger::D(TAG, "MmiCode 2: separate call %d", callIndex);
            ((CGsmCallTracker*)mCT.Get())->Separate(conn);
        }
        else {
            if (LOCAL_DEBUG) Logger::D(TAG, "separate: invalid call index %d", callIndex);
            NotifySuppServiceFailed(IPhoneSuppService_SEPARATE);
        }
        // } catch (CallStateException e) {
        //     if (LOCAL_DEBUG) Logger::D(TAG,
        //         "separate failed", e);
        //     notifySuppServiceFailed(IPhoneSuppService_SEPARATE);
        // }
    }
    else {
        // try {
        AutoPtr<ICall> ringCall;
        GetRingingCall((ICall**)&ringCall);
        ICallState state;
        ringCall->GetState(&state);
        if (state != ICallState_IDLE) {
            if (LOCAL_DEBUG) Logger::D(TAG,
            "MmiCode 2: accept ringing call");
            ((CGsmCallTracker*)mCT.Get())->AcceptCall();
        }
        else {
            if (LOCAL_DEBUG) Logger::D(TAG,
            "MmiCode 2: switchWaitingOrHoldingAndActive");
            ((CGsmCallTracker*)mCT.Get())->SwitchWaitingOrHoldingAndActive();
        }
        // } catch (CallStateException e) {
        //     if (LOCAL_DEBUG) Logger::D(TAG,
        //         "switch failed", e);
        //     notifySuppServiceFailed(IPhoneSuppService_SWITCH);
        // }
    }

    return TRUE;
}

Boolean CGSMPhone::HandleMultipartyIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    if (dialString.GetLength() > 1) {
        return FALSE;
    }

    if (LOCAL_DEBUG) Logger::D(TAG, "MmiCode 3: merge calls");
    Conference();
    return TRUE;
}

Boolean CGSMPhone::HandleEctIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    Int32 len = dialString.GetLength();

    if (len != 1) {
        return FALSE;
    }

    if (LOCAL_DEBUG) Logger::D(TAG, "MmiCode 4: explicit call transfer");
    ExplicitCallTransfer();
    return TRUE;
}

Boolean CGSMPhone::HandleCcbsIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    if (dialString.GetLength() > 1) {
        return FALSE;
    }

    Logger::I(TAG, "MmiCode 5: CCBS not supported!");
    // Treat it as an "unknown" service.
    NotifySuppServiceFailed(IPhoneSuppService_UNKNOWN);
    return TRUE;
}

void CGSMPhone::StoreVoiceMailNumber(
    /* [in] */ const String& number)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&sp);
    AutoPtr<ISharedPreferencesEditor> editor;
    sp->Edit((ISharedPreferencesEditor**)&editor);

    Int64 id;
    GetSubId(&id);
    StringBuilder sb(VM_NUMBER); sb += id;
    editor->PutString(sb.ToString(), number);
    editor->Apply();
    String str;
    GetSubscriberId(&str);
    SetSimImsi(str);
}

String CGSMPhone::GetSimImsi()
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&sp);
    Int64 id;
    GetSubId(&id);
    String str;
    StringBuilder sb(SIM_IMSI); sb += id;
    sp->GetString(sb.ToString(), String(NULL), &str);
    return str;
}

void CGSMPhone::SetSimImsi(
    /* [in] */ const String& imsi)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&sp);
    AutoPtr<ISharedPreferencesEditor> editor;

    sp->Edit((ISharedPreferencesEditor**)&editor);
    Int64 id;
    GetSubId(&id);
    StringBuilder sb(SIM_IMSI); sb += id;
    editor->PutString(sb.ToString(), imsi);
    editor->Apply();
}

Boolean CGSMPhone::IsValidCommandInterfaceCFReason (
    /* [in] */ Int32 commandInterfaceCFReason)
{
    switch (commandInterfaceCFReason) {
    case ICommandsInterface::CF_REASON_UNCONDITIONAL:
    case ICommandsInterface::CF_REASON_BUSY:
    case ICommandsInterface::CF_REASON_NO_REPLY:
    case ICommandsInterface::CF_REASON_NOT_REACHABLE:
    case ICommandsInterface::CF_REASON_ALL:
    case ICommandsInterface::CF_REASON_ALL_CONDITIONAL:
        return TRUE;
    default:
        return FALSE;
    }
}

Boolean CGSMPhone::IsValidCommandInterfaceCFAction (
    /* [in] */ Int32 commandInterfaceCFAction)
{
    switch (commandInterfaceCFAction) {
    case ICommandsInterface::CF_ACTION_DISABLE:
    case ICommandsInterface::CF_ACTION_ENABLE:
    case ICommandsInterface::CF_ACTION_REGISTRATION:
    case ICommandsInterface::CF_ACTION_ERASURE:
        return TRUE;
    default:
        return FALSE;
    }
}

void CGSMPhone::UpdateCallForwardStatus()
{
    if (LOCAL_DEBUG) Logger::D(TAG, "updateCallForwardStatus got sim records");
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

    Boolean b;
    if (r != NULL && r->IsCallForwardStatusStored(&b)) {
        // The Sim card has the CF info
        if (LOCAL_DEBUG) Logger::D(TAG, "Callforwarding info is present on sim");
        NotifyCallForwardingIndicator();
    }
    else {
        AutoPtr<IMessage> msg;
        ObtainMessage(CHECK_CALLFORWARDING_STATUS, (IMessage**)&msg);
        Boolean b;
        SendMessage(msg, &b);
    }
}

void CGSMPhone::OnNetworkInitiatedUssd(
    /* [in] */ IGsmMmiCode* mmi)
{
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, mmi, NULL);
    mMmiCompleteRegistrants->NotifyRegistrants(ar);
}

void CGSMPhone::OnIncomingUSSD (
    /* [in] */ Int32 ussdMode,
    /* [in] */ const String& ussdMessage)
{
    Boolean isUssdError;
    Boolean isUssdRequest;

    isUssdRequest
        = (ussdMode == ICommandsInterface::USSD_MODE_REQUEST);

    isUssdError
        = (ussdMode != ICommandsInterface::USSD_MODE_NOTIFY
            && ussdMode != ICommandsInterface::USSD_MODE_REQUEST);

    // See comments in GsmMmiCode.java
    // USSD requests aren't finished until one
    // of these two events happen
    AutoPtr<IGsmMmiCode> found;
    Int32 size;
    mPendingMMIs->GetSize(&size);
    for (Int32 i = 0, s = size ; i < s; i++) {
        AutoPtr<IInterface> obj;
        mPendingMMIs->Get(i, (IInterface**)&obj);
        Boolean b;
        IGsmMmiCode::Probe(obj)->IsPendingUSSD(&b);
        if(b) {
            found = IGsmMmiCode::Probe(obj);
            break;
        }
    }

    if (found != NULL) {
        // Complete pending USSD

        if (isUssdError) {
            ((CGsmMmiCode*)found.Get())->OnUssdFinishedError();
        }
        else {
            ((CGsmMmiCode*)found.Get())->OnUssdFinished(ussdMessage, isUssdRequest);
        }
    }
    else { // pending USSD not found
        // The network may initiate its own USSD request

        // ignore everything that isnt a Notify or a Request
        // also, discard if there is no message to present
        if (!isUssdError && ussdMessage != NULL) {
            AutoPtr<IInterface> obj;
            mUiccApplication->Get((IInterface**)&obj);
            AutoPtr<IUiccCardApplication> app = IUiccCardApplication::Probe(obj);
            AutoPtr<IGsmMmiCode> mmi;
            CGsmMmiCode::NewNetworkInitiatedUssd(ussdMessage,
                                               isUssdRequest,
                                               IGSMPhone::Probe(this),
                                               app, (IGsmMmiCode**)&mmi);
            OnNetworkInitiatedUssd(mmi);
        }
    }
}

void CGSMPhone::ProcessIccRecordEvents(
    /* [in] */ Int32 eventCode)
{
    switch (eventCode) {
        case IIccRecords::EVENT_CFI:
            NotifyCallForwardingIndicator();
            break;
    }
}

void CGSMPhone::HandleCfuQueryResult(
    /* [in] */ ArrayOf<ICallForwardInfo*>* infos)
{
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

    if (r != NULL) {
        if (infos == NULL || infos->GetLength() == 0) {
            // Assume the default is not active
            // Set unconditional CFF in SIM to FALSE
            r->SetVoiceCallForwardingFlag(1, FALSE, String(NULL));
        }
        else {
            for (Int32 i = 0, s = infos->GetLength(); i < s; i++) {
                AutoPtr<CallForwardInfo> cfi = (CallForwardInfo*)((*infos)[i]);
                if ((cfi->mServiceClass & ICommandsInterface::SERVICE_CLASS_VOICE) != 0) {
                    SetCallForwardingPreference(cfi->mStatus == 1);
                    r->SetVoiceCallForwardingFlag(1, (cfi->mStatus == 1),
                        cfi->mNumber);
                    // should only have the one
                    break;
                }
            }
        }
    }
}

void CGSMPhone::RegisterForSimRecordEvents()
{
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

    if (r == NULL) {
        return;
    }
    r->RegisterForNetworkSelectionModeAutomatic(
            this, EVENT_SET_NETWORK_AUTOMATIC, NULL);
    r->RegisterForRecordsEvents(this, EVENT_ICC_RECORD_EVENTS, NULL);
    r->RegisterForRecordsLoaded(this, EVENT_SIM_RECORDS_LOADED, NULL);
}

void CGSMPhone::UnregisterForSimRecordEvents()
{
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);

    if (r == NULL) {
        return;
    }
    r->UnregisterForNetworkSelectionModeAutomatic(this);
    r->UnregisterForRecordsEvents(this);
    r->UnregisterForRecordsLoaded(this);
}

Int32 CGSMPhone::GetStoredVoiceMessageCount()
{
    Int32 countVoiceMessages = 0;
    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(mContext, (ISharedPreferences**)&sp);
    Int64 id;
    GetSubId(&id);
    String imsi;
    StringBuilder sb(VM_ID); sb += id;
    sp->GetString(sb.ToString(), String(NULL), &imsi);
    String currentImsi;
    GetSubscriberId(&currentImsi);

    Logger::D(TAG, "Voicemail count retrieval for Imsi = %s current Imsi = %s",
            imsi.string(), currentImsi.string() );

    if ((imsi != NULL) && (currentImsi != NULL)
            && (currentImsi.Equals(imsi))) {
        StringBuilder sb(VM_COUNT); sb += id;
        // get voice mail count from preferences
        sp->GetInt32(sb.ToString(), 0, &countVoiceMessages);
        Logger::D(TAG, "Voice Mail Count from preference = %d", countVoiceMessages );
    }
    return countVoiceMessages;
}

Boolean CGSMPhone::IsValidFacilityString(
    /* [in] */ const String& facility)
{
    if ((facility.Equals(ICommandsInterface::CB_FACILITY_BAOC))
            || (facility.Equals(ICommandsInterface::CB_FACILITY_BAOIC))
            || (facility.Equals(ICommandsInterface::CB_FACILITY_BAOICxH))
            || (facility.Equals(ICommandsInterface::CB_FACILITY_BAIC))
            || (facility.Equals(ICommandsInterface::CB_FACILITY_BAICr))
            || (facility.Equals(ICommandsInterface::CB_FACILITY_BA_ALL))
            || (facility.Equals(ICommandsInterface::CB_FACILITY_BA_MO))
            || (facility.Equals(ICommandsInterface::CB_FACILITY_BA_MT))
            || (facility.Equals(ICommandsInterface::CB_FACILITY_BA_SIM))
            || (facility.Equals(ICommandsInterface::CB_FACILITY_BA_FD))) {
        return TRUE;
    }
    Logger::E(TAG, " Invalid facility String : %s", facility.string());
    return FALSE;
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
