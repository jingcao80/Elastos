
#include "elastos/droid/incallui/CallButtonPresenter.h"
#include "elastos/droid/incallui/AudioModeProvider.h"
#include "elastos/droid/incallui/InCallPresenter.h"
#include "elastos/droid/incallui/CallList.h"
#include "elastos/droid/incallui/TelecomAdapter.h"
#include "elastos/utility/Objects.h"
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Telephony.h"

using Elastos::Droid::InCallUI::EIID_IInCallStateListener;
using Elastos::Droid::InCallUI::EIID_IAudioModeListener;
using Elastos::Droid::InCallUI::EIID_IIncomingCallListener;
using Elastos::Droid::InCallUI::EIID_IInCallDetailsListener;
using Elastos::Droid::Telecom::IAudioStateHelper;
using Elastos::Droid::Telecom::CAudioStateHelper;
using Elastos::Droid::Telecom::IAudioState;
using Elastos::Droid::Telecom::IPhoneCapabilities;
using Elastos::Droid::Telecom::IVideoProfile;
using Elastos::Droid::Telecom::CVideoProfile;
using Elastos::Droid::Telecom::IVideoProfileVideoState;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Utility::Objects;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

CAR_INTERFACE_IMPL_4(CallButtonPresenter, Presenter
        , IInCallStateListener
        , IAudioModeListener
        , IIncomingCallListener
        , IInCallDetailsListener);

CallButtonPresenter::CallButtonPresenter()
    : mAutomaticallyMuted(FALSE)
    , mPreviousMuteState(FALSE)
{}

ECode CallButtonPresenter::OnUiReady(
    /* [in] */ IUi* ui)
{
    Presenter::OnUiReady(ui);

    AudioModeProvider::GetInstance()->AddListener(IAudioModeListener::Probe(this));

    // register for call state changes last
    InCallPresenter::GetInstance()->AddListener(IInCallStateListener::Probe(this));
    InCallPresenter::GetInstance()->AddIncomingCallListener(IIncomingCallListener::Probe(this));
    InCallPresenter::GetInstance()->AddDetailsListener(IInCallDetailsListener::Probe(this));
    return NOERROR;
}

ECode CallButtonPresenter::OnUiUnready(
    /* [in] */ IUi* ui)
{
    Presenter::OnUiUnready(ui);

    InCallPresenter::GetInstance()->RemoveListener(IInCallStateListener::Probe(this));
    AudioModeProvider::GetInstance()->RemoveListener(IAudioModeListener::Probe(this));
    InCallPresenter::GetInstance()->RemoveIncomingCallListener(IIncomingCallListener::Probe(this));
    InCallPresenter::GetInstance()->RemoveDetailsListener(IInCallDetailsListener::Probe(this));
    return NOERROR;
}

ECode CallButtonPresenter::OnStateChange(
    /* [in] */ InCallState oldState,
    /* [in] */ InCallState newState,
    /* [in] */ ICallList* _callList)
{
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<ICallButtonUi> ui = ICallButtonUi::Probe(temp);

    AutoPtr<CallList> callList = (CallList*)_callList;
    if (newState == InCallState_OUTGOING) {
        mCall = callList->GetOutgoingCall();
    }
    else if (newState == InCallState_INCALL) {
        mCall = callList->GetActiveOrBackgroundCall();

        // When connected to voice mail, automatically shows the dialpad.
        // (On previous releases we showed it when in-call shows up, before waiting for
        // OUTGOING.  We may want to do that once we start showing "Voice mail" label on
        // the dialpad too.)
        if (ui != NULL) {
            if (oldState == InCallState_OUTGOING && mCall != NULL) {
                AutoPtr<IPhoneNumberUtils> utils;
                CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
                Boolean result;
                if (utils->IsVoiceMailNumber(mCall->GetNumber(), &result), result) {
                    ui->DisplayDialpad(TRUE /* show */, TRUE /* animate */);
                }
            }
        }
    }
    else if (newState == InCallState_INCOMING) {
        if (ui != NULL) {
            ui->DisplayDialpad(FALSE /* show */, TRUE /* animate */);
        }
        mCall = NULL;
    }
    else {
        mCall = NULL;
    }
    UpdateUi(newState, mCall);
    return NOERROR;
}

ECode CallButtonPresenter::OnDetailsChanged(
    /* [in] */ ICall* call,
    /* [in] */ ICallDetails* details)
{
    // If the details change is not for the currently active call no update is required.
    if (!Objects::Equals(call, (ICall*)mCall)) {
        return NOERROR;
    }

    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<ICallButtonUi> ui = ICallButtonUi::Probe(temp);
    AutoPtr<IContext> ctx;
    ui->GetContext((IContext**)&ctx);
    UpdateCallButtons(call, ctx);
    return NOERROR;
}

ECode CallButtonPresenter::OnIncomingCall(
    /* [in] */ InCallState oldState,
    /* [in] */ InCallState newState,
    /* [in] */ ICall* call)
{
    AutoPtr<ICallList> callList = (ICallList*)CallList::GetInstance();
    OnStateChange(oldState, newState, callList);
    return NOERROR;
}

ECode CallButtonPresenter::OnAudioMode(
    /* [in] */ Int32 mode)
{
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<ICallButtonUi> ui = ICallButtonUi::Probe(temp);
    if (ui != NULL) {
        ui->SetAudio(mode);
    }
    return NOERROR;
}

ECode CallButtonPresenter::OnSupportedAudioMode(
    /* [in] */ Int32 mask)
{
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<ICallButtonUi> ui = ICallButtonUi::Probe(temp);
    if (ui != NULL) {
        ui->SetSupportedAudio(mask);
    }
    return NOERROR;
}

ECode CallButtonPresenter::OnMute(
    /* [in] */ Boolean muted)
{
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<ICallButtonUi> ui = ICallButtonUi::Probe(temp);
    if (ui != NULL && !mAutomaticallyMuted) {
        ui->SetMute(muted);
    }
    return NOERROR;
}

Int32 CallButtonPresenter::GetAudioMode()
{
    return AudioModeProvider::GetInstance()->GetAudioMode();
}

Int32 CallButtonPresenter::GetSupportedAudio()
{
    return AudioModeProvider::GetInstance()->GetSupportedModes();
}

void CallButtonPresenter::SetAudioMode(
    /* [in] */ Int32 mode)
{
    // TODO: Set a intermediate state in this presenter until we get
    // an update for onAudioMode().  This will make UI response immediate
    // if it turns out to be slow

    AutoPtr<IAudioStateHelper> helper;
    CAudioStateHelper::AcquireSingleton((IAudioStateHelper**)&helper);
    String str;
    helper->AudioRouteToString(mode, &str);
    Logger::D("CallButtonPresenter", "Sending new Audio Mode: %s", str.string());
    TelecomAdapter::GetInstance()->SetAudioRoute(mode);
}

void CallButtonPresenter::ToggleSpeakerphone()
{
    // this function should not be called if bluetooth is available
    if (0 != (IAudioState::ROUTE_BLUETOOTH & GetSupportedAudio())) {

        // It's clear the UI is wrong, so update the supported mode once again.
        Logger::E("CallButtonPresenter", "toggling speakerphone not allowed when bluetooth supported.");
        AutoPtr<IUi> temp;
        GetUi((IUi**)&temp);
        ICallButtonUi::Probe(temp)->SetSupportedAudio(GetSupportedAudio());
        return;
    }

    Int32 newMode = IAudioState::ROUTE_SPEAKER;

    // if speakerphone is already on, change to wired/earpiece
    if (GetAudioMode() == IAudioState::ROUTE_SPEAKER) {
        newMode = IAudioState::ROUTE_WIRED_OR_EARPIECE;
    }

    SetAudioMode(newMode);
}

void CallButtonPresenter::MuteClicked(
    /* [in] */ Boolean checked)
{
    Logger::D("CallButtonPresenter", "turning on mute: %d", checked);
    TelecomAdapter::GetInstance()->Mute(checked);
}

void CallButtonPresenter::HoldClicked(
    /* [in] */ Boolean checked)
{
    if (mCall == NULL) {
        return;
    }
    if (checked) {
        Logger::I("CallButtonPresenter", "Putting the call on hold: %s", TO_CSTR(mCall));
        TelecomAdapter::GetInstance()->HoldCall(mCall->GetId());
    }
    else {
        Logger::I("CallButtonPresenter", "Removing the call from hold: %s", TO_CSTR(mCall));
        TelecomAdapter::GetInstance()->UnholdCall(mCall->GetId());
    }
}

void CallButtonPresenter::SwapClicked()
{
    if (mCall == NULL) {
        return;
    }

    Logger::I("CallButtonPresenter", "Swapping the call: %s", TO_CSTR(mCall));
    TelecomAdapter::GetInstance()->Swap(mCall->GetId());
}

void CallButtonPresenter::MergeClicked()
{
    TelecomAdapter::GetInstance()->Merge(mCall->GetId());
}

void CallButtonPresenter::AddCallClicked()
{
    // Automatically mute the current call
    mAutomaticallyMuted = TRUE;
    mPreviousMuteState = AudioModeProvider::GetInstance()->GetMute();
    // Simulate a click on the mute button
    MuteClicked(TRUE);

    TelecomAdapter::GetInstance()->AddCall();
}

void CallButtonPresenter::ChangeToVoiceClicked()
{
    AutoPtr<IInCallServiceVideoCall> videoCall = mCall->GetVideoCall();
    if (videoCall == NULL) {
        return;
    }

    AutoPtr<IVideoProfile> videoProfile;
    CVideoProfile::New(IVideoProfileVideoState::AUDIO_ONLY, IVideoProfile::QUALITY_DEFAULT, (IVideoProfile**)&videoProfile);
    videoCall->SendSessionModifyRequest(videoProfile);
}

void CallButtonPresenter::ShowDialpadClicked(
    /* [in] */ Boolean checked)
{
    Logger::V("CallButtonPresenter", "Show dialpad %d", checked);
    AutoPtr<IUi> ui;
    GetUi((IUi**)&ui);
    ICallButtonUi::Probe(ui)->DisplayDialpad(checked /* show */, TRUE /* animate */);
}

void CallButtonPresenter::ChangeToVideoClicked()
{
    AutoPtr<IInCallServiceVideoCall> videoCall = mCall->GetVideoCall();
    if (videoCall == NULL) {
        return;
    }

    AutoPtr<IVideoProfile> videoProfile;
    CVideoProfile::New(IVideoProfileVideoState::BIDIRECTIONAL, (IVideoProfile**)&videoProfile);
    videoCall->SendSessionModifyRequest(videoProfile);

    mCall->SetSessionModificationState(Call::SessionModificationState::REQUEST_FAILED);
}

void CallButtonPresenter::SwitchCameraClicked(
    /* [in] */ Boolean useFrontFacingCamera)
{
    AutoPtr<InCallCameraManager> cameraManager = InCallPresenter::GetInstance()->GetInCallCameraManager();
    cameraManager->SetUseFrontFacingCamera(useFrontFacingCamera);

    AutoPtr<IInCallServiceVideoCall> videoCall = mCall->GetVideoCall();
    if (videoCall == NULL) {
        return;
    }

    String cameraId = cameraManager->GetActiveCameraId();
    if (!cameraId.IsNull()) {
        videoCall->SetCamera(cameraId);
        videoCall->RequestCameraCapabilities();
    }
    AutoPtr<IUi> ui;
    GetUi((IUi**)&ui);
    ICallButtonUi::Probe(ui)->SetSwitchCameraButton(!useFrontFacingCamera);
}

void CallButtonPresenter::PauseVideoClicked(
    /* [in] */ Boolean pause)
{
    AutoPtr<IInCallServiceVideoCall> videoCall = mCall->GetVideoCall();
    if (videoCall == NULL) {
        return;
    }

    if (pause) {
        videoCall->SetCamera(String(NULL));
        AutoPtr<IVideoProfile> videoProfile;
        CVideoProfile::New(mCall->GetVideoState() | IVideoProfileVideoState::PAUSED, (IVideoProfile**)&videoProfile);
        videoCall->SendSessionModifyRequest(videoProfile);
    }
    else {
        AutoPtr<InCallCameraManager> cameraManager = InCallPresenter::GetInstance()->GetInCallCameraManager();
        videoCall->SetCamera(cameraManager->GetActiveCameraId());
        AutoPtr<IVideoProfile> videoProfile;
        CVideoProfile::New(mCall->GetVideoState() & ~IVideoProfileVideoState::PAUSED, (IVideoProfile**)&videoProfile);
        videoCall->SendSessionModifyRequest(videoProfile);
    }
    AutoPtr<IUi> ui;
    GetUi((IUi**)&ui);
    ICallButtonUi::Probe(ui)->SetPauseVideoButton(pause);
}

void CallButtonPresenter::UpdateUi(
    /* [in] */ InCallState state,
    /* [in] */ ICall* call)
{
    Logger::D("CallButtonPresenter", "Updating call UI for call: ", call);

    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<ICallButtonUi> ui = ICallButtonUi::Probe(temp);
    if (ui == NULL) {
        return;
    }

    Boolean isEnabled = (state == InCallState_INCOMING || state == InCallState_OUTGOING || state == InCallState_INCALL)
            && (state != InCallState_INCOMING) && call != NULL;
    ui->SetEnabled(isEnabled);

    if (!isEnabled) {
        return;
    }

    AutoPtr<IContext> ctx;
    ui->GetContext((IContext**)&ctx);
    UpdateCallButtons(call, ctx);

    ui->EnableMute(((Call*)call)->Can(IPhoneCapabilities::MUTE));
}

void CallButtonPresenter::UpdateCallButtons(
    /* [in] */ ICall* _call,
    /* [in] */ IContext* context)
{
    AutoPtr<Call> call = (Call*)_call;
    if (call->IsVideoCall(context)) {
        UpdateVideoCallButtons();
    }
    else {
        UpdateVoiceCallButtons(call);
    }
}

void CallButtonPresenter::UpdateVideoCallButtons()
{
    Logger::V("CallButtonPresenter", "Showing buttons for video call.");
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<ICallButtonUi> ui = ICallButtonUi::Probe(temp);

    // Hide all voice-call-related buttons.
    ui->ShowAudioButton(FALSE);
    ui->ShowDialpadButton(FALSE);
    ui->ShowHoldButton(FALSE);
    ui->ShowSwapButton(FALSE);
    ui->ShowChangeToVideoButton(FALSE);
    ui->ShowAddCallButton(FALSE);
    ui->ShowMergeButton(FALSE);
    ui->ShowOverflowButton(FALSE);

    // Show all video-call-related buttons.
    ui->ShowChangeToVoiceButton(TRUE);
    ui->ShowSwitchCameraButton(TRUE);
    ui->ShowPauseVideoButton(TRUE);
}

void CallButtonPresenter::UpdateVoiceCallButtons(
    /* [in] */ Call* call)
{
    Logger::V("CallButtonPresenter", "Showing buttons for voice call.");
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<ICallButtonUi> ui = ICallButtonUi::Probe(temp);

    // Hide all video-call-related buttons.
    ui->ShowChangeToVoiceButton(FALSE);
    ui->ShowSwitchCameraButton(FALSE);
    ui->ShowPauseVideoButton(FALSE);

    // Show all voice-call-related buttons.
    ui->ShowAudioButton(TRUE);
    ui->ShowDialpadButton(TRUE);

    Logger::V("CallButtonPresenter", "Show hold %d", call->Can(IPhoneCapabilities::SUPPORT_HOLD));
    Logger::V("CallButtonPresenter", "Enable hold%d", call->Can(IPhoneCapabilities::HOLD));
    Logger::V("CallButtonPresenter", "Show merge %d", call->Can(IPhoneCapabilities::MERGE_CONFERENCE));
    Logger::V("CallButtonPresenter", "Show swap %d", call->Can(IPhoneCapabilities::SWAP_CONFERENCE));
    Logger::V("CallButtonPresenter", "Show add call %d", call->Can(IPhoneCapabilities::ADD_CALL));
    Logger::V("CallButtonPresenter", "Show mute %d", call->Can(IPhoneCapabilities::MUTE));

    Boolean canAdd = call->Can(IPhoneCapabilities::ADD_CALL);
    Boolean enableHoldOption = call->Can(IPhoneCapabilities::HOLD);
    Boolean supportHold = call->Can(IPhoneCapabilities::SUPPORT_HOLD);

    Boolean canVideoCall = call->Can(IPhoneCapabilities::SUPPORTS_VT_LOCAL)
            && call->Can(IPhoneCapabilities::SUPPORTS_VT_REMOTE);
    ui->ShowChangeToVideoButton(canVideoCall);

    Boolean showMergeOption = call->Can(IPhoneCapabilities::MERGE_CONFERENCE);
    Boolean showAddCallOption = canAdd;

    // Show either HOLD or SWAP, but not both. If neither HOLD or SWAP is available:
    //     (1) If the device normally can hold, show HOLD in a disabled state.
    //     (2) If the device doesn't have the concept of hold/swap, remove the button.
    Boolean showSwapOption = call->Can(IPhoneCapabilities::SWAP_CONFERENCE);
    Boolean showHoldOption = !showSwapOption && (enableHoldOption || supportHold);

    ui->SetHold(call->GetState() == Call::State::ONHOLD);
    // If we show video upgrade and add/merge and hold/swap, the overflow menu is needed.
    Boolean isVideoOverflowScenario = canVideoCall
            && (showAddCallOption || showMergeOption) && (showHoldOption || showSwapOption);
    // If we show hold/swap, add, and merge simultaneously, the overflow menu is needed.
    Boolean isCdmaConferenceOverflowScenario =
            (showHoldOption || showSwapOption) && showMergeOption && showAddCallOption;

    if (isVideoOverflowScenario) {
        ui->ShowHoldButton(FALSE);
        ui->ShowSwapButton(FALSE);
        ui->ShowAddCallButton(FALSE);
        ui->ShowMergeButton(FALSE);

        ui->ShowOverflowButton(TRUE);
        ui->ConfigureOverflowMenu(
                showMergeOption,
                showAddCallOption /* showAddMenuOption */,
                showHoldOption && enableHoldOption /* showHoldMenuOption */,
                showSwapOption);
    }
    else {
        if (isCdmaConferenceOverflowScenario) {
            ui->ShowAddCallButton(FALSE);
            ui->ShowMergeButton(FALSE);

            ui->ConfigureOverflowMenu(
                    showMergeOption,
                    showAddCallOption /* showAddMenuOption */,
                    FALSE /* showHoldMenuOption */,
                    FALSE /* showSwapMenuOption */);
        }
        else {
            ui->ShowMergeButton(showMergeOption);
            ui->ShowAddCallButton(showAddCallOption);
        }

        ui->ShowHoldButton(showHoldOption);
        ui->EnableHold(enableHoldOption);
        ui->ShowSwapButton(showSwapOption);
    }
}

void CallButtonPresenter::RefreshMuteState()
{
    // Restore the previous mute state
    if (mAutomaticallyMuted &&
            AudioModeProvider::GetInstance()->GetMute() != mPreviousMuteState) {
        AutoPtr<IUi> ui;
        if (GetUi((IUi**)&ui), ui == NULL) {
            return;
        }
        MuteClicked(mPreviousMuteState);
    }
    mAutomaticallyMuted = FALSE;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos