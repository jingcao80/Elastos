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
#include "elastos/droid/ims/CImsCallProfile.h"
#include "elastos/droid/ims/CImsStreamMediaProfile.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhone.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneCallTracker.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneCall.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneConnection.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Ims::CImsCallProfile;
using Elastos::Droid::Ims::CImsStreamMediaProfile;
using Elastos::Droid::Ims::IImsCallProfile;
using Elastos::Droid::Ims::IImsStreamMediaProfile;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

//=====================================================================
//                    ImsPhoneConnection::MyHandler
//=====================================================================
ImsPhoneConnection::MyHandler::MyHandler(
    /* [in] */ ILooper* l,
    /* [in] */ ImsPhoneConnection* host)
    : Handler(l)
    , mHost(host)
{
}

ECode ImsPhoneConnection::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_NEXT_POST_DIAL:
        case EVENT_DTMF_DONE:
        case EVENT_PAUSE_DONE:
            mHost->ProcessNextPostDialChar();
            break;
        case EVENT_WAKE_LOCK_TIMEOUT:
            mHost->ReleaseWakeLock();
            break;
    }
    return NOERROR;
}

//=====================================================================
//                          ImsPhoneConnection
//=====================================================================
CAR_INTERFACE_IMPL(ImsPhoneConnection, Connection, IImsPhoneConnection);

const String ImsPhoneConnection::LOGTAG("ImsPhoneConnection");
const Boolean ImsPhoneConnection::DBG = TRUE;
const Int32 ImsPhoneConnection::EVENT_DTMF_DONE;
const Int32 ImsPhoneConnection::EVENT_PAUSE_DONE;
const Int32 ImsPhoneConnection::EVENT_NEXT_POST_DIAL;
const Int32 ImsPhoneConnection::EVENT_WAKE_LOCK_TIMEOUT;
const Int32 ImsPhoneConnection::PAUSE_DELAY_MILLIS;
const Int32 ImsPhoneConnection::WAKE_LOCK_TIMEOUT_MILLIS;

ImsPhoneConnection::ImsPhoneConnection()
    : mDisconnected(FALSE)
    , mDisconnectTime(0)
    , mNextPostDialChar(0)
    , mCause(IDisconnectCause::NOT_DISCONNECTED)
    , mPostDialState(PostDialState_NOT_STARTED)
{
}

ImsPhoneConnection::~ImsPhoneConnection()
{
    ReleaseWakeLock();
}

ECode ImsPhoneConnection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ IImsPhoneCallTracker* ct,
    /* [in] */ IImsPhoneCall* parent)
{
    CreateWakeLock(context);
    AcquireWakeLock();

    mOwner = ct;
    AutoPtr<ILooper> looper;
    IHandler::Probe(mOwner)->GetLooper((ILooper**)&looper);
    mHandler = new MyHandler(looper, this);
    mImsCall = imsCall;

// TODO: Need ImsCall
    // if ((imsCall != NULL) && (imsCall.getCallProfile() != NULL)) {
    //     mAddress = imsCall.getCallProfile().getCallExtra(ImsCallProfile.EXTRA_OI);
    //     mCnapName = imsCall.getCallProfile().getCallExtra(ImsCallProfile.EXTRA_CNA);
    //     mNumberPresentation = ImsCallProfile.OIRToPresentation(
    //             imsCall.getCallProfile().getCallExtraInt(ImsCallProfile.EXTRA_OIR));
    //     mCnapNamePresentation = ImsCallProfile.OIRToPresentation(
    //             imsCall.getCallProfile().getCallExtraInt(ImsCallProfile.EXTRA_CNAP));

    //     ImsCallProfile imsCallProfile = imsCall.getCallProfile();
    //     if (imsCallProfile != NULL) {
    //         setVideoState(ImsCallProfile.getVideoStateFromImsCallProfile(imsCallProfile));

    //         ImsStreamMediaProfile mediaProfile = imsCallProfile.mMediaProfile;
    //         if (mediaProfile != NULL) {
    //             setAudioQuality(getAudioQualityFromMediaProfile(mediaProfile));
    //         }
    //     }

    //     // Determine if the current call have video capabilities.
    //     try {
    //         ImsCallProfile localCallProfile = imsCall.getLocalCallProfile();
    //         if (localCallProfile != NULL) {
    //             Boolean isLocalVideoCapable = localCallProfile.mCallType
    //                     == ImsCallProfile.CALL_TYPE_VT;

    //             setLocalVideoCapable(isLocalVideoCapable);
    //         }
    //         ImsCallProfile remoteCallProfile = imsCall.getRemoteCallProfile();
    //         if (remoteCallProfile != NULL) {
    //             Boolean isRemoteVideoCapable = remoteCallProfile.mCallType
    //                     == ImsCallProfile.CALL_TYPE_VT;

    //             setRemoteVideoCapable(isRemoteVideoCapable);
    //         }
    //     } catch (ImsException e) {
    //         // No session, so cannot get local capabilities.
    //     }
    // }
    // else {
    //     mNumberPresentation = IPhoneConstants::PRESENTATION_UNKNOWN;
    //     mCnapNamePresentation = IPhoneConstants::PRESENTATION_UNKNOWN;
    // }

    mIsIncoming = TRUE;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mCreateTime);
    mUusInfo = NULL;

    //mIndex = index;

    mParent = parent;
    ((ImsPhoneCall*)mParent.Get())->Attach(this, ICallState_INCOMING);
    return NOERROR;
}

ECode ImsPhoneConnection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& dialString,
    /* [in] */ IImsPhoneCallTracker* ct,
    /* [in] */ IImsPhoneCall* parent,
    /* [in] */ IBundle* extras)
{
    CreateWakeLock(context);
    AcquireWakeLock();
    Boolean isConferenceUri = FALSE;
    Boolean isSkipSchemaParsing = FALSE;
    if (extras != NULL) {
        extras->GetBoolean(ITelephonyProperties::EXTRA_DIAL_CONFERENCE_URI, FALSE, &isConferenceUri);
        extras->GetBoolean(ITelephonyProperties::EXTRA_SKIP_SCHEMA_PARSING, FALSE, &isSkipSchemaParsing);
    }

    mOwner = ct;
    AutoPtr<ILooper> looper;
    IHandler::Probe(mOwner)->GetLooper((ILooper**)&looper);
    mHandler = new MyHandler(looper, this);

    mDialString = dialString;

    if (isConferenceUri || isSkipSchemaParsing) {
        mAddress = dialString;
        mPostDialString = String("");
    }
    else {
        PhoneNumberUtils::ExtractNetworkPortionAlt(dialString, &mAddress);
        PhoneNumberUtils::ExtractPostDialPortion(dialString, &mPostDialString);
    }

    //mIndex = -1;

    mIsIncoming = FALSE;
    mCnapName = NULL;
    mCnapNamePresentation = IPhoneConstants::PRESENTATION_ALLOWED;
    mNumberPresentation = IPhoneConstants::PRESENTATION_ALLOWED;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mCreateTime);

    if (extras != NULL) {
        mCallExtras = extras;
    }

    mParent = parent;
    ((ImsPhoneCall*)parent)->AttachFake(this, ICallState_DIALING);
    return NOERROR;
}

ECode ImsPhoneConnection::Dispose()
{
    return NOERROR;
}

Boolean ImsPhoneConnection::EqualsHandlesNulls(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b)
{
    Boolean e;
    return (a == NULL) ? (b == NULL) : (IObject::Probe(a)->Equals(b, &e), e);
}

ECode ImsPhoneConnection::GetOrigDialString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDialString;
    return NOERROR;
}

ECode ImsPhoneConnection::GetCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ICall::Probe(mParent);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneConnection::GetDisconnectTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDisconnectTime;
    return NOERROR;
}

ECode ImsPhoneConnection::GetHoldingStartTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHoldingStartTime;
    return NOERROR;
}

ECode ImsPhoneConnection::GetHoldDurationMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    ICallState state;
    GetState(&state);
    if (state != ICallState_HOLDING) {
        // If not holding, return 0
        *result = 0;
    }
    else {
        *result = SystemClock::GetElapsedRealtime() - mHoldingStartTime;
    }
    return NOERROR;
}

ECode ImsPhoneConnection::GetDisconnectCause(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCause;
    return NOERROR;
}

ECode ImsPhoneConnection::SetDisconnectCause(
    /* [in] */ Int32 cause)
{
    mCause = cause;
    return NOERROR;
}

ECode ImsPhoneConnection::GetOwner(
    /* [out] */ IImsPhoneCallTracker** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneConnection::GetState(
    /* [out] */ ICallState* result)
{
    VALIDATE_NOT_NULL(result);
    if (mDisconnected) {
        *result = ICallState_DISCONNECTED;
    }
    else {
        Connection::GetState(result);
    }
    return NOERROR;
}

ECode ImsPhoneConnection::Hangup()
{
    if (!mDisconnected) {
        ((ImsPhoneCallTracker*)mOwner.Get())->Hangup(this);
    }
    else {
        // throw new CallStateException ("disconnected");
        return E_CALL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode ImsPhoneConnection::Separate()
{
    // trow new CallStateException ("not supported");
    return E_CALL_STATE_EXCEPTION;
}

ECode ImsPhoneConnection::GetPostDialState(
    /* [out] */ IConnectionPostDialState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPostDialState;
    return NOERROR;
}

ECode ImsPhoneConnection::ProceedAfterWaitChar()
{
    if (mPostDialState != PostDialState_WAIT) {
        Logger::W(LOGTAG,
            "ImsPhoneConnection.proceedAfterWaitChar(): Expected getPostDialState() to be WAIT but was %d",
            mPostDialState);
        return NOERROR;
    }

    SetPostDialState(PostDialState_STARTED);

    ProcessNextPostDialChar();
    return NOERROR;
}

ECode ImsPhoneConnection::ProceedAfterWildChar(
    /* [in] */ const String& str)
{
    if (mPostDialState != PostDialState_WILD) {
        Logger::W(LOGTAG,
            "ImsPhoneConnection.proceedAfterWaitChar(): Expected getPostDialState() to be WILD but was %d",
            mPostDialState);
        return NOERROR;
    }

    SetPostDialState(PostDialState_STARTED);

    // make a new postDialString, with the wild char replacement string
    // at the beginning, followed by the remaining postDialString.

    StringBuilder buf(str);
    buf.Append(mPostDialString.Substring(mNextPostDialChar));
    buf.ToString(&mPostDialString);
    mNextPostDialChar = 0;
    if (IPhone::DEBUG_PHONE) {
        Logger::D(LOGTAG, "proceedAfterWildChar: new postDialString is %s",
                mPostDialString.string());
    }

    ProcessNextPostDialChar();
    return NOERROR;
}

ECode ImsPhoneConnection::CancelPostDial()
{
    SetPostDialState(PostDialState_CANCELLED);
    return NOERROR;
}

ECode ImsPhoneConnection::OnHangupLocal()
{
    mCause = IDisconnectCause::LOCAL;
    return NOERROR;
}

ECode ImsPhoneConnection::OnDisconnect(
    /* [in] */ Int32 cause,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::D(LOGTAG, "onDisconnect: cause=%d", cause);
    if (mCause != IDisconnectCause::LOCAL) mCause = cause;
    return OnDisconnect(result);
}

ECode ImsPhoneConnection::OnDisconnect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean changed = FALSE;

    if (!mDisconnected) {
        //mIndex = -1;

        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&mDisconnectTime);
        mDuration = SystemClock::GetElapsedRealtime() - mConnectTimeReal;
        mDisconnected = TRUE;

        ((ImsPhone*)((ImsPhoneCallTracker*)mOwner.Get())->mPhone.Get())->NotifyDisconnect(this);

        if (mParent != NULL) {
            Boolean b;
            changed = ((ImsPhoneCall*)mParent.Get())->
                    ConnectionDisconnected(IImsPhoneConnection::Probe(this), &b);
        }
        else {
            Logger::D(LOGTAG, "onDisconnect: no parent");
        }
// TODO: Need IImsCall
        // if (mImsCall != NULL) mImsCall->Close();
        mImsCall = NULL;
    }
    ReleaseWakeLock();
    *result = changed;
    return NOERROR;
}

ECode ImsPhoneConnection::OnConnectedInOrOut()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mConnectTime);
    mConnectTimeReal = SystemClock::GetElapsedRealtime();
    mDuration = 0;

    if (IPhone::DEBUG_PHONE) {
        Logger::D(LOGTAG, "onConnectedInOrOut: connectTime=%d", mConnectTime);
    }

    if (!mIsIncoming) {
        // outgoing calls only
        ProcessNextPostDialChar();
    }
    ReleaseWakeLock();
    return NOERROR;
}

ECode ImsPhoneConnection::OnStartedHolding()
{
    mHoldingStartTime = SystemClock::GetElapsedRealtime();
    return NOERROR;
}

ECode ImsPhoneConnection::GetRemainingPostDialString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (mPostDialState == PostDialState_CANCELLED
        || mPostDialState == PostDialState_COMPLETE
        || mPostDialString == NULL
        || mPostDialString.GetLength() <= mNextPostDialChar
    ) {
        *result = String("");
    }

    *result = mPostDialString.Substring(mNextPostDialChar);
    return NOERROR;
}

ECode ImsPhoneConnection::ReleaseWakeLock()
{
    AutoLock lock(mPartialWakeLock);
    Boolean b;
    if (mPartialWakeLock->IsHeld(&b), b) {
        Logger::D(LOGTAG, "releaseWakeLock");
        mPartialWakeLock->ReleaseLock();
    }

    return NOERROR;
}

ECode ImsPhoneConnection::GetNumberPresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumberPresentation;
    return NOERROR;
}

ECode ImsPhoneConnection::GetUUSInfo(
    /* [out] */ IUUSInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUusInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneConnection::GetOrigConnection(
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode ImsPhoneConnection::IsMultiparty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
// TODO: Need IImsCall
    // *result = mImsCall != NULL && mImsCall->IsMultiparty();
    *result = FALSE;
    return NOERROR;
}

ECode ImsPhoneConnection::GetImsCall(
    /* [out] */ /*TODO IImsCall*/IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mImsCall;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneConnection::SetImsCall(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    mImsCall = imsCall;
    return NOERROR;
}

ECode ImsPhoneConnection::ChangeParent(
    /* [in] */ IImsPhoneCall* parent)
{
    mParent = parent;
    return NOERROR;
}

ECode ImsPhoneConnection::Update(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */  ICallState state,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean changed = FALSE;

    if (state == ICallState_ACTIVE) {
        ICallState state;
        ICall::Probe(mParent)->GetState(&state);
// TODO: Need ICallState::IsRinging, IsDialing
        // if (state->IsRinging()
        //         || state->IsDialing()) {
        //     OnConnectedInOrOut();
        // }

// TODO: Need ICallState::IsRinging
        // if (state->IsRinging()
        //         || mParent == mOwner->mBackgroundCall) {
        //     //mForegroundCall should be IDLE
        //     //when accepting WAITING call
        //     //before accept WAITING call,
        //     //the ACTIVE call should be held ahead
        //     mParent->Detach(this);
        //     mParent = mOwner->mForegroundCall;
        //     mParent->Attach(this);
        // }
    }
    else if (state == ICallState_HOLDING) {
        OnStartedHolding();
    }

    ((ImsPhoneCall*)mParent.Get())->Update(this, imsCall, state, &changed);

    if (imsCall != NULL) {
        // Check for a change in the video capabilities for the call and update the
        // {@link ImsPhoneConnection} with this information.
        // try {
        // Get the current local VT capabilities (i.e. even if currentCallType above is
        // audio-only, the local capability could support bi-directional video).
        AutoPtr<IImsCallProfile> localCallProfile;
// TODO: Need IImsCall
        // imsCall->GetLocalCallProfile((IImsCallProfile**)&localCallProfile);
        // Logger::D(LOGTAG, " update localCallProfile=" + localCallProfile
        //         + "isLocalVideoCapable()= " + isLocalVideoCapable());
        if (localCallProfile != NULL) {
            Boolean newLocalVideoCapable = ((CImsCallProfile*)localCallProfile.Get())->mCallType
                    == IImsCallProfile::CALL_TYPE_VT;

            Boolean b;
            if ((IsLocalVideoCapable(&b), b) != newLocalVideoCapable) {
                SetLocalVideoCapable(newLocalVideoCapable);
                changed = TRUE;
            }
        }

        AutoPtr<IImsCallProfile> remoteCallProfile;
// TODO: Need IImsCall
        // imsCall->GetRemoteCallProfile((IImsCallProfile**)&remoteCallProfile);
        // Logger::D(LOGTAG, " update remoteCallProfile=" + remoteCallProfile
        //         + "isRemoteVideoCapable()= " + isRemoteVideoCapable());
        if (remoteCallProfile != NULL) {
            Boolean newRemoteVideoCapable = ((CImsCallProfile*)remoteCallProfile.Get())->mCallType
                    == IImsCallProfile::CALL_TYPE_VT;

            Boolean b;
            if ((IsRemoteVideoCapable(&b), b) != newRemoteVideoCapable) {
                SetRemoteVideoCapable(newRemoteVideoCapable);
                changed = TRUE;
            }
        }

        // Check if call substate has changed. If so notify listeners of call state changed.
        Int32 callSubstate;
        GetCallSubstate(&callSubstate);
        Int32 newCallSubstate;
// TODO: Need IImsCall
        // imsCall->GetCallSubstate(&newCallSubstate);

        if (callSubstate != newCallSubstate) {
            SetCallSubstate(newCallSubstate);
            changed = TRUE;
        }

        // } catch (ImsException e) {
        //     // No session in place -- no change
        // }

        // Check for a change in the call type / video state, or audio quality of the
        // {@link ImsCall} and update the {@link ImsPhoneConnection} with this information.
        AutoPtr<IImsCallProfile> callProfile;
// TODO: Need IImsCall
        // imsCall->GetCallProfile((IImsCallProfile**)&callProfile);
        if (callProfile != NULL) {
            Int32 oldVideoState;
            GetVideoState(&oldVideoState);
            Int32 newVideoState;
// TODO: Need ImsCallProfile::GetVideoStateFromImsCallProfile
            // CImsCallProfile::GetVideoStateFromImsCallProfile(callProfile, &newVideoState);

            if (oldVideoState != newVideoState) {
                SetVideoState(newVideoState);
                changed = TRUE;
            }

            AutoPtr<IImsStreamMediaProfile> mediaProfile =
                    ((CImsCallProfile*)callProfile.Get())->mMediaProfile;
            if (mediaProfile != NULL) {
                Int32 oldAudioQuality;
                GetAudioQuality(&oldAudioQuality);
                Int32 newAudioQuality = GetAudioQualityFromMediaProfile(mediaProfile);

                if (oldAudioQuality != newAudioQuality) {
                    SetAudioQuality(newAudioQuality);
                    changed = TRUE;
                }
            }
        }
    }
    *result = changed;
    return NOERROR;
}

ECode ImsPhoneConnection::GetPreciseDisconnectCause(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

ECode ImsPhoneConnection::GetCallExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCallExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneConnection::OnDisconnectConferenceParticipant(
    /* [in] */ IUri* endpoint)
{
    AutoPtr</*TODO IImsCall*/IInterface> imsCall;
    GetImsCall((/*TODO IImsCall*/IInterface**)&imsCall);
    if (imsCall == NULL) {
        return NOERROR;
    }
    // try {
// TODO: Need IImsCall
    // imsCall->RemoveParticipants(new String[]{endpoint.ToString()});
    // } catch (ImsException e) {
    //     // No session in place -- no change
    //     Logger::E(LOGTAG, "onDisconnectConferenceParticipant: no session in place. "+
    //             "Failed to disconnect endpoint = " + endpoint);
    // }
    return NOERROR;
}

ECode ImsPhoneConnection::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder sb;
    sb.Append("[ImsPhoneConnection objId: ");

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int32 val;
    system->IdentityHashCode(TO_IINTERFACE(this), &val);
    sb.Append(val);
    sb.Append(" address:");
    String str;
    GetAddress(&str);
// TODO: Need Log.pii
    // sb.Append(Log.pii(str));
    sb.Append(" ImsCall:");
    if (mImsCall == NULL) {
        sb.Append("NULL");
    } else {
        IObject::Probe(mImsCall)->ToString(&str);
        sb.Append(str);
    }
    sb.Append("]");
    return sb.ToString(result);
}

Int32 ImsPhoneConnection::GetAudioQualityFromMediaProfile(
    /* [in] */ IImsStreamMediaProfile* _mediaProfile)
{
    AutoPtr<CImsStreamMediaProfile> mediaProfile = (CImsStreamMediaProfile*)_mediaProfile;
    Int32 audioQuality;

    // The Adaptive Multi-Rate Wideband codec is used for high definition audio calls.
    if (mediaProfile->mAudioQuality == IImsStreamMediaProfile::AUDIO_QUALITY_AMR_WB) {
        audioQuality = AUDIO_QUALITY_HIGH_DEFINITION;
    }
    else {
        audioQuality = AUDIO_QUALITY_STANDARD;
    }

    return audioQuality;
}

Boolean ImsPhoneConnection::ProcessPostDialChar(
    /* [in] */ Char16 c)
{
    Boolean b;
    if (PhoneNumberUtils::Is12Key(c, &b), b) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(EVENT_DTMF_DONE, (IMessage**)&msg);
        ((ImsPhoneCallTracker*)mOwner.Get())->mCi->SendDtmf(c, msg);
    }
    else if (c == IPhoneNumberUtils::PAUSE) {
        // From TS 22.101:
        // It continues...
        // Upon the called party answering the UE shall send the DTMF digits
        // automatically to the network after a delay of 3 seconds( 20 ).
        // The digits shall be sent according to the procedures and timing
        // specified in 3GPP TS 24.008 [13]. The first occurrence of the
        // "DTMF Control Digits Separator" shall be used by the ME to
        // distinguish between the addressing digits (i.e. the phone number)
        // and the DTMF digits. Upon subsequent occurrences of the
        // separator,
        // the UE shall pause again for 3 seconds ( 20 ) before sending
        // any further DTMF digits.
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(EVENT_PAUSE_DONE, (IMessage**)&msg);
        mHandler->SendMessageDelayed(msg, PAUSE_DELAY_MILLIS, &b);
    }
    else if (c == IPhoneNumberUtils::WAIT) {
        SetPostDialState(PostDialState_WAIT);
    }
    else if (c == IPhoneNumberUtils::WILD) {
        SetPostDialState(PostDialState_WILD);
    }
    else {
        return FALSE;
    }

    return TRUE;
}

void ImsPhoneConnection::ProcessNextPostDialChar()
{
    Char32 c = 0;
    AutoPtr<IRegistrant> postDialHandler;

    if (mPostDialState == PostDialState_CANCELLED) {
        //Rlog.d(LOGTAG, "##### processNextPostDialChar: postDialState == CANCELLED, bail");
        return;
    }

    if (mPostDialString == NULL ||
            mPostDialString.GetLength() <= mNextPostDialChar) {
        SetPostDialState(PostDialState_COMPLETE);

        // notifyMessage.arg1 is 0 on complete
        c = 0;
    }
    else {
        Boolean isValid;

        SetPostDialState(PostDialState_STARTED);

        c = mPostDialString.GetChar(mNextPostDialChar++);

        isValid = ProcessPostDialChar(c);

        if (!isValid) {
            // Will call processNextPostDialChar
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(EVENT_NEXT_POST_DIAL, (IMessage**)&msg);
            msg->SendToTarget();
            // Don't notify application
            Logger::E(LOGTAG, "processNextPostDialChar: c=%d isn't valid!", c);
            return;
        }
    }

    postDialHandler = ((ImsPhone*)((ImsPhoneCallTracker*)mOwner.Get())->mPhone.Get())->mPostDialHandler;

    AutoPtr<IMessage> notifyMessage;
    postDialHandler->MessageForRegistrant((IMessage**)&notifyMessage);
    if (postDialHandler != NULL
            && notifyMessage != NULL) {
        // The AsyncResult.result is the Connection object
        IConnectionPostDialState state = mPostDialState;
        AutoPtr<AsyncResult> ar = AsyncResult::ForMessage(notifyMessage);
        ar->mResult = TO_IINTERFACE(this);
        ar->mUserObj = CoreUtils::Convert(state);

        // arg1 is the character that was/is being processed
        notifyMessage->SetArg1(c);

        //Rlog.v(LOGTAG, "##### processNextPostDialChar: send msg to postDialHandler, arg1=" + c);
        notifyMessage->SendToTarget();
    }
}

void ImsPhoneConnection::SetPostDialState(
    /* [in] */ IConnectionPostDialState s)
{
    if (mPostDialState != PostDialState_STARTED
            && s == PostDialState_STARTED) {
        AcquireWakeLock();
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(EVENT_WAKE_LOCK_TIMEOUT, (IMessage**)&msg);
        Boolean b;
        mHandler->SendMessageDelayed(msg, WAKE_LOCK_TIMEOUT_MILLIS, &b);
    }
    else if (mPostDialState == PostDialState_STARTED
            && s != PostDialState_STARTED) {
        mHandler->RemoveMessages(EVENT_WAKE_LOCK_TIMEOUT);
        ReleaseWakeLock();
    }
    mPostDialState = s;
}

void ImsPhoneConnection::CreateWakeLock(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, LOGTAG, (IPowerManagerWakeLock**)&mPartialWakeLock);
}

void ImsPhoneConnection::AcquireWakeLock()
{
    Logger::D(LOGTAG, "acquireWakeLock");
    mPartialWakeLock->AcquireLock();
}

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
