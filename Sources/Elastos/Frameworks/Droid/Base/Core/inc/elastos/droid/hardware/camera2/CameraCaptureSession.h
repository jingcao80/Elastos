
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_CAMERACAPTURESESSION_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_CAMERACAPTURESESSION_H__

#include "Elastos.Droid.Hardware.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

class CameraCaptureSession
    : public Object
    , public ICameraCaptureSession
    , public ICloseable
{
public:
    class StateCallback
        : public Object
        , public ICameraCaptureSessionStateCallback
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * This method is called every time the session has no more capture requests to process.
         *
         * <p>During the creation of a new session, this callback is invoked right after
         * {@link #onConfigured} if no capture requests were submitted to the session prior to it
         * completing configuration.</p>
         *
         * <p>Otherwise, this callback will be invoked any time the session finishes processing
         * all of its active capture requests, and no repeating request or burst is set up.</p>
         *
         * @param session the session returned by {@link CameraDevice#createCaptureSession}
         *
         */
        CARAPI OnReady(
            /* [in] */ ICameraCaptureSession* session);

        /**
         * This method is called when the session starts actively processing capture requests.
         *
         * <p>If capture requests are submitted prior to {@link #onConfigured} being called,
         * then the session will start processing those requests immediately after the callback,
         * and this method will be immediately called after {@link #onConfigured}.
         *
         * <p>If the session runs out of capture requests to process and calls {@link #onReady},
         * then this callback will be invoked again once new requests are submitted for capture.</p>
         *
         * @param session the session returned by {@link CameraDevice#createCaptureSession}
         */
        CARAPI OnActive(
            /* [in] */ ICameraCaptureSession* session);

        /**
         * This method is called when the session is closed.
         *
         * <p>A session is closed when a new session is created by the parent camera device,
         * or when the parent camera device is closed (either by the user closing the device,
         * or due to a camera device disconnection or fatal error).</p>
         *
         * <p>Once a session is closed, all methods on it will throw an IllegalStateException, and
         * any repeating requests or bursts are stopped (as if {@link #stopRepeating()} was called).
         * However, any in-progress capture requests submitted to the session will be completed
         * as normal.</p>
         *
         * @param session the session returned by {@link CameraDevice#createCaptureSession}
         */
        CARAPI OnClosed(
            /* [in] */ ICameraCaptureSession* session);
    };

    class StateListener
        : public StateCallback
    {
    };

    class CaptureCallback
        : public Object
        , public ICameraCaptureSessionCaptureCallback
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * This method is called when the camera device has started capturing
         * the output image for the request, at the beginning of image exposure.
         *
         * <p>This callback is invoked right as the capture of a frame begins,
         * so it is the most appropriate time for playing a shutter sound,
         * or triggering UI indicators of capture.</p>
         *
         * <p>The request that is being used for this capture is provided, along
         * with the actual timestamp for the start of exposure. This timestamp
         * matches the timestamp that will be included in
         * {@link CaptureResult#SENSOR_TIMESTAMP the result timestamp field},
         * and in the buffers sent to each output Surface. These buffer
         * timestamps are accessible through, for example,
         * {@link android.media.Image#getTimestamp() Image.getTimestamp()} or
         * {@link android.graphics.SurfaceTexture#getTimestamp()}.
         * The frame number included is equal to the frame number that will be included in
         * {@link CaptureResult#getFrameNumber}.</p>
         *
         * <p>For the simplest way to play a shutter sound camera shutter or a
         * video recording start/stop sound, see the
         * {@link android.media.MediaActionSound} class.</p>
         *
         * <p>The default implementation of this method does nothing.</p>
         *
         * @param session the session returned by {@link CameraDevice#createCaptureSession}
         * @param request the request for the capture that just begun
         * @param timestamp the timestamp at start of capture, in nanoseconds.
         * @param frameNumber the frame number for this capture
         *
         * @see android.media.MediaActionSound
         */
        CARAPI OnCaptureStarted(
            /* [in] */ ICameraCaptureSession* session,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ Int64 timestamp,
            /* [in] */ Int64 frameNumber);

        /**
         * Temporary for API change transition
         * @hide
         */
        CARAPI OnCaptureStarted(
            /* [in] */ ICameraCaptureSession* session,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ Int64 timestamp);

        /**
         * This method is called when some results from an image capture are
         * available.
         *
         * <p>The result provided here will contain some subset of the fields of
         * a full result. Multiple onCapturePartial calls may happen per
         * capture; a given result field will only be present in one partial
         * capture at most. The final onCaptureCompleted call will always
         * contain all the fields, whether onCapturePartial was called or
         * not.</p>
         *
         * <p>The default implementation of this method does nothing.</p>
         *
         * @param session the session returned by {@link CameraDevice#createCaptureSession}
         * @param request The request that was given to the CameraDevice
         * @param result The partial output metadata from the capture, which
         * includes a subset of the CaptureResult fields.
         *
         * @see #capture
         * @see #captureBurst
         * @see #setRepeatingRequest
         * @see #setRepeatingBurst
         *
         * @hide
         */
        CARAPI OnCapturePartial(
            /* [in] */ ICameraCaptureSession* session,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ICaptureResult* result);

        /**
         * This method is called when an image capture makes partial forward progress; some
         * (but not all) results from an image capture are available.
         *
         * <p>The result provided here will contain some subset of the fields of
         * a full result. Multiple {@link #onCaptureProgressed} calls may happen per
         * capture; a given result field will only be present in one partial
         * capture at most. The final {@link #onCaptureCompleted} call will always
         * contain all the fields (in particular, the union of all the fields of all
         * the partial results composing the total result).</p>
         *
         * <p>For each request, some result data might be available earlier than others. The typical
         * delay between each partial result (per request) is a single frame interval.
         * For performance-oriented use-cases, applications should query the metadata they need
         * to make forward progress from the partial results and avoid waiting for the completed
         * result.</p>
         *
         * <p>Each request will generate at least {@code 1} partial results, and at most
         * {@link CameraCharacteristics#REQUEST_PARTIAL_RESULT_COUNT} partial results.</p>
         *
         * <p>Depending on the request settings, the number of partial results per request
         * will vary, although typically the partial count could be the same as long as the
         * camera device subsystems enabled stay the same.</p>
         *
         * <p>The default implementation of this method does nothing.</p>
         *
         * @param session the session returned by {@link CameraDevice#createCaptureSession}
         * @param request The request that was given to the CameraDevice
         * @param partialResult The partial output metadata from the capture, which
         * includes a subset of the {@link TotalCaptureResult} fields.
         *
         * @see #capture
         * @see #captureBurst
         * @see #setRepeatingRequest
         * @see #setRepeatingBurst
         */
        CARAPI OnCaptureProgressed(
            /* [in] */ ICameraCaptureSession* session,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ICaptureResult* partialResult);

        /**
         * This method is called when an image capture has fully completed and all the
         * result metadata is available.
         *
         * <p>This callback will always fire after the last {@link #onCaptureProgressed};
         * in other words, no more partial results will be delivered once the completed result
         * is available.</p>
         *
         * <p>For performance-intensive use-cases where latency is a factor, consider
         * using {@link #onCaptureProgressed} instead.</p>
         *
         * <p>The default implementation of this method does nothing.</p>
         *
         * @param session the session returned by {@link CameraDevice#createCaptureSession}
         * @param request The request that was given to the CameraDevice
         * @param result The total output metadata from the capture, including the
         * final capture parameters and the state of the camera system during
         * capture.
         *
         * @see #capture
         * @see #captureBurst
         * @see #setRepeatingRequest
         * @see #setRepeatingBurst
         */
        CARAPI OnCaptureCompleted(
            /* [in] */ ICameraCaptureSession* session,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ITotalCaptureResult* result);

        /**
         * This method is called instead of {@link #onCaptureCompleted} when the
         * camera device failed to produce a {@link CaptureResult} for the
         * request.
         *
         * <p>Other requests are unaffected, and some or all image buffers from
         * the capture may have been pushed to their respective output
         * streams.</p>
         *
         * <p>The default implementation of this method does nothing.</p>
         *
         * @param session
         *            The session returned by {@link CameraDevice#createCaptureSession}
         * @param request
         *            The request that was given to the CameraDevice
         * @param failure
         *            The output failure from the capture, including the failure reason
         *            and the frame number.
         *
         * @see #capture
         * @see #captureBurst
         * @see #setRepeatingRequest
         * @see #setRepeatingBurst
         */
        CARAPI OnCaptureFailed(
            /* [in] */ ICameraCaptureSession* session,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ICaptureFailure* failure);

        /**
         * This method is called independently of the others in CaptureCallback,
         * when a capture sequence finishes and all {@link CaptureResult}
         * or {@link CaptureFailure} for it have been returned via this listener.
         *
         * <p>In total, there will be at least one result/failure returned by this listener
         * before this callback is invoked. If the capture sequence is aborted before any
         * requests have been processed, {@link #onCaptureSequenceAborted} is invoked instead.</p>
         *
         * <p>The default implementation does nothing.</p>
         *
         * @param session
         *            The session returned by {@link CameraDevice#createCaptureSession}
         * @param sequenceId
         *            A sequence ID returned by the {@link #capture} family of functions.
         * @param frameNumber
         *            The last frame number (returned by {@link CaptureResult#getFrameNumber}
         *            or {@link CaptureFailure#getFrameNumber}) in the capture sequence.
         *
         * @see CaptureResult#getFrameNumber()
         * @see CaptureFailure#getFrameNumber()
         * @see CaptureResult#getSequenceId()
         * @see CaptureFailure#getSequenceId()
         * @see #onCaptureSequenceAborted
         */
        CARAPI OnCaptureSequenceCompleted(
            /* [in] */ ICameraCaptureSession* session,
            /* [in] */ Int32 sequenceId,
            /* [in] */ Int64 frameNumber);

        /**
         * This method is called independently of the others in CaptureCallback,
         * when a capture sequence aborts before any {@link CaptureResult}
         * or {@link CaptureFailure} for it have been returned via this listener.
         *
         * <p>Due to the asynchronous nature of the camera device, not all submitted captures
         * are immediately processed. It is possible to clear out the pending requests
         * by a variety of operations such as {@link CameraCaptureSession#stopRepeating} or
         * {@link CameraCaptureSession#abortCaptures}. When such an event happens,
         * {@link #onCaptureSequenceCompleted} will not be called.</p>
         *
         * <p>The default implementation does nothing.</p>
         *
         * @param session
         *            The session returned by {@link CameraDevice#createCaptureSession}
         * @param sequenceId
         *            A sequence ID returned by the {@link #capture} family of functions.
         *
         * @see CaptureResult#getFrameNumber()
         * @see CaptureFailure#getFrameNumber()
         * @see CaptureResult#getSequenceId()
         * @see CaptureFailure#getSequenceId()
         * @see #onCaptureSequenceCompleted
         */
        CARAPI OnCaptureSequenceAborted(
            /* [in] */ ICameraCaptureSession* session,
            /* [in] */ Int32 sequenceId);
    };

    class CaptureListener
        : public CaptureCallback
    {
    };

public:
    CAR_INTERFACE_DECL()
};

} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_CAMERACAPTURESESSION_H__
