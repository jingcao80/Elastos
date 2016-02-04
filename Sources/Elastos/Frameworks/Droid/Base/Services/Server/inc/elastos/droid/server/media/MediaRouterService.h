/*
 * Copyright (C) 2013 The Android Open Source Project
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

using Elastos::Droid::Server::IWatchdog;

using Elastos::Droid::Manifest;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Media::IAudioSystem;
using Elastos::Droid::Media::IIMediaRouterClient;
using Elastos::Droid::Media::IIMediaRouterService;
using Elastos::Droid::Media::IMediaRouter;
using Elastos::Droid::Media::IMediaRouterClientState;
using Elastos::Droid::Media::IRemoteDisplayState;
using Elastos::Droid::Media::IRemoteDisplayState.RemoteDisplayInfo;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::ITimeUtils;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;
using Elastos::Utility::IObjects;

/**
 * Provides a mechanism for discovering media routes and manages media playback
 * behalf of applications.
 * <p>
 * Currently supports discovering remote displays via remote display provider
 * services that have been registered by applications.
 * </p>
 */
public class MediaRouterService extends IMediaRouterService.Stub
        implements Watchdog.Monitor {
    private static const String TAG = "MediaRouterService";
    private static const Boolean DEBUG = Log->IsLoggable(TAG, Log.DEBUG);

    /**
     * Timeout in milliseconds for a selected route to transition from a
     * disconnected state to a connecting state.  If we don't observe any
     * progress within this interval, then we will give up and unselect the route.
     */
    static const Int64 CONNECTING_TIMEOUT = 5000;

    /**
     * Timeout in milliseconds for a selected route to transition from a
     * connecting state to a connected state.  If we don't observe any
     * progress within this interval, then we will give up and unselect the route.
     */
    static const Int64 CONNECTED_TIMEOUT = 60000;

    private final Context mContext;

    // State guarded by mLock.
    private final Object mLock = new Object();
    private final SparseArray<UserRecord> mUserRecords = new SparseArray<UserRecord>();
    private final ArrayMap<IBinder, ClientRecord> mAllClientRecords =
            new ArrayMap<IBinder, ClientRecord>();
    private Int32 mCurrentUserId = -1;

    public MediaRouterService(Context context) {
        mContext = context;
        Watchdog->GetInstance()->AddMonitor(this);
    }

    CARAPI SystemRunning() {
        IntentFilter filter = new IntentFilter(IIntent::ACTION_USER_SWITCHED);
        mContext->RegisterReceiver(new BroadcastReceiver() {
            //@Override
            CARAPI OnReceive(Context context, Intent intent) {
                if (intent->GetAction()->Equals(IIntent::ACTION_USER_SWITCHED)) {
                    SwitchUser();
                }
            }
        }, filter);

        SwitchUser();
    }

    //@Override
    CARAPI Monitor() {
        synchronized(mLock) { /* check for deadlock */ }
    }

    // Binder call
    //@Override
    CARAPI RegisterClientAsUser(IMediaRouterClient client, String packageName, Int32 userId) {
        if (client == NULL) {
            throw new IllegalArgumentException("client must not be NULL");
        }

        final Int32 uid = Binder->GetCallingUid();
        if (!ValidatePackageName(uid, packageName)) {
            throw new SecurityException("packageName must match the calling uid");
        }

        final Int32 pid = Binder->GetCallingPid();
        final Int32 resolvedUserId = ActivityManager->HandleIncomingUser(pid, uid, userId,
                FALSE /*allowAll*/, TRUE /*requireFull*/, "registerClientAsUser", packageName);
        final Boolean trusted = mContext->CheckCallingOrSelfPermission(
                Manifest::permission::CONFIGURE_WIFI_DISPLAY) ==
                PackageManager.PERMISSION_GRANTED;
        final Int64 token = Binder->ClearCallingIdentity();
        try {
            synchronized(mLock) {
                RegisterClientLocked(client, pid, packageName, resolvedUserId, trusted);
            }
        } finally {
            Binder->RestoreCallingIdentity(token);
        }
    }

    // Binder call
    //@Override
    CARAPI UnregisterClient(IMediaRouterClient client) {
        if (client == NULL) {
            throw new IllegalArgumentException("client must not be NULL");
        }

        final Int64 token = Binder->ClearCallingIdentity();
        try {
            synchronized(mLock) {
                UnregisterClientLocked(client, FALSE);
            }
        } finally {
            Binder->RestoreCallingIdentity(token);
        }
    }

    // Binder call
    //@Override
    public MediaRouterClientState GetState(IMediaRouterClient client) {
        if (client == NULL) {
            throw new IllegalArgumentException("client must not be NULL");
        }

        final Int64 token = Binder->ClearCallingIdentity();
        try {
            synchronized(mLock) {
                return GetStateLocked(client);
            }
        } finally {
            Binder->RestoreCallingIdentity(token);
        }
    }

    // Binder call
    //@Override
    CARAPI SetDiscoveryRequest(IMediaRouterClient client,
            Int32 routeTypes, Boolean activeScan) {
        if (client == NULL) {
            throw new IllegalArgumentException("client must not be NULL");
        }

        final Int64 token = Binder->ClearCallingIdentity();
        try {
            synchronized(mLock) {
                SetDiscoveryRequestLocked(client, routeTypes, activeScan);
            }
        } finally {
            Binder->RestoreCallingIdentity(token);
        }
    }

    // Binder call
    // A NULL routeId means that the client wants to unselect its current route.
    // The explicit flag indicates whether the change was explicitly requested by the
    // user or the application which may cause changes to propagate out to the rest
    // of the system.  Should be FALSE when the change is in response to a new globally
    // selected route or a default selection.
    //@Override
    CARAPI SetSelectedRoute(IMediaRouterClient client, String routeId, Boolean explicit) {
        if (client == NULL) {
            throw new IllegalArgumentException("client must not be NULL");
        }

        final Int64 token = Binder->ClearCallingIdentity();
        try {
            synchronized(mLock) {
                SetSelectedRouteLocked(client, routeId, explicit);
            }
        } finally {
            Binder->RestoreCallingIdentity(token);
        }
    }

    // Binder call
    //@Override
    CARAPI RequestSetVolume(IMediaRouterClient client, String routeId, Int32 volume) {
        if (client == NULL) {
            throw new IllegalArgumentException("client must not be NULL");
        }
        if (routeId == NULL) {
            throw new IllegalArgumentException("routeId must not be NULL");
        }

        final Int64 token = Binder->ClearCallingIdentity();
        try {
            synchronized(mLock) {
                RequestSetVolumeLocked(client, routeId, volume);
            }
        } finally {
            Binder->RestoreCallingIdentity(token);
        }
    }

    // Binder call
    //@Override
    CARAPI RequestUpdateVolume(IMediaRouterClient client, String routeId, Int32 direction) {
        if (client == NULL) {
            throw new IllegalArgumentException("client must not be NULL");
        }
        if (routeId == NULL) {
            throw new IllegalArgumentException("routeId must not be NULL");
        }

        final Int64 token = Binder->ClearCallingIdentity();
        try {
            synchronized(mLock) {
                RequestUpdateVolumeLocked(client, routeId, direction);
            }
        } finally {
            Binder->RestoreCallingIdentity(token);
        }
    }

    // Binder call
    //@Override
    CARAPI Dump(FileDescriptor fd, final PrintWriter pw, String[] args) {
        if (mContext->CheckCallingOrSelfPermission(Manifest::permission::DUMP)
                != PackageManager.PERMISSION_GRANTED) {
            pw->Println("Permission Denial: can't dump MediaRouterService from from pid="
                    + Binder->GetCallingPid()
                    + ", uid=" + Binder->GetCallingUid());
            return;
        }

        pw->Println("MEDIA ROUTER SERVICE (dumpsys media_router)");
        pw->Println();
        pw->Println("Global state");
        pw->Println("  mCurrentUserId=" + mCurrentUserId);

        synchronized(mLock) {
            final Int32 count = mUserRecords->Size();
            for (Int32 i = 0; i < count; i++) {
                UserRecord userRecord = mUserRecords->ValueAt(i);
                pw->Println();
                userRecord->Dump(pw, "");
            }
        }
    }

    void SwitchUser() {
        synchronized(mLock) {
            Int32 userId = ActivityManager->GetCurrentUser();
            if (mCurrentUserId != userId) {
                final Int32 oldUserId = mCurrentUserId;
                mCurrentUserId = userId; // do this first

                UserRecord oldUser = mUserRecords->Get(oldUserId);
                if (oldUser != NULL) {
                    oldUser.mHandler->SendEmptyMessage(UserHandler.MSG_STOP);
                    DisposeUserIfNeededLocked(oldUser); // since no longer current user
                }

                UserRecord newUser = mUserRecords->Get(userId);
                if (newUser != NULL) {
                    newUser.mHandler->SendEmptyMessage(UserHandler.MSG_START);
                }
            }
        }
    }

    void ClientDied(ClientRecord clientRecord) {
        synchronized(mLock) {
            UnregisterClientLocked(clientRecord.mClient, TRUE);
        }
    }

    private void RegisterClientLocked(IMediaRouterClient client,
            Int32 pid, String packageName, Int32 userId, Boolean trusted) {
        final IBinder binder = client->AsBinder();
        ClientRecord clientRecord = mAllClientRecords->Get(binder);
        if (clientRecord == NULL) {
            Boolean newUser = FALSE;
            UserRecord userRecord = mUserRecords->Get(userId);
            if (userRecord == NULL) {
                userRecord = new UserRecord(userId);
                newUser = TRUE;
            }
            clientRecord = new ClientRecord(userRecord, client, pid, packageName, trusted);
            try {
                binder->LinkToDeath(clientRecord, 0);
            } catch (RemoteException ex) {
                throw new RuntimeException("Media router client died prematurely.", ex);
            }

            if (newUser) {
                mUserRecords->Put(userId, userRecord);
                InitializeUserLocked(userRecord);
            }

            userRecord.mClientRecords->Add(clientRecord);
            mAllClientRecords->Put(binder, clientRecord);
            InitializeClientLocked(clientRecord);
        }
    }

    private void UnregisterClientLocked(IMediaRouterClient client, Boolean died) {
        ClientRecord clientRecord = mAllClientRecords->Remove(client->AsBinder());
        if (clientRecord != NULL) {
            UserRecord userRecord = clientRecord.mUserRecord;
            userRecord.mClientRecords->Remove(clientRecord);
            DisposeClientLocked(clientRecord, died);
            DisposeUserIfNeededLocked(userRecord); // since client removed from user
        }
    }

    private MediaRouterClientState GetStateLocked(IMediaRouterClient client) {
        ClientRecord clientRecord = mAllClientRecords->Get(client->AsBinder());
        if (clientRecord != NULL) {
            return clientRecord->GetState();
        }
        return NULL;
    }

    private void SetDiscoveryRequestLocked(IMediaRouterClient client,
            Int32 routeTypes, Boolean activeScan) {
        final IBinder binder = client->AsBinder();
        ClientRecord clientRecord = mAllClientRecords->Get(binder);
        if (clientRecord != NULL) {
            // Only let the system discover remote display routes for now.
            if (!clientRecord.mTrusted) {
                routeTypes &= ~MediaRouter.ROUTE_TYPE_REMOTE_DISPLAY;
            }

            if (clientRecord.mRouteTypes != routeTypes
                    || clientRecord.mActiveScan != activeScan) {
                if (DEBUG) {
                    Slogger::D(TAG, clientRecord + ": Set discovery request, routeTypes=0x"
                            + Integer->ToHexString(routeTypes) + ", activeScan=" + activeScan);
                }
                clientRecord.mRouteTypes = routeTypes;
                clientRecord.mActiveScan = activeScan;
                clientRecord.mUserRecord.mHandler->SendEmptyMessage(
                        UserHandler.MSG_UPDATE_DISCOVERY_REQUEST);
            }
        }
    }

    private void SetSelectedRouteLocked(IMediaRouterClient client,
            String routeId, Boolean explicit) {
        ClientRecord clientRecord = mAllClientRecords->Get(client->AsBinder());
        if (clientRecord != NULL) {
            final String oldRouteId = clientRecord.mSelectedRouteId;
            if (!Objects->Equals(routeId, oldRouteId)) {
                if (DEBUG) {
                    Slogger::D(TAG, clientRecord + ": Set selected route, routeId=" + routeId
                            + ", oldRouteId=" + oldRouteId
                            + ", explicit=" + explicit);
                }

                clientRecord.mSelectedRouteId = routeId;
                if (explicit) {
                    // Any app can disconnect from the globally selected route.
                    if (oldRouteId != NULL) {
                        clientRecord.mUserRecord.mHandler->ObtainMessage(
                                UserHandler.MSG_UNSELECT_ROUTE, oldRouteId).SendToTarget();
                    }
                    // Only let the system connect to new global routes for now.
                    // A similar check exists in the display manager for wifi display.
                    if (routeId != NULL && clientRecord.mTrusted) {
                        clientRecord.mUserRecord.mHandler->ObtainMessage(
                                UserHandler.MSG_SELECT_ROUTE, routeId).SendToTarget();
                    }
                }
            }
        }
    }

    private void RequestSetVolumeLocked(IMediaRouterClient client,
            String routeId, Int32 volume) {
        final IBinder binder = client->AsBinder();
        ClientRecord clientRecord = mAllClientRecords->Get(binder);
        if (clientRecord != NULL) {
            clientRecord.mUserRecord.mHandler->ObtainMessage(
                    UserHandler.MSG_REQUEST_SET_VOLUME, volume, 0, routeId).SendToTarget();
        }
    }

    private void RequestUpdateVolumeLocked(IMediaRouterClient client,
            String routeId, Int32 direction) {
        final IBinder binder = client->AsBinder();
        ClientRecord clientRecord = mAllClientRecords->Get(binder);
        if (clientRecord != NULL) {
            clientRecord.mUserRecord.mHandler->ObtainMessage(
                    UserHandler.MSG_REQUEST_UPDATE_VOLUME, direction, 0, routeId).SendToTarget();
        }
    }

    private void InitializeUserLocked(UserRecord userRecord) {
        if (DEBUG) {
            Slogger::D(TAG, userRecord + ": Initialized");
        }
        if (userRecord.mUserId == mCurrentUserId) {
            userRecord.mHandler->SendEmptyMessage(UserHandler.MSG_START);
        }
    }

    private void DisposeUserIfNeededLocked(UserRecord userRecord) {
        // If there are no records left and the user is no longer current then go ahead
        // and purge the user record and all of its associated state.  If the user is current
        // then leave it alone since we might be connected to a route or want to query
        // the same route information again soon.
        if (userRecord.mUserId != mCurrentUserId
                && userRecord.mClientRecords->IsEmpty()) {
            if (DEBUG) {
                Slogger::D(TAG, userRecord + ": Disposed");
            }
            mUserRecords->Remove(userRecord.mUserId);
            // Note: User already stopped (by switchUser) so no need to send stop message here.
        }
    }

    private void InitializeClientLocked(ClientRecord clientRecord) {
        if (DEBUG) {
            Slogger::D(TAG, clientRecord + ": Registered");
        }
    }

    private void DisposeClientLocked(ClientRecord clientRecord, Boolean died) {
        if (DEBUG) {
            if (died) {
                Slogger::D(TAG, clientRecord + ": Died!");
            } else {
                Slogger::D(TAG, clientRecord + ": Unregistered");
            }
        }
        if (clientRecord.mRouteTypes != 0 || clientRecord.mActiveScan) {
            clientRecord.mUserRecord.mHandler->SendEmptyMessage(
                    UserHandler.MSG_UPDATE_DISCOVERY_REQUEST);
        }
        clientRecord->Dispose();
    }

    private Boolean ValidatePackageName(Int32 uid, String packageName) {
        if (packageName != NULL) {
            String[] packageNames = mContext->GetPackageManager()->GetPackagesForUid(uid);
            if (packageNames != NULL) {
                for (String n : packageNames) {
                    if (n->Equals(packageName)) {
                        return TRUE;
                    }
                }
            }
        }
        return FALSE;
    }

    /**
     * Information about a particular client of the media router.
     * The contents of this object is guarded by mLock.
     */
    final class ClientRecord implements DeathRecipient {
        public final UserRecord mUserRecord;
        public final IMediaRouterClient mClient;
        public final Int32 mPid;
        public final String mPackageName;
        public final Boolean mTrusted;

        public Int32 mRouteTypes;
        public Boolean mActiveScan;
        public String mSelectedRouteId;

        public ClientRecord(UserRecord userRecord, IMediaRouterClient client,
                Int32 pid, String packageName, Boolean trusted) {
            mUserRecord = userRecord;
            mClient = client;
            mPid = pid;
            mPackageName = packageName;
            mTrusted = trusted;
        }

        CARAPI Dispose() {
            mClient->AsBinder()->UnlinkToDeath(this, 0);
        }

        //@Override
        CARAPI BinderDied() {
            ClientDied(this);
        }

        MediaRouterClientState GetState() {
            return mTrusted ? mUserRecord.mTrustedState : mUserRecord.mUntrustedState;
        }

        CARAPI Dump(PrintWriter pw, String prefix) {
            pw->Println(prefix + this);

            final String indent = prefix + "  ";
            pw->Println(indent + "mTrusted=" + mTrusted);
            pw->Println(indent + "mRouteTypes=0x" + Integer->ToHexString(mRouteTypes));
            pw->Println(indent + "mActiveScan=" + mActiveScan);
            pw->Println(indent + "mSelectedRouteId=" + mSelectedRouteId);
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "Client " + mPackageName + " (pid " + mPid + ")";
        }
    }

    /**
     * Information about a particular user.
     * The contents of this object is guarded by mLock.
     */
    final class UserRecord {
        public final Int32 mUserId;
        public final ArrayList<ClientRecord> mClientRecords = new ArrayList<ClientRecord>();
        public final UserHandler mHandler;
        public MediaRouterClientState mTrustedState;
        public MediaRouterClientState mUntrustedState;

        public UserRecord(Int32 userId) {
            mUserId = userId;
            mHandler = new UserHandler(MediaRouterService.this, this);
        }

        CARAPI Dump(final PrintWriter pw, String prefix) {
            pw->Println(prefix + this);

            final String indent = prefix + "  ";
            final Int32 clientCount = mClientRecords->Size();
            if (clientCount != 0) {
                for (Int32 i = 0; i < clientCount; i++) {
                    mClientRecords->Get(i).Dump(pw, indent);
                }
            } else {
                pw->Println(indent + "<no clients>");
            }

            pw->Println(indent + "State");
            pw->Println(indent + "mTrustedState=" + mTrustedState);
            pw->Println(indent + "mUntrustedState=" + mUntrustedState);

            if (!mHandler->RunWithScissors(new Runnable() {
                //@Override
                CARAPI Run() {
                    mHandler->Dump(pw, indent);
                }
            }, 1000)) {
                pw->Println(indent + "<could not dump handler state>");
            }
         }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "User " + mUserId;
        }
    }

    /**
     * Media router handler
     * <p>
     * Since remote display providers are designed to be single-threaded by nature,
     * this class encapsulates all of the associated functionality and exports state
     * to the service as it evolves.
     * </p><p>
     * One important task of this class is to keep track of the current globally selected
     * route id for certain routes that have global effects, such as remote displays.
     * Global route selections override local selections made within apps.  The change
     * is propagated to all apps so that they are all in sync.  Synchronization works
     * both ways.  Whenever the globally selected route is explicitly unselected by any
     * app, then it becomes unselected globally and all apps are informed.
     * </p><p>
     * This class is currently hardcoded to work with remote display providers but
     * it is intended to be eventually extended to support more general route providers
     * similar to the support library media router.
     * </p>
     */
    static const class UserHandler extends Handler
            implements RemoteDisplayProviderWatcher.Callback,
            RemoteDisplayProviderProxy.Callback {
        public static const Int32 MSG_START = 1;
        public static const Int32 MSG_STOP = 2;
        public static const Int32 MSG_UPDATE_DISCOVERY_REQUEST = 3;
        public static const Int32 MSG_SELECT_ROUTE = 4;
        public static const Int32 MSG_UNSELECT_ROUTE = 5;
        public static const Int32 MSG_REQUEST_SET_VOLUME = 6;
        public static const Int32 MSG_REQUEST_UPDATE_VOLUME = 7;
        private static const Int32 MSG_UPDATE_CLIENT_STATE = 8;
        private static const Int32 MSG_CONNECTION_TIMED_OUT = 9;

        private static const Int32 TIMEOUT_REASON_NOT_AVAILABLE = 1;
        private static const Int32 TIMEOUT_REASON_CONNECTION_LOST = 2;
        private static const Int32 TIMEOUT_REASON_WAITING_FOR_CONNECTING = 3;
        private static const Int32 TIMEOUT_REASON_WAITING_FOR_CONNECTED = 4;

        // The relative order of these constants is important and expresses progress
        // through the process of connecting to a route.
        private static const Int32 PHASE_NOT_AVAILABLE = -1;
        private static const Int32 PHASE_NOT_CONNECTED = 0;
        private static const Int32 PHASE_CONNECTING = 1;
        private static const Int32 PHASE_CONNECTED = 2;

        private final MediaRouterService mService;
        private final UserRecord mUserRecord;
        private final RemoteDisplayProviderWatcher mWatcher;
        private final ArrayList<ProviderRecord> mProviderRecords =
                new ArrayList<ProviderRecord>();
        private final ArrayList<IMediaRouterClient> mTempClients =
                new ArrayList<IMediaRouterClient>();

        private Boolean mRunning;
        private Int32 mDiscoveryMode = RemoteDisplayState.DISCOVERY_MODE_NONE;
        private RouteRecord mGloballySelectedRouteRecord;
        private Int32 mConnectionPhase = PHASE_NOT_AVAILABLE;
        private Int32 mConnectionTimeoutReason;
        private Int64 mConnectionTimeoutStartTime;
        private Boolean mClientStateUpdateScheduled;

        public UserHandler(MediaRouterService service, UserRecord userRecord) {
            Super(Looper->GetMainLooper(), NULL, TRUE);
            mService = service;
            mUserRecord = userRecord;
            mWatcher = new RemoteDisplayProviderWatcher(service.mContext, this,
                    this, mUserRecord.mUserId);
        }

        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case MSG_START: {
                    Start();
                    break;
                }
                case MSG_STOP: {
                    Stop();
                    break;
                }
                case MSG_UPDATE_DISCOVERY_REQUEST: {
                    UpdateDiscoveryRequest();
                    break;
                }
                case MSG_SELECT_ROUTE: {
                    SelectRoute((String)msg.obj);
                    break;
                }
                case MSG_UNSELECT_ROUTE: {
                    UnselectRoute((String)msg.obj);
                    break;
                }
                case MSG_REQUEST_SET_VOLUME: {
                    RequestSetVolume((String)msg.obj, msg.arg1);
                    break;
                }
                case MSG_REQUEST_UPDATE_VOLUME: {
                    RequestUpdateVolume((String)msg.obj, msg.arg1);
                    break;
                }
                case MSG_UPDATE_CLIENT_STATE: {
                    UpdateClientState();
                    break;
                }
                case MSG_CONNECTION_TIMED_OUT: {
                    ConnectionTimedOut();
                    break;
                }
            }
        }

        CARAPI Dump(PrintWriter pw, String prefix) {
            pw->Println(prefix + "Handler");

            final String indent = prefix + "  ";
            pw->Println(indent + "mRunning=" + mRunning);
            pw->Println(indent + "mDiscoveryMode=" + mDiscoveryMode);
            pw->Println(indent + "mGloballySelectedRouteRecord=" + mGloballySelectedRouteRecord);
            pw->Println(indent + "mConnectionPhase=" + mConnectionPhase);
            pw->Println(indent + "mConnectionTimeoutReason=" + mConnectionTimeoutReason);
            pw->Println(indent + "mConnectionTimeoutStartTime=" + (mConnectionTimeoutReason != 0 ?
                    TimeUtils->FormatUptime(mConnectionTimeoutStartTime) : "<n/a>"));

            mWatcher->Dump(pw, prefix);

            final Int32 providerCount = mProviderRecords->Size();
            if (providerCount != 0) {
                for (Int32 i = 0; i < providerCount; i++) {
                    mProviderRecords->Get(i).Dump(pw, prefix);
                }
            } else {
                pw->Println(indent + "<no providers>");
            }
        }

        private void Start() {
            if (!mRunning) {
                mRunning = TRUE;
                mWatcher->Start(); // also starts all providers
            }
        }

        private void Stop() {
            if (mRunning) {
                mRunning = FALSE;
                UnselectGloballySelectedRoute();
                mWatcher->Stop(); // also stops all providers
            }
        }

        private void UpdateDiscoveryRequest() {
            Int32 routeTypes = 0;
            Boolean activeScan = FALSE;
            synchronized(mService.mLock) {
                final Int32 count = mUserRecord.mClientRecords->Size();
                for (Int32 i = 0; i < count; i++) {
                    ClientRecord clientRecord = mUserRecord.mClientRecords->Get(i);
                    routeTypes |= clientRecord.mRouteTypes;
                    activeScan |= clientRecord.mActiveScan;
                }
            }

            final Int32 newDiscoveryMode;
            if ((routeTypes & MediaRouter.ROUTE_TYPE_REMOTE_DISPLAY) != 0) {
                if (activeScan) {
                    newDiscoveryMode = RemoteDisplayState.DISCOVERY_MODE_ACTIVE;
                } else {
                    newDiscoveryMode = RemoteDisplayState.DISCOVERY_MODE_PASSIVE;
                }
            } else {
                newDiscoveryMode = RemoteDisplayState.DISCOVERY_MODE_NONE;
            }

            if (mDiscoveryMode != newDiscoveryMode) {
                mDiscoveryMode = newDiscoveryMode;
                final Int32 count = mProviderRecords->Size();
                for (Int32 i = 0; i < count; i++) {
                    mProviderRecords->Get(i).GetProvider()->SetDiscoveryMode(mDiscoveryMode);
                }
            }
        }

        private void SelectRoute(String routeId) {
            if (routeId != NULL
                    && (mGloballySelectedRouteRecord == NULL
                            || !routeId->Equals(mGloballySelectedRouteRecord->GetUniqueId()))) {
                RouteRecord routeRecord = FindRouteRecord(routeId);
                if (routeRecord != NULL) {
                    UnselectGloballySelectedRoute();

                    Slogger::I(TAG, "Selected global route:" + routeRecord);
                    mGloballySelectedRouteRecord = routeRecord;
                    CheckGloballySelectedRouteState();
                    routeRecord->GetProvider()->SetSelectedDisplay(routeRecord->GetDescriptorId());

                    ScheduleUpdateClientState();
                }
            }
        }

        private void UnselectRoute(String routeId) {
            if (routeId != NULL
                    && mGloballySelectedRouteRecord != NULL
                    && routeId->Equals(mGloballySelectedRouteRecord->GetUniqueId())) {
                UnselectGloballySelectedRoute();
            }
        }

        private void UnselectGloballySelectedRoute() {
            if (mGloballySelectedRouteRecord != NULL) {
                Slogger::I(TAG, "Unselected global route:" + mGloballySelectedRouteRecord);
                mGloballySelectedRouteRecord->GetProvider()->SetSelectedDisplay(NULL);
                mGloballySelectedRouteRecord = NULL;
                CheckGloballySelectedRouteState();

                ScheduleUpdateClientState();
            }
        }

        private void RequestSetVolume(String routeId, Int32 volume) {
            if (mGloballySelectedRouteRecord != NULL
                    && routeId->Equals(mGloballySelectedRouteRecord->GetUniqueId())) {
                mGloballySelectedRouteRecord->GetProvider()->SetDisplayVolume(volume);
            }
        }

        private void RequestUpdateVolume(String routeId, Int32 direction) {
            if (mGloballySelectedRouteRecord != NULL
                    && routeId->Equals(mGloballySelectedRouteRecord->GetUniqueId())) {
                mGloballySelectedRouteRecord->GetProvider()->AdjustDisplayVolume(direction);
            }
        }

        //@Override
        CARAPI AddProvider(RemoteDisplayProviderProxy provider) {
            provider->SetCallback(this);
            provider->SetDiscoveryMode(mDiscoveryMode);
            provider->SetSelectedDisplay(NULL); // just to be safe

            ProviderRecord providerRecord = new ProviderRecord(provider);
            mProviderRecords->Add(providerRecord);
            providerRecord->UpdateDescriptor(provider->GetDisplayState());

            ScheduleUpdateClientState();
        }

        //@Override
        CARAPI RemoveProvider(RemoteDisplayProviderProxy provider) {
            Int32 index = FindProviderRecord(provider);
            if (index >= 0) {
                ProviderRecord providerRecord = mProviderRecords->Remove(index);
                providerRecord->UpdateDescriptor(NULL); // mark routes invalid
                provider->SetCallback(NULL);
                provider->SetDiscoveryMode(RemoteDisplayState.DISCOVERY_MODE_NONE);

                CheckGloballySelectedRouteState();
                ScheduleUpdateClientState();
            }
        }

        //@Override
        CARAPI OnDisplayStateChanged(RemoteDisplayProviderProxy provider,
                RemoteDisplayState state) {
            UpdateProvider(provider, state);
        }

        private void UpdateProvider(RemoteDisplayProviderProxy provider,
                RemoteDisplayState state) {
            Int32 index = FindProviderRecord(provider);
            if (index >= 0) {
                ProviderRecord providerRecord = mProviderRecords->Get(index);
                if (providerRecord->UpdateDescriptor(state)) {
                    CheckGloballySelectedRouteState();
                    ScheduleUpdateClientState();
                }
            }
        }

        /**
         * This function is called whenever the state of the globally selected route
         * may have changed.  It checks the state and updates timeouts or unselects
         * the route as appropriate.
         */
        private void CheckGloballySelectedRouteState() {
            // Unschedule timeouts when the route is unselected.
            if (mGloballySelectedRouteRecord == NULL) {
                mConnectionPhase = PHASE_NOT_AVAILABLE;
                UpdateConnectionTimeout(0);
                return;
            }

            // Ensure that the route is still present and enabled.
            if (!mGloballySelectedRouteRecord->IsValid()
                    || !mGloballySelectedRouteRecord->IsEnabled()) {
                UpdateConnectionTimeout(TIMEOUT_REASON_NOT_AVAILABLE);
                return;
            }

            // Make sure we haven't lost our connection.
            final Int32 oldPhase = mConnectionPhase;
            mConnectionPhase = GetConnectionPhase(mGloballySelectedRouteRecord->GetStatus());
            if (oldPhase >= PHASE_CONNECTING && mConnectionPhase < PHASE_CONNECTING) {
                UpdateConnectionTimeout(TIMEOUT_REASON_CONNECTION_LOST);
                return;
            }

            // Check the route status.
            switch (mConnectionPhase) {
                case PHASE_CONNECTED:
                    if (oldPhase != PHASE_CONNECTED) {
                        Slogger::I(TAG, "Connected to global route: "
                                + mGloballySelectedRouteRecord);
                    }
                    UpdateConnectionTimeout(0);
                    break;
                case PHASE_CONNECTING:
                    if (oldPhase != PHASE_CONNECTING) {
                        Slogger::I(TAG, "Connecting to global route: "
                                + mGloballySelectedRouteRecord);
                    }
                    UpdateConnectionTimeout(TIMEOUT_REASON_WAITING_FOR_CONNECTED);
                    break;
                case PHASE_NOT_CONNECTED:
                    UpdateConnectionTimeout(TIMEOUT_REASON_WAITING_FOR_CONNECTING);
                    break;
                case PHASE_NOT_AVAILABLE:
                default:
                    UpdateConnectionTimeout(TIMEOUT_REASON_NOT_AVAILABLE);
                    break;
            }
        }

        private void UpdateConnectionTimeout(Int32 reason) {
            if (reason != mConnectionTimeoutReason) {
                if (mConnectionTimeoutReason != 0) {
                    RemoveMessages(MSG_CONNECTION_TIMED_OUT);
                }
                mConnectionTimeoutReason = reason;
                mConnectionTimeoutStartTime = SystemClock->UptimeMillis();
                switch (reason) {
                    case TIMEOUT_REASON_NOT_AVAILABLE:
                    case TIMEOUT_REASON_CONNECTION_LOST:
                        // Route became unavailable or connection lost.
                        // Unselect it immediately.
                        SendEmptyMessage(MSG_CONNECTION_TIMED_OUT);
                        break;
                    case TIMEOUT_REASON_WAITING_FOR_CONNECTING:
                        // Waiting for route to start connecting.
                        SendEmptyMessageDelayed(MSG_CONNECTION_TIMED_OUT, CONNECTING_TIMEOUT);
                        break;
                    case TIMEOUT_REASON_WAITING_FOR_CONNECTED:
                        // Waiting for route to complete connection.
                        SendEmptyMessageDelayed(MSG_CONNECTION_TIMED_OUT, CONNECTED_TIMEOUT);
                        break;
                }
            }
        }

        private void ConnectionTimedOut() {
            if (mConnectionTimeoutReason == 0 || mGloballySelectedRouteRecord == NULL) {
                // Shouldn't get here.  There must be a bug somewhere.
                Log->Wtf(TAG, "Handled connection timeout for no reason.");
                return;
            }

            switch (mConnectionTimeoutReason) {
                case TIMEOUT_REASON_NOT_AVAILABLE:
                    Slogger::I(TAG, "Global route no longer available: "
                            + mGloballySelectedRouteRecord);
                    break;
                case TIMEOUT_REASON_CONNECTION_LOST:
                    Slogger::I(TAG, "Global route connection lost: "
                            + mGloballySelectedRouteRecord);
                    break;
                case TIMEOUT_REASON_WAITING_FOR_CONNECTING:
                    Slogger::I(TAG, "Global route timed out while waiting for "
                            + "connection attempt to begin after "
                            + (SystemClock->UptimeMillis() - mConnectionTimeoutStartTime)
                            + " ms: " + mGloballySelectedRouteRecord);
                    break;
                case TIMEOUT_REASON_WAITING_FOR_CONNECTED:
                    Slogger::I(TAG, "Global route timed out while connecting after "
                            + (SystemClock->UptimeMillis() - mConnectionTimeoutStartTime)
                            + " ms: " + mGloballySelectedRouteRecord);
                    break;
            }
            mConnectionTimeoutReason = 0;

            UnselectGloballySelectedRoute();
        }

        private void ScheduleUpdateClientState() {
            if (!mClientStateUpdateScheduled) {
                mClientStateUpdateScheduled = TRUE;
                SendEmptyMessage(MSG_UPDATE_CLIENT_STATE);
            }
        }

        private void UpdateClientState() {
            mClientStateUpdateScheduled = FALSE;

            final String globallySelectedRouteId = mGloballySelectedRouteRecord != NULL ?
                    mGloballySelectedRouteRecord->GetUniqueId() : NULL;

            // Build a new client state for trusted clients.
            MediaRouterClientState trustedState = new MediaRouterClientState();
            trustedState.globallySelectedRouteId = globallySelectedRouteId;
            final Int32 providerCount = mProviderRecords->Size();
            for (Int32 i = 0; i < providerCount; i++) {
                mProviderRecords->Get(i).AppendClientState(trustedState);
            }

            // Build a new client state for untrusted clients that can only see
            // the currently selected route.
            MediaRouterClientState untrustedState = new MediaRouterClientState();
            untrustedState.globallySelectedRouteId = globallySelectedRouteId;
            if (globallySelectedRouteId != NULL) {
                untrustedState.routes->Add(trustedState->GetRoute(globallySelectedRouteId));
            }

            try {
                synchronized(mService.mLock) {
                    // Update the UserRecord.
                    mUserRecord.mTrustedState = trustedState;
                    mUserRecord.mUntrustedState = untrustedState;

                    // Collect all clients.
                    final Int32 count = mUserRecord.mClientRecords->Size();
                    for (Int32 i = 0; i < count; i++) {
                        mTempClients->Add(mUserRecord.mClientRecords->Get(i).mClient);
                    }
                }

                // Notify all clients (outside of the lock).
                final Int32 count = mTempClients->Size();
                for (Int32 i = 0; i < count; i++) {
                    try {
                        mTempClients->Get(i).OnStateChanged();
                    } catch (RemoteException ex) {
                        // ignore errors, client probably died
                    }
                }
            } finally {
                // Clear the list in preparation for the next time.
                mTempClients->Clear();
            }
        }

        private Int32 FindProviderRecord(RemoteDisplayProviderProxy provider) {
            final Int32 count = mProviderRecords->Size();
            for (Int32 i = 0; i < count; i++) {
                ProviderRecord record = mProviderRecords->Get(i);
                if (record->GetProvider() == provider) {
                    return i;
                }
            }
            return -1;
        }

        private RouteRecord FindRouteRecord(String uniqueId) {
            final Int32 count = mProviderRecords->Size();
            for (Int32 i = 0; i < count; i++) {
                RouteRecord record = mProviderRecords->Get(i).FindRouteByUniqueId(uniqueId);
                if (record != NULL) {
                    return record;
                }
            }
            return NULL;
        }

        private static Int32 GetConnectionPhase(Int32 status) {
            switch (status) {
                case MediaRouter.RouteInfo.STATUS_NONE:
                case MediaRouter.RouteInfo.STATUS_CONNECTED:
                    return PHASE_CONNECTED;
                case MediaRouter.RouteInfo.STATUS_CONNECTING:
                    return PHASE_CONNECTING;
                case MediaRouter.RouteInfo.STATUS_SCANNING:
                case MediaRouter.RouteInfo.STATUS_AVAILABLE:
                    return PHASE_NOT_CONNECTED;
                case MediaRouter.RouteInfo.STATUS_NOT_AVAILABLE:
                case MediaRouter.RouteInfo.STATUS_IN_USE:
                default:
                    return PHASE_NOT_AVAILABLE;
            }
        }

        static const class ProviderRecord {
            private final RemoteDisplayProviderProxy mProvider;
            private final String mUniquePrefix;
            private final ArrayList<RouteRecord> mRoutes = new ArrayList<RouteRecord>();
            private RemoteDisplayState mDescriptor;

            public ProviderRecord(RemoteDisplayProviderProxy provider) {
                mProvider = provider;
                mUniquePrefix = provider->GetFlattenedComponentName() + ":";
            }

            public RemoteDisplayProviderProxy GetProvider() {
                return mProvider;
            }

            public String GetUniquePrefix() {
                return mUniquePrefix;
            }

            public Boolean UpdateDescriptor(RemoteDisplayState descriptor) {
                Boolean changed = FALSE;
                if (mDescriptor != descriptor) {
                    mDescriptor = descriptor;

                    // Update all existing routes and reorder them to match
                    // the order of their descriptors.
                    Int32 targetIndex = 0;
                    if (descriptor != NULL) {
                        if (descriptor->IsValid()) {
                            final List<RemoteDisplayInfo> routeDescriptors = descriptor.displays;
                            final Int32 routeCount = routeDescriptors->Size();
                            for (Int32 i = 0; i < routeCount; i++) {
                                final RemoteDisplayInfo routeDescriptor =
                                        routeDescriptors->Get(i);
                                final String descriptorId = routeDescriptor.id;
                                final Int32 sourceIndex = FindRouteByDescriptorId(descriptorId);
                                if (sourceIndex < 0) {
                                    // Add the route to the provider.
                                    String uniqueId = AssignRouteUniqueId(descriptorId);
                                    RouteRecord route =
                                            new RouteRecord(this, descriptorId, uniqueId);
                                    mRoutes->Add(targetIndex++, route);
                                    route->UpdateDescriptor(routeDescriptor);
                                    changed = TRUE;
                                } else if (sourceIndex < targetIndex) {
                                    // Ignore route with duplicate id.
                                    Slogger::W(TAG, "Ignoring route descriptor with duplicate id: "
                                            + routeDescriptor);
                                } else {
                                    // Reorder existing route within the list.
                                    RouteRecord route = mRoutes->Get(sourceIndex);
                                    Collections->Swap(mRoutes, sourceIndex, targetIndex++);
                                    changed |= route->UpdateDescriptor(routeDescriptor);
                                }
                            }
                        } else {
                            Slogger::W(TAG, "Ignoring invalid descriptor from media route provider: "
                                    + mProvider->GetFlattenedComponentName());
                        }
                    }

                    // Dispose all remaining routes that do not have matching descriptors.
                    for (Int32 i = mRoutes->Size() - 1; i >= targetIndex; i--) {
                        RouteRecord route = mRoutes->Remove(i);
                        route->UpdateDescriptor(NULL); // mark route invalid
                        changed = TRUE;
                    }
                }
                return changed;
            }

            CARAPI AppendClientState(MediaRouterClientState state) {
                final Int32 routeCount = mRoutes->Size();
                for (Int32 i = 0; i < routeCount; i++) {
                    state.routes->Add(mRoutes->Get(i).GetInfo());
                }
            }

            public RouteRecord FindRouteByUniqueId(String uniqueId) {
                final Int32 routeCount = mRoutes->Size();
                for (Int32 i = 0; i < routeCount; i++) {
                    RouteRecord route = mRoutes->Get(i);
                    if (route->GetUniqueId()->Equals(uniqueId)) {
                        return route;
                    }
                }
                return NULL;
            }

            private Int32 FindRouteByDescriptorId(String descriptorId) {
                final Int32 routeCount = mRoutes->Size();
                for (Int32 i = 0; i < routeCount; i++) {
                    RouteRecord route = mRoutes->Get(i);
                    if (route->GetDescriptorId()->Equals(descriptorId)) {
                        return i;
                    }
                }
                return -1;
            }

            CARAPI Dump(PrintWriter pw, String prefix) {
                pw->Println(prefix + this);

                final String indent = prefix + "  ";
                mProvider->Dump(pw, indent);

                final Int32 routeCount = mRoutes->Size();
                if (routeCount != 0) {
                    for (Int32 i = 0; i < routeCount; i++) {
                        mRoutes->Get(i).Dump(pw, indent);
                    }
                } else {
                    pw->Println(indent + "<no routes>");
                }
            }

            //@Override
            CARAPI ToString(
        /* [out] */ String* str)
    {
                return "Provider " + mProvider->GetFlattenedComponentName();
            }

            private String AssignRouteUniqueId(String descriptorId) {
                return mUniquePrefix + descriptorId;
            }
        }

        static const class RouteRecord {
            private final ProviderRecord mProviderRecord;
            private final String mDescriptorId;
            private final MediaRouterClientState.RouteInfo mMutableInfo;
            private MediaRouterClientState.RouteInfo mImmutableInfo;
            private RemoteDisplayInfo mDescriptor;

            public RouteRecord(ProviderRecord providerRecord,
                    String descriptorId, String uniqueId) {
                mProviderRecord = providerRecord;
                mDescriptorId = descriptorId;
                mMutableInfo = new MediaRouterClientState->RouteInfo(uniqueId);
            }

            public RemoteDisplayProviderProxy GetProvider() {
                return mProviderRecord->GetProvider();
            }

            public ProviderRecord GetProviderRecord() {
                return mProviderRecord;
            }

            public String GetDescriptorId() {
                return mDescriptorId;
            }

            public String GetUniqueId() {
                return mMutableInfo.id;
            }

            public MediaRouterClientState.RouteInfo GetInfo() {
                if (mImmutableInfo == NULL) {
                    mImmutableInfo = new MediaRouterClientState->RouteInfo(mMutableInfo);
                }
                return mImmutableInfo;
            }

            public Boolean IsValid() {
                return mDescriptor != NULL;
            }

            public Boolean IsEnabled() {
                return mMutableInfo.enabled;
            }

            public Int32 GetStatus() {
                return mMutableInfo.statusCode;
            }

            public Boolean UpdateDescriptor(RemoteDisplayInfo descriptor) {
                Boolean changed = FALSE;
                if (mDescriptor != descriptor) {
                    mDescriptor = descriptor;
                    if (descriptor != NULL) {
                        final String name = ComputeName(descriptor);
                        if (!Objects->Equals(mMutableInfo.name, name)) {
                            mMutableInfo.name = name;
                            changed = TRUE;
                        }
                        final String description = ComputeDescription(descriptor);
                        if (!Objects->Equals(mMutableInfo.description, description)) {
                            mMutableInfo.description = description;
                            changed = TRUE;
                        }
                        final Int32 supportedTypes = ComputeSupportedTypes(descriptor);
                        if (mMutableInfo.supportedTypes != supportedTypes) {
                            mMutableInfo.supportedTypes = supportedTypes;
                            changed = TRUE;
                        }
                        final Boolean enabled = ComputeEnabled(descriptor);
                        if (mMutableInfo.enabled != enabled) {
                            mMutableInfo.enabled = enabled;
                            changed = TRUE;
                        }
                        final Int32 statusCode = ComputeStatusCode(descriptor);
                        if (mMutableInfo.statusCode != statusCode) {
                            mMutableInfo.statusCode = statusCode;
                            changed = TRUE;
                        }
                        final Int32 playbackType = ComputePlaybackType(descriptor);
                        if (mMutableInfo.playbackType != playbackType) {
                            mMutableInfo.playbackType = playbackType;
                            changed = TRUE;
                        }
                        final Int32 playbackStream = ComputePlaybackStream(descriptor);
                        if (mMutableInfo.playbackStream != playbackStream) {
                            mMutableInfo.playbackStream = playbackStream;
                            changed = TRUE;
                        }
                        final Int32 volume = ComputeVolume(descriptor);
                        if (mMutableInfo.volume != volume) {
                            mMutableInfo.volume = volume;
                            changed = TRUE;
                        }
                        final Int32 volumeMax = ComputeVolumeMax(descriptor);
                        if (mMutableInfo.volumeMax != volumeMax) {
                            mMutableInfo.volumeMax = volumeMax;
                            changed = TRUE;
                        }
                        final Int32 volumeHandling = ComputeVolumeHandling(descriptor);
                        if (mMutableInfo.volumeHandling != volumeHandling) {
                            mMutableInfo.volumeHandling = volumeHandling;
                            changed = TRUE;
                        }
                        final Int32 presentationDisplayId = ComputePresentationDisplayId(descriptor);
                        if (mMutableInfo.presentationDisplayId != presentationDisplayId) {
                            mMutableInfo.presentationDisplayId = presentationDisplayId;
                            changed = TRUE;
                        }
                    }
                }
                if (changed) {
                    mImmutableInfo = NULL;
                }
                return changed;
            }

            CARAPI Dump(PrintWriter pw, String prefix) {
                pw->Println(prefix + this);

                final String indent = prefix + "  ";
                pw->Println(indent + "mMutableInfo=" + mMutableInfo);
                pw->Println(indent + "mDescriptorId=" + mDescriptorId);
                pw->Println(indent + "mDescriptor=" + mDescriptor);
            }

            //@Override
            CARAPI ToString(
        /* [out] */ String* str)
    {
                return "Route " + mMutableInfo.name + " (" + mMutableInfo.id + ")";
            }

            private static String ComputeName(RemoteDisplayInfo descriptor) {
                // Note that IsValid() already ensures the name is non-empty.
                return descriptor.name;
            }

            private static String ComputeDescription(RemoteDisplayInfo descriptor) {
                final String description = descriptor.description;
                return TextUtils->IsEmpty(description) ? NULL : description;
            }

            private static Int32 ComputeSupportedTypes(RemoteDisplayInfo descriptor) {
                return MediaRouter.ROUTE_TYPE_LIVE_AUDIO
                        | MediaRouter.ROUTE_TYPE_LIVE_VIDEO
                        | MediaRouter.ROUTE_TYPE_REMOTE_DISPLAY;
            }

            private static Boolean ComputeEnabled(RemoteDisplayInfo descriptor) {
                switch (descriptor.status) {
                    case RemoteDisplayInfo.STATUS_CONNECTED:
                    case RemoteDisplayInfo.STATUS_CONNECTING:
                    case RemoteDisplayInfo.STATUS_AVAILABLE:
                        return TRUE;
                    default:
                        return FALSE;
                }
            }

            private static Int32 ComputeStatusCode(RemoteDisplayInfo descriptor) {
                switch (descriptor.status) {
                    case RemoteDisplayInfo.STATUS_NOT_AVAILABLE:
                        return MediaRouter.RouteInfo.STATUS_NOT_AVAILABLE;
                    case RemoteDisplayInfo.STATUS_AVAILABLE:
                        return MediaRouter.RouteInfo.STATUS_AVAILABLE;
                    case RemoteDisplayInfo.STATUS_IN_USE:
                        return MediaRouter.RouteInfo.STATUS_IN_USE;
                    case RemoteDisplayInfo.STATUS_CONNECTING:
                        return MediaRouter.RouteInfo.STATUS_CONNECTING;
                    case RemoteDisplayInfo.STATUS_CONNECTED:
                        return MediaRouter.RouteInfo.STATUS_CONNECTED;
                    default:
                        return MediaRouter.RouteInfo.STATUS_NONE;
                }
            }

            private static Int32 ComputePlaybackType(RemoteDisplayInfo descriptor) {
                return MediaRouter.RouteInfo.PLAYBACK_TYPE_REMOTE;
            }

            private static Int32 ComputePlaybackStream(RemoteDisplayInfo descriptor) {
                return AudioSystem.STREAM_MUSIC;
            }

            private static Int32 ComputeVolume(RemoteDisplayInfo descriptor) {
                final Int32 volume = descriptor.volume;
                final Int32 volumeMax = descriptor.volumeMax;
                if (volume < 0) {
                    return 0;
                } else if (volume > volumeMax) {
                    return volumeMax;
                }
                return volume;
            }

            private static Int32 ComputeVolumeMax(RemoteDisplayInfo descriptor) {
                final Int32 volumeMax = descriptor.volumeMax;
                return volumeMax > 0 ? volumeMax : 0;
            }

            private static Int32 ComputeVolumeHandling(RemoteDisplayInfo descriptor) {
                final Int32 volumeHandling = descriptor.volumeHandling;
                switch (volumeHandling) {
                    case RemoteDisplayInfo.PLAYBACK_VOLUME_VARIABLE:
                        return MediaRouter.RouteInfo.PLAYBACK_VOLUME_VARIABLE;
                    case RemoteDisplayInfo.PLAYBACK_VOLUME_FIXED:
                    default:
                        return MediaRouter.RouteInfo.PLAYBACK_VOLUME_FIXED;
                }
            }

            private static Int32 ComputePresentationDisplayId(RemoteDisplayInfo descriptor) {
                // The MediaRouter class validates that the id corresponds to an extant
                // presentation display.  So all we do here is canonicalize the NULL case.
                final Int32 displayId = descriptor.presentationDisplayId;
                return displayId < 0 ? -1 : displayId;
            }
        }
    }
}
