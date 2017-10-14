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

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Ims.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/app/CPendingIntent.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/ims/CImsCallProfile.h"
#include "elastos/droid/internal/telephony/Call.h"
#include "elastos/droid/internal/telephony/gsm/CSuppServiceNotification.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhone.h"
#include "elastos/droid/internal/telephony/imsphone/CImsPhoneCall.h"
#include "elastos/droid/internal/telephony/imsphone/CImsPhoneConnection.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneCallTracker.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneConnection.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/preference/PreferenceManager.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CPendingIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Ims::CImsCallProfile;
using Elastos::Droid::Ims::IImsCallProfile;
using Elastos::Droid::Internal::Telephony::Call;
using Elastos::Droid::Internal::Telephony::Gsm::CSuppServiceNotification;
using Elastos::Droid::Internal::Telephony::Gsm::ISuppServiceNotification;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Preference::PreferenceManager;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

//=====================================================================
//             ImsPhoneCallTracker::InnerBroadcastReceiver
//=====================================================================
ImsPhoneCallTracker::InnerBroadcastReceiver::InnerBroadcastReceiver(
    /* [in] */ ImsPhoneCallTracker* owner)
    : mOwner(owner)
{
}

ECode ImsPhoneCallTracker::InnerBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(/*IImsManager::ACTION_IMS_INCOMING_CALL*/"com.android.ims.IMS_INCOMING_CALL")) {
        if (DBG) mOwner->Log(String("onReceive : incoming call intent"));

        if (mOwner->mImsManager == NULL) return NOERROR;

        if (mOwner->mServiceId < 0) return NOERROR;

        // try {
        // Network initiated USSD will be treated by mImsUssdListener
        Boolean isUssd = FALSE;
// TODO: Need IImsManager
        // intent->GetBooleanExtra(IImsManager::EXTRA_USSD, FALSE, &isUssd);
        if (isUssd) {
            if (DBG) mOwner->Log(String("onReceive : USSD"));
// TODO: Need IImsManager
            // mOwner->mImsManager->TakeCall(mOwner->mServiceId, intent, mOwner->mImsUssdListener,
            //         (/*TODO IImsCall*/IInterface**)&mOwner->mUssdSession);
            if (mOwner->mUssdSession != NULL) {
// TODO: Need IImsCall
                // mOwner->mUssdSession->Accept(IImsCallProfile::CALL_TYPE_VOICE);
            }
            return NOERROR;
        }

        // Normal MT call
        AutoPtr</*TODO IImsCall*/IInterface> imsCall;
// TODO: Need IImsCall
        // mImsManager->TakeCall(mServiceId, intent, mImsCallListener,
        //         (/*TODO IImsCall*/IInterface**)&imsCall);
        AutoPtr<IContext> ctx;
        IPhone::Probe(mOwner->mPhone)->GetContext((IContext**)&ctx);
        AutoPtr<IImsPhoneConnection> conn;
        CImsPhoneConnection::New(ctx, imsCall,
                mOwner, mOwner->mRingingCall, (IImsPhoneConnection**)&conn);
        mOwner->AddConnection(conn);

// TODO: Need IIImsVideoCallProvider
        // AutoPtr<IIImsVideoCallProvider> imsVideoCallProvider =
        //         imsCall.getCallSession().getVideoCallProvider();
        // if (imsVideoCallProvider != NULL) {
        //     ImsVideoCallProviderWrapper imsVideoCallProviderWrapper =
        //             new ImsVideoCallProviderWrapper(imsVideoCallProvider);
        //     conn->SetVideoProvider(imsVideoCallProviderWrapper);
        // }

        ICallState fgState;
        ICall::Probe(mOwner->mForegroundCall)->GetState(&fgState);

        ICallState bgState;
        ICall::Probe(mOwner->mBackgroundCall)->GetState(&bgState);

        if ((fgState != ICallState_IDLE) ||
                (bgState != ICallState_IDLE)) {
            Boolean b;
            ((ImsPhoneConnection*)conn.Get())->Update(imsCall, ICallState_WAITING, &b);
        }

        ((ImsPhone*)mOwner->mPhone.Get())->NotifyNewRingingConnection(IConnection::Probe(conn));
        ((ImsPhone*)mOwner->mPhone.Get())->NotifyIncomingRing();

        mOwner->UpdatePhoneState();
        ((ImsPhone*)mOwner->mPhone.Get())->NotifyPreciseCallStateChanged();
        // } catch (ImsException e) {
        //     Loge("onReceive : exception " + e);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

//=====================================================================
//              ImsPhoneCallTracker::InnerImsCallListener
//=====================================================================
ImsPhoneCallTracker::InnerImsCallListener::InnerImsCallListener(
    /* [in] */ ImsPhoneCallTracker* owner)
    : mOwner(owner)
{
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallProgressing(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    if (DBG) mOwner->Log(String("onCallProgressing"));

    mOwner->mPendingMO = NULL;
    mOwner->ProcessCallStateChange(imsCall, ICallState_ALERTING,
            IDisconnectCause::NOT_DISCONNECTED);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallStarted(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    if (DBG) mOwner->Log(String("onCallStarted"));

    mOwner->mPendingMO = NULL;
    mOwner->ProcessCallStateChange(imsCall, ICallState_ACTIVE,
            IDisconnectCause::NOT_DISCONNECTED);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallUpdated(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    if (DBG) mOwner->Log(String("onCallUpdated"));
    if (imsCall == NULL) {
        return NOERROR;
    }
    AutoPtr<IImsPhoneConnection> conn = mOwner->FindConnection(imsCall);
    if (conn != NULL) {
        AutoPtr<ICall> call;
        IConnection::Probe(conn)->GetCall((ICall**)&call);
        mOwner->ProcessCallStateChange(imsCall, ((Call*)call.Get())->mState,
                IDisconnectCause::NOT_DISCONNECTED);
    }
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallStartFailed(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    Int32 code;
    reasonInfo->GetCode(&code);
    if (DBG) mOwner->Log(String("onCallStartFailed reasonCode=") + StringUtils::ToString(code));

    if (mOwner->mPendingMO != NULL) {
        // To initiate dialing circuit-switched call
        ICallState backgroundCallState;
        ICall::Probe(mOwner->mBackgroundCall)->GetState(&backgroundCallState);
        ICallState ringingCallState;
        ICall::Probe(mOwner->mRingingCall)->GetState(&ringingCallState);

        if (code == IImsReasonInfo::CODE_LOCAL_CALL_CS_RETRY_REQUIRED
                && backgroundCallState == ICallState_IDLE
                && ringingCallState == ICallState_IDLE) {
            ((ImsPhoneCall*)mOwner->mForegroundCall.Get())->Detach(mOwner->mPendingMO);
            mOwner->RemoveConnection(mOwner->mPendingMO);
            // mOwner->mPendingMO->Finalize();
            mOwner->mPendingMO = NULL;
            ((ImsPhone*)mOwner->mPhone.Get())->InitiateSilentRedial();
            return NOERROR;
        }
        mOwner->mPendingMO->SetDisconnectCause(IDisconnectCause::ERROR_UNSPECIFIED);
        Boolean b;
        mOwner->SendEmptyMessageDelayed(EVENT_HANGUP_PENDINGMO, TIMEOUT_HANGUP_PENDINGMO, &b);
    }
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallTerminated(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    Int32 code;
    reasonInfo->GetCode(&code);
    if (DBG) mOwner->Log(String("onCallTerminated reasonCode=") + StringUtils::ToString(code));

    ICallState oldState;
    ICall::Probe(mOwner->mForegroundCall)->GetState(&oldState);
    Int32 cause = mOwner->GetDisconnectCauseFromReasonInfo(reasonInfo);
    AutoPtr<IImsPhoneConnection> conn = mOwner->FindConnection(imsCall);
    // if (DBG) Log("cause = " + cause + " conn = " + conn);

    Boolean b;
    Int64 time;
    if (conn != NULL && (IConnection::Probe(conn)->IsIncoming(&b), b)
            && (IConnection::Probe(conn)->GetConnectTime(&time), time) == 0) {
        // Missed or rejected call
        if (cause == IDisconnectCause::LOCAL) {
            cause = IDisconnectCause::INCOMING_REJECTED;
        }
        else {
            cause = IDisconnectCause::INCOMING_MISSED;
        }
    }

// TODO: Need IImsCall
    // if (cause == IDisconnectCause::NORMAL && conn != NULL && conn->GetImsCall()->IsMerged()) {
    //     // Call was terminated while it is merged instead of a remote disconnect.
    //     cause = IDisconnectCause::IMS_MERGED_SUCCESSFULLY;
    // }

    mOwner->ProcessCallStateChange(imsCall, ICallState_DISCONNECTED, cause);

    if (code == IImsReasonInfo::CODE_USER_TERMINATED) {
        ICallState foregroundCallState;
        ICall::Probe(mOwner->mForegroundCall)->GetState(&foregroundCallState);
        ICallState backgroundCallState;
        ICall::Probe(mOwner->mBackgroundCall)->GetState(&backgroundCallState);
        if ((oldState == ICallState_DISCONNECTING)
                && (foregroundCallState == ICallState_DISCONNECTED)
                && (backgroundCallState == ICallState_HOLDING)) {
            mOwner->SendEmptyMessage(EVENT_RESUME_BACKGROUND, &b);
        }
    }
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallHeld(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    if (DBG) mOwner->Log(String("onCallHeld"));

    AutoLock lock(mOwner->mSyncHold);
    ICallState oldState;
    ICall::Probe(mOwner->mBackgroundCall)->GetState(&oldState);
    mOwner->ProcessCallStateChange(imsCall, ICallState_HOLDING,
            IDisconnectCause::NOT_DISCONNECTED);
    if (oldState == ICallState_ACTIVE) {
        ICallState foregroundCallState;
        ICall::Probe(mOwner->mForegroundCall)->GetState(&foregroundCallState);
        ICallState ringingCallState;
        ICall::Probe(mOwner->mRingingCall)->GetState(&ringingCallState);
        if ((foregroundCallState == ICallState_HOLDING)
                || (ringingCallState == ICallState_WAITING)) {
            Boolean isOwner = TRUE;
// TODO: Need CallGroup
            // CallGroup callGroup =  imsCall.getCallGroup();
            // if (callGroup != NULL) {
            //     isOwner = callGroup.isOwner(imsCall);
            // }
            // if (isOwner) {
            //     SendEmptyMessage(EVENT_RESUME_BACKGROUND);
            // }
        }
        else {
            //when multiple connections belong to background call,
            //only the first callback reaches here
            //otherwise the oldState is already HOLDING
            if (mOwner->mPendingMO != NULL) {
                Boolean b;
                mOwner->SendEmptyMessage(EVENT_DIAL_PENDINGMO, &b);
            }
        }
    }

    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallHoldFailed(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    Int32 code;
    reasonInfo->GetCode(&code);
    if (DBG) mOwner->Log(String("onCallHoldFailed reasonCode=") + StringUtils::ToString(code));

    AutoLock lock(mOwner->mSyncHold);
    ICallState bgState;
    ICall::Probe(mOwner->mBackgroundCall)->GetState(&bgState);
if (code == IImsReasonInfo::CODE_LOCAL_CALL_TERMINATED) {
        // disconnected while processing hold
        if (mOwner->mPendingMO != NULL) {
            Boolean b;
            mOwner->SendEmptyMessage(EVENT_DIAL_PENDINGMO, &b);
        }
    }
    else if (bgState == ICallState_ACTIVE) {
        ((ImsPhoneCall*)mOwner->mForegroundCall.Get())->SwitchWith(mOwner->mBackgroundCall);

        if (mOwner->mPendingMO != NULL) {
            mOwner->mPendingMO->SetDisconnectCause(IDisconnectCause::ERROR_UNSPECIFIED);
            Boolean b;
            mOwner->SendEmptyMessageDelayed(EVENT_HANGUP_PENDINGMO, TIMEOUT_HANGUP_PENDINGMO, &b);
        }
    }

    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallResumed(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    if (DBG) mOwner->Log(String("onCallResumed"));

    mOwner->ProcessCallStateChange(imsCall, ICallState_ACTIVE,
            IDisconnectCause::NOT_DISCONNECTED);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallResumeFailed(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    // TODO : What should be done?
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallResumeReceived(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    if (DBG) mOwner->Log(String("onCallResumeReceived"));

    if (mOwner->mOnHoldToneStarted) {
        ((ImsPhone*)mOwner->mPhone.Get())->StopOnHoldTone();
        mOwner->mOnHoldToneStarted = FALSE;
    }

    AutoPtr<ISuppServiceNotification> supp;
    CSuppServiceNotification::New((ISuppServiceNotification**)&supp);
    // Type of notification: 0 = MO; 1 = MT
    // Refer SuppServiceNotification class documentation.
    ((CSuppServiceNotification*)supp.Get())->mNotificationType = 1;
    ((CSuppServiceNotification*)supp.Get())->mCode = ISuppServiceNotification::MT_CODE_CALL_RETRIEVED;
    mOwner->mPhone->NotifySuppSvcNotification(supp);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallHoldReceived(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    if (DBG) mOwner->Log(String("onCallHoldReceived"));

    AutoPtr<IImsPhoneConnection> conn = mOwner->FindConnection(imsCall);
    ICallState state;
    if (conn != NULL && (IConnection::Probe(conn)->GetState(&state), state) == ICallState_ACTIVE) {
        Boolean b = FALSE;
// TODO: Need IImsCall
        // ImsPhoneCall->IsLocalTone(imsCall, &b);
        if (!mOwner->mOnHoldToneStarted && b) {
            ((ImsPhone*)mOwner->mPhone.Get())->StartOnHoldTone();
            mOwner->mOnHoldToneStarted = TRUE;
        }
    }

    AutoPtr<ISuppServiceNotification> supp;
    CSuppServiceNotification::New((ISuppServiceNotification**)&supp);
    // Type of notification: 0 = MO; 1 = MT
    // Refer SuppServiceNotification class documentation.
    ((CSuppServiceNotification*)supp.Get())->mNotificationType = 1;
    ((CSuppServiceNotification*)supp.Get())->mCode = ISuppServiceNotification::MT_CODE_CALL_ON_HOLD;
    ((ImsPhone*)mOwner->mPhone.Get())->NotifySuppSvcNotification(supp);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallMerged(
    /* [in] */ /*TODO IImsCall*/IInterface* call)
{
    if (DBG) mOwner->Log(String("onCallMerged"));

    ICallState state;
    ICall::Probe(mOwner->mForegroundCall)->GetState(&state);
    ((ImsPhoneCall*)mOwner->mForegroundCall.Get())->Merge(mOwner->mBackgroundCall, state);
    mOwner->UpdatePhoneState();
    ((ImsPhone*)mOwner->mPhone.Get())->NotifyPreciseCallStateChanged();
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallMergeFailed(
    /* [in] */ /*TODO IImsCall*/IInterface* call,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    Int32 code;
    reasonInfo->GetCode(&code);

    if (DBG) mOwner->Log(String("onCallMergeFailed reasonCode=") + StringUtils::ToString(code));
    ((ImsPhone*)mOwner->mPhone.Get())->NotifySuppServiceFailed(IPhoneSuppService_CONFERENCE);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnConferenceParticipantsStateChanged(
    /* [in] */ /*TODO IImsCall*/IInterface* call,
    /* [in] */ IList/*<IConferenceParticipant>*/* participants)
{
    if (DBG) mOwner->Log(String("onConferenceParticipantsStateChanged"));

    AutoPtr<IImsPhoneConnection> conn = mOwner->FindConnection(call);
    if (conn != NULL) {
        IConnection::Probe(conn)->UpdateConferenceParticipants(participants);
    }
    return NOERROR;
}

//=====================================================================
//              ImsPhoneCallTracker::InnerImsCallListener1
//=====================================================================
ImsPhoneCallTracker::InnerImsCallListener1::InnerImsCallListener1(
    /* [in] */ ImsPhoneCallTracker* owner)
    : mOwner(owner)
{
}

ECode ImsPhoneCallTracker::InnerImsCallListener1::OnCallStarted(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    if (DBG) mOwner->Log(String("mImsUssdListener onCallStarted"));

    if (imsCall == mOwner->mUssdSession) {
        if (mOwner->mPendingUssd != NULL) {
            AsyncResult::ForMessage(mOwner->mPendingUssd);
            mOwner->mPendingUssd->SendToTarget();
            mOwner->mPendingUssd = NULL;
        }
    }
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener1::OnCallStartFailed(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    Int32 code;
    reasonInfo->GetCode(&code);
    if (DBG) mOwner->Log(String("mImsUssdListener onCallStartFailed reasonCode=") + StringUtils::ToString(code));

    OnCallTerminated(imsCall, reasonInfo);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener1::OnCallTerminated(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    Int32 code;
    reasonInfo->GetCode(&code);
    if (DBG) mOwner->Log(String("mImsUssdListener onCallTerminated reasonCode=") + StringUtils::ToString(code));

    if (imsCall == mOwner->mUssdSession) {
        mOwner->mUssdSession = NULL;
        if (mOwner->mPendingUssd != NULL) {
            AutoPtr<ICommandException> ex;
// TODO: Need CommandException
            // ex = new CommandException(CommandException.Error.GENERIC_FAILURE);
            AsyncResult::ForMessage(mOwner->mPendingUssd, NULL, IThrowable::Probe(ex));
            mOwner->mPendingUssd->SendToTarget();
            mOwner->mPendingUssd = NULL;
        }
    }
// TODO: Need IImsCall
    // imsCall->Close();
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener1::OnCallUssdMessageReceived(
    /* [in] */ /*TODO IImsCall*/IInterface* call,
    /* [in] */ Int32 mode,
    /* [in] */ const String& ussdMessage)
{
    if (DBG) mOwner->Log(String("mImsUssdListener onCallUssdMessageReceived mode=") +
            StringUtils::ToString(mode));

    Int32 ussdMode = -1;

// TODO: Need IImsCall
    // switch(mode) {
    //     case IImsCall::USSD_MODE_REQUEST:
    //         ussdMode = ICommandsInterface::USSD_MODE_REQUEST;
    //         break;

    //     case IImsCall::USSD_MODE_NOTIFY:
    //         ussdMode = ICommandsInterface::USSD_MODE_NOTIFY;
    //         break;
    // }

    ((ImsPhone*)mOwner->mPhone.Get())->OnIncomingUSSD(ussdMode, ussdMessage);
    return NOERROR;
}

//=====================================================================
//         ImsPhoneCallTracker::InnerImsConnectionStateListener
//=====================================================================
ImsPhoneCallTracker::InnerImsConnectionStateListener::InnerImsConnectionStateListener(
    /* [in] */ ImsPhoneCallTracker* owner)
    : mOwner(owner)
{
}

ECode ImsPhoneCallTracker::InnerImsConnectionStateListener::OnImsConnected()
{
    if (DBG) mOwner->Log(String("onImsConnected"));
    ((ImsPhone*)mOwner->mPhone.Get())->SetServiceState(IServiceState::STATE_IN_SERVICE);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsConnectionStateListener::OnImsDisconnected()
{
    if (DBG) mOwner->Log(String("onImsDisconnected"));
    ((ImsPhone*)mOwner->mPhone.Get())->SetServiceState(IServiceState::STATE_OUT_OF_SERVICE);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsConnectionStateListener::OnImsResumed()
{
    if (DBG) mOwner->Log(String("onImsResumed"));
    ((ImsPhone*)mOwner->mPhone.Get())->SetServiceState(IServiceState::STATE_IN_SERVICE);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsConnectionStateListener::OnImsSuspended()
{
    if (DBG) mOwner->Log(String("onImsSuspended"));
    ((ImsPhone*)mOwner->mPhone.Get())->SetServiceState(IServiceState::STATE_OUT_OF_SERVICE);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsConnectionStateListener::OnFeatureCapabilityChanged(
    /* [in] */ Int32 serviceClass,
    /* [in] */ ArrayOf<Int32>* enabledFeatures,
    /* [in] */ ArrayOf<Int32>* disabledFeatures)
{
// TODO: Need IImsServiceClass
//     if (serviceClass == IImsServiceClass::MMTEL) {
//         Boolean tmpIsVtEnabled = mOwner->mIsVtEnabled;

// // TODO: Need ImsConfig
//         // if ((*disabledFeatures)[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_LTE] ==
//         //         ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_LTE ||
//         //         (*disabledFeatures)[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI] ==
//         //         ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI) {
//         //     mIsVolteEnabled = FALSE;
//         // }
//         // if ((*disabledFeatures)[ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE] ==
//         //         ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE ||
//         //         (*disabledFeatures)[ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI] ==
//         //         ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI) {
//         //     mIsVtEnabled = FALSE;
//         // }
//         // if ((*enabledFeatures)[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_LTE] ==
//         //         ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_LTE ||
//         //         (*enabledFeatures)[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI] ==
//         //         ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI) {
//         //     mIsVolteEnabled = TRUE;
//         // }
//         // if ((*enabledFeatures)[ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE] ==
//         //         ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE ||
//         //         (*enabledFeatures)[ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI] ==
//         //         ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI) {
//         //     mIsVtEnabled = TRUE;
//         // }

//         if (tmpIsVtEnabled != mOwner->mIsVtEnabled) {
//             mPhone->NotifyForVideoCapabilityChanged(mOwner->mIsVtEnabled);
//         }
//     }

    if (DBG) mOwner->Log(String("onFeatureCapabilityChanged, mIsVolteEnabled = ") +
            StringUtils::BooleanToString(mOwner->mIsVolteEnabled)
            + " mIsVtEnabled = " + StringUtils::BooleanToString(mOwner->mIsVtEnabled));
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsConnectionStateListener::OnVoiceMessageCountChanged(
    /* [in] */ Int32 count)
{
    if (DBG) mOwner->Log(String("onVoiceMessageCountChanged :: count=") + StringUtils::ToString(count));
    ((ImsPhone*)mOwner->mPhone.Get())->mDefaultPhone->SetVoiceMessageCount(count);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerThread::Run()
{
    mOwner->GetImsService();
    return NOERROR;
}

//=====================================================================
//                         ImsPhoneCallTracker
//=====================================================================
CAR_INTERFACE_IMPL(ImsPhoneCallTracker, CallTracker, IImsPhoneCallTracker);

const String ImsPhoneCallTracker::LOGTAG("ImsPhoneCallTracker");
const Int32 ImsPhoneCallTracker::MAX_CONNECTIONS;
const Int32 ImsPhoneCallTracker::MAX_CONNECTIONS_PER_CALL;
const Boolean ImsPhoneCallTracker::DBG = TRUE;
const Int32 ImsPhoneCallTracker::EVENT_HANGUP_PENDINGMO;
const Int32 ImsPhoneCallTracker::EVENT_RESUME_BACKGROUND;
const Int32 ImsPhoneCallTracker::EVENT_DIAL_PENDINGMO;
const Int32 ImsPhoneCallTracker::TIMEOUT_HANGUP_PENDINGMO;

ImsPhoneCallTracker::ImsPhoneCallTracker()
    : mState(PhoneConstantsState_IDLE)
    , mIsVolteEnabled(FALSE)
    , mIsVtEnabled(FALSE)
    , mClirMode(ICommandsInterface::CLIR_DEFAULT)
    , mDesiredMute(FALSE)
    , mOnHoldToneStarted(FALSE)
    , mServiceId(-1)
    , mSrvccState(ICallSrvccState_NONE)
    , mIsInEmergencyCall(FALSE)
    , mPendingCallClirMode(0)
    , mPendingCallVideoState(0)
    , mPendingCallInEcm(FALSE)
{
    mReceiver = new InnerBroadcastReceiver(this);
    CArrayList::New((IArrayList**)&mConnections);
    mVoiceCallEndedRegistrants = new RegistrantList();
    mVoiceCallStartedRegistrants = new RegistrantList();
    CImsPhoneCall::New((IImsPhoneCall**)&mRingingCall);
    CImsPhoneCall::New((IImsPhoneCall**)&mForegroundCall);
    CImsPhoneCall::New((IImsPhoneCall**)&mBackgroundCall);
    CImsPhoneCall::New((IImsPhoneCall**)&mHandoverCall);
}

ImsPhoneCallTracker::~ImsPhoneCallTracker()
{
    Log(String("ImsPhoneCallTracker finalized"));
}

ECode ImsPhoneCallTracker::constructor(
    /* [in] */ IImsPhone* phone)
{
    mPhone = phone;

    AutoPtr<IIntentFilter> intentfilter;
    CIntentFilter::New((IIntentFilter**)&intentfilter);
// TODO: Need IImsManager
    // intentfilter->AddAction(IImsManager::ACTION_IMS_INCOMING_CALL);
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    AutoPtr<IIntent> tmp;
    ctx->RegisterReceiver(mReceiver, intentfilter, (IIntent**)&tmp);

    AutoPtr<InnerThread> t = new InnerThread(this);
    t->Start();
    return NOERROR;
}

ECode ImsPhoneCallTracker::Dispose()
{
    if (DBG) Log(String("dispose"));
    mRingingCall->Dispose();
    mBackgroundCall->Dispose();
    mForegroundCall->Dispose();
    mHandoverCall->Dispose();

    ClearDisconnected();
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    ctx->UnregisterReceiver(mReceiver);
    return NOERROR;
}

ECode ImsPhoneCallTracker::RegisterForVoiceCallStarted(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mVoiceCallStartedRegistrants->Add(r);
    return NOERROR;
}

ECode ImsPhoneCallTracker::UnregisterForVoiceCallStarted(
    /* [in] */ IHandler* h)
{
    mVoiceCallStartedRegistrants->Remove(h);
    return NOERROR;
}

ECode ImsPhoneCallTracker::RegisterForVoiceCallEnded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mVoiceCallEndedRegistrants->Add(r);
    return NOERROR;
}

ECode ImsPhoneCallTracker::UnregisterForVoiceCallEnded(
    /* [in] */ IHandler* h)
{
    mVoiceCallEndedRegistrants->Remove(h);
    return NOERROR;
}

ECode ImsPhoneCallTracker::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    return Dial(dialString, videoState, NULL, result);
}

ECode ImsPhoneCallTracker::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&sp);
    Int32 oirMode;
    sp->GetInt32(IPhoneBase::CLIR_KEY, ICommandsInterface::CLIR_DEFAULT, &oirMode);
    return Dial(dialString, oirMode, videoState, extras, result);
}

// synchronized
ECode ImsPhoneCallTracker::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 clirMode,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    Boolean isPhoneInEcmMode;
    SystemProperties::GetBoolean(
            ITelephonyProperties::PROPERTY_INECM_MODE, FALSE, &isPhoneInEcmMode);
    Boolean isEmergencyNumber;
    PhoneNumberUtils::IsEmergencyNumber(dialString, &isEmergencyNumber);

    if (DBG) Log(String("dial clirMode=") + StringUtils::ToString(clirMode));

    // note that this triggers call state changed notif
    ClearDisconnected();

    if (mImsManager == NULL) {
        // throw new CallStateException("service not available");
        return E_CALL_STATE_EXCEPTION;
    }

    Boolean b;
    if (CanDial(&b), !b) {
        // throw new CallStateException("cannot dial in current state");
        return E_CALL_STATE_EXCEPTION;
    }

    if (isPhoneInEcmMode && isEmergencyNumber) {
        HandleEcmTimer(ImsPhone::CANCEL_ECM_TIMER);
    }

    Boolean holdBeforeDial = FALSE;

    // The new call must be assigned to the foreground call.
    // That call must be idle, so place anything that's
    // there on hold
    ICallState foregroundCallState;
    ICall::Probe(mForegroundCall)->GetState(&foregroundCallState);
    ICallState backgroundCallState;
    ICall::Probe(mBackgroundCall)->GetState(&backgroundCallState);
    if (foregroundCallState == ICallState_ACTIVE) {
        if (backgroundCallState != ICallState_IDLE) {
            //we should have failed in !canDial() above before we get here
            // throw new CallStateException("cannot dial in current state");
            return E_CALL_STATE_EXCEPTION;
        }
        // foreground call is empty for the newly dialed connection
        holdBeforeDial = TRUE;
        // Cache the video state for pending MO call.
        mPendingCallVideoState = videoState;
        SwitchWaitingOrHoldingAndActive();
    }

    ICallState fgState = ICallState_IDLE;
    ICallState bgState = ICallState_IDLE;

    mClirMode = clirMode;

    {
        AutoLock lock(mSyncHold);
        if (holdBeforeDial) {
            ICall::Probe(mForegroundCall)->GetState(&fgState);
            ICall::Probe(mBackgroundCall)->GetState(&bgState);

            //holding foreground call failed
            if (fgState == ICallState_ACTIVE) {
                // throw new CallStateException("cannot dial in current state");
                return E_CALL_STATE_EXCEPTION;
            }

            //holding foreground call succeeded
            if (bgState == ICallState_HOLDING) {
                holdBeforeDial = FALSE;
            }
        }

        AutoPtr<IContext> ctx;
        IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
        CImsPhoneConnection::New(ctx,
                CheckForTestEmergencyNumber(dialString), this,
                mForegroundCall, extras, (IImsPhoneConnection**)&mPendingMO);
    }
    AddConnection(mPendingMO);

    if (!holdBeforeDial) {
        if ((!isPhoneInEcmMode) || (isPhoneInEcmMode && isEmergencyNumber)) {
            DialInternal(mPendingMO, clirMode, videoState, extras);
        }
        else {
            // try {
// TODO: Need IImsEcbm
            // GetEcbmInterface()->ExitEmergencyCallbackMode();
            // } catch (ImsException e) {
            //     e.printStackTrace();
            //     throw new CallStateException("service not available");
            // }
            mPhone->SetOnEcbModeExitResponse(this, EVENT_EXIT_ECM_RESPONSE_CDMA, NULL);
            mPendingCallClirMode = clirMode;
            mPendingCallVideoState = videoState;
            mPendingCallInEcm = TRUE;
        }
    }

    UpdatePhoneState();
    ((ImsPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();

    *result = IConnection::Probe(mPendingMO);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneCallTracker::AddParticipant(
    /* [in] */ const String& dialString)
{
    if (mForegroundCall != NULL) {
// TODO: Need IImsCall
        // AutoPtr<IImsCall> imsCall;
        // mForegroundCall->GetImsCall((IImsCall**)&imsCall);
        // if (imsCall == NULL) {
        //     Loge(String("addParticipant : No foreground ims call"));
        // }
        // else {
        //     AutoPtr<IImsCallSession> imsCallSession;
        //     imsCall->GetCallSession((IImsCallSession**)&imsCallSession);
        //     if (imsCallSession != NULL) {
        //         String[] callees = new String[] { dialString };
        //         imsCallSession->InviteParticipants(callees);
        //     }
        //     else {
        //         Loge(String("addParticipant : ImsCallSession does not exist"));
        //     }
        // }
    }
    else {
        Loge(String("addParticipant : Foreground call does not exist"));
    }
    return NOERROR;
}

ECode ImsPhoneCallTracker::AcceptCall(
    /* [in] */ Int32 videoState)
{
    if (DBG) Log(String("acceptCall"));

    ICallState foregroundCallState;
    ICall::Probe(mForegroundCall)->GetState(&foregroundCallState);
    ICallState backgroundCallState;
    ICall::Probe(mBackgroundCall)->GetState(&backgroundCallState);
    ICallState ringingCallState;
    ICall::Probe(mRingingCall)->GetState(&ringingCallState);

// TODO: Need ICallState::IsAlive
//     if (foregroundCallState->IsAlive()
//             && backgroundCallState->IsAlive()) {
//         // throw new CallStateException("cannot accept call");
//         return E_CALL_STATE_EXCEPTION;
//     }

//     if ((ringingCallState == ICallState_WAITING)
//             && foregroundCallState->IsAlive()) {
//         SetMute(FALSE);
//         // Cache video state for pending MT call.
//         mPendingCallVideoState = videoState;
//         SwitchWaitingOrHoldingAndActive();
//     }
//     else if (ringingCallState.isRinging()) {
//         if (DBG) Log(String("acceptCall: incoming..."));
//         // Always unmute when answering a new call
//         SetMute(FALSE);
//         // try {
// // TODO: Need IImsCall
//         // ImsCall imsCall = mRingingCall.getImsCall();
//         // if (imsCall != NULL) {
//         //     imsCall.accept(ImsCallProfile.getCallTypeFromVideoState(videoState));
//         // }
//         // else {
//         //     // throw new CallStateException("no valid ims call");
//         //     return E_CALL_STATE_EXCEPTION;
//         // }
//         // } catch (ImsException e) {
//         //     // throw new CallStateException("cannot accept call");
//         //     return E_CALL_STATE_EXCEPTION;
//         // }
//     }
//     else {
//         // throw new CallStateException("phone not ringing");
//         return E_CALL_STATE_EXCEPTION;
//     }
    return NOERROR;
}

ECode ImsPhoneCallTracker::DeflectCall(
    /* [in] */ const String& number)
{
    if (DBG) Log(String("deflectCall"));

    ICallState ringingCallState;
    ICall::Probe(mRingingCall)->GetState(&ringingCallState);

// TODO: Need ICallState::IsRinging
//     if (ringingCallState->IsRinging()) {
//         // try {
// // TODO: Need IImsCall
//         // ImsCall imsCall = mRingingCall.getImsCall();
//         // if (imsCall != NULL) {
//         //     imsCall.deflect(number);
//         // }
//         // else {
//         //     // throw new CallStateException("no valid ims call to deflect");
//         //     return E_CALL_STATE_EXCEPTION;
//         // }
//         // } catch (ImsException e) {
//         //     throw new CallStateException("cannot deflect call");
//         // }
//     }
//     else {
//         // throw new CallStateException("phone not ringing");
//         return E_CALL_STATE_EXCEPTION;
//     }
    return NOERROR;
}

ECode ImsPhoneCallTracker::RejectCall()
{
    if (DBG) Log(String("rejectCall"));

    ICallState ringingCallState;
    ICall::Probe(mRingingCall)->GetState(&ringingCallState);

// TODO: Need ICallState::IsRinging
    // if (ringingCallState->IsRinging()) {
    //     Hangup(mRingingCall);
    // }
    // else {
    //     // throw new CallStateException("phone not ringing");
    //     return E_CALL_STATE_EXCEPTION;
    // }
    return NOERROR;
}

ECode ImsPhoneCallTracker::SwitchWaitingOrHoldingAndActive()
{
    if (DBG) Log(String("switchWaitingOrHoldingAndActive"));

    ICallState ringingCallState;
    ICall::Probe(mRingingCall)->GetState(&ringingCallState);
    if (ringingCallState == ICallState_INCOMING) {
        // throw new CallStateException("cannot be in the incoming state");
        return E_CALL_STATE_EXCEPTION;
    }

    ICallState foregroundCallState;
    ICall::Probe(mForegroundCall)->GetState(&foregroundCallState);
    ICallState backgroundCallState;
    ICall::Probe(mBackgroundCall)->GetState(&backgroundCallState);

    if (foregroundCallState == ICallState_ACTIVE) {
// TODO: Need ImsCall
        // ImsCall imsCall = mForegroundCall.getImsCall();
        // if (imsCall == NULL) {
        //     // throw new CallStateException("no ims call");
        //     return E_CALL_STATE_EXCEPTION;
        // }

        ((ImsPhoneCall*)mForegroundCall.Get())->SwitchWith(mBackgroundCall);

        // try {
// TODO: Need ImsCall
        // imsCall->Hold();
        // } catch (ImsException e) {
        //     mForegroundCall.switchWith(mBackgroundCall);
        //     throw new CallStateException(e.getMessage());
        // }
    }
    else if (backgroundCallState == ICallState_HOLDING) {
        ResumeWaitingOrHolding();
    }
    return NOERROR;
}

ECode ImsPhoneCallTracker::Conference()
{
    if (DBG) Log(String("conference"));

// TODO: Need ImsCall
    // ImsCall fgImsCall = mForegroundCall.getImsCall();
    // if (fgImsCall == NULL) {
    //     Log(String("conference no foreground ims call"));
    //     return NOERROR;
    // }

    // ImsCall bgImsCall = mBackgroundCall.getImsCall();
    // if (bgImsCall == NULL) {
    //     Log(String("conference no background ims call"));
    //     return NOERROR;
    // }

    // try {
// TODO: Need ImsCall
    // fgImsCall->Merge(bgImsCall);
    // } catch (ImsException e) {
    //     Log("conference " + e.getMessage());
    // }
    return NOERROR;
}

ECode ImsPhoneCallTracker::ExplicitCallTransfer()
{
    //TODO : implement
    return NOERROR;
}

ECode ImsPhoneCallTracker::ClearDisconnected()
{
    if (DBG) Log(String("clearDisconnected"));

    InternalClearDisconnected();

    UpdatePhoneState();
    ((ImsPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();
    return NOERROR;
}

ECode ImsPhoneCallTracker::CanConference(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ICallState foregroundCallState;
    ICall::Probe(mForegroundCall)->GetState(&foregroundCallState);
    ICallState backgroundCallState;
    ICall::Probe(mBackgroundCall)->GetState(&backgroundCallState);

    Boolean b1, b2;
    *result = foregroundCallState == ICallState_ACTIVE
        && backgroundCallState == ICallState_HOLDING
        && (((ImsPhoneCall*)mBackgroundCall.Get())->IsFull(&b1), b1)
        && (((ImsPhoneCall*)mForegroundCall.Get())->IsFull(&b2), b2);
    return NOERROR;
}

ECode ImsPhoneCallTracker::CanDial(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean ret = FALSE;
    Int32 serviceState;
    AutoPtr<IServiceState> ss;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&ss);
    Int32 state;
    ss->GetState(&state);
    String disableCall;
    SystemProperties::Get(
            ITelephonyProperties::PROPERTY_DISABLE_CALL, String("FALSE"), &disableCall);

    // Log::D(LOGTAG, "canDial(): "
    //         + "\nserviceState = " + serviceState
    //         + "\npendingMO == NULL::=" + String.valueOf(mPendingMO == NULL)
    //         + "\nringingCall: " + mRingingCall.getState()
    //         + "\ndisableCall = " + disableCall
    //         + "\nforegndCall: " + mForegroundCall.getState()
    //         + "\nbackgndCall: " + mBackgroundCall.getState());

    ICallState foregroundCallState;
    ICall::Probe(mForegroundCall)->GetState(&foregroundCallState);
    ICallState backgroundCallState;
    ICall::Probe(mBackgroundCall)->GetState(&backgroundCallState);

// TODO: Need ICallState::isRinging,isAlive
    // ret = (serviceState != IServiceState::STATE_POWER_OFF)
    //     && mPendingMO == NULL
    //     && !mRingingCall.isRinging()
    //     && !disableCall.Equals("TRUE")
    //     && (!foregroundCallState.isAlive()
    //             || !backgroundCallState.isAlive());

    *result = ret;
    return NOERROR;
}

ECode ImsPhoneCallTracker::CanTransfer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    ICallState foregroundCallState;
    ICall::Probe(mForegroundCall)->GetState(&foregroundCallState);
    ICallState backgroundCallState;

    ICall::Probe(mBackgroundCall)->GetState(&backgroundCallState);
    *result = foregroundCallState == ICallState_ACTIVE
        && backgroundCallState == ICallState_HOLDING;
    return NOERROR;
}

ECode ImsPhoneCallTracker::SetUiTTYMode(
    /* [in] */ Int32 uiTtyMode,
    /* [in] */ IMessage* onComplete)
{
    // try {
// TODO: Need IImsManager
    // mImsManager->SetUiTTYMode(mServiceId, uiTtyMode, onComplete);
    // } catch (ImsException e) {
    //     Loge("setTTYMode : " + e);
    //     mPhone.sendErrorResponse(onComplete, e);
    // }
    return NOERROR;
}

ECode ImsPhoneCallTracker::SetMute(
    /* [in] */ Boolean mute)
{
    mDesiredMute = mute;
    ((ImsPhoneCall*)mForegroundCall.Get())->SetMute(mute);
    return NOERROR;
}

ECode ImsPhoneCallTracker::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDesiredMute;
    return NOERROR;
}

ECode ImsPhoneCallTracker::SendDtmf(
    /* [in] */ Char16 c)
{
    if (DBG) Log(String("sendDtmf"));

// TODO: Need ImsCall
    // ImsCall imscall = mForegroundCall.getImsCall();
    // if (imscall != NULL) {
    //     imscall.sendDtmf(c);
    // }
    return NOERROR;
}

ECode ImsPhoneCallTracker::StartDtmf(
    /* [in] */ Char16 c)
{
    if (DBG) Log(String("startDtmf"));

// TODO: Need ImsCall
    // ImsCall imscall = mForegroundCall.getImsCall();
    // if (imscall != NULL) {
    //     imscall.startDtmf(c);
    // }
    // else {
    //     Loge(String("startDtmf : no foreground call"));
    // }
    return NOERROR;
}

ECode ImsPhoneCallTracker::StopDtmf()
{
    if (DBG) Log(String("stopDtmf"));

// TODO: Need ImsCall
    // ImsCall imscall = mForegroundCall.getImsCall();
    // if (imscall != NULL) {
    //     imscall.stopDtmf();
    // }
    // else {
    //     Loge(String("stopDtmf : no foreground call"));
    // }
    return NOERROR;
}

ECode ImsPhoneCallTracker::Hangup(
    /* [in] */ IImsPhoneConnection* conn)
{
    if (DBG) Log(String("hangup connection"));

    AutoPtr<IImsPhoneCallTracker> ct;
    conn->GetOwner((IImsPhoneCallTracker**)&ct);
    if (ct.Get() != (IImsPhoneCallTracker*)this) {
        // throw new CallStateException ("ImsPhoneConnection " + conn
        //         + "does not belong to ImsPhoneCallTracker " + this);
        return E_CALL_STATE_EXCEPTION;
    }

    AutoPtr<ICall> call;
    IConnection::Probe(conn)->GetCall((ICall**)&call);
    Hangup(IImsPhoneConnection::Probe(call));
    return NOERROR;
}

ECode ImsPhoneCallTracker::Hangup(
    /* [in] */ IImsPhoneCall* call)
{
    if (DBG) Log(String("hangup call"));

    AutoPtr<IList> conns;
    ICall::Probe(call)->GetConnections((IList**)&conns);
    Int32 size;
    conns->GetSize(&size);
    if (size == 0) {
        // throw new CallStateException("no connections");
        return E_CALL_STATE_EXCEPTION;
    }

// TODO: Need IImsCall
    // ImsCall imsCall = call.getImsCall();
    Boolean rejectCall = FALSE;

    if (call == mRingingCall) {
        if (IPhone::DEBUG_PHONE) Log(String("(ringing) hangup incoming"));
        rejectCall = TRUE;
    }
    else if (call == mForegroundCall) {
        Boolean b;
        if (ICall::Probe(call)->IsDialingOrAlerting(&b), b) {
            if (IPhone::DEBUG_PHONE) {
                Log(String("(foregnd) hangup dialing or alerting..."));
            }
        }
        else {
            if (IPhone::DEBUG_PHONE) {
                Log(String("(foregnd) hangup foreground"));
            }
            //held call will be resumed by onCallTerminated
        }
    }
    else if (call == mBackgroundCall) {
        if (IPhone::DEBUG_PHONE) {
            Log(String("(backgnd) hangup waiting or background"));
        }
    }
    else {
        // throw new CallStateException ("ImsPhoneCall " + call +
        //         "does not belong to ImsPhoneCallTracker " + this);
        return E_CALL_STATE_EXCEPTION;
    }

    ((ImsPhoneCall*)call)->OnHangupLocal();

    // try {
// TODO: Need IImsCall
    // if (imsCall != NULL) {
    //     if (rejectCall) imsCall.reject(IImsReasonInfo::CODE_USER_DECLINE);

    //     else imsCall.terminate(IImsReasonInfo::CODE_USER_TERMINATED);
    // }
    // else if (mPendingMO != NULL && call == mForegroundCall) {
    //     // is holding a foreground call
    //     mPendingMO->Update(NULL, ICallState_DISCONNECTED);
    //     mPendingMO->OnDisconnect();
    //     RemoveConnection(mPendingMO);
    //     mPendingMO = NULL;
    //     UpdatePhoneState();
    //     RemoveMessages(EVENT_DIAL_PENDINGMO);
    // }
    // } catch (ImsException e) {
    //     throw new CallStateException(e.getMessage());
    // }

    ((ImsPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();
    return NOERROR;
}

ECode ImsPhoneCallTracker::ResumeWaitingOrHolding()
{
    if (DBG) Log(String("resumeWaitingOrHolding"));

    // try {
    ICallState foregroundCallState;
    ICall::Probe(mForegroundCall)->GetState(&foregroundCallState);
    ICallState ringingCallState;
    ICall::Probe(mRingingCall)->GetState(&ringingCallState);

// TODO: Need IImsCall
    // if (foregroundCallState.isAlive()) {
    //     //resume foreground call after holding background call
    //     //they were switched before holding
    //     ImsCall imsCall = mForegroundCall.getImsCall();
    //     if (imsCall != NULL) imsCall.resume();
    // } else if (ringingCallState == ICallState_WAITING) {
    //     //accept waiting call after holding background call
    //     ImsCall imsCall = mRingingCall.getImsCall();
    //     if (imsCall != NULL) {
    //         imsCall.accept(
    //             ImsCallProfile.getCallTypeFromVideoState(mPendingCallVideoState));
    //     }
    // } else {
    //     //Just resume background call.
    //     //To distinguish resuming call with swapping calls
    //     //we do not switch calls.here
    //     //ImsPhoneConnection.update will chnage the parent when completed
    //     ImsCall imsCall = mBackgroundCall.getImsCall();
    //     if (imsCall != NULL) imsCall.resume();
    // }
    // } catch (ImsException e) {
    //     throw new CallStateException(e.getMessage());
    // }
    return NOERROR;
}

ECode ImsPhoneCallTracker::SendUSSD(
    /* [in] */ const String& ussdString,
    /* [in] */ IMessage* response)
{
    if (DBG) Log(String("sendUSSD"));

    // try {
    if (mUssdSession != NULL) {
// TODO: Need IImsCall
        // mUssdSession->SendUssd(ussdString);
        AsyncResult::ForMessage(response, NULL, NULL);
        response->SendToTarget();
        return NOERROR;
    }

    AutoPtr<ArrayOf<String> > callees = ArrayOf<String>::Alloc(1);
    (*callees)[0] = ussdString;
    AutoPtr<IImsCallProfile> profile;
// TODO: Need IImsManager
    // mImsManager->CreateCallProfile(mServiceId,
    //         IImsCallProfile::SERVICE_TYPE_NORMAL, IImsCallProfile::CALL_TYPE_VOICE,
    //         (IImsCallProfile**)&profile);
    profile->SetCallExtraInt32(IImsCallProfile::EXTRA_DIALSTRING,
            IImsCallProfile::DIALSTRING_USSD);

// TODO: Need IImsManager
    // mUssdSession = mImsManager->MakeCall(mServiceId, profile,
    //         callees, mImsUssdListener);
    // } catch (ImsException e) {
    //     Loge("sendUSSD : " + e);
    //     mPhone.sendErrorResponse(response, e);
    // }
    return NOERROR;
}

ECode ImsPhoneCallTracker::CancelUSSD()
{
    if (mUssdSession == NULL) return NOERROR;

    // try {
// TODO: Need IImsCall
    // mUssdSession->Terminate(IImsReasonInfo::CODE_USER_TERMINATED);
    // } catch (ImsException e) {
    // }
    return NOERROR;
}

ECode ImsPhoneCallTracker::GetUtInterface(
    /* [out] */ /*TODO IImsUtInterface*/IInterface** result)
{
    VALIDATE_NOT_NULL(result);
// TODO: Need IImsManager
    // if (mImsManager == NULL) {
    //     throw new ImsException("no ims manager", IImsReasonInfo::CODE_UNSPECIFIED);
    // }

    // ImsUtInterface ut = mImsManager.getSupplementaryServiceConfiguration(mServiceId);
    // *result = ut;
    // REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneCallTracker::NotifySrvccState(
    /* [in] */ ICallSrvccState state)
{
    if (DBG) Log(String("notifySrvccState state=") + StringUtils::ToString(state));

    mSrvccState = state;

    if (mSrvccState == ICallSrvccState_COMPLETED) {
        TransferHandoverConnections(mForegroundCall);
        TransferHandoverConnections(mBackgroundCall);
        TransferHandoverConnections(mRingingCall);
        // release wake lock hold
        AutoPtr<IImsPhoneConnection> con;
        ((ImsPhoneCall*)mHandoverCall.Get())->GetHandoverConnection((IImsPhoneConnection**)&con);
        if (con != NULL) {
            ((ImsPhoneConnection*)con.Get())->ReleaseWakeLock();
        }
    }
    return NOERROR;
}

ECode ImsPhoneCallTracker::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;
    Int32 what;
    msg->GetWhat(&what);
    if (DBG) Log(String("handleMessage what=") + StringUtils::ToString(what));

    switch (what) {
        case EVENT_HANGUP_PENDINGMO:
            if (mPendingMO != NULL) {
                Boolean b;
                ((ImsPhoneConnection*)mPendingMO.Get())->OnDisconnect(&b);
                RemoveConnection(mPendingMO);
                mPendingMO = NULL;
            }

            UpdatePhoneState();
            ((ImsPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();
            break;
        case EVENT_RESUME_BACKGROUND:
            // try {
            ResumeWaitingOrHolding();
            // } catch (CallStateException e) {
            //     if (IPhone::DEBUG_PHONE) {
            //         Loge("handleMessage EVENT_RESUME_BACKGROUND exception=" + e);
            //     }
            // }
            break;
        case EVENT_DIAL_PENDINGMO: {
            AutoPtr<IBundle> bundle;
            mPendingMO->GetCallExtras((IBundle**)&bundle);
            DialInternal(mPendingMO, mClirMode, mPendingCallVideoState, bundle);
            break;
        }
        case EVENT_EXIT_ECM_RESPONSE_CDMA: {
            // no matter the result, we still do the same here
            if (mPendingCallInEcm) {
                AutoPtr<IBundle> bundle;
                mPendingMO->GetCallExtras((IBundle**)&bundle);
                DialInternal(mPendingMO, mPendingCallClirMode,
                        mPendingCallVideoState, bundle);
                mPendingCallInEcm = FALSE;
            }
            mPhone->UnsetOnEcbModeExitResponse(this);
            break;
        }
    }
    return NOERROR;
}

ECode ImsPhoneCallTracker::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("ImsPhoneCallTracker extends:"));
    CallTracker::Dump(fd, pw, args);
    // pw->Println(String(" mVoiceCallEndedRegistrants=") + mVoiceCallEndedRegistrants);
    // pw->Println(String(" mVoiceCallStartedRegistrants=") + mVoiceCallStartedRegistrants);
    // pw->Println(String(" mRingingCall=") + mRingingCall);
    // pw->Println(String(" mForegroundCall=") + mForegroundCall);
    // pw->Println(String(" mBackgroundCall=") + mBackgroundCall);
    // pw->Println(String(" mHandoverCall=") + mHandoverCall);
    // pw->Println(String(" mPendingMO=") + mPendingMO);
    // //pw->Println(String(" mHangupPendingMO=") + mHangupPendingMO);
    // pw->Println(String(" mPhone=") + mPhone);
    // pw->Println(String(" mDesiredMute=") + mDesiredMute);
    pw->Println(String(" mState=") + StringUtils::ToString(mState));
    return NOERROR;
}

ECode ImsPhoneCallTracker::GetEcbmInterface(
    /* [out] */ /*TODO IImsEcbm*/IInterface** result)
{
    VALIDATE_NOT_NULL(result);
// TODO: Need IImsEcbm,ImsManager
    // if (mImsManager == NULL) {
    //     throw new ImsException("no ims manager", IImsReasonInfo::CODE_UNSPECIFIED);
    // }

    // AutoPtr<IImsEcbm> ecbm = mImsManager->GetEcbmInterface(mServiceId);
    // *result = ecbm;
    // REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneCallTracker::IsInEmergencyCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsInEmergencyCall;
    return NOERROR;
}

ECode ImsPhoneCallTracker::IsVolteEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsVolteEnabled;
    return NOERROR;
}

ECode ImsPhoneCallTracker::IsVtEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsVtEnabled;
    return NOERROR;
}

void ImsPhoneCallTracker::Log(
    /* [in] */ const String& msg)
{
    Logger::D(LOGTAG, "[ImsPhoneCallTracker] %s", msg.string());
}

void ImsPhoneCallTracker::Loge(
    /* [in] */ const String& msg)
{
    Logger::E(LOGTAG, "[ImsPhoneCallTracker] %s", msg.string());
}

void ImsPhoneCallTracker::HandlePollCalls(
    /* [in] */ AsyncResult* ar)
{
    return;
}

AutoPtr<IPendingIntent> ImsPhoneCallTracker::CreateIncomingCallPendingIntent()
{
    AutoPtr<IIntent> intent;
// TODO: Need IImsManager
    // CIntent::New(IImsManager::ACTION_IMS_INCOMING_CALL, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);

    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);

    AutoPtr<IPendingIntent> pi;
    CPendingIntent::GetBroadcast(ctx, 0, intent,
            IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&pi);
    return pi;
}

void ImsPhoneCallTracker::GetImsService()
{
    if (DBG) Log(String("getImsService"));

    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    Int64 sid;
    IPhone::Probe(mPhone)->GetSubId(&sid);
// TODO: Need ImsManager
    // mImsManager = ImsManager::GetInstance(ctx, sid);

    // try {
// TODO: Need ImsManager
    // mServiceId = mImsManager->Open(IImsServiceClass::MMTEL,
    //         CreateIncomingCallPendingIntent(),
    //         mImsConnectionStateListener);

    // Get the ECBM interface and set IMSPhone's listener object for notifications
// TODO: Need IImsEcbm
    // GetEcbmInterface().setEcbmStateListener(mPhone.mImsEcbmStateListener);
    Boolean b;
    if (mPhone->IsInEcm(&b), b) {
        // Call exit ECBM which will invoke onECBMExited
        IPhone::Probe(mPhone)->ExitEmergencyCallbackMode();
    }
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    Int32 mPreferredTtyMode;
    Settings::Secure::GetInt32(
        cr, ISettingsSecure::PREFERRED_TTY_MODE,
        IPhone::TTY_MODE_OFF, &mPreferredTtyMode);
// TODO: Need ImsManager
   // mImsManager->SetUiTTYMode(mServiceId, mPreferredTtyMode, NULL);

    // } catch (ImsException e) {
    //     Loge("getImsService: " + e);
    //     //Leave mImsManager as NULL, then CallStateException will be thrown when dialing
    //     mImsManager = NULL;
    // }
}

void ImsPhoneCallTracker::HandleEcmTimer(
    /* [in] */ Int32 action)
{
    ((ImsPhone*)mPhone.Get())->HandleTimerInEmergencyCallbackMode(action);
    switch (action) {
        case ImsPhone::CANCEL_ECM_TIMER:
            break;
        case ImsPhone::RESTART_ECM_TIMER:
            break;
        default:
            Log(String("handleEcmTimer, unsupported action ") + StringUtils::ToString(action));
    }
}

void ImsPhoneCallTracker::DialInternal(
    /* [in] */ IImsPhoneConnection* conn,
    /* [in] */ Int32 clirMode,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras)
{
    if (conn == NULL) {
        return;
    }

    Boolean isConferenceUri = FALSE;
    Boolean isSkipSchemaParsing = FALSE;

    if (extras != NULL) {
        extras->GetBoolean(ITelephonyProperties::EXTRA_DIAL_CONFERENCE_URI, FALSE, &isConferenceUri);
        extras->GetBoolean(ITelephonyProperties::EXTRA_SKIP_SCHEMA_PARSING, FALSE, &isSkipSchemaParsing);
    }

    String addr;
    IConnection::Probe(conn)->GetAddress(&addr);
    if (!isConferenceUri && !isSkipSchemaParsing && (addr.IsNull()
            || addr.GetLength() == 0
            || addr.IndexOf(IPhoneNumberUtils::WILD) >= 0)) {
        // Phone number is invalid
        conn->SetDisconnectCause(IDisconnectCause::INVALID_NUMBER);
        Boolean b;
        SendEmptyMessageDelayed(EVENT_HANGUP_PENDINGMO, TIMEOUT_HANGUP_PENDINGMO, &b);
        return;
    }

    // Always unmute when initiating a new call
    SetMute(FALSE);
    Boolean b;
    PhoneNumberUtils::IsEmergencyNumber(addr, &b);
    Int32 serviceType = b ?
            IImsCallProfile::SERVICE_TYPE_EMERGENCY : IImsCallProfile::SERVICE_TYPE_NORMAL;
    Int32 callType;
    CImsCallProfile::GetCallTypeFromVideoState(videoState, &callType);
    //TODO(vt): Is this sufficient?  At what point do we know the video state of the call?
    IConnection::Probe(conn)->SetVideoState(videoState);

    // try {
    AutoPtr<ArrayOf<String> > callees = ArrayOf<String>::Alloc(1);
    (*callees)[0] = addr;
    AutoPtr<IImsCallProfile> profile;
// TODO: Need IImsManager
    // mImsManager->CreateCallProfile(mServiceId, serviceType, callType, (IImsCallProfile**)&profile);
    profile->SetCallExtraInt32(IImsCallProfile::EXTRA_OIR, clirMode);
    profile->SetCallExtraBoolean(ITelephonyProperties::EXTRAS_IS_CONFERENCE_URI,
            isConferenceUri);

    if (extras != NULL) {
        // Pack the OEM-specific call extras.
        ((CImsCallProfile*)profile.Get())->mCallExtras->PutBundle(
                IImsCallProfile::EXTRA_OEM_EXTRAS, extras);
        Log(String("Packing OEM extras bundle in call profile."));
    }
    else {
        Log(String("No dial extras packed in call profile."));
    }

// TODO: Need IImsCall
    // ImsCall imsCall = mImsManager->MakeCall(mServiceId, profile,
    //         callees, mImsCallListener);
    // conn->SetImsCall(imsCall);

    // IImsVideoCallProvider imsVideoCallProvider =
    //         imsCall.getCallSession().getVideoCallProvider();
    // if (imsVideoCallProvider != NULL) {
    //     ImsVideoCallProviderWrapper imsVideoCallProviderWrapper =
    //             new ImsVideoCallProviderWrapper(imsVideoCallProvider);
    //     conn->SetVideoProvider(imsVideoCallProviderWrapper);
    // }
    // } catch (ImsException e) {
    //     Loge("dialInternal : " + e);
    //     conn.setDisconnectCause(IDisconnectCause::ERROR_UNSPECIFIED);
    //     SendEmptyMessageDelayed(EVENT_HANGUP_PENDINGMO, TIMEOUT_HANGUP_PENDINGMO);
    // } catch (RemoteException e) {
    // }
}

void ImsPhoneCallTracker::InternalClearDisconnected()
{
    ((ImsPhoneCall*)mRingingCall.Get())->ClearDisconnected();
    ((ImsPhoneCall*)mForegroundCall.Get())->ClearDisconnected();
    ((ImsPhoneCall*)mBackgroundCall.Get())->ClearDisconnected();
    ((ImsPhoneCall*)mHandoverCall.Get())->ClearDisconnected();
}

void ImsPhoneCallTracker::UpdatePhoneState()
{
    PhoneConstantsState oldState = mState;

// TODO: Need ICallState::isRinging,isIdle
    // if (mRingingCall.isRinging()) {
    //     mState = PhoneConstantsState_RINGING;
    // }
    // else if (mPendingMO != NULL ||
    //         !(mForegroundCall.isIdle() && mBackgroundCall.isIdle())) {
    //     mState = PhoneConstantsState_OFFHOOK;
    // }
    // else {
    //     mState = PhoneConstantsState_IDLE;
    // }

    if (mState == PhoneConstantsState_IDLE && oldState != mState) {
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, NULL, NULL);
        mVoiceCallEndedRegistrants->NotifyRegistrants(ar);
    }
    else if (oldState == PhoneConstantsState_IDLE && oldState != mState) {
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, NULL, NULL);
        mVoiceCallStartedRegistrants->NotifyRegistrants(ar);
    }

    if (DBG) Log(String("updatePhoneState oldState=") +
            StringUtils::ToString(oldState) +
            ", newState=" + StringUtils::ToString(mState));

    if (mState != oldState) {
        ((ImsPhone*)mPhone.Get())->NotifyPhoneStateChanged();
    }
}

void ImsPhoneCallTracker::HandleRadioNotAvailable()
{
    // handlePollCalls will clear out its
    // call list when it gets the CommandException
    // error result from this
    PollCallsWhenSafe();
}

void ImsPhoneCallTracker::DumpState()
{
    AutoPtr<IList> l;

    Log(String("Phone State:") + StringUtils::ToString(mState));

    String str;
    ((ImsPhoneCall*)mRingingCall.Get())->ToString(&str);
    Log(String("Ringing call: ") + str);

    ((ImsPhoneCall*)mRingingCall.Get())->GetConnections((IList**)&l);
    Int32 size;
    l->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        l->Get(i, (IInterface**)&obj);
        IObject::Probe(obj)->ToString(&str);
        Log(str);
    }

    ((ImsPhoneCall*)mForegroundCall.Get())->ToString(&str);
    Log(String("Foreground call: ") + str);

    l = NULL;
    ((ImsPhoneCall*)mForegroundCall.Get())->GetConnections((IList**)&l);
    l->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        l->Get(i, (IInterface**)&obj);
        IObject::Probe(obj)->ToString(&str);
        Log(str);
    }

    ((ImsPhoneCall*)mBackgroundCall.Get())->ToString(&str);
    Log(String("Background call: ") + str);

    l = NULL;
    ((ImsPhoneCall*)mBackgroundCall.Get())->GetConnections((IList**)&l);
    l->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        l->Get(i, (IInterface**)&obj);
        IObject::Probe(obj)->ToString(&str);
        Log(str);
    }
}

// synchronized
AutoPtr<IImsPhoneConnection> ImsPhoneCallTracker::FindConnection(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    // for (ImsPhoneConnection conn : mConnections) {
    //     if (conn.getImsCall() == imsCall) {
    //         return conn;
    //     }
    // }
    // return NULL;
    AutoPtr<IImsPhoneConnection> empty;
    return empty;
}

// synchronized
void ImsPhoneCallTracker::RemoveConnection(
    /* [in] */ IImsPhoneConnection* conn)
{
    mConnections->Remove(conn);
}

// synchronized
void ImsPhoneCallTracker::AddConnection(
    /* [in] */ IImsPhoneConnection* conn)
{
    mConnections->Add(conn);
}

void ImsPhoneCallTracker::ProcessCallStateChange(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ ICallState state,
    /* [in] */ Int32 cause)
{
    if (DBG) Log(String("processCallStateChange state=") +
            StringUtils::ToString(state) +
            " cause=" + StringUtils::ToString(cause));

    if (imsCall == NULL) return;

    Boolean changed = FALSE;
    AutoPtr<IImsPhoneConnection> conn = FindConnection(imsCall);

    if (conn == NULL) {
        // TODO : what should be done?
        return;
    }

    ((ImsPhoneConnection*)conn.Get())->Update(imsCall, state, &changed);

    if (state == ICallState_DISCONNECTED) {
        Boolean b;
        changed = (((ImsPhoneConnection*)conn.Get())->OnDisconnect(cause, &b), b) || changed;
        RemoveConnection(conn);
    }

    if (changed) {
        AutoPtr<ICall> call;
        IConnection::Probe(conn)->GetCall((ICall**)&call);
        if (call.Get() == ICall::Probe(mHandoverCall)) return;
        UpdatePhoneState();
        ((ImsPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();
    }
}

Int32 ImsPhoneCallTracker::GetDisconnectCauseFromReasonInfo(
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    Int32 cause = IDisconnectCause::ERROR_UNSPECIFIED;

    //int type = reasonInfo.getReasonType();
    Int32 code;
    reasonInfo->GetCode(&code);

    switch (code) {
        case IImsReasonInfo::CODE_SIP_BAD_ADDRESS:
        case IImsReasonInfo::CODE_SIP_NOT_REACHABLE:
            return IDisconnectCause::NUMBER_UNREACHABLE;

        case IImsReasonInfo::CODE_SIP_BUSY:
            return IDisconnectCause::BUSY;

        case IImsReasonInfo::CODE_USER_TERMINATED:
            return IDisconnectCause::LOCAL;

        case IImsReasonInfo::CODE_USER_TERMINATED_BY_REMOTE:
            return IDisconnectCause::NORMAL;

        case IImsReasonInfo::CODE_SIP_REDIRECTED:
        case IImsReasonInfo::CODE_SIP_BAD_REQUEST:
        case IImsReasonInfo::CODE_SIP_FORBIDDEN:
        case IImsReasonInfo::CODE_SIP_NOT_ACCEPTABLE:
        case IImsReasonInfo::CODE_SIP_USER_REJECTED:
        case IImsReasonInfo::CODE_SIP_GLOBAL_ERROR:
            return IDisconnectCause::SERVER_ERROR;

        case IImsReasonInfo::CODE_SIP_SERVICE_UNAVAILABLE:
        case IImsReasonInfo::CODE_SIP_NOT_FOUND:
        case IImsReasonInfo::CODE_SIP_SERVER_ERROR:
            return IDisconnectCause::SERVER_UNREACHABLE;

        case IImsReasonInfo::CODE_LOCAL_NETWORK_ROAMING:
        case IImsReasonInfo::CODE_LOCAL_NETWORK_IP_CHANGED:
        case IImsReasonInfo::CODE_LOCAL_IMS_SERVICE_DOWN:
        case IImsReasonInfo::CODE_LOCAL_SERVICE_UNAVAILABLE:
        case IImsReasonInfo::CODE_LOCAL_NOT_REGISTERED:
        case IImsReasonInfo::CODE_LOCAL_NETWORK_NO_LTE_COVERAGE:
        case IImsReasonInfo::CODE_LOCAL_NETWORK_NO_SERVICE:
        case IImsReasonInfo::CODE_LOCAL_CALL_VCC_ON_PROGRESSING:
            return IDisconnectCause::OUT_OF_SERVICE;

        case IImsReasonInfo::CODE_SIP_REQUEST_TIMEOUT:
        case IImsReasonInfo::CODE_TIMEOUT_1XX_WAITING:
        case IImsReasonInfo::CODE_TIMEOUT_NO_ANSWER:
        case IImsReasonInfo::CODE_TIMEOUT_NO_ANSWER_CALL_UPDATE:
            return IDisconnectCause::TIMED_OUT;

        case IImsReasonInfo::CODE_LOCAL_LOW_BATTERY:
        case IImsReasonInfo::CODE_LOCAL_POWER_OFF:
            return IDisconnectCause::POWER_OFF;

        default:
            break;
    }

    return cause;
}

void ImsPhoneCallTracker::TransferHandoverConnections(
    /* [in] */ IImsPhoneCall* _call)
{
    AutoPtr<ImsPhoneCall> call = (ImsPhoneCall*)_call;

    if (call->mConnections != NULL) {
        Int32 size;
        call->mConnections->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            call->mConnections->Get(i, (IInterface**)&obj);
            AutoPtr<IConnection> c = IConnection::Probe(obj);

            ((Connection*)c.Get())->mPreHandoverState = call->mState;
            ICallState state;
            c->GetStateBeforeHandover(&state);
            Log(String("Connection state before handover is ") + StringUtils::ToString(state));
        }
    }

    AutoPtr<ImsPhoneCall> hCall = (ImsPhoneCall*)mHandoverCall.Get();

    if (hCall->mConnections == NULL ) {
        hCall->mConnections = call->mConnections;
    }
    else { // Multi-call SRVCC
        hCall->mConnections->AddAll(ICollection::Probe(call->mConnections));
    }
    if (hCall->mConnections != NULL) {
        AutoPtr</*TODO IImsCall*/IInterface> imsCall;
        call->GetImsCall((/*TODO IImsCall*/IInterface**)&imsCall);
        if (imsCall != NULL) {
// TODO: Need IImsCall
            // imsCall->Close();
        }

        Int32 size;
        hCall->mConnections->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            hCall->mConnections->Get(i, (IInterface**)&obj);
            AutoPtr<IConnection> c = IConnection::Probe(obj);

            ((ImsPhoneConnection*)c.Get())->ChangeParent(mHandoverCall);
        }
    }

    ICallState state;
    call->GetState(&state);
// TODO: Need ICallState::isAlive
    // if (state->isAlive()) {
    //     Log (String("Call is alive and state is ") + StringUtils::ToString(call->mState));
    //     mHandoverCall->mState = call->mState;
    // }
    call->mConnections->Clear();
    call->mState = ICallState_IDLE;
}

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
