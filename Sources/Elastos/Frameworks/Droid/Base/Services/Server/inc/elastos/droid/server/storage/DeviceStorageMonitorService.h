

/*
 * Copyright (C) 2007-2008 The Android Open Source Project
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

using Elastos::Droid::Server::Storage;
using Elastos::Droid::Server::ISystemService;
using Elastos::Droid::Server::Pm::CPackageManagerService;

using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageDataObserver;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IFileObserver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IStatFs;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Istorage.StorageManager;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::Iformat.Formatter;
using Elastos::Droid::Utility::IEventLog;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::ITimeUtils;

using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

import dalvik.system.VMRuntime;

/**
 * This class implements a service to monitor the amount of disk
 * storage space on the device.  If the free storage on device is less
 * than a tunable threshold value (a secure settings parameter;
 * default 10%) a low memory notification is displayed to alert the
 * user. If the user clicks on the low memory notification the
 * Application Manager application gets launched to let the user free
 * storage space.
 *
 * Event log events: A low memory event with the free storage on
 * device in bytes is logged to the event log when the device goes low
 * on storage space.  The amount of free storage on the device is
 * periodically logged to the event log. The log interval is a secure
 * settings parameter with a default value of 12 hours.  When the free
 * storage differential goes below a threshold (again a secure
 * settings parameter with a default value of 2MB), the free memory is
 * logged to the event log.
 */
public class DeviceStorageMonitorService extends SystemService {
    static const String TAG = "DeviceStorageMonitorService";

    static const Boolean DEBUG = FALSE;
    static const Boolean localLOGV = FALSE;

    static const Int32 DEVICE_MEMORY_WHAT = 1;
    private static const Int32 MONITOR_INTERVAL = 1; //in minutes
    private static const Int32 LOW_MEMORY_NOTIFICATION_ID = 1;

    private static const Int32 DEFAULT_FREE_STORAGE_LOG_INTERVAL_IN_MINUTES = 12*60; //in minutes
    private static const Int64 DEFAULT_DISK_FREE_CHANGE_REPORTING_THRESHOLD = 2 * 1024 * 1024; // 2MB
    private static const Int64 DEFAULT_CHECK_INTERVAL = MONITOR_INTERVAL*60*1000;

    private Int64 mFreeMem;  // on /data
    private Int64 mFreeMemAfterLastCacheClear;  // on /data
    private Int64 mLastReportedFreeMem;
    private Int64 mLastReportedFreeMemTime;
    Boolean mLowMemFlag=FALSE;
    private Boolean mMemFullFlag=FALSE;
    private final Boolean mIsBootImageOnDisk;
    private final ContentResolver mResolver;
    private final Int64 mTotalMemory;  // on /data
    private final StatFs mDataFileStats;
    private final StatFs mSystemFileStats;
    private final StatFs mCacheFileStats;

    private static const File DATA_PATH = Environment->GetDataDirectory();
    private static const File SYSTEM_PATH = Environment->GetRootDirectory();
    private static const File CACHE_PATH = Environment->GetDownloadCacheDirectory();

    private Int64 mThreadStartTime = -1;
    Boolean mClearSucceeded = FALSE;
    Boolean mClearingCache;
    private final Intent mStorageLowIntent;
    private final Intent mStorageOkIntent;
    private final Intent mStorageFullIntent;
    private final Intent mStorageNotFullIntent;
    private CachePackageDataObserver mClearCacheObserver;
    private CacheFileDeletedObserver mCacheFileDeletedObserver;
    private static const Int32 _TRUE = 1;
    private static const Int32 _FALSE = 0;
    // This is the raw threshold that has been set at which we consider
    // storage to be low.
    Int64 mMemLowThreshold;
    // This is the threshold at which we start trying to flush caches
    // to get below the low threshold limit.  It is less than the low
    // threshold; we will allow storage to get a bit beyond the limit
    // before flushing and checking if we are actually low.
    private Int64 mMemCacheStartTrimThreshold;
    // This is the threshold that we try to get to when deleting cache
    // files.  This is greater than the low threshold so that we will flush
    // more files than absolutely needed, to reduce the frequency that
    // flushing takes place.
    private Int64 mMemCacheTrimToThreshold;
    private Int64 mMemFullThreshold;

    /**
     * This string is used for ServiceManager access to this class.
     */
    static const String SERVICE = "devicestoragemonitor";

    /**
    * Handler that checks the amount of disk space on the device and sends a
    * notification if the device runs low on disk space
    */
    private final Handler mHandler = new Handler() {
        //@Override
        CARAPI HandleMessage(Message msg) {
            //don't handle an invalid message
            if (msg.what != DEVICE_MEMORY_WHAT) {
                Slogger::E(TAG, "Will not process invalid message");
                return;
            }
            CheckMemory(msg.arg1 == _TRUE);
        }
    };

    private class CachePackageDataObserver extends IPackageDataObserver.Stub {
        CARAPI OnRemoveCompleted(String packageName, Boolean succeeded) {
            mClearSucceeded = succeeded;
            mClearingCache = FALSE;
            If(localLOGV) Slogger::I(TAG, " Clear succeeded:"+mClearSucceeded
                    +", mClearingCache:"+mClearingCache+" Forcing memory check");
            PostCheckMemoryMsg(FALSE, 0);
        }
    }

    private void RestatDataDir() {
        try {
            mDataFileStats->Restat(DATA_PATH->GetAbsolutePath());
            mFreeMem = (Int64) mDataFileStats->GetAvailableBlocks() *
                mDataFileStats->GetBlockSize();
        } catch (IllegalArgumentException e) {
            // use the old value of mFreeMem
        }
        // Allow freemem to be overridden by debug.freemem for testing
        String debugFreeMem = SystemProperties->Get("debug.freemem");
        if (!"".Equals(debugFreeMem)) {
            mFreeMem = Long->ParseLong(debugFreeMem);
        }
        // Read the log interval from secure settings
        Int64 freeMemLogInterval = Settings::Global::>GetLong(mResolver,
                Settings::Global::SYS_FREE_STORAGE_LOG_INTERVAL,
                DEFAULT_FREE_STORAGE_LOG_INTERVAL_IN_MINUTES)*60*1000;
        //log the amount of free memory in event log
        Int64 currTime = SystemClock->ElapsedRealtime();
        If((mLastReportedFreeMemTime == 0) ||
           (currTime-mLastReportedFreeMemTime) >= freeMemLogInterval) {
            mLastReportedFreeMemTime = currTime;
            Int64 mFreeSystem = -1, mFreeCache = -1;
            try {
                mSystemFileStats->Restat(SYSTEM_PATH->GetAbsolutePath());
                mFreeSystem = (Int64) mSystemFileStats->GetAvailableBlocks() *
                    mSystemFileStats->GetBlockSize();
            } catch (IllegalArgumentException e) {
                // ignore; report -1
            }
            try {
                mCacheFileStats->Restat(CACHE_PATH->GetAbsolutePath());
                mFreeCache = (Int64) mCacheFileStats->GetAvailableBlocks() *
                    mCacheFileStats->GetBlockSize();
            } catch (IllegalArgumentException e) {
                // ignore; report -1
            }
            EventLog->WriteEvent(EventLogTags.FREE_STORAGE_LEFT,
                                mFreeMem, mFreeSystem, mFreeCache);
        }
        // Read the reporting threshold from secure settings
        Int64 threshold = Settings::Global::>GetLong(mResolver,
                Settings::Global::DISK_FREE_CHANGE_REPORTING_THRESHOLD,
                DEFAULT_DISK_FREE_CHANGE_REPORTING_THRESHOLD);
        // If mFree changed significantly log the new value
        Int64 delta = mFreeMem - mLastReportedFreeMem;
        if (delta > threshold || delta < -threshold) {
            mLastReportedFreeMem = mFreeMem;
            EventLog->WriteEvent(EventLogTags.FREE_STORAGE_CHANGED, mFreeMem);
        }
    }

    private void ClearCache() {
        if (mClearCacheObserver == NULL) {
            // Lazy instantiation
            mClearCacheObserver = new CachePackageDataObserver();
        }
        mClearingCache = TRUE;
        try {
            if (localLOGV) Slogger::I(TAG, "Clearing cache");
            IPackageManager.Stub->AsInterface(ServiceManager->GetService("package")).
                    FreeStorageAndNotify(mMemCacheTrimToThreshold, mClearCacheObserver);
        } catch (RemoteException e) {
            Slogger::W(TAG, "Failed to get handle for PackageManger Exception: "+e);
            mClearingCache = FALSE;
            mClearSucceeded = FALSE;
        }
    }

    void CheckMemory(Boolean checkCache) {
        //if the thread that was started to clear cache is still running do nothing till its
        //finished clearing cache. Ideally this flag could be modified by clearCache
        // and should be accessed via a lock but even if it does this test will fail now and
        //hopefully the next time this flag will be set to the correct value.
        If(mClearingCache) {
            If(localLOGV) Slogger::I(TAG, "Thread already running just skip");
            //make sure the thread is not hung for too Int64
            Int64 diffTime = System->CurrentTimeMillis() - mThreadStartTime;
            If(diffTime > (10*60*1000)) {
                Slogger::W(TAG, "Thread that clears cache file seems to run for ever");
            }
        } else {
            RestatDataDir();
            if (localLOGV)  Slogger::V(TAG, "freeMemory="+mFreeMem);

            //post intent to NotificationManager to display icon if necessary
            if (mFreeMem < mMemLowThreshold) {
                if (checkCache) {
                    // We are allowed to clear cache files at this point to
                    // try to get down below the limit, because this is not
                    // the initial call after a cache clear has been attempted.
                    // In this case we will try a cache clear if our free
                    // space has gone below the cache clear limit.
                    if (mFreeMem < mMemCacheStartTrimThreshold) {
                        // We only clear the cache if the free storage has changed
                        // a significant amount since the last time.
                        if ((mFreeMemAfterLastCacheClear-mFreeMem)
                                >= ((mMemLowThreshold-mMemCacheStartTrimThreshold)/4)) {
                            // See if clearing cache helps
                            // Note that clearing cache is asynchronous and so we do a
                            // memory check again once the cache has been cleared.
                            mThreadStartTime = System->CurrentTimeMillis();
                            mClearSucceeded = FALSE;
                            ClearCache();
                        }
                    }
                } else {
                    // This is a call from after clearing the cache.  Note
                    // the amount of free storage at this point.
                    mFreeMemAfterLastCacheClear = mFreeMem;
                    if (!mLowMemFlag) {
                        // We tried to clear the cache, but that didn't get us
                        // below the low storage limit.  Tell the user.
                        Slogger::I(TAG, "Running low on memory. Sending notification");
                        SendNotification();
                        mLowMemFlag = TRUE;
                    } else {
                        if (localLOGV) Slogger::V(TAG, "Running low on memory " +
                                "notification already sent. do nothing");
                    }
                }
            } else {
                mFreeMemAfterLastCacheClear = mFreeMem;
                if (mLowMemFlag) {
                    Slogger::I(TAG, "Memory available. Cancelling notification");
                    CancelNotification();
                    mLowMemFlag = FALSE;
                }
            }
            if (!mLowMemFlag && !mIsBootImageOnDisk) {
                Slogger::I(TAG, "No boot image on disk due to lack of space. Sending notification");
                SendNotification();
            }
            if (mFreeMem < mMemFullThreshold) {
                if (!mMemFullFlag) {
                    SendFullNotification();
                    mMemFullFlag = TRUE;
                }
            } else {
                if (mMemFullFlag) {
                    CancelFullNotification();
                    mMemFullFlag = FALSE;
                }
            }
        }
        If(localLOGV) Slogger::I(TAG, "Posting Message again");
        //keep posting messages to itself periodically
        PostCheckMemoryMsg(TRUE, DEFAULT_CHECK_INTERVAL);
    }

    void PostCheckMemoryMsg(Boolean clearCache, Int64 delay) {
        // Remove queued messages
        mHandler->RemoveMessages(DEVICE_MEMORY_WHAT);
        mHandler->SendMessageDelayed(mHandler->ObtainMessage(DEVICE_MEMORY_WHAT,
                clearCache ?_TRUE : _FALSE, 0),
                delay);
    }

    public DeviceStorageMonitorService(Context context) {
        Super(context);
        mLastReportedFreeMemTime = 0;
        mResolver = context->GetContentResolver();
        mIsBootImageOnDisk = IsBootImageOnDisk();
        //create StatFs object
        mDataFileStats = new StatFs(DATA_PATH->GetAbsolutePath());
        mSystemFileStats = new StatFs(SYSTEM_PATH->GetAbsolutePath());
        mCacheFileStats = new StatFs(CACHE_PATH->GetAbsolutePath());
        //initialize total storage on device
        mTotalMemory = (Int64)mDataFileStats->GetBlockCount() *
                        mDataFileStats->GetBlockSize();
        mStorageLowIntent = new Intent(IIntent::ACTION_DEVICE_STORAGE_LOW);
        mStorageLowIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        mStorageOkIntent = new Intent(IIntent::ACTION_DEVICE_STORAGE_OK);
        mStorageOkIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        mStorageFullIntent = new Intent(IIntent::ACTION_DEVICE_STORAGE_FULL);
        mStorageFullIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        mStorageNotFullIntent = new Intent(IIntent::ACTION_DEVICE_STORAGE_NOT_FULL);
        mStorageNotFullIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    }

    private static Boolean IsBootImageOnDisk() {
        for (String instructionSet : PackageManagerService->GetAllDexCodeInstructionSets()) {
            if (!VMRuntime->IsBootClassPathOnDisk(instructionSet)) {
                return FALSE;
            }
        }
        return TRUE;
    }

    /**
    * Initializes the disk space threshold value and posts an empty message to
    * kickstart the process.
    */
    //@Override
    CARAPI OnStart() {
        // cache storage thresholds
        final StorageManager sm = StorageManager->From(GetContext());
        mMemLowThreshold = sm->GetStorageLowBytes(DATA_PATH);
        mMemFullThreshold = sm->GetStorageFullBytes(DATA_PATH);

        mMemCacheStartTrimThreshold = ((mMemLowThreshold*3)+mMemFullThreshold)/4;
        mMemCacheTrimToThreshold = mMemLowThreshold
                + ((mMemLowThreshold-mMemCacheStartTrimThreshold)*2);
        mFreeMemAfterLastCacheClear = mTotalMemory;
        CheckMemory(TRUE);

        mCacheFileDeletedObserver = new CacheFileDeletedObserver();
        mCacheFileDeletedObserver->StartWatching();

        PublishBinderService(SERVICE, mRemoteService);
        PublishLocalService(DeviceStorageMonitorInternal.class, mLocalService);
    }

    private final DeviceStorageMonitorInternal mLocalService = new DeviceStorageMonitorInternal() {
        //@Override
        CARAPI CheckMemory() {
            // force an early check
            PostCheckMemoryMsg(TRUE, 0);
        }

        //@Override
        public Boolean IsMemoryLow() {
            return mLowMemFlag || !mIsBootImageOnDisk;
        }

        //@Override
        public Int64 GetMemoryLowThreshold() {
            return mMemLowThreshold;
        }
    };

    private final IBinder mRemoteService = new Binder() {
        //@Override
        protected void Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
            if (GetContext()->CheckCallingOrSelfPermission(Manifest::permission::DUMP)
                    != PackageManager.PERMISSION_GRANTED) {

                pw->Println("Permission Denial: can't dump " + SERVICE + " from from pid="
                        + Binder->GetCallingPid()
                        + ", uid=" + Binder->GetCallingUid());
                return;
            }

            DumpImpl(pw);
        }
    };

    void DumpImpl(PrintWriter pw) {
        final Context context = GetContext();

        pw->Println("Current DeviceStorageMonitor state:");

        pw->Print("  mFreeMem="); pw->Print(Formatter->FormatFileSize(context, mFreeMem));
        pw->Print(" mTotalMemory=");
        pw->Println(Formatter->FormatFileSize(context, mTotalMemory));

        pw->Print("  mFreeMemAfterLastCacheClear=");
        pw->Println(Formatter->FormatFileSize(context, mFreeMemAfterLastCacheClear));

        pw->Print("  mLastReportedFreeMem=");
        pw->Print(Formatter->FormatFileSize(context, mLastReportedFreeMem));
        pw->Print(" mLastReportedFreeMemTime=");
        TimeUtils->FormatDuration(mLastReportedFreeMemTime, SystemClock->ElapsedRealtime(), pw);
        pw->Println();

        pw->Print("  mLowMemFlag="); pw->Print(mLowMemFlag);
        pw->Print(" mMemFullFlag="); pw->Println(mMemFullFlag);
        pw->Print(" mIsBootImageOnDisk="); pw->Print(mIsBootImageOnDisk);

        pw->Print("  mClearSucceeded="); pw->Print(mClearSucceeded);
        pw->Print(" mClearingCache="); pw->Println(mClearingCache);

        pw->Print("  mMemLowThreshold=");
        pw->Print(Formatter->FormatFileSize(context, mMemLowThreshold));
        pw->Print(" mMemFullThreshold=");
        pw->Println(Formatter->FormatFileSize(context, mMemFullThreshold));

        pw->Print("  mMemCacheStartTrimThreshold=");
        pw->Print(Formatter->FormatFileSize(context, mMemCacheStartTrimThreshold));
        pw->Print(" mMemCacheTrimToThreshold=");
        pw->Println(Formatter->FormatFileSize(context, mMemCacheTrimToThreshold));
    }

    /**
    * This method sends a notification to NotificationManager to display
    * an error dialog indicating low disk space and launch the Installer
    * application
    */
    private void SendNotification() {
        final Context context = GetContext();
        If(localLOGV) Slogger::I(TAG, "Sending low memory notification");
        //log the event to event log with the amount of free Storage(in bytes) left on the device
        EventLog->WriteEvent(EventLogTags.LOW_STORAGE, mFreeMem);
        //  Pack up the values and broadcast them to everyone
        Intent lowMemIntent = new Intent(Environment->IsExternalStorageEmulated()
                ? Settings.ACTION_INTERNAL_STORAGE_SETTINGS
                : IIntent::ACTION_MANAGE_PACKAGE_STORAGE);
        lowMemIntent->PutExtra("memory", mFreeMem);
        lowMemIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        NotificationManager mNotificationMgr =
                (NotificationManager)context->GetSystemService(
                        Context.NOTIFICATION_SERVICE);
        CharSequence title = context->GetText(
                R::string::low_internal_storage_view_title);
        CharSequence details = context->GetText(mIsBootImageOnDisk
                ? R::string::low_internal_storage_view_text
                : R::string::low_internal_storage_view_text_no_boot);
        PendingIntent intent = PendingIntent->GetActivityAsUser(context, 0,  lowMemIntent, 0,
                NULL, UserHandle.CURRENT);
        Notification notification = new Notification->Builder(context)
                .SetSmallIcon(R.drawable.stat_notify_disk_full)
                .SetTicker(title)
                .SetColor(context->GetResources()->GetColor(
                    R::Color::system_notification_accent_color))
                .SetContentTitle(title)
                .SetContentText(details)
                .SetContentIntent(intent)
                .SetStyle(new Notification->BigTextStyle()
                      .BigText(details))
                .SetVisibility(Notification.VISIBILITY_PUBLIC)
                .SetCategory(Notification.CATEGORY_SYSTEM)
                .Build();
        notification.flags |= Notification.FLAG_NO_CLEAR;
        mNotificationMgr->NotifyAsUser(NULL, LOW_MEMORY_NOTIFICATION_ID, notification,
                UserHandle.ALL);
        context->SendStickyBroadcastAsUser(mStorageLowIntent, UserHandle.ALL);
    }

    /**
     * Cancels low storage notification and sends OK intent.
     */
    private void CancelNotification() {
        final Context context = GetContext();
        If(localLOGV) Slogger::I(TAG, "Canceling low memory notification");
        NotificationManager mNotificationMgr =
                (NotificationManager)context->GetSystemService(
                        Context.NOTIFICATION_SERVICE);
        //cancel notification since memory has been freed
        mNotificationMgr->CancelAsUser(NULL, LOW_MEMORY_NOTIFICATION_ID, UserHandle.ALL);

        context->RemoveStickyBroadcastAsUser(mStorageLowIntent, UserHandle.ALL);
        context->SendBroadcastAsUser(mStorageOkIntent, UserHandle.ALL);
    }

    /**
     * Send a notification when storage is full.
     */
    private void SendFullNotification() {
        If(localLOGV) Slogger::I(TAG, "Sending memory full notification");
        GetContext()->SendStickyBroadcastAsUser(mStorageFullIntent, UserHandle.ALL);
    }

    /**
     * Cancels memory full notification and sends "not full" intent.
     */
    private void CancelFullNotification() {
        If(localLOGV) Slogger::I(TAG, "Canceling memory full notification");
        GetContext()->RemoveStickyBroadcastAsUser(mStorageFullIntent, UserHandle.ALL);
        GetContext()->SendBroadcastAsUser(mStorageNotFullIntent, UserHandle.ALL);
    }

    private static class CacheFileDeletedObserver extends FileObserver {
        public CacheFileDeletedObserver() {
            Super(Environment->GetDownloadCacheDirectory()->GetAbsolutePath(), FileObserver.DELETE);
        }

        //@Override
        CARAPI OnEvent(Int32 event, String path) {
            EventLogTags->WriteCacheFileDeleted(path);
        }
    }
}
