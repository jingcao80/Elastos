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

package com.android.server.media.projection;

using Elastos::Droid::Server::IWatchdog;

using Elastos::Droid::Manifest;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Media::IMediaRouter;
using Elastos::Droid::Media::Iprojection.IMediaProjectionManager;
using Elastos::Droid::Media::Iprojection.IMediaProjection;
using Elastos::Droid::Media::Iprojection.IMediaProjectionCallback;
using Elastos::Droid::Media::Iprojection.IMediaProjectionWatcherCallback;
using Elastos::Droid::Media::Iprojection.MediaProjectionInfo;
using Elastos::Droid::Media::Iprojection.MediaProjectionManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IIBinder.DeathRecipient;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::Server::ISystemService;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

/**
 * Manages MediaProjection sessions.
 *
 * The {@link MediaProjectionManagerService} manages the creation and lifetime of MediaProjections,
 * as well as the capabilities they grant. Any service using MediaProjection tokens as permission
 * grants <b>must</b> validate the token before use by calling {@link
 * IMediaProjectionService#isValidMediaProjection}.
 */
public class MediaProjectionManagerService extends SystemService
        implements Watchdog.Monitor {
    private static const String TAG = "MediaProjectionManagerService";

    private final Object mLock = new Object(); // Protects the list of media projections
    private final Map<IBinder, IBinder.DeathRecipient> mDeathEaters;
    private final CallbackDelegate mCallbackDelegate;

    private final Context mContext;
    private final AppOpsManager mAppOps;

    private final MediaRouter mMediaRouter;
    private final MediaRouterCallback mMediaRouterCallback;
    private MediaRouter.RouteInfo mMediaRouteInfo;

    private IBinder mProjectionToken;
    private MediaProjection mProjectionGrant;

    public MediaProjectionManagerService(Context context) {
        Super(context);
        mContext = context;
        mDeathEaters = new ArrayMap<IBinder, IBinder.DeathRecipient>();
        mCallbackDelegate = new CallbackDelegate();
        mAppOps = (AppOpsManager) mContext->GetSystemService(Context.APP_OPS_SERVICE);
        mMediaRouter = (MediaRouter) mContext->GetSystemService(Context.MEDIA_ROUTER_SERVICE);
        mMediaRouterCallback = new MediaRouterCallback();
        Watchdog->GetInstance()->AddMonitor(this);
    }

    //@Override
    CARAPI OnStart() {
        PublishBinderService(Context.MEDIA_PROJECTION_SERVICE, new BinderService(),
                FALSE /*allowIsolated*/);
        mMediaRouter->AddCallback(MediaRouter.ROUTE_TYPE_REMOTE_DISPLAY, mMediaRouterCallback,
                MediaRouter.CALLBACK_FLAG_PASSIVE_DISCOVERY);
    }

    //@Override
    CARAPI OnSwitchUser(Int32 userId) {
        mMediaRouter->RebindAsUser(userId);
    }

    //@Override
    CARAPI Monitor() {
        synchronized(mLock) { /* check for deadlock */ }
    }

    private void StartProjectionLocked(final MediaProjection projection) {
        if (mProjectionGrant != NULL) {
            mProjectionGrant->Stop();
        }
        if (mMediaRouteInfo != NULL) {
            mMediaRouter->GetDefaultRoute()->Select();
        }
        mProjectionToken = projection->AsBinder();
        mProjectionGrant = projection;
        DispatchStart(projection);
    }

    private void StopProjectionLocked(final MediaProjection projection) {
        mProjectionToken = NULL;
        mProjectionGrant = NULL;
        DispatchStop(projection);
    }

    private void AddCallback(final IMediaProjectionWatcherCallback callback) {
        IBinder.DeathRecipient deathRecipient = new IBinder->DeathRecipient() {
            //@Override
            CARAPI BinderDied() {
                synchronized(mLock) {
                    RemoveCallback(callback);
                }
            }
        };
        synchronized(mLock) {
            mCallbackDelegate->Add(callback);
            LinkDeathRecipientLocked(callback, deathRecipient);
        }
    }

    private void RemoveCallback(IMediaProjectionWatcherCallback callback) {
        synchronized(mLock) {
            UnlinkDeathRecipientLocked(callback);
            mCallbackDelegate->Remove(callback);
        }
    }

    private void LinkDeathRecipientLocked(IMediaProjectionWatcherCallback callback,
            IBinder.DeathRecipient deathRecipient) {
        try {
            final IBinder token = callback->AsBinder();
            token->LinkToDeath(deathRecipient, 0);
            mDeathEaters->Put(token, deathRecipient);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Unable to link to death for media projection monitoring callback", e);
        }
    }

    private void UnlinkDeathRecipientLocked(IMediaProjectionWatcherCallback callback) {
        final IBinder token = callback->AsBinder();
        IBinder.DeathRecipient deathRecipient = mDeathEaters->Remove(token);
        if (deathRecipient != NULL) {
            token->UnlinkToDeath(deathRecipient, 0);
        }
    }

    private void DispatchStart(MediaProjection projection) {
        mCallbackDelegate->DispatchStart(projection);
    }

    private void DispatchStop(MediaProjection projection) {
        mCallbackDelegate->DispatchStop(projection);
    }

    private Boolean IsValidMediaProjection(IBinder token) {
        synchronized(mLock) {
            if (mProjectionToken != NULL) {
                return mProjectionToken->Equals(token);
            }
            return FALSE;
        }
    }

    private MediaProjectionInfo GetActiveProjectionInfo() {
        synchronized(mLock) {
            if (mProjectionGrant == NULL) {
                return NULL;
            }
            return mProjectionGrant->GetProjectionInfo();
        }
    }

    private void Dump(final PrintWriter pw) {
        pw->Println("MEDIA PROJECTION MANAGER (dumpsys media_projection)");
        synchronized(mLock) {
            pw->Println("Media Projection: ");
            if (mProjectionGrant != NULL ) {
                mProjectionGrant->Dump(pw);
            } else {
                pw->Println("NULL");
            }
        }
    }

    private final class BinderService extends IMediaProjectionManager.Stub {

        //@Override // Binder call
        public Boolean HasProjectionPermission(Int32 uid, String packageName) {
            Int64 token = Binder->ClearCallingIdentity();
            Boolean hasPermission = FALSE;
            try {
                hasPermission |= CheckPermission(packageName,
                        Manifest::permission::CAPTURE_VIDEO_OUTPUT)
                        || mAppOps->NoteOpNoThrow(
                                AppOpsManager.OP_PROJECT_MEDIA, uid, packageName)
                        == AppOpsManager.MODE_ALLOWED;
            } finally {
                Binder->RestoreCallingIdentity(token);
            }
            return hasPermission;
        }

        //@Override // Binder call
        public IMediaProjection CreateProjection(Int32 uid, String packageName, Int32 type,
                Boolean isPermanentGrant) {
            if (mContext->CheckCallingPermission(Manifest::permission::MANAGE_MEDIA_PROJECTION)
                        != PackageManager.PERMISSION_GRANTED) {
                throw new SecurityException("Requires MANAGE_MEDIA_PROJECTION in order to grant "
                        + "projection permission");
            }
            if (packageName == NULL || packageName->IsEmpty()) {
                throw new IllegalArgumentException("package name must not be empty");
            }
            Int64 callingToken = Binder->ClearCallingIdentity();
            MediaProjection projection;
            try {
                projection = new MediaProjection(type, uid, packageName);
                if (isPermanentGrant) {
                    mAppOps->SetMode(AppOpsManager.OP_PROJECT_MEDIA,
                            projection.uid, projection.packageName, AppOpsManager.MODE_ALLOWED);
                }
            } finally {
                Binder->RestoreCallingIdentity(callingToken);
            }
            return projection;
        }

        //@Override // Binder call
        public Boolean IsValidMediaProjection(IMediaProjection projection) {
            return MediaProjectionManagerService.this->IsValidMediaProjection(
                    projection->AsBinder());
        }

        //@Override // Binder call
        public MediaProjectionInfo GetActiveProjectionInfo() {
            if (mContext->CheckCallingPermission(Manifest::permission::MANAGE_MEDIA_PROJECTION)
                        != PackageManager.PERMISSION_GRANTED) {
                throw new SecurityException("Requires MANAGE_MEDIA_PROJECTION in order to add "
                        + "projection callbacks");
            }
            final Int64 token = Binder->ClearCallingIdentity();
            try {
                return MediaProjectionManagerService.this->GetActiveProjectionInfo();
            } finally {
                Binder->RestoreCallingIdentity(token);
            }
        }

        //@Override // Binder call
        CARAPI StopActiveProjection() {
            if (mContext->CheckCallingPermission(Manifest::permission::MANAGE_MEDIA_PROJECTION)
                        != PackageManager.PERMISSION_GRANTED) {
                throw new SecurityException("Requires MANAGE_MEDIA_PROJECTION in order to add "
                        + "projection callbacks");
            }
            final Int64 token = Binder->ClearCallingIdentity();
            try {
                if (mProjectionGrant != NULL) {
                    mProjectionGrant->Stop();
                }
            } finally {
                Binder->RestoreCallingIdentity(token);
            }

        }

        //@Override //Binder call
        CARAPI AddCallback(final IMediaProjectionWatcherCallback callback) {
            if (mContext->CheckCallingPermission(Manifest::permission::MANAGE_MEDIA_PROJECTION)
                        != PackageManager.PERMISSION_GRANTED) {
                throw new SecurityException("Requires MANAGE_MEDIA_PROJECTION in order to add "
                        + "projection callbacks");
            }
            final Int64 token = Binder->ClearCallingIdentity();
            try {
                MediaProjectionManagerService.this->AddCallback(callback);
            } finally {
                Binder->RestoreCallingIdentity(token);
            }
        }

        //@Override
        CARAPI RemoveCallback(IMediaProjectionWatcherCallback callback) {
            if (mContext->CheckCallingPermission(Manifest::permission::MANAGE_MEDIA_PROJECTION)
                        != PackageManager.PERMISSION_GRANTED) {
                throw new SecurityException("Requires MANAGE_MEDIA_PROJECTION in order to remove "
                        + "projection callbacks");
            }
            final Int64 token = Binder->ClearCallingIdentity();
            try {
                MediaProjectionManagerService.this->RemoveCallback(callback);
            } finally {
                Binder->RestoreCallingIdentity(token);
            }
        }

        //@Override // Binder call
        CARAPI Dump(FileDescriptor fd, final PrintWriter pw, String[] args) {
            if (mContext == NULL
                    || mContext->CheckCallingOrSelfPermission(Manifest::permission::DUMP)
                    != PackageManager.PERMISSION_GRANTED) {
                pw->Println("Permission Denial: can't dump MediaProjectionManager from from pid="
                        + Binder->GetCallingPid() + ", uid=" + Binder->GetCallingUid());
                return;
            }

            final Int64 token = Binder->ClearCallingIdentity();
            try {
                Dump(pw);
            } finally {
                Binder->RestoreCallingIdentity(token);
            }
        }


        private Boolean CheckPermission(String packageName, String permission) {
            return mContext->GetPackageManager()->CheckPermission(permission, packageName)
                    == PackageManager.PERMISSION_GRANTED;
        }
    }

    private final class MediaProjection extends IMediaProjection.Stub {
        public final Int32 uid;
        public final String packageName;
        public final UserHandle userHandle;

        private IBinder mToken;
        private IBinder.DeathRecipient mDeathEater;
        private Int32 mType;

        public MediaProjection(Int32 type, Int32 uid, String packageName) {
            mType = type;
            this.uid = uid;
            this.packageName = packageName;
            userHandle = new UserHandle(UserHandle->GetUserId(uid));
        }

        //@Override // Binder call
        public Boolean CanProjectVideo() {
            return mType == MediaProjectionManager.TYPE_MIRRORING ||
                    mType == MediaProjectionManager.TYPE_SCREEN_CAPTURE;
        }

        //@Override // Binder call
        public Boolean CanProjectSecureVideo() {
            return FALSE;
        }

        //@Override // Binder call
        public Boolean CanProjectAudio() {
            return mType == MediaProjectionManager.TYPE_MIRRORING ||
                    mType == MediaProjectionManager.TYPE_PRESENTATION;
        }

        //@Override // Binder call
        public Int32 ApplyVirtualDisplayFlags(Int32 flags) {
            if (mType == MediaProjectionManager.TYPE_SCREEN_CAPTURE) {
                flags &= ~DisplayManager.VIRTUAL_DISPLAY_FLAG_OWN_CONTENT_ONLY;
                flags |= DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR
                        | DisplayManager.VIRTUAL_DISPLAY_FLAG_PRESENTATION;
                return flags;
            } else if (mType == MediaProjectionManager.TYPE_MIRRORING) {
                flags &= ~(DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC |
                        DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR);
                flags |= DisplayManager.VIRTUAL_DISPLAY_FLAG_OWN_CONTENT_ONLY |
                        DisplayManager.VIRTUAL_DISPLAY_FLAG_PRESENTATION;
                return flags;
            } else if (mType == MediaProjectionManager.TYPE_PRESENTATION) {
                flags &= ~DisplayManager.VIRTUAL_DISPLAY_FLAG_OWN_CONTENT_ONLY;
                flags |= DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC |
                        DisplayManager.VIRTUAL_DISPLAY_FLAG_PRESENTATION |
                        DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR;
                return flags;
            } else  {
                throw new RuntimeException("Unknown MediaProjection type");
            }
        }

        //@Override // Binder call
        CARAPI Start(final IMediaProjectionCallback callback) {
            if (callback == NULL) {
                throw new IllegalArgumentException("callback must not be NULL");
            }
            synchronized(mLock) {
                if (IsValidMediaProjection(AsBinder())) {
                    throw new IllegalStateException(
                            "Cannot start already started MediaProjection");
                }
                RegisterCallback(callback);
                try {
                    mToken = callback->AsBinder();
                    mDeathEater = new IBinder->DeathRecipient() {
                        //@Override
                        CARAPI BinderDied() {
                            mCallbackDelegate->Remove(callback);
                            Stop();
                        }
                    };
                    mToken->LinkToDeath(mDeathEater, 0);
                } catch (RemoteException e) {
                    Slogger::W(TAG,
                            "MediaProjectionCallbacks must be valid, aborting MediaProjection", e);
                    return;
                }
                StartProjectionLocked(this);
            }
        }

        //@Override // Binder call
        CARAPI Stop() {
            synchronized(mLock) {
                if (!IsValidMediaProjection(AsBinder())) {
                    Slogger::W(TAG, "Attempted to stop inactive MediaProjection "
                            + "(uid=" + Binder->GetCallingUid() + ", "
                            + "pid=" + Binder->GetCallingPid() + ")");
                    return;
                }
                mToken->UnlinkToDeath(mDeathEater, 0);
                StopProjectionLocked(this);
            }
        }

        //@Override
        CARAPI RegisterCallback(IMediaProjectionCallback callback) {
            if (callback == NULL) {
                throw new IllegalArgumentException("callback must not be NULL");
            }
            mCallbackDelegate->Add(callback);
        }

        //@Override
        CARAPI UnregisterCallback(IMediaProjectionCallback callback) {
            if (callback == NULL) {
                throw new IllegalArgumentException("callback must not be NULL");
            }
            mCallbackDelegate->Remove(callback);
        }

        public MediaProjectionInfo GetProjectionInfo() {
            return new MediaProjectionInfo(packageName, userHandle);
        }

        CARAPI Dump(PrintWriter pw) {
            pw->Println("(" + packageName + ", uid=" + uid + "): " + TypeToString(mType));
        }
    }

    private class MediaRouterCallback extends MediaRouter.SimpleCallback {
        //@Override
        CARAPI OnRouteSelected(MediaRouter router, Int32 type, MediaRouter.RouteInfo info) {
            synchronized(mLock) {
                if ((type & MediaRouter.ROUTE_TYPE_REMOTE_DISPLAY) != 0) {
                    mMediaRouteInfo = info;
                    if (mProjectionGrant != NULL) {
                        mProjectionGrant->Stop();
                    }
                }
            }
        }

        //@Override
        CARAPI OnRouteUnselected(MediaRouter route, Int32 type, MediaRouter.RouteInfo info) {
            if (mMediaRouteInfo == info) {
                mMediaRouteInfo = NULL;
            }
        }
    }


    private static class CallbackDelegate {
        private Map<IBinder, IMediaProjectionCallback> mClientCallbacks;
        private Map<IBinder, IMediaProjectionWatcherCallback> mWatcherCallbacks;
        private Handler mHandler;
        private Object mLock = new Object();

        public CallbackDelegate() {
            mHandler = new Handler(Looper->GetMainLooper(), NULL, TRUE /*async*/);
            mClientCallbacks = new ArrayMap<IBinder, IMediaProjectionCallback>();
            mWatcherCallbacks = new ArrayMap<IBinder, IMediaProjectionWatcherCallback>();
        }

        CARAPI Add(IMediaProjectionCallback callback) {
            synchronized(mLock) {
                mClientCallbacks->Put(callback->AsBinder(), callback);
            }
        }

        CARAPI Add(IMediaProjectionWatcherCallback callback) {
            synchronized(mLock) {
                mWatcherCallbacks->Put(callback->AsBinder(), callback);
            }
        }

        CARAPI Remove(IMediaProjectionCallback callback) {
            synchronized(mLock) {
                mClientCallbacks->Remove(callback->AsBinder());
            }
        }

        CARAPI Remove(IMediaProjectionWatcherCallback callback) {
            synchronized(mLock) {
                mWatcherCallbacks->Remove(callback->AsBinder());
            }
        }

        CARAPI DispatchStart(MediaProjection projection) {
            if (projection == NULL) {
                Slogger::E(TAG, "Tried to dispatch start notification for a NULL media projection."
                        + " Ignoring!");
                return;
            }
            synchronized(mLock) {
                for (IMediaProjectionWatcherCallback callback : mWatcherCallbacks->Values()) {
                    MediaProjectionInfo info = projection->GetProjectionInfo();
                    mHandler->Post(new WatcherStartCallback(info, callback));
                }
            }
        }

        CARAPI DispatchStop(MediaProjection projection) {
            if (projection == NULL) {
                Slogger::E(TAG, "Tried to dispatch stop notification for a NULL media projection."
                        + " Ignoring!");
                return;
            }
            synchronized(mLock) {
                for (IMediaProjectionCallback callback : mClientCallbacks->Values()) {
                    mHandler->Post(new ClientStopCallback(callback));
                }

                for (IMediaProjectionWatcherCallback callback : mWatcherCallbacks->Values()) {
                    MediaProjectionInfo info = projection->GetProjectionInfo();
                    mHandler->Post(new WatcherStopCallback(info, callback));
                }
            }
        }
    }

    private static const class WatcherStartCallback implements Runnable {
        private IMediaProjectionWatcherCallback mCallback;
        private MediaProjectionInfo mInfo;

        public WatcherStartCallback(MediaProjectionInfo info,
                IMediaProjectionWatcherCallback callback) {
            mInfo = info;
            mCallback = callback;
        }

        //@Override
        CARAPI Run() {
            try {
                mCallback->OnStart(mInfo);
            } catch (RemoteException e) {
                Slogger::W(TAG, "Failed to notify media projection has stopped", e);
            }
        }
    }

    private static const class WatcherStopCallback implements Runnable {
        private IMediaProjectionWatcherCallback mCallback;
        private MediaProjectionInfo mInfo;

        public WatcherStopCallback(MediaProjectionInfo info,
                IMediaProjectionWatcherCallback callback) {
            mInfo = info;
            mCallback = callback;
        }

        //@Override
        CARAPI Run() {
            try {
                mCallback->OnStop(mInfo);
            } catch (RemoteException e) {
                Slogger::W(TAG, "Failed to notify media projection has stopped", e);
            }
        }
    }

    private static const class ClientStopCallback implements Runnable {
        private IMediaProjectionCallback mCallback;

        public ClientStopCallback(IMediaProjectionCallback callback) {
            mCallback = callback;
        }

        //@Override
        CARAPI Run() {
            try {
                mCallback->OnStop();
            } catch (RemoteException e) {
                Slogger::W(TAG, "Failed to notify media projection has stopped", e);
            }
        }
    }


    private static String TypeToString(Int32 type) {
        switch (type) {
            case MediaProjectionManager.TYPE_SCREEN_CAPTURE:
                return "TYPE_SCREEN_CAPTURE";
            case MediaProjectionManager.TYPE_MIRRORING:
                return "TYPE_MIRRORING";
            case MediaProjectionManager.TYPE_PRESENTATION:
                return "TYPE_PRESENTATION";
        }
        return Integer->ToString(type);
    }
}
