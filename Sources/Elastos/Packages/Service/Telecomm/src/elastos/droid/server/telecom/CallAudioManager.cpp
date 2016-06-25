
#include "elastos/droid/server/telecom/CallAudioManager.h"
#include "R.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/ConnectionServiceWrapper.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/Ringer.h"
#include "elastos/droid/server/telecom/ThreadUtil.h"
#include "elastos/droid/server/telecom/WiredHeadsetManager.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/droid/R.h>
#include <elastos/utility/Objects.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Telecomm::Telecom::CAudioState;
using Elastos::Droid::Telecomm::Telecom::CAudioStateHelper;
using Elastos::Droid::Telecomm::Telecom::IAudioStateHelper;
using Elastos::Droid::Telecomm::Telecom::ICallState;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;
using Elastos::Utility::IList;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CAR_INTERFACE_IMPL(CallAudioManager, CallsManagerListenerBase, IWiredHeadsetManagerListener)

const Int32 CallAudioManager::STREAM_NONE = -1;

CallAudioManager::CallAudioManager()
    : mMostRecentlyUsedMode(IAudioManager::MODE_IN_CALL)
    , mSpeedUpAudioForMtCall(FALSE)
{}

ECode CallAudioManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ StatusBarNotifier* statusBarNotifier,
    /* [in] */ WiredHeadsetManager* wiredHeadsetManager)
{
    mStatusBarNotifier = statusBarNotifier;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    mAudioManager = IAudioManager::Probe(obj);
    mBluetoothManager = new BluetoothManager();
    mBluetoothManager->constructor(context, this);
    mWiredHeadsetManager = wiredHeadsetManager;
    mWiredHeadsetManager->AddListener(this);
    AutoPtr<IAudioState> audioState;
    GetInitialAudioState(NULL, (IAudioState**)&audioState);
    SaveAudioState(audioState);
    mAudioFocusStreamType = STREAM_NONE;
    mContext = context;
    return NOERROR;
}

ECode CallAudioManager::GetAudioState(
    /* [out] */ IAudioState** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAudioState;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallAudioManager::OnCallAdded(
    /* [in] */ ICall* call)
{
    OnCallUpdated(call);
    Boolean hasFocus;
    HasFocus(&hasFocus);
    AutoPtr<ICall> foregroundCall;
    GetForegroundCall((ICall**)&foregroundCall);
    if (hasFocus && foregroundCall.Get() == call) {
        Boolean isIncoming;
        ((Call*) call)->IsIncoming(&isIncoming);
        if (!isIncoming) {
            // Unmute new outgoing call.
            Int32 supportedRouteMask;
            mAudioState->GetSupportedRouteMask(&supportedRouteMask);
            Int32 route;
            mAudioState->GetRoute(&route);
            SetSystemAudioState(FALSE, route, supportedRouteMask);
        }
    }
    return NOERROR;
}

ECode CallAudioManager::OnCallRemoved(
    /* [in] */ ICall* call)
{
    // If we didn't already have focus, there's nothing to do.
    Boolean hasFocus;
    HasFocus(&hasFocus);
    if (hasFocus) {
        AutoPtr<IList> calls;
        CallsManager::GetInstance()->GetCalls((IList**)&calls);
        Boolean isEmpty;
        calls->IsEmpty(&isEmpty);
        if (isEmpty) {
            Log::V("CallAudioManager", "all calls removed, reseting system audio to default state");
            SetInitialAudioState(NULL, FALSE /* force */);
            mWasSpeakerOn = FALSE;
        }
        UpdateAudioStreamAndMode();
    }
    return NOERROR;
}

ECode CallAudioManager::OnCallStateChanged(
    /* [in] */ ICall* call,
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
    return OnCallUpdated(call);
}

ECode CallAudioManager::OnIncomingCallAnswered(
    /* [in] */ ICall* call)
{
    Int32 route;
    mAudioState->GetRoute(&route);
    // BT stack will connect audio upon receiving active call state.
    // We unmute the audio for the new incoming call.
    Int32 supportedRouteMask;
    mAudioState->GetSupportedRouteMask(&supportedRouteMask);
    SetSystemAudioState(FALSE /* isMute */, route, supportedRouteMask);
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Boolean isConfigSpeedUpAudioOnMtCalls;
    resources->GetBoolean(
            Elastos::Droid::Server::Telecom::R::bool_::config_speed_up_audio_on_mt_calls, &isConfigSpeedUpAudioOnMtCalls);
    if (mContext == NULL) {
        Log::D("CallAudioManager", "Speedup Audio Path enhancement: Context is null");
    } else if (isConfigSpeedUpAudioOnMtCalls) {
        Log::D("CallAudioManager", "Speedup Audio Path enhancement");
        mSpeedUpAudioForMtCall = TRUE;
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        ((Call*) call)->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
        phoneAccountHandle->GetId(&mSubId);
        if (mIsRinging) {
            SetIsRinging(FALSE);
        } else {
            UpdateAudioStreamAndMode();
        }
    }
    return NOERROR;
}

ECode CallAudioManager::OnForegroundCallChanged(
    /* [in] */ ICall* oldForegroundCall,
    /* [in] */ ICall* newForegroundCall)
{
    OnCallUpdated(newForegroundCall);
    // Ensure that the foreground call knows about the latest audio state.
    UpdateAudioForForegroundCall();
    return NOERROR;
}

ECode CallAudioManager::OnIsVoipAudioModeChanged(
    /* [in] */ ICall* call)
{
    return UpdateAudioStreamAndMode();
}

ECode CallAudioManager::OnWiredHeadsetPluggedInChanged(
    /* [in] */ Boolean oldIsPluggedIn,
    /* [in] */ Boolean newIsPluggedIn)
{
    // This can happen even when there are no calls and we don't have focus.
    Boolean hasFocus;
    HasFocus(&hasFocus);
    if (!hasFocus) {
        return NOERROR;
    }
    Boolean isBluetoothAudioOn;
    IsBluetoothAudioOn(&isBluetoothAudioOn);
    Log::D("CallAudioManager","isBluetoothAudioOn(): %d", isBluetoothAudioOn);
    if (!isBluetoothAudioOn) {
        Int32 newRoute = IAudioState::ROUTE_EARPIECE;
        if (newIsPluggedIn) {
            newRoute = IAudioState::ROUTE_WIRED_HEADSET;
        } else if (mWasSpeakerOn) {
            AutoPtr<ICall> call;
            GetForegroundCall((ICall**)&call);
            Boolean isAlive;
            ((Call*) call.Get())->IsAlive(&isAlive);
            if (call != NULL && isAlive) {
                // Restore the speaker state.
                newRoute = IAudioState::ROUTE_SPEAKER;
            }
        }
        Int32 supportedRoutes;
        CalculateSupportedRoutes(&supportedRoutes);
        Boolean isMuted;
        mAudioState->GetIsMuted(&isMuted);
        SetSystemAudioState(isMuted, newRoute, supportedRoutes);
    }
    return NOERROR;
}

ECode CallAudioManager::ToggleMute()
{
    Boolean isMuted;
    mAudioState->GetIsMuted(&isMuted);
    return Mute(!isMuted);
}

ECode CallAudioManager::Mute(
    /* [in] */ Boolean shouldMute)
{
    Boolean hasFocus;
    HasFocus(&hasFocus);
    if (!hasFocus) {
        return NOERROR;
    }
    Log::V("CallAudioManager", "mute, shouldMute: %d", shouldMute);
    // Don't mute if there are any emergency calls.
    Boolean hasEmergencyCall;
    CallsManager::GetInstance()->HasEmergencyCall(&hasEmergencyCall);
    if (hasEmergencyCall) {
        shouldMute = FALSE;
        Log::V("CallAudioManager", "ignoring mute for emergency call");
    }
    Boolean isMuted;
    mAudioState->GetIsMuted(&isMuted);
    if (isMuted != shouldMute) {
        Int32 supportedRouteMask;
        mAudioState->GetSupportedRouteMask(&supportedRouteMask);
        Int32 route;
        mAudioState->GetRoute(&route);
        SetSystemAudioState(shouldMute, route, supportedRouteMask);
    }
    return NOERROR;
}

ECode CallAudioManager::SetAudioRoute(
    /* [in] */ Int32 route)
{
    // This can happen even when there are no calls and we don't have focus.
    Boolean hasFocus;
    HasFocus(&hasFocus);
    if (!hasFocus) {
        return NOERROR;
    }
    AutoPtr<IAudioStateHelper> helper;
    CAudioStateHelper::AcquireSingleton((IAudioStateHelper**)&helper);
    String s;
    helper->AudioRouteToString(route, &s);
    Log::V("CallAudioManager", "setAudioRoute, route: %s", s.string());
    // Change ROUTE_WIRED_OR_EARPIECE to a single entry.
    Int32 newRoute;
    Int32 supportedRouteMask;
    mAudioState->GetSupportedRouteMask(&supportedRouteMask);
    SelectWiredOrEarpiece(route, supportedRouteMask, &newRoute);
    // If route is unsupported, do nothing.
    if ((supportedRouteMask | newRoute) == 0) {
        Log::Wtf("CallAudioManager", "Asking to set to a route that is unsupported: %d", newRoute);
        return NOERROR;
    }
    Int32 mAudioStateRoute;
    mAudioState->GetRoute(&mAudioStateRoute);
    if (mAudioStateRoute != newRoute) {
        // Remember the new speaker state so it can be restored when the user plugs and unplugs
        // a headset.
        mWasSpeakerOn = newRoute == IAudioState::ROUTE_SPEAKER;
        Int32 supportedRouteMask;
        mAudioState->GetSupportedRouteMask(&supportedRouteMask);
        Boolean isMuted;
        mAudioState->GetIsMuted(&isMuted);
        SetSystemAudioState(isMuted, newRoute, supportedRouteMask);
    }
    return NOERROR;
}

ECode CallAudioManager::SetIsRinging(
    /* [in] */ Boolean isRinging)
{
    if (mIsRinging != isRinging) {
        Log::V("CallAudioManager", "setIsRinging %d -> %d", mIsRinging, isRinging);
        mIsRinging = isRinging;
        UpdateAudioStreamAndMode();
    }
    return NOERROR;
}

ECode CallAudioManager::SetIsTonePlaying(
    /* [in] */ Boolean isPlayingNew)
{
    ThreadUtil::CheckOnMainThread();
    if (mIsTonePlaying != isPlayingNew) {
        Log::V("CallAudioManager", "mIsTonePlaying %d -> %d.", mIsTonePlaying, isPlayingNew);
        mIsTonePlaying = isPlayingNew;
        UpdateAudioStreamAndMode();
    }
    return NOERROR;
}

ECode CallAudioManager::OnBluetoothStateChange(
    /* [in] */ BluetoothManager* bluetoothManager)
{
    // This can happen even when there are no calls and we don't have focus.
    Boolean hasFocus;
    HasFocus(&hasFocus);
    if (!hasFocus) {
        return NOERROR;
    }
    Int32 supportedRoutes;
    CalculateSupportedRoutes(&supportedRoutes);
    Int32 newRoute;
    mAudioState->GetRoute(&newRoute);
    Boolean isBluetoothAudioConnectedOrPending;
    bluetoothManager->IsBluetoothAudioConnectedOrPending(&isBluetoothAudioConnectedOrPending);
    if (isBluetoothAudioConnectedOrPending) {
        newRoute = IAudioState::ROUTE_BLUETOOTH;
    }
    else if (newRoute == IAudioState::ROUTE_BLUETOOTH) {
        SelectWiredOrEarpiece(IAudioState::ROUTE_WIRED_OR_EARPIECE, supportedRoutes, &newRoute);
        // Do not switch to speaker when bluetooth disconnects.
        mWasSpeakerOn = FALSE;
    }
    Boolean isMuted;
    mAudioState->GetIsMuted(&isMuted);
    SetSystemAudioState(isMuted, newRoute, supportedRoutes);
    return NOERROR;
}

ECode CallAudioManager::IsBluetoothAudioOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isBluetoothAudioConnected;
    mBluetoothManager->IsBluetoothAudioConnected(&isBluetoothAudioConnected);
    *result = isBluetoothAudioConnected;
    return NOERROR;
}

ECode CallAudioManager::IsBluetoothDeviceAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isBluetoothAvailable;
    mBluetoothManager->IsBluetoothAvailable(&isBluetoothAvailable);
    *result = isBluetoothAvailable;
    return NOERROR;
}

ECode CallAudioManager::SaveAudioState(
    /* [in] */ IAudioState* audioState)
{
    mAudioState = audioState;
    Boolean isMuted;
    mAudioState->GetIsMuted(&isMuted);
    mStatusBarNotifier->NotifyMute(isMuted);
    Int32 route;
    mAudioState->GetRoute(&route);
    mStatusBarNotifier->NotifySpeakerphone(route == IAudioState::ROUTE_SPEAKER);
    return NOERROR;
}

ECode CallAudioManager::OnCallUpdated(
    /* [in] */ ICall* call)
{
    Boolean wasNotVoiceCall = mAudioFocusStreamType != IAudioManager::STREAM_VOICE_CALL;
    UpdateAudioStreamAndMode();
    AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
    ((Call*) call)->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
    Int32 state;
    ((Call*) call)->GetState(&state);
    String id;
    phoneAccountHandle->GetId(&id);
    if ((call != NULL) && (state == ICallState::ACTIVE) &&
            (phoneAccountHandle != NULL) &&
            id.Equals(mSubId) && mSpeedUpAudioForMtCall) {
        Log::D("CallAudioManager","Reset mSpeedUpAudioForMtCall");
        mSpeedUpAudioForMtCall = FALSE;
    }
    // If we transition from not voice call to voice call, we need to set an initial state.
    if (wasNotVoiceCall && mAudioFocusStreamType == IAudioManager::STREAM_VOICE_CALL) {
        SetInitialAudioState(call, TRUE /* force */);
    }
    return NOERROR;
}

ECode CallAudioManager::SetSystemAudioState(
    /* [in] */ Boolean isMuted,
    /* [in] */ Int32 route,
    /* [in] */ Int32 supportedRouteMask)
{
    return SetSystemAudioState(FALSE /* force */, isMuted, route, supportedRouteMask);
}

ECode CallAudioManager::SetSystemAudioState(
    /* [in] */ Boolean force,
    /* [in] */ Boolean isMuted,
    /* [in] */ Int32 route,
    /* [in] */ Int32 supportedRouteMask)
{
    Boolean hasFocus;
    HasFocus(&hasFocus);
    if (!hasFocus) {
        return NOERROR;
    }
    AutoPtr<IAudioState> oldAudioState = mAudioState;
    AutoPtr<IAudioState> newAudioState;
    CAudioState::New(isMuted, route, supportedRouteMask, (IAudioState**)&newAudioState);
    SaveAudioState(newAudioState);
    if (!force && Objects::Equals(oldAudioState, mAudioState)) {
        return NOERROR;
    }
    Log::I("CallAudioManager", "changing audio state from %s to %s", TO_CSTR(oldAudioState), TO_CSTR(mAudioState));
    // Mute.
    Boolean isMicrophoneMute;
    mAudioManager->IsMicrophoneMute(&isMicrophoneMute);
    Boolean mAudioStateIsMuted;
    mAudioState->GetIsMuted(&mAudioStateIsMuted);
    if (mAudioStateIsMuted != isMicrophoneMute) {
        Log::I("CallAudioManager", "changing microphone mute state to: %d", isMuted);
        mAudioManager->SetMicrophoneMute(isMuted);
    }
    // Audio route.
    Int32 mAudioStateRoute;
    mAudioState->GetRoute(&mAudioStateRoute);
    if (mAudioStateRoute == IAudioState::ROUTE_BLUETOOTH) {
        TurnOnSpeaker(FALSE);
        TurnOnBluetooth(TRUE);
    } else if (mAudioStateRoute == IAudioState::ROUTE_SPEAKER) {
        TurnOnBluetooth(FALSE);
        TurnOnSpeaker(TRUE);
    } else if (mAudioStateRoute == IAudioState::ROUTE_EARPIECE ||
            mAudioStateRoute == IAudioState::ROUTE_WIRED_HEADSET) {
        TurnOnBluetooth(FALSE);
        TurnOnSpeaker(FALSE);
    }
    Boolean isEquals;
    IObject::Probe(oldAudioState)->Equals(mAudioState, &isEquals);
    if (!isEquals) {
        CallsManager::GetInstance()->OnAudioStateChanged(oldAudioState, mAudioState);
        UpdateAudioForForegroundCall();
    }
    return NOERROR;
}

ECode CallAudioManager::TurnOnSpeaker(
    /* [in] */ Boolean on)
{
    // Wired headset and earpiece work the same way
    Boolean isSpeakerphoneOn;
    mAudioManager->IsSpeakerphoneOn(&isSpeakerphoneOn);
    if (isSpeakerphoneOn != on) {
        Log::I("CallAudioManager", "turning speaker phone %d", on);
        mAudioManager->SetSpeakerphoneOn(on);
    }
    return NOERROR;
}

ECode CallAudioManager::TurnOnBluetooth(
    /* [in] */ Boolean on)
{
    Boolean isBluetoothAvailable;
    mBluetoothManager->IsBluetoothAvailable(&isBluetoothAvailable);
    if (isBluetoothAvailable) {
        Boolean isBluetoothAudioConnectedOrPending;
        mBluetoothManager->IsBluetoothAudioConnectedOrPending(&isBluetoothAudioConnectedOrPending);
        Boolean isAlreadyOn = isBluetoothAudioConnectedOrPending;
        if (on != isAlreadyOn) {
            Log::I("CallAudioManager", "connecting bluetooth %d", on);
            if (on) {
                mBluetoothManager->ConnectBluetoothAudio();
            } else {
                mBluetoothManager->DisconnectBluetoothAudio();
            }
        }
    }
    return NOERROR;
}

ECode CallAudioManager::UpdateAudioStreamAndMode()
{
    Log::I("CallAudioManager", "updateAudioStreamAndMode, mIsRinging: %d, mIsTonePlaying: %d", mIsRinging,
            mIsTonePlaying);
    Log::V("CallAudioManager", "updateAudioStreamAndMode, mSpeedUpAudioForMtCall: %d", mSpeedUpAudioForMtCall);
    if (mIsRinging && !mSpeedUpAudioForMtCall) {
        RequestAudioFocusAndSetMode(IAudioManager::STREAM_RING, IAudioManager::MODE_RINGTONE);
    } else {
        AutoPtr<ICall> call;
        GetForegroundCall((ICall**)&call);
        Boolean hasRingingForegroundCall;
        HasRingingForegroundCall(&hasRingingForegroundCall);
        if (call != NULL) {
            Boolean isVoipAudioMode;
            call->GetIsVoipAudioMode(&isVoipAudioMode);
            Int32 mode = isVoipAudioMode ?
                    IAudioManager::MODE_IN_COMMUNICATION : IAudioManager::MODE_IN_CALL;
            RequestAudioFocusAndSetMode(IAudioManager::STREAM_VOICE_CALL, mode);
        } else if (mIsTonePlaying) {
            // There is no call, however, we are still playing a tone, so keep focus.
            // Since there is no call from which to determine the mode, use the most
            // recently used mode instead.
            RequestAudioFocusAndSetMode(
                    IAudioManager::STREAM_VOICE_CALL, mMostRecentlyUsedMode);
        } else if (!hasRingingForegroundCall) {
            AbandonAudioFocus();
        } else {
            // mIsRinging is false, but there is a foreground ringing call present. Don't
            // abandon audio focus immediately to prevent audio focus from getting lost between
            // the time it takes for the foreground call to transition from RINGING to ACTIVE/
            // DISCONNECTED. When the call eventually transitions to the next state, audio
            // focus will be correctly abandoned by the if clause above.
        }
    }
    return NOERROR;
}

ECode CallAudioManager::RequestAudioFocusAndSetMode(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 mode)
{
    Log::I("CallAudioManager", "requestAudioFocusAndSetMode, stream: %d -> %d", mAudioFocusStreamType, stream);
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    preconditionsHelper->CheckState(stream != STREAM_NONE);
    // Even if we already have focus, if the stream is different we update audio manager to give
    // it a hint about the purpose of our focus.
    if (mAudioFocusStreamType != stream) {
        Log::V("CallAudioManager", "requesting audio focus for stream: %d", stream);
        mAudioManager->RequestAudioFocusForCall(stream,
                IAudioManager::AUDIOFOCUS_GAIN_TRANSIENT);
    }
    mAudioFocusStreamType = stream;
    SetMode(mode);
    return NOERROR;
}

ECode CallAudioManager::AbandonAudioFocus()
{
    Boolean hasFocus;
    HasFocus(&hasFocus);
    if (hasFocus) {
        SetMode(IAudioManager::MODE_NORMAL);
        Log::V("CallAudioManager", "abandoning audio focus");
        mAudioManager->AbandonAudioFocusForCall();
        mAudioFocusStreamType = STREAM_NONE;
        mSpeedUpAudioForMtCall = FALSE;
    }
    return NOERROR;
}

ECode CallAudioManager::SetMode(
    /* [in] */ Int32 newMode)
{
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    Boolean hasFocus;
    HasFocus(&hasFocus);
    preconditionsHelper->CheckState(hasFocus);
    Int32 oldMode;
    mAudioManager->GetMode(&oldMode);
    Log::V("CallAudioManager", "Request to change audio mode from %d to %d", oldMode, newMode);
    if (oldMode != newMode) {
        if (oldMode == IAudioManager::MODE_IN_CALL && newMode == IAudioManager::MODE_RINGTONE) {
            Log::I("CallAudioManager", "Transition from IN_CALL -> RINGTONE. Resetting to NORMAL first.");
            mAudioManager->SetMode(IAudioManager::MODE_NORMAL);
        }
        mAudioManager->SetMode(newMode);
        Log::D("CallAudioManager", "SetMode Done");
        mMostRecentlyUsedMode = newMode;
    }
    return NOERROR;
}

ECode CallAudioManager::SelectWiredOrEarpiece(
    /* [in] */ Int32 route,
    /* [in] */ Int32 supportedRouteMask,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    // Since they are mutually exclusive and one is ALWAYS valid, we allow a special input of
    // ROUTE_WIRED_OR_EARPIECE so that callers dont have to make a call to check which is
    // supported before calling setAudioRoute.
    if (route == IAudioState::ROUTE_WIRED_OR_EARPIECE) {
        route = IAudioState::ROUTE_WIRED_OR_EARPIECE & supportedRouteMask;
        if (route == 0) {
            Log::Wtf("CallAudioManager", "One of wired headset or earpiece should always be valid.");
            // assume earpiece in this case.
            route = IAudioState::ROUTE_EARPIECE;
        }
    }
    *result = route;
    return NOERROR;
}

ECode CallAudioManager::CalculateSupportedRoutes(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 routeMask = IAudioState::ROUTE_SPEAKER;
    Boolean isPluggedIn;
    mWiredHeadsetManager->IsPluggedIn(&isPluggedIn);
    if (isPluggedIn) {
        routeMask |= IAudioState::ROUTE_WIRED_HEADSET;
    } else {
        routeMask |= IAudioState::ROUTE_EARPIECE;
    }
    Boolean isBluetoothAvailable;
    mBluetoothManager->IsBluetoothAvailable(&isBluetoothAvailable);
    if (isBluetoothAvailable) {
        routeMask |=  IAudioState::ROUTE_BLUETOOTH;
    }
    *result = routeMask;
    return NOERROR;
}

ECode CallAudioManager::GetInitialAudioState(
    /* [in] */ ICall* call,
    /* [out] */ IAudioState** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 supportedRouteMask;
    CalculateSupportedRoutes(&supportedRouteMask);
    Int32 route;
    SelectWiredOrEarpiece(
            IAudioState::ROUTE_WIRED_OR_EARPIECE, supportedRouteMask, &route);
    // We want the UI to indicate that "bluetooth is in use" in two slightly different cases:
    // (a) The obvious case: if a bluetooth headset is currently in use for an ongoing call.
    // (b) The not-so-obvious case: if an incoming call is ringing, and we expect that audio
    //     *will* be routed to a bluetooth headset once the call is answered. In this case, just
    //     check if the headset is available. Note this only applies when we are dealing with
    //     the first call.
    Boolean isBluetoothAvailable;
    mBluetoothManager->IsBluetoothAvailable(&isBluetoothAvailable);
    Boolean isBluetoothAudioOn;
    IsBluetoothAudioOn(&isBluetoothAudioOn);
    if (call != NULL && isBluetoothAvailable && isBluetoothAudioOn) {
        Int32 state;
        ((Call*) call)->GetState(&state);
        switch(state) {
            case ICallState::ACTIVE:
            case ICallState::ON_HOLD:
            case ICallState::DIALING:
            case ICallState::CONNECTING:
            case ICallState::RINGING:
                route = IAudioState::ROUTE_BLUETOOTH;
                break;
            default:
                break;
        }
    }
    CAudioState::New(FALSE, route, supportedRouteMask, result);
    return NOERROR;
}

ECode CallAudioManager::SetInitialAudioState(
    /* [in] */ ICall* call,
    /* [in] */ Boolean force)
{
    AutoPtr<IAudioState> audioState;
    GetInitialAudioState(call, (IAudioState**)&audioState);
    Log::V("CallAudioManager", "setInitialAudioState %s, %s", TO_CSTR(audioState), TO_CSTR(call));
    Boolean isMuted;
    audioState->GetIsMuted(&isMuted);
    Int32 route;
    audioState->GetRoute(&route);
    Int32 supportedRouteMask;
    audioState->GetSupportedRouteMask(&supportedRouteMask);
    SetSystemAudioState(
            force, isMuted, route, supportedRouteMask);
    return NOERROR;
}

ECode CallAudioManager::UpdateAudioForForegroundCall()
{
    AutoPtr<ICall> call;
    CallsManager::GetInstance()->GetForegroundCall((ICall**)&call);
    AutoPtr<IConnectionServiceWrapper> connectionService;
    ((Call*) call.Get())->GetConnectionService((IConnectionServiceWrapper**)&connectionService);
    if (call != NULL && connectionService != NULL) {
        ((ConnectionServiceWrapper*) connectionService.Get())->OnAudioStateChanged(call, mAudioState);
    }
    return NOERROR;
}

ECode CallAudioManager::GetForegroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ICall> call;
    CallsManager::GetInstance()->GetForegroundCall((ICall**)&call);
    // We ignore any foreground call that is in the ringing state because we deal with ringing
    // calls exclusively through the mIsRinging variable set by {@link Ringer}.
    Int32 state;
    ((Call*) call.Get())->GetState(&state);
    if (call != NULL && state == ICallState::RINGING && !mSpeedUpAudioForMtCall ) {
        call = NULL;
    }
    *result = call;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallAudioManager::HasRingingForegroundCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ICall> call;
    CallsManager::GetInstance()->GetForegroundCall((ICall**)&call);
    Int32 state;
    ((Call*) call.Get())->GetState(&state);
    *result = call != NULL && state == ICallState::RINGING;
    return NOERROR;
}

ECode CallAudioManager::HasFocus(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mAudioFocusStreamType != STREAM_NONE;
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
