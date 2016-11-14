#ifndef __ELASTOS_DROID_MEDIA_PROJECTION_CMEDIAPROJECTION_H__
#define __ELASTOS_DROID_MEDIA_PROJECTION_CMEDIAPROJECTION_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Media_Projection_CMediaProjection.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/core/Runnable.h>

using Elastos::Droid::Hardware::Display::IVirtualDisplayCallback;
using Elastos::Droid::Hardware::Display::IVirtualDisplay;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioRecord;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Projection {

/**
 * @hide CANDIDATE FOR PUBLIC API
 */
CarClass(CMediaProjection)
    , public Object
    , public IMediaProjection
{
private:
    class CallbackRecordRunnable
        : public Runnable
    {
    public:
        CallbackRecordRunnable(
            /* [in] */ CMediaProjection* host,
            /* [in] */ IMediaProjectionCallback * cb)
            : mHost(host)
            , mCallback(cb)
        {}

        CARAPI Run();

    private:
        CMediaProjection* mHost;
        AutoPtr<IMediaProjectionCallback> mCallback;
    };

    class MediaProjectionCallback
        : public Object
        , public IIMediaProjectionCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MediaProjectionCallback(
            /* [in] */ CMediaProjection * host)
            : mHost(host)
        {}

        CARAPI OnStop();

    private:
        CMediaProjection * mHost;
    };

    class CallbackRecord
        : public Object
    {
    public:
        CallbackRecord(
            /* [in] */ CMediaProjection * host,
            /* [in] */ IMediaProjectionCallback * cb,
            /* [in] */ IHandler * handler)
            : mHost(host)
            , mCallback(cb)
            , mHandler(handler)
        {}

        CARAPI OnStop();

    private:
        CMediaProjection * mHost;
        AutoPtr<IMediaProjectionCallback> mCallback;
        AutoPtr<IHandler> mHandler;
    };

public:
    CMediaProjection();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CMediaProjection();

    /** @hide */
    CARAPI constructor(
        /* [in] */ IContext * context,
        /* [in] */ IIMediaProjection * impl);

    /** Register a listener to receive notifications about when the {@link
     * MediaProjection} changes state.
     *
     * @param callback The callback to call.
     * @param handler The handler on which the callback should be invoked, or
     * null if the callback should be invoked on the calling thread's looper.
     *
     * @see #unregisterCallback
     */
    CARAPI RegisterCallback(
        /* [in] */ IMediaProjectionCallback * cb,
        /* [in] */ IHandler * handler);

    /** Unregister a MediaProjection listener.
     *
     * @param callback The callback to unregister.
     *
     * @see #registerCallback
     */
    CARAPI UnregisterCallback(
        /* [in] */ IMediaProjectionCallback * cb);

    /**
     * @hide
     */
    CARAPI CreateVirtualDisplay(
        /* [in] */ const String& name,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 dpi,
        /* [in] */ Boolean isSecure,
        /* [in] */ ISurface * surface,
        /* [in] */ IVirtualDisplayCallback * cb,
        /* [in] */ IHandler * handler,
        /* [out] */ IVirtualDisplay ** result);

    /**
     * Creates a {@link android.hardware.display.VirtualDisplay} to capture the
     * contents of the screen.
     *
     * @param name The name of the virtual display, must be non-empty.
     * @param width The width of the virtual display in pixels. Must be
     * greater than 0.
     * @param height The height of the virtual display in pixels. Must be
     * greater than 0.
     * @param dpi The density of the virtual display in dpi. Must be greater
     * than 0.
     * @param surface The surface to which the content of the virtual display
     * should be rendered, or null if there is none initially.
     * @param flags A combination of virtual display flags. See {@link DisplayManager} for the full
     * list of flags.
     * @param callback Callback to call when the virtual display's state
     * changes, or null if none.
     * @param handler The {@link android.os.Handler} on which the callback should be
     * invoked, or null if the callback should be invoked on the calling
     * thread's main {@link android.os.Looper}.
     *
     * @see android.hardware.display.DisplayManager#createVirtualDisplay(
     * String, int, int, int, int, Surface, VirtualDisplay.Callback, Handler)
     */
    CARAPI CreateVirtualDisplay(
        /* [in] */ const String& name,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 dpi,
        /* [in] */ Int32 flags,
        /* [in] */ ISurface * surface,
        /* [in] */ IVirtualDisplayCallback * cb,
        /* [in] */ IHandler * handler,
        /* [out] */ IVirtualDisplay ** result);

    /**
     * Creates an AudioRecord to capture audio played back by the system.
     * @hide
     */
    CARAPI CreateAudioRecord(
        /* [in] */ Int32 sampleRateInHz,
        /* [in] */ Int32 channelConfig,
        /* [in] */ Int32 audioFormat,
        /* [in] */ Int32 bufferSizeInBytes,
        /* [out] */ IAudioRecord ** result);

    /**
     * Stops projection.
     */
    CARAPI Stop();

    /**
     * Get the underlying IMediaProjection.
     * @hide
     */
    CARAPI GetProjection(
        /* [out] */ IIMediaProjection ** result);

private:
    static String TAG; // = "MediaProjection";
    AutoPtr<IIMediaProjection> mImpl;
    AutoPtr<IContext> mContext;
    AutoPtr<IArrayMap> mCallbacks;

};

} // namespace Projection
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_PROJECTION_CMEDIAPROJECTION_H__
