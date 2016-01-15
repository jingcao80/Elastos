
#ifndef __ELASTOS_DROID_WIDGET_INTERNAL_TRANSPORTCONTROLVIEW_H__
#define __ELASTOS_DROID_WIDGET_INTERNAL_TRANSPORTCONTROLVIEW_H__

#include "elastos/droid/widget/FrameLayout.h"

#include <elastos/core/Math.h>
#include "elastos/droid/media/CAudioManager.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Media::EIID_IIRemoteControlDisplay;
using Elastos::Droid::Media::IIRemoteControlDisplay;
using Elastos::Droid::Media::IRemoteControlClient;
using Elastos::Droid::Media::CAudioManager;
using Elastos::Droid::Media::IAudioManager;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

class TransportControlView : public FrameLayout
{
private:
    class MyHandler
        : public HandlerBase
    {
    public:
        MyHandler(
            /* [in] */ TransportControlView* host)
            : mHost(host)
        {
        }

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        TransportControlView* mHost;
    };

public:

    TransportControlView();

    TransportControlView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    virtual CARAPI OnFinishInflate();

    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

    class Metadata : public ElRefBase
    {
    private:
        String mArtist;
        String mTrackTitle;
        String mAlbumTitle;
        AutoPtr<IBitmap> mBitmap;

    public:
        CARAPI_(String) ToString();
        friend class TransportControlView;
    };

    class TransportControlViewSavedState {

    public:

        TransportControlViewSavedState();

        TransportControlViewSavedState(
            /* [in] */ IParcelable* superState);

        virtual CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        virtual CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

    };
    virtual CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    virtual CARAPI_(void) OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    virtual CARAPI OnClick(
        /* [in] */ IView* v);

    virtual CARAPI SetCallback(
        /* [in] */ ILockScreenWidgetCallback* callback);

    virtual CARAPI_(Boolean) ProvidesClock();

private:
    CARAPI_(void) SendMediaButtonClick(
        /* [in] */ Int32 keyCode);

    CARAPI_(void) UpdateTransportControls(
        /* [in] */ Int32 transportControlFlags);

    CARAPI_(String) GetMdString(
        /* [in] */ IBundle* data,
        /* [in] */ Int32 id);

    CARAPI_(void) UpdateMetadata(
        /* [in] */ IBundle* data);

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


    CARAPI_(Boolean) WasPlayingRecently(
        /* [in] */ Int32 state,
        /* [in] */ Int64 stateChangeTimeMs);


    /**
     * This class is required to have weak linkage to the current TransportControlView
     * because the remote process can hold a strong reference to this binder object and
     * we can't predict when it will be GC'd in the remote process. Without this code, it
     * would allow a heavyweight object to be held on this side of the binder when there's
     * no requirement to run a GC on the other side.
     */
    class IRemoteControlDisplayWeak
        : public IIRemoteControlDisplay
        , public ElRefBase
    {
    private:
        AutoPtr<IWeakReference> mLocalHandler; // WeakReference<Handler> mLocalHandler;
        TransportControlView* mHost;
    public:
        CAR_INTERFACE_DECL()

        IRemoteControlDisplayWeak(
            /* [in] */ IHandler* handler,
            /* [in] */ TransportControlView* host);

        virtual CARAPI SetPlaybackState(
            /* [in] */ Int32 generationId,
            /* [in] */ Int32 state,
            /* [in] */ Int64 stateChangeTimeMs);

        virtual CARAPI SetMetadata(
            /* [in] */ Int32 generationId,
            /* [in] */ IBundle* metadata);

        virtual CARAPI SetTransportControlFlags(
            /* [in] */ Int32 generationId,
            /* [in] */ Int32 flags);

        virtual CARAPI SetArtwork(
            /* [in] */ Int32 generationId,
            /* [in] */ IBitmap* bitmap);

        virtual CARAPI SetAllMetadata(
            /* [in] */ Int32 generationId,
            /* [in] */ IBundle* metadata,
            /* [in] */ IBitmap* bitmap);

        virtual CARAPI SetCurrentClientId(
            /* [in] */ Int32 clientGeneration,
            /* [in] */ IPendingIntent* mediaIntent,
            /* [in] */ Boolean clearing);
    };

protected:
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    static const Boolean DEBUG = FALSE;
    static const String TAG;

private:
    // Functions For Handler
    CARAPI HandleUpdateState(
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2);

    CARAPI HandleSetMetaData(
        /* [in] */ Int32 arg1,
        /* [in] */ IBundle* obj);

    CARAPI HandleSetTransportControls(
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2);

    CARAPI HandleSetArtwork(
        /* [in] */ Int32 arg1,
        /* [in] */ IBitmap* obj);

    CARAPI HandleSetGenerationId(
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IPendingIntent* obj);

    CARAPI InitFromResource(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

private:
    static const Int32 MSG_UPDATE_STATE = 100;
    static const Int32 MSG_SET_METADATA = 101;
    static const Int32 MSG_SET_TRANSPORT_CONTROLS = 102;
    static const Int32 MSG_SET_ARTWORK = 103;
    static const Int32 MSG_SET_GENERATION_ID = 104;
    static const Int32 MAXDIM = 512;
    static const Int32 DISPLAY_TIMEOUT_MS = 5000; // 5s


    AutoPtr<IImageView> mAlbumArt;
    AutoPtr<ITextView> mTrackTitle;
    AutoPtr<IImageView> mBtnPrev;
    AutoPtr<IImageView> mBtnPlay;
    AutoPtr<IImageView> mBtnNext;
    Int32 mClientGeneration;
    AutoPtr<Metadata> mMetadata;
    Boolean mAttached;
    AutoPtr<IPendingIntent> mClientIntent;
    Int32 mTransportControlFlags;
    Int32 mCurrentPlayState;
    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<ILockScreenWidgetCallback> mWidgetCallbacks;
    AutoPtr<IRemoteControlDisplayWeak> mIRCD;

    /**
     * The metadata which should be populated into the view once we've been attached
     */
    AutoPtr<IBundle> mPopulateMetadataWhenAttached;// = NULL;

    // This handler is required to ensure messages from IRCD are handled in sequence and on
    // the UI thread.
    AutoPtr<IHandler> mHandler;
};


}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif
