
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_CAPTUREFAILURE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_CAPTUREFAILURE_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

class CaptureFailure
    : public Object
    , public ICaptureFailure
{
public:
    CAR_INTERFACE_DECL()

    CaptureFailure();

    virtual ~CaptureFailure() {}

    CARAPI constructor();

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ICaptureRequest* request,
        /* [in] */ Int32 reason,
        /* [in] */ Boolean dropped,
        /* [in] */ Int32 sequenceId,
        /* [in] */ Int64 frameNumber);

    /**
     * Get the request associated with this failed capture.
     *
     * <p>Whenever a request is unsuccessfully captured, with
     * {@link CameraCaptureSession.CaptureCallback#onCaptureFailed},
     * the {@code failed capture}'s {@code getRequest()} will return that {@code request}.
     * </p>
     *
     * <p>In particular,
     * <code><pre>cameraDevice.capture(someRequest, new CaptureCallback() {
     *     {@literal @}Override
     *     void onCaptureFailed(CaptureRequest myRequest, CaptureFailure myFailure) {
     *         assert(myFailure.getRequest.equals(myRequest) == true);
     *     }
     * };
     * </code></pre>
     * </p>
     *
     * @return The request associated with this failed capture. Never {@code null}.
     */
    CARAPI GetRequest(
        /* [out] */ ICaptureRequest** outcr);

    /**
     * Get the frame number associated with this failed capture.
     *
     * <p>Whenever a request has been processed, regardless of failed capture or success,
     * it gets a unique frame number assigned to its future result/failed capture.</p>
     *
     * <p>This value monotonically increments, starting with 0,
     * for every new result or failure; and the scope is the lifetime of the
     * {@link CameraDevice}.</p>
     *
     * @return long frame number
     */
    CARAPI GetFrameNumber(
        /* [out] */  Int64* value);

    /**
     * Determine why the request was dropped, whether due to an error or to a user
     * action.
     *
     * @return int One of {@code REASON_*} integer constants.
     *
     * @see #REASON_ERROR
     * @see #REASON_FLUSHED
     */
    CARAPI GetReason(
        /* [out] */  Int32* value);

    /**
     * Determine if the image was captured from the camera.
     *
     * <p>If the image was not captured, no image buffers will be available.
     * If the image was captured, then image buffers may be available.</p>
     *
     * @return boolean True if the image was captured, false otherwise.
     */
    CARAPI WasImageCaptured(
        /* [out] */  Boolean* value);

    /**
     * The sequence ID for this failed capture that was returned by the
     * {@link CameraCaptureSession#capture} family of functions.
     *
     * <p>The sequence ID is a unique monotonically increasing value starting from 0,
     * incremented every time a new group of requests is submitted to the CameraDevice.</p>
     *
     * @return int The ID for the sequence of requests that this capture failure is the result of
     *
     * @see CameraDevice.CaptureCallback#onCaptureSequenceCompleted
     */
    CARAPI GetSequenceId(
        /* [out] */  Int32* value);

private:
    AutoPtr<ICaptureRequest> mRequest;
    Int32 mReason;
    Boolean mDropped;
    Int32 mSequenceId;
    Int64 mFrameNumber;
};

} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_CAPTUREFAILURE_H__
