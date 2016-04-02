#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_VIDEOCALLIMPL_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_VIDEOCALLIMPL_H__

#include "Elastos.Droid.Telecomm.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Telecomm::Internal::IIVideoProvider;
using Elastos::Droid::View::ISurface;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * Implementation of a Video Call, which allows InCallUi to communicate commands to the underlying
 * {@link Connection.VideoProvider}, and direct callbacks from the
 * {@link Connection.VideoProvider} to the appropriate {@link VideoCall.Listener}.
 *
 * {@hide}
 */
class VideoCallImpl
    : public Object
    , public IInCallServiceVideoCall
    , public IVideoCallImpl
{
public:
    class ProxyDeathRecipient
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        ProxyDeathRecipient(
            /* [in] */ VideoCallImpl* host);

        CARAPI ProxyDied();

    public:
        VideoCallImpl* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ ILooper* loop,
            /* [in] */ VideoCallImpl* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        VideoCallImpl* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    VideoCallImpl();

    /** {@hide} */
    CARAPI constructor(
        /* [in] */ IIVideoProvider* videoProvider);

    /** {@inheritDoc} */
    CARAPI SetVideoCallListener(
        /* [in] */ IVideoCallListener* videoCallListener);

    /** {@inheritDoc} */
    CARAPI SetCamera(
        /* [in] */ const String& cameraId);

    /** {@inheritDoc} */
    CARAPI SetPreviewSurface(
        /* [in] */ ISurface* surface);

    /** {@inheritDoc} */
    CARAPI SetDisplaySurface(
        /* [in] */ ISurface* surface);

    /** {@inheritDoc} */
    CARAPI SetDeviceOrientation(
        /* [in] */ Int32 rotation);

    /** {@inheritDoc} */
    CARAPI SetZoom(
        /* [in] */ Float value);

    /** {@inheritDoc} */
    CARAPI SendSessionModifyRequest(
        /* [in] */ IVideoProfile* requestProfile);

    /** {@inheritDoc} */
    CARAPI SendSessionModifyResponse(
        /* [in] */ IVideoProfile* responseProfile);

    /** {@inheritDoc} */
    CARAPI RequestCameraCapabilities();

    /** {@inheritDoc} */
    CARAPI RequestCallDataUsage();

    /** {@inheritDoc} */
    CARAPI SetPauseImage(
        /* [in] */ const String& uri);

public:
    static const Int32 MSG_RECEIVE_SESSION_MODIFY_REQUEST;
    static const Int32 MSG_RECEIVE_SESSION_MODIFY_RESPONSE;
    static const Int32 MSG_HANDLE_CALL_SESSION_EVENT;
    static const Int32 MSG_CHANGE_PEER_DIMENSIONS;
    static const Int32 MSG_CHANGE_CALL_DATA_USAGE;
    static const Int32 MSG_CHANGE_CAMERA_CAPABILITIES;

    AutoPtr<IIVideoProvider> mVideoProvider;
    AutoPtr<IVideoCallListenerBinder> mBinder;
    AutoPtr<IVideoCallListener> mVideoCallListener;

    AutoPtr<IProxyDeathRecipient> mDeathRecipient;

    /** Default handler used to consolidate binder method calls onto a single thread. */
    AutoPtr<IHandler> mHandler;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_VIDEOCALLIMPL_H__