
#include <Elastos.Droid.Provider.h>
#include "elastos/droid/server/media/MediaSessionService.h"
#include "elastos/droid/server/Watchdog.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/Manifest.h"
#include "elastos/core/StringUtils.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Media::IAudioAttributesHelper;
using Elastos::Droid::Media::Session::IMediaControllerPlaybackInfo;
using Elastos::Droid::Media::Session::IMediaSessionToken;
using Elastos::Droid::Media::Session::CMediaSessionToken;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Server::Watchdog;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

//==============================================================================
//                  MediaSessionService::UserRecord
//==============================================================================

void MediaSessionService::UserRecord::StartLocked()
{
    // nothing for now
}

void MediaSessionService::UserRecord::StopLocked()
{
    // nothing for now
}

void MediaSessionService::UserRecord::DestroyLocked()
{
    List<AutoPtr<MediaSessionRecord> >::ReverseIterator rit = mSessions.RBegin();
    for (; rit != mSessions.REnd(); ++rit) {
        mHost->DestroySessionLocked(*rit);
    }
}

List<AutoPtr<MediaSessionRecord> >& MediaSessionService::UserRecord::GetSessionsLocked()
{
    return mSessions;
}

void MediaSessionService::UserRecord::AddSessionLocked(
    /* [in] */ MediaSessionRecord* session)
{
    mSessions.PushBack(session);
}

void MediaSessionService::UserRecord::RemoveSessionLocked(
    /* [in] */ MediaSessionRecord* session)
{
    mSessions.Remove(session);
}


//==============================================================================
//                  MediaSessionService::SessionsListenerRecord
//==============================================================================

CAR_INTERFACE_IMPL(MediaSessionService::SessionsListenerRecord, Object, IProxyDeathRecipient)

ECode MediaSessionService::SessionsListenerRecord::ProxyDied()
{
    AutoLock lock(mHost->mLock);
    mHost->mSessionsListeners.Remove(this);
    return NOERROR;
}


//==============================================================================
//                  MediaSessionService::SettingsObserver
//==============================================================================

MediaSessionService::SettingsObserver::SettingsObserver(
    /* [in] */ MediaSessionService* host)
    : mHost(host)
{
    ContentObserver::constructor(NULL);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetUriFor(ISettingsSecure::ENABLED_NOTIFICATION_LISTENERS, (IUri**)&mSecureSettingsUri);
}

void MediaSessionService::SettingsObserver::Observe()
{
    mHost->mContentResolver->RegisterContentObserver(mSecureSettingsUri,
            FALSE, this, IUserHandle::USER_ALL);
}

ECode MediaSessionService::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    mHost->UpdateActiveSessionListeners();
    return NOERROR;
}


//==============================================================================
//                  MediaSessionService::MessageHandler
//==============================================================================

const Int32 MediaSessionService::MessageHandler::MSG_SESSIONS_CHANGED;

ECode MediaSessionService::MessageHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_SESSIONS_CHANGED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->PushSessionsChanged(arg1);
            break;
        }
    }
    return NOERROR;
}

void MediaSessionService::MessageHandler::Post(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(what, arg1, arg2, (IMessage**)&msg);
    msg->SendToTarget();
}


//==============================================================================
//                  MediaSessionService
//==============================================================================

const String MediaSessionService::TAG("MediaSessionService");
const Boolean MediaSessionService::DEBUG;
const Int32 MediaSessionService::WAKELOCK_TIMEOUT;

MediaSessionService::MediaSessionService(
    /* [in] */ IContext* context)
    : mCurrentUserId(-1)
{
    SystemService::constructor(context);
    mHandler = new MessageHandler(this);
    CSessionManagerImpl::NewByFriend((Handle64)this, (CSessionManagerImpl**)&mSessionManagerImpl);
    mPriorityStack = new MediaSessionStack();
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(service);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, String("handleMediaEvent"), (IPowerManagerWakeLock**)&mMediaEventWakeLock);
}

CAR_INTERFACE_IMPL(MediaSessionService, SystemService, IWatchdogMonitor)

ECode MediaSessionService::OnStart()
{
    PublishBinderService(IContext::MEDIA_SESSION_SERVICE, mSessionManagerImpl);
    Watchdog::GetInstance()->AddMonitor(this);
    UpdateUser();
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&service);
    mKeyguardManager = IKeyguardManager::Probe(service);
    mAudioService = GetAudioService();
    context->GetContentResolver((IContentResolver**)&mContentResolver);
    mSettingsObserver = new SettingsObserver(this);
    mSettingsObserver->Observe();
    return NOERROR;
}

AutoPtr<IIAudioService> MediaSessionService::GetAudioService()
{
    AutoPtr<IInterface> b = ServiceManager::GetService(IContext::AUDIO_SERVICE);
    return IIAudioService::Probe(b);
}

void MediaSessionService::UpdateSession(
    /* [in] */ MediaSessionRecord* record)
{
    synchronized (mLock) {
        if (Find(mAllSessions.Begin(), mAllSessions.End(), AutoPtr<MediaSessionRecord>(record)) == mAllSessions.End()) {
            Slogger::D(TAG, "Unknown session updated. Ignoring.");
            return;
        }
        mPriorityStack->OnSessionStateChange(record);
    }
    mHandler->Post(MessageHandler::MSG_SESSIONS_CHANGED, record->GetUserId(), 0);
}

void MediaSessionService::OnSessionPlaystateChange(
    /* [in] */ MediaSessionRecord* record,
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
    Boolean updateSessions = FALSE;
    synchronized (mLock) {
        if (Find(mAllSessions.Begin(), mAllSessions.End(), AutoPtr<MediaSessionRecord>(record)) == mAllSessions.End()) {
            Slogger::D(TAG, "Unknown session changed playback state. Ignoring.");
            return;
        }
        updateSessions = mPriorityStack->OnPlaystateChange(record, oldState, newState);
    }
    if (updateSessions) {
        mHandler->Post(MessageHandler::MSG_SESSIONS_CHANGED, record->GetUserId(), 0);
    }
}

void MediaSessionService::OnSessionPlaybackTypeChanged(
    /* [in] */ MediaSessionRecord* record)
{
    synchronized (mLock) {
        if (Find(mAllSessions.Begin(), mAllSessions.End(), AutoPtr<MediaSessionRecord>(record)) == mAllSessions.End()) {
            Slogger::D(TAG, "Unknown session changed playback type. Ignoring.");
            return;
        }
        PushRemoteVolumeUpdateLocked(record->GetUserId());
    }
}

ECode MediaSessionService::OnStartUser(
    /* [in] */ Int32 userHandle)
{
    UpdateUser();
    return NOERROR;
}

ECode MediaSessionService::OnSwitchUser(
    /* [in] */ Int32 userId)
{
    UpdateUser();
    return NOERROR;
}

ECode MediaSessionService::OnStopUser(
    /* [in] */ Int32 userHandle)
{
    synchronized (mLock) {
        HashMap<Int32, AutoPtr<UserRecord> >::Iterator it = mUserRecords.Find(userHandle);
        AutoPtr<UserRecord> user;
        if (it != mUserRecords.End()) {
            user = it->mSecond;
        }
        if (user != NULL) {
            DestroyUserLocked(user);
        }
    }
    return NOERROR;
}

ECode MediaSessionService::Monitor()
{
    synchronized (mLock) {
        // Check for deadlock
    }
    return NOERROR;
}

ECode MediaSessionService::EnforcePhoneStatePermission(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 perm;
    if (context->CheckPermission(Elastos::Droid::Manifest::permission::MODIFY_PHONE_STATE,
            pid, uid, &perm), perm != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E(TAG, "Must hold the MODIFY_PHONE_STATE permission.");
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

void MediaSessionService::SessionDied(
    /* [in] */ MediaSessionRecord* session)
{
    synchronized (mLock) {
        DestroySessionLocked(session);
    }
}

void MediaSessionService::DestroySession(
    /* [in] */ MediaSessionRecord* session)
{
    synchronized (mLock) {
        DestroySessionLocked(session);
    }
}

void MediaSessionService::UpdateUser()
{
    synchronized (mLock) {
        AutoPtr<IActivityManagerHelper> helper;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
        Int32 userId;
        helper->GetCurrentUser(&userId);
        if (mCurrentUserId != userId) {
            Int32 oldUserId = mCurrentUserId;
            mCurrentUserId = userId; // do this first

            HashMap<Int32, AutoPtr<UserRecord> >::Iterator it = mUserRecords.Find(oldUserId);
            AutoPtr<UserRecord> oldUser;
            if (it != mUserRecords.End()) {
                oldUser = it->mSecond;
            }
            if (oldUser != NULL) {
                oldUser->StopLocked();
            }

            AutoPtr<UserRecord> newUser = GetOrCreateUser(userId);
            newUser->StartLocked();
        }
    }
}

void MediaSessionService::UpdateActiveSessionListeners()
{
    synchronized (mLock) {
        List<AutoPtr<SessionsListenerRecord> >::ReverseIterator rit = mSessionsListeners.RBegin();
        while (rit != mSessionsListeners.REnd()) {
            AutoPtr<SessionsListenerRecord> listener = *rit;
            // try {
            if (FAILED(EnforceMediaPermissions(listener->mComponentName, listener->mPid, listener->mUid,
                    listener->mUserId))) {
                Slogger::E(TAG, "ActiveSessionsListener %p is no longer authorized. Disconnecting.",
                        listener->mComponentName.Get());
                mSessionsListeners.Erase((++rit).GetBase());
                // try {
                AutoPtr<IList> sessions;
                CArrayList::New((IList**)&sessions);
                listener->mListener->OnActiveSessionsChanged(sessions);
                continue;
                // } catch (Exception e1) {
                //     // ignore
                // }
            }
            ++rit;
            // } catch (SecurityException e) {
            //     Log.i(TAG, "ActiveSessionsListener " + listener.mComponentName
            //             + " is no longer authorized. Disconnecting.");
            //     mSessionsListeners.remove(i);
            //     try {
            //         listener.mListener
            //                 .onActiveSessionsChanged(new ArrayList<MediaSession.Token>());
            //     } catch (Exception e1) {
            //         // ignore
            //     }
            // }
        }
    }
}

void MediaSessionService::DestroyUserLocked(
    /* [in] */ UserRecord* user)
{
    user->StopLocked();
    user->DestroyLocked();
    mUserRecords.Erase(user->mUserId);
}

void MediaSessionService::DestroySessionLocked(
    /* [in] */ MediaSessionRecord* session)
{
    Int32 userId = session->GetUserId();
    HashMap<Int32, AutoPtr<UserRecord> >::Iterator it = mUserRecords.Find(userId);
    AutoPtr<UserRecord> user;
    if (it != mUserRecords.End()) {
        user = it->mSecond;
    }
    if (user != NULL) {
        user->RemoveSessionLocked(session);
    }

    mPriorityStack->RemoveSession(session);
    mAllSessions.Remove(session);

    // try {
    AutoPtr<IBinder> b = IBinder::Probe(session->GetCallback());
    AutoPtr<IProxy> proxy = (IProxy*)b->Probe(EIID_IProxy);
    if (proxy != NULL) {
        Boolean result;
        proxy->UnlinkToDeath((IProxyDeathRecipient*)session, 0, &result);
    }
    // } catch (Exception e) {
    //     // ignore exceptions while destroying a session.
    // }
    session->OnDestroy();

    mHandler->Post(MessageHandler::MSG_SESSIONS_CHANGED, session->GetUserId(), 0);
}

ECode MediaSessionService::EnforcePackageName(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    if (TextUtils::IsEmpty(packageName)) {
        Slogger::E(TAG, "packageName may not be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<ArrayOf<String> > packages;
    pm->GetPackagesForUid(uid, (ArrayOf<String>**)&packages);
    Int32 packageCount = packages->GetLength();
    for (Int32 i = 0; i < packageCount; i++) {
        if (packageName.Equals((*packages)[i])) {
            return NOERROR;
        }
    }
    Slogger::E(TAG, "packageName is not owned by the calling process");
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode MediaSessionService::EnforceMediaPermissions(
    /* [in] */ IComponentName* compName,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 resolvedUserId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 perm;
    if (context->CheckPermission(Elastos::Droid::Manifest::permission::MEDIA_CONTENT_CONTROL,
            pid, uid, &perm), perm != IPackageManager::PERMISSION_GRANTED) {
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        Int32 userId;
        helper->GetUserId(uid, &userId);
        if (!IsEnabledNotificationListener(compName, userId, resolvedUserId)) {
            Slogger::E(TAG, "Missing permission to control media.");
            return E_SECURITY_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode MediaSessionService::EnforceStatusBarPermission(
    /* [in] */ const String& action,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 perm;
    if (context->CheckPermission(Elastos::Droid::Manifest::permission::STATUS_BAR_SERVICE,
            pid, uid, &perm), perm != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E(TAG, "Only system ui may %s", action.string());
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

Boolean MediaSessionService::IsEnabledNotificationListener(
    /* [in] */ IComponentName* compName,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 forUserId)
{
    if (userId != forUserId) {
        // You may not access another user's content as an enabled listener.
        return FALSE;
    }
    if (DEBUG) {
        Slogger::D(TAG, "Checking if enabled notification listener %p", compName);
    }
    if (compName != NULL) {
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        String enabledNotifListeners;
        settingsSecure->GetStringForUser(mContentResolver,
                ISettingsSecure::ENABLED_NOTIFICATION_LISTENERS,
                userId, &enabledNotifListeners);
        if (!enabledNotifListeners.IsNull()) {
            AutoPtr<ArrayOf<String> > components;
            StringUtils::Split(enabledNotifListeners, ":", (ArrayOf<String>**)&components);
            for (Int32 i = 0; i < components->GetLength(); i++) {
                AutoPtr<IComponentNameHelper> helper;
                CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
                AutoPtr<IComponentName> component;
                helper->UnflattenFromString((*components)[i], (IComponentName**)&component);
                if (component != NULL) {
                    Boolean equals;
                    if (IObject::Probe(compName)->Equals(component, &equals), equals) {
                        if (DEBUG) {
                            Slogger::D(TAG, "ok to get sessions: %p is authorized notification listener", component.Get());
                        }
                        return TRUE;
                    }
                }
            }
        }
        if (DEBUG) {
            Slogger::D(TAG, "not ok to get sessions, %pis not in list of ENABLED_NOTIFICATION_LISTENERS for user %d",
                    compName, userId);
        }
    }
    return FALSE;
}

ECode MediaSessionService::CreateSessionInternal(
    /* [in] */ Int32 callerPid,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 userId,
    /* [in] */ const String& callerPackageName,
    /* [in] */ IISessionCallback* cb,
    /* [in] */ const String& tag,
    /* [out] */ MediaSessionRecord** record)
{
    VALIDATE_NOT_NULL(record)
    ECode ec;
    synchronized (mLock) {
        ec = CreateSessionLocked(callerPid, callerUid, userId, callerPackageName, cb, tag, record);
    }
    return ec;
}

ECode MediaSessionService::CreateSessionLocked(
    /* [in] */ Int32 callerPid,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 userId,
    /* [in] */ const String& callerPackageName,
    /* [in] */ IISessionCallback* cb,
    /* [in] */ const String& tag,
    /* [out] */ MediaSessionRecord** record)
{
    VALIDATE_NOT_NULL(record)
    *record = NULL;

    AutoPtr<MediaSessionRecord> session = new MediaSessionRecord(callerPid, callerUid, userId,
            callerPackageName, cb, tag, this, mHandler);
    // try {
    AutoPtr<IBinder> b = IBinder::Probe(cb);
    AutoPtr<IProxy> proxy = (IProxy*)b->Probe(EIID_IProxy);
    if (proxy != NULL) {
        if (FAILED(proxy->LinkToDeath((IProxyDeathRecipient*)session.Get(), 0))) {
            Slogger::E(TAG, "Media Session owner died prematurely.");
            return E_RUNTIME_EXCEPTION;
        }
    }
    // } catch (RemoteException e) {
    //     throw new RuntimeException("Media Session owner died prematurely.", e);
    // }

    mAllSessions.PushBack(session);
    mPriorityStack->AddSession(session);

    AutoPtr<UserRecord> user = GetOrCreateUser(userId);
    user->AddSessionLocked(session);

    mHandler->Post(MessageHandler::MSG_SESSIONS_CHANGED, userId, 0);

    if (DEBUG) {
        Slogger::D(TAG, "Created session for package %s with tag %s", callerPackageName.string(), tag.string());
    }
    *record = session;
    REFCOUNT_ADD(*record)
    return NOERROR;
}

AutoPtr<MediaSessionService::UserRecord> MediaSessionService::GetOrCreateUser(
    /* [in] */ Int32 userId)
{
    HashMap<Int32, AutoPtr<UserRecord> >::Iterator it = mUserRecords.Find(userId);
    AutoPtr<UserRecord> user;
    if (it != mUserRecords.End()) {
        user = it->mSecond;
    }
    if (user == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        user = new UserRecord(context, userId, this);
        mUserRecords[userId] = user;
    }
    return user;
}

List<AutoPtr<MediaSessionService::SessionsListenerRecord> >::Iterator MediaSessionService::FindIndexOfSessionsListenerLocked(
    /* [in] */ IIActiveSessionsListener* listener)
{
    List<AutoPtr<SessionsListenerRecord> >::Iterator it = mSessionsListeners.Begin();
    for (; it != mSessionsListeners.End(); ++it) {
        if ((*it)->mListener.Get() == listener) {
            return it;
        }
    }
    return mSessionsListeners.End();
}

Boolean MediaSessionService::IsSessionDiscoverable(
    /* [in] */ MediaSessionRecord* record)
{
    // TODO probably want to check more than if it's active.
    return record->IsActive();
}

void MediaSessionService::PushSessionsChanged(
    /* [in] */ Int32 userId)
{
    synchronized (mLock) {
        AutoPtr<List<AutoPtr<MediaSessionRecord> > > records = mPriorityStack->GetActiveSessions(userId);
        if (records->IsEmpty() == FALSE) {
            RememberMediaButtonReceiverLocked((*records->Begin()));
        }
        AutoPtr<IList> tokens;
        CArrayList::New((IList**)&tokens);
        List<AutoPtr<MediaSessionRecord> >::Iterator it = records->Begin();
        for (; it != records->End(); ++it) {
            AutoPtr<IISessionController> controller = (*it)->GetControllerBinder();
            AutoPtr<IMediaSessionToken> token;
            CMediaSessionToken::New(controller, (IMediaSessionToken**)&token);
            tokens->Add(token);
        }
        PushRemoteVolumeUpdateLocked(userId);
        List<AutoPtr<SessionsListenerRecord> >::ReverseIterator rit = mSessionsListeners.RBegin();
        while (rit != mSessionsListeners.REnd()) {
            AutoPtr<SessionsListenerRecord> record = *rit;
            if (record->mUserId == IUserHandle::USER_ALL || record->mUserId == userId) {
                // try {
                if (FAILED(record->mListener->OnActiveSessionsChanged(tokens))) {
                    Slogger::W(TAG, "Dead ActiveSessionsListener in pushSessionsChanged, removing");
                    mSessionsListeners.Erase((++rit).GetBase());
                    continue;
                }
                // } catch (RemoteException e) {
                //     Log.w(TAG, "Dead ActiveSessionsListener in pushSessionsChanged, removing",
                //             e);
                //     mSessionsListeners.remove(i);
                // }
            }
            ++rit;
        }
    }
}

void MediaSessionService::PushRemoteVolumeUpdateLocked(
    /* [in] */ Int32 userId)
{
    if (mRvc != NULL) {
        // try {
        AutoPtr<MediaSessionRecord> record = mPriorityStack->GetDefaultRemoteSession(userId);
        mRvc->UpdateRemoteController(record == NULL ? NULL : record->GetControllerBinder());
        // } catch (RemoteException e) {
        //     Log.wtf(TAG, "Error sending default remote volume to sys ui.", e);
        // }
    }
}

void MediaSessionService::RememberMediaButtonReceiverLocked(
    /* [in] */ MediaSessionRecord* record)
{
    AutoPtr<IPendingIntent> receiver = record->GetMediaButtonReceiver();
    HashMap<Int32, AutoPtr<UserRecord> >::Iterator it = mUserRecords.Find(record->GetUserId());
    AutoPtr<UserRecord> user;
    if (it != mUserRecords.End()) {
        user = it->mSecond;
    }
    if (receiver != NULL && user != NULL) {
        user->mLastMediaButtonReceiver = receiver;
    }
}

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos
