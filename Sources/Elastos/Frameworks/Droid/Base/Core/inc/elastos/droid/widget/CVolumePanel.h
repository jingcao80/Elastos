
#ifndef __ELASTOS_DROID_WIDGET_CVOLUMEPANEL_H__
#define __ELASTOS_DROID_WIDGET_CVOLUMEPANEL_H__

#include "_Elastos_Droid_Widget_CVolumePanel.h"
#include "elastos/droid/ext/frameworkext.h"

#include "elastos/droid/os/Handler.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Media::IAudioService;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IToneGenerator;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Widget {

extern "C" const InterfaceID EIID_StreamControl;

CarClass(CVolumePanel), public Handler
{
private:
    class StreamResources {
    public:
        Int32 mStreamType;
        Int32 mDescRes;
        Int32 mIconRes;
        Int32 mIconMuteRes;
        // RING, VOICE_CALL & BLUETOOTH_SCO are hidden unless explicitly requested
        Boolean mShow;

        StreamResources(
            /* [in] */ Int32 streamType,
            /* [in] */ Int32 descRes,
            /* [in] */ Int32 iconRes,
            /* [in] */ Int32 iconMuteRes,
            /* [in] */ Boolean show)
        {
            mStreamType = streamType;
            mDescRes = descRes;
            mIconRes = iconRes;
            mIconMuteRes = iconMuteRes;
            mShow = show;
        }
    };

    /** Object that contains data for each slider */
    class StreamControl
        : public IInterface
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        Int32 mStreamType;
        AutoPtr<IViewGroup> mGroup;
        AutoPtr<IImageView> mIcon;
        AutoPtr<ISeekBar> mSeekbarView;
        Int32 mIconRes;
        Int32 mIconMuteRes;
    };

    class WarningDialogReceiver
        : public BroadcastReceiver
        , public IDialogInterfaceOnDismissListener
    {
    public:
        CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        WarningDialogReceiver(
            /* [in] */ IContext* context,
            /* [in] */ IDialog* dialog);

        //Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CVolumePanel::WarningDialogReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }

        CARAPI OnDismiss(
            /* [int] */ IDialogInterface* unused);
    private:
        AutoPtr<IContext> mContext;
        AutoPtr<IDialog> mDialog;
    };

    class RingerModeChangedActionReceiver
        : public BroadcastReceiver
    {
    public:
        RingerModeChangedActionReceiver(
            /* [in] */ IHandler* host);

        //Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CVolumePanel::RingerModeChangedActionReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        IHandler* mHost;
    };

    class ViewOnTouchListener
        : public IViewOnTouchListener
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        ViewOnTouchListener(
            /* [in] */ CVolumePanel* host);

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);
    private:
        CVolumePanel* mHost;
    };

    class DialogOnDismissListener
        : public IDialogInterfaceOnDismissListener
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        DialogOnDismissListener(
            /* [in] */ CVolumePanel* host);

        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);
    private:
        CVolumePanel* mHost;
    };

    class PositiveButtonOnClickListener
        : public ElRefBase
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        PositiveButtonOnClickListener(
            /* [in] */ CVolumePanel* host);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);
    private:
        CVolumePanel* mHost;
    };
public:
    IHANDLER_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CVolumePanel();

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAudioService* realOwner);

    CARAPI PostVolumeChanged(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 flags);

    CARAPI PostRemoteVolumeChanged(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 flags);

    CARAPI PostRemoteSliderVisibility(
        /* [in] */ Boolean visible);

    /**
     * Called by AudioService when it has received new remote playback information that
     * would affect the VolumePanel display (mainly volumes). The difference with
     * {@link #postRemoteVolumeChanged(int, int)} is that the handling of the posted message
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

    CARAPI PostDisplaySafeVolumeWarning();

    CARAPI OnClick(
        /* [in] */ IView* v);

    CARAPI OnProgressChanged(
        /* [in] */ ISeekBar* seekBar,
        /* [in] */ Int32 progress,
        /* [in] */ Boolean fromUser);

    CARAPI OnStartTrackingTouch(
        /* [in] */ ISeekBar* seekBar);

    CARAPI OnStopTrackingTouch(
        /* [in] */ ISeekBar* seekBar);

    virtual CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

protected:
    /**
     * Override this if you have other work to do when the volume changes (for
     * example, vibrating, playing a sound, etc.). Make sure to call through to
     * the superclass implementation.
     */
    void OnVolumeChanged(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 flags);

    void OnMuteChanged(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 flags);

    void OnShowVolumeChanged(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 flags);

    void OnPlaySound(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 flags);

    void OnStopSounds();

    void OnVibrate();

    void OnRemoteVolumeChanged(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 flags);

    void OnRemoteVolumeUpdateIfShown();

    /**
     * Handler for MSG_SLIDER_VISIBILITY_CHANGED
     * Hide or show a slider
     * @param streamType can be a valid stream type value, or VolumePanel.STREAM_MASTER,
     *                   or AudioService.STREAM_REMOTE_MUSIC
     * @param visible
     */
    /* synchronized */ void OnSliderVisibilityChanged(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 visible);

    void OnDisplaySafeVolumeWarning();

    void OnFreeResources();

private:
    void ListenToRingerMode();

    Boolean IsMuted(
        /* [in] */ Int32 streamType) ;

    Int32 GetStreamMaxVolume(
        /* [in] */ Int32 streamType);

    Int32 GetStreamVolume(
        /* [in] */ Int32 streamType);

    void SetStreamVolume(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 index,
        /* [in] */ Int32 flags);

    void CreateSliders();

    void ReorderSliders(
        /* [in] */ Int32 activeStreamType);

    void AddOtherVolumes();

    void UpdateSlider(
        /* [in] */ StreamControl* sc);

    Boolean IsExpanded();

    void Expand();

    void Collapse();

    void UpdateStates();

    AutoPtr<IToneGenerator> GetOrCreateToneGenerator(
        /* [in] */ Int32 streamType);

    /**
     * Switch between icons because Bluetooth music is same as music volume, but with
     * different icons.
     */
    void SetMusicIcon(
        /* [in] */ Int32 resId,
        /* [in] */ Int32 resMuteId);

    void ResetTimeout();

    void ForceTimeout();

protected:
    friend class WarningDialogReceiver;
    friend class ViewOnTouchListener;
    friend class VolumePanelDialog;
    friend class PositiveButtonOnClickListener;

    AutoPtr<IContext> mContext;
    IAudioService* mAudioService;

private:
    static const String TAG;
    static const Boolean LOGD;

    static const Int32 VIBRATE_DURATION;// = 300;
    static const Int32 BEEP_DURATION;// = 150;
    static const Int32 MAX_VOLUME;// = 100;
    static const Int32 FREE_DELAY;// = 10000;
    static const Int32 TIMEOUT_DELAY;// = 3000;

    static const Int32 MSG_VOLUME_CHANGED;// = 0;
    static const Int32 MSG_FREE_RESOURCES;// = 1;
    static const Int32 MSG_PLAY_SOUND;// = 2;
    static const Int32 MSG_STOP_SOUNDS;// = 3;
    static const Int32 MSG_VIBRATE;// = 4;
    static const Int32 MSG_TIMEOUT;// = 5;
    static const Int32 MSG_RINGER_MODE_CHANGED;// = 6;
    static const Int32 MSG_MUTE_CHANGED;// = 7;
    static const Int32 MSG_REMOTE_VOLUME_CHANGED;// = 8;
    static const Int32 MSG_REMOTE_VOLUME_UPDATE_IF_SHOWN;// = 9;
    static const Int32 MSG_SLIDER_VISIBILITY_CHANGED;// = 10;
    static const Int32 MSG_DISPLAY_SAFE_VOLUME_WARNING;// = 11;

    // Pseudo stream type for master volume
    static const Int32 STREAM_MASTER;// = -100;
    // Pseudo stream type for remote volume is defined in AudioService.STREAM_REMOTE_MUSIC

    AutoPtr<IAudioManager> mAudioManager;

    Boolean mRingIsSilent;
    Boolean mShowCombinedVolumes;
    Boolean mVoiceCapable;

   // True if we want to play tones on the system stream when the master stream is specified.
    Boolean mPlayMasterStreamTones;

    /** Dialog containing all the sliders */
    AutoPtr<IDialog> mDialog;
    /** Dialog's content view */
    AutoPtr<IView> mView;

    /** The visible portion of the volume overlay */
    AutoPtr<IViewGroup> mPanel;
    /** Contains the sliders and their touchable icons */
    AutoPtr<IViewGroup> mSliderGroup;
    /** The button that expands the dialog to show all sliders */
    AutoPtr<IView> mMoreButton;
    /** Dummy divider icon that needs to vanish with the more button */
    AutoPtr<IView> mDivider;

    /** Currently active stream that shows up at the top of the list of sliders */
    Int32 mActiveStreamType;// = -1;
    /** All the slider controls mapped by stream type */
    HashMap<Int32, AutoPtr<StreamControl> > mStreamControls;

    static StreamResources BluetoothSCOStream;
    static StreamResources RingerStream;
    static StreamResources VoiceStream;
    static StreamResources AlarmStream;
    static StreamResources MediaStream;
    static StreamResources NotificationStream;
    // for now, use media resources for master volume
    static StreamResources MasterStream;
    static StreamResources RemoteStream;

    // List of stream types and their order
    static const Int32 STREAMS_LENGTH = 8;
    static StreamResources* STREAMS[8];

    // Synchronize when accessing this
    AutoPtr<ArrayOf<IToneGenerator*> > mToneGenerators;
    AutoPtr<IVibrator> mVibrator;

    static AutoPtr<IAlertDialog> sConfirmSafeVolumeDialog;
    static Object sConfirmSafeVolumeLock;// = new Object();

};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CVOLUMEPANEL_H__
