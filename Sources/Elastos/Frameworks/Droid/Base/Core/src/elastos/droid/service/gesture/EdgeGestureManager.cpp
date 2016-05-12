#include "elastos/droid/service/gesture/EdgeGestureManager.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Gesture {


//======================================================================
// EdgeGestureManager::EdgeGestureActivationListener
//======================================================================

CAR_INTERFACE_IMPL(EdgeGestureManager::EdgeGestureActivationListener, Object, IEdgeGestureActivationListener)

EdgeGestureManager::EdgeGestureActivationListener::EdgeGestureActivationListener()
{}

ECode EdgeGestureManager::EdgeGestureActivationListener::constructor();
{
    AutoPtr<ILooper> mainLooper = Looper::GetMainLooper();
    return constructor(mainLooper);
}

ECode EdgeGestureManager::EdgeGestureActivationListener::constructor(
    /* [in] */ ILooper* looper);
{
    CHandler::New(looper, (IHandler**)&mHandler);
    return CEdgeGestureActivationListenerDelegator::New(this, (IEdgeGestureActivationListener**)&mDelegator);
}

ECode EdgeGestureManager::EdgeGestureActivationListener::SetHostCallback(
    /* [in] */ IIEdgeGestureHostCallback* hostCallback)
{
    mCallback = hostCallback;
    return NOERROR;
}

ECode EdgeGestureManager::EdgeGestureActivationListener::GainTouchFocus(
    /* [in] */ IBinder* applicationWindowToken,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ECode ec = mCallback->GainTouchFocus(applicationWindowToken, result);
    if (FAILED(ec)) {
        Slogger::W(EdgeGestureManager::TAG, "gainTouchFocus failed: ec=%08x", ec);
        /* fall through */
    }
    return NOERROR;
}

ECode EdgeGestureManager::EdgeGestureActivationListener::DropEventsUntilLift(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ECode ec = mCallback->DropEventsUntilLift(result);
    if (FAILED(ec)) {
        Slogger::W(EdgeGestureManager::TAG, "dropNextEvents failed: ec=%08x", ec);
        /* fall through */
    }
    return NOERROR;
}

ECode EdgeGestureManager::EdgeGestureActivationListener::RestoreListenerState()
{
    if (EdgeGestureManager::DEBUG) {
        AutoPtr<IThread> thread = Thread::GetCurrentThread();
        String name;
        thread->GetName(&name);
        Slogger::D(TAG, "restore listener state: %s", name.string());
    }

    ECode ec = mCallback->RestoreListenerState();
    if (FAILED(ec)) {
        Slogger::W(EdgeGestureManager::TAG, "restoreListenerState failed: ec=%08x", ec);
        /* fall through */
    }
    return NOERROR;
}

//======================================================================
// EdgeGestureManager
//======================================================================

const String EdgeGestureManager::TAG("EdgeGestureManager");
const Boolean EdgeGestureManager::DEBUG = FALSE;

CAR_INTERFACE_IMPL(EdgeGestureManager, Object, IEdgeGestureManager)

EdgeGestureManager::EdgeGestureManager()
{}

ECode EdgeGestureManager::constructor()
{
    return NOERROR;
}

ECode EdgeGestureManager::constructor(
    /* [in] */ IIEdgeGestureService ps)
{
    mPs = ps;
    return NOERROR;
}

AutoPtr<IEdgeGestureManager> EdgeGestureManager::GetInstance()
{
    synchronized(sLock) {
        if (sInstance == NULL) {
            AutoPtr<IInterface> obj = ServiceManager::GetService(String("edgegestureservice"));
            IIEdgeGestureService* service = IIEdgeGestureService::Probe(obj);
            CEdgeGestureManager::New(service, (IEdgeGestureManager**)&sInstance);
        }
        return sInstance;
    }
    return sInstance;
}

ECode EdgeGestureManager::IsPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPs != NULL;
    return NOERROR;
}

ECode EdgeGestureManager::SetEdgeGestureActivationListener(
    /* [in] */ IEdgeGestureActivationListener* listener,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (DEBUG) {
        Slogger::D(TAG, "Set edge gesture activation listener");
    }

    EdgeGestureActivationListener* agal = (EdgeGestureActivationListener*)listener;

    AutoPtr<IIEdgeGestureHostCallback> callback;
    ECode ec = mPs->RegisterEdgeGestureActivationListener(
        agal->mDelegator, (IIEdgeGestureHostCallback**)&callback);
    FAIL_GOTO(ec, _EXIT_)

    listener->SetHostCallback(callback);
    *result = TRUE;

_EXIT_:
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to set edge gesture activation listener: ec=%08x", ec);
    }

    return NOERROR;
}

ECode EdgeGestureManager::UpdateEdgeGestureActivationListener(
    /* [in] */ IEdgeGestureActivationListener* listener,
    /* [in] */ Int32 positions)
{
    if (DEBUG) {
        Slogger::D(TAG, "Update edge gesture activation listener: 0x08x", positions);
    }

    EdgeGestureActivationListener* agal = (EdgeGestureActivationListener*)listener;
    ECode ec = mPs->UpdateEdgeGestureActivationListener(IBinder::Probe(agal->mDelegator), positions);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to update edge gesture activation listener: %08x", ec);
    }
}


} // namespace Gesture
} // namespace Service
} // namepsace Droid
} // namespace Elastos
