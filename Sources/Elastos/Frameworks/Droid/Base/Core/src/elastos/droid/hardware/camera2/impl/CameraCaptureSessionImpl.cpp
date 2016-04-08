
#include "elastos/droid/hardware/camera2/impl/CameraCaptureSessionImpl.h"
#include "elastos/droid/hardware/camera2/impl/CCallbackProxiesSessionStateCallbackProxy.h"
#include "elastos/droid/hardware/camera2/impl/CCallbackProxiesDeviceCaptureCallbackProxy.h"
#include "elastos/droid/hardware/camera2/dispatch/CInvokeDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CHandlerDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CDuckTypingDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CArgumentReplacingDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CBroadcastDispatcher.h"
#include "elastos/droid/hardware/camera2/utils/CTaskDrainer.h"
#include "elastos/droid/hardware/camera2/utils/CTaskSingleDrainer.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Camera2::Impl::CCallbackProxiesSessionStateCallbackProxy;
using Elastos::Droid::Hardware::Camera2::Impl::CCallbackProxiesDeviceCaptureCallbackProxy;
using Elastos::Droid::Hardware::Camera2::Utils::CTaskDrainer;
using Elastos::Droid::Hardware::Camera2::Utils::CTaskSingleDrainer;
using Elastos::Droid::Hardware::Camera2::Utils::EIID_ITaskDrainerDrainListener;
using Elastos::Droid::Hardware::Camera2::Dispatch::IDispatchable;
using Elastos::Droid::Hardware::Camera2::Dispatch::CInvokeDispatcher;
using Elastos::Droid::Hardware::Camera2::Dispatch::IInvokeDispatcher;
using Elastos::Droid::Hardware::Camera2::Dispatch::CHandlerDispatcher;
using Elastos::Droid::Hardware::Camera2::Dispatch::IHandlerDispatcher;
using Elastos::Droid::Hardware::Camera2::Dispatch::CDuckTypingDispatcher;
using Elastos::Droid::Hardware::Camera2::Dispatch::IDuckTypingDispatcher;
using Elastos::Droid::Hardware::Camera2::Dispatch::CArgumentReplacingDispatcher;
using Elastos::Droid::Hardware::Camera2::Dispatch::IArgumentReplacingDispatcher;
using Elastos::Droid::Hardware::Camera2::Dispatch::CBroadcastDispatcher;
using Elastos::Droid::Hardware::Camera2::Dispatch::IBroadcastDispatcher;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Core::StringBuilder;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Impl {

CAR_INTERFACE_IMPL(CameraCaptureSessionImpl::SequenceDrainListener, Object, ITaskDrainerDrainListener)

CameraCaptureSessionImpl::SequenceDrainListener::SequenceDrainListener(
    /* [in] */ CameraCaptureSessionImpl* host)
    : mHost(host)
{
}

ECode CameraCaptureSessionImpl::SequenceDrainListener::OnDrained()
{
    /*
     * No repeating request is set; and the capture queue has fully drained.
     *
     * If no captures were queued to begin with, and an abort was queued,
     * it's still possible to get another BUSY before the last IDLE.
     *
     * If the camera is already "IDLE" and no aborts are pending,
     * then the drain immediately finishes.
     */
    if (mHost->VERBOSE) {
        Slogger::V(mHost->TAG, "%s onSequenceDrained", mHost->mIdString.string());
    }
    return mHost->mAbortDrainer->BeginDrain();
}

CAR_INTERFACE_IMPL(CameraCaptureSessionImpl::AbortDrainListener, Object, ITaskDrainerDrainListener)

CameraCaptureSessionImpl::AbortDrainListener::AbortDrainListener(
    /* [in] */ CameraCaptureSessionImpl* host)
    : mHost(host)
{
}

ECode CameraCaptureSessionImpl::AbortDrainListener::OnDrained()
{
    if (mHost->VERBOSE) {
        Slogger::V(mHost->TAG, "%s onAbortDrained", mHost->mIdString.string());
    }
    synchronized(mHost) {
        /*
         * Any queued aborts have now completed.
         *
         * It's now safe to wait to receive the final "IDLE" event, as the camera device
         * will no longer again transition to "ACTIVE" by itself.
         *
         * If the camera is already "IDLE", then the drain immediately finishes.
         */
        return mHost->mIdleDrainer->BeginDrain();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CameraCaptureSessionImpl::IdleDrainListener, Object, ITaskDrainerDrainListener)

CameraCaptureSessionImpl::IdleDrainListener::IdleDrainListener(
    /* [in] */ CameraCaptureSessionImpl* host)
    : mHost(host)
{
}

ECode CameraCaptureSessionImpl::IdleDrainListener::OnDrained()
{
    if (mHost->VERBOSE) {
        Slogger::V(mHost->TAG, "%s onIdleDrained", mHost->mIdString.string());
    }

    // Take device lock before session lock so that we can call back into device
    // without causing a deadlock
    CameraDeviceImpl* impl = (CameraDeviceImpl*)(mHost->mDeviceImpl.Get());
    Object& lock = impl->mInterfaceLock;
    synchronized(lock) {
        synchronized(mHost) {
            /*
             * The device is now IDLE, and has settled. It will not transition to
             * ACTIVE or BUSY again by itself.
             *
             * It's now safe to unconfigure the outputs and after it's done invoke #onClosed.
             *
             * This operation is idempotent; a session will not be closed twice.
             */
            if (mHost->VERBOSE)
                Slogger::V(mHost->TAG, "%s Session drain complete, skip unconfigure: %d"
                        , mHost->mIdString.string(), mHost->mSkipUnconfigure);

            // Fast path: A new capture session has replaced this one; don't unconfigure.
            if (mHost->mSkipUnconfigure) {
                return mHost->mStateCallback->OnClosed(mHost);
            }

            // Slow path: #close was called explicitly on this session; unconfigure first
            mHost->mUnconfigureDrainer->TaskStarted();

            //try {
            Boolean tmp;
            ECode ec = mHost->mDeviceImpl->ConfigureOutputsChecked(NULL, &tmp); // begin transition to unconfigured
            //} catch (CameraAccessException e) {
            if (ec == (ECode)E_CAMERA_ACCESS_EXCEPTION) {
                // OK: do not throw checked exceptions.
                Slogger::E(TAG, "%s Exception while configuring outputs: %d", mHost->mIdString.string(), ec);

                // TODO: call onError instead of onClosed if this happens
            }
            //} catch (IllegalStateException e) {
            if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
                // Camera is already closed, so go straight to the close callback
                if (mHost->VERBOSE) Slogger::V(mHost->TAG,
                        "%s Camera was already closed or busy, skipping unconfigure", mHost->mIdString.string());
                mHost->mUnconfigureDrainer->TaskFinished();
            }
            //}

            return mHost->mUnconfigureDrainer->BeginDrain();
        }
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CameraCaptureSessionImpl::UnconfigureDrainListener, Object, ITaskDrainerDrainListener)

CameraCaptureSessionImpl::UnconfigureDrainListener::UnconfigureDrainListener(
    /* [in] */ CameraCaptureSessionImpl* host)
    : mHost(host)
{
}

ECode CameraCaptureSessionImpl::UnconfigureDrainListener::OnDrained()
{
    if (mHost->VERBOSE) {
        Slogger::V(TAG, "%s onUnconfigureDrained", mHost->mIdString.string());
    }
    synchronized(mHost) {
        // The device has finished unconfiguring. It's now fully closed.
        return mHost->mStateCallback->OnClosed(mHost);
    }
    return NOERROR;
}

CameraCaptureSessionImpl::MyStateCallbackKK::MyStateCallbackKK(
    /* [in] */ CameraCaptureSessionImpl* host,
    /* [in] */ ICameraCaptureSession* session)
    : mBusy(FALSE)
    , mActive(FALSE)
    , mHost(host)
    , mSession(session)

{
}

ECode CameraCaptureSessionImpl::MyStateCallbackKK::OnOpened(
    /* [in] */ ICameraDevice* camera)
{
    //throw new AssertionError("Camera must already be open before creating a session");
    Slogger::E(mHost->TAG, "Camera must already be open before creating a session");
    return E_ASSERTION_ERROR;
}


ECode CameraCaptureSessionImpl::MyStateCallbackKK::OnDisconnected(
    /* [in] */ ICameraDevice* camera)
{
    if (mHost->VERBOSE) {
        Slogger::V(mHost->TAG, "%s, onDisconnected",mHost->mIdString.string());
    }
    return mHost->Close();
}


ECode CameraCaptureSessionImpl::MyStateCallbackKK::OnError(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ Int32 error)
{
    // Should not be reached, handled by device code
    Slogger::W(mHost->TAG, "%s Got device error %d", mHost->mIdString.string(), error);
    return NOERROR;
}


ECode CameraCaptureSessionImpl::MyStateCallbackKK::OnActive(
    /* [in] */ ICameraDevice* camera)
{
    mHost->mIdleDrainer->TaskStarted();
    mActive = TRUE;

    if (mHost->VERBOSE) {
        Slogger::V(mHost->TAG, "%s onActive", mHost->mIdString.string());
    }
    return mHost->mStateCallback->OnActive(mSession);
}


ECode CameraCaptureSessionImpl::MyStateCallbackKK::OnIdle(
    /* [in] */ ICameraDevice* camera)
{
    Boolean isAborting = FALSE;
    if (mHost->VERBOSE) {
        Slogger::V(TAG, "%s onIdle", mHost->mIdString.string());
    }

    synchronized (mSession) {
        isAborting = mHost->mAborting;
    }

    /*
     * Check which states we transitioned through:
     *
     * (ACTIVE -> IDLE)
     * (BUSY -> IDLE)
     *
     * Note that this is also legal:
     * (ACTIVE -> BUSY -> IDLE)
     *
     * and mark those tasks as finished
     */
    if (mBusy && isAborting) {
        mHost->mAbortDrainer->TaskFinished();

        synchronized (mSession) {
            mHost->mAborting = FALSE;
        }
    }

    if (mActive) {
        mHost->mIdleDrainer->TaskFinished();
    }

    mBusy = FALSE;
    mActive = FALSE;

    return mHost->mStateCallback->OnReady(mSession);
}


ECode CameraCaptureSessionImpl::MyStateCallbackKK::OnBusy(
    /* [in] */ ICameraDevice* camera)
{
    mBusy = TRUE;

    // TODO: Queue captures during abort instead of failing them
    // since the app won't be able to distinguish the two actives
    // Don't signal the application since there's no clean mapping here
    if (mHost->VERBOSE) {
        Slogger::V(mHost->TAG, "%s onBusy", mHost->mIdString.string());
    }
    return NOERROR;
}


ECode CameraCaptureSessionImpl::MyStateCallbackKK::OnUnconfigured(
    /* [in] */ ICameraDevice* camera)
{
    if (mHost->VERBOSE) {
        Slogger::V(mHost->TAG, "%d onUnconfigured", mHost->mIdString.string());
    }
    synchronized (mSession) {
        // Ignore #onUnconfigured before #close is called.
        //
        // Normally, this is reached when this session is closed and no immediate other
        // activity happens for the camera, in which case the camera is configured to
        // null streams by this session and the UnconfigureDrainer task is started.
        // However, we can also end up here if
        //
        // 1) Session is closed
        // 2) New session is created before this session finishes closing, setting
        //    mSkipUnconfigure and therefore this session does not configure null or
        //    start the UnconfigureDrainer task.
        // 3) And then the new session fails to be created, so onUnconfigured fires
        //    _anyway_.
        // In this second case, need to not finish a task that was never started, so
        // guard with mSkipUnconfigure
        if (mHost->mClosed && mHost->mConfigureSuccess && !mHost->mSkipUnconfigure) {
            return mHost->mUnconfigureDrainer->TaskFinished();
        }
    }
    return NOERROR;
}

CameraCaptureSessionImpl::MyCaptureCallback::MyCaptureCallback(
    /* [in] */ CameraCaptureSessionImpl* host)
    : mHost(host)
{
}

CameraCaptureSessionImpl::MyCaptureCallback::OnCaptureSequenceCompleted(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ Int32 sequenceId,
    /* [in] */ Int64 frameNumber)
{
    return mHost->FinishPendingSequence(sequenceId);
}

CameraCaptureSessionImpl::MyCaptureCallback::OnCaptureSequenceAborted(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ Int32 sequenceId)
{
    return mHost->FinishPendingSequence(sequenceId);
}

const String CameraCaptureSessionImpl::TAG("CameraCaptureSession");
const Boolean CameraCaptureSessionImpl::VERBOSE = FALSE;//Log.isLoggable(TAG, Log.VERBOSE);

CAR_INTERFACE_IMPL(CameraCaptureSessionImpl, CameraCaptureSession, ICameraCaptureSessionImpl)

CameraCaptureSessionImpl::CameraCaptureSessionImpl()
    : mId(0)
    , mIdString(NULL)
    , mClosed(FALSE)
    , mConfigureSuccess(FALSE)
    , mSkipUnconfigure(FALSE)
    , mAborting(FALSE)
{
}

CameraCaptureSessionImpl::~CameraCaptureSessionImpl()
{
    //try {
    Close();
    //} finally {
    //    super.finalize();
    //}
}

ECode CameraCaptureSessionImpl::constructor()
{
    return NOERROR;
}

ECode CameraCaptureSessionImpl::constructor(
    /* [in] */ Int32 id,
    /* [in] */ IList* outputs,
    /* [in] */ ICameraCaptureSessionStateCallback* _callback,
    /* [in] */ IHandler* stateHandler,
    /* [in] */ ICameraDeviceImpl* deviceImpl,
    /* [in] */ IHandler* deviceStateHandler,
    /* [in] */ Boolean configureSuccess)
{
    Boolean res;
    if (outputs == NULL || (outputs->IsEmpty(&res), res)) {
        //throw new IllegalArgumentException("outputs must be a non-null, non-empty list");
        Slogger::E(TAG, "outputs must be a non-null, non-empty list");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (_callback == NULL) {
        //throw new IllegalArgumentException("callback must not be null");
        Slogger::E(TAG, "callback must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mId = id;
    StringBuilder sb;
    sb += "Session ";
    sb += mId;
    sb.ToString(&mIdString);

    // TODO: extra verification of outputs
    mOutputs = outputs;
    CameraDeviceImpl::CheckHandler(stateHandler, (IHandler**)&mStateHandler);
    CreateUserStateCallbackProxy(mStateHandler, _callback, (ICameraCaptureSessionStateCallback**)&mStateCallback);

    FAIL_RETURN(Preconditions::CheckNotNull(deviceStateHandler, String("deviceStateHandler must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(deviceImpl, String("deviceImpl must not be null")))
    mDeviceHandler = deviceStateHandler;
    mDeviceImpl = deviceImpl;

    /*
     * Use the same handler as the device's StateCallback for all the internal coming events
     *
     * This ensures total ordering between CameraDevice.StateCallback and
     * CameraDeviceImpl.CaptureCallback events.
     */
    AutoPtr<SequenceDrainListener> sequenceDrainListener = new SequenceDrainListener(this);
    CTaskDrainer::New(mDeviceHandler, sequenceDrainListener,
            /*name*/String("seq"), (ITaskDrainer**)&mSequenceDrainer);

    AutoPtr<IdleDrainListener> idleDrainListener = new IdleDrainListener(this);
    CTaskSingleDrainer::New(mDeviceHandler, idleDrainListener,
            /*name*/String("idle"), (ITaskSingleDrainer**)&mIdleDrainer);

    AutoPtr<AbortDrainListener> abortDrainListener = new AbortDrainListener(this);
    CTaskSingleDrainer::New(mDeviceHandler, abortDrainListener,
            /*name*/String("abort"), (ITaskSingleDrainer**)&mAbortDrainer);

    AutoPtr<UnconfigureDrainListener> unconfigureDrainListener = new UnconfigureDrainListener(this);
    CTaskSingleDrainer::New(mDeviceHandler, unconfigureDrainListener,
            /*name*/String("unconf"), (ITaskSingleDrainer**)&mUnconfigureDrainer);

    // CameraDevice should call configureOutputs and have it finish before constructing us

    if (configureSuccess) {
        mStateCallback->OnConfigured(this);
        if (VERBOSE) Slogger::V(TAG, "%s Created session successfully", mIdString.string());
        mConfigureSuccess = TRUE;
    }
    else {
        mStateCallback->OnConfigureFailed(this);
        mClosed = TRUE; // do not fire any other callbacks, do not allow any other work
        Slogger::E(TAG, "%s Failed to create capture session; configuration failed", mIdString.string());
        mConfigureSuccess = FALSE;
    }
    return NOERROR;
}

ECode CameraCaptureSessionImpl::GetDevice(
    /* [out] */ ICameraDevice** outcd)
{
    VALIDATE_NOT_NULL(outcd)

    *outcd = ICameraDevice::Probe(mDeviceImpl);
    REFCOUNT_ADD(*outcd);
    return NOERROR;
}

ECode CameraCaptureSessionImpl::Capture(
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICameraCaptureSessionCaptureCallback* _callback,
    /* [in] */ IHandler* handler,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    synchronized(this) {
        if (request == NULL) {
            //throw new IllegalArgumentException("request must not be null");
            Slogger::E(TAG, "request must not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        FAIL_RETURN(CheckNotClosed())

        AutoPtr<IHandler> newHandler;
        CameraDeviceImpl::CheckHandler(handler, TO_IINTERFACE(_callback), (IHandler**)&newHandler);

        if (VERBOSE) {
            // Slogger::V(TAG, mIdString + "capture - request " + request + ", callback " + _callback +
            //         " newHandler " + newHandler);
        }

        AutoPtr<ICameraDeviceImplCaptureCallback> back;
        CreateCaptureCallbackProxy(newHandler, _callback, (ICameraDeviceImplCaptureCallback**)&back);
        Int32 _value;
        FAIL_RETURN(mDeviceImpl->Capture(request, back, mDeviceHandler, &_value))
        return AddPendingSequence(_value, value);
    }
    return NOERROR;
}

ECode CameraCaptureSessionImpl::CaptureBurst(
    /* [in] */ IList* requests,
    /* [in] */ ICameraCaptureSessionCaptureCallback* _callback,
    /* [in] */ IHandler* handler,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    synchronized(this) {
        Boolean res;
        if (requests == NULL) {
            //throw new IllegalArgumentException("requests must not be null");
            Slogger::E(TAG, "requests must not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        else if (requests->IsEmpty(&res), res) {
            //throw new IllegalArgumentException("requests must have at least one element");
            Slogger::E(TAG, "requests must have at least one element");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        FAIL_RETURN(CheckNotClosed())

         AutoPtr<IHandler> newHandler;
        CameraDeviceImpl::CheckHandler(handler, _callback, (IHandler**)&newHandler);

        if (VERBOSE) {
            // CaptureRequest[] requestArray = requests.toArray(new CaptureRequest[0]);
            // Log.v(TAG, mIdString + "captureBurst - requests " + Arrays.toString(requestArray) +
            //         ", callback " + _callback + " newHandler " + newHandler);
        }

        AutoPtr<ICameraDeviceImplCaptureCallback> back;
        CreateCaptureCallbackProxy(newHandler, _callback, (ICameraDeviceImplCaptureCallback**)&back);
        Int32 _value;
        FAIL_RETURN(mDeviceImpl->CaptureBurst(requests, back, mDeviceHandler, &_value))
        return AddPendingSequence(_value, value);
    }
    return NOERROR;
}

ECode CameraCaptureSessionImpl::SetRepeatingRequest(
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICameraCaptureSessionCaptureCallback* _callback,
    /* [in] */ IHandler* handler,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    synchronized(this) {
        if (request == NULL) {
            //throw new IllegalArgumentException("request must not be null");
            Slogger::E(TAG, "request must not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        FAIL_RETURN(CheckNotClosed())

        AutoPtr<IHandler> tmp = handler;
        CameraDeviceImpl::CheckHandler(handler, _callback, (IHandler**)handler);

        if (VERBOSE) {
            // Log.v(TAG, mIdString + "setRepeatingRequest - request " + request + ", callback " +
            //         _callback + " handler" + " " + handler);
        }

        AutoPtr<ICameraDeviceImplCaptureCallback> back;
        CreateCaptureCallbackProxy(handler, _callback, (ICameraDeviceImplCaptureCallback**)&back);
        Int32 _value;
        FAIL_RETURN(mDeviceImpl->SetRepeatingRequest(request, back, mDeviceHandler, &_value))
        return AddPendingSequence(_value, value);
    }
    return NOERROR;
}

ECode CameraCaptureSessionImpl::SetRepeatingBurst(
    /* [in] */ IList* requests,
    /* [in] */ ICameraCaptureSessionCaptureCallback* _callback,
    /* [in] */ IHandler* handler,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    synchronized(this) {
        Boolean res;
        if (requests == NULL) {
            //throw new IllegalArgumentException("requests must not be null");
            Slogger::E(TAG, "requests must not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        else if (requests->IsEmpty(&res), res) {
            //throw new IllegalArgumentException("requests must have at least one element");
            Slogger::E(TAG, "requests must have at least one element");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        FAIL_RETURN(CheckNotClosed())

        CameraDeviceImpl::CheckHandler(handler, _callback, (IHandler**)handler);

        if (VERBOSE) {
            // CaptureRequest[] requestArray = requests.toArray(new CaptureRequest[0]);
            // Log.v(TAG, mIdString + "setRepeatingBurst - requests " + Arrays.toString(requestArray) +
            //         ", callback " + _callback + " handler" + "" + handler);
        }

        AutoPtr<ICameraDeviceImplCaptureCallback> back;
        CreateCaptureCallbackProxy(handler, _callback, (ICameraDeviceImplCaptureCallback**)&back);
        Int32 _value;
        FAIL_RETURN(mDeviceImpl->SetRepeatingBurst(requests, back, mDeviceHandler, &_value))
        return AddPendingSequence(_value, value);
    }
    return NOERROR;
}

ECode CameraCaptureSessionImpl::StopRepeating()
{
    synchronized(this) {
        FAIL_RETURN(CheckNotClosed())

        if (VERBOSE) {
            Slogger::V(TAG, "%s stopRepeating", mIdString.string());
        }

        return mDeviceImpl->StopRepeating();
    }
    return NOERROR;
}

ECode CameraCaptureSessionImpl::AbortCaptures()
{
    synchronized(this) {
        FAIL_RETURN(CheckNotClosed())

        if (VERBOSE) {
            Slogger::V(TAG, "%s abortCaptures", mIdString.string());
        }

        if (mAborting) {
            Slogger::W(TAG, "%s abortCaptures - Session is already aborting; doing nothing", mIdString.string());
            return NOERROR;
        }

        mAborting = TRUE;
        mAbortDrainer->TaskStarted();

        return mDeviceImpl->Flush();
        // The next BUSY -> IDLE set of transitions will mark the end of the abort.
    }
    return NOERROR;
}

ECode CameraCaptureSessionImpl::ReplaceSessionClose()
{
    synchronized(this) {
        /*
         * In order for creating new sessions to be fast, the new session should be created
         * before the old session is closed.
         *
         * Otherwise the old session will always unconfigure if there is no new session to
         * replace it.
         *
         * Unconfiguring could add hundreds of milliseconds of delay. We could race and attempt
         * to skip unconfigure if a new session is created before the captures are all drained,
         * but this would introduce nondeterministic behavior.
         */

        if (VERBOSE) {
            Slogger::V(TAG, "%s replaceSessionClose", mIdString.string());
        }

        // Set up fast shutdown. Possible alternative paths:
        // - This session is active, so close() below starts the shutdown drain
        // - This session is mid-shutdown drain, and hasn't yet reached the idle drain listener.
        // - This session is already closed and has executed the idle drain listener, and
        //   configureOutputsChecked(null) has already been called.
        //
        // Do not call configureOutputsChecked(null) going forward, since it would race with the
        // configuration for the new session. If it was already called, then we don't care, since it
        // won't get called again.
        mSkipUnconfigure = TRUE;

        return Close();
    }
    return NOERROR;
}

ECode CameraCaptureSessionImpl::Close()
{
    synchronized(this) {
        if (mClosed) {
            if (VERBOSE) {
                Slogger::V(TAG, "%s close - reentering", mIdString.string());
            }
            return NOERROR;
        }

        if (VERBOSE) {
            Slogger::V(TAG, "%s close - first time", mIdString.string());
        }

        mClosed = TRUE;

        /*
         * Flush out any repeating request. Since camera is closed, no new requests
         * can be queued, and eventually the entire request queue will be drained.
         *
         * If the camera device was already closed, short circuit and do nothing; since
         * no more internal device callbacks will fire anyway.
         *
         * Otherwise, once stopRepeating is done, wait for camera to idle, then unconfigure the
         * camera. Once that's done, fire #onClosed.
         */
        //try {
        ECode ec = mDeviceImpl->StopRepeating();
        //} catch (IllegalStateException e) {
        if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
            // OK: Camera device may already be closed, nothing else to do
            Slogger::W(TAG, "%s The camera device was already closed: %d", mIdString.string(), ec);

            // TODO: Fire onClosed anytime we get the device onClosed or the ISE?
            // or just suppress the ISE only and rely onClosed.
            // Also skip any of the draining work if this is already closed.

            // Short-circuit; queue callback immediately and return
            return mStateCallback->OnClosed(this);
        }
        //} catch (CameraAccessException e) {
        if (ec == (ECode)E_CAMERA_ACCESS_EXCEPTION) {
            // OK: close does not throw checked exceptions.
            Slogger::E(TAG, "%s Exception while stopping repeating: %d",mIdString.string(), ec);

            // TODO: call onError instead of onClosed if this happens
        }
        //}

        // If no sequences are pending, fire #onClosed immediately
        return mSequenceDrainer->BeginDrain();
    }
    return NOERROR;
}

ECode CameraCaptureSessionImpl::IsAborting(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mAborting;
    return NOERROR;
}

ECode CameraCaptureSessionImpl::CreateUserStateCallbackProxy(
    /* [in] */ IHandler* handler,
    /* [in] */ ICameraCaptureSessionStateCallback* _callback,
    /* [out] */ ICameraCaptureSessionStateCallback** back)
{
    VALIDATE_NOT_NULL(back);
    *back = NULL;

    AutoPtr<IInvokeDispatcher> userCallbackSink;
    CInvokeDispatcher::New(TO_IINTERFACE(_callback), (IInvokeDispatcher**)&userCallbackSink);
    AutoPtr<IHandlerDispatcher> handlerPassthrough;
    CHandlerDispatcher::New(IDispatchable::Probe(userCallbackSink), handler, (IHandlerDispatcher**)&handlerPassthrough);

    AutoPtr<ICallbackProxiesSessionStateCallbackProxy> proxy;
    CCallbackProxiesSessionStateCallbackProxy::New(IDispatchable::Probe(handlerPassthrough),
            (ICallbackProxiesSessionStateCallbackProxy**)&proxy);
    *back = ICameraCaptureSessionStateCallback::Probe(proxy);
    REFCOUNT_ADD(*back);
    return NOERROR;
}

ECode CameraCaptureSessionImpl::CreateCaptureCallbackProxy(
    /* [in] */ IHandler* handler,
    /* [in] */ ICameraCaptureSessionCaptureCallback* _callback,
    /* [out] */ ICameraDeviceImplCaptureCallback** back)
{
    VALIDATE_NOT_NULL(back);
    *back = NULL;

    AutoPtr<ICameraDeviceImplCaptureCallback> localCallback = new MyCaptureCallback(this);

    /*
     * Split the calls from the device callback into local callback and the following chain:
     * - replace the first CameraDevice arg with a CameraCaptureSession
     * - duck type from device callback to session callback
     * - then forward the call to a handler
     * - then finally invoke the destination method on the session callback object
     */
    if (_callback == NULL) {
        // OK: API allows the user to not specify a callback, and the handler may
        // also be null in that case. Collapse whole dispatch chain to only call the local
        // callback
        *back = localCallback;
        REFCOUNT_ADD(*back);
        return NOERROR;
    }

    AutoPtr<IInvokeDispatcher> localSink;
    CInvokeDispatcher::New(localCallback, (IInvokeDispatcher**)&localSink);

    AutoPtr<IInvokeDispatcher> userCallbackSink;
    CInvokeDispatcher::New(_callback, (IInvokeDispatcher**)&userCallbackSink);

    AutoPtr<IHandlerDispatcher> handlerPassthrough;
    CHandlerDispatcher::New(IDispatchable::Probe(userCallbackSink), handler,
            (IHandlerDispatcher**)&handlerPassthrough);

    AutoPtr<IDuckTypingDispatcher> duckToSession;
    assert(0);
    const String moduleName("............../Elastos.Droid.Core.eco");
    const String klassName(".............../CCameraDeviceImplCaptureCallback;");
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IModuleInfo> moduleInfo;
    ASSERT_SUCCEEDED(_CReflector_AcquireModuleInfo(moduleName, (IModuleInfo**)&moduleInfo));
    ASSERT_SUCCEEDED(moduleInfo->GetClassInfo(klassName, (IClassInfo**)&classInfo))
    CDuckTypingDispatcher::New(IDispatchable::Probe(handlerPassthrough),
            classInfo, (IDuckTypingDispatcher**)&duckToSession);

    AutoPtr<IArgumentReplacingDispatcher> replaceDeviceWithSession;
    CArgumentReplacingDispatcher::New(IDispatchable::Probe(duckToSession), /*argumentIndex*/0,
            TO_IINTERFACE(this), (IArgumentReplacingDispatcher**)&replaceDeviceWithSession);

    AutoPtr<ArrayOf<IDispatchable*> > array = ArrayOf<IDispatchable*>::Alloc(2);
    array->Set(0, IDispatchable::Probe(replaceDeviceWithSession));
    array->Set(1, IDispatchable::Probe(localSink));
    AutoPtr<IBroadcastDispatcher> broadcaster;
    CBroadcastDispatcher::New(array, (IBroadcastDispatcher**)&broadcaster);

    AutoPtr<ICallbackProxiesDeviceCaptureCallbackProxy> proxy;
    CCallbackProxiesDeviceCaptureCallbackProxy::New(IDispatchable::Probe(broadcaster),
            (ICallbackProxiesDeviceCaptureCallbackProxy**)&proxy);
    *back = ICameraDeviceImplCaptureCallback::Probe(proxy);
    REFCOUNT_ADD(*back);
    return NOERROR;
}

ECode CameraCaptureSessionImpl::GetDeviceStateCallback(
    /* [out] */ ICameraDeviceImplStateCallbackKK** back)
{
    VALIDATE_NOT_NULL(back);
    *back = NULL;

    AutoPtr<ICameraCaptureSession> session = this;
    AutoPtr<MyStateCallbackKK> backKK = new MyStateCallbackKK(this, session);
    *back = ICameraDeviceImplStateCallbackKK::Probe(backKK);
    REFCOUNT_ADD(*back);
    return NOERROR;
}

ECode CameraCaptureSessionImpl::CheckNotClosed()
{
    if (mClosed) {
        // throw new IllegalStateException(
        //         "Session has been closed; further changes are illegal.");
        Slogger::E(TAG, "Session has been closed; further changes are illegal.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CameraCaptureSessionImpl::AddPendingSequence(
    /* [in] */ Int32 sequenceId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IInteger32> obj = CoreUtils::Convert(sequenceId);
    mSequenceDrainer->TaskStarted(TO_IINTERFACE(obj));
    *result = sequenceId;
    return NOERROR;
}

ECode CameraCaptureSessionImpl::FinishPendingSequence(
    /* [in] */ Int32 sequenceId)
{
    AutoPtr<IInteger32> obj = CoreUtils::Convert(sequenceId);
    return mSequenceDrainer->TaskFinished(TO_IINTERFACE(obj));
}

} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos