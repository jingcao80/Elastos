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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CAMERADEVICEIMPL_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CAMERADEVICEIMPL_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/hardware/camera2/CameraDevice.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Hardware::Camera2::ICameraDeviceStateCallback;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::Runnable;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::ITreeSet;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Impl {

#ifndef DECLARE_INNER_RUNNABLE
#define DECLARE_INNER_RUNNABLE(className)           \
    class className                                 \
        : public Runnable                           \
    {                                               \
    public:                                         \
        className(                                  \
            /* [in] */ IWeakReference* host);       \
                                                    \
        CARAPI Run();                               \
                                                    \
    private:                                        \
        AutoPtr<IWeakReference> mWeakHost;          \
    };
#endif

class CameraDeviceImpl
    : public CameraDevice
    , public ICameraDeviceImpl
{
public:
    class CaptureCallback
        : public Object
        , public ICameraDeviceImplCaptureCallback
    {
    public:
        TO_STRING_IMPL("CameraDeviceImpl::CaptureCallback")

        CAR_INTERFACE_DECL()

        /**
         * This method is called when the camera device has started capturing
         * the output image for the request, at the beginning of image exposure.
         *
         * @see android.media.MediaActionSound
         */
        CARAPI OnCaptureStarted(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ Int64 timestamp,
            /* [in] */ Int64 frameNumber);

        /**
         * This method is called when some results from an image capture are
         * available.
         *
         * @hide
         */
        CARAPI OnCapturePartial(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ICaptureResult* result);

        /**
         * This method is called when an image capture makes partial forward progress; some
         * (but not all) results from an image capture are available.
         *
         */
        CARAPI OnCaptureProgressed(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ICaptureResult* partialResult);

        /**
         * This method is called when an image capture has fully completed and all the
         * result metadata is available.
         */
        CARAPI OnCaptureCompleted(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ITotalCaptureResult* result);

        /**
         * This method is called instead of {@link #onCaptureCompleted} when the
         * camera device failed to produce a {@link CaptureResult} for the
         * request.
         */
        CARAPI OnCaptureFailed(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ICaptureFailure* failure);

        /**
         * This method is called independently of the others in CaptureCallback,
         * when a capture sequence finishes and all {@link CaptureResult}
         * or {@link CaptureFailure} for it have been returned via this callback.
         */
        CARAPI OnCaptureSequenceCompleted(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ Int32 sequenceId,
            /* [in] */ Int64 frameNumber);

        /**
         * This method is called independently of the others in CaptureCallback,
         * when a capture sequence aborts before any {@link CaptureResult}
         * or {@link CaptureFailure} for it have been returned via this callback.
         */
        CARAPI OnCaptureSequenceAborted(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ Int32 sequenceId);
    };

    class StateCallbackKK
        : public CameraDevice::StateCallback
        , public ICameraDeviceImplStateCallbackKK
    {
    public:
        TO_STRING_IMPL("CameraDeviceImpl::StateCallbackKK")

        CAR_INTERFACE_DECL()

        /**
         * The method called when a camera device has no outputs configured.
         *
         */
        CARAPI OnUnconfigured(
            /* [in] */ ICameraDevice* camera);

        /**
         * The method called when a camera device begins processing
         * {@link CaptureRequest capture requests}.
         *
         */
        CARAPI OnActive(
            /* [in] */ ICameraDevice* camera);

        /**
         * The method called when a camera device is busy.
         *
         */
        CARAPI OnBusy(
            /* [in] */ ICameraDevice* camera);

        /**
         * The method called when a camera device has finished processing all
         * submitted capture requests and has reached an idle state.
         *
         */
        CARAPI OnIdle(
            /* [in] */ ICameraDevice* camera);

        CARAPI OnClosed(
            /* [in] */ ICameraDevice* camera);
    };

    class CaptureCallbackHolder
        : public Object
        , public ICameraDeviceImplCaptureCallbackHolder
    {
    public:
        CAR_INTERFACE_DECL()

        CaptureCallbackHolder();

        virtual ~CaptureCallbackHolder();

        CARAPI constructor(
            /* [in] */ ICameraDeviceImplCaptureCallback* _callback,
            /* [in] */ IList* requestList,
            /* [in] */ IHandler* handler,
            /* [in] */ Boolean repeating);

        CARAPI IsRepeating(
            /* [out] */ Boolean* result);

        CARAPI GetCallback(
            /* [out] */ ICameraDeviceImplCaptureCallback** _callback);

        CARAPI GetRequest(
            /* [in] */ Int32 subsequenceId,
            /* [out] */ ICaptureRequest** request);

        CARAPI GetRequest(
            /* [out] */ ICaptureRequest** request);

        CARAPI GetHandler(
            /* [out] */ IHandler** handler);

    private:
        Boolean mRepeating;
        AutoPtr<ICameraDeviceImplCaptureCallback> mCallback;
        AutoPtr<IList> mRequestList;
        AutoPtr<IHandler> mHandler;
    };

    class FrameNumberTracker
        : public Object
        , public ICameraDeviceImplFrameNumberTracker
    {
    public:
        CAR_INTERFACE_DECL()

        FrameNumberTracker();

        virtual ~FrameNumberTracker() {}

        CARAPI constructor(
            /* [in] */ ICameraDeviceImpl* host);

        /**
         * This function is called every time when a result or an error is received.
         * @param frameNumber the frame number corresponding to the result or error
         * @param isError true if it is an error, false if it is not an error
         */
        CARAPI UpdateTracker(
            /* [in] */ Int64 frameNumber,
            /* [in] */ Boolean isError);

        /**
         * This function is called every time a result has been completed.
         *
         * <p>It keeps a track of all the partial results already created for a particular
         * frame number.</p>
         *
         * @param frameNumber the frame number corresponding to the result
         * @param result the total or partial result
         * @param partial {@true} if the result is partial, {@code false} if total
         */
        CARAPI UpdateTracker(
            /* [in] */ Int64 frameNumber,
            /* [in] */ ICaptureResult* result,
            /* [in] */ Boolean partial);

        /**
         * Attempt to pop off all of the partial results seen so far for the {@code frameNumber}.
         *
         * <p>Once popped-off, the partial results are forgotten (unless {@code updateTracker}
         * is called again with new partials for that frame number).</p>
         *
         * @param frameNumber the frame number corresponding to the result
         * @return a list of partial results for that frame with at least 1 element,
         *         or {@code null} if there were no partials recorded for that frame
         */
        CARAPI PopPartialResults(
            /* [in] */ Int64 frameNumber,
            /* [out] */ IList** list);

        CARAPI GetCompletedFrameNumber(
            /* [out] */ Int64* number);

    private:
        CARAPI Update();

    private:
        Int64 mCompletedFrameNumber;
        AutoPtr<ITreeSet> mFutureErrorSet;
        /** Map frame numbers to list of partial results */
        HashMap<Int64, AutoPtr<IList> > mPartialResults;
        CameraDeviceImpl* mHost;
    };

    class CameraDeviceCallbacks
        : public Object
        , public ICameraDeviceImplCameraDeviceCallbacks
        , public IICameraDeviceCallbacks
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CameraDeviceCallbacks();

        virtual ~CameraDeviceCallbacks() {}

        CARAPI constructor(
            /* [in] */ ICameraDeviceImpl* host);

        //@Override
        CARAPI AsBinder(
            /* [out] */ IBinder** binder);

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

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        /**
         * Called by onDeviceError for handling single-capture failures.
         */
        CARAPI OnCaptureErrorLocked(
            /* [in] */ Int32 errorCode,
            /* [in] */ ICaptureResultExtras* resultExtras);

    private:
        CameraDeviceImpl* mHost;
    };

private:
    DECLARE_INNER_RUNNABLE(CallOnOpeneRunnable)
    DECLARE_INNER_RUNNABLE(CallOnUnconfiguredRunnable)
    DECLARE_INNER_RUNNABLE(CallOnActiveRunnable)
    DECLARE_INNER_RUNNABLE(CallOnBusyRunnable)
    DECLARE_INNER_RUNNABLE(CallOnIdlRunnable)
    DECLARE_INNER_RUNNABLE(CallOnDisconnectedRunnable)

    class CallOnCloseRunnable
        : public Runnable
    {
    public:
        CallOnCloseRunnable(
            /* [in] */ IWeakReference* host);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
        Boolean mClosedOnce;
    };

    class RemoteFailureRunnable
        : public Runnable
    {
    public:
        RemoteFailureRunnable(
            /* [in] */ IWeakReference* host,
            /* [in] */ Boolean isError,
            /* [in] */ Int32 code);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
        Boolean mIsError;
        Int32 mCode;
    };

    class ResultDispatchRunnable
        : public Runnable
    {
    public:
        ResultDispatchRunnable(
            /* [in] */ IWeakReference* host,
            /* [in] */ Int32 requestId,
            /* [in] */ Int64 lastFrameNumber,
            /* [in] */ ICameraDeviceImplCaptureCallbackHolder* holder);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
        Int32 mRequestId;
        Int64 mLastFrameNumber;
        AutoPtr<ICameraDeviceImplCaptureCallbackHolder> mHolder;
    };

    class SequenceCompleteResultDispatchRunnable
        : public Runnable
    {
    public:
        SequenceCompleteResultDispatchRunnable(
            /* [in] */ IWeakReference* host,
            /* [in] */ Int32 requestId,
            /* [in] */ Int64 keyValue,
            /* [in] */ ICameraDeviceImplCaptureCallbackHolder* holder);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
        Int32 mRequestId;
        Int64 mKeyValue;
        AutoPtr<ICameraDeviceImplCaptureCallbackHolder> mHolder;
    };

    class OnDeviceErrorRunnable
        : public Runnable
    {
    public:
        OnDeviceErrorRunnable(
            /* [in] */ IWeakReference* host,
            /* [in] */ Int32 errorCode);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
        Int32 mErrorCode;
    };

    class OnCaptureStartedRunnable
        : public Runnable
    {
    public:
        OnCaptureStartedRunnable(
            /* [in] */ IWeakReference* host,
            /* [in] */ ICaptureResultExtras* resultExtras,
            /* [in] */ Int64 timestamp,
            /* [in] */ Int64 frameNumber,
            /* [in] */ ICameraDeviceImplCaptureCallbackHolder* holder);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
        AutoPtr<ICaptureResultExtras> mResultExtras;
        Int64 mTimestamp;
        Int64 mFrameNumber;
        AutoPtr<ICameraDeviceImplCaptureCallbackHolder> mHolder;
    };

    class OnResultReceivedRunnable
        : public Runnable
    {
    public:
        OnResultReceivedRunnable(
            /* [in] */ IWeakReference* host,
            /* [in] */ ICameraDeviceImplCaptureCallbackHolder* holder,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ICaptureResult* result);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
        AutoPtr<ICameraDeviceImplCaptureCallbackHolder> mHolder;
        AutoPtr<ICaptureRequest> mRequest;
        AutoPtr<ICaptureResult> mResult;
    };

    class FinalCaptureResultRunnable
        : public Runnable
    {
    public:
        FinalCaptureResultRunnable(
            /* [in] */ IWeakReference* host,
            /* [in] */ ICameraDeviceImplCaptureCallbackHolder* holder,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ITotalCaptureResult* result);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
        AutoPtr<ICameraDeviceImplCaptureCallbackHolder> mHolder;
        AutoPtr<ICaptureRequest> mRequest;
        AutoPtr<ITotalCaptureResult> mResult;
    };

    class OnCaptureErrorLockedRunnable
        : public Runnable
    {
    public:
        OnCaptureErrorLockedRunnable(
            /* [in] */ IWeakReference* host,
            /* [in] */ ICameraDeviceImplCaptureCallbackHolder* holder,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ICaptureFailure* failure);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
        AutoPtr<ICameraDeviceImplCaptureCallbackHolder> mHolder;
        AutoPtr<ICaptureRequest> mRequest;
        AutoPtr<ICaptureFailure> mFailure;
    };

public:
    CAR_INTERFACE_DECL()

    CameraDeviceImpl();

    virtual ~CameraDeviceImpl();

    CARAPI constructor(
        /* [in] */ const String& cameraId,
        /* [in] */ ICameraDeviceStateCallback* _callback,
        /* [in] */ IHandler* handler,
        /* [in] */ ICameraCharacteristics* characteristics);

    CARAPI GetCallbacks(
        /* [out] */ ICameraDeviceImplCameraDeviceCallbacks** outcd);

    CARAPI SetRemoteDevice(
        /* [in] */ IICameraDeviceUser* remoteDevice);

    /**
     * Call to indicate failed connection to a remote camera device.
     *
     * <p>This places the camera device in the error state and informs the callback.
     * Use in place of setRemoteDevice() when startup fails.</p>
     */
    CARAPI SetRemoteFailure(
        /* [in] */ ECode failure);

    //@Override
    CARAPI GetId(
        /* [out] */ String* str);

    CARAPI ConfigureOutputs(
        /* [in] */ IList* outputs);

    /**
     * Attempt to configure the outputs; the device goes to idle and then configures the
     * new outputs if possible.
     *
     * <p>The configuration may gracefully fail, if there are too many outputs, if the formats
     * are not supported, or if the sizes for that format is not supported. In this case this
     * function will return {@code false} and the unconfigured callback will be fired.</p>
     *
     * <p>If the configuration succeeds (with 1 or more outputs), then the idle callback is fired.
     * Unconfiguring the device always fires the idle callback.</p>
     *
     * @param outputs a list of one or more surfaces, or {@code null} to unconfigure
     * @return whether or not the configuration was successful
     *
     * @throws CameraAccessException if there were any unexpected problems during configuration
     */
    CARAPI ConfigureOutputsChecked(
        /* [in] */ IList* outputs,
        /* [out] */ Boolean* value);

    //@Override
    CARAPI CreateCaptureSession(
        /* [in] */ IList* outputs,
        /* [in] */ ICameraCaptureSessionStateCallback* _callback,
        /* [in] */ IHandler* handler);

    /**
     * For use by backwards-compatibility code only.
     */
    CARAPI SetSessionListener(
        /* [in] */ ICameraDeviceImplStateCallbackKK* sessionCallback);

    // /@Override
    CARAPI CreateCaptureRequest(
        /* [in] */ Int32 templateType,
        /* [out] */ ICaptureRequestBuilder** builder);

    CARAPI Capture(
        /* [in] */ ICaptureRequest* request,
        /* [in] */ ICameraDeviceImplCaptureCallback* ccallback,
        /* [in] */ IHandler* handler,
        /* [out] */ Int32* value);

    CARAPI CaptureBurst(
        /* [in] */ IList* requests,
        /* [in] */ ICameraDeviceImplCaptureCallback* ccallback,
        /* [in] */ IHandler* handler,
        /* [out] */ Int32* value);

    CARAPI SetRepeatingRequest(
        /* [in] */ ICaptureRequest* request,
        /* [in] */ ICameraDeviceImplCaptureCallback* ccallback,
        /* [in] */ IHandler* handler,
        /* [out] */ Int32* value);

    CARAPI SetRepeatingBurst(
        /* [in] */ IList* requests,
        /* [in] */ ICameraDeviceImplCaptureCallback* ccallback,
        /* [in] */ IHandler* handler,
        /* [out] */ Int32* result);

    CARAPI StopRepeating();


    CARAPI Flush();

    //@Override
    CARAPI Close();

    /**
     * Default handler management.
     *
     * <p>
     * If handler is null, get the current thread's
     * Looper to create a Handler with. If no looper exists, throw {@code IllegalArgumentException}.
     * </p>
     */
    static CARAPI CheckHandler(
        /* [in] */ IHandler* handler,
        /* [out] */ IHandler** result);

    /**
     * Default handler management, conditional on there being a callback.
     *
     * <p>If the callback isn't null, check the handler, otherwise pass it through.</p>
     */
    static CARAPI CheckHandler(
        /* [in] */ IHandler* handler,
        /* [in] */ IInterface* _callback,
        /* [out] */ IHandler** result);

private:
    /**
     * This method checks lastFrameNumber returned from ICameraDeviceUser methods for
     * starting and stopping repeating request and flushing.
     *
     * <p>If lastFrameNumber is NO_FRAMES_CAPTURED, it means that the request was never
     * sent to HAL. Then onCaptureSequenceAborted is immediately triggered.
     * If lastFrameNumber is non-negative, then the requestId and lastFrameNumber pair
     * is added to the list mFrameNumberRequestPairs.</p>
     *
     * @param requestId the request ID of the current repeating request.
     *
     * @param lastFrameNumber last frame number returned from binder.
     */
    CARAPI CheckEarlyTriggerSequenceComplete(
        /* [in] */ Int32 requestId,
        /* [in] */ Int64 lastFrameNumber);

    CARAPI SubmitCaptureRequest(
        /* [in] */ IList* requestList,
        /* [in] */ ICameraDeviceImplCaptureCallback* _callback,
        /* [in] */ IHandler* handler,
        /* [in] */ Boolean repeating,
        /* [out] */ Int32* result);

    CARAPI WaitUntilIdle();

    CARAPI CheckAndFireSequenceComplete();

    CARAPI CheckIfCameraClosedOrInError();

    /** Whether the camera device has started to close (may not yet have finished) */
    CARAPI_(Boolean) IsClosed();

    CARAPI_(AutoPtr<ICameraCharacteristics>) GetCharacteristics();

private:
    friend class CameraCaptureSessionImpl;

    String TAG;
    Boolean DEBUG;

    static const Int32 REQUEST_ID_NONE;

    // TODO: guard every function with if (!mRemoteDevice) check (if it was closed)
    AutoPtr<IICameraDeviceUser> mRemoteDevice;

    // Lock to synchronize cross-thread access to device public interface
    Object mInterfaceLock; // access from this class and Session only!
    AutoPtr<ICameraDeviceImplCameraDeviceCallbacks> mCallbacks;

    AutoPtr<ICameraDeviceStateCallback> mDeviceCallback;
    AutoPtr<ICameraDeviceImplStateCallbackKK> mSessionStateCallback;
    AutoPtr<IHandler> mDeviceHandler;

    Boolean mClosing;
    Boolean mInError;
    Boolean mIdle;

    /** map request IDs to callback/request data */
    AutoPtr<ISparseArray> mCaptureCallbackMap;

    Int32 mRepeatingRequestId;
    AutoPtr<IArrayList> mRepeatingRequestIdDeletedList;
    // Map stream IDs to Surfaces
    AutoPtr<ISparseArray> mConfiguredOutputs;

    String mCameraId;
    AutoPtr<ICameraCharacteristics> mCharacteristics;
    Int32 mTotalPartialCount;

    /**
     * A list tracking request and its expected last frame.
     * Updated when calling ICameraDeviceUser methods.
     */
    AutoPtr<IList> mFrameNumberRequestPairs;

    /**
     * An object tracking received frame numbers.
     * Updated when receiving callbacks from ICameraDeviceCallbacks.
     */
    AutoPtr<ICameraDeviceImplFrameNumberTracker> mFrameNumberTracker;

    AutoPtr<ICameraCaptureSessionImpl> mCurrentSession;
    Int32 mNextSessionId;

    AutoPtr<CallOnOpeneRunnable> mCallOnOpened;
    AutoPtr<CallOnUnconfiguredRunnable> mCallOnUnconfigured;
    AutoPtr<CallOnActiveRunnable> mCallOnActive;
    AutoPtr<CallOnBusyRunnable> mCallOnBusy;
    AutoPtr<CallOnCloseRunnable> mCallOnClosed;
    AutoPtr<CallOnIdlRunnable> mCallOnIdle;
    AutoPtr<CallOnDisconnectedRunnable> mCallOnDisconnected;
};

} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CAMERADEVICEIMPL_H__
