
#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_VOLUMEPANEL_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_VOLUMEPANEL_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/Dialog.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/systemui/statusbar/phone/SystemUIDialog.h"
#include "elastos/droid/systemui/volume/ZenModePanel.h"
#include "elastos/droid/R.h"
#include <elastos/droid/media/session/MediaControllerCallback.h>

using Elastos::Droid::App::Dialog;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioService;
using Elastos::Droid::Media::IMediaMetadata;
using Elastos::Droid::Media::IToneGenerator;
using Elastos::Droid::Media::Session::IMediaController;
using Elastos::Droid::Media::Session::IMediaControllerCallback;
using Elastos::Droid::Media::Session::IMediaControllerPlaybackInfo;
using Elastos::Droid::Media::Session::IPlaybackState;
using Elastos::Droid::Media::Session::MediaControllerCallback;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::SystemUI::StatusBar::Phone::SystemUIDialog;
using Elastos::Droid::SystemUI::StatusBar::Policy::IZenModeController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IZenModeControllerCallback;
using Elastos::Droid::SystemUI::Volume::IInteractionCallback;
using Elastos::Droid::SystemUI::Volume::IVolumePanelCallback;
using Elastos::Droid::SystemUI::Volume::IVolumePanelStreamControl;
using Elastos::Droid::SystemUI::Volume::IZenModePanelCallback;
using Elastos::Droid::SystemUI::Volume::ZenModePanel;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::R;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Volume {

class CVolumePanelBroadcastReceiver1;
class CVolumePanelBroadcastReceiver2;

/**
 * Handles the user interface for the volume keys.
 *
 * @hide
 */
class VolumePanel
    : public Handler
    , public IVolumePanel
{
public:
    class StreamResources
        : public Object
    {
    public:
        StreamResources(
            /* [in] */ Int32 streamType,
            /* [in] */ Int32 descRes,
            /* [in] */ Int32 iconRes,
            /* [in] */ Int32 iconMuteRes,
            /* [in] */ Boolean show);

    public:
        Int32 mStreamType;
        Int32 mDescRes;
        Int32 mIconRes;
        Int32 mIconMuteRes;
        // RING, VOICE_CALL & BLUETOOTH_SCO are hidden unless explicitly requested
        Boolean mShow;
    };


    class ENUM_StreamResources
        : public Object
    {
    public:
        const static AutoPtr<StreamResources> BluetoothSCOStream;
        const static AutoPtr<StreamResources> RingerStream;
        const static AutoPtr<StreamResources> VoiceStream;
        const static AutoPtr<StreamResources> AlarmStream;
        const static AutoPtr<StreamResources> MediaStream;
        const static AutoPtr<StreamResources> NotificationStream;
        const static AutoPtr<StreamResources> MasterStream;
        const static AutoPtr<StreamResources> RemoteStream;
    };

private:
    /** Object that contains data for each slider */
    class StreamControl
        : public Object
        , public IVolumePanelStreamControl
    {
    public:
        CAR_INTERFACE_DECL()

        Int32 mStreamType;
        AutoPtr<IMediaController> mController;
        AutoPtr<IViewGroup> mGroup;
        AutoPtr<IImageView> mIcon;
        AutoPtr<ISeekBar> mSeekbarView;
        AutoPtr<ITextView> mSuppressorView;
        Int32 mIconRes;
        Int32 mIconMuteRes;
        Int32 mIconSuppressedRes;
    };

    class SafetyWarning
        : public SystemUIDialog
        , public IDialogInterfaceOnDismissListener
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        SafetyWarning(
            /* [in] */ IContext* context,
            /* [in] */ VolumePanel* volumePanel,
            /* [in] */ IAudioManager* audioManager);

        // @Override
        CARAPI OnKeyDown(
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnKeyUp(
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

        // @Override
        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* unused);

        CARAPI CleanUp();

    private:
        AutoPtr<IContext> mContext;
        VolumePanel* mVolumePanel;
        AutoPtr<IAudioManager> mAudioManager;
        Boolean mNewVolumeUp;
        AutoPtr<IBroadcastReceiver> mReceiver;
    };

    class MySeekListener
        : public Object
        , public ISeekBarOnSeekBarChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        MySeekListener(
            /* [in] */ VolumePanel* host);

        // @Override
        CARAPI OnProgressChanged(
            /* [in] */ ISeekBar* seekBar,
            /* [in] */ Int32 progress,
            /* [in] */ Boolean fromUser);

        // @Override
        CARAPI OnStartTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

        // @Override
        CARAPI OnStopTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

    private:
        VolumePanel* mHost;
    };

    class MyZenCallback
        : public Object
        , public IZenModeControllerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyZenCallback(
            /* [in] */ VolumePanel* host);

        // @Override
        CARAPI OnZenAvailableChanged(
            /* [in] */ Boolean available);

        // @Override
        CARAPI OnEffectsSupressorChanged();

        // @Override
        CARAPI OnZenChanged(
           /* [in] */ Int32 zen);

        // @Override
        CARAPI OnExitConditionChanged(
           /* [in] */ ICondition* exitCondition);

        // @Override
        CARAPI OnConditionsChanged(
           /* [in] */ ArrayOf<ICondition*>* conditions);

        // @Override
        CARAPI OnNextAlarmChanged();

    private:
        VolumePanel* mHost;
    };

    class MyMediaControllerCb: public MediaControllerCallback
    {
    public:
        MyMediaControllerCb(
            /* [in] */ VolumePanel* host);

        CARAPI OnAudioInfoChanged(
            /* [in] */ IMediaControllerPlaybackInfo* info);

    private:
        VolumePanel* mHost;
    };

    class MyDialog
        : public Dialog
    {
    public:
        MyDialog(
            /* [in] */ IContext* ctx,
            /* [in] */ VolumePanel* host);

        CARAPI OnTouchEvent(
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IContext> mCtx;
        VolumePanel* mHost;
    };

    class MyDismissListener
        : public Object
        , public IDialogInterfaceOnDismissListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyDismissListener(
            /* [in] */ VolumePanel* host);

        // @Override
        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

    private:
        VolumePanel* mHost;
    };

    class InteractionCallback
        : public Object
        , public IInteractionCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InteractionCallback(
            /* [in] */ VolumePanel* host);

        // @Override
        CARAPI OnInteraction();

    private:
        VolumePanel* mHost;
    };

    class ZMPCallback
        : public Object
        , public IZenModePanelCallback
    {
    public:
        CAR_INTERFACE_DECL()

        ZMPCallback(
            /* [in] */ VolumePanel* host);

        CARAPI OnMoreSettings();

        CARAPI OnInteraction();

        CARAPI OnExpanded(
            /* [in] */ Boolean expanded);

    private:
        VolumePanel* mHost;
    };

    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ StreamControl* sc,
            /* [in] */ VolumePanel* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        AutoPtr<StreamControl> mSc;
        VolumePanel* mHost;
    };

    class MyOnTouchListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnTouchListener(
            /* [in] */ VolumePanel* host);

        // @Override
        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        VolumePanel* mHost;
    };

    friend class SafetyWarning;
    friend class StreamControl;
    friend class MySeekListener;
    friend class MyZenCallback;
    friend class MyDialog;
    friend class InteractionCallback;
    friend class ZMPCallback;
    friend class CVolumePanelBroadcastReceiver1;
    friend class CVolumePanelBroadcastReceiver2;
    friend class MyOnTouchListener;

public:
    CAR_INTERFACE_DECL()

    VolumePanel();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IZenModeController* zenController);

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI IsMuted(
        /* [in] */ Int32 streamType,
        /* [out] */ Boolean* isMuted);

    CARAPI GetStreamMaxVolume(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* smv);

    CARAPI GetStreamVolume(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* sv);

    CARAPI SetCallback(
        /* [in] */ IVolumePanelCallback* callback);

    CARAPI IsZenPanelVisible(
        /* [out] */ Boolean* result);

    CARAPI UpdateStates();

    CARAPI PostVolumeChanged(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 flags);

    CARAPI PostRemoteVolumeChanged(
        /* [in] */ IMediaController* controller,
        /* [in] */ Int32 flags);

    CARAPI PostRemoteSliderVisibility(
        /* [in] */ Boolean visible);

    /**
     * Called by AudioService when it has received new remote playback information that
     * would affect the VolumePanel display (mainly volumes). The difference with
     * {@link #postRemoteVolumeChanged(Int32, Int32)} is that the handling of the posted message
     * (MSG_REMOTE_VOLUME_UPDATE_IF_SHOWN) will only update the volume slider if it is being
     * displayed.
     * This special code path is due to the fact that remote volume updates arrive to AudioService
     * asynchronously. So after AudioService has sent the volume update (which should be treated
     * as a request to update the volume), the application will likely set a new volume. If the UI
     * is still up, we need to refresh the display to show this new value.
     */
    CARAPI PostHasNewRemotePlaybackInfo();

    CARAPI PostMasterVolumeChanged(
        /* [in] */ Int32 flags);

    CARAPI PostMuteChanged(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 flags);

    CARAPI PostMasterMuteChanged(
        /* [in] */ Int32 flags);

    CARAPI PostDisplaySafeVolumeWarning(
        /* [in] */ Int32 flags);

    CARAPI PostDismiss(
        /* [in] */ Int64 delay);

    CARAPI PostLayoutDirection(
        /* [in] */ Int32 layoutDirection);

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    CARAPI GetZenController(
        /* [out] */ IZenModeController** zc);

protected:
    /**
     * Override this if you have other work to do when the volume changes (for
     * example, vibrating, playing a sound, etc.). Make sure to call through to
     * the superclass implementation.
     */
    CARAPI OnVolumeChanged(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 flags);

    CARAPI OnMuteChanged(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 flags);

    CARAPI OnShowVolumeChanged(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 flags,
        /* [in] */ IMediaController* controller);

    CARAPI OnPlaySound(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 flags);

    CARAPI OnStopSounds();

    CARAPI OnVibrate();

    CARAPI OnRemoteVolumeChanged(
        /* [in] */ IMediaController* controller,
        /* [in] */ Int32 flags);

    CARAPI OnRemoteVolumeUpdateIfShown();

    /**
     * Handler for MSG_SLIDER_VISIBILITY_CHANGED Hide or show a slider
     *
     * @param streamType can be a valid stream type value, or
     *            VolumePanel.STREAM_MASTER, or VolumePanel.STREAM_REMOTE_MUSIC
     * @param visible
     */
    /*synchronized*/
    CARAPI OnSliderVisibilityChanged(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 visible);

    CARAPI OnDisplaySafeVolumeWarning(
        /* [in] */ Int32 flags);

    CARAPI OnFreeResources();

private:
    CARAPI_(void) UpdateWidth();

    CARAPI_(void) InitZenModePanel();

    CARAPI_(void) SetLayoutDirection(
        /* [in] */ Int32 layoutDirection);

    CARAPI_(void) RegisterReceiver();

    CARAPI_(void) SetStreamVolume(
        /* [in] */ StreamControl* sc,
        /* [in] */ Int32 index,
        /* [in] */ Int32 flags);

    CARAPI_(void) CreateSliders();

    CARAPI_(void) Toggle(
        /* [in] */ StreamControl* sc);

    CARAPI_(void) ReorderSliders(
        /* [in] */ Int32 activeStreamType);

    CARAPI_(void) UpdateSliderProgress(
        /* [in] */ StreamControl* sc,
        /* [in] */ Int32 progress);

    CARAPI_(void) UpdateSliderIcon(
        /* [in] */ StreamControl* sc,
        /* [in] */ Boolean muted);

    CARAPI_(void) UpdateSliderSupressor(
        /* [in] */ StreamControl* sc);

    CARAPI_(String) GetSuppressorCaption(
        /* [in] */ IComponentName* suppressor);

    /** Update the mute and progress state of a slider */
    CARAPI_(void) UpdateSlider(
        /* [in] */ StreamControl* sc);

    CARAPI_(void) UpdateSliderEnabled(
        /* [in] */ StreamControl* sc,
        /* [in] */ Boolean muted,
        /* [in] */ Boolean fixedVolume);

    CARAPI_(void) ShowSilentHint();

    static CARAPI_(Boolean) IsNotificationOrRing(
        /* [in] */ Int32 streamType);

    CARAPI_(void) UpdateTimeoutDelay();


    CARAPI_(void) SetZenPanelVisible(
        /* [in] */ Boolean visible);

    CARAPI_(void) UpdateZenPanelVisible();

    CARAPI_(void) AnnounceDialogShown();

    CARAPI_(Boolean) IsShowing();

    /**
     * Clear the current remote stream controller.
     */
    CARAPI_(void) ClearRemoteStreamController();

    /**
     * Lock on this VolumePanel instance as long as you use the returned ToneGenerator.
     */
    CARAPI_(AutoPtr<IToneGenerator>) GetOrCreateToneGenerator(
        /* [in] */ Int32 streamType);


    /**
     * Switch between icons because Bluetooth music is same as music volume, but with
     * different icons.
     */
    CARAPI_(void) SetMusicIcon(
        /* [in] */ Int32 resId,
        /* [in] */ Int32 resMuteId);

    CARAPI_(void) ResetTimeout();

    CARAPI_(void) ForceTimeout(
        /* [in] */ Int64 delay);

public:
    /**
     * The delay before vibrating. This small period exists so if the user is
     * moving to silent mode, it will not emit a short vibrate (it normally
     * would since vibrate is between normal mode and silent mode using hardware
     * keys).
     */
    const static Int32 VIBRATE_DELAY = 300;

protected:
    AutoPtr<IContext> mContext;
    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<IZenModeController> mZenController;
    Boolean mRingIsSilent;
    Boolean mVoiceCapable;
    Boolean mZenModeAvailable;
    Boolean mZenPanelExpanded;
    Int32 mTimeoutDelay;
    Float mDisabledAlpha;
    Int32 mLastRingerMode;
    Int32 mLastRingerProgress;

    // True if we want to play tones on the system stream when the master stream is specified.
    Boolean mPlayMasterStreamTones;


    /** Volume panel content view */
    AutoPtr<IView> mView;
    /** Dialog hosting the panel */
    AutoPtr<IDialog> mDialog;

    /** The visible portion of the volume overlay */
    AutoPtr<IViewGroup> mPanel;
    /** Contains the slider and its touchable icons */
    AutoPtr<IViewGroup> mSliderPanel;
    /** The zen mode configuration panel view */
    AutoPtr<ZenModePanel> mZenPanel;

    AutoPtr<IVolumePanelCallback> mCallback;

    /** Currently active stream that shows up at the top of the list of sliders */
    Int32 mActiveStreamType;
    /** All the slider controls mapped by stream type */
    AutoPtr<ISparseArray> mStreamControls;
    AutoPtr<IAccessibilityManager> mAccessibilityManager;

    // List of stream types and their order
    const static AutoPtr<ArrayOf<StreamResources*> > STREAMS;

    // Synchronize when accessing this
    AutoPtr<ArrayOf<IToneGenerator*> > mToneGenerators;
    AutoPtr<IVibrator> mVibrator;

    static AutoPtr<IAlertDialog> sSafetyWarning;
    static AutoPtr<IInterface> sSafetyWarningLock;

private:
    const static String TAG;
    static Boolean LOGD;
    const static Int32 PLAY_SOUND_DELAY = IAudioService::PLAY_SOUND_DELAY;
    const static Int32 VIBRATE_DURATION = 300;
    const static Int32 BEEP_DURATION = 150;
    const static Int32 MAX_VOLUME = 100;
    const static Int32 FREE_DELAY = 10000;
    const static Int32 TIMEOUT_DELAY = 3000;
    const static Int32 TIMEOUT_DELAY_SHORT = 1500;
    const static Int32 TIMEOUT_DELAY_COLLAPSED = 4500;
    const static Int32 TIMEOUT_DELAY_SAFETY_WARNING = 5000;
    const static Int32 TIMEOUT_DELAY_EXPANDED = 10000;

    const static Int32 MSG_VOLUME_CHANGED = 0;
    const static Int32 MSG_FREE_RESOURCES = 1;
    const static Int32 MSG_PLAY_SOUND = 2;
    const static Int32 MSG_STOP_SOUNDS = 3;
    const static Int32 MSG_VIBRATE = 4;
    const static Int32 MSG_TIMEOUT = 5;
    const static Int32 MSG_RINGER_MODE_CHANGED = 6;
    const static Int32 MSG_MUTE_CHANGED = 7;
    const static Int32 MSG_REMOTE_VOLUME_CHANGED = 8;
    const static Int32 MSG_REMOTE_VOLUME_UPDATE_IF_SHOWN = 9;
    const static Int32 MSG_SLIDER_VISIBILITY_CHANGED = 10;
    const static Int32 MSG_DISPLAY_SAFE_VOLUME_WARNING = 11;
    const static Int32 MSG_LAYOUT_DIRECTION = 12;
    const static Int32 MSG_ZEN_MODE_AVAILABLE_CHANGED = 13;
    const static Int32 MSG_USER_ACTIVITY = 14;
    const static Int32 MSG_NOTIFICATION_EFFECTS_SUPPRESSOR_CHANGED = 15;

    // Pseudo stream type for master volume
    const static Int32 STREAM_MASTER = -100;
    // Pseudo stream type for remote volume
    const static Int32 STREAM_REMOTE_MUSIC = -200;

    const static AutoPtr<IAudioAttributes> VIBRATION_ATTRIBUTES;

    const static Int32 IC_AUDIO_VOL = R::drawable::ic_audio_vol;
    const static Int32 IC_AUDIO_VOL_MUTE = R::drawable::ic_audio_vol_mute;

    String mTag;

    AutoPtr<MySeekListener> mSeekListener;
    AutoPtr<MyZenCallback> mZenCallback;
    AutoPtr<MyMediaControllerCb> mMediaControllerCb;
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::SystemUI::Volume::VolumePanel::StreamResources, IInterface)

#endif //__ELASTOS_DROID_SYSTEMUI_VOLUME_VOLUMEPANEL_H__