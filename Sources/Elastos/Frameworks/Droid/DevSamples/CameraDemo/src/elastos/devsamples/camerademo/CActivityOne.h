
#ifndef __Elastos_DevSamples_CameraDemo_CActivityOne_H__
#define __Elastos_DevSamples_CameraDemo_CActivityOne_H__

#include "_Elastos_DevSamples_CameraDemo_CActivityOne.h"
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <elastos/core/Runnable.h>
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/view/OrientationEventListener.h>
#include "elastos/droid/hardware/camera2/CameraDevice.h"
#include "elastos/droid/hardware/camera2/CameraCaptureSession.h"

using Elastos::Core::Runnable;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::App::Activity;
using Elastos::Droid::Utility::ISize;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::View::ITextureView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::ISurfaceTextureListener;
using Elastos::Droid::Hardware::Camera2::ICameraManager;
using Elastos::Droid::Hardware::Camera2::ICameraDeviceStateCallback;
using Elastos::Droid::Hardware::Camera2::ICameraCaptureSessionCaptureCallback;
using Elastos::Droid::Hardware::Camera2::ICameraDevice;
using Elastos::Droid::Hardware::Camera2::CameraDevice;
using Elastos::Droid::Hardware::Camera2::ICameraCaptureSession;
using Elastos::Droid::Hardware::Camera2::CameraCaptureSession;
using Elastos::Droid::Hardware::Camera2::ICaptureRequest;
using Elastos::Droid::Hardware::Camera2::ICaptureRequestBuilder;
using Elastos::Droid::Hardware::Camera2::ICaptureResult;
using Elastos::Droid::Hardware::Camera2::ITotalCaptureResult;
using Elastos::Droid::Media::IImage;
using Elastos::Droid::Media::IImageReader;
using Elastos::Droid::Media::IImageReaderOnImageAvailableListener;
using Elastos::Utility::Concurrent::ISemaphore;
using Elastos::Core::IComparator;
using Elastos::IO::IFile;

namespace Elastos {
namespace DevSamples {
namespace CameraDemo {

CarClass(CActivityOne)
    , public Activity
{
private:
    class SurfaceTextureListener
        : public Object
        , public ISurfaceTextureListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CActivityOne::SurfaceTextureListener")

        SurfaceTextureListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnSurfaceTextureAvailable(
            /* [in] */ ISurfaceTexture* texture,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI OnSurfaceTextureSizeChanged(
            /* [in] */ ISurfaceTexture* texture,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI OnSurfaceTextureDestroyed(
            /* [in] */ ISurfaceTexture* texture,
            /* [out] */ Boolean* result);

        CARAPI OnSurfaceTextureUpdated(
            /* [in] */ ISurfaceTexture* texture);

    private:
        CActivityOne* mHost;
    };

    class StateCallback
        : public CameraDevice::StateCallback
    {
    public:
        TO_STRING_IMPL("CActivityOne::StateCallback")

        StateCallback(
            /* [in] */ CActivityOne* host);

        CARAPI OnOpened(
            /* [in] */ ICameraDevice* texture);

        CARAPI OnDisconnected(
            /* [in] */ ICameraDevice* texture);

        CARAPI OnError(
            /* [in] */ ICameraDevice* texture,
            /* [in] */ Int32 error);

    private:
        CActivityOne* mHost;
    };

    class CameraCaptureSessionStateCallback
        : public CameraCaptureSession::StateCallback
    {
    public:
        TO_STRING_IMPL("CActivityOne::CameraCaptureSessionStateCallback")

        CameraCaptureSessionStateCallback(
            /* [in] */ CActivityOne* host);

        CARAPI OnConfigured(
            /* [in] */ ICameraCaptureSession* cameraCaptureSession);

        CARAPI OnConfigureFailed(
            /* [in] */ ICameraCaptureSession* cameraCaptureSession);

    private:
        CActivityOne* mHost;
    };

    class ImageAvailableListener
        : public Object
        , public IImageReaderOnImageAvailableListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CActivityOne::ImageAvailableListener")

        ImageAvailableListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnImageAvailable(
            /* [in] */ IImageReader* texture);

    private:
        CActivityOne* mHost;
    };

    class CaptureStillPictureCallback
        : public CameraCaptureSession::CaptureCallback
    {
    public:
        TO_STRING_IMPL("CActivityOne::CaptureStillPictureCallback")

        CaptureStillPictureCallback(
            /* [in] */ CActivityOne* host);

        CARAPI OnCaptureCompleted(
            /* [in] */ ICameraCaptureSession* session,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ITotalCaptureResult* result);

    private:
        CActivityOne* mHost;
    };

    class CaptureCallback
        : public CameraCaptureSession::CaptureCallback
    {
    public:
        TO_STRING_IMPL("CActivityOne::CaptureCallback")

        CaptureCallback(
            /* [in] */ CActivityOne* host);

        CARAPI OnCaptureProgressed(
            /* [in] */ ICameraCaptureSession* session,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ICaptureResult* partialResult);

        CARAPI OnCaptureCompleted(
            /* [in] */ ICameraCaptureSession* session,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ITotalCaptureResult* result);
    private:
        CARAPI Process(
            /* [in] */ ICaptureResult* texture);

    private:
        CActivityOne* mHost;
    };

    class ImageSaver
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CActivityOne::ImageSaver")

        ImageSaver(
            /* [in] */ CActivityOne* host,
            /* [in] */ IImage* image,
            /* [in] */ IFile* file);

        CARAPI Run();

    private:
        CActivityOne* mHost;
        AutoPtr<IImage> mImage;
        AutoPtr<IFile> mFile;
    };

    class ShowToastRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CActivityOne::ShowToastRunnable")

        ShowToastRunnable(
            /* [in] */ CActivityOne* host,
            /* [in] */ const String& text);

        CARAPI Run();

    private:
        CActivityOne* mHost;
        String mText;
    };

    class MyListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CActivityOne* mHost;
    };

    /**
     * Compares two {@code Size}s based on their areas.
     */
    class CompareSizesByArea
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out) */ Int32* result);
    };

public:
    CAR_OBJECT_DECL()

    CActivityOne();

    ~CActivityOne();

    CARAPI constructor();

protected:

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    /**
     * Shows a {@link Toast} on the UI thread.
     *
     * @param text The message to show
     */
    CARAPI ShowToast(
        /* [in] */ const String& text);

    /**
     * Given {@code choices} of {@code Size}s supported by a camera, choose the smallest one that
     * is at least as large as the respective texture view size, and that is at most as large as the
     * respective max size, and whose aspect ratio matches with the specified value. If such size
     * doesn't exist, choose the largest one that is at most as large as the respective max size,
     * and whose aspect ratio matches with the specified value.
     *
     * @param choices           The list of sizes that the camera supports for the intended output
     *                          class
     * @param textureViewWidth  The width of the texture view relative to sensor coordinate
     * @param textureViewHeight The height of the texture view relative to sensor coordinate
     * @param maxWidth          The maximum width that can be chosen
     * @param maxHeight         The maximum height that can be chosen
     * @param aspectRatio       The aspect ratio
     * @return The optimal {@code Size}, or an arbitrary one if none were big enough
     */
    static AutoPtr<ISize> ChooseOptimalSize(
        /* [in] */ ArrayOf<ISize*>* choices,
        /* [in] */ Int32 textureViewWidth,
        /* [in] */ Int32 textureViewHeight,
        /* [in] */ Int32 maxWidth,
        /* [in] */ Int32 maxHeight,
        /* [in] */ ISize* aspectRatio);

    /**
     * Sets up member variables related to camera.
     *
     * @param width  The width of available size for camera preview
     * @param height The height of available size for camera preview
     */
    CARAPI SetUpCameraOutputs(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Opens the camera specified by {@link Camera2BasicFragment#mCameraId}.
     */
    CARAPI OpenCamera(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Closes the current {@link CameraDevice}.
     */
    CARAPI CloseCamera();

    /**
     * Starts a background thread and its {@link Handler}.
     */
    CARAPI StartBackgroundThread();

    /**
     * Stops the background thread and its {@link Handler}.
     */
    CARAPI StopBackgroundThread();

    /**
     * Creates a new {@link CameraCaptureSession} for camera preview.
     */
    CARAPI CreateCameraPreviewSession();

    /**
     * Configures the necessary {@link android.graphics.Matrix} transformation to `mTextureView`.
     * This method should be called after the camera preview size is determined in
     * setUpCameraOutputs and also the size of `mTextureView` is fixed.
     *
     * @param viewWidth  The width of `mTextureView`
     * @param viewHeight The height of `mTextureView`
     */
    CARAPI ConfigureTransform(
        /* [in] */ Int32 viewWidth,
        /* [in] */ Int32 viewHeight);

    /**
     * Initiate a still image capture.
     */
    CARAPI TakePicture();

    /**
     * Lock the focus as the first step for a still image capture.
     */
    CARAPI LockFocus();

    /**
     * Run the precapture sequence for capturing a still image. This method should be called when
     * we get a response in {@link #mCaptureCallback} from {@link #lockFocus()}.
     */
    CARAPI RunPrecaptureSequence();

    /**
     * Capture a still picture. This method should be called when we get a response in
     * {@link #mCaptureCallback} from both {@link #lockFocus()}.
     */
    CARAPI CaptureStillPicture();

    /**
     * Retrieves the JPEG orientation from the specified screen rotation.
     *
     * @param rotation The screen rotation.
     * @return The JPEG orientation (one of 0, 90, 270, and 360)
     */
    Int32 GetOrientation(
        /* [in] */ Int32 rotation);

    CARAPI UnlockFocus();

    CARAPI OnClick(
        /* [in] */ IView* view);

    CARAPI SetAutoFlash(
        /* [in] */ ICaptureRequestBuilder* requestBuilder);

private:
    /**
     * Camera state: Showing camera preview.
     */
    static const Int32 STATE_PREVIEW = 0;

    /**
     * Camera state: Waiting for the focus to be locked.
     */
    static const Int32 STATE_WAITING_LOCK = 1;

    /**
     * Camera state: Waiting for the exposure to be precapture state.
     */
    static const Int32 STATE_WAITING_PRECAPTURE = 2;

    /**
     * Camera state: Waiting for the exposure state to be something other than precapture.
     */
    static const Int32 STATE_WAITING_NON_PRECAPTURE = 3;

    /**
     * Camera state: Picture was taken.
     */
    static const Int32 STATE_PICTURE_TAKEN = 4;

    /**
     * Max preview width that is guaranteed by Camera2 API
     */
    static const Int32 MAX_PREVIEW_WIDTH = 1920;

    /**
     * Max preview height that is guaranteed by Camera2 API
     */
    static const Int32 MAX_PREVIEW_HEIGHT = 1080;

private:
    /**
     * Conversion from screen rotation to JPEG orientation.
     */
    static const AutoPtr<HashMap<Int32, Int32> > ORIENTATIONS;

    // static const Int32 REQUEST_CAMERA_PERMISSION = 1;
    // private static final String FRAGMENT_DIALOG = "dialog";


    /**
     * ID of the current {@link CameraDevice}.
     */
    String mCameraId;

    /**
     * {@link TextureView.SurfaceTextureListener} handles several lifecycle events on a
     * {@link TextureView}.
     */
    AutoPtr<ISurfaceTextureListener> mSurfaceTextureListener;

    /**
     * An {@link AutoFitTextureView} for camera preview.
     */
    AutoPtr<ITextureView> mTextureView;

    /**
     * A {@link CameraCaptureSession } for camera preview.
     */
    AutoPtr<ICameraCaptureSession> mCaptureSession;

    /**
     * A reference to the opened {@link CameraDevice}.
     */
    AutoPtr<ICameraDevice> mCameraDevice;

    /**
     * The {@link android.util.Size} of camera preview.
     */
    AutoPtr<ISize> mPreviewSize;

    /**
     * {@link CameraDevice.StateCallback} is called when {@link CameraDevice} changes its state.
     */
    AutoPtr<ICameraDeviceStateCallback> mStateCallback;

    /**
     * An additional thread for running tasks that shouldn't block the UI.
     */
    AutoPtr<IHandlerThread> mBackgroundThread;

    /**
     * A {@link Handler} for running tasks in the background.
     */
    AutoPtr<IHandler> mBackgroundHandler;

    /**
     * An {@link ImageReader} that handles still image capture.
     */
    AutoPtr<IImageReader> mImageReader;

    /**
     * This is the output file for our picture.
     */
    AutoPtr<IFile> mFile;

    AutoPtr<IImageReaderOnImageAvailableListener> mOnImageAvailableListener;

    /**
     * {@link CaptureRequest.Builder} for the camera preview
     */
    AutoPtr<ICaptureRequestBuilder> mPreviewRequestBuilder;

    /**
     * {@link CaptureRequest} generated by {@link #mPreviewRequestBuilder}
     */
    AutoPtr<ICaptureRequest> mPreviewRequest;

    /**
     * The current state of camera state for taking pictures.
     *
     * @see #mCaptureCallback
     */
    Int32 mState;// = STATE_PREVIEW;

    /**
     * A {@link Semaphore} to prevent the app from exiting before closing the camera.
     */
    AutoPtr<ISemaphore> mCameraOpenCloseLock;// = new Semaphore(1);

    /**
     * Whether the current camera device supports Flash or not.
     */
    Boolean mFlashSupported;

    /**
     * Orientation of the camera sensor
     */
    Int32 mSensorOrientation;

    /**
     * A {@link CameraCaptureSession.CaptureCallback} that handles events related to JPEG capture.
     */
    AutoPtr<ICameraCaptureSessionCaptureCallback> mCaptureCallback;

    AutoPtr<ICameraManager> mCameraManager;
};

} // namespace CameraDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_CameraDemo_CActivityOne_H__
