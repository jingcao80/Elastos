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

using Elastos::Droid::Manifest;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Media::IRemoteDisplayState;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISlog;

using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;

/**
 * Watches for remote display provider services to be installed.
 * Adds a provider to the media router for each registered service.
 *
 * @see RemoteDisplayProviderProxy
 */
public class RemoteDisplayProviderWatcher {
    private static const String TAG = "RemoteDisplayProvider";  // max. 23 chars
    private static const Boolean DEBUG = Log->IsLoggable(TAG, Log.DEBUG);

    private final Context mContext;
    private final Callback mCallback;
    private final Handler mHandler;
    private final Int32 mUserId;
    private final PackageManager mPackageManager;

    private final ArrayList<RemoteDisplayProviderProxy> mProviders =
            new ArrayList<RemoteDisplayProviderProxy>();
    private Boolean mRunning;

    public RemoteDisplayProviderWatcher(Context context,
            Callback callback, Handler handler, Int32 userId) {
        mContext = context;
        mCallback = callback;
        mHandler = handler;
        mUserId = userId;
        mPackageManager = context->GetPackageManager();
    }

    CARAPI Dump(PrintWriter pw, String prefix) {
        pw->Println(prefix + "Watcher");
        pw->Println(prefix + "  mUserId=" + mUserId);
        pw->Println(prefix + "  mRunning=" + mRunning);
        pw->Println(prefix + "  mProviders->Size()=" + mProviders->Size());
    }

    CARAPI Start() {
        if (!mRunning) {
            mRunning = TRUE;

            IntentFilter filter = new IntentFilter();
            filter->AddAction(IIntent::ACTION_PACKAGE_ADDED);
            filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
            filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
            filter->AddAction(IIntent::ACTION_PACKAGE_REPLACED);
            filter->AddAction(IIntent::ACTION_PACKAGE_RESTARTED);
            filter->AddDataScheme("package");
            mContext->RegisterReceiverAsUser(mScanPackagesReceiver,
                    new UserHandle(mUserId), filter, NULL, mHandler);

            // Scan packages.
            // Also has the side-effect of restarting providers if needed.
            mHandler->Post(mScanPackagesRunnable);
        }
    }

    CARAPI Stop() {
        if (mRunning) {
            mRunning = FALSE;

            mContext->UnregisterReceiver(mScanPackagesReceiver);
            mHandler->RemoveCallbacks(mScanPackagesRunnable);

            // Stop all providers.
            for (Int32 i = mProviders->Size() - 1; i >= 0; i--) {
                mProviders->Get(i).Stop();
            }
        }
    }

    private void ScanPackages() {
        if (!mRunning) {
            return;
        }

        // Add providers for all new services.
        // Reorder the list so that providers left at the end will be the ones to remove.
        Int32 targetIndex = 0;
        Intent intent = new Intent(RemoteDisplayState.SERVICE_INTERFACE);
        for (ResolveInfo resolveInfo : mPackageManager->QueryIntentServicesAsUser(
                intent, 0, mUserId)) {
            ServiceInfo serviceInfo = resolveInfo.serviceInfo;
            if (serviceInfo != NULL && VerifyServiceTrusted(serviceInfo)) {
                Int32 sourceIndex = FindProvider(serviceInfo.packageName, serviceInfo.name);
                if (sourceIndex < 0) {
                    RemoteDisplayProviderProxy provider =
                            new RemoteDisplayProviderProxy(mContext,
                            new ComponentName(serviceInfo.packageName, serviceInfo.name),
                            mUserId);
                    provider->Start();
                    mProviders->Add(targetIndex++, provider);
                    mCallback->AddProvider(provider);
                } else if (sourceIndex >= targetIndex) {
                    RemoteDisplayProviderProxy provider = mProviders->Get(sourceIndex);
                    provider->Start(); // restart the provider if needed
                    provider->RebindIfDisconnected();
                    Collections->Swap(mProviders, sourceIndex, targetIndex++);
                }
            }
        }

        // Remove providers for missing services.
        if (targetIndex < mProviders->Size()) {
            for (Int32 i = mProviders->Size() - 1; i >= targetIndex; i--) {
                RemoteDisplayProviderProxy provider = mProviders->Get(i);
                mCallback->RemoveProvider(provider);
                mProviders->Remove(provider);
                provider->Stop();
            }
        }
    }

    private Boolean VerifyServiceTrusted(ServiceInfo serviceInfo) {
        if (serviceInfo.permission == NULL || !serviceInfo.permission->Equals(
                Manifest::permission::BIND_REMOTE_DISPLAY)) {
            // If the service does not require this permission then any app could
            // potentially bind to it and cause the remote display service to
            // misbehave.  So we only want to trust providers that require the
            // correct permissions.
            Slogger::W(TAG, "Ignoring remote display provider service because it did not "
                    + "require the BIND_REMOTE_DISPLAY permission in its manifest: "
                    + serviceInfo.packageName + "/" + serviceInfo.name);
            return FALSE;
        }
        if (!HasCaptureVideoPermission(serviceInfo.packageName)) {
            // If the service does not have permission to capture video then it
            // isn't going to be terribly useful as a remote display, is it?
            // Kind of makes you wonder what it's doing there in the first place.
            Slogger::W(TAG, "Ignoring remote display provider service because it does not "
                    + "have the CAPTURE_VIDEO_OUTPUT or CAPTURE_SECURE_VIDEO_OUTPUT "
                    + "permission: " + serviceInfo.packageName + "/" + serviceInfo.name);
            return FALSE;
        }
        // Looks good.
        return TRUE;
    }

    private Boolean HasCaptureVideoPermission(String packageName) {
        if (mPackageManager->CheckPermission(Manifest::permission::CAPTURE_VIDEO_OUTPUT,
                packageName) == PackageManager.PERMISSION_GRANTED) {
            return TRUE;
        }
        if (mPackageManager->CheckPermission(Manifest::permission::CAPTURE_SECURE_VIDEO_OUTPUT,
                packageName) == PackageManager.PERMISSION_GRANTED) {
            return TRUE;
        }
        return FALSE;
    }

    private Int32 FindProvider(String packageName, String className) {
        Int32 count = mProviders->Size();
        for (Int32 i = 0; i < count; i++) {
            RemoteDisplayProviderProxy provider = mProviders->Get(i);
            if (provider->HasComponentName(packageName, className)) {
                return i;
            }
        }
        return -1;
    }

    private final BroadcastReceiver mScanPackagesReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            if (DEBUG) {
                Slogger::D(TAG, "Received package manager broadcast: " + intent);
            }
            ScanPackages();
        }
    };

    private final Runnable mScanPackagesRunnable = new Runnable() {
        //@Override
        CARAPI Run() {
            ScanPackages();
        }
    };

    public interface Callback {
        void AddProvider(RemoteDisplayProviderProxy provider);
        void RemoveProvider(RemoteDisplayProviderProxy provider);
    }
}
