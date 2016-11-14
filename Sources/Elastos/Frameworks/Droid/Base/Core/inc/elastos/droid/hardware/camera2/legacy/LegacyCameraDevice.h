
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYCAMERADEVICE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYCAMERADEVICE_H__

#include "Elastos.Droid.Hardware.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/hardware/camera2/legacy/RequestThreadManager.h"
#include "elastos/droid/hardware/camera2/legacy/RequestHolder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Camera2::Impl::ICaptureResultExtras;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraDeviceImplCameraDeviceCallbacks;
using Elastos::Droid::Hardware::Camera2::Utils::ILongParcelable;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IConditionVariable;
using Elastos::Droid::Utility::ISize;
using Elastos::Droid::View::ISurface;
using Elastos::Core::Object;
using Elastos::IO::ICloseable;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class LegacyCameraDevice
    : public Object
    , public ILegacyCameraDevice
    , public ICloseable
{
private:
    class MyRunnableOnError
        : public Runnable
    {
    public:
        MyRunnableOnError(
            /* [in] */ LegacyCameraDevice* owner,
            /* [in] */ Int32 id,
            /* [in] */ Int32 errorCode,
            /* [in] */ ICaptureResultExtras* extras);

        CARAPI Run();

    private:
        LegacyCameraDevice* mOwner;
        Int32 mId;
        Int32 mErrorCode;
        AutoPtr<ICaptureResultExtras> mExtras;
    };

    class MyRunnableOnIdle
        : public Runnable
    {
    public:
        MyRunnableOnIdle(
            /* [in] */ LegacyCameraDevice* owner);

        CARAPI Run();

    private:
        LegacyCameraDevice* mOwner;
    };

    class MyRunnableOnCaptureStarted
        : public Runnable
    {
    public:
        MyRunnableOnCaptureStarted(
            /* [in] */ LegacyCameraDevice* owner,
            /* [in] */ Int32 id,
            /* [in] */ Int64 timestamp,
            /* [in] */ ICaptureResultExtras* extras);

        CARAPI Run();

    private:
        LegacyCameraDevice* mOwner;
        Int32 mId;
        Int64 mTimestamp;
        AutoPtr<ICaptureResultExtras> mExtras;
    };

    class MyRunnableonCaptureResult
        : public Runnable
    {
    public:
        MyRunnableonCaptureResult(
            /* [in] */ LegacyCameraDevice* owner,
            /* [in] */ Int32 id,
            /* [in] */ ICameraMetadataNative* result,
            /* [in] */ ICaptureResultExtras* extras);

        CARAPI Run();

    private:
        LegacyCameraDevice* mOwner;
        Int32 mId;
        AutoPtr<ICameraMetadataNative> mResult;
        AutoPtr<ICaptureResultExtras> mExtras;
    };

    class MyListener
        : public Object
        , public ICameraDeviceStateListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ LegacyCameraDevice* host);

        CARAPI OnError(
            /* [in] */ Int32 errorCode,
            /* [in] */ IRequestHolder* holder);
        CARAPI OnConfiguring();

        CARAPI OnIdle();

        CARAPI OnBusy();

        CARAPI OnCaptureStarted(
            /* [in] */ IRequestHolder* holder,
            /* [in] */ Int64 timestamp);

        CARAPI OnCaptureResult(
            /* [in] */ ICameraMetadataNative* result,
            /* [in] */ IRequestHolder* holder);

    private:
        LegacyCameraDevice* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    LegacyCameraDevice();

    virtual ~LegacyCameraDevice();

    CARAPI constructor();

    /**
     * Create a new emulated camera device from a given Camera 1 API camera.
     *
     * <p>
     * The {@link Camera} provided to this constructor must already have been successfully opened,
     * and ownership of the provided camera is passed to this object.  No further calls to the
     * camera methods should be made following this constructor.
     * </p>
     *
     * @param cameraId the id of the camera.
     * @param camera an open {@link Camera} device.
     * @param characteristics the static camera characteristics for this camera device
     * @param callbacks {@link ICameraDeviceCallbacks} callbacks to call for Camera2 API operations.
     */
    CARAPI constructor(
        /* [in] */ Int32 cameraId,
        /* [in] */ IHardwareCamera* camera,
        /* [in] */ ICameraCharacteristics* characteristics,
        /* [in] */ IICameraDeviceCallbacks* callbacks);

    /**
     * Configure the device with a set of output surfaces.
     *
     * <p>Using empty or {@code null} {@code outputs} is the same as unconfiguring.</p>
     *
     * <p>Every surface in {@code outputs} must be non-{@code null}.</p>
     *
     * @param outputs a list of surfaces to set.
     * @return an error code for this binder operation, or {@link NO_ERROR}
     *          on success.
     */
    CARAPI ConfigureOutputs(
        /* [in] */ IList* outputs,
        /* [out] */ Int32* value);

    /**
     * Submit a burst of capture requests.
     *
     * @param requestList a list of capture requests to execute.
     * @param repeating {@code true} if this burst is repeating.
     * @param frameNumber an output argument that contains either the frame number of the last frame
     *                    that will be returned for this request, or the frame number of the last
     *                    frame that will be returned for the current repeating request if this
     *                    burst is set to be repeating.
     * @return the request id.
     */
    CARAPI SubmitRequestList(
        /* [in] */ IList* requestList,
        /* [in] */ Boolean repeating,
        /* [in] */ ILongParcelable* frameNumber,
        /* [out] */ Int32* value);

    /**
     * Submit a single capture request.
     *
     * @param request the capture request to execute.
     * @param repeating {@code true} if this request is repeating.
     * @param frameNumber an output argument that contains either the frame number of the last frame
     *                    that will be returned for this request, or the frame number of the last
     *                    frame that will be returned for the current repeating request if this
     *                    request is set to be repeating.
     * @return the request id.
     */
    CARAPI SubmitRequest(
        /* [in] */ ICaptureRequest* request,
        /* [in] */ Boolean repeating,
        /* [in] */ ILongParcelable* frameNumber,
        /* [out] */ Int32* value);

    /**
     * Cancel the repeating request with the given request id.
     *
     * @param requestId the request id of the request to cancel.
     * @return the last frame number to be returned from the HAL for the given repeating request, or
     *          {@code INVALID_FRAME} if none exists.
     */
    CARAPI CancelRequest(
        /* [in] */ Int32 requestId,
        /* [out] */ Int64* value);

    /**
     * Block until the {@link ICameraDeviceCallbacks#onCameraIdle()} callback is received.
     */
    CARAPI WaitUntilIdle();

    /**
     * Flush any pending requests.
     *
     * @return the last frame number.
     */
    CARAPI Flush(
        /* [out] */ Int64* value);

    /**
     * Return {@code true} if the device has been closed.
     */
    CARAPI IsClosed(
        /* [out] */ Boolean* value);

    //@Override
    CARAPI Close();

protected:
    friend class RequestHolderBuilder;
    friend class SurfaceTextureRenderer;
    friend class RequestThreadManager;
    friend class RequestHolder::Builder;
    friend class RequestThreadManager::CameraPictureCallback;

    /**
     * Check if a given surface uses {@link ImageFormat#YUV_420_888} or format that can be readily
     * converted to this; YV12 and NV21 are the two currently supported formats.
     *
     * @param s the surface to check.
     * @return {@code true} if the surfaces uses {@link ImageFormat#YUV_420_888} or a compatible
     *          format.
     */
    static CARAPI_(Boolean) NeedsConversion(
        /* [in] */ ISurface* s);

    /**
     * Query the surface for its currently configured default buffer size.
     * @param surface a non-{@code null} {@code Surface}
     * @return the width and height of the surface
     *
     * @throws NullPointerException if the {@code surface} was {@code null}
     * @throws IllegalStateException if the {@code surface} was invalid
     */
    static CARAPI GetSurfaceSize(
        /* [in] */ ISurface* surface,
        /* [out] */ ISize** size);

    static CARAPI DetectSurfaceType(
        /* [in] */ ISurface* surface,
        /* [out] */ Int32* type);

    static CARAPI ConfigureSurface(
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 pixelFormat);

    static CARAPI ProduceFrame(
        /* [in] */ ISurface* surface,
        /* [in] */ ArrayOf<Byte>* pixelBuffer,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 pixelFormat);

    static CARAPI SetSurfaceFormat(
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 pixelFormat);

    static CARAPI SetSurfaceDimens(
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    static CARAPI_(Int64) GetSurfaceId(
        /* [in] */ ISurface* surface);

    static CARAPI GetSurfaceIds(
        /* [in] */ ICollection* surfaces,
        /* [out] */ IList** ids);

    static CARAPI_(Boolean) ContainsSurfaceId(
        /* [in] */ ISurface* s,
        /* [in] */ ICollection* ids);

    static CARAPI SetSurfaceOrientation(
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 facing,
        /* [in] */ Int32 sensorOrientation);

    static CARAPI GetTextureSize(
        /* [in] */ ISurfaceTexture* surfaceTexture,
        /* [out] */ ISize** size);

    static CARAPI SetNextTimestamp(
        /* [in] */ ISurface* surface,
        /* [in] */ Int64 timestamp);

    static CARAPI_(Int32) NativeGetJpegFooterSize();

private:
    CARAPI_(AutoPtr<ICaptureResultExtras>) GetExtrasFromRequest(
        /* [in] */ IRequestHolder* holder);

    static CARAPI_(Int32) NativeDetectSurfaceType(
        /* [in] */ ISurface* surface);

    static CARAPI_(Int32) NativeDetectSurfaceDimens(
        /* [in] */ ISurface* surface,
        /* [in] */ ArrayOf<Int32>* dimens);

    static CARAPI_(Int32) NativeConfigureSurface(
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 pixelFormat);

    static CARAPI NativeProduceFrame(
        /* [in] */ ISurface* surface,
        /* [in] */ ArrayOf<Byte>* pixelBuffer,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 pixelFormat,
        /* [out] */ Int32* result);

    static CARAPI_(Int32) NativeSetSurfaceFormat(
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 pixelFormat);

    static CARAPI_(Int32) NativeSetSurfaceDimens(
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    static CARAPI_(Int64) NativeGetSurfaceId(
        /* [in] */ ISurface* surface);

    static CARAPI_(Int32) NativeSetSurfaceOrientation(
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 facing,
        /* [in] */ Int32 sensorOrientation);

    static CARAPI_(Int32) NativeDetectTextureDimens(
        /* [in] */ ISurfaceTexture* surfaceTexture,
        /* [in] */ ArrayOf<Int32>* dimens);

    static CARAPI_(Int32) NativeSetNextTimestamp(
        /* [in] */ ISurface* surface,
        /* [in] */ Int64 timestamp);

private:
    static const String DEBUG_PROP;
    String TAG;

    static const Boolean DEBUG;
    Int32 mCameraId;
    AutoPtr<ICameraCharacteristics> mStaticCharacteristics;
    AutoPtr<IICameraDeviceCallbacks> mDeviceCallbacks;
    AutoPtr<ICameraDeviceState> mDeviceState;
    AutoPtr<IList> mConfiguredSurfaces;
    Boolean mClosed;

    AutoPtr<IConditionVariable> mIdle;

    AutoPtr<IHandlerThread> mResultThread;
    AutoPtr<IHandlerThread> mCallbackHandlerThread;
    AutoPtr<IHandler> mCallbackHandler;
    AutoPtr<IHandler> mResultHandler;
    static const Int32 ILLEGAL_VALUE;

    AutoPtr<IRequestThreadManager> mRequestThreadManager;

    AutoPtr<MyListener> mStateListener;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYCAMERADEVICE_H__
