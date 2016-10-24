
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
#include <elastos/core/ClassLoader.h>
#include <elastos/utility/logging/Logger.h>

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
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::ClassLoader;
using Elastos::Core::IClassLoader;
using Elastos::Utility::Logging::Logger;

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
        Logger::V(mHost->TAG, "%s onSequenceDrained", mHost->mIdString.string());
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
        Logger::V(mHost->TAG, "%s onAbortDrained", mHost->mIdString.string());
    }
    {    AutoLock syncLock(mHost);
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

//==============================================================================================
// CameraCaptureSessionImpl::IdleDrainListener
//==============================================================================================
ECode CameraCaptureSessionImpl::IdleDrainListener::OnDrained()
{
    if (mHost->VERBOSE) {
        Logger::V(mHost->TAG, "%s onIdleDrained", mHost->mIdString.string());
    }

    // Take device lock before session lock so that we can call back into device
    // without causing a deadlock
    CameraDeviceImpl* impl = (CameraDeviceImpl*)(mHost->mDeviceImpl.Get());
    {
        AutoLock syncLock(impl->mInterfaceLock);
        {
            AutoLock syncLock(mHost);
            /*
             * The device is now IDLE, and has settled. It will not transition to
             * ACTIVE or BUSY again by itself.
             *
             * It's now safe to unconfigure the outputs and after it's done invoke #onClosed.
             *
             * This operation is idempotent; a session will not be closed twice.
             */
            if (mHost->VERBOSE)
                Logger::V(mHost->TAG, "%s Session drain complete, skip unconfigure: %d"
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
                Logger::E(TAG, "%s Exception while configuring outputs: %d", mHost->mIdString.string(), ec);

                // TODO: call onError instead of onClosed if this happens
            }
            //} catch (IllegalStateException e) {
            if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
                // Camera is already closed, so go straight to the close callback
                if (mHost->VERBOSE) Logger::V(mHost->TAG,
                        "%s Camera was already closed or busy, skipping unconfigure", mHost->mIdString.string());
                mHost->mUnconfigureDrainer->TaskFinished();
            }
            //}

            return mHost->mUnconfigureDrainer->BeginDrain();
        }
    }
    return NOERROR;
}

//==============================================================================================
// CameraCaptureSessionImpl::UnconfigureDrainListener
//==============================================================================================
CAR_INTERFACE_IMPL(CameraCaptureSessionImpl::UnconfigureDrainListener, Object, ITaskDrainerDrainListener)

CameraCaptureSessionImpl::UnconfigureDrainListener::UnconfigureDrainListener(
    /* [in] */ CameraCaptureSessionImpl* host)
    : mHost(host)
{
}

ECode CameraCaptureSessionImpl::UnconfigureDrainListener::OnDrained()
{
    if (mHost->VERBOSE) {
        Logger::V(TAG, "%s onUnconfigureDrained", mHost->mIdString.string());
    }
    {
        AutoLock syncLock(mHost);
        // The device has finished unconfiguring. It's now fully closed.
        return mHost->mStateCallback->OnClosed(mHost);
    }
    return NOERROR;
}

//==============================================================================================
// CameraCaptureSessionImpl::MyStateCallbackKK
//==============================================================================================
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
    Logger::E(mHost->TAG, "Camera must already be open before creating a session");
    return E_ASSERTION_ERROR;
}


ECode CameraCaptureSessionImpl::MyStateCallbackKK::OnDisconnected(
    /* [in] */ ICameraDevice* camera)
{
    if (mHost->VERBOSE) {
        Logger::V(mHost->TAG, "%s, onDisconnected",mHost->mIdString.string());
    }
    return mHost->Close();
}


ECode CameraCaptureSessionImpl::MyStateCallbackKK::OnError(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ Int32 error)
{
    // Should not be reached, handled by device code
    Logger::W(mHost->TAG, "%s Got device error %d", mHost->mIdString.string(), error);
    return NOERROR;
}


ECode CameraCaptureSessionImpl::MyStateCallbackKK::OnActive(
    /* [in] */ ICameraDevice* camera)
{
    mHost->mIdleDrainer->TaskStarted();
    mActive = TRUE;

    if (mHost->VERBOSE) {
        Logger::V(mHost->TAG, "%s onActive", mHost->mIdString.string());
    }
    return mHost->mStateCallback->OnActive(mSession);
}


ECode CameraCaptureSessionImpl::MyStateCallbackKK::OnIdle(
    /* [in] */ ICameraDevice* camera)
{
    Boolean isAborting = FALSE;
    if (mHost->VERBOSE) {
        Logger::V(TAG, "%s onIdle", mHost->mIdString.string());
    }

    {
        AutoLock syncLock(mSession);
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

        {    AutoLock syncLock(mSession);
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
        Logger::V(mHost->TAG, "%s onBusy", mHost->mIdString.string());
    }
    return NOERROR;
}


ECode CameraCaptureSessionImpl::MyStateCallbackKK::OnUnconfigured(
    /* [in] */ ICameraDevice* camera)
{
    if (mHost->VERBOSE) {
        Logger::V(mHost->TAG, "%d onUnconfigured", mHost->mIdString.string());
    }
    {
        AutoLock syncLock(mSession);
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

//==============================================================================================
// CameraCaptureSessionImpl::MyCaptureCallback
//==============================================================================================
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

//==============================================================================================
// CameraCaptureSessionImpl
//==============================================================================================
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
    /* [in] */ ICameraCaptureSessionStateCallback* incallback,
    /* [in] */ IHandler* stateHandler,
    /* [in] */ ICameraDeviceImpl* deviceImpl,
    /* [in] */ IHandler* deviceStateHandler,
    /* [in] */ Boolean configureSuccess)
{
    Boolean res;
    if (outputs == NULL || (outputs->IsEmpty(&res), res)) {
        Logger::E(TAG, "outputs must be a non-null, non-empty list");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (incallback == NULL) {
        Logger::E(TAG, "callback must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mId = id;
    StringBuilder sb;
    sb += "CameraCaptureSession ";
    sb += mId;
    sb.ToString(&mIdString);

    // TODO: extra verification of outputs
    mOutputs = outputs;
    CameraDeviceImpl::CheckHandler(stateHandler, (IHandler**)&mStateHandler);
    CreateUserStateCallbackProxy(mStateHandler, incallback,
        (ICameraCaptureSessionStateCallback**)&mStateCallback);
    assert(mStateCallback != NULL);

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
        if (VERBOSE) Logger::V(TAG, "%s Created session successfully", mIdString.string());
        mConfigureSuccess = TRUE;
    }
    else {
        mStateCallback->OnConfigureFailed(this);
        mClosed = TRUE; // do not fire any other callbacks, do not allow any other work
        Logger::E(TAG, "%s Failed to create capture session; configuration failed", mIdString.string());
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

    {
        AutoLock syncLock(this);
        if (request == NULL) {
            Logger::E(TAG, "request must not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        FAIL_RETURN(CheckNotClosed())

        AutoPtr<IHandler> newHandler;
        CameraDeviceImpl::CheckHandler(handler, TO_IINTERFACE(_callback), (IHandler**)&newHandler);

        if (VERBOSE) {
            Logger::V(TAG, "%s capture - request %s, callback %s newHandler %s",
                mIdString.string(), TO_CSTR(request), TO_CSTR(_callback), TO_CSTR(newHandler));
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

    {
        AutoLock syncLock(this);
        Boolean res;
        if (requests == NULL) {
            Logger::E(TAG, "requests must not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        else if (requests->IsEmpty(&res), res) {
            Logger::E(TAG, "requests must have at least one element");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        FAIL_RETURN(CheckNotClosed())

        AutoPtr<IHandler> newHandler;
        CameraDeviceImpl::CheckHandler(handler, _callback, (IHandler**)&newHandler);

        if (VERBOSE) {
            Logger::V(TAG, "%s captureBurst - requests %s, callback %s newHandler %s",
                mIdString.string(), TO_CSTR(requests), TO_CSTR(_callback), TO_CSTR(newHandler));
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

    {
        AutoLock syncLock(this);
        if (request == NULL) {
            Logger::E(TAG, "request must not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        FAIL_RETURN(CheckNotClosed())

        AutoPtr<IHandler> newHandler;
        CameraDeviceImpl::CheckHandler(handler, _callback, (IHandler**)&newHandler);

        if (VERBOSE) {
            Logger::V(TAG, "%s setRepeatingRequest - requests %s, callback %s newHandler %s",
                mIdString.string(), TO_CSTR(request), TO_CSTR(_callback), TO_CSTR(newHandler));
        }

        AutoPtr<ICameraDeviceImplCaptureCallback> back;
        CreateCaptureCallbackProxy(newHandler, _callback, (ICameraDeviceImplCaptureCallback**)&back);
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

    {
        AutoLock syncLock(this);
        Boolean res;
        if (requests == NULL) {
            Logger::E(TAG, "requests must not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        else if (requests->IsEmpty(&res), res) {
            Logger::E(TAG, "requests must have at least one element");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        FAIL_RETURN(CheckNotClosed())

        AutoPtr<IHandler> newHandler;
        CameraDeviceImpl::CheckHandler(handler, _callback, (IHandler**)&newHandler);

        if (VERBOSE) {
            Logger::V(TAG, "%s SetRepeatingBurst - requests %s, callback %s newHandler %s",
                mIdString.string(), TO_CSTR(requests), TO_CSTR(_callback), TO_CSTR(newHandler));
        }

        AutoPtr<ICameraDeviceImplCaptureCallback> back;
        CreateCaptureCallbackProxy(newHandler, _callback, (ICameraDeviceImplCaptureCallback**)&back);
        Int32 _value;
        FAIL_RETURN(mDeviceImpl->SetRepeatingBurst(requests, back, mDeviceHandler, &_value))
        return AddPendingSequence(_value, value);
    }
    return NOERROR;
}

ECode CameraCaptureSessionImpl::StopRepeating()
{
    {
        AutoLock syncLock(this);
        FAIL_RETURN(CheckNotClosed())

        if (VERBOSE) {
            Logger::V(TAG, "%s stopRepeating", mIdString.string());
        }

        return mDeviceImpl->StopRepeating();
    }
    return NOERROR;
}

ECode CameraCaptureSessionImpl::AbortCaptures()
{
    {
        AutoLock syncLock(this);
        FAIL_RETURN(CheckNotClosed())

        if (VERBOSE) {
            Logger::V(TAG, "%s abortCaptures", mIdString.string());
        }

        if (mAborting) {
            Logger::W(TAG, "%s abortCaptures - Session is already aborting; doing nothing", mIdString.string());
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
    {
        AutoLock syncLock(this);
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
            Logger::V(TAG, "%s replaceSessionClose", mIdString.string());
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
    {
        AutoLock syncLock(this);
        if (mClosed) {
            if (VERBOSE) {
                Logger::V(TAG, "%s close - reentering", mIdString.string());
            }
            return NOERROR;
        }

        if (VERBOSE) {
            Logger::V(TAG, "%s close - first time", mIdString.string());
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
            Logger::W(TAG, "%s The camera device was already closed: %d", mIdString.string(), ec);

            // TODO: Fire onClosed anytime we get the device onClosed or the ISE?
            // or just suppress the ISE only and rely onClosed.
            // Also skip any of the draining work if this is already closed.

            // Short-circuit; queue callback immediately and return
            return mStateCallback->OnClosed(this);
        }
        //} catch (CameraAccessException e) {
        if (ec == (ECode)E_CAMERA_ACCESS_EXCEPTION) {
            // OK: close does not throw checked exceptions.
            Logger::E(TAG, "%s Exception while stopping repeating: %d",mIdString.string(), ec);

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

    AutoPtr<IDispatchable> userCallbackSink, handlerPassthrough;
    CInvokeDispatcher::New(TO_IINTERFACE(_callback), (IDispatchable**)&userCallbackSink);
    CHandlerDispatcher::New(userCallbackSink, handler, (IDispatchable**)&handlerPassthrough);

    return CCallbackProxiesSessionStateCallbackProxy::New(handlerPassthrough, back);
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

    AutoPtr<IDispatchable> localSink;
    CInvokeDispatcher::New(localCallback, (IDispatchable**)&localSink);

    AutoPtr<IDispatchable> userCallbackSink;
    CInvokeDispatcher::New(_callback, (IDispatchable**)&userCallbackSink);

    AutoPtr<IDispatchable> handlerPassthrough;
    CHandlerDispatcher::New(userCallbackSink, handler, (IDispatchable**)&handlerPassthrough);

    const String itfcName("Elastos.Droid.Hardware.Camera2.Impl.ICameraDeviceImplCaptureCallback");
    AutoPtr<IClassLoader> cl = ClassLoader::GetSystemClassLoader();
    AutoPtr<IInterfaceInfo> classInfo;
    ASSERT_SUCCEEDED(cl->LoadInterface(itfcName, (IInterfaceInfo**)&classInfo))
    AutoPtr<IDispatchable> duckToSession;
    CDuckTypingDispatcher::New(handlerPassthrough, classInfo, (IDispatchable**)&duckToSession);

    AutoPtr<IDispatchable> replaceDeviceWithSession;
    CArgumentReplacingDispatcher::New(duckToSession, /*argumentIndex*/0,
        TO_IINTERFACE(this), (IDispatchable**)&replaceDeviceWithSession);

    AutoPtr<ArrayOf<IDispatchable*> > array = ArrayOf<IDispatchable*>::Alloc(2);
    array->Set(0, replaceDeviceWithSession);
    array->Set(1, localSink);
    AutoPtr<IDispatchable> broadcaster;
    CBroadcastDispatcher::New(array, (IDispatchable**)&broadcaster);

    return CCallbackProxiesDeviceCaptureCallbackProxy::New(broadcaster, back);
}

ECode CameraCaptureSessionImpl::GetDeviceStateCallback(
    /* [out] */ ICameraDeviceImplStateCallbackKK** back)
{
    VALIDATE_NOT_NULL(back);
    *back = NULL;

    AutoPtr<ICameraCaptureSession> session = this;
    AutoPtr<MyStateCallbackKK> backKK = new MyStateCallbackKK(this, session);
    *back = backKK.Get();
    REFCOUNT_ADD(*back);
    return NOERROR;
}

ECode CameraCaptureSessionImpl::CheckNotClosed()
{
    if (mClosed) {
        Logger::E(TAG, "Session has been closed; further changes are illegal.");
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
    mSequenceDrainer->TaskStarted(obj);
    *result = sequenceId;
    return NOERROR;
}

ECode CameraCaptureSessionImpl::FinishPendingSequence(
    /* [in] */ Int32 sequenceId)
{
    AutoPtr<IInteger32> obj = CoreUtils::Convert(sequenceId);
    return mSequenceDrainer->TaskFinished(obj);
}

} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos