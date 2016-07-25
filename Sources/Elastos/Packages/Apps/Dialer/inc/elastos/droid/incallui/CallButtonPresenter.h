
#ifndef __ELASTOS_DROID_INCALLUI_CALLBUTTONPRESENTER_H__
#define __ELASTOS_DROID_INCALLUI_CALLBUTTONPRESENTER_H__

#include "Elastos.Droid.Telecom.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/incallui/Call.h"
#include "elastos/droid/incallui/Presenter.h"

using Elastos::Droid::Telecom::ICallDetails;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class CallButtonPresenter
    : public Presenter
    , public IInCallStateListener
    , public IAudioModeListener
    , public IIncomingCallListener
    , public IInCallDetailsListener
{
public:
    CAR_INTERFACE_DECL();

    CallButtonPresenter();

    // @Override
    CARAPI OnUiReady(
        /* [in] */ IUi* ui);

    // @Override
    CARAPI OnUiUnready(
        /* [in] */ IUi* ui);

    // @Override
    CARAPI OnStateChange(
        /* [in] */ InCallState oldState,
        /* [in] */ InCallState newState,
        /* [in] */ ICallList* callList);

    // @Override
    CARAPI OnDetailsChanged(
        /* [in] */ ICall* call,
        /* [in] */ ICallDetails* details);

    // @Override
    CARAPI OnIncomingCall(
        /* [in] */ InCallState oldState,
        /* [in] */ InCallState newState,
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnAudioMode(
        /* [in] */ Int32 newMode);

    // @Override
    CARAPI OnSupportedAudioMode(
        /* [in] */ Int32 modeMask);

    // @Override
    CARAPI OnMute(
        /* [in] */ Boolean muted);

    CARAPI_(Int32) GetAudioMode();

    CARAPI_(Int32) GetSupportedAudio();

    CARAPI_(void) SetAudioMode(
        /* [in] */ Int32 mode);

    /**
     * Function assumes that bluetooth is not supported.
     */
    CARAPI_(void) ToggleSpeakerphone();

    CARAPI_(void) MuteClicked(
        /* [in] */ Boolean checked);

    CARAPI_(void) HoldClicked(
        /* [in] */ Boolean checked);

    CARAPI_(void) SwapClicked();

    CARAPI_(void) MergeClicked();

    CARAPI_(void) AddCallClicked();

    CARAPI_(void) ChangeToVoiceClicked();

    CARAPI_(void) ShowDialpadClicked(
        /* [in] */ Boolean checked);

    CARAPI_(void) ChangeToVideoClicked();

    /**
     * Switches the camera between the front-facing and back-facing camera.
     * @param useFrontFacingCamera True if we should switch to using the front-facing camera, or
     *     false if we should switch to using the back-facing camera.
     */
    CARAPI_(void) SwitchCameraClicked(
        /* [in] */ Boolean useFrontFacingCamera);

    /**
     * Stop or start client's video transmission.
     * @param pause True if pausing the local user's video, or false if starting the local user's
     *    video.
     */
    CARAPI_(void) PauseVideoClicked(
        /* [in] */ Boolean pause);

    CARAPI_(void) RefreshMuteState();

private:
    CARAPI_(void) UpdateUi(
        /* [in] */ InCallState state,
        /* [in] */ ICall* call);

    /**
     * Updates the buttons applicable for the UI.
     *
     * @param call The active call.
     * @param context The context.
     */
    CARAPI_(void) UpdateCallButtons(
        /* [in] */ ICall* call,
        /* [in] */ IContext* context);

    CARAPI_(void) UpdateVideoCallButtons();

    CARAPI_(void) UpdateVoiceCallButtons(
        /* [in] */ ICall* call);

private:
    AutoPtr<Call> mCall;
    Boolean mAutomaticallyMuted;
    Boolean mPreviousMuteState;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CALLBUTTONPRESENTER_H__
