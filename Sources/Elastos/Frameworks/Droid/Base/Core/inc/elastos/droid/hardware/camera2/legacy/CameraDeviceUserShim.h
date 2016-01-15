
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CAMERADEVICEUSERSHIM_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CAMERADEVICEUSERSHIM_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Camera2::IICameraDeviceCallbacks;
using Elastos::Droid::Hardware::Camera2::Impl::ICaptureResultExtras;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Utils::ILongParcelable;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IConditionVariable;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::ISurface;
using Elastos::Core::Object;
using Elastos::Core::IThread;
using Elastos::Core::IRunnable;
using Elastos::IO::ICloseable;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class CameraDeviceUserShim
    : public Object
    , public ICameraDeviceUserShim
    , public IICameraDeviceUser
{
private:
    /**
     * Create a separate looper/thread for the camera to run on; open the camera.
     *
     * <p>Since the camera automatically latches on to the current thread's looper,
     * it's important that we have our own thread with our own looper to guarantee
     * that the camera callbacks get correctly posted to our own thread.</p>
     */
    class CameraLooper
        : public Object
        , public ICameraDeviceUserShimCameraLooper
        , public IRunnable
        , public ICloseable
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Spin up a new thread, immediately open the camera in the background.
         *
         * <p>Use {@link #waitForOpen} to block until the camera is finished opening.</p>
         *
         * @param cameraId numeric camera Id
         *
         * @see #waitForOpen
         */
        CameraLooper(
            /* [in] */ Int32 cameraId);

        CARAPI GetCamera(
            /* [out] */ IHardwareCamera** camera);

        //@Override
        CARAPI Run();

        /**
         * Quit the looper safely; then join until the thread shuts down.
         */
        //@Override
        CARAPI Close();

        /**
         * Block until the camera opens; then return its initialization error code (if any).
         *
         * @param timeoutMs timeout in milliseconds
         *
         * @return int error code
         *
         * @throws CameraRuntimeException if the camera open times out with ({@code CAMERA_ERROR})
         */
        CARAPI WaitForOpen(
            /* [in] */ Int32 timeoutMs,
            /* [out] */ Int32* result);

    private:
        Int32 mCameraId;
        AutoPtr<ILooper> mLooper;
        /*volatile*/ Int32 mInitErrors;
        AutoPtr<IHardwareCamera> mCamera;
        AutoPtr<IConditionVariable> mStartDone;
        AutoPtr<IThread> mThread;
    };

    class CallbackHandler;

    /**
     * A thread to process callbacks to send back to the camera client.
     *
     * <p>This effectively emulates one-way binder semantics when in the same process as the
     * callee.</p>
     */
    class CameraCallbackThread
        : public Object
        , public ICameraDeviceUserShimCameraCallbackThread
        , public IICameraDeviceCallbacks
    {
    public:
        CAR_INTERFACE_DECL()

        CameraCallbackThread(
            /* [in] */ IICameraDeviceCallbacks* callbacks);

        CARAPI Close();

        //@Override
        CARAPI OnDeviceError(
            /* [in] */ Int32 errorCode,
            /* [in] */ ICaptureResultExtras* resultExtras);

        //@Override
        CARAPI OnDeviceIdle();

        //@Override
        CARAPI OnCaptureStarted(
            /* [in] */ ICaptureResultExtras* resultExtras,
            /* [in] */ Int64 timestamp);

        //@Override
        CARAPI OnResultReceived(
            /* [in] */ ICameraMetadataNative* result,
            /* [in] */ ICaptureResultExtras* resultExtras);

        //@Override
        CARAPI AsBinder(
            /* [out] */ IBinder** binder);

        friend class CallbackHandler;

    private:
        CARAPI_(AutoPtr<IHandler>) GetHandler();

    private:
        static const Int32 CAMERA_ERROR = 0;
        static const Int32 CAMERA_IDLE = 1;
        static const Int32 CAPTURE_STARTED = 2;
        static const Int32 RESULT_RECEIVED = 3;

        AutoPtr<IHandlerThread> mHandlerThread;
        AutoPtr<IHandler> mHandler;

        AutoPtr<IICameraDeviceCallbacks> mCallbacks;
    };

    class CallbackHandler
        : public Handler
    {
    public:
        CallbackHandler(
            /* [in] */ CameraCallbackThread* host,
            /* [in] */ ILooper* looper);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CameraCallbackThread* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CameraDeviceUserShim();

    virtual ~CameraDeviceUserShim() {}

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 cameraId,
        /* [in] */ ILegacyCameraDevice* legacyCamera,
        /* [in] */ ICameraCharacteristics* characteristics,
        /* [in] */ ICameraDeviceUserShimCameraLooper* cameraInit,
        /* [in] */ ICameraDeviceUserShimCameraCallbackThread* cameraCallbacks);

    static CARAPI ConnectBinderShim(
        /* [in] */ IICameraDeviceCallbacks* callbacks,
        /* [in] */ Int32 cameraId,
        /* [out] */ ICameraDeviceUserShim** shim);

    CARAPI Disconnect();

    CARAPI SubmitRequest(
        /* [in] */ ICaptureRequest* request,
        /* [in] */ Boolean streaming,
        /* [in] */ ILongParcelable* lastFrameNumber,
        /* [out] */ Int32* result);

    CARAPI SubmitRequestList(
        /* [in] */ IList* requestList,
        /* [in] */ Boolean streaming,
        /* [in] */ ILongParcelable* lastFrameNumber,
        /* [out] */ Int32* result);

    CARAPI CancelRequest(
        /* [in] */ Int32 requestId,
        /* [in] */ ILongParcelable* lastFrameNumber,
        /* [out] */ Int32* result);

    CARAPI BeginConfigure(
       /* [out] */ Int32* result);

    CARAPI EndConfigure(
        /* [out] */ Int32* result);

    CARAPI DeleteStream(
        /* [in] */ Int32 streamId,
        /* [out] */ Int32* result);

    CARAPI CreateStream(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ ISurface* surface,
        /* [out] */ Int32* result);

    CARAPI CreateDefaultRequest(
        /* [in] */ Int32 templateId,
        /* [in] */ ICameraMetadataNative* request,
        /* [out] */ Int32* result);

    CARAPI GetCameraInfo(
        /* [in] */ ICameraMetadataNative* info,
        /* [out] */ Int32* result);

    CARAPI WaitUntilIdle(
        /* [out] */ Int32* result);

    CARAPI Flush(
        /* [in] */ ILongParcelable* lastFrameNumber,
        /* [out] */ Int32* result);

    CARAPI AsBinder(
        /* [out] */ IBinder** binder);

private:
    static CARAPI_(Int32) TranslateErrorsFromCamera1(
        /* [in] */ Int32 errorCode);

private:
    static const String TAG;

    static const Boolean DEBUG;
    static const Int32 OPEN_CAMERA_TIMEOUT_MS;

    AutoPtr<ILegacyCameraDevice> mLegacyDevice;

    Object mConfigureLock;
    Int32 mSurfaceIdCounter;
    Boolean mConfiguring;
    AutoPtr<ISparseArray> mSurfaces;
    AutoPtr<ICameraCharacteristics> mCameraCharacteristics;
    AutoPtr<ICameraDeviceUserShimCameraLooper> mCameraInit;
    AutoPtr<ICameraDeviceUserShimCameraCallbackThread> mCameraCallbacks;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CAMERADEVICEUSERSHIM_H__
