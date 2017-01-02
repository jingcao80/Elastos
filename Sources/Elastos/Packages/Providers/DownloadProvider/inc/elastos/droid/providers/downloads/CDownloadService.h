//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADSERVICE_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADSERVICE_H__

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Providers_Downloads_CDownloadService.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::Job::IJobScheduler;
using Elastos::Droid::App::Job::IJobInfo;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IMessage;
using Elastos::Core::IRunnable;
using Elastos::Core::IThrowable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IMap;
using Elastos::Utility::Concurrent::IExecutorService;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

/**
 * Performs background downloads as requested by applications that use
 * {@link DownloadManager}. Multiple start commands can be issued at this
 * service, and it will continue running until no downloads are being actively
 * processed. It may schedule alarms to resume downloads in future.
 * <p>
 * Any database updates important enough to initiate tasks should always be
 * delivered through {@link Context#startService(Intent)}.
 */
CarClass(CDownloadService)
    , public Elastos::Droid::App::Service
    , public IDownloadService
{
private:
    /**
     * Receives notifications when the data in the content provider changes
     */
    class DownloadManagerContentObserver
        : public ContentObserver
    {
    public:
        DownloadManagerContentObserver(
            /* [in] */ CDownloadService* host);

        CARAPI constructor();

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    public:
        CDownloadService* mHost;
    };

    class MyThreadPoolExecutor
        : public Object // ThreadPoolExecutor
    {
    public:
        void AfterExecute(
            /* [in] */ IRunnable* r,
            /* [in] */ IThrowable* t);
    };

    class HandlerCallback
        : public Object
        , public IHandlerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        HandlerCallback(
            /* [in] */ CDownloadService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

    public:
        CDownloadService* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDownloadService();

    /**
     * Returns an IBinder instance when someone wants to connect to this
     * service. Binding to this service is not allowed.
     *
     * @throws UnsupportedOperationException
     */
    CARAPI OnBind(
        /* [in] */ IIntent* i,
        /* [out] */ IBinder** result);

    /**
     * Initializes the service when it is first created
     */
    CARAPI OnCreate();

    CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* result);

    CARAPI OnDestroy();

    /**
     * Enqueue an {@link #updateLocked()} pass to occur in future.
     */
    CARAPI EnqueueUpdate();

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(Boolean) NeedToScheduleCleanup(
        /* [in] */ IJobScheduler* js);

    /**
     * Enqueue an {@link #updateLocked()} pass to occur after delay, usually to
     * catch any finished operations that didn't trigger an update pass.
     */
    CARAPI_(void) EnqueueFinalUpdate();

    /**
     * Update {@link #mDownloads} to match {@link DownloadProvider} state.
     * Depending on current download state it may enqueue {@link DownloadThread}
     * instances, request {@link DownloadScanner} scans, update user-visible
     * notifications, and/or schedule future actions with {@link AlarmManager}.
     * <p>
     * Should only be called from {@link #mUpdateThread} as after being
     * requested through {@link #enqueueUpdate()}.
     *
     * @return If there are active tasks being processed, as of the database
     *         snapshot taken in this update.
     */
    CARAPI_(Boolean) UpdateLocked();

    /**
     * Keeps a local copy of the info about a download, and initiates the
     * download if appropriate.
     */
    CARAPI_(AutoPtr<IDownloadInfo>) InsertDownloadLocked(
        /* [in] */ IDownloadInfoReader* reader,
        /* [in] */ Int64 now);

    /**
     * Updates the local copy of the info about a download.
     */
    CARAPI_(void) UpdateDownload(
        /* [in] */ IDownloadInfoReader* reader,
        /* [in] */ IDownloadInfo* info,
        /* [in] */ Int64 now);

    /**
     * Removes the local copy of the info about a download.
     */
    CARAPI_(void) DeleteDownloadLocked(
        /* [in] */ Int64 id);

    CARAPI_(void) DeleteFileIfExists(
        /* [in] */ String path);

    static CARAPI_(AutoPtr<IExecutorService>) BuildDownloadExecutor();

protected:
    // TODO: migrate WakeLock from individual DownloadThreads out into
    // DownloadReceiver to protect our entire workflow.

    static Boolean DEBUG_LIFECYCLE;

    AutoPtr<ISystemFacade> mSystemFacade;

    AutoPtr<IAlarmManager> mAlarmManager;

    /** Observer to get notified when the content observer's data changes */
    AutoPtr<DownloadManagerContentObserver> mObserver;

    /** Class to handle Notification Manager updates */
    AutoPtr<IDownloadNotifier> mNotifier;

    /** Scheduling of the periodic cleanup job */
    AutoPtr<IJobInfo> mCleanupJob;

    static Int32 CLEANUP_JOB_ID;
    static Int64 CLEANUP_JOB_PERIOD; // one day
    static AutoPtr<IComponentName> sCleanupServiceName;

    /**
     * The Service's view of the list of downloads, mapping download IDs to the corresponding info
     * object. This is kept independently from the content provider, and the Service only initiates
     * downloads based on this data, so that it can deal with situation where the data in the
     * content provider changes or disappears.
     */
    AutoPtr<IMap> mDownloads;

    AutoPtr<IExecutorService> mExecutor;

    AutoPtr<IDownloadScanner> mScanner;

    AutoPtr<IHandlerThread> mUpdateThread;
    AutoPtr<IHandler> mUpdateHandler;

    volatile Int32 mLastStartId;

    static Int32 MSG_UPDATE;
    static Int32 MSG_FINAL_UPDATE;

    AutoPtr<IHandlerCallback> mUpdateCallback;

    static String TAG;
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADSERVICE_H__
