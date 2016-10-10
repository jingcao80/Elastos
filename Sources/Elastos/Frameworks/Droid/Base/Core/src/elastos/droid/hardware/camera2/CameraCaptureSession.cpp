
#include "elastos/droid/hardware/camera2/CameraCaptureSession.h"

using Elastos::IO::EIID_ICloseable;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CAR_INTERFACE_IMPL(CameraCaptureSession::StateCallback, Object, ICameraCaptureSessionStateCallback)

ECode CameraCaptureSession::StateCallback::OnReady(
    /* [in] */ ICameraCaptureSession* session)
{
    // default empty implementation
    return NOERROR;
}

ECode CameraCaptureSession::StateCallback::OnActive(
    /* [in] */ ICameraCaptureSession* session)
{
    // default empty implementation
    return NOERROR;
}

ECode CameraCaptureSession::StateCallback::OnClosed(
    /* [in] */ ICameraCaptureSession* session)
{
    // default empty implementation
    return NOERROR;
}

CAR_INTERFACE_IMPL(CameraCaptureSession::StateListener, StateCallback, ICameraCaptureSessionStateListener)

CAR_INTERFACE_IMPL(CameraCaptureSession::CaptureCallback, Object, ICameraCaptureSessionCaptureCallback)

ECode CameraCaptureSession::CaptureCallback::OnCaptureStarted(
    /* [in] */ ICameraCaptureSession* session,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ Int64 timestamp,
    /* [in] */ Int64 frameNumber)
{
    // Temporary trampoline for API change transition
    return OnCaptureStarted(session, request, timestamp);
}

ECode CameraCaptureSession::CaptureCallback::OnCaptureStarted(
    /* [in] */ ICameraCaptureSession* session,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ Int64 timestamp)
{
    // default empty implementation
    return NOERROR;
}

ECode CameraCaptureSession::CaptureCallback::OnCapturePartial(
    /* [in] */ ICameraCaptureSession* session,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICaptureResult* result)
{
    // default empty implementation
    return NOERROR;
}

ECode CameraCaptureSession::CaptureCallback::OnCaptureProgressed(
    /* [in] */ ICameraCaptureSession* session,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICaptureResult* partialResult)
{
    // default empty implementation
    return NOERROR;
}

ECode CameraCaptureSession::CaptureCallback::OnCaptureCompleted(
    /* [in] */ ICameraCaptureSession* session,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ITotalCaptureResult* result)
{
    // default empty implementation
    return NOERROR;
}

ECode CameraCaptureSession::CaptureCallback::OnCaptureFailed(
    /* [in] */ ICameraCaptureSession* session,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICaptureFailure* failure)
{
    // default empty implementation
    return NOERROR;
}

ECode CameraCaptureSession::CaptureCallback::OnCaptureSequenceCompleted(
    /* [in] */ ICameraCaptureSession* session,
    /* [in] */ Int32 sequenceId,
    /* [in] */ Int64 frameNumber)
{
    // default empty implementation
    return NOERROR;
}

ECode CameraCaptureSession::CaptureCallback::OnCaptureSequenceAborted(
    /* [in] */ ICameraCaptureSession* session,
    /* [in] */ Int32 sequenceId)
{
    // default empty implementation
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CameraCaptureSession, Object, ICameraCaptureSession, ICloseable)

} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos