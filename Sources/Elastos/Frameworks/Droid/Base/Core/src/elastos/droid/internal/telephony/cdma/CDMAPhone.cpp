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

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/internal/telephony/cdma/CCdmaServiceStateTracker.h"
#include "elastos/droid/internal/telephony/cdma/CCdmaCallTracker.h"
#include "elastos/droid/internal/telephony/cdma/CdmaCallTracker.h"
#include "elastos/droid/internal/telephony/cdma/CdmaMmiCode.h"
#include "elastos/droid/internal/telephony/cdma/CDMAPhone.h"
#include "elastos/droid/internal/telephony/cdma/CdmaSubscriptionSourceManager.h"
#include "elastos/droid/internal/telephony/cdma/CEriManager.h"
#include "elastos/droid/internal/telephony/cdma/CRuimPhoneBookInterfaceManager.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcTracker.h"
#include "elastos/droid/internal/telephony/CPhoneFactory.h"
#include "elastos/droid/internal/telephony/CPhoneSubInfo.h"
#include "elastos/droid/internal/telephony/MccTable.h"
#include "elastos/droid/internal/telephony/PhoneProxy.h"
#include "elastos/droid/internal/telephony/SubscriptionController.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/preference/PreferenceManager.h"
#include "elastos/droid/provider/CTelephonyCarriers.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include "elastos/droid/telephony/cdma/CCdmaCellLocation.h"
#include "elastos/droid/telephony/CServiceState.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Internal::Telephony::Cdma::CdmaSubscriptionSourceManager;
using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Droid::Internal::Telephony::CPhoneSubInfo;
using Elastos::Droid::Internal::Telephony::DataConnection::CDcTracker;
using Elastos::Droid::Internal::Telephony::DataConnection::IDcTracker;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::MccTable;
using Elastos::Droid::Internal::Telephony::PhoneProxy;
using Elastos::Droid::Internal::Telephony::SubscriptionController;
using Elastos::Droid::Internal::Telephony::Uicc::IIccException;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Preference::PreferenceManager;
using Elastos::Droid::Provider::CTelephonyCarriers;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ITelephonyCarriers;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::Cdma::CCdmaCellLocation;
using Elastos::Droid::Telephony::Cdma::ICdmaCellLocation;
using Elastos::Droid::Telephony::CServiceState;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IPatternHelper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                       CDMAPhone::InnerRunnable
//=====================================================================
CDMAPhone::InnerRunnable::InnerRunnable(
    /* [in] */ CDMAPhone* owner)
    : mOwner(owner)
{
}

ECode CDMAPhone::InnerRunnable::Run()
{
    return mOwner->ExitEmergencyCallbackMode();
}

//=====================================================================
//                              CDMAPhone
//=====================================================================
CAR_INTERFACE_IMPL(CDMAPhone, PhoneBase, ICDMAPhone);

const String CDMAPhone::LOGTAG("CDMAPhone");
const Int32 CDMAPhone::RESTART_ECM_TIMER;
const Int32 CDMAPhone::CANCEL_ECM_TIMER;
const Boolean CDMAPhone::DBG = TRUE;
const Boolean CDMAPhone::VDBG = FALSE;
const Int32 CDMAPhone::DEFAULT_ECM_EXIT_TIMER_VALUE;
const String CDMAPhone::VM_NUMBER_CDMA("vm_number_key_cdma");
const String CDMAPhone::IS683A_FEATURE_CODE("*228");
const Int32 CDMAPhone::IS683A_FEATURE_CODE_NUM_DIGITS;
const Int32 CDMAPhone::IS683A_SYS_SEL_CODE_NUM_DIGITS;
const Int32 CDMAPhone::IS683A_SYS_SEL_CODE_OFFSET;
const Int32 CDMAPhone::IS683_CONST_800MHZ_A_BAND;
const Int32 CDMAPhone::IS683_CONST_800MHZ_B_BAND;
const Int32 CDMAPhone::IS683_CONST_1900MHZ_A_BLOCK;
const Int32 CDMAPhone::IS683_CONST_1900MHZ_B_BLOCK;
const Int32 CDMAPhone::IS683_CONST_1900MHZ_C_BLOCK;
const Int32 CDMAPhone::IS683_CONST_1900MHZ_D_BLOCK;
const Int32 CDMAPhone::IS683_CONST_1900MHZ_E_BLOCK;
const Int32 CDMAPhone::IS683_CONST_1900MHZ_F_BLOCK;
const Int32 CDMAPhone::INVALID_SYSTEM_SELECTION_CODE;

static AutoPtr<IPattern> InitPattern(
    /* [in] */ const String& regex)
{
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);

    AutoPtr<IPattern> pattern;
    helper->Compile(regex, (IPattern**)&pattern);
    return pattern;
}
AutoPtr<IPattern> CDMAPhone::pOtaSpNumSchema = InitPattern(String("[,\\s]+"));

CDMAPhone::CDMAPhone()
    : mCdmaSubscriptionSource(ICdmaSubscriptionSourceManager::SUBSCRIPTION_SOURCE_UNKNOWN)
    , mIsPhoneInEcmState(FALSE)
{
    CArrayList::New((IArrayList**)&mPendingMmis);

    mEriFileLoadedRegistrants = new RegistrantList();
    mEcmTimerResetRegistrants = new RegistrantList();

    mExitEcmRunnable = new InnerRunnable(this);
}

CDMAPhone::~CDMAPhone()
{
    if(DBG) Logger::D(LOGTAG, "CDMAPhone finalized");
    Boolean b;
    if (mWakeLock->IsHeld(&b), b) {
        Logger::E(LOGTAG, "UNEXPECTED; mWakeLock is held when finalizing.");
        mWakeLock->ReleaseLock();
    }
}

ECode CDMAPhone::constructor()
{
    return NOERROR;
}

ECode CDMAPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier)
{
    PhoneBase::constructor(String("CDMA"), notifier, context, ci, FALSE);
    InitSstIcc();
    Init(context, notifier);
    return NOERROR;
}

ECode CDMAPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ Int32 phoneId)
{
    PhoneBase::constructor(String("CDMA"), notifier, context, ci, FALSE, phoneId);
    InitSstIcc();
    Init(context, notifier);
    return NOERROR;
}

ECode CDMAPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ Boolean unitTestMode)
{
    PhoneBase::constructor(String("CDMA"), notifier, context, ci, unitTestMode);
    InitSstIcc();
    Init(context, notifier);
    return NOERROR;
}

ECode CDMAPhone::Dispose()
{
    AutoLock lock(PhoneProxy::lockForRadioTechnologyChange);
    PhoneBase::Dispose();
    Log(String("dispose"));

    //Unregister from all former registered events
    UnregisterForRuimRecordEvents();
    mCi->UnregisterForAvailable(this); //EVENT_RADIO_AVAILABLE
    mCi->UnregisterForOffOrNotAvailable(this); //EVENT_RADIO_OFF_OR_NOT_AVAILABLE
    mCi->UnregisterForOn(this); //EVENT_RADIO_ON
    IServiceStateTracker::Probe(mSST)->UnregisterForNetworkAttached(this); //EVENT_REGISTERED_TO_NETWORK
    mCi->UnSetOnSuppServiceNotification(this);
    mCi->UnregisterForExitEmergencyCallbackMode(this);
    RemoveCallbacks(mExitEcmRunnable);

    mPendingMmis->Clear();

    //Force all referenced classes to unregister their former registered events
    mCT->Dispose();
    mDcTracker->Dispose();
    IServiceStateTracker::Probe(mSST)->Dispose();
    mCdmaSSM->Dispose(this);
    IIccPhoneBookInterfaceManager::Probe(mRuimPhoneBookInterfaceManager)->Dispose();
    mSubInfo->Dispose();
    mEriManager->Dispose();

    return NOERROR;
}

ECode CDMAPhone::RemoveReferences()
{
    Log(String("removeReferences"));
    mRuimPhoneBookInterfaceManager = NULL;
    mSubInfo = NULL;
    mCT = NULL;
    mSST = NULL;
    mEriManager = NULL;
    mExitEcmRunnable = NULL;

    PhoneBase::RemoveReferences();
    return NOERROR;
}

ECode CDMAPhone::GetServiceState(
    /* [out] */ IServiceState** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 state;
    ((CdmaServiceStateTracker*)mSST.Get())->mSS->GetState(&state);
    if ((mSST == NULL) || state != IServiceState::STATE_IN_SERVICE) {
        if (mImsPhone != NULL) {
            AutoPtr<IServiceState> ss;
            IPhone::Probe(mImsPhone)->GetServiceState((IServiceState**)&ss);
            ss->GetState(&state);
            if (state == IServiceState::STATE_IN_SERVICE) {
                return IPhone::Probe(mImsPhone)->GetServiceState(result);
            }
        }
    }

    if (mSST != NULL) {
        *result = ((CdmaServiceStateTracker*)mSST.Get())->mSS;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        // avoid potential NPE in EmergencyCallHelper during Phone switch
        return CServiceState::New(result);
    }
    return NOERROR;
}

ECode CDMAPhone::GetCallTracker(
    /* [out] */ ICallTracker** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ICallTracker::Probe(mCT);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDMAPhone::GetState(
    /* [out] */ PhoneConstantsState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((CdmaCallTracker*)mCT.Get())->mState;
    return NOERROR;
}

ECode CDMAPhone::GetServiceStateTracker(
    /* [out] */ IServiceStateTracker** result)
{
    VALIDATE_NOT_NULL(result);
    *result = IServiceStateTracker::Probe(mSST);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDMAPhone::GetPhoneType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPhoneConstants::PHONE_TYPE_CDMA;
    return NOERROR;
}

ECode CDMAPhone::CanTransfer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::E(LOGTAG, "canTransfer: not possible in CDMA");
    *result = FALSE;
    return NOERROR;
}

ECode CDMAPhone::GetRingingCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IImsPhone> imPhone = mImsPhone;
    Boolean b;
    if ( ((CdmaCallTracker*)mCT.Get())->mRingingCall != NULL
        && (ICall::Probe(((CdmaCallTracker*)mCT.Get())->mRingingCall)->IsRinging(&b), b) ) {
        *result = ICall::Probe(((CdmaCallTracker*)mCT.Get())->mRingingCall);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if ( imPhone != NULL ) {
        return IPhone::Probe(imPhone)->GetRingingCall(result);
    }
    *result = ICall::Probe(((CdmaCallTracker*)mCT.Get())->mRingingCall);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDMAPhone::SetUiTTYMode(
    /* [in] */ Int32 uiTtyMode,
    /* [in] */ IMessage* onComplete)
{
    VALIDATE_NOT_NULL(onComplete);
    if (mImsPhone != NULL) {
        IPhone::Probe(mImsPhone)->SetUiTTYMode(uiTtyMode, onComplete);
    }
    return NOERROR;
}

ECode CDMAPhone::SetMute(
    /* [in] */ Boolean muted)
{
    return ((CdmaCallTracker*)mCT.Get())->SetMute(muted);
}

ECode CDMAPhone::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return ((CdmaCallTracker*)mCT.Get())->GetMute(result);
}

ECode CDMAPhone::Conference()
{
    Boolean b;
    if (mImsPhone != NULL && (IPhone::Probe(mImsPhone)->CanConference(&b), b)) {
        Log(String("conference() - delegated to IMS phone"));
        return IPhone::Probe(mImsPhone)->Conference();
    }
    // three way calls in CDMA will be handled by feature codes
    Logger::E(LOGTAG, "conference: not possible in CDMA");
    return NOERROR;
}

ECode CDMAPhone::EnableEnhancedVoicePrivacy(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    VALIDATE_NOT_NULL(onComplete);
    return mCi->SetPreferredVoicePrivacy(enable, onComplete);
}

ECode CDMAPhone::GetEnhancedVoicePrivacy(
    /* [in] */ IMessage* onComplete)
{
    VALIDATE_NOT_NULL(onComplete);
    return mCi->GetPreferredVoicePrivacy(onComplete);
}

ECode CDMAPhone::ClearDisconnected()
{
    return ((CdmaCallTracker*)mCT.Get())->ClearDisconnected();
}

ECode CDMAPhone::GetDataActivityState(
    /* [out] */ IPhoneDataActivityState* result)
{
    VALIDATE_NOT_NULL(result);
    IPhoneDataActivityState ret = IPhoneDataActivityState_NONE;

    Int32 state;
    IServiceStateTracker::Probe(mSST)->GetCurrentDataConnectionState(&state);
    if (state == IServiceState::STATE_IN_SERVICE) {

        DctConstantsActivity val;
        mDcTracker->GetActivity(&val);
        switch (val) {
            case DctConstantsActivity_DATAIN:
                ret = IPhoneDataActivityState_DATAIN;
            break;

            case DctConstantsActivity_DATAOUT:
                ret = IPhoneDataActivityState_DATAOUT;
            break;

            case DctConstantsActivity_DATAINANDOUT:
                ret = IPhoneDataActivityState_DATAINANDOUT;
            break;

            case DctConstantsActivity_DORMANT:
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

ECode CDMAPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    return Dial(dialString, videoState, NULL, result);
}

ECode CDMAPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IImsPhone> imsPhone = mImsPhone;

    Boolean imsUseEnabled = FALSE;
// TODO: Need ImsManager
    // imsUseEnabled =
    //         ImsManager.isEnhanced4gLteModeSettingEnabledByPlatform(mContext) &&
    //         ImsManager.isEnhanced4gLteModeSettingEnabledByUser(mContext);
    if (!imsUseEnabled) {
        Logger::W(LOGTAG, "IMS is disabled: forced to CS");
    }

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    if (imsUseEnabled && imsPhone != NULL) {
        AutoPtr<IServiceState> ss;
        IPhone::Probe(imsPhone)->GetServiceState((IServiceState**)&ss);
        Int32 state;
        ss->GetState(&state);
        Boolean b1, b2, b3;
        if ((state == IServiceState::STATE_IN_SERVICE
            && (PhoneNumberUtils::IsEmergencyNumber(dialString, &b1), !b1))
            || ((PhoneNumberUtils::IsEmergencyNumber(dialString, &b2), b2)
            && (res->GetBoolean(R::bool_::useImsAlwaysForEmergencyCall, &b3), b3))) {
            // try {
            if (DBG) Logger::D(LOGTAG, "Trying IMS PS call");
            return IPhone::Probe(imsPhone)->Dial(dialString, videoState, extras, result);
            // } catch (CallStateException e) {
            //     if (DBG) Logger::D(LOGTAG, "IMS PS call exception " + e);
            //     if (!ImsPhone.CS_FALLBACK.equals(e->GetMessage())) {
            //         CallStateException ce = new CallStateException(e->GetMessage());
            //         ce.setStackTrace(e->GetStackTrace());
            //         throw ce;
            //     }
            // }
        }
    }

    if (DBG) Logger::D(LOGTAG, "Trying (non-IMS) CS call");
    return DialInternal(dialString, NULL, videoState, result);
}

ECode CDMAPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(uusInfo);
    VALIDATE_NOT_NULL(extras);
    VALIDATE_NOT_NULL(result);
    return Dial(dialString, uusInfo, videoState, NULL, result);
}

ECode CDMAPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // throw new CallStateException("Sending UUS information NOT supported in CDMA!");
    return E_CALL_STATE_EXCEPTION;
}

ECode CDMAPhone::GetPendingMmiCodes(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = IList::Probe(mPendingMmis);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDMAPhone::RegisterForSuppServiceNotification(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    VALIDATE_NOT_NULL(h);
    VALIDATE_NOT_NULL(obj);
    Logger::E(LOGTAG, "method registerForSuppServiceNotification is NOT supported in CDMA!");
    return NOERROR;
}

ECode CDMAPhone::GetBackgroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ICall::Probe(((CdmaCallTracker*)mCT.Get())->mBackgroundCall);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDMAPhone::HandleInCallMmiCommands(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::E(LOGTAG, "method handleInCallMmiCommands is NOT supported in CDMA!");
    *result = FALSE;
    return NOERROR;
}

ECode CDMAPhone::IsInCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

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
    // *result = (foregroundCallState->IsAlive()
    //         || backgroundCallState->IsAlive()
    //         || ringingCallState->IsAlive());
    return NOERROR;
}

ECode CDMAPhone::SetNetworkSelectionModeAutomatic(
    /* [in] */ IMessage* response)
{
    VALIDATE_NOT_NULL(response);
    Logger::E(LOGTAG, "method setNetworkSelectionModeAutomatic is NOT supported in CDMA!");
    if (response != NULL) {
        Logger::E(LOGTAG,
                "setNetworkSelectionModeAutomatic: not possible in CDMA- Posting exception");
// TODO: Need CommandException
        // CommandException ce = new CommandException(
        //         CommandException.Error.REQUEST_NOT_SUPPORTED);
        // AsyncResult::ForMessage(response)->mException = ce;
        response->SendToTarget();
    }
    return NOERROR;
}

ECode CDMAPhone::UnregisterForSuppServiceNotification(
    /* [in] */ IHandler* h)
{
    VALIDATE_NOT_NULL(h);
    Logger::E(LOGTAG, "method unregisterForSuppServiceNotification is NOT supported in CDMA!");
    return NOERROR;
}

ECode CDMAPhone::AcceptCall(
    /* [in] */ Int32 videoState)
{
    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    AutoPtr<ICall> call;
    IPhone::Probe(imsPhone)->GetRingingCall((ICall**)&call);
    Boolean b;
    if ( imsPhone != NULL && (call->IsRinging(&b), b) ) {
        IPhone::Probe(imsPhone)->AcceptCall(videoState);
    }
    else {
        ((CdmaCallTracker*)mCT.Get())->AcceptCall();
    }
    return NOERROR;
}

ECode CDMAPhone::DeflectCall(
    /* [in] */ const String& number)
{
    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    AutoPtr<ICall> call;
    IPhone::Probe(imsPhone)->GetRingingCall((ICall**)&call);
    Boolean b;
    if ( imsPhone != NULL && (call->IsRinging(&b), b) ) {
        IPhone::Probe(imsPhone)->DeflectCall(number);
    }
    else {
        // throw new CallStateException("Deflect call NOT supported in CDMA!");
        return E_CALL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CDMAPhone::RejectCall()
{
    return ((CdmaCallTracker*)mCT.Get())->RejectCall();
}

ECode CDMAPhone::SwitchHoldingAndActive()
{
    return ((CdmaCallTracker*)mCT.Get())->SwitchWaitingOrHoldingAndActive();
}

ECode CDMAPhone::GetIccSerialNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
    if (r == NULL) {
        // to get ICCID form SIMRecords because it is on MF.
        mUiccController->GetIccRecords(mPhoneId, IUiccController::APP_FAM_3GPP, (IIccRecords**)&r);
    }

    String str;
    *result = (r != NULL) ? (r->GetIccId(&str), str) : String(NULL);
    return NOERROR;
}

ECode CDMAPhone::GetLine1Number(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mSST->GetMdnNumber(result);
}

ECode CDMAPhone::GetCdmaPrlVersion(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mSST->GetPrlVersion(result);
}

ECode CDMAPhone::GetCdmaMin(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mSST->GetCdmaMin(result);
}

ECode CDMAPhone::IsMinInfoReady(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mSST->IsMinInfoReady(result);
}

ECode CDMAPhone::GetCallWaiting(
    /* [in] */ IMessage* onComplete)
{
    VALIDATE_NOT_NULL(onComplete);
    return mCi->QueryCallWaiting(ICommandsInterface::SERVICE_CLASS_VOICE, onComplete);
}

ECode CDMAPhone::SetRadioPower(
    /* [in] */ Boolean power)
{
    return IServiceStateTracker::Probe(mSST)->SetRadioPower(power);
}

ECode CDMAPhone::GetEsn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEsn;
    return NOERROR;
}

ECode CDMAPhone::GetMeid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMeid;
    return NOERROR;
}

ECode CDMAPhone::GetDeviceId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String id;
    GetMeid(&id);
    Boolean b;
    if ((id.IsNull()) || (StringUtils::Matches(id, String("^0*$"), &b), b)) {
        Logger::D(LOGTAG, "getDeviceId(): MEID is not initialized use ESN");
        GetEsn(&id);
    }
    *result = id;
    return NOERROR;
}

ECode CDMAPhone::GetDeviceSvn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::D(LOGTAG, "getDeviceSvn(): return 0");
    *result = String("0");
    return NOERROR;
}

ECode CDMAPhone::GetSubscriberId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return ((CdmaServiceStateTracker*)mSST.Get())->GetImsi(result);
}

ECode CDMAPhone::GetGroupIdLevel1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::E(LOGTAG, "GID1 is not available in CDMA");
    *result = String(NULL);
    return NOERROR;
}

ECode CDMAPhone::GetImei(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::E(LOGTAG, "getImei() called for CDMAPhone");
    *result = mImei;
    return NOERROR;
}

ECode CDMAPhone::CanConference(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b;
    if (mImsPhone != NULL && (IPhone::Probe(mImsPhone)->CanConference(&b), b)) {
        *result = TRUE;
        return NOERROR;
    }
    Logger::E(LOGTAG, "canConference: not possible in CDMA");
    *result = FALSE;
    return NOERROR;
}

ECode CDMAPhone::GetCellLocation(
    /* [out] */ ICellLocation** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICdmaCellLocation> loc = ((CdmaServiceStateTracker*)mSST.Get())->mCellLoc;

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    Int32 mode;
    Settings::Secure::GetInt32(cr,
            ISettingsSecure::LOCATION_MODE, ISettingsSecure::LOCATION_MODE_OFF, &mode);
    if (mode == ISettingsSecure::LOCATION_MODE_OFF) {
        // clear lat/long values for location privacy
        AutoPtr<ICdmaCellLocation> privateLoc;
        CCdmaCellLocation::New((ICdmaCellLocation**)&privateLoc);
        Int32 stationId;
        loc->GetBaseStationId(&stationId);
        Int32 systemId;
        loc->GetSystemId(&systemId);
        Int32 networkId;
        loc->GetNetworkId(&networkId);
        privateLoc->SetCellLocationData(stationId,
                ICdmaCellLocation::INVALID_LAT_LONG,
                ICdmaCellLocation::INVALID_LAT_LONG,
                systemId, networkId);
        loc = privateLoc;
    }
    *result = ICellLocation::Probe(loc);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDMAPhone::GetForegroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ICall::Probe(((CdmaCallTracker*)mCT.Get())->mForegroundCall);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDMAPhone::SelectNetworkManually(
    /* [in] */ IOperatorInfo* network,
    /* [in] */ IMessage* response)
{
    Logger::E(LOGTAG, "selectNetworkManually: not possible in CDMA");
    if (response != NULL) {
// TODO: Need CommandException
        // CommandException ce = new CommandException(
        //         CommandException.Error.REQUEST_NOT_SUPPORTED);
        // AsyncResult::ForMessage(response)->mException = ce;
        response->SendToTarget();
    }
    return NOERROR;
}

ECode CDMAPhone::SetOnPostDialCharacter(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return CRegistrant::New(h, what, obj, (IRegistrant**)&mPostDialHandler);
}

ECode CDMAPhone::HandlePinMmi(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mUiccApplication->Get((IInterface**)&obj);

    AutoPtr<ICdmaMmiCode> mmi = CdmaMmiCode::NewFromDialString(
            dialString, this, IUiccCardApplication::Probe(obj));

    Boolean b;
    if (mmi == NULL) {
        Logger::E(LOGTAG, "Mmi is NULL!");
        *result = FALSE;
        return NOERROR;
    }
    else if (((CdmaMmiCode*)mmi.Get())->IsPinPukCommand(&b), b) {
        mPendingMmis->Add(mmi);
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, mmi, NULL);
        mMmiRegistrants->NotifyRegistrants(ar);
        ((CdmaMmiCode*)mmi.Get())->ProcessCode();
        *result = TRUE;
        return NOERROR;
    }
    Logger::E(LOGTAG, "Unrecognized mmi!");
    *result = FALSE;
    return NOERROR;
}

ECode CDMAPhone::OnMMIDone(
    /* [in] */ ICdmaMmiCode* mmi)
{
    /*
     * Only notify complete if it's on the pending list. Otherwise, it's
     * already been handled (eg, previously canceled).
     */
    Boolean b;
    mPendingMmis->Remove(mmi, &b);
    if (b) {
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, mmi, NULL);
        mMmiCompleteRegistrants->NotifyRegistrants(ar);
    }
    return NOERROR;
}

ECode CDMAPhone::SetLine1Number(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ IMessage* onComplete)
{
    Logger::E(LOGTAG, "setLine1Number: not possible in CDMA");
    return NOERROR;
}

ECode CDMAPhone::SetCallWaiting(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    Logger::E(LOGTAG, "method setCallWaiting is NOT supported in CDMA!");
    return NOERROR;
}

ECode CDMAPhone::UpdateServiceLocation()
{
    return IServiceStateTracker::Probe(mSST)->EnableSingleLocationUpdate();
}

ECode CDMAPhone::SetDataRoamingEnabled(
    /* [in] */ Boolean enable)
{
    return mDcTracker->SetDataOnRoamingEnabled(enable);
}

ECode CDMAPhone::RegisterForCdmaOtaStatusChange(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mCi->RegisterForCdmaOtaProvision(h, what, obj);
}

ECode CDMAPhone::UnregisterForCdmaOtaStatusChange(
    /* [in] */ IHandler* h)
{
    return mCi->UnregisterForCdmaOtaProvision(h);
}

ECode CDMAPhone::RegisterForSubscriptionInfoReady(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mSST->RegisterForSubscriptionInfoReady(h, what, obj);
}

ECode CDMAPhone::UnregisterForSubscriptionInfoReady(
    /* [in] */ IHandler* h)
{
    return mSST->UnregisterForSubscriptionInfoReady(h);
}

ECode CDMAPhone::SetOnEcbModeExitResponse(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return CRegistrant::New(h, what, obj, (IRegistrant**)&mEcmExitRespRegistrant);
}

ECode CDMAPhone::UnsetOnEcbModeExitResponse(
    /* [in] */ IHandler* h)
{
    return mEcmExitRespRegistrant->Clear();
}

ECode CDMAPhone::RegisterForCallWaiting(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mCT->RegisterForCallWaiting(h, what, obj);
}

ECode CDMAPhone::UnregisterForCallWaiting(
    /* [in] */ IHandler* h)
{
    return mCT->UnregisterForCallWaiting(h);
}

ECode CDMAPhone::GetNeighboringCids(
    /* [in] */ IMessage* response)
{
    /*
     * This is currently not implemented.  At least as of June
     * 2009, there is no neighbor cell information available for
     * CDMA because some party is resisting making this
     * information readily available.  Consequently, calling this
     * function can have no useful effect.  This situation may
     * (and hopefully will) change in the future.
     */
    if (response != NULL) {
// TODO: Need CommandException
        // CommandException ce = new CommandException(
        //         CommandException.Error.REQUEST_NOT_SUPPORTED);
        // AsyncResult::ForMessage(response)->mException = ce;
        response->SendToTarget();
    }
    return NOERROR;
}

ECode CDMAPhone::GetDataConnectionState(
    /* [in] */ const String& apnType,
    /* [out] */ PhoneConstantsState* result)
{
    VALIDATE_NOT_NULL(result);
    PhoneConstantsDataState ret = PhoneConstantsDataState_DISCONNECTED;

    Int32 s;
    Boolean b1, b2;
    if (mSST == NULL) {
         // Radio Technology Change is ongoning, dispose() and removeReferences() have
         // already been called

         ret = PhoneConstantsDataState_DISCONNECTED;
    }
    else if ((IServiceStateTracker::Probe(mSST)->GetCurrentDataConnectionState(&s), s)
            != IServiceState::STATE_IN_SERVICE && mOosIsDisconnect) {
        // If we're out of service, open TCP sockets may still work
        // but no data will flow
        ret = PhoneConstantsDataState_DISCONNECTED;
        Log(String("getDataConnectionState: Data is Out of Service. ret = ")
                + StringUtils::ToString(ret));
    }
    else if ((mDcTracker->IsApnTypeEnabled(apnType, &b1), b1) == FALSE ||
            (mDcTracker->IsApnTypeActive(apnType, &b2), b2) == FALSE) {
        ret = PhoneConstantsDataState_DISCONNECTED;
    }
    else {
        DctConstantsState state;
        mDcTracker->GetState(apnType, &state);
        switch (state) {
            case DctConstantsState_RETRYING:
            case DctConstantsState_FAILED:
            case DctConstantsState_IDLE:
                ret = PhoneConstantsDataState_DISCONNECTED;
            break;

            case DctConstantsState_CONNECTED:
            case DctConstantsState_DISCONNECTING:
                if ( ((CdmaCallTracker*)mCT.Get())->mState != PhoneConstantsState_IDLE
                        && (IServiceStateTracker::Probe(mSST)->IsConcurrentVoiceAndDataAllowed(&b1), b1)) {
                    ret = PhoneConstantsDataState_SUSPENDED;
                }
                else {
                    ret = PhoneConstantsDataState_CONNECTED;
                }
            break;

            case DctConstantsState_CONNECTING:
            case DctConstantsState_SCANNING:
                ret = PhoneConstantsDataState_CONNECTING;
            break;
        }
    }

    Log(String("getDataConnectionState apnType=") + apnType + " ret=" + StringUtils::ToString(ret));
    *result = ret;
    return NOERROR;
}

ECode CDMAPhone::SendUssdResponse(
    /* [in] */ const String& ussdMessge)
{
    Logger::E(LOGTAG, "sendUssdResponse: not possible in CDMA");
    return NOERROR;
}

ECode CDMAPhone::SendDtmf(
    /* [in] */ Char32 c)
{
    Boolean b;
    if (PhoneNumberUtils::Is12Key(c, &b), !b) {
        Logger::E(LOGTAG,
                "sendDtmf called with invalid character '%d'", c);
    }
    else {
        if (((CdmaCallTracker*)mCT.Get())->mState ==  PhoneConstantsState_OFFHOOK) {
            mCi->SendDtmf(c, NULL);
        }
    }
    return NOERROR;
}

ECode CDMAPhone::StartDtmf(
    /* [in] */ Char32 c)
{
    Boolean b;
    if (PhoneNumberUtils::Is12Key(c, &b), !b) {
        Logger::E(LOGTAG,
                "startDtmf called with invalid character '%d'", c);
    }
    else {
        mCi->StartDtmf(c, NULL);
    }
    return NOERROR;
}

ECode CDMAPhone::StopDtmf()
{
    return mCi->StopDtmf(NULL);
}

ECode CDMAPhone::SendBurstDtmf(
    /* [in] */ const String& dtmfString,
    /* [in] */ Int32 on,
    /* [in] */ Int32 off,
    /* [in] */ IMessage* onComplete)
{
    Boolean check = TRUE;
    for (Int32 itr = 0; itr < dtmfString.GetLength(); itr++) {
        Boolean b;
        if (PhoneNumberUtils::Is12Key(dtmfString.GetChar(itr), &b), !b) {
            Logger::E(LOGTAG,
                    "sendDtmf called with invalid character '%d'", dtmfString.GetChar(itr));
            check = FALSE;
            break;
        }
    }
    if ((((CdmaCallTracker*)mCT.Get())->mState ==  PhoneConstantsState_OFFHOOK)&&(check)) {
        mCi->SendBurstDtmf(dtmfString, on, off, onComplete);
    }
    return NOERROR;
}

ECode CDMAPhone::GetAvailableNetworks(
    /* [in] */ IMessage* response)
{
    Logger::E(LOGTAG, "getAvailableNetworks: not possible in CDMA");
    if (response != NULL) {
// TODO: Need CommandException
        // CommandException ce = new CommandException(
        //         CommandException.Error.REQUEST_NOT_SUPPORTED);
        // AsyncResult::ForMessage(response)->mException = ce;
        response->SendToTarget();
    }
    return NOERROR;
}

ECode CDMAPhone::SetOutgoingCallerIdDisplay(
    /* [in] */ Int32 commandInterfaceCLIRMode,
    /* [in] */ IMessage* onComplete)
{
    Logger::E(LOGTAG, "setOutgoingCallerIdDisplay: not possible in CDMA");
    return NOERROR;
}

ECode CDMAPhone::EnableLocationUpdates()
{
    return IServiceStateTracker::Probe(mSST)->EnableLocationUpdates();
}

ECode CDMAPhone::DisableLocationUpdates()
{
    return IServiceStateTracker::Probe(mSST)->DisableLocationUpdates();
}

ECode CDMAPhone::GetDataCallList(
    /* [in] */ IMessage* response)
{
    VALIDATE_NOT_NULL(response);
    return mCi->GetDataCallList(response);
}

ECode CDMAPhone::GetDataRoamingEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mDcTracker->GetDataOnRoamingEnabled(result);
}

ECode CDMAPhone::SetDataEnabled(
    /* [in] */ Boolean enable)
{
    return mDcTracker->SetDataEnabled(enable);
}

ECode CDMAPhone::GetDataEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mDcTracker->GetDataEnabled(result);
}

ECode CDMAPhone::SetVoiceMailNumber(
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

ECode CDMAPhone::GetVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String number;
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&sp);
    Int64 sid;
    GetSubId(&sid);
    sp->GetString(VM_NUMBER_CDMA + StringUtils::ToString(sid), String(NULL), &number);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    if (TextUtils::IsEmpty(number)) {
        AutoPtr<ArrayOf<String> > listArray;
        res->GetStringArray(R::array::config_default_vm_number, (ArrayOf<String>**)&listArray);
        if (listArray != NULL && listArray->GetLength() > 0) {
            for (Int32 i=0; i<listArray->GetLength(); i++) {
                if (!TextUtils::IsEmpty((*listArray)[i])) {
                    AutoPtr<ArrayOf<String> > defaultVMNumberArray;
                    StringUtils::Split((*listArray)[i], ";",
                            (ArrayOf<String>**)&defaultVMNumberArray);
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
    if (TextUtils::IsEmpty(number)) {
        // Read platform settings for dynamic voicemail number
        Boolean b;
        if (res->GetBoolean(R::bool_::config_telephony_use_own_number_for_voicemail, &b), b) {
            GetLine1Number(&number);
        }
        else {
            number = String("*86");
        }
    }
    *result = number;
    return NOERROR;
}

ECode CDMAPhone::GetVoiceMailAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // TODO: Where can we get this value has to be clarified with QC.
    String ret = String("");//TODO: Remove = "", if we know where to get this value.

    //ret = mSIMRecords->GetVoiceMailAlphaTag();

    if (ret == NULL || ret.GetLength() == 0) {
        AutoPtr<ICharSequence> cs;
        mContext->GetText(
            R::string::defaultVoiceMailAlphaTag, (ICharSequence**)&cs);
        cs->ToString(&ret);
    }

    *result = ret;
    return NOERROR;
}

ECode CDMAPhone::GetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ IMessage* onComplete)
{
    Logger::E(LOGTAG, "getCallForwardingOption: not possible in CDMA");
    return NOERROR;
}

ECode CDMAPhone::SetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ const String& dialingNumber,
    /* [in] */ Int32 timerSeconds,
    /* [in] */ IMessage* onComplete)
{
    Logger::E(LOGTAG, "setCallForwardingOption: not possible in CDMA");
    return NOERROR;
}

ECode CDMAPhone::GetCallForwardingUncondTimerOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ IMessage* onComplete)
{
    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    if (imsPhone != NULL) {
        IPhone::Probe(imsPhone)->GetCallForwardingOption(commandInterfaceCFReason, onComplete);
    }
    else {
        if (onComplete != NULL) {
// TODO: Need CommandException
            // AsyncResult::ForMessage(onComplete, NULL,
            //         new CommandException(CommandException.Error.GENERIC_FAILURE));
            onComplete->SendToTarget();
        }
    }
    return NOERROR;
}

ECode CDMAPhone::SetCallForwardingUncondTimerOption(
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
// TODO: Need CommandException
            // AsyncResult::ForMessage(onComplete, NULL,
            //         new CommandException(CommandException.Error.GENERIC_FAILURE));
            onComplete->SendToTarget();
       }
    }
    return NOERROR;
}

ECode CDMAPhone::GetOutgoingCallerIdDisplay(
    /* [in] */ IMessage* onComplete)
{
    Logger::E(LOGTAG, "getOutgoingCallerIdDisplay: not possible in CDMA");
    return NOERROR;
}

ECode CDMAPhone::GetCallForwardingIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::E(LOGTAG, "getCallForwardingIndicator: not possible in CDMA");
    *result = FALSE;
    return NOERROR;
}

ECode CDMAPhone::ExplicitCallTransfer()
{
    Logger::E(LOGTAG, "explicitCallTransfer: not possible in CDMA");
    return NOERROR;
}

ECode CDMAPhone::GetLine1AlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::E(LOGTAG, "getLine1AlphaTag: not possible in CDMA");
    *result = NULL;
    return NOERROR;
}

ECode CDMAPhone::NotifyPhoneStateChanged()
{
    return mNotifier->NotifyPhoneState(this);
}

ECode CDMAPhone::NotifyPreciseCallStateChanged()
{
    /* we'd love it if this was package-scoped*/
    PhoneBase::NotifyPreciseCallStateChangedP();
    return NOERROR;
}

ECode CDMAPhone::NotifyServiceStateChanged(
    /* [in] */ IServiceState* ss)
{
    VALIDATE_NOT_NULL(ss);
    PhoneBase::NotifyServiceStateChangedP(ss);
    return NOERROR;
}

ECode CDMAPhone::NotifyLocationChanged()
{
    return mNotifier->NotifyCellLocation(this);
}

ECode CDMAPhone::NotifyNewRingingConnection(
    /* [in] */ IConnection* c)
{
    VALIDATE_NOT_NULL(c);
    return PhoneBase::NotifyNewRingingConnectionP(c);
}

ECode CDMAPhone::NotifyDisconnect(
    /* [in] */ IConnection* cn)
{
    VALIDATE_NOT_NULL(cn);
    mDisconnectRegistrants->NotifyResult(cn);

    Int32 val1, val2;
    cn->GetDisconnectCause(&val1);
    cn->GetPreciseDisconnectCause(&val2);
    mNotifier->NotifyDisconnectCause(val1, val2);
    return NOERROR;
}

ECode CDMAPhone::NotifyUnknownConnection(
    /* [in] */ IConnection* connection)
{
    VALIDATE_NOT_NULL(connection);
    mUnknownConnectionRegistrants->NotifyResult(connection);
    return NOERROR;
}

ECode CDMAPhone::IsInEmergencyCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return ((CdmaCallTracker*)mCT.Get())->IsInEmergencyCall(result);
}

ECode CDMAPhone::IsInEcm(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsPhoneInEcmState;
    return NOERROR;
}

ECode CDMAPhone::SendEmergencyCallbackModeChange()
{
    //Send an Intent
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED, (IIntent**)&intent);
    intent->PutExtra(IPhoneConstants::PHONE_IN_ECM_STATE, mIsPhoneInEcmState);
    Int32 pid;
    GetPhoneId(&pid);
    SubscriptionManager::PutPhoneIdAndSubIdExtra(intent, pid);
    ActivityManagerNative::BroadcastStickyIntent(intent, String(NULL), IUserHandle::USER_ALL);
    if (DBG) Logger::D(LOGTAG, "SendEmergencyCallbackModeChange");
    return NOERROR;
}

ECode CDMAPhone::ExitEmergencyCallbackMode()
{
    Boolean b;
    if (mWakeLock->IsHeld(&b), b) {
        mWakeLock->ReleaseLock();
    }
    // Send a message which will invoke handleExitEmergencyCallbackMode
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE, (IMessage**)&msg);
    mCi->ExitEmergencyCallbackMode(msg);
    return NOERROR;
}

ECode CDMAPhone::HandleTimerInEmergencyCallbackMode(
    /* [in] */ Int32 action)
{
    switch(action) {
    case CANCEL_ECM_TIMER:
        RemoveCallbacks(mExitEcmRunnable);
        mEcmTimerResetRegistrants->NotifyResult(CoreUtils::Convert(TRUE));
        break;
    case RESTART_ECM_TIMER:
        Int64 delayInMillis;
        SystemProperties::GetInt64(
                ITelephonyProperties::PROPERTY_ECM_EXIT_TIMER,
                DEFAULT_ECM_EXIT_TIMER_VALUE, &delayInMillis);
        Boolean b;
        PostDelayed(mExitEcmRunnable, delayInMillis, &b);
        mEcmTimerResetRegistrants->NotifyResult(CoreUtils::Convert(FALSE));
        break;
    default:
        Logger::E(LOGTAG, "handleTimerInEmergencyCallbackMode, unsupported action %d", action);
    }
    return NOERROR;
}

ECode CDMAPhone::NotifyEcbmTimerReset(
    /* [in] */ Boolean flag)
{
    mEcmTimerResetRegistrants->NotifyResult(CoreUtils::Convert(flag));
    return NOERROR;
}

ECode CDMAPhone::RegisterForEcmTimerReset(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mEcmTimerResetRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

ECode CDMAPhone::UnregisterForEcmTimerReset(
    /* [in] */ IHandler* h)
{
    mEcmTimerResetRegistrants->Remove(h);
    return NOERROR;
}

ECode CDMAPhone::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;
    AutoPtr<IMessage> onComplete;

    if (!mIsTheCurrentActivePhone) {
        // Logger::E(LOGTAG, "Received message " + msg +
        //         "[" + msg.what + "] while being destroyed. Ignoring.");
        return NOERROR;
    }

    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
        case EVENT_RADIO_AVAILABLE: {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_GET_BASEBAND_VERSION_DONE, (IMessage**)&msg);
            mCi->GetBasebandVersion(msg);

            msg = NULL;
            ObtainMessage(EVENT_GET_DEVICE_IDENTITY_DONE, (IMessage**)&msg);
            mCi->GetDeviceIdentity(msg);
        }
        break;

        case EVENT_GET_BASEBAND_VERSION_DONE:{
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            if (ar->mException != NULL) {
                break;
            }

            // if (DBG) Logger::D(LOGTAG, "Baseband version: " + ar->mResult);
            String str;
            ICharSequence::Probe(ar->mResult)->ToString(&str);
            if (!str.Equals("")) {
                SetSystemProperty(ITelephonyProperties::PROPERTY_BASEBAND_VERSION, str);
            }
        }
        break;

        case EVENT_GET_DEVICE_IDENTITY_DONE:{
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            if (ar->mException != NULL) {
                break;
            }

            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
            obj = NULL;
            iArray->Get(0, (IInterface**)&obj);
            ICharSequence::Probe(obj)->ToString(&mImei);
            obj = NULL;
            iArray->Get(1, (IInterface**)&obj);
            ICharSequence::Probe(obj)->ToString(&mImeiSv);
            obj = NULL;
            iArray->Get(2, (IInterface**)&obj);
            ICharSequence::Probe(obj)->ToString(&mEsn);
            obj = NULL;
            iArray->Get(3, (IInterface**)&obj);
            ICharSequence::Probe(obj)->ToString(&mMeid);
        }
        break;

        case EVENT_EMERGENCY_CALLBACK_MODE_ENTER:{
            HandleEnterEmergencyCallbackMode(msg);
        }
        break;

        case  EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE:{
            HandleExitEmergencyCallbackMode(msg);
        }
        break;

        case EVENT_RUIM_RECORDS_LOADED:{
            Logger::D(LOGTAG, "Event EVENT_RUIM_RECORDS_LOADED Received");
            Boolean b;
            UpdateCurrentCarrierInProvider(&b);
            // Notify voicemails.
            Log(String("notifyMessageWaitingChanged"));
            mNotifier->NotifyMessageWaitingChanged(this);
            UpdateVoiceMail();
        }
        break;

        case EVENT_RADIO_OFF_OR_NOT_AVAILABLE:{
            Logger::D(LOGTAG, "Event EVENT_RADIO_OFF_OR_NOT_AVAILABLE Received");
        }
        break;

        case EVENT_RADIO_ON:{
            Logger::D(LOGTAG, "Event EVENT_RADIO_ON Received");
            Int32 val;
            mCdmaSSM->GetCdmaSubscriptionSource(&val);
            HandleCdmaSubscriptionSource(val);
        }
        break;

        case EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED:{
            Logger::D(LOGTAG, "EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED");
            Int32 val;
            mCdmaSSM->GetCdmaSubscriptionSource(&val);
            HandleCdmaSubscriptionSource(val);
        }
        break;

        case EVENT_SSN:{
            Logger::D(LOGTAG, "Event EVENT_SSN Received");
        }
        break;

        case EVENT_REGISTERED_TO_NETWORK:{
            Logger::D(LOGTAG, "Event EVENT_REGISTERED_TO_NETWORK Received");
        }
        break;

        case EVENT_NV_READY:{
            Logger::D(LOGTAG, "Event EVENT_NV_READY Received");
            PrepareEri();
            // Notify voicemails.
            Log(String("notifyMessageWaitingChanged"));
            mNotifier->NotifyMessageWaitingChanged(this);
            UpdateVoiceMail();
            AutoPtr<ISubInfoRecordUpdater> subInfoRecordUpdater;
            AutoPtr<IPhoneFactory> pf;
            CPhoneFactory::AcquireSingleton((IPhoneFactory**)&pf);
            pf->GetSubInfoRecordUpdater((ISubInfoRecordUpdater**)&subInfoRecordUpdater);
            if (subInfoRecordUpdater != NULL) {
                subInfoRecordUpdater->UpdateSubIdForNV (mPhoneId);
            }
        }
        break;

        case EVENT_SET_VM_NUMBER_DONE:{
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (IIccException::Probe(ar->mException) != NULL) {
                StoreVoiceMailNumber(mVmNumber);
                ar->mException = NULL;
            }
            onComplete = IMessage::Probe(ar->mUserObj);
            if (onComplete != NULL) {
                AsyncResult::ForMessage(onComplete, ar->mResult, ar->mException);
                onComplete->SendToTarget();
            }
        }
        break;

        default:{
            PhoneBase::HandleMessage(msg);
        }
    }
    return NOERROR;
}

ECode CDMAPhone::GetPhoneSubInfo(
    /* [out] */ IPhoneSubInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSubInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDMAPhone::GetIccPhoneBookInterfaceManager(
    /* [out] */ IIccPhoneBookInterfaceManager** result)
{
    VALIDATE_NOT_NULL(result);
    *result = IIccPhoneBookInterfaceManager::Probe(mRuimPhoneBookInterfaceManager);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDMAPhone::RegisterForEriFileLoaded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mEriFileLoadedRegistrants->Add(r);
    return NOERROR;
}

ECode CDMAPhone::UnregisterForEriFileLoaded(
    /* [in] */ IHandler* h)
{
    mEriFileLoadedRegistrants->Remove(h);
    return NOERROR;
}

ECode CDMAPhone::SetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& value)
{
    return PhoneBase::SetSystemProperty(property, value);
}

ECode CDMAPhone::GetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& defValue,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return PhoneBase::GetSystemProperty(property, defValue, result);
}

ECode CDMAPhone::ActivateCellBroadcastSms(
    /* [in] */ Int32 activate,
    /* [in] */ IMessage* response)
{
    Logger::E(LOGTAG, "[CDMAPhone] activateCellBroadcastSms() is obsolete; use SmsManager");
    return response->SendToTarget();
}

ECode CDMAPhone::GetCellBroadcastSmsConfig(
    /* [in] */ IMessage* response)
{
    Logger::E(LOGTAG, "[CDMAPhone] getCellBroadcastSmsConfig() is obsolete; use SmsManager");
    return response->SendToTarget();
}

ECode CDMAPhone::SetCellBroadcastSmsConfig(
    /* [in] */ ArrayOf<Int32>* configValuesArray,
    /* [in] */ IMessage* response)
{
    Logger::E(LOGTAG, "[CDMAPhone] setCellBroadcastSmsConfig() is obsolete; use SmsManager");
    return response->SendToTarget();
}

ECode CDMAPhone::NeedsOtaServiceProvisioning(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 val;
    ((CdmaServiceStateTracker*)mSST.Get())->GetOtasp(&val);
    *result = val != IServiceStateTracker::OTASP_NOT_NEEDED;
    return NOERROR;
}

ECode CDMAPhone::IsOtaSpNumber(
    /* [in] */ const String& dialStr,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean isOtaSpNum = FALSE;
    String dialableStr;
    PhoneNumberUtils::ExtractNetworkPortionAlt(dialStr, &dialableStr);
    if (dialableStr != NULL) {
        isOtaSpNum = IsIs683OtaSpDialStr(dialableStr);
        if (isOtaSpNum == FALSE) {
            isOtaSpNum = IsCarrierOtaSpNum(dialableStr);
        }
    }
    if (DBG) Logger::D(LOGTAG, "isOtaSpNumber %d", isOtaSpNum);
    *result = isOtaSpNum;
    return NOERROR;
}

ECode CDMAPhone::GetCdmaEriIconIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IServiceState> ss;
    GetServiceState((IServiceState**)&ss);
    return ss->GetCdmaEriIconIndex(result);
}

ECode CDMAPhone::GetCdmaEriIconMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IServiceState> ss;
    GetServiceState((IServiceState**)&ss);
    return ss->GetCdmaEriIconMode(result);
}

ECode CDMAPhone::GetCdmaEriText(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    AutoPtr<IServiceState> ss;
    GetServiceState((IServiceState**)&ss);
    Int32 roamInd;
    ss->GetCdmaRoamingIndicator(&roamInd);
    Int32 defRoamInd;
    ss->GetCdmaDefaultRoamingIndicator(&defRoamInd);
    return mEriManager->GetCdmaEriText(roamInd, defRoamInd, result);
}

ECode CDMAPhone::UpdateCurrentCarrierInProvider(
    /* [in] */ const String& operatorNumeric,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Log(String("CDMAPhone: UpdateCurrentCarrierInProvider called"));
    if (!TextUtils::IsEmpty(operatorNumeric)) {
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
            Log(String("UpdateCurrentCarrierInProvider from system: numeric=") + operatorNumeric);
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            AutoPtr<IContentResolver> cr;
            ctx->GetContentResolver((IContentResolver**)&cr);
            AutoPtr<IUri> tmpUri;
            cr->Insert(uri, map, (IUri**)&tmpUri);

            // Updates MCC MNC device configuration information
            Log(String("update mccmnc=") + operatorNumeric);
            MccTable::UpdateMccMncConfiguration(mContext, operatorNumeric, FALSE);

            *result = TRUE;
            return NOERROR;
        // } catch (SQLException e) {
        //     Logger::E(LOGTAG, "Can't store current operator", e);
        // }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CDMAPhone::UpdateCurrentCarrierInProvider(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode CDMAPhone::PrepareEri()
{
    if (mEriManager == NULL) {
        Logger::E(LOGTAG, "PrepareEri: Trying to access stale objects");
        return NOERROR;
    }
    mEriManager->LoadEriFile();
    Boolean b;
    if(mEriManager->IsEriFileLoaded(&b), b) {
        // when the ERI file is loaded
        Log(String("ERI read, notify registrants"));
        mEriFileLoadedRegistrants->NotifyRegistrants();
    }
    return NOERROR;
}

ECode CDMAPhone::IsEriFileLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mEriManager->IsEriFileLoaded(result);
}

ECode CDMAPhone::SetVoiceMessageWaiting(
    /* [in] */ Int32 line,
    /* [in] */ Int32 countWaiting)
{
    return SetVoiceMessageCount(countWaiting);
}

ECode CDMAPhone::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("CDMAPhone extends:"));
    PhoneBase::Dump(fd, pw, args);
    // pw->Println(String(" mVmNumber=") + mVmNumber);
    // pw->Println(String(" mCT=") + mCT);
    // pw->Println(String(" mSST=") + mSST);
    // pw->Println(String(" mCdmaSSM=") + mCdmaSSM);
    // pw->Println(String(" mPendingMmis=") + mPendingMmis);
    // pw->Println(String(" mRuimPhoneBookInterfaceManager=") + mRuimPhoneBookInterfaceManager);
    // pw->Println(String(" mCdmaSubscriptionSource=") + mCdmaSubscriptionSource);
    // pw->Println(String(" mSubInfo=") + mSubInfo);
    // pw->Println(String(" mEriManager=") + mEriManager);
    // pw->Println(String(" mWakeLock=") + mWakeLock);
    // pw->Println(String(" mIsPhoneInEcmState=") + mIsPhoneInEcmState);
    // if (VDBG) pw->Println(String(" mImei=") + mImei);
    // if (VDBG) pw->Println(String(" mImeiSv=") + mImeiSv);
    // if (VDBG) pw->Println(String(" mEsn=") + mEsn);
    // if (VDBG) pw->Println(String(" mMeid=") + mMeid);
    // pw->Println(String(" mCarrierOtaSpNumSchema=") + mCarrierOtaSpNumSchema);
    // pw->Println(String(" getCdmaEriIconIndex()=") + getCdmaEriIconIndex());
    // pw->Println(String(" getCdmaEriIconMode()=") + getCdmaEriIconMode());
    // pw->Println(String(" getCdmaEriText()=") + getCdmaEriText());
    // pw->Println(String(" isMinInfoReady()=") + isMinInfoReady());
    // pw->Println(String(" isCspPlmnEnabled()=") + isCspPlmnEnabled());
    return NOERROR;
}

ECode CDMAPhone::SetOperatorBrandOverride(
    /* [in] */ const String& brand,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mUiccController == NULL) {
        *result = FALSE;
        return NOERROR;
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
            String str;
            iccRecords->GetServiceProviderName(&str);
            SystemProperties::Set(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, str);
        }
        if (mSST != NULL) {
            IServiceStateTracker::Probe(mSST)->PollState();
        }
    }
    *result = status;
    return NOERROR;
}

void CDMAPhone::InitSstIcc()
{
    CCdmaServiceStateTracker::New(this, (ICdmaServiceStateTracker**)&mSST);
}

void CDMAPhone::Init(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* notifier)
{
    mCi->SetPhoneType(IPhoneConstants::PHONE_TYPE_CDMA);
    CCdmaCallTracker::New(this, (ICdmaCallTracker**)&mCT);
    mCdmaSSM = CdmaSubscriptionSourceManager::GetInstance(context, mCi, this,
            EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, NULL);
    CDcTracker::New(this, (IDcTracker**)&mDcTracker);
    CRuimPhoneBookInterfaceManager::New(this, (IRuimPhoneBookInterfaceManager**)&mRuimPhoneBookInterfaceManager);
    CPhoneSubInfo::New(this, (IPhoneSubInfo**)&mSubInfo);
    CEriManager::New(this, context, CEriManager::ERI_FROM_XML, (IEriManager**)&mEriManager);

    mCi->RegisterForAvailable(this, EVENT_RADIO_AVAILABLE, NULL);
    mCi->RegisterForOffOrNotAvailable(this, EVENT_RADIO_OFF_OR_NOT_AVAILABLE, NULL);
    mCi->RegisterForOn(this, EVENT_RADIO_ON, NULL);
    mCi->SetOnSuppServiceNotification(this, EVENT_SSN, NULL);
    IServiceStateTracker::Probe(mSST)->RegisterForNetworkAttached(this, EVENT_REGISTERED_TO_NETWORK, NULL);
    mCi->SetEmergencyCallbackMode(this, EVENT_EMERGENCY_CALLBACK_MODE_ENTER, NULL);
    mCi->RegisterForExitEmergencyCallbackMode(this, EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE,
            NULL);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK,LOGTAG,
            (IPowerManagerWakeLock**)&mWakeLock);

    //Change the system setting
    SystemProperties::Set(ITelephonyProperties::CURRENT_ACTIVE_PHONE,
            StringUtils::ToString(IPhoneConstants::PHONE_TYPE_CDMA));

    // This is needed to handle phone process crashes
    String inEcm;
    SystemProperties::Get(ITelephonyProperties::PROPERTY_INECM_MODE, String("FALSE"), &inEcm);
    mIsPhoneInEcmState = inEcm.Equals("TRUE");
    if (mIsPhoneInEcmState) {
        // Send a message which will invoke handleExitEmergencyCallbackMode
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE, (IMessage**)&msg);
        mCi->ExitEmergencyCallbackMode(msg);
    }

    // get the string that specifies the carrier OTA Sp number
    SystemProperties::Get(
            ITelephonyProperties::PROPERTY_OTASP_NUM_SCHEMA, String(""), &mCarrierOtaSpNumSchema);

    // Sets operator properties by retrieving from build-time system property
    String operatorAlpha;
    SystemProperties::Get(String("ro.cdma.home.operator.alpha"), &operatorAlpha);
    String operatorNumeric;
    SystemProperties::Get(PROPERTY_CDMA_HOME_OPERATOR_NUMERIC, &operatorNumeric);
    Log(String("init: operatorAlpha='") + operatorAlpha
            + "' operatorNumeric='" + operatorNumeric + "'");
    AutoPtr<IUiccCardApplication> app;
    mUiccController->GetUiccCardApplication(mPhoneId, IUiccController::APP_FAM_3GPP,
        (IUiccCardApplication**)&app);
    if (app == NULL) {
        Log(String("init: APP_FAM_3GPP == NULL"));
        if (!TextUtils::IsEmpty(operatorAlpha)) {
            Log(String("init: set 'gsm.sim.operator.alpha' to operator='") + operatorAlpha + "'");
            SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, operatorAlpha);
        }
        if (!TextUtils::IsEmpty(operatorNumeric)) {
            Log(String("init: set 'gsm.sim.operator.numeric' to operator='") + operatorNumeric + "'");
            Log(String("update icc_operator_numeric=") + operatorNumeric);
            SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_NUMERIC, operatorNumeric);

            Int64 sid;
            GetSubId(&sid);
            Int32 val;
            SubscriptionController::GetInstance()->SetMccMnc(operatorNumeric, sid, &val);
        }
        SetIsoCountryProperty(operatorNumeric);
    }

    // Sets current entry in the telephony carrier table
    Boolean b;
    UpdateCurrentCarrierInProvider(operatorNumeric, &b);
}

ECode CDMAPhone::DialInternal(
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** connection)
{
    VALIDATE_NOT_NULL(connection);
    // Need to make sure dialString gets parsed properly
    String newDialString;
    PhoneNumberUtils::StripSeparators(dialString, &newDialString);
    return ((CdmaCallTracker*)mCT.Get())->Dial(newDialString, connection);
}

AutoPtr<IUiccCardApplication> CDMAPhone::GetUiccCardApplication()
{
    AutoPtr<IUiccCardApplication> app;
    mUiccController->GetUiccCardApplication(mPhoneId, IUiccController::APP_FAM_3GPP2,
            (IUiccCardApplication**)&app);
    return app;
}

void CDMAPhone::SetCardInPhoneBook()
{
    if (mUiccController == NULL ) {
        return;
    }

    AutoPtr<IUiccCard> card;
    mUiccController->GetUiccCard(mPhoneId, (IUiccCard**)&card);
    IIccPhoneBookInterfaceManager::Probe(mRuimPhoneBookInterfaceManager)->SetIccCard(card);
}

ECode CDMAPhone::OnUpdateIccAvailability()
{
    if (mUiccController == NULL ) {
        return NOERROR;
    }

    // Get the latest info on the card and
    // send this to Phone Book
    SetCardInPhoneBook();

    AutoPtr<IUiccCardApplication> newUiccApplication = GetUiccCardApplication();

    if (newUiccApplication == NULL) {
        Log(String("can't find 3GPP2 application; trying APP_FAM_3GPP"));

        mUiccController->GetUiccCardApplication(mPhoneId, IUiccController::APP_FAM_3GPP,
                (IUiccCardApplication**)&newUiccApplication);
    }

    AutoPtr<IInterface> obj;
    mUiccApplication->Get((IInterface**)&obj);
    AutoPtr<IUiccCardApplication> app = IUiccCardApplication::Probe(obj);
    if (app != newUiccApplication) {
        if (app != NULL) {
            Log(String("Removing stale icc objects."));
            obj = NULL;
            mIccRecords->Get((IInterface**)&obj);
            if (obj != NULL) {
                UnregisterForRuimRecordEvents();
            }
            mIccRecords->Set(NULL);
            mUiccApplication->Set(NULL);
        }
        if (newUiccApplication != NULL) {
            Log(String("New Uicc application found"));
            mUiccApplication->Set(newUiccApplication);
            AutoPtr<IIccRecords> card;
            newUiccApplication->GetIccRecords((IIccRecords**)&card);
            mIccRecords->Set(card);
            RegisterForRuimRecordEvents();
        }
    }
    return NOERROR;
}

void CDMAPhone::SetIsoCountryProperty(
    /* [in] */ const String& operatorNumeric)
{
    if (TextUtils::IsEmpty(operatorNumeric)) {
        Log(String("setIsoCountryProperty: clear 'gsm.sim.operator.iso-country'"));
        SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ISO_COUNTRY, String(""));
    }
    else {
        String iso = String("");
        // try {
            iso = MccTable::CountryCodeForMcc(StringUtils::ParseInt32(
                    operatorNumeric.Substring(0,3)));
        // } catch (NumberFormatException ex) {
        //     loge("setIsoCountryProperty: countryCodeForMcc error", ex);
        // } catch (StringIndexOutOfBoundsException ex) {
        //     loge("setIsoCountryProperty: countryCodeForMcc error", ex);
        // }

        Log(String("setIsoCountryProperty: set 'gsm.sim.operator.iso-country' to iso=") + iso);
        SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ISO_COUNTRY, iso);
    }
}

void CDMAPhone::RegisterForRuimRecordEvents()
{
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
    if (r == NULL) {
        return;
    }
    r->RegisterForRecordsLoaded(this, EVENT_RUIM_RECORDS_LOADED, NULL);
}

void CDMAPhone::UnregisterForRuimRecordEvents()
{
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
    if (r == NULL) {
        return;
    }
    r->UnregisterForRecordsLoaded(this);
}

void CDMAPhone::Log(
    /* [in] */ const String& s)
{
    if (DBG)
        Logger::D(LOGTAG, s);
}

//void CDMAPhone::Loge(
//    /* [in] */ const String& s,
//    /* [in] */ Exception* e)
//{
////    // if (DBG)
//    //     Logger::E(LOGTAG, s, e);
////}

void CDMAPhone::UpdateVoiceMail()
{
    SetVoiceMessageCount(GetStoredVoiceMessageCount());
}

Int32 CDMAPhone::GetStoredVoiceMessageCount()
{
    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(mContext, (ISharedPreferences**)&sp);
    Int64 sid;
    GetSubId(&sid);
    Int32 val;
    sp->GetInt32(VM_COUNT + StringUtils::ToString(sid), 0, &val);
    return val;
}

void CDMAPhone::HandleEnterEmergencyCallbackMode(
    /* [in] */ IMessage* msg)
{
    if (DBG) {
        Logger::D(LOGTAG, "handleEnterEmergencyCallbackMode,mIsPhoneInEcmState= %d"
                , mIsPhoneInEcmState);
    }
    // if phone is not in Ecm mode, and it's changed to Ecm mode
    if (mIsPhoneInEcmState == FALSE) {
        mIsPhoneInEcmState = TRUE;
        // notify change
        SendEmergencyCallbackModeChange();
        PhoneBase::SetSystemProperty(ITelephonyProperties::PROPERTY_INECM_MODE, String("TRUE"));

        // Post this runnable so we will automatically exit
        // if no one invokes exitEmergencyCallbackMode() directly.
        Int64 delayInMillis;
        SystemProperties::GetInt64(
                ITelephonyProperties::PROPERTY_ECM_EXIT_TIMER, DEFAULT_ECM_EXIT_TIMER_VALUE,
                &delayInMillis);
        Boolean b;
        PostDelayed(mExitEcmRunnable, delayInMillis, &b);
        // We don't want to go to sleep while in Ecm
        mWakeLock->AcquireLock();
    }
}

void CDMAPhone::HandleExitEmergencyCallbackMode(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<AsyncResult> ar = (AsyncResult*)IAsyncResult::Probe(obj);
    if (DBG) {
        // Logger::D(LOGTAG, "handleExitEmergencyCallbackMode,ar->mException , mIsPhoneInEcmState "
        //         + ar->mException + mIsPhoneInEcmState);
    }
    // Remove pending exit Ecm runnable, if any
    RemoveCallbacks(mExitEcmRunnable);

    if (mEcmExitRespRegistrant != NULL) {
        mEcmExitRespRegistrant->NotifyRegistrant(ar);
    }
    // if exiting ecm success
    if (ar->mException == NULL) {
        if (mIsPhoneInEcmState) {
            mIsPhoneInEcmState = FALSE;
            PhoneBase::SetSystemProperty(ITelephonyProperties::PROPERTY_INECM_MODE, String("FALSE"));
        }
        // send an Intent
        SendEmergencyCallbackModeChange();
        // Re-initiate data connection
        Boolean b;
        mDcTracker->SetInternalDataEnabled(TRUE, &b);
    }
}

void CDMAPhone::HandleCdmaSubscriptionSource(
    /* [in] */ Int32 newSubscriptionSource)
{
    if (newSubscriptionSource != mCdmaSubscriptionSource) {
         mCdmaSubscriptionSource = newSubscriptionSource;
         if (newSubscriptionSource == CDMA_SUBSCRIPTION_NV) {
             // NV is ready when subscription source is NV
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_NV_READY, (IMessage**)&msg);
            Boolean b;
            SendMessage(msg, &b);
         }
    }
}

Boolean CDMAPhone::IsIs683OtaSpDialStr(
    /* [in] */ const String& dialStr)
{
    Int32 sysSelCodeInt;
    Boolean isOtaspDialString = FALSE;
    Int32 dialStrLen = dialStr.GetLength();

    if (dialStrLen == IS683A_FEATURE_CODE_NUM_DIGITS) {
        if (dialStr.Equals(IS683A_FEATURE_CODE)) {
            isOtaspDialString = TRUE;
        }
    }
    else {
        sysSelCodeInt = ExtractSelCodeFromOtaSpNum(dialStr);
        switch (sysSelCodeInt) {
            case IS683_CONST_800MHZ_A_BAND:
            case IS683_CONST_800MHZ_B_BAND:
            case IS683_CONST_1900MHZ_A_BLOCK:
            case IS683_CONST_1900MHZ_B_BLOCK:
            case IS683_CONST_1900MHZ_C_BLOCK:
            case IS683_CONST_1900MHZ_D_BLOCK:
            case IS683_CONST_1900MHZ_E_BLOCK:
            case IS683_CONST_1900MHZ_F_BLOCK:
                isOtaspDialString = TRUE;
                break;
            default:
                break;
        }
    }
    return isOtaspDialString;
}

Int32 CDMAPhone::ExtractSelCodeFromOtaSpNum(
    /* [in] */ const String& dialStr)
{
    Int32 dialStrLen = dialStr.GetLength();
    Int32 sysSelCodeInt = INVALID_SYSTEM_SELECTION_CODE;

    if ((dialStr.RegionMatches(0, IS683A_FEATURE_CODE,
                               0, IS683A_FEATURE_CODE_NUM_DIGITS)) &&
        (dialStrLen >= (IS683A_FEATURE_CODE_NUM_DIGITS +
                        IS683A_SYS_SEL_CODE_NUM_DIGITS))) {
            // Since we checked the condition above, the system selection code
            // extracted from dialStr will not cause any exception
            sysSelCodeInt = StringUtils::ParseInt32 (
                            dialStr.Substring (IS683A_FEATURE_CODE_NUM_DIGITS,
                            IS683A_FEATURE_CODE_NUM_DIGITS + IS683A_SYS_SEL_CODE_NUM_DIGITS));
    }
    if (DBG) Logger::D(LOGTAG, "extractSelCodeFromOtaSpNum %d", sysSelCodeInt);
    return sysSelCodeInt;
}

Boolean CDMAPhone::CheckOtaSpNumBasedOnSysSelCode(
    /* [in] */ Int32 sysSelCodeInt,
    /* [in] */ ArrayOf<String>* sch)
{
    Boolean isOtaSpNum = FALSE;
    // try {
    // Get how many number of system selection code ranges
    Int32 selRc = StringUtils::ParseInt32((*sch)[1]);
    for (Int32 i = 0; i < selRc; i++) {
        if (!TextUtils::IsEmpty((*sch)[i+2]) && !TextUtils::IsEmpty((*sch)[i+3])) {
            Int32 selMin = StringUtils::ParseInt32((*sch)[i+2]);
            Int32 selMax = StringUtils::ParseInt32((*sch)[i+3]);
            // Check if the selection code extracted from the dial string falls
            // within any of the range pairs specified in the schema.
            if ((sysSelCodeInt >= selMin) && (sysSelCodeInt <= selMax)) {
                isOtaSpNum = TRUE;
                break;
            }
        }
    }
    // } catch (NumberFormatException ex) {
    //     // If the carrier ota sp number schema is not correct, we still allow dial
    //     // and only log the error:
    //     Logger::E(LOGTAG, "checkOtaSpNumBasedOnSysSelCode, error", ex);
    // }
    return isOtaSpNum;
}

Boolean CDMAPhone::IsCarrierOtaSpNum(
    /* [in] */ const String& dialStr)
{
    Boolean isOtaSpNum = FALSE;
    Int32 sysSelCodeInt = ExtractSelCodeFromOtaSpNum(dialStr);
    if (sysSelCodeInt == INVALID_SYSTEM_SELECTION_CODE) {
        return isOtaSpNum;
    }
    // mCarrierOtaSpNumSchema is retrieved from PROPERTY_OTASP_NUM_SCHEMA:
    if (!TextUtils::IsEmpty(mCarrierOtaSpNumSchema)) {
        AutoPtr<IMatcher> m;
        pOtaSpNumSchema->Matcher(mCarrierOtaSpNumSchema, (IMatcher**)&m);
        if (DBG) {
            Logger::D(LOGTAG, "isCarrierOtaSpNum,schema %s", mCarrierOtaSpNumSchema.string());
        }

        Boolean b;
        if (m->Find(&b), b) {
            AutoPtr<ArrayOf<String> > sch;
            pOtaSpNumSchema->Split(mCarrierOtaSpNumSchema, (ArrayOf<String>**)&sch);
            // If carrier uses system selection code mechanism
            if (!TextUtils::IsEmpty((*sch)[0]) && (*sch)[0].Equals("SELC")) {
                if (sysSelCodeInt != INVALID_SYSTEM_SELECTION_CODE) {
                    isOtaSpNum = CheckOtaSpNumBasedOnSysSelCode(sysSelCodeInt, sch);
                }
                else {
                    if (DBG) {
                        Logger::D(LOGTAG, "isCarrierOtaSpNum,sysSelCodeInt is invalid");
                    }
                }
            }
            else if (!TextUtils::IsEmpty((*sch)[0]) && (*sch)[0].Equals("FC")) {
                Int32 fcLen =  StringUtils::ParseInt32((*sch)[1]);
                String fc = (*sch)[2];
                if (dialStr.RegionMatches(0,fc,0,fcLen)) {
                    isOtaSpNum = TRUE;
                }
                else {
                    if (DBG) Logger::D(LOGTAG, "isCarrierOtaSpNum,not otasp number");
                }
            }
            else {
                if (DBG) {
                    Logger::D(LOGTAG, "isCarrierOtaSpNum,ota schema not supported %s"
                        , (*sch)[0].string());
                }
            }
        }
        else {
            if (DBG) {
                Logger::D(LOGTAG, "isCarrierOtaSpNum,ota schema pattern not right %s",
                      mCarrierOtaSpNumSchema.string());
            }
        }
    }
    else {
        if (DBG) Logger::D(LOGTAG, "isCarrierOtaSpNum,ota schema pattern empty");
    }
    return isOtaSpNum;
}

void CDMAPhone::StoreVoiceMailNumber(
    /* [in] */ const String& number)
{
    // Update the preference value of voicemail number
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&sp);
    AutoPtr<ISharedPreferencesEditor> editor;
    sp->Edit((ISharedPreferencesEditor**)&editor);
    Int64 sid;
    GetSubId(&sid);
    editor->PutString(VM_NUMBER_CDMA + StringUtils::ToString(sid), number);
    editor->Apply();
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
