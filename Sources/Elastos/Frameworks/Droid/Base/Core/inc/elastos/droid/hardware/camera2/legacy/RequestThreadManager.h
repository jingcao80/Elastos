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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_REQUESTTHREADMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_REQUESTTHREADMANAGER_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IOnFrameAvailableListener;
using Elastos::Droid::Hardware::Camera2::Legacy::IRequestHolder;
using Elastos::Droid::Hardware::Camera2::Utils::ILongParcelable;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IConditionVariable;
using Elastos::Droid::Utility::ISize;
using Elastos::Droid::View::ISurface;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class RequestThreadManager
    : public Object
    , public IRequestThreadManager
{
public:
    /**
     * Counter class used to calculate and log the current FPS of frame production.
     */
    class RequestThreadManagerFpsCounter
        : public Object
        , public IRequestThreadManagerFpsCounter
    {
    public:
        CAR_INTERFACE_DECL()

        RequestThreadManagerFpsCounter();

        CARAPI constructor(
            /* [in] */ const String& streamType);

        CARAPI CountFrame();

        CARAPI CheckFps(
            /* [out] */ Double* result);

        CARAPI StaggeredLog();

        CARAPI CountAndLog();

    private:
        //TODO: Hook this up to SystTrace?
        static const String TAG;
        Int32 mFrameCount;
        Int64 mLastTime;
        Int64 mLastPrintTime;
        Double mLastFps;
        String mStreamType;
        static const Int64 NANO_PER_SECOND; //ns
    };

private:
    friend class LegacyCameraDevice;
    /**
     * Container object for Configure messages.
     */
    class ConfigureHolder
        : public Object
    {
    public:
        ConfigureHolder(
            /* [in] */ IConditionVariable* condition,
            /* [in] */ ICollection* surfaces);

    public:
        AutoPtr<IConditionVariable> mCondition;
        AutoPtr<ICollection> mSurfaces;
    };

    class CameraErrorCallback
        : public Object
        , public ICameraErrorCallback
    {
    public:
        CAR_INTERFACE_DECL()

        CameraErrorCallback(
            /* [in] */ RequestThreadManager* host);

        CARAPI OnError(
            /* [in] */ Int32 error,
            /* [in] */ IHardwareCamera* camera);

    private:
        RequestThreadManager* mHost;
    };

    class CameraPictureCallback
        : public Object
        , public IPictureCallback
    {
    public:
        CAR_INTERFACE_DECL()

        CameraPictureCallback(
            /* [in] */ RequestThreadManager* host);

        CARAPI OnPictureTaken(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ IHardwareCamera* camera);

    private:
        RequestThreadManager* mHost;
    };

    class CameraShutterCallback
        : public Object
        , public IShutterCallback
    {
    public:
        CAR_INTERFACE_DECL()

        CameraShutterCallback(
            /* [in] */ RequestThreadManager* host);

        CARAPI OnShutter();

    private:
        RequestThreadManager* mHost;
    };

    class SurfaceTextureOnFrameAvailableListener
        : public Object
        , public IOnFrameAvailableListener
    {
    public:
        CAR_INTERFACE_DECL()

        SurfaceTextureOnFrameAvailableListener(
            /* [in] */ RequestThreadManager* host);

        CARAPI OnFrameAvailable(
            /* [in] */ ISurfaceTexture* surfaceTexture);

    private:
        RequestThreadManager* mHost;
    };

    class MyHandlerCallback
        : public Object
        , public IHandlerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyHandlerCallback(
            /* [in] */ RequestThreadManager* host);

        CARAPI HandleMessage(
           /* [in] */ IMessage* msg,
           /* [out] */ Boolean* result);

    private:
        Boolean mCleanup;
        AutoPtr<ILegacyResultMapper> mMapper;
        RequestThreadManager* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    RequestThreadManager();

    virtual ~RequestThreadManager() {}

    /**
     * Create a new RequestThreadManager.
     *
     * @param cameraId the id of the camera to use.
     * @param camera an open camera object.  The RequestThreadManager takes ownership of this camera
     *               object, and is responsible for closing it.
     * @param characteristics the static camera characteristics corresponding to this camera device
     * @param deviceState a {@link CameraDeviceState} state machine.
     */
    CARAPI constructor(
        /* [in] */ Int32 cameraId,
        /* [in] */ IHardwareCamera* camera,
        /* [in] */ ICameraCharacteristics* characteristics,
        /* [in] */ ICameraDeviceState* deviceState);

    /**
     * Start the request thread.
     */
    CARAPI Start();

    /**
     * Flush any pending requests.
     *
     * @return the last frame number.
     */
    CARAPI Flush(
        /* [out] */ Int64* value);

    /**
     * Quit the request thread, and clean up everything.
     */
    CARAPI Quit();

    /**
     * Submit the given burst of requests to be captured.
     *
     * <p>If the burst is repeating, replace the current repeating burst.</p>
     *
     * @param requests the burst of requests to add to the queue.
     * @param repeating true if the burst is repeating.
     * @param frameNumber an output argument that contains either the frame number of the last frame
     *                    that will be returned for this request, or the frame number of the last
     *                    frame that will be returned for the current repeating request if this
     *                    burst is set to be repeating.
     * @return the request id.
     */
    CARAPI SubmitCaptureRequests(
        /* [in] */ IList* requests,
        /* [in] */ Boolean repeating,
        /* [in] */ ILongParcelable* frameNumber,
        /* [out] */ Int32* value);

    /**
     * Cancel a repeating request.
     *
     * @param requestId the id of the repeating request to cancel.
     * @return the last frame to be returned from the HAL for the given repeating request, or
     *          {@code INVALID_FRAME} if none exists.
     */
    CARAPI CancelRepeating(
        /* [in] */ Int32 requestId,
        /* [out] */ Int64* value);

    /**
     * Configure with the current list of output Surfaces.
     *
     * <p>
     * This operation blocks until the configuration is complete.
     * </p>
     *
     * <p>Using a {@code null} or empty {@code outputs} list is the equivalent of unconfiguring.</p>
     *
     * @param outputs a {@link java.util.Collection} of outputs to configure.
     */
    CARAPI Configure(
        /* [in] */ ICollection* outputs);

private:
    /**
     * Fake preview for jpeg captures when there is no active preview
     */
    CARAPI_(void) CreateDummySurface();

    CARAPI_(void) StopPreview();

    CARAPI_(void) StartPreview();

    CARAPI DoJpegCapturePrepare(
        /* [in] */ IRequestHolder* request);

    CARAPI_(void) DoJpegCapture(
        /* [in] */ IRequestHolder* request);

    CARAPI DoPreviewCapture(
        /* [in] */ IRequestHolder* request);

    CARAPI ConfigureOutputs(
        /* [in] */ ICollection* outputs);

    CARAPI_(void) ResetJpegSurfaceFormats(
        /* [in] */ ICollection* surfaces);

    /**
     * Find a JPEG size (that is supported by the legacy camera device) which is equal to or larger
     * than all of the configured {@code JPEG} outputs (by both width and height).
     *
     * <p>If multiple supported JPEG sizes are larger, select the smallest of them which
     * still satisfies the above constraint.</p>
     *
     * <p>As a result, the returned size is guaranteed to be usable without needing
     * to upscale any of the outputs. If only one {@code JPEG} surface is used,
     * then no scaling/cropping is necessary between the taken picture and
     * the {@code JPEG} output surface.</p>
     *
     * @param callbackOutputs a non-{@code null} list of {@code Surface}s with any image formats
     * @param params api1 parameters (used for reading only)
     *
     * @return a size large enough to fit all of the configured {@code JPEG} outputs, or
     *          {@code null} if the {@code callbackOutputs} did not have any {@code JPEG}
     *          surfaces.
     */
    CARAPI CalculatePictureSize(
        /* [in] */ ICollection* callbackOutputs,
        /* [in] */ IParameters* params,
        /* [out] */ ISize** size);

    static CARAPI_(Boolean) CheckAspectRatiosMatch(
        /* [in] */ ISize* a,
        /* [in] */ ISize* b);

    // Calculate the highest FPS range supported
    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetPhotoPreviewFpsRange(
        /* [in] */ IList* frameRates);

private:
    String TAG;
    Int32 mCameraId;
    AutoPtr<IRequestHandlerThread> mRequestThread;

    static const Boolean DEBUG;
    // For slightly more spammy messages that will get repeated every frame
    static const Boolean VERBOSE;
    AutoPtr<IHardwareCamera> mCamera;
    AutoPtr<ICameraCharacteristics> mCharacteristics;

    AutoPtr<ICameraDeviceState> mDeviceState;
    AutoPtr<ICaptureCollector> mCaptureCollector;
    AutoPtr<ILegacyFocusStateMapper> mFocusStateMapper;
    AutoPtr<ILegacyFaceDetectMapper> mFaceDetectMapper;

    static const Int32 MSG_CONFIGURE_OUTPUTS = 1;
    static const Int32 MSG_SUBMIT_CAPTURE_REQUEST = 2;
    static const Int32 MSG_CLEANUP = 3;

    static const Int32 MAX_IN_FLIGHT_REQUESTS;

    static const Int32 PREVIEW_FRAME_TIMEOUT; // ms
    static const Int32 JPEG_FRAME_TIMEOUT; // ms (same as CTS for API2)
    static const Int32 REQUEST_COMPLETE_TIMEOUT; // ms (same as JPEG timeout)

    static const Float ASPECT_RATIO_TOLERANCE;
    Boolean mPreviewRunning;

    AutoPtr<IList> mPreviewOutputs;
    AutoPtr<IList> mCallbackOutputs;
    AutoPtr<IGLThreadManager> mGLThreadManager;
    AutoPtr<ISurfaceTexture> mPreviewTexture;
    AutoPtr<IParameters> mParams;

    AutoPtr<IList> mJpegSurfaceIds;

    AutoPtr<ISize> mIntermediateBufferSize;

    AutoPtr<IRequestQueue> mRequestQueue;
    AutoPtr<ILegacyRequest> mLastRequest;
    AutoPtr<ISurfaceTexture> mDummyTexture;
    AutoPtr<ISurface> mDummySurface;

    Object mIdleLock;
    AutoPtr<IRequestThreadManagerFpsCounter> mPrevCounter;
    AutoPtr<IRequestThreadManagerFpsCounter> mRequestCounter;

    // Stuff JPEGs into HAL_PIXEL_FORMAT_RGBA_8888 gralloc buffers to get around SW write
    // limitations for (b/17379185).
    static const Boolean USE_BLOB_FORMAT_OVERRIDE;

    AutoPtr<CameraErrorCallback> mErrorCallback;

    AutoPtr<IConditionVariable> mReceivedJpeg;

    AutoPtr<CameraPictureCallback> mJpegCallback;

    AutoPtr<CameraShutterCallback> mJpegShutterCallback;

    AutoPtr<SurfaceTextureOnFrameAvailableListener> mPreviewCallback;

    AutoPtr<MyHandlerCallback> mRequestHandlerCb;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_REQUESTTHREADMANAGER_H__
