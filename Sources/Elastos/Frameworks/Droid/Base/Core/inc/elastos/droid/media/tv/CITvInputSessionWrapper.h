#ifndef __ELASTOS_DROID_MEDIA_TV_CITVINPUTSESSIONWRAPPER_H__
#define __ELASTOS_DROID_MEDIA_TV_CITVINPUTSESSIONWRAPPER_H__

#include "_Elastos_Droid_Media_Tv_CITvInputSessionWrapper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "elastos/droid/view/InputEventReceiver.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::Os::IHandlerCaller;
using Elastos::Droid::Internal::Os::IHandlerCallerCallback;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::InputEventReceiver;
using Elastos::Droid::View::ISurface;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
* Implements the internal ITvInputSession interface to convert incoming calls on to it back to
* calls on the public TvInputSession interface, scheduling them on the main thread of the process.
*
* @hide
*/
CarClass(CITvInputSessionWrapper)
    , public Object
    , public IITvInputSession
    , public IHandlerCallerCallback
{
private:
    class TvInputEventReceiver
        : public InputEventReceiver
    {
    public:
        TvInputEventReceiver(
            /* [in] */ IInputChannel* inputChannel,
            /* [in] */ ILooper* looper,
            /* [in] */ ITvInputServiceSession* impl);

        CARAPI OnInputEvent(
            /* [in] */ IInputEvent* event);

    private:
        AutoPtr<ITvInputServiceSession> mImpl;
    };

public:
    CITvInputSessionWrapper();

    virtual ~CITvInputSessionWrapper();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ITvInputServiceSession* sessionImpl,
        /* [in] */ IInputChannel* channel);

    CARAPI ExecuteMessage(
        /* [in] */ IMessage* msg);

    CARAPI ReleaseResources();

    CARAPI SetMain(
        /* [in] */ Boolean isMain);

    CARAPI SetSurface(
        /* [in] */ ISurface* surface);

    CARAPI DispatchSurfaceChanged(
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    // TODO: Remove this once it becomes irrelevant for applications to handle audio focus. The plan
    // is to introduce some new concepts that will solve a number of problems in audio policy today.
    CARAPI SetVolume(
        /* [in] */ Float volume);

    CARAPI Tune(
        /* [in] */ IUri* channelUri,
        /* [in] */ IBundle* params);

    CARAPI SetCaptionEnabled(
        /* [in] */ Boolean enabled);

    CARAPI SelectTrack(
        /* [in] */ Int32 type,
        /* [in] */ const String& trackId);

    CARAPI AppPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data);

    CARAPI CreateOverlayView(
        /* [in] */ IBinder* windowToken,
        /* [in] */ IRect* frame);

    CARAPI RelayoutOverlayView(
        /* [in] */ IRect* frame);

    CARAPI RemoveOverlayView();

    CARAPI RequestUnblockContent(
        /* [in] */ const String& unblockedRating);

private:
    static String TAG;

    static Int32 MESSAGE_HANDLING_DURATION_THRESHOLD_MILLIS;

    static const Int32 DO_RELEASE;
    static const Int32 DO_SET_MAIN;
    static const Int32 DO_SET_SURFACE;
    static const Int32 DO_DISPATCH_SURFACE_CHANGED;
    static const Int32 DO_SET_STREAM_VOLUME;
    static const Int32 DO_TUNE;
    static const Int32 DO_SET_CAPTION_ENABLED;
    static const Int32 DO_SELECT_TRACK;
    static const Int32 DO_APP_PRIVATE_COMMAND;
    static const Int32 DO_CREATE_OVERLAY_VIEW;
    static const Int32 DO_RELAYOUT_OVERLAY_VIEW;
    static const Int32 DO_REMOVE_OVERLAY_VIEW;
    static const Int32 DO_REQUEST_UNBLOCK_CONTENT;

    AutoPtr<IHandlerCaller> mCaller;

    AutoPtr<ITvInputServiceSession> mTvInputSessionImpl;
    AutoPtr<IInputChannel> mChannel;
    AutoPtr<TvInputEventReceiver> mReceiver;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CITVINPUTSESSIONWRAPPER_H__
