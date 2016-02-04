/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.server.media;

using Elastos::Droid::Manifest;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntent.CanceledException;
using Elastos::Droid::Content::IActivityNotFoundException;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioSystem;
using Elastos::Droid::Media::IIAudioService;
using Elastos::Droid::Media::IIRemoteVolumeController;
using Elastos::Droid::Media::Isession.IActiveSessionsListener;
using Elastos::Droid::Media::Isession.ISession;
using Elastos::Droid::Media::Isession.ISessionCallback;
using Elastos::Droid::Media::Isession.ISessionManager;
using Elastos::Droid::Media::Isession.MediaController.PlaybackInfo;
using Elastos::Droid::Media::Isession.MediaSession;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Speech::IRecognizerIntent;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IKeyEvent;

using Elastos::Droid::Server::ISystemService;
using Elastos::Droid::Server::IWatchdog;
using Elastos::Droid::Server::IWatchdog.Monitor;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

/**
 * System implementation of MediaSessionManager
 */
public class MediaSessionService extends SystemService implements Monitor {
    private static const String TAG = "MediaSessionService";
    private static const Boolean DEBUG = Log->IsLoggable(TAG, Log.DEBUG);

    private static const Int32 WAKELOCK_TIMEOUT = 5000;

    private final SessionManagerImpl mSessionManagerImpl;
    private final MediaSessionStack mPriorityStack;

    private final ArrayList<MediaSessionRecord> mAllSessions = new ArrayList<MediaSessionRecord>();
    private final SparseArray<UserRecord> mUserRecords = new SparseArray<UserRecord>();
    private final ArrayList<SessionsListenerRecord> mSessionsListeners
            = new ArrayList<SessionsListenerRecord>();
    private final Object mLock = new Object();
    private final MessageHandler mHandler = new MessageHandler();
    private final PowerManager.WakeLock mMediaEventWakeLock;

    private KeyguardManager mKeyguardManager;
    private IAudioService mAudioService;
    private ContentResolver mContentResolver;
    private SettingsObserver mSettingsObserver;

    private Int32 mCurrentUserId = -1;

    // Used to notify system UI when remote volume was changed. TODO find a
    // better way to handle this.
    private IRemoteVolumeController mRvc;

    public MediaSessionService(Context context) {
        Super(context);
        mSessionManagerImpl = new SessionManagerImpl();
        mPriorityStack = new MediaSessionStack();
        PowerManager pm = (PowerManager) context->GetSystemService(Context.POWER_SERVICE);
        mMediaEventWakeLock = pm->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "handleMediaEvent");
    }

    //@Override
    CARAPI OnStart() {
        PublishBinderService(Context.MEDIA_SESSION_SERVICE, mSessionManagerImpl);
        Watchdog->GetInstance()->AddMonitor(this);
        UpdateUser();
        mKeyguardManager =
                (KeyguardManager) GetContext()->GetSystemService(Context.KEYGUARD_SERVICE);
        mAudioService = GetAudioService();
        mContentResolver = GetContext()->GetContentResolver();
        mSettingsObserver = new SettingsObserver();
        mSettingsObserver->Observe();
    }

    private IAudioService GetAudioService() {
        IBinder b = ServiceManager->GetService(Context.AUDIO_SERVICE);
        return IAudioService.Stub->AsInterface(b);
    }

    CARAPI UpdateSession(MediaSessionRecord record) {
        synchronized(mLock) {
            if (!mAllSessions->Contains(record)) {
                Logger::D(TAG, "Unknown session updated. Ignoring.");
                return;
            }
            mPriorityStack->OnSessionStateChange(record);
        }
        mHandler->Post(MessageHandler.MSG_SESSIONS_CHANGED, record->GetUserId(), 0);
    }

    CARAPI OnSessionPlaystateChange(MediaSessionRecord record, Int32 oldState, Int32 newState) {
        Boolean updateSessions = FALSE;
        synchronized(mLock) {
            if (!mAllSessions->Contains(record)) {
                Logger::D(TAG, "Unknown session changed playback state. Ignoring.");
                return;
            }
            updateSessions = mPriorityStack->OnPlaystateChange(record, oldState, newState);
        }
        if (updateSessions) {
            mHandler->Post(MessageHandler.MSG_SESSIONS_CHANGED, record->GetUserId(), 0);
        }
    }

    CARAPI OnSessionPlaybackTypeChanged(MediaSessionRecord record) {
        synchronized(mLock) {
            if (!mAllSessions->Contains(record)) {
                Logger::D(TAG, "Unknown session changed playback type. Ignoring.");
                return;
            }
            PushRemoteVolumeUpdateLocked(record->GetUserId());
        }
    }

    //@Override
    CARAPI OnStartUser(Int32 userHandle) {
        UpdateUser();
    }

    //@Override
    CARAPI OnSwitchUser(Int32 userHandle) {
        UpdateUser();
    }

    //@Override
    CARAPI OnStopUser(Int32 userHandle) {
        synchronized(mLock) {
            UserRecord user = mUserRecords->Get(userHandle);
            if (user != NULL) {
                DestroyUserLocked(user);
            }
        }
    }

    //@Override
    CARAPI Monitor() {
        synchronized(mLock) {
            // Check for deadlock
        }
    }

    protected void EnforcePhoneStatePermission(Int32 pid, Int32 uid) {
        if (GetContext()->CheckPermission(Manifest::permission::MODIFY_PHONE_STATE, pid, uid)
                != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException("Must hold the MODIFY_PHONE_STATE permission.");
        }
    }

    void SessionDied(MediaSessionRecord session) {
        synchronized(mLock) {
            DestroySessionLocked(session);
        }
    }

    void DestroySession(MediaSessionRecord session) {
        synchronized(mLock) {
            DestroySessionLocked(session);
        }
    }

    private void UpdateUser() {
        synchronized(mLock) {
            Int32 userId = ActivityManager->GetCurrentUser();
            if (mCurrentUserId != userId) {
                final Int32 oldUserId = mCurrentUserId;
                mCurrentUserId = userId; // do this first

                UserRecord oldUser = mUserRecords->Get(oldUserId);
                if (oldUser != NULL) {
                    oldUser->StopLocked();
                }

                UserRecord newUser = GetOrCreateUser(userId);
                newUser->StartLocked();
            }
        }
    }

    private void UpdateActiveSessionListeners() {
        synchronized(mLock) {
            for (Int32 i = mSessionsListeners->Size() - 1; i >= 0; i--) {
                SessionsListenerRecord listener = mSessionsListeners->Get(i);
                try {
                    EnforceMediaPermissions(listener.mComponentName, listener.mPid, listener.mUid,
                            listener.mUserId);
                } catch (SecurityException e) {
                    Logger::I(TAG, "ActiveSessionsListener " + listener.mComponentName
                            + " is no longer authorized. Disconnecting.");
                    mSessionsListeners->Remove(i);
                    try {
                        listener.mListener
                                .OnActiveSessionsChanged(new ArrayList<MediaSession.Token>());
                    } catch (Exception e1) {
                        // ignore
                    }
                }
            }
        }
    }

    /**
     * Stop the user and unbind from everything.
     *
     * @param user The user to dispose of
     */
    private void DestroyUserLocked(UserRecord user) {
        user->StopLocked();
        user->DestroyLocked();
        mUserRecords->Remove(user.mUserId);
    }

    /*
     * When a session is removed several things need to happen.
     * 1. We need to remove it from the relevant user.
     * 2. We need to remove it from the priority stack.
     * 3. We need to remove it from all sessions.
     * 4. If this is the system priority session we need to clear it.
     * 5. We need to unlink to death from the cb binder
     * 6. We need to tell the session to do any final cleanup (onDestroy)
     */
    private void DestroySessionLocked(MediaSessionRecord session) {
        Int32 userId = session->GetUserId();
        UserRecord user = mUserRecords->Get(userId);
        if (user != NULL) {
            user->RemoveSessionLocked(session);
        }

        mPriorityStack->RemoveSession(session);
        mAllSessions->Remove(session);

        try {
            session->GetCallback()->AsBinder().UnlinkToDeath(session, 0);
        } catch (Exception e) {
            // ignore exceptions while destroying a session.
        }
        session->OnDestroy();

        mHandler->Post(MessageHandler.MSG_SESSIONS_CHANGED, session->GetUserId(), 0);
    }

    private void EnforcePackageName(String packageName, Int32 uid) {
        if (TextUtils->IsEmpty(packageName)) {
            throw new IllegalArgumentException("packageName may not be empty");
        }
        String[] packages = GetContext()->GetPackageManager().GetPackagesForUid(uid);
        final Int32 packageCount = packages.length;
        for (Int32 i = 0; i < packageCount; i++) {
            if (packageName->Equals(packages[i])) {
                return;
            }
        }
        throw new IllegalArgumentException("packageName is not owned by the calling process");
    }

    /**
     * Checks a caller's authorization to register an IRemoteControlDisplay.
     * Authorization is granted if one of the following is TRUE:
     * <ul>
     * <li>the caller has Manifest::permission::MEDIA_CONTENT_CONTROL
     * permission</li>
     * <li>the caller's listener is one of the enabled notification listeners
     * for the caller's user</li>
     * </ul>
     */
    private void EnforceMediaPermissions(ComponentName compName, Int32 pid, Int32 uid,
            Int32 resolvedUserId) {
        if (GetContext()
                .CheckPermission(Manifest::permission::MEDIA_CONTENT_CONTROL, pid, uid)
                    != PackageManager.PERMISSION_GRANTED
                && !IsEnabledNotificationListener(compName, UserHandle->GetUserId(uid),
                        resolvedUserId)) {
            throw new SecurityException("Missing permission to control media.");
        }
    }

    private void EnforceStatusBarPermission(String action, Int32 pid, Int32 uid) {
        if (GetContext()->CheckPermission(Manifest::permission::STATUS_BAR_SERVICE,
                pid, uid) != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException("Only system ui may " + action);
        }
    }

    /**
     * This checks if the component is an enabled notification listener for the
     * specified user. Enabled components may only operate on behalf of the user
     * they're running as.
     *
     * @param compName The component that is enabled.
     * @param userId The user id of the caller.
     * @param forUserId The user id they're making the request on behalf of.
     * @return True if the component is enabled, FALSE otherwise
     */
    private Boolean IsEnabledNotificationListener(ComponentName compName, Int32 userId,
            Int32 forUserId) {
        if (userId != forUserId) {
            // You may not access another user's content as an enabled listener.
            return FALSE;
        }
        if (DEBUG) {
            Logger::D(TAG, "Checking if enabled notification listener " + compName);
        }
        if (compName != NULL) {
            final String enabledNotifListeners = Settings.Secure->GetStringForUser(mContentResolver,
                    Settings.Secure.ENABLED_NOTIFICATION_LISTENERS,
                    userId);
            if (enabledNotifListeners != NULL) {
                final String[] components = enabledNotifListeners->Split(":");
                for (Int32 i = 0; i < components.length; i++) {
                    final ComponentName component =
                            ComponentName->UnflattenFromString(components[i]);
                    if (component != NULL) {
                        if (compName->Equals(component)) {
                            if (DEBUG) {
                                Logger::D(TAG, "ok to get sessions: " + component +
                                        " is authorized notification listener");
                            }
                            return TRUE;
                        }
                    }
                }
            }
            if (DEBUG) {
                Logger::D(TAG, "not ok to get sessions, " + compName +
                        " is not in list of ENABLED_NOTIFICATION_LISTENERS for user " + userId);
            }
        }
        return FALSE;
    }

    private MediaSessionRecord CreateSessionInternal(Int32 callerPid, Int32 callerUid, Int32 userId,
            String callerPackageName, ISessionCallback cb, String tag) throws RemoteException {
        synchronized(mLock) {
            return CreateSessionLocked(callerPid, callerUid, userId, callerPackageName, cb, tag);
        }
    }

    /*
     * When a session is created the following things need to happen.
     * 1. Its callback binder needs a link to death
     * 2. It needs to be added to all sessions.
     * 3. It needs to be added to the priority stack.
     * 4. It needs to be added to the relevant user record.
     */
    private MediaSessionRecord CreateSessionLocked(Int32 callerPid, Int32 callerUid, Int32 userId,
            String callerPackageName, ISessionCallback cb, String tag) {

        final MediaSessionRecord session = new MediaSessionRecord(callerPid, callerUid, userId,
                callerPackageName, cb, tag, this, mHandler);
        try {
            cb->AsBinder()->LinkToDeath(session, 0);
        } catch (RemoteException e) {
            throw new RuntimeException("Media Session owner died prematurely.", e);
        }

        mAllSessions->Add(session);
        mPriorityStack->AddSession(session);

        UserRecord user = GetOrCreateUser(userId);
        user->AddSessionLocked(session);

        mHandler->Post(MessageHandler.MSG_SESSIONS_CHANGED, userId, 0);

        if (DEBUG) {
            Logger::D(TAG, "Created session for package " + callerPackageName + " with tag " + tag);
        }
        return session;
    }

    private UserRecord GetOrCreateUser(Int32 userId) {
        UserRecord user = mUserRecords->Get(userId);
        if (user == NULL) {
            user = new UserRecord(GetContext(), userId);
            mUserRecords->Put(userId, user);
        }
        return user;
    }

    private Int32 FindIndexOfSessionsListenerLocked(IActiveSessionsListener listener) {
        for (Int32 i = mSessionsListeners->Size() - 1; i >= 0; i--) {
            if (mSessionsListeners->Get(i).mListener == listener) {
                return i;
            }
        }
        return -1;
    }

    private Boolean IsSessionDiscoverable(MediaSessionRecord record) {
        // TODO probably want to check more than if it's active.
        return record->IsActive();
    }

    private void PushSessionsChanged(Int32 userId) {
        synchronized(mLock) {
            List<MediaSessionRecord> records = mPriorityStack->GetActiveSessions(userId);
            Int32 size = records->Size();
            if (size > 0) {
                RememberMediaButtonReceiverLocked(records->Get(0));
            }
            ArrayList<MediaSession.Token> tokens = new ArrayList<MediaSession.Token>();
            for (Int32 i = 0; i < size; i++) {
                tokens->Add(new MediaSession->Token(records->Get(i).GetControllerBinder()));
            }
            PushRemoteVolumeUpdateLocked(userId);
            for (Int32 i = mSessionsListeners->Size() - 1; i >= 0; i--) {
                SessionsListenerRecord record = mSessionsListeners->Get(i);
                if (record.mUserId == UserHandle.USER_ALL || record.mUserId == userId) {
                    try {
                        record.mListener->OnActiveSessionsChanged(tokens);
                    } catch (RemoteException e) {
                        Logger::W(TAG, "Dead ActiveSessionsListener in pushSessionsChanged, removing",
                                e);
                        mSessionsListeners->Remove(i);
                    }
                }
            }
        }
    }

    private void PushRemoteVolumeUpdateLocked(Int32 userId) {
        if (mRvc != NULL) {
            try {
                MediaSessionRecord record = mPriorityStack->GetDefaultRemoteSession(userId);
                mRvc->UpdateRemoteController(record == NULL ? NULL : record->GetControllerBinder());
            } catch (RemoteException e) {
                Log->Wtf(TAG, "Error sending default remote volume to sys ui.", e);
            }
        }
    }

    private void RememberMediaButtonReceiverLocked(MediaSessionRecord record) {
        PendingIntent receiver = record->GetMediaButtonReceiver();
        UserRecord user = mUserRecords->Get(record->GetUserId());
        if (receiver != NULL && user != NULL) {
            user.mLastMediaButtonReceiver = receiver;
        }
    }

    /**
     * Information about a particular user. The contents of this object is
     * guarded by mLock.
     */
    final class UserRecord {
        private final Int32 mUserId;
        private final ArrayList<MediaSessionRecord> mSessions = new ArrayList<MediaSessionRecord>();
        private PendingIntent mLastMediaButtonReceiver;

        public UserRecord(Context context, Int32 userId) {
            mUserId = userId;
        }

        CARAPI StartLocked() {
            // nothing for now
        }

        CARAPI StopLocked() {
            // nothing for now
        }

        CARAPI DestroyLocked() {
            for (Int32 i = mSessions->Size() - 1; i >= 0; i--) {
                MediaSessionRecord session = mSessions->Get(i);
                MediaSessionService.this->DestroySessionLocked(session);
            }
        }

        public ArrayList<MediaSessionRecord> GetSessionsLocked() {
            return mSessions;
        }

        CARAPI AddSessionLocked(MediaSessionRecord session) {
            mSessions->Add(session);
        }

        CARAPI RemoveSessionLocked(MediaSessionRecord session) {
            mSessions->Remove(session);
        }

        CARAPI DumpLocked(PrintWriter pw, String prefix) {
            pw->Println(prefix + "Record for user " + mUserId);
            String indent = prefix + "  ";
            pw->Println(indent + "MediaButtonReceiver:" + mLastMediaButtonReceiver);
            Int32 size = mSessions->Size();
            pw->Println(indent + size + " Sessions:");
            for (Int32 i = 0; i < size; i++) {
                // Just print the short version, the full session dump will
                // already be in the list of all sessions.
                pw->Println(indent + mSessions->Get(i).ToString());
            }
        }
    }

    final class SessionsListenerRecord implements IBinder.DeathRecipient {
        private final IActiveSessionsListener mListener;
        private final ComponentName mComponentName;
        private final Int32 mUserId;
        private final Int32 mPid;
        private final Int32 mUid;

        public SessionsListenerRecord(IActiveSessionsListener listener,
                ComponentName componentName,
                Int32 userId, Int32 pid, Int32 uid) {
            mListener = listener;
            mComponentName = componentName;
            mUserId = userId;
            mPid = pid;
            mUid = uid;
        }

        //@Override
        CARAPI BinderDied() {
            synchronized(mLock) {
                mSessionsListeners->Remove(this);
            }
        }
    }

    final class SettingsObserver extends ContentObserver {
        private final Uri mSecureSettingsUri = Settings.Secure->GetUriFor(
                Settings.Secure.ENABLED_NOTIFICATION_LISTENERS);

        private SettingsObserver() {
            Super(NULL);
        }

        private void Observe() {
            mContentResolver->RegisterContentObserver(mSecureSettingsUri,
                    FALSE, this, UserHandle.USER_ALL);
        }

        //@Override
        CARAPI OnChange(Boolean selfChange, Uri uri) {
            UpdateActiveSessionListeners();
        }
    }

    class SessionManagerImpl extends ISessionManager.Stub {
        private static const String EXTRA_WAKELOCK_ACQUIRED =
                "android.media.AudioService.WAKELOCK_ACQUIRED";
        private static const Int32 WAKELOCK_RELEASE_ON_FINISHED = 1980; // magic number

        private Boolean mVoiceButtonDown = FALSE;
        private Boolean mVoiceButtonHandled = FALSE;

        //@Override
        public ISession CreateSession(String packageName, ISessionCallback cb, String tag,
                Int32 userId) throws RemoteException {
            final Int32 pid = Binder->GetCallingPid();
            final Int32 uid = Binder->GetCallingUid();
            final Int64 token = Binder->ClearCallingIdentity();
            try {
                EnforcePackageName(packageName, uid);
                Int32 resolvedUserId = ActivityManager->HandleIncomingUser(pid, uid, userId,
                        FALSE /* allowAll */, TRUE /* requireFull */, "createSession", packageName);
                if (cb == NULL) {
                    throw new IllegalArgumentException("Controller callback cannot be NULL");
                }
                return CreateSessionInternal(pid, uid, resolvedUserId, packageName, cb, tag)
                        .GetSessionBinder();
            } finally {
                Binder->RestoreCallingIdentity(token);
            }
        }

        //@Override
        public List<IBinder> GetSessions(ComponentName componentName, Int32 userId) {
            final Int32 pid = Binder->GetCallingPid();
            final Int32 uid = Binder->GetCallingUid();
            final Int64 token = Binder->ClearCallingIdentity();

            try {
                Int32 resolvedUserId = VerifySessionsRequest(componentName, userId, pid, uid);
                ArrayList<IBinder> binders = new ArrayList<IBinder>();
                synchronized(mLock) {
                    ArrayList<MediaSessionRecord> records = mPriorityStack
                            .GetActiveSessions(resolvedUserId);
                    Int32 size = records->Size();
                    for (Int32 i = 0; i < size; i++) {
                        binders->Add(records->Get(i).GetControllerBinder()->AsBinder());
                    }
                }
                return binders;
            } finally {
                Binder->RestoreCallingIdentity(token);
            }
        }

        //@Override
        CARAPI AddSessionsListener(IActiveSessionsListener listener,
                ComponentName componentName, Int32 userId) throws RemoteException {
            final Int32 pid = Binder->GetCallingPid();
            final Int32 uid = Binder->GetCallingUid();
            final Int64 token = Binder->ClearCallingIdentity();

            try {
                Int32 resolvedUserId = VerifySessionsRequest(componentName, userId, pid, uid);
                synchronized(mLock) {
                    Int32 index = FindIndexOfSessionsListenerLocked(listener);
                    if (index != -1) {
                        Logger::W(TAG, "ActiveSessionsListener is already added, ignoring");
                        return;
                    }
                    SessionsListenerRecord record = new SessionsListenerRecord(listener,
                            componentName, resolvedUserId, pid, uid);
                    try {
                        listener->AsBinder()->LinkToDeath(record, 0);
                    } catch (RemoteException e) {
                        Logger::E(TAG, "ActiveSessionsListener is dead, ignoring it", e);
                        return;
                    }
                    mSessionsListeners->Add(record);
                }
            } finally {
                Binder->RestoreCallingIdentity(token);
            }
        }

        //@Override
        CARAPI RemoveSessionsListener(IActiveSessionsListener listener)
                throws RemoteException {
            synchronized(mLock) {
                Int32 index = FindIndexOfSessionsListenerLocked(listener);
                if (index != -1) {
                    SessionsListenerRecord record = mSessionsListeners->Remove(index);
                    try {
                        record.mListener->AsBinder()->UnlinkToDeath(record, 0);
                    } catch (Exception e) {
                        // ignore exceptions, the record is being removed
                    }
                }
            }
        }

        /**
         * Handles the dispatching of the media button events to one of the
         * registered listeners, or if there was none, broadcast an
         * ACTION_MEDIA_BUTTON intent to the rest of the system.
         *
         * @param keyEvent a non-NULL KeyEvent whose key code is one of the
         *            supported media buttons
         * @param needWakeLock TRUE if a PARTIAL_WAKE_LOCK needs to be held
         *            while this key event is dispatched.
         */
        //@Override
        CARAPI DispatchMediaKeyEvent(KeyEvent keyEvent, Boolean needWakeLock) {
            if (keyEvent == NULL || !KeyEvent->IsMediaKey(keyEvent->GetKeyCode())) {
                Logger::W(TAG, "Attempted to dispatch NULL or non-media key event.");
                return;
            }
            final Int32 pid = Binder->GetCallingPid();
            final Int32 uid = Binder->GetCallingUid();
            final Int64 token = Binder->ClearCallingIdentity();

            try {
                synchronized(mLock) {
                    MediaSessionRecord session = mPriorityStack
                            .GetDefaultMediaButtonSession(mCurrentUserId);
                    if (IsVoiceKey(keyEvent->GetKeyCode())) {
                        HandleVoiceKeyEventLocked(keyEvent, needWakeLock, session);
                    } else {
                        DispatchMediaKeyEventLocked(keyEvent, needWakeLock, session);
                    }
                }
            } finally {
                Binder->RestoreCallingIdentity(token);
            }
        }

        //@Override
        CARAPI DispatchAdjustVolume(Int32 suggestedStream, Int32 delta, Int32 flags)
                throws RemoteException {
            final Int32 pid = Binder->GetCallingPid();
            final Int32 uid = Binder->GetCallingUid();
            final Int64 token = Binder->ClearCallingIdentity();
            try {
                synchronized(mLock) {
                    MediaSessionRecord session = mPriorityStack
                            .GetDefaultVolumeSession(mCurrentUserId);
                    DispatchAdjustVolumeLocked(suggestedStream, delta, flags, session);
                }
            } finally {
                Binder->RestoreCallingIdentity(token);
            }
        }

        //@Override
        CARAPI SetRemoteVolumeController(IRemoteVolumeController rvc) {
            final Int32 pid = Binder->GetCallingPid();
            final Int32 uid = Binder->GetCallingUid();
            final Int64 token = Binder->ClearCallingIdentity();
            try {
                EnforceStatusBarPermission("listen for volume changes", pid, uid);
                mRvc = rvc;
            } finally {
                Binder->RestoreCallingIdentity(token);
            }
        }

        //@Override
        public Boolean IsGlobalPriorityActive() {
            return mPriorityStack->IsGlobalPriorityActive();
        }

        //@Override
        CARAPI Dump(FileDescriptor fd, final PrintWriter pw, String[] args) {
            if (GetContext()->CheckCallingOrSelfPermission(Manifest::permission::DUMP)
                    != PackageManager.PERMISSION_GRANTED) {
                pw->Println("Permission Denial: can't dump MediaSessionService from from pid="
                        + Binder->GetCallingPid()
                        + ", uid=" + Binder->GetCallingUid());
                return;
            }

            pw->Println("MEDIA SESSION SERVICE (dumpsys media_session)");
            pw->Println();

            synchronized(mLock) {
                Int32 count = mAllSessions->Size();
                pw->Println(count + " Sessions:");
                for (Int32 i = 0; i < count; i++) {
                    mAllSessions->Get(i).Dump(pw, "");
                    pw->Println();
                }
                mPriorityStack->Dump(pw, "");

                pw->Println("User Records:");
                count = mUserRecords->Size();
                for (Int32 i = 0; i < count; i++) {
                    UserRecord user = mUserRecords->Get(i);
                    user->DumpLocked(pw, "");
                }
            }
        }

        private Int32 VerifySessionsRequest(ComponentName componentName, Int32 userId, final Int32 pid,
                final Int32 uid) {
            String packageName = NULL;
            if (componentName != NULL) {
                // If they gave us a component name verify they own the
                // package
                packageName = componentName->GetPackageName();
                EnforcePackageName(packageName, uid);
            }
            // Check that they can make calls on behalf of the user and
            // get the final user id
            Int32 resolvedUserId = ActivityManager->HandleIncomingUser(pid, uid, userId,
                    TRUE /* allowAll */, TRUE /* requireFull */, "getSessions", packageName);
            // Check if they have the permissions or their component is
            // enabled for the user they're calling from.
            EnforceMediaPermissions(componentName, pid, uid, resolvedUserId);
            return resolvedUserId;
        }

        private void DispatchAdjustVolumeLocked(Int32 suggestedStream, Int32 direction, Int32 flags,
                MediaSessionRecord session) {
            if (DEBUG) {
                String description = session == NULL ? NULL : session->ToString();
                Logger::D(TAG, "Adjusting session " + description + " by " + direction + ". flags="
                        + flags + ", suggestedStream=" + suggestedStream);

            }
            if (session == NULL) {
                if ((flags & AudioManager.FLAG_ACTIVE_MEDIA_ONLY) != 0
                        && !AudioSystem->IsStreamActive(AudioManager.STREAM_MUSIC, 0)) {
                    if (DEBUG) {
                        Logger::D(TAG, "No active session to adjust, skipping media only volume event");
                    }
                    return;
                }
                try {
                    mAudioService->AdjustSuggestedStreamVolume(direction, suggestedStream, flags,
                            GetContext()->GetOpPackageName());
                } catch (RemoteException e) {
                    Logger::E(TAG, "Error adjusting default volume.", e);
                }
            } else {
                session->AdjustVolume(direction, flags, GetContext()->GetPackageName(),
                        UserHandle->MyUserId(), TRUE);
                if (session->GetPlaybackType() == PlaybackInfo.PLAYBACK_TYPE_REMOTE
                        && mRvc != NULL) {
                    try {
                        mRvc->RemoteVolumeChanged(session->GetControllerBinder(), flags);
                    } catch (Exception e) {
                        Log->Wtf(TAG, "Error sending volume change to system UI.", e);
                    }
                }
            }
        }

        private void HandleVoiceKeyEventLocked(KeyEvent keyEvent, Boolean needWakeLock,
                MediaSessionRecord session) {
            if (session != NULL && session->HasFlag(MediaSession.FLAG_EXCLUSIVE_GLOBAL_PRIORITY)) {
                // If the phone app has priority just give it the event
                DispatchMediaKeyEventLocked(keyEvent, needWakeLock, session);
                return;
            }
            Int32 action = keyEvent->GetAction();
            Boolean isLongPress = (keyEvent->GetFlags() & KeyEvent.FLAG_LONG_PRESS) != 0;
            if (action == KeyEvent.ACTION_DOWN) {
                if (keyEvent->GetRepeatCount() == 0) {
                    mVoiceButtonDown = TRUE;
                    mVoiceButtonHandled = FALSE;
                } else if (mVoiceButtonDown && !mVoiceButtonHandled && isLongPress) {
                    mVoiceButtonHandled = TRUE;
                    StartVoiceInput(needWakeLock);
                }
            } else if (action == KeyEvent.ACTION_UP) {
                if (mVoiceButtonDown) {
                    mVoiceButtonDown = FALSE;
                    if (!mVoiceButtonHandled && !keyEvent->IsCanceled()) {
                        // Resend the down then send this event through
                        KeyEvent downEvent = KeyEvent->ChangeAction(keyEvent, KeyEvent.ACTION_DOWN);
                        DispatchMediaKeyEventLocked(downEvent, needWakeLock, session);
                        DispatchMediaKeyEventLocked(keyEvent, needWakeLock, session);
                    }
                }
            }
        }

        private void DispatchMediaKeyEventLocked(KeyEvent keyEvent, Boolean needWakeLock,
                MediaSessionRecord session) {
            if (session != NULL) {
                if (DEBUG) {
                    Logger::D(TAG, "Sending media key to " + session->ToString());
                }
                if (needWakeLock) {
                    mKeyEventReceiver->AquireWakeLockLocked();
                }
                // If we don't need a wakelock use -1 as the id so we
                // won't release it later
                session->SendMediaButton(keyEvent,
                        needWakeLock ? mKeyEventReceiver.mLastTimeoutId : -1,
                        mKeyEventReceiver);
            } else {
                // Launch the last PendingIntent we had with priority
                Int32 userId = ActivityManager->GetCurrentUser();
                UserRecord user = mUserRecords->Get(userId);
                if (user.mLastMediaButtonReceiver != NULL) {
                    if (DEBUG) {
                        Logger::D(TAG, "Sending media key to last known PendingIntent");
                    }
                    if (needWakeLock) {
                        mKeyEventReceiver->AquireWakeLockLocked();
                    }
                    Intent mediaButtonIntent = new Intent(IIntent::ACTION_MEDIA_BUTTON);
                    mediaButtonIntent->PutExtra(Intent.EXTRA_KEY_EVENT, keyEvent);
                    try {
                        user.mLastMediaButtonReceiver->Send(GetContext(),
                                needWakeLock ? mKeyEventReceiver.mLastTimeoutId : -1,
                                mediaButtonIntent, mKeyEventReceiver, NULL);
                    } catch (CanceledException e) {
                        Logger::I(TAG, "Error sending key event to media button receiver "
                                + user.mLastMediaButtonReceiver, e);
                    }
                } else {
                    if (DEBUG) {
                        Logger::D(TAG, "Sending media key ordered broadcast");
                    }
                    if (needWakeLock) {
                        mMediaEventWakeLock->Acquire();
                    }
                    // Fallback to legacy behavior
                    Intent keyIntent = new Intent(IIntent::ACTION_MEDIA_BUTTON, NULL);
                    keyIntent->PutExtra(Intent.EXTRA_KEY_EVENT, keyEvent);
                    if (needWakeLock) {
                        keyIntent->PutExtra(EXTRA_WAKELOCK_ACQUIRED,
                                WAKELOCK_RELEASE_ON_FINISHED);
                    }
                    GetContext()->SendOrderedBroadcastAsUser(keyIntent, UserHandle.ALL,
                            NULL, mKeyEventDone, mHandler, Activity.RESULT_OK, NULL, NULL);
                }
            }
        }

        private void StartVoiceInput(Boolean needWakeLock) {
            Intent voiceIntent = NULL;
            // select which type of search to launch:
            // - screen on and device unlocked: action is ACTION_WEB_SEARCH
            // - device locked or screen off: action is
            // ACTION_VOICE_SEARCH_HANDS_FREE
            // with EXTRA_SECURE set to TRUE if the device is securely locked
            PowerManager pm = (PowerManager) GetContext()->GetSystemService(Context.POWER_SERVICE);
            Boolean isLocked = mKeyguardManager != NULL && mKeyguardManager->IsKeyguardLocked();
            if (!isLocked && pm->IsScreenOn()) {
                voiceIntent = new Intent(android.speech.RecognizerIIntent::ACTION_WEB_SEARCH);
                Logger::I(TAG, "voice-based interactions: about to use ACTION_WEB_SEARCH");
            } else {
                voiceIntent = new Intent(RecognizerIIntent::ACTION_VOICE_SEARCH_HANDS_FREE);
                voiceIntent->PutExtra(RecognizerIntent.EXTRA_SECURE,
                        isLocked && mKeyguardManager->IsKeyguardSecure());
                Logger::I(TAG, "voice-based interactions: about to use ACTION_VOICE_SEARCH_HANDS_FREE");
            }
            // start the search activity
            if (needWakeLock) {
                mMediaEventWakeLock->Acquire();
            }
            try {
                if (voiceIntent != NULL) {
                    voiceIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                            | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
                    GetContext()->StartActivityAsUser(voiceIntent, UserHandle.CURRENT);
                }
            } catch (ActivityNotFoundException e) {
                Logger::W(TAG, "No activity for search: " + e);
            } finally {
                if (needWakeLock) {
                    mMediaEventWakeLock->Release();
                }
            }
        }

        private Boolean IsVoiceKey(Int32 keyCode) {
            return keyCode == KeyEvent.KEYCODE_HEADSETHOOK;
        }

        private KeyEventWakeLockReceiver mKeyEventReceiver = new KeyEventWakeLockReceiver(mHandler);

        class KeyEventWakeLockReceiver extends ResultReceiver implements Runnable,
                PendingIntent.OnFinished {
            private final Handler mHandler;
            private Int32 mRefCount = 0;
            private Int32 mLastTimeoutId = 0;

            public KeyEventWakeLockReceiver(Handler handler) {
                Super(handler);
                mHandler = handler;
            }

            CARAPI OnTimeout() {
                synchronized(mLock) {
                    if (mRefCount == 0) {
                        // We've already released it, so just return
                        return;
                    }
                    mLastTimeoutId++;
                    mRefCount = 0;
                    ReleaseWakeLockLocked();
                }
            }

            CARAPI AquireWakeLockLocked() {
                if (mRefCount == 0) {
                    mMediaEventWakeLock->Acquire();
                }
                mRefCount++;
                mHandler->RemoveCallbacks(this);
                mHandler->PostDelayed(this, WAKELOCK_TIMEOUT);

            }

            //@Override
            CARAPI Run() {
                OnTimeout();
            }

            //@Override
            protected void OnReceiveResult(Int32 resultCode, Bundle resultData) {
                if (resultCode < mLastTimeoutId) {
                    // Ignore results from calls that were before the last
                    // timeout, just in case.
                    return;
                } else {
                    synchronized(mLock) {
                        if (mRefCount > 0) {
                            mRefCount--;
                            if (mRefCount == 0) {
                                ReleaseWakeLockLocked();
                            }
                        }
                    }
                }
            }

            private void ReleaseWakeLockLocked() {
                mMediaEventWakeLock->Release();
                mHandler->RemoveCallbacks(this);
            }

            //@Override
            CARAPI OnSendFinished(PendingIntent pendingIntent, Intent intent, Int32 resultCode,
                    String resultData, Bundle resultExtras) {
                OnReceiveResult(resultCode, NULL);
            }
        };

        BroadcastReceiver mKeyEventDone = new BroadcastReceiver() {
            //@Override
            CARAPI OnReceive(Context context, Intent intent) {
                if (intent == NULL) {
                    return;
                }
                Bundle extras = intent->GetExtras();
                if (extras == NULL) {
                    return;
                }
                synchronized(mLock) {
                    if (extras->ContainsKey(EXTRA_WAKELOCK_ACQUIRED)
                            && mMediaEventWakeLock->IsHeld()) {
                        mMediaEventWakeLock->Release();
                    }
                }
            }
        };
    }

    final class MessageHandler extends Handler {
        private static const Int32 MSG_SESSIONS_CHANGED = 1;

        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case MSG_SESSIONS_CHANGED:
                    PushSessionsChanged(msg.arg1);
                    break;
            }
        }

        CARAPI Post(Int32 what, Int32 arg1, Int32 arg2) {
            ObtainMessage(what, arg1, arg2).SendToTarget();
        }
    }
}
