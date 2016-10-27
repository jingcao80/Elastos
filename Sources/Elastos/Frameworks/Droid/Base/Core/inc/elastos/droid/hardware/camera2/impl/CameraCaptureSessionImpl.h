
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CAMERACAPTURESESSIONIMPL_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CAMERACAPTURESESSIONIMPL_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/hardware/camera2/CameraCaptureSession.h"
#include "elastos/droid/hardware/camera2/impl/CameraDeviceImpl.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Camera2::Utils::ITaskDrainer;
using Elastos::Droid::Hardware::Camera2::Utils::ITaskSingleDrainer;
using Elastos::Droid::Hardware::Camera2::Utils::ITaskDrainerDrainListener;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Impl {

class CameraCaptureSessionImpl
    : public CameraCaptureSession
    , public ICameraCaptureSessionImpl
{
private:
    class SequenceDrainListener
        : public Object
        , public ITaskDrainerDrainListener
    {
    public:
        CAR_INTERFACE_DECL()

        SequenceDrainListener(
            /* [in] */ CameraCaptureSessionImpl* host);

        //@Override
        CARAPI OnDrained();

    private:
        CameraCaptureSessionImpl* mHost;
    };

    class AbortDrainListener
        : public Object
        , public ITaskDrainerDrainListener
    {
    public:
        CAR_INTERFACE_DECL()

        AbortDrainListener(
            /* [in] */ CameraCaptureSessionImpl* host);

        //@Override
        CARAPI OnDrained();

    private:
        CameraCaptureSessionImpl* mHost;
    };

    class IdleDrainListener
        : public Object
        , public ITaskDrainerDrainListener
    {
    public:
        CAR_INTERFACE_DECL()

        IdleDrainListener(
            /* [in] */ CameraCaptureSessionImpl* host);

        //@Override
        CARAPI OnDrained();

    private:
        CameraCaptureSessionImpl* mHost;
    };

    class UnconfigureDrainListener
        : public Object
        , public ITaskDrainerDrainListener
    {
    public:
        CAR_INTERFACE_DECL()

        UnconfigureDrainListener(
            /* [in] */ CameraCaptureSessionImpl* host);

        //@Override
        CARAPI OnDrained();

    private:
        CameraCaptureSessionImpl* mHost;
    };

    class MyStateCallbackKK
        : public CameraDeviceImpl::StateCallbackKK
    {
    public:
        TO_STRING_IMPL("CameraCaptureSessionImpl::MyStateCallbackKK")

        MyStateCallbackKK(
            /* [in] */ CameraCaptureSessionImpl* host,
            /* [in] */ ICameraCaptureSession* session);

        //@Override
        CARAPI OnOpened(
            /* [in] */ ICameraDevice* camera);

        //@Override
        CARAPI OnDisconnected(
            /* [in] */ ICameraDevice* camera);

        //@Override
        CARAPI OnError(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ Int32 error);

        //@Override
        CARAPI OnActive(
            /* [in] */ ICameraDevice* camera);

        //@Override
        CARAPI OnIdle(
            /* [in] */ ICameraDevice* camera);

        //@Override
        CARAPI OnBusy(
            /* [in] */ ICameraDevice* camera);

        //@Override
        CARAPI OnUnconfigured(
            /* [in] */ ICameraDevice* camera);

    private:
        Boolean mBusy;
        Boolean mActive;
        CameraCaptureSessionImpl* mHost;
        ICameraCaptureSession* mSession;
    };

    class MyCaptureCallback
        : public CameraDeviceImpl::CaptureCallback
    {
    public:
        TO_STRING_IMPL("CameraCaptureSessionImpl::MyCaptureCallback")

        MyCaptureCallback(
            /* [in] */ CameraCaptureSessionImpl* host);

        //@Override
        CARAPI OnCaptureSequenceCompleted(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ Int32 sequenceId,
            /* [in] */ Int64 frameNumber);

        //@Override
        CARAPI OnCaptureSequenceAborted(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ Int32 sequenceId);

    private:
        CameraCaptureSessionImpl* mHost;
    };

    class SessionCaptureCallbackWrapper
        : public CameraDeviceImpl::CaptureCallback
    {
    public:
        TO_STRING_IMPL("CameraCaptureSessionImpl::SessionCaptureCallbackWrapper")

        SessionCaptureCallbackWrapper(
            /* [in] */ CameraCaptureSessionImpl* host,
            /* [in] */ ICameraCaptureSessionCaptureCallback* callback);

        CARAPI OnCaptureStarted(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ Int64 timestamp,
            /* [in] */ Int64 frameNumber);

        CARAPI OnCapturePartial(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ICaptureResult* result);

        CARAPI OnCaptureProgressed(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ICaptureResult* partialResult);

        CARAPI OnCaptureCompleted(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ITotalCaptureResult* result);

        CARAPI OnCaptureFailed(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ICaptureFailure* failure);

        CARAPI OnCaptureSequenceCompleted(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ Int32 sequenceId,
            /* [in] */ Int64 frameNumber);

        CARAPI OnCaptureSequenceAborted(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ Int32 sequenceId);

    private:
        CameraCaptureSessionImpl* mHost;
        AutoPtr<ICameraCaptureSessionCaptureCallback> mSessionCaptureCallback;
    };

public:
    CAR_INTERFACE_DECL()

    CameraCaptureSessionImpl();

    virtual ~CameraCaptureSessionImpl();

    CARAPI constructor();

    /**
     * Create a new CameraCaptureSession.
     *
     * <p>The camera device must already be in the {@code IDLE} state when this is invoked.
     * There must be no pending actions
     * (e.g. no pending captures, no repeating requests, no flush).</p>
     */
    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ IList* outputs,
        /* [in] */ ICameraCaptureSessionStateCallback* callback,
        /* [in] */ IHandler* stateHandler,
        /* [in] */ ICameraDeviceImpl* deviceImpl,
        /* [in] */ IHandler* deviceStateHandler,
        /* [in] */ Boolean configureSuccess);

    //@Override
    CARAPI GetDevice(
        /* [out] */ ICameraDevice** outcd);

    //@Override
    CARAPI Capture(
        /* [in] */ ICaptureRequest* request,
        /* [in] */ ICameraCaptureSessionCaptureCallback* listener,
        /* [in] */ IHandler* handler,
        /* [out] */ Int32* value);

    //@Override
    CARAPI CaptureBurst(
        /* [in] */ IList* requests,
        /* [in] */ ICameraCaptureSessionCaptureCallback* listener,
        /* [in] */ IHandler* handler,
        /* [out] */ Int32* value);

    //@Override
    CARAPI SetRepeatingRequest(
        /* [in] */ ICaptureRequest* request,
        /* [in] */ ICameraCaptureSessionCaptureCallback* listener,
        /* [in] */ IHandler* handler,
        /* [out] */ Int32* value);

    //@Override
    CARAPI SetRepeatingBurst(
        /* [in] */ IList* requests,
        /* [in] */ ICameraCaptureSessionCaptureCallback* listener,
        /* [in] */ IHandler* handler,
        /* [out] */ Int32* value);

    //@Override
    CARAPI StopRepeating();

    //@Override
    CARAPI AbortCaptures();

    /**
     * Replace this session with another session.
     *
     * <p>This is an optimization to avoid unconfiguring and then immediately having to
     * reconfigure again.</p>
     *
     * <p>The semantics are identical to {@link #close}, except that unconfiguring will be skipped.
     * <p>
     *
     * <p>After this call completes, the session will not call any further methods on the camera
     * device.</p>
     *
     * @see CameraCaptureSession#close
     */
    CARAPI ReplaceSessionClose();

    //@Override
    CARAPI Close();

    /**
     * Whether currently in mid-abort.
     *
     * <p>This is used by the implementation to set the capture failure
     * reason, in lieu of more accurate error codes from the camera service.
     * Unsynchronized to avoid deadlocks between simultaneous session->device,
     * device->session calls.</p>
     *
     * <p>Package-private.</p>
     */
    CARAPI IsAborting(
        /* [out] */ Boolean* result);

private:
    CARAPI CreateUserStateCallbackProxy(
        /* [in] */ IHandler* handler,
        /* [in] */ ICameraCaptureSessionStateCallback* callback,
        /* [out] */ ICameraCaptureSessionStateCallback** back);

    /**
     * Forward callbacks from
     * CameraDeviceImpl.CaptureCallback to the CameraCaptureSession.CaptureCallback.
     *
     * <p>In particular, all calls are automatically split to go both to our own
     * internal callback, and to the user-specified callback (by transparently posting
     * to the user-specified handler).</p>
     *
     * <p>When a capture sequence finishes, update the pending checked sequences set.</p>
     */
    //@SuppressWarnings("deprecation")
    CARAPI CreateCaptureCallbackProxy(
        /* [in] */ IHandler* handler,
        /* [in] */ ICameraCaptureSessionCaptureCallback* callback,
        /* [out] */ ICameraDeviceImplCaptureCallback** back);

    /**
     *
     * Create an internal state callback, to be invoked on the mDeviceHandler
     *
     * <p>It has a few behaviors:
     * <ul>
     * <li>Convert device state changes into session state changes.
     * <li>Keep track of async tasks that the session began (idle, abort).
     * </ul>
     * </p>
     * */
    CARAPI GetDeviceStateCallback(
        /* [out] */ ICameraDeviceImplStateCallbackKK** back);

    CARAPI CheckNotClosed();

    /**
     * Notify the session that a pending capture sequence has just been queued.
     *
     * <p>During a shutdown/close, the session waits until all pending sessions are finished
     * before taking any further steps to shut down itself.</p>
     *
     * @see #finishPendingSequence
     */
    CARAPI AddPendingSequence(
        /* [in] */ Int32 sequenceId,
        /* [out] */ Int32* result);

    /**
     * Notify the session that a pending capture sequence is now finished.
     *
     * <p>During a shutdown/close, once all pending sequences finish, it is safe to
     * close the camera further by unconfiguring and then firing {@code onClosed}.</p>
     */
    CARAPI FinishPendingSequence(
        /* [in] */ Int32 sequenceId);

private:
    static const String TAG;
    static const Boolean VERBOSE;

    /** Simple integer ID for session for debugging */
    Int32 mId;
    String mIdString;

    /** User-specified set of surfaces used as the configuration outputs */
    AutoPtr<IList> mOutputs;
    /**
     * User-specified state callback, used for outgoing events; calls to this object will be
     * automatically {@link Handler#post(Runnable) posted} to {@code mStateHandler}.
     */
    AutoPtr<ICameraCaptureSessionStateCallback> mStateCallback;
    /** User-specified state handler used for outgoing state callback events */
    AutoPtr<IHandler> mStateHandler;

    /** Internal camera device; used to translate calls into existing deprecated API */
    AutoPtr<ICameraDeviceImpl> mDeviceImpl;
    /** Internal handler; used for all incoming events to preserve total order */
    AutoPtr<IHandler> mDeviceHandler;

    /** Drain Sequence IDs which have been queued but not yet finished with aborted/completed */
    AutoPtr<ITaskDrainer> mSequenceDrainer;
    /** Drain state transitions from ACTIVE -> IDLE */
    AutoPtr<ITaskSingleDrainer> mIdleDrainer;
    /** Drain state transitions from BUSY -> IDLE */
    AutoPtr<ITaskSingleDrainer> mAbortDrainer;
    /** Drain the UNCONFIGURED state transition */
    AutoPtr<ITaskSingleDrainer> mUnconfigureDrainer;

    /** This session is closed; all further calls will throw ISE */
    Boolean mClosed;
    /** This session failed to be configured successfully */
    Boolean mConfigureSuccess;
    /** Do not unconfigure if this is set; another session will overwrite configuration */
    Boolean mSkipUnconfigure;

    /** Is the session in the process of aborting? Pay attention to BUSY->IDLE transitions. */
    Boolean mAborting;
};

} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CAMERACAPTURESESSIONIMPL_H__
