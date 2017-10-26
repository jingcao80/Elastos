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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDTRANSPORTCONTROLVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDTRANSPORTCONTROLVIEW_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardTransportControlView.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitorCallback.h"
#include "elastos/droid/view/View.h"
#include <elastos/droid/widget/FrameLayout.h>
#include <elastos/core/Runnable.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Transition.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <Elastos.CoreLibrary.Text.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Media::IMetadata;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IRemoteController;
using Elastos::Droid::Media::IRemoteControllerMetadataEditor;
using Elastos::Droid::Media::IRemoteControllerOnClientUpdateListener;
using Elastos::Core::Runnable;
using Elastos::Droid::Transition::ITransitionSet;
using Elastos::Droid::View::View;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
using Elastos::Text::IDateFormat;
using Elastos::Utility::IDate;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;

/**
 * This is the widget responsible for showing music controls in keyguard.
 */
namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardTransportControlView)
    , public Elastos::Droid::Widget::FrameLayout
    , public IKeyguardTransportControlView
{
private:
    class MyRemoteControllerOnClientUpdateListener
        : public Object
        , public IRemoteControllerOnClientUpdateListener
    {
    public:
        TO_STRING_IMPL("CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener")

        CAR_INTERFACE_DECL()

        MyRemoteControllerOnClientUpdateListener(
            /* [in] */ CKeyguardTransportControlView* host)
            : mHost(host)
        {}

        CARAPI OnClientChange(
            /* [in] */ Boolean clearing);

        //@Override
        CARAPI OnClientPlaybackStateUpdate(
            /* [in] */ Int32 state);

        //@Override
        CARAPI OnClientPlaybackStateUpdate(
            /* [in] */ Int32 state,
            /* [in] */ Int64 stateChangeTimeMs,
            /* [in] */ Int64 currentPosMs,
            /* [in] */ Float speed);

        //@Override
        CARAPI OnClientTransportControlUpdate(
            /* [in] */ Int32 transportControlFlags);

        //@Override
        CARAPI OnClientMetadataUpdate(
            /* [in] */ IRemoteControllerMetadataEditor* metadataEditor);

        /**
         * @hide
         */
        CARAPI OnClientFolderInfoBrowsedPlayer(
            /* [in] */ const String& stringUri);

        /**
         * @hide
         */
        CARAPI OnClientUpdateNowPlayingEntries(
            /* [in] */ ArrayOf<Int64>* playList);

        /**
         * @hide
         */
        CARAPI OnClientNowPlayingContentChange();

        /**
         * @hide
         */
        CARAPI OnClientPlayItemResponse(
            /* [in] */ Boolean success);

    private:
        CKeyguardTransportControlView* mHost;
    };

    class UpdateSeekBarRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardTransportControlView::UpdateSeekBarRunnable")

        UpdateSeekBarRunnable(
            /* [in] */ CKeyguardTransportControlView* host)
            : mHost(host)
        {}

        CARAPI Run();

        CARAPI UpdateOnce(
            /* [out] */ Boolean* result);

    private:
        CKeyguardTransportControlView* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardTransportControlView::MyRunnable")

        MyRunnable(
            /* [in] */ CKeyguardTransportControlView* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CKeyguardTransportControlView* mHost;
    };

    class MyViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CKeyguardTransportControlView::MyViewOnClickListener")

        CAR_INTERFACE_DECL()

        MyViewOnClickListener(
            /* [in] */ CKeyguardTransportControlView* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CKeyguardTransportControlView* mHost;
    };

    class MyViewOnLongClickListener
        : public Object
        , public IViewOnLongClickListener
    {
    public:
        TO_STRING_IMPL("CKeyguardTransportControlView::MyViewOnLongClickListener")

        CAR_INTERFACE_DECL()

        MyViewOnLongClickListener(
            /* [in] */ CKeyguardTransportControlView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

    private:
        CKeyguardTransportControlView* mHost;
    };

    // This class is here to throttle scrub position updates to the music client
    class FutureSeekRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardTransportControlView::FutureSeekRunnable")

        FutureSeekRunnable(
            /* [in] */ CKeyguardTransportControlView* host)
            : mHost(host)
        {}

        CARAPI Run();

        CARAPI SetProgress(
            /* [in] */  Int32 progress);

    private:
        CKeyguardTransportControlView* mHost;
        Int32 mProgress;
        Boolean mPending;
    };

    class MySeekBarOnSeekBarChangeListener
        : public Object
        , public ISeekBarOnSeekBarChangeListener
    {
    public:
        TO_STRING_IMPL("CKeyguardTransportControlView::MySeekBarOnSeekBarChangeListener")

        CAR_INTERFACE_DECL()

        MySeekBarOnSeekBarChangeListener(
            /* [in] */ CKeyguardTransportControlView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnProgressChanged(
            /* [in] */ ISeekBar* seekBar,
            /* [in] */ Int32 progress,
            /* [in] */ Boolean fromUser);

        //@Override
        CARAPI OnStartTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

        //@Override
        CARAPI OnStopTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

    private:
        CKeyguardTransportControlView* mHost;
    };

    class MyKeyguardUpdateMonitorCallback
        : public KeyguardUpdateMonitorCallback
    {
    public:
        TO_STRING_IMPL("CKeyguardTransportControlView::MyKeyguardUpdateMonitorCallback")

        MyKeyguardUpdateMonitorCallback(
            /* [in] */ CKeyguardTransportControlView* host)
            : mHost(host)
        {}

        CARAPI OnScreenTurnedOff(
            /* [in] */ Int32 why);

        CARAPI OnScreenTurnedOn();

    private:
        CKeyguardTransportControlView* mHost;
    };

    class Metadata
        : public Object
    {
    public:
        TO_STRING_IMPL("CKeyguardTransportControlView::Metadata")

        CARAPI Clear();

        // CARAPI ToString(
        //     /* [out] */ String* str);

    private:
        friend class CKeyguardTransportControlView;
        String mArtist;
        String mTrackTitle;
        String mAlbumTitle;
        AutoPtr<IBitmap> mBitmap;
        Int64 mDuration;
    };

    class SavedState
        : public View::BaseSavedState
        , public IKeyguardTransportControlViewSavedState
    {
    public:
        TO_STRING_IMPL("CKeyguardTransportControlView::SavedState")

        CAR_INTERFACE_DECL()

        SavedState()
            : mClientPresent(FALSE)
            , mArtist(NULL)
            , mTrackTitle(NULL)
            , mAlbumTitle(NULL)
            , mDuration(0)
        {}

        CARAPI constructor(
            /* [in] */ IParcelable* superState);

        //@Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* out);

    public:
        // static AutoPtr<IParcelableCreator> <SavedState> CREATOR
        //         = new Parcelable.Creator<SavedState>() {
        //     public SavedState createFromParcel(Parcel in) {
        //         return new SavedState(in);
        //     }

        //     public SavedState[] newArray(int size) {
        //         return new SavedState[size];
        //     }
        // };

    private:
        CARAPI constructor(
            /* [in] */ IParcel* in);

    private:
        friend class CKeyguardTransportControlView;
        Boolean mClientPresent;
        String mArtist;
        String mTrackTitle;
        String mAlbumTitle;
        Int64 mDuration;
        AutoPtr<IBitmap> mBitmap;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyguardTransportControlView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetSeekBarsEnabled(
        /* [in] */ Boolean enabled);

    CARAPI SetTransportControlCallback(
        /* [in] */ IKeyguardHostViewTransportControlCallback* transportControlCallback);

    //@Override
    CARAPI OnFinishInflate();

    //@Override
    CARAPI OnAttachedToWindow();

    //@Override
    CARAPI OnDetachedFromWindow();

    CARAPI SetBadgeIcon(
        /* [in] */ IDrawable* bmp);

    CARAPI ClearMetadata();

    CARAPI UpdateMetadata(
        /* [in] */ IRemoteControllerMetadataEditor* data);

    CARAPI UpdateSeekDisplay();

    CARAPI TryToggleSeekBar(
        /* [out] */ Boolean* result);

    CARAPI ResetToMetadata();

    CARAPI DelayResetToMetadata();

    CARAPI CancelResetToMetadata();

    CARAPI SetSeekBarDuration(
        /* [in] */ Int64 duration);

    CARAPI ScrubTo(
        /* [in] */ Int32 progress);

    CARAPI UpdateSeekBars(
        /* [out] */ Boolean* result);

    CARAPI ProvidesClock(
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    //@Override
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

private:
    // This is here because RemoteControlClient's method isn't visible :/
    static CARAPI_(Boolean) PlaybackPositionShouldMove(
        /* [in] */ Int32 playstate);

    CARAPI_(void) UpdateTransportControls(
        /* [in] */ Int32 transportControlFlags);

    CARAPI_(void) SetEnableMarquee(
        /* [in] */ Boolean enabled);

    /**
     * Populates the given metadata into the view
     */
    CARAPI_(void) PopulateMetadata();

    static CARAPI_(void) SetVisibilityBasedOnFlag(
        /* [in] */ IView* view,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 flag);

    CARAPI_(void) UpdatePlayPauseState(
        /* [in] */ Int32 state);

    CARAPI_(void) SendMediaButtonClick(
        /* [in] */ Int32 keyCode);

protected:
    static const Boolean DEBUG;
    static const String TAG;

    static const Int64 QUIESCENT_PLAYBACK_FACTOR;

private:
    static const Int32 RESET_TO_METADATA_DELAY;

    static const Boolean ANIMATE_TRANSITIONS;

    AutoPtr<IViewGroup> mMetadataContainer;
    AutoPtr<IViewGroup> mInfoContainer;
    AutoPtr<ITextView> mTrackTitle;
    AutoPtr<ITextView> mTrackArtistAlbum;

    AutoPtr<IView> mTransientSeek;
    AutoPtr<ISeekBar> mTransientSeekBar;
    AutoPtr<ITextView> mTransientSeekTimeElapsed;
    AutoPtr<ITextView> mTransientSeekTimeTotal;

    AutoPtr<IImageView> mBtnPrev;
    AutoPtr<IImageView> mBtnPlay;
    AutoPtr<IImageView> mBtnNext;
    AutoPtr<Metadata> mMetadata;
    Int32 mTransportControlFlags;
    Int32 mCurrentPlayState;
    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<IRemoteController> mRemoteController;

    AutoPtr<IImageView> mBadge;

    Boolean mSeekEnabled;
    AutoPtr<IDateFormat> mFormat;

    AutoPtr<IDate> mTempDate;

    /**
     * The metadata which should be populated into the view once we've been attached
     */
    AutoPtr<IRemoteControllerMetadataEditor> mPopulateMetadataWhenAttached;

    AutoPtr<IRemoteControllerOnClientUpdateListener> mRCClientUpdateListener;

    AutoPtr<IRunnable> mUpdateSeekBars;

    AutoPtr<IRunnable> mResetToMetadata;

    AutoPtr<IViewOnClickListener> mTransportCommandListener;

    AutoPtr<IViewOnLongClickListener> mTransportShowSeekBarListener;

    AutoPtr<IRunnable> mFutureSeekRunnable;

    AutoPtr<ISeekBarOnSeekBarChangeListener> mOnSeekBarChangeListener;

    static const Int32 TRANSITION_DURATION;
    AutoPtr<ITransitionSet> mMetadataChangeTransition;

    AutoPtr<IKeyguardHostViewTransportControlCallback> mTransportControlCallback;

    AutoPtr<KeyguardUpdateMonitorCallback> mUpdateMonitor;

};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDTRANSPORTCONTROLVIEW_H__