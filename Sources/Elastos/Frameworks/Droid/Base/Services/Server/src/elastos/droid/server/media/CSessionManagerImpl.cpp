
#include <Elastos.Droid.Speech.h>
#include "elastos/droid/server/media/CSessionManagerImpl.h"
#include "elastos/droid/os/Binder.h"
#include <elastos/droid/os/UserHandle.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Media::IAudioSystem;
using Elastos::Droid::Media::CAudioSystem;
using Elastos::Droid::Media::Session::EIID_IISessionManager;
using Elastos::Droid::Media::Session::IMediaControllerPlaybackInfo;
using Elastos::Droid::Media::Session::IMediaSession;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Speech::IRecognizerIntent;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

//==============================================================================
//                  CSessionManagerImpl::KeyEventDone
//==============================================================================

ECode CSessionManagerImpl::KeyEventDone::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (intent == NULL) {
        return NOERROR;
    }
    AutoPtr<IBundle> extras;
    intent->GetExtras((IBundle**)&extras);
    if (extras == NULL) {
        return NOERROR;
    }
    AutoLock lock(mHost->mLock);
    Boolean containsKey, isHeld;
    if ((extras->ContainsKey(EXTRA_WAKELOCK_ACQUIRED, &containsKey), containsKey)
            && (mHost->mMediaEventWakeLock->IsHeld(&isHeld), isHeld)) {
        mHost->mMediaEventWakeLock->ReleaseLock();
    }
    return NOERROR;
}


//==============================================================================
//                  CSessionManagerImpl
//==============================================================================

const String CSessionManagerImpl::EXTRA_WAKELOCK_ACQUIRED("android.media.AudioService.WAKELOCK_ACQUIRED");
const Int32 CSessionManagerImpl::WAKELOCK_RELEASE_ON_FINISHED; // magic number

CSessionManagerImpl::CSessionManagerImpl()
    : mVoiceButtonDown(FALSE)
    , mVoiceButtonHandled(FALSE)
{
}

CAR_INTERFACE_IMPL_2(CSessionManagerImpl, Object, IISessionManager, IBinder)

CAR_OBJECT_IMPL(CSessionManagerImpl)

ECode CSessionManagerImpl::constructor(
    /* [in] */ Handle64 host)
{
    mHost = (MediaSessionService*)host;
    CKeyEventWakeLockReceiver::NewByFriend(mHost->mHandler, (Handle64)mHost, (CKeyEventWakeLockReceiver**)&mKeyEventReceiver);
    mKeyEventDone = new KeyEventDone(mHost);
    return NOERROR;
}

ECode CSessionManagerImpl::CreateSession(
    /* [in] */ const String& packageName,
    /* [in] */ IISessionCallback* cb,
    /* [in] */ const String& tag,
    /* [in] */ Int32 userId,
    /* [out] */ IISession** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 pid = Binder::GetCallingPid();
    Int32 uid = Binder::GetCallingUid();
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    FAIL_RETURN(mHost->EnforcePackageName(packageName, uid))
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 resolvedUserId;
    helper->HandleIncomingUser(pid, uid, userId,
            FALSE /* allowAll */, TRUE /* requireFull */, String("createSession"), packageName, &resolvedUserId);
    if (cb == NULL) {
        Slogger::E(MediaSessionService::TAG, "Controller callback cannot be null");
        Binder::RestoreCallingIdentity(token);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<MediaSessionRecord> record;
    ECode ec = mHost->CreateSessionInternal(pid, uid, resolvedUserId, packageName, cb, tag, (MediaSessionRecord**)&record);
    if (FAILED(ec)) {
        Slogger::E(MediaSessionService::TAG, "CreateSessionInternal failed");
        Binder::RestoreCallingIdentity(token);
        return ec;
    }

    AutoPtr<IISession> temp = record->GetSessionBinder();
    *result = temp;
    REFCOUNT_ADD(*result)
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CSessionManagerImpl::GetSessions(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 userId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 pid = Binder::GetCallingPid();
    Int32 uid = Binder::GetCallingUid();
    Int64 token = Binder::ClearCallingIdentity();

    // try {
    Int32 resolvedUserId;
    ECode ec = VerifySessionsRequest(componentName, userId, pid, uid, &resolvedUserId);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(token);
        return ec;
    }
    AutoPtr<IList> binders;
    CArrayList::New((IList**)&binders);
    AutoLock lock(mHost->mLock);
    AutoPtr<List<AutoPtr<MediaSessionRecord> > > records = mHost->mPriorityStack->GetActiveSessions(resolvedUserId);
    List<AutoPtr<MediaSessionRecord> >::Iterator it = records->Begin();
    for (; it != records->End(); ++it) {
        AutoPtr<IBinder> b = IBinder::Probe((*it)->GetControllerBinder());
        binders->Add(b);
    }
    *result = binders;
    REFCOUNT_ADD(*result)
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CSessionManagerImpl::AddSessionsListener(
    /* [in] */ IIActiveSessionsListener* listener,
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 userId)
{
    Int32 pid = Binder::GetCallingPid();
    Int32 uid = Binder::GetCallingUid();
    Int64 token = Binder::ClearCallingIdentity();

    // try {
    Int32 resolvedUserId;
    ECode ec = VerifySessionsRequest(componentName, userId, pid, uid, &resolvedUserId);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(token);
        return NOERROR;
    }
    AutoLock lock(mHost->mLock);
    List<AutoPtr<MediaSessionService::SessionsListenerRecord> >::Iterator it = mHost->FindIndexOfSessionsListenerLocked(listener);
    if (it != mHost->mSessionsListeners.End()) {
        Slogger::W(MediaSessionService::TAG, "ActiveSessionsListener is already added, ignoring");
        return NOERROR;
    }
    AutoPtr<MediaSessionService::SessionsListenerRecord> record = new MediaSessionService::SessionsListenerRecord(listener,
            componentName, resolvedUserId, pid, uid, mHost);
    // try {
    AutoPtr<IProxy> proxy = (IProxy*)IBinder::Probe(listener)->Probe(EIID_IProxy);
    if (proxy != NULL) {
        if (FAILED(proxy->LinkToDeath((IProxyDeathRecipient*)record.Get(), 0))) {
            Slogger::E(MediaSessionService::TAG, "ActiveSessionsListener is dead, ignoring it");
            Binder::RestoreCallingIdentity(token);
            return NOERROR;
        }
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, "ActiveSessionsListener is dead, ignoring it", e);
    //     return;
    // }
    mHost->mSessionsListeners.PushBack(record);
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CSessionManagerImpl::RemoveSessionsListener(
    /* [in] */ IIActiveSessionsListener* listener)
{
    AutoLock lock(mHost->mLock);
    List<AutoPtr<MediaSessionService::SessionsListenerRecord> >::Iterator it = mHost->FindIndexOfSessionsListenerLocked(listener);
    if (it != mHost->mSessionsListeners.End()) {
        AutoPtr<MediaSessionService::SessionsListenerRecord> record = *it;
        mHost->mSessionsListeners.Erase(it);
        // try {
        AutoPtr<IProxy> proxy = (IProxy*)IBinder::Probe(record->mListener)->Probe(EIID_IProxy);
        if (proxy != NULL) {
            Boolean result;
            return proxy->UnlinkToDeath((IProxyDeathRecipient*)record.Get(), 0, &result);
        }
        // } catch (Exception e) {
        //     // ignore exceptions, the record is being removed
        // }
    }
    return NOERROR;
}

ECode CSessionManagerImpl::DispatchMediaKeyEvent(
    /* [in] */ IKeyEvent* keyEvent,
    /* [in] */ Boolean needWakeLock)
{
    if (keyEvent == NULL) {
        Slogger::W(MediaSessionService::TAG, "Attempted to dispatch null or non-media key event.");
        return NOERROR;
    }
    else {
        Int32 keyCode;
        keyEvent->GetKeyCode(&keyCode);
        AutoPtr<IKeyEventHelper> helper;
        CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
        Boolean isMediaKey;
        if (helper->IsMediaKey(keyCode, &isMediaKey), !isMediaKey) {
            Slogger::W(MediaSessionService::TAG, "Attempted to dispatch null or non-media key event.");
            return NOERROR;
        }
    }

    Int64 token = Binder::ClearCallingIdentity();

    // try {
    AutoLock lock(mHost->mLock);
    AutoPtr<MediaSessionRecord> session = mHost->mPriorityStack->GetDefaultMediaButtonSession(mHost->mCurrentUserId);
    Int32 keyCode;
    keyEvent->GetKeyCode(&keyCode);
    if (IsVoiceKey(keyCode)) {
        HandleVoiceKeyEventLocked(keyEvent, needWakeLock, session);
    }
    else {
        DispatchMediaKeyEventLocked(keyEvent, needWakeLock, session);
    }
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CSessionManagerImpl::DispatchAdjustVolume(
    /* [in] */ Int32 suggestedStream,
    /* [in] */ Int32 delta,
    /* [in] */ Int32 flags)
{
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    AutoLock lock(mHost->mLock);
    AutoPtr<MediaSessionRecord> session = mHost->mPriorityStack->GetDefaultVolumeSession(mHost->mCurrentUserId);
    ECode ec = DispatchAdjustVolumeLocked(suggestedStream, delta, flags, session);
    // }
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return ec;
}

ECode CSessionManagerImpl::SetRemoteVolumeController(
    /* [in] */ IIRemoteVolumeController* rvc)
{
    Int32 pid = Binder::GetCallingPid();
    Int32 uid = Binder::GetCallingUid();
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    ECode ec = mHost->EnforceStatusBarPermission(String("listen for volume changes"), pid, uid);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(token);
        return ec;
    }
    mHost->mRvc = rvc;
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return ec;
}

ECode CSessionManagerImpl::IsGlobalPriorityActive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->mPriorityStack->IsGlobalPriorityActive();
    return NOERROR;
}

ECode CSessionManagerImpl::VerifySessionsRequest(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    String packageName(NULL);
    if (componentName != NULL) {
        // If they gave us a component name verify they own the
        // package
        String packageName;
        componentName->GetPackageName(&packageName);
        FAIL_RETURN(mHost->EnforcePackageName(packageName, uid))
    }
    // Check that they can make calls on behalf of the user and
    // get the final user id
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 resolvedUserId;
    helper->HandleIncomingUser(pid, uid, userId, TRUE /* allowAll */, TRUE /* requireFull */,
            String("getSessions"), packageName, &resolvedUserId);
    // Check if they have the permissions or their component is
    // enabled for the user they're calling from.
    FAIL_RETURN(mHost->EnforceMediaPermissions(componentName, pid, uid, resolvedUserId))
    *result = resolvedUserId;
    return NOERROR;
}

ECode CSessionManagerImpl::DispatchAdjustVolumeLocked(
    /* [in] */ Int32 suggestedStream,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags,
    /* [in] */ MediaSessionRecord* session)
{
    if (MediaSessionService::DEBUG) {
        String description(NULL);
        if (session != NULL) {
            session->ToString(&description);
        }
        Slogger::D(MediaSessionService::TAG, "Adjusting session %s by %d. flags=%d, suggestedStream=%d",
                description.string(), direction, flags, suggestedStream);

    }
    if (session == NULL) {
        if ((flags & IAudioManager::FLAG_ACTIVE_MEDIA_ONLY) != 0) {
            AutoPtr<IAudioSystem> audioSys;
            CAudioSystem::AcquireSingleton((IAudioSystem**)&audioSys);
            Boolean isStreamActive;
            if (audioSys->IsStreamActive(IAudioManager::STREAM_MUSIC, 0, &isStreamActive), !isStreamActive) {
                if (MediaSessionService::DEBUG) {
                    Slogger::D(MediaSessionService::TAG, "No active session to adjust, skipping media only volume event");
                }
                return NOERROR;
            }
        }
        // try {
        AutoPtr<IContext> ctx;
        mHost->GetContext((IContext**)&ctx);
        String opPackageName;
        ctx->GetOpPackageName(&opPackageName);
        ECode ec = mHost->mAudioService->AdjustSuggestedStreamVolume(direction, suggestedStream, flags, opPackageName);
        if (FAILED(ec)) {
            Slogger::E(MediaSessionService::TAG, "Error adjusting default volume.");
            return ec;
        }
        // } catch (RemoteException e) {
        //     Log.e(TAG, "Error adjusting default volume.", e);
        // }
    }
    else {
        AutoPtr<IContext> ctx;
        mHost->GetContext((IContext**)&ctx);
        String packageName;
        ctx->GetPackageName(&packageName);
        session->AdjustVolume(direction, flags, packageName, UserHandle::GetMyUserId(), TRUE);
        if (session->GetPlaybackType() == IMediaControllerPlaybackInfo::PLAYBACK_TYPE_REMOTE
                && mHost->mRvc != NULL) {
            // try {
            ECode ec = mHost->mRvc->RemoteVolumeChanged(session->GetControllerBinder(), flags);
            if (FAILED(ec)) {
                Slogger::E(MediaSessionService::TAG, "Error sending volume change to system UI. 0x%08x", ec);
                return ec;
            }
            // } catch (Exception e) {
            //     Log.wtf(TAG, "Error sending volume change to system UI.", e);
            // }
        }
    }
    return NOERROR;
}

void CSessionManagerImpl::HandleVoiceKeyEventLocked(
    /* [in] */ IKeyEvent* keyEvent,
    /* [in] */ Boolean needWakeLock,
    /* [in] */ MediaSessionRecord* session)
{
    if (session != NULL && session->HasFlag(IMediaSession::FLAG_EXCLUSIVE_GLOBAL_PRIORITY)) {
        // If the phone app has priority just give it the event
        DispatchMediaKeyEventLocked(keyEvent, needWakeLock, session);
        return;
    }
    Int32 action;
    keyEvent->GetAction(&action);
    Int32 flags;
    keyEvent->GetFlags(&flags);
    Boolean isLongPress = (flags & IKeyEvent::FLAG_LONG_PRESS) != 0;
    if (action == IKeyEvent::ACTION_DOWN) {
        Int32 count;
        if (keyEvent->GetRepeatCount(&count), count == 0) {
            mVoiceButtonDown = TRUE;
            mVoiceButtonHandled = FALSE;
        }
        else if (mVoiceButtonDown && !mVoiceButtonHandled && isLongPress) {
            mVoiceButtonHandled = TRUE;
            StartVoiceInput(needWakeLock);
        }
    }
    else if (action == IKeyEvent::ACTION_UP) {
        if (mVoiceButtonDown) {
            mVoiceButtonDown = FALSE;
            Boolean isCanceled;
            if (!mVoiceButtonHandled && (keyEvent->IsCanceled(&isCanceled), !isCanceled)) {
                // Resend the down then send this event through
                AutoPtr<IKeyEventHelper> helper;
                CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
                AutoPtr<IKeyEvent> downEvent;
                helper->ChangeAction(keyEvent, IKeyEvent::ACTION_DOWN, (IKeyEvent**)&downEvent);
                DispatchMediaKeyEventLocked(downEvent, needWakeLock, session);
                DispatchMediaKeyEventLocked(keyEvent, needWakeLock, session);
            }
        }
    }
}

void CSessionManagerImpl::DispatchMediaKeyEventLocked(
    /* [in] */ IKeyEvent* keyEvent,
    /* [in] */ Boolean needWakeLock,
    /* [in] */ MediaSessionRecord* session)
{
    if (session != NULL) {
        if (MediaSessionService::DEBUG) {
            String str;
            session->ToString(&str);
            Slogger::D(MediaSessionService::TAG, "Sending media key to %s", str.string());
        }
        if (needWakeLock) {
            mKeyEventReceiver->AquireWakeLockLocked();
        }
        // If we don't need a wakelock use -1 as the id so we
        // won't release it later
        session->SendMediaButton(keyEvent,
                needWakeLock ? mKeyEventReceiver->mLastTimeoutId : -1,
                mKeyEventReceiver);
    }
    else {
        // Launch the last PendingIntent we had with priority
        AutoPtr<IActivityManagerHelper> helper;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
        Int32 userId;
        helper->GetCurrentUser(&userId);
        HashMap<Int32, AutoPtr<MediaSessionService::UserRecord> >::Iterator it = mHost->mUserRecords.Find(userId);
        AutoPtr<MediaSessionService::UserRecord> user;
        if (it != mHost->mUserRecords.End()) {
            user = it->mSecond;
        }
        if (user->mLastMediaButtonReceiver != NULL) {
            if (MediaSessionService::DEBUG) {
                Slogger::D(MediaSessionService::TAG, "Sending media key to last known PendingIntent");
            }
            if (needWakeLock) {
                mKeyEventReceiver->AquireWakeLockLocked();
            }
            AutoPtr<IIntent> mediaButtonIntent;
            CIntent::New(IIntent::ACTION_MEDIA_BUTTON, (IIntent**)&mediaButtonIntent);
            mediaButtonIntent->PutExtra(IIntent::EXTRA_KEY_EVENT, IParcelable::Probe(keyEvent));
            // try {
            AutoPtr<IContext> ctx;
            mHost->GetContext((IContext**)&ctx);
            ECode ec = user->mLastMediaButtonReceiver->Send(ctx,
                    needWakeLock ? mKeyEventReceiver->mLastTimeoutId : -1,
                    mediaButtonIntent, (IPendingIntentOnFinished*)mKeyEventReceiver.Get(), NULL);
            if (FAILED(ec)) {
                Slogger::I(MediaSessionService::TAG, "Error sending key event to media button receiver %p, 0x%08x",
                        user->mLastMediaButtonReceiver.Get(), ec);
            }
            // } catch (CanceledException e) {
            //     Log.i(TAG, "Error sending key event to media button receiver "
            //             + user.mLastMediaButtonReceiver, e);
            // }
        }
        else {
            if (MediaSessionService::DEBUG) {
                Slogger::D(MediaSessionService::TAG, "Sending media key ordered broadcast");
            }
            if (needWakeLock) {
                mHost->mMediaEventWakeLock->AcquireLock();
            }
            // Fallback to legacy behavior
            AutoPtr<IIntent> keyIntent;
            CIntent::New(IIntent::ACTION_MEDIA_BUTTON, NULL, (IIntent**)&keyIntent);
            keyIntent->PutExtra(IIntent::EXTRA_KEY_EVENT, IParcelable::Probe(keyEvent));
            if (needWakeLock) {
                keyIntent->PutExtra(EXTRA_WAKELOCK_ACQUIRED, WAKELOCK_RELEASE_ON_FINISHED);
            }
            AutoPtr<IContext> ctx;
            mHost->GetContext((IContext**)&ctx);
            ctx->SendOrderedBroadcastAsUser(keyIntent, UserHandle::ALL,
                    String(NULL), mKeyEventDone, mHost->mHandler, IActivity::RESULT_OK, String(NULL), NULL);
        }
    }
}

void CSessionManagerImpl::StartVoiceInput(
    /* [in] */ Boolean needWakeLock)
{
    AutoPtr<IIntent> voiceIntent;
    // select which type of search to launch:
    // - screen on and device unlocked: action is ACTION_WEB_SEARCH
    // - device locked or screen off: action is
    // ACTION_VOICE_SEARCH_HANDS_FREE
    // with EXTRA_SECURE set to true if the device is securely locked
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(service);
    Boolean isKeyguardLocked;
    Boolean isLocked = mHost->mKeyguardManager != NULL &&
            (mHost->mKeyguardManager->IsKeyguardLocked(&isKeyguardLocked), isKeyguardLocked);
    Boolean isScreenOn;
    if (!isLocked && (pm->IsScreenOn(&isScreenOn), isScreenOn)) {
        CIntent::New(IRecognizerIntent::ACTION_WEB_SEARCH, (IIntent**)&voiceIntent);
        Slogger::I(MediaSessionService::TAG, "voice-based interactions: about to use ACTION_WEB_SEARCH");
    }
    else {
        CIntent::New(IRecognizerIntent::ACTION_VOICE_SEARCH_HANDS_FREE, (IIntent**)&voiceIntent);
        Boolean isKeyguardSecure;
        voiceIntent->PutExtra(IRecognizerIntent::EXTRA_SECURE,
                isLocked && (mHost->mKeyguardManager->IsKeyguardSecure(&isKeyguardSecure), isKeyguardSecure));
        Slogger::I(MediaSessionService::TAG, "voice-based interactions: about to use ACTION_VOICE_SEARCH_HANDS_FREE");
    }
    // start the search activity
    if (needWakeLock) {
        mHost->mMediaEventWakeLock->AcquireLock();
    }
    // try {
    if (voiceIntent != NULL) {
        voiceIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
        AutoPtr<IContext> context;
        mHost->GetContext((IContext**)&context);
        ECode ec = context->StartActivityAsUser(voiceIntent, UserHandle::CURRENT);
        if (FAILED(ec)) {
            Slogger::W(MediaSessionService::TAG, "No activity for search: 0x%08x", ec);
        }
    }
    // } catch (ActivityNotFoundException e) {
    //     Log.w(TAG, "No activity for search: " + e);
    // } finally {
    //     if (needWakeLock) {
    //         mMediaEventWakeLock.release();
    //     }
    // }
    if (needWakeLock) {
        mHost->mMediaEventWakeLock->ReleaseLock();
    }
}

Boolean CSessionManagerImpl::IsVoiceKey(
    /* [in] */ Int32 keyCode)
{
    return keyCode == IKeyEvent::KEYCODE_HEADSETHOOK;
}

ECode CSessionManagerImpl::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return Object::ToString(str);
}

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos
