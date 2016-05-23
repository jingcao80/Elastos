
#include "elastos/droid/media/session/CMediaSessionManager.h"
#include "elastos/droid/media/session/CMediaController.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/utility/CArrayMap.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL(CMediaSessionManager::ActiveSessionsListener, Object, IIActiveSessionsListener)

CAR_INTERFACE_IMPL(CMediaSessionManager, Object, IMediaSessionManager)

CAR_OBJECT_IMPL(CMediaSessionManager)

const String CMediaSessionManager::TAG("SessionManager");

ECode CMediaSessionManager::ActiveSessionsChangedRunnable::Run()
{
    AutoPtr<IArrayList> controllers;
    CArrayList::New((IArrayList**)&controllers);
    Int32 size;
    mTokens->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mTokens->Get(i, (IInterface**)&obj);
        AutoPtr<IMediaSessionToken> token = IMediaSessionToken::Probe(obj);
        AutoPtr<IMediaController> controller;
        CMediaController::New(mContext, token, (IMediaController**)&controller);
        controllers->Add(controller);
    }
    return mListener->OnActiveSessionsChanged(IList::Probe(controllers));
}

ECode CMediaSessionManager::ActiveSessionsListener::OnActiveSessionsChanged(
    /* [in] */ IList * tokens)
{
    if (mHandler != NULL) {
        AutoPtr<ActiveSessionsChangedRunnable> run
             = new ActiveSessionsChangedRunnable(this, mContext, mListener, mHandler, tokens);
        Boolean b;
        mHandler->Post(run, &b);
    }
    return NOERROR;
}

CMediaSessionManager::SessionsChangedWrapper::SessionsChangedWrapper(
    /* [in] */ IMediaSessionManagerOnActiveSessionsChangedListener * listener,
    /* [in] */ IHandler * handler,
    /* [in] */ CMediaSessionManager * host,
    /* [in] */ IContext * context)
    : mListener(listener)
    , mHandler(handler)
    , mContext(context)
    , mHost(host)
{
    mStub = new ActiveSessionsListener(this, mContext, mListener, mHandler);
}

CMediaSessionManager::CMediaSessionManager()
{
    CArrayMap::New((IArrayMap**)&mListeners);
}

CMediaSessionManager::~CMediaSessionManager()
{
}

ECode CMediaSessionManager::constructor(
        /* [in] */ IContext * context)
{
    // Consider rewriting like DisplayManagerGlobal
    // Decide if we need context
    mContext = context;
    AutoPtr<IInterface> service = ServiceManager::GetService(IContext::MEDIA_SESSION_SERVICE);
    mService = IISessionManager::Probe(service);
    return NOERROR;
}

ECode CMediaSessionManager::CreateSession(
    /* [in] */ IISessionCallback * cbStub,
    /* [in] */ const String& tag,
    /* [in] */ Int32 userId,
    /* [out] */ IISession ** result)
{
    VALIDATE_NOT_NULL(result)
    String pkgName;
    mContext->GetPackageName(&pkgName);
    return mService->CreateSession(pkgName, IISessionCallback::Probe(cbStub), tag, userId, result);
}

ECode CMediaSessionManager::GetActiveSessions(
    /* [in] */ IComponentName * notificationListener,
    /* [out] */ IList ** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 myUserId;
    helper->GetMyUserId(&myUserId);
    return GetActiveSessionsForUser(notificationListener, myUserId, result);
}

ECode CMediaSessionManager::GetActiveSessionsForUser(
    /* [in] */ IComponentName * notificationListener,
    /* [in] */ Int32 userId,
    /* [out] */ IList ** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IArrayList> controllers;
    CArrayList::New((IArrayList**)&controllers);
    // try {
    AutoPtr<IList> binders;
    mService->GetSessions(notificationListener, userId, (IList**)&binders);
    Int32 size;
    binders->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        binders->Get(i, (IInterface**)&obj);
        AutoPtr<IBinder> binder = IBinder::Probe(obj);
        AutoPtr<IMediaController> controller;
        CMediaController::New(mContext, IISessionController::Probe(binder), (IMediaController**)&controller);
        controllers->Add(controller);
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Failed to get active sessions: ", e);
    // }
    *result = IList::Probe(controllers);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaSessionManager::AddOnActiveSessionsChangedListener(
    /* [in] */ IMediaSessionManagerOnActiveSessionsChangedListener * sessionListener,
    /* [in] */ IComponentName * notificationListener)
{
    return AddOnActiveSessionsChangedListener(sessionListener, notificationListener, NULL);
}

ECode CMediaSessionManager::AddOnActiveSessionsChangedListener(
    /* [in] */ IMediaSessionManagerOnActiveSessionsChangedListener * sessionListener,
    /* [in] */ IComponentName * notificationListener,
    /* [in] */ IHandler * handler)
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 myUserId;
    helper->GetMyUserId(&myUserId);
    return AddOnActiveSessionsChangedListener(sessionListener, notificationListener,
            myUserId, handler);
}

ECode CMediaSessionManager::AddOnActiveSessionsChangedListener(
    /* [in] */ IMediaSessionManagerOnActiveSessionsChangedListener * sessionListener,
    /* [in] */ IComponentName * notificationListener,
    /* [in] */ Int32 userId,
    /* [in] */ IHandler * _handler)
{
    if (sessionListener == NULL) {
        // throw new IllegalArgumentException("listener may not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHandler> handler = _handler;
    if (handler == NULL) {
        CHandler::New((IHandler**)&handler);
    }
    {    AutoLock syncLock(mLock);
        AutoPtr<IInterface> obj;
        IMap::Probe(mListeners)->Get(sessionListener, (IInterface**)&obj);
        AutoPtr<SessionsChangedWrapper> wrapper = (SessionsChangedWrapper*)(IObject*)obj.Get();
        if (wrapper != NULL) {
            Logger::W(TAG, String("Attempted to add session listener twice, ignoring."));
            return NOERROR;
        }
        wrapper = new SessionsChangedWrapper(sessionListener, handler, this, mContext);
        // try {
        mService->AddSessionsListener(wrapper->mStub, notificationListener, userId);
        IMap::Probe(mListeners)->Put(sessionListener, (IInterface*)(IObject*)wrapper);
        // } catch (RemoteException e) {
        //     Log.e(TAG, "Error in addOnActiveSessionsChangedListener.", e);
        // }
    }
     return NOERROR;
}

ECode CMediaSessionManager::RemoveOnActiveSessionsChangedListener(
    /* [in] */ IMediaSessionManagerOnActiveSessionsChangedListener * listener)
{
    if (listener == NULL) {
        // throw new IllegalArgumentException("listener may not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    {    AutoLock syncLock(mLock);
        AutoPtr<IInterface> obj;
        IMap::Probe(mListeners)->Remove(listener, (IInterface**)&obj);
        AutoPtr<SessionsChangedWrapper> wrapper = (SessionsChangedWrapper*)(IObject*)obj.Get();
        if (wrapper != NULL) {
            // try {
            mService->RemoveSessionsListener(wrapper->mStub);
            // } catch (RemoteException e) {
            //     Log.e(TAG, "Error in removeOnActiveSessionsChangedListener.", e);
            // }
        }
    }
    return NOERROR;
}

ECode CMediaSessionManager::SetRemoteVolumeController(
    /* [in] */ IIRemoteVolumeController * rvc)
{
    // try {
    return mService->SetRemoteVolumeController(rvc);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error in setRemoteVolumeController.", e);
    // }
}

ECode CMediaSessionManager::DispatchMediaKeyEvent(
    /* [in] */ IKeyEvent * keyEvent)
{
    return DispatchMediaKeyEvent(keyEvent, FALSE);
}

ECode CMediaSessionManager::DispatchMediaKeyEvent(
    /* [in] */ IKeyEvent * keyEvent,
    /* [in] */ Boolean needWakeLock)
{
    // try {
    return mService->DispatchMediaKeyEvent(keyEvent, needWakeLock);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Failed to send key event.", e);
    // }
}

ECode CMediaSessionManager::DispatchAdjustVolume(
    /* [in] */ Int32 suggestedStream,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags)
{
    // try {
    return mService->DispatchAdjustVolume(suggestedStream, direction, flags);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Failed to send adjust volume.", e);
    // }
}

ECode CMediaSessionManager::IsGlobalPriorityActive(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return mService->IsGlobalPriorityActive(result);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Failed to check if the global priority is active.", e);
    // }
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
