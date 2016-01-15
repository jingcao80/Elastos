#include "CDeviceStorageMonitorService.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/R.h"

using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Droid::R;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::EIID_IPackageDataObserver;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IFileObserver;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Os::SystemClock;
using Elastos::Utility::Logging::Slogger;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Server {

const String CDeviceStorageMonitorService::TAG("DeviceStorageMonitorService");
const Boolean CDeviceStorageMonitorService::DEBUG = FALSE;
const Boolean CDeviceStorageMonitorService::localLOGV = FALSE;
const Int32 CDeviceStorageMonitorService::DEVICE_MEMORY_WHAT;
const Int32 CDeviceStorageMonitorService::MONITOR_INTERVAL; //in minutes
const Int32 CDeviceStorageMonitorService::LOW_MEMORY_NOTIFICATION_ID;
const Int32 CDeviceStorageMonitorService::DEFAULT_THRESHOLD_PERCENTAGE;
const Int32 CDeviceStorageMonitorService::DEFAULT_THRESHOLD_MAX_BYTES; // 500MB
const Int32 CDeviceStorageMonitorService::DEFAULT_FREE_STORAGE_LOG_INTERVAL_IN_MINUTES; //in minutes
const Int64 CDeviceStorageMonitorService::DEFAULT_DISK_FREE_CHANGE_REPORTING_THRESHOLD = 2 * 1024 * 1024; // 2MB
const Int64 CDeviceStorageMonitorService::DEFAULT_CHECK_INTERVAL = MONITOR_INTERVAL*60*1000;
const Int32 CDeviceStorageMonitorService::DEFAULT_FULL_THRESHOLD_BYTES; // 1MB

const String CDeviceStorageMonitorService::DATA_PATH("/data");
const String CDeviceStorageMonitorService::SYSTEM_PATH("/system");
const String CDeviceStorageMonitorService::CACHE_PATH("/cache");

const Int32 CDeviceStorageMonitorService::_TRUE;
const Int32 CDeviceStorageMonitorService::_FALSE;

const String CDeviceStorageMonitorService::SERVICE("devicestoragemonitor");


ECode CDeviceStorageMonitorService::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);

    //don't handle an invalid message
    if (what != DEVICE_MEMORY_WHAT) {
        Slogger::E(CDeviceStorageMonitorService::TAG, "Will not process invalid message");
        return NOERROR;
    }

    mHost->CheckMemory(arg1 == _TRUE);
    return NOERROR;
}

CDeviceStorageMonitorService::CDeviceStorageMonitorService()
    : mFreeMem(0L)
    , mFreeMemAfterLastCacheClear(0L)
    , mLastReportedFreeMem(0)
    , mLastReportedFreeMemTime(0)
    , mLowMemFlag(FALSE)
    , mMemFullFlag(FALSE)
    , mThreadStartTime(-1)
    , mClearSucceeded(FALSE)
    , mClearingCache(FALSE)
    , mMemLowThreshold(0)
    , mMemCacheStartTrimThreshold(0)
    , mMemCacheTrimToThreshold(0)
    , mMemFullThreshold(0)
{
    mHandler = new MyHandler(this);
}

ECode CDeviceStorageMonitorService::ToString(
    /* [out] */ String* res)
{
    return E_NOT_IMPLEMENTED;
}

CAR_INTERFACE_IMPL_2(CDeviceStorageMonitorService::CachePackageDataObserver, IPackageDataObserver, IBinder);

CDeviceStorageMonitorService::CachePackageDataObserver::CachePackageDataObserver(
    /* [in] */ CDeviceStorageMonitorService* host)
{
    mHost = host;
}

ECode CDeviceStorageMonitorService::CachePackageDataObserver::OnRemoveCompleted(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean succeeded)
{
    mHost->mClearSucceeded = succeeded;
    mHost->mClearingCache = FALSE;
    if(localLOGV)
        Slogger::I(TAG, " Clear succeeded:%d, mClearingCache:%d Forcing memory check", mHost->mClearSucceeded, mHost->mClearingCache);
    mHost->PostCheckMemoryMsg(FALSE, 0, 0, 0);
    return NOERROR;
}

ECode CDeviceStorageMonitorService::CachePackageDataObserver::GetHashCode(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (Int32)this;
    return NOERROR;
}

ECode CDeviceStorageMonitorService::CachePackageDataObserver::ToString(
    /* [out] */ String* res)
{
    return E_NOT_IMPLEMENTED;
}

void CDeviceStorageMonitorService::RestatDataDir()
{
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    mDataFileStats = NULL;
    os->Statfs(DATA_PATH, (IStructStatFs**)&mDataFileStats);

    Int64 bavail;
    mDataFileStats->GetBavail(&bavail);

    Int64 bsize;
    mDataFileStats->GetBsize(&bsize);

    mFreeMem = bavail * bsize;

    // Allow freemem to be overridden by debug.freemem for testing
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String debugFreeMem;
    sysProp->Get(String("debug.freemem"), &debugFreeMem);
    if (!debugFreeMem.IsNullOrEmpty()) {
        mFreeMem = StringUtils::ParseInt64(debugFreeMem);
    }
    // Read the log interval from secure settings
    Int64 freeMemLogInterval;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetInt64(mContentResolver,
            ISettingsGlobal::SYS_FREE_STORAGE_LOG_INTERVAL,
            DEFAULT_FREE_STORAGE_LOG_INTERVAL_IN_MINUTES, &freeMemLogInterval);
    freeMemLogInterval = freeMemLogInterval * 60 * 1000;
    //log the amount of free memory in event log
    Int64 currTime = SystemClock::GetElapsedRealtime();
    if((mLastReportedFreeMemTime == 0) ||
       (currTime-mLastReportedFreeMemTime) >= freeMemLogInterval) {
        mLastReportedFreeMemTime = currTime;
        Int64 mFreeSystem = -1;
        Int64 mFreeCache = -1;

        mSystemFileStats = NULL;
        os->Statfs(SYSTEM_PATH, (IStructStatFs**)&mSystemFileStats);
        mSystemFileStats->GetBavail(&bavail);
        mSystemFileStats->GetBsize(&bsize);
        mFreeSystem = bavail * bsize;

        mCacheFileStats = NULL;
        os->Statfs(CACHE_PATH, (IStructStatFs**)&mCacheFileStats);
        mCacheFileStats->GetBavail(&bavail);
        mCacheFileStats->GetBsize(&bsize);
        mFreeCache = bavail * bsize;

        //EventLog.writeEvent(EventLogTags.FREE_STORAGE_LEFT,
        //                    mFreeMem, mFreeSystem, mFreeCache);
    }
    // Read the reporting threshold from secure settings
    Int64 threshold;
    settingsGlobal->GetInt64(mContentResolver,
            ISettingsGlobal::DISK_FREE_CHANGE_REPORTING_THRESHOLD,
            DEFAULT_DISK_FREE_CHANGE_REPORTING_THRESHOLD, &threshold);
    // If mFree changed significantly log the new value
    Int64 delta = mFreeMem - mLastReportedFreeMem;
    if (delta > threshold || delta < -threshold) {
        mLastReportedFreeMem = mFreeMem;
        //EventLog.writeEvent(EventLogTags.FREE_STORAGE_CHANGED, mFreeMem);
    }
}

void CDeviceStorageMonitorService::ClearCache()
{
    if (mClearCacheObserver == NULL) {
        // Lazy instantiation
        mClearCacheObserver = new CachePackageDataObserver(this);
    }
    mClearingCache = TRUE;

    if (localLOGV)
        Slogger::I(TAG, "Clearing cache");
    AutoPtr<IIPackageManager> pm = IIPackageManager::Probe(ServiceManager::GetService(String("package")));
    if (FAILED(pm->FreeStorageAndNotify(mMemCacheTrimToThreshold, mClearCacheObserver))) {
        Slogger::W(TAG, "Failed to get handle for PackageManger Exception: ");
        mClearingCache = FALSE;
        mClearSucceeded = FALSE;
    }
}

void CDeviceStorageMonitorService::CheckMemory(
    /* [in] */ Boolean checkCache)
{
    //if the thread that was started to clear cache is still running do nothing till its
    //finished clearing cache. Ideally this flag could be modified by clearCache
    // and should be accessed via a lock but even if it does this test will fail now and
    //hopefully the next time this flag will be set to the correct value.
    if(mClearingCache) {
        if(localLOGV)
            Slogger::I(TAG, "Thread already running just skip");
        //make sure the thread is not hung for too long
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 millis;
        system->GetCurrentTimeMillis(&millis);
        Int64 diffTime = millis - mThreadStartTime;
        if(diffTime > (10 * 60 * 1000)) {
            Slogger::W(TAG, "Thread that clears cache file seems to run for ever");
        }
    }
    else
    {
        RestatDataDir();
        if (localLOGV)
            Slogger::V(TAG, "freeMemory=%d", mFreeMem);

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
                        AutoPtr<ISystem> system;
                        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
                        Int64 millis;
                        system->GetCurrentTimeMillis(&millis);
                        mThreadStartTime = millis;
                        mClearSucceeded = FALSE;
                        ClearCache();
                    }
                }
            }
            else
            {
                // This is a call from after clearing the cache.  Note
                // the amount of free storage at this point.
                mFreeMemAfterLastCacheClear = mFreeMem;
                if (!mLowMemFlag) {
                    // We tried to clear the cache, but that didn't get us
                    // below the low storage limit.  Tell the user.
                    Slogger::I(TAG, "Running low on memory. Sending notification");
                    SendNotification();
                    mLowMemFlag = TRUE;
                }
                else
                {
                    if (localLOGV)
                        Slogger::V(TAG, "Running low on memory notification already sent. do nothing");
                }
            }
        }
        else
        {
            mFreeMemAfterLastCacheClear = mFreeMem;
            if (mLowMemFlag) {
                Slogger::I(TAG, "Memory available. Cancelling notification");
                CancelNotification();
                mLowMemFlag = FALSE;
            }
        }
        if (mFreeMem < mMemFullThreshold) {
            if (!mMemFullFlag) {
                SendFullNotification();
                mMemFullFlag = TRUE;
            }
        }
        else
        {
            if (mMemFullFlag) {
                CancelFullNotification();
                mMemFullFlag = FALSE;
            }
        }
    }
    if(localLOGV)
        Slogger::I(TAG, "Posting Message again");
    //keep posting messages to itself periodically
    PostCheckMemoryMsg(TRUE, DEFAULT_CHECK_INTERVAL, 0, 0);
}

void CDeviceStorageMonitorService::PostCheckMemoryMsg(
    /* [in] */ Boolean clearCache,
    /* [in] */ Int64 delay,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1)
{
    // Remove queued messages
    mHandler->RemoveMessages(DEVICE_MEMORY_WHAT);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(DEVICE_MEMORY_WHAT, (IMessage**)&msg);
    msg->SetArg1(clearCache ?_TRUE : _FALSE);
    Boolean result;
    mHandler->SendMessageDelayed(msg, delay, &result);
}

/*
 * just query settings to retrieve the memory threshold.
 * Preferred this over using a ContentObserver since Settings.Secure caches the value
 * any way
 */
Int64 CDeviceStorageMonitorService::GetMemThreshold()
{
    Int64 value;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetInt64(
                          mContentResolver,
                          ISettingsGlobal::SYS_STORAGE_THRESHOLD_PERCENTAGE,
                          DEFAULT_THRESHOLD_PERCENTAGE,
                          &value);
    if(localLOGV)
        Slogger::V(TAG, "Threshold Percentage=%d",value);
    value = (value*mTotalMemory)/100;
    Int64 maxValue;
    settingsGlobal->GetInt64(
            mContentResolver,
            ISettingsGlobal::SYS_STORAGE_THRESHOLD_MAX_BYTES,
            DEFAULT_THRESHOLD_MAX_BYTES,
            &maxValue);
    //evaluate threshold value
    return value < maxValue ? value : maxValue;
}

/*
 * just query settings to retrieve the memory full threshold.
 * Preferred this over using a ContentObserver since Settings.Secure caches the value
 * any way
 */
Int32 CDeviceStorageMonitorService::GetMemFullThreshold()
{
    Int32 value;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetInt32(
            mContentResolver,
            ISettingsGlobal::SYS_STORAGE_FULL_THRESHOLD_BYTES,
            DEFAULT_FULL_THRESHOLD_BYTES,
            &value);
    if(localLOGV)
        Slogger::V(TAG, "Full Threshold Bytes=%d", value);

    return value;
}

ECode CDeviceStorageMonitorService::constructor(
    /* [in] */ IContext* context)
{
    mLastReportedFreeMemTime = 0;
    mContext = context;
    mContext->GetContentResolver((IContentResolver**)&mContentResolver);
    //create StatFs object
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    os->Statfs(DATA_PATH, (IStructStatFs**)&mDataFileStats);
    os->Statfs(SYSTEM_PATH, (IStructStatFs**)&mSystemFileStats);
    os->Statfs(CACHE_PATH, (IStructStatFs**)&mCacheFileStats);
    Int64 blocks, bsize;
    mDataFileStats->GetBlocks(&blocks);
    mDataFileStats->GetBsize(&bsize);
    //initialize total storage on device
    mTotalMemory = blocks * bsize;
    CIntent::New(IIntent::ACTION_DEVICE_STORAGE_LOW, (IIntent**)&mStorageLowIntent);
    mStorageLowIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    CIntent::New(IIntent::ACTION_DEVICE_STORAGE_OK, (IIntent**)&mStorageOkIntent);
    mStorageOkIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    CIntent::New(IIntent::ACTION_DEVICE_STORAGE_FULL, (IIntent**)&mStorageFullIntent);
    mStorageFullIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    CIntent::New(IIntent::ACTION_DEVICE_STORAGE_NOT_FULL, (IIntent**)&mStorageNotFullIntent);
    mStorageNotFullIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    // cache storage thresholds
    mMemLowThreshold = GetMemThreshold();
    mMemFullThreshold = GetMemFullThreshold();
    mMemCacheStartTrimThreshold = ((mMemLowThreshold * 3) + mMemFullThreshold) / 4;
    mMemCacheTrimToThreshold = mMemLowThreshold
            + ((mMemLowThreshold-mMemCacheStartTrimThreshold) * 2);
    mFreeMemAfterLastCacheClear = mTotalMemory;
    CheckMemory(TRUE);
    mCacheFileDeletedObserver = new CacheFileDeletedObserver();
    mCacheFileDeletedObserver->StartWatching();
    mHandler = new MyHandler(this);
    return NOERROR;
}

void CDeviceStorageMonitorService::SendNotification()
{
    if(localLOGV)
        Slogger::I(TAG, "Sending low memory notification");
    //log the event to event log with the amount of free storage(in bytes) left on the device
    //EventLog.writeEvent(EventLogTags.LOW_STORAGE, mFreeMem);
    //  Pack up the values and broadcast them to everyone
    AutoPtr<IIntent> lowMemIntent;
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    Boolean isEmulated;
    env->IsExternalStorageEmulated(&isEmulated);
    CIntent::New(isEmulated
            ? ISettings::ACTION_INTERNAL_STORAGE_SETTINGS
            : IIntent::ACTION_MANAGE_PACKAGE_STORAGE, (IIntent**)&lowMemIntent);

    lowMemIntent->PutExtra(String("memory"), mFreeMem);
    lowMemIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    AutoPtr<INotificationManager> mNotificationMgr;
    mContext->GetSystemService(
                    IContext::NOTIFICATION_SERVICE, (IInterface**)&mNotificationMgr);
    AutoPtr<ICharSequence> title;
    mContext->GetText(R::string::low_internal_storage_view_title, (ICharSequence**)&title);
    AutoPtr<ICharSequence> details;
    mContext->GetText(R::string::low_internal_storage_view_text, (ICharSequence**)&details);

    AutoPtr<IPendingIntent> intent;
    AutoPtr<IPendingIntentHelper> pih;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pih);
    AutoPtr<IUserHandle> UserHandle_CURRENT;
    CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&UserHandle_CURRENT);
    pih->GetActivityAsUser(mContext, 0,  lowMemIntent, 0,
            NULL, UserHandle_CURRENT, (IPendingIntent**)&intent);

    AutoPtr<INotification> notification;
    CNotification::New((INotification**)&notification);
    notification->SetIcon(0x01080519);//com.android.internal.R.drawable.stat_notify_disk_full
    notification->SetTickerText(title);
    Int32 flags;
    notification->GetFlags(&flags);
    notification->SetFlags(flags |= INotification::FLAG_NO_CLEAR);
    notification->SetLatestEventInfo(mContext, title, details, intent);

    AutoPtr<IUserHandle> UserHandle_ALL;
    CUserHandle::New(IUserHandle::USER_ALL, (IUserHandle**)&UserHandle_ALL);
    mNotificationMgr->NotifyAsUser(String(NULL), LOW_MEMORY_NOTIFICATION_ID, notification,
            UserHandle_ALL);
    mContext->SendStickyBroadcastAsUser(mStorageLowIntent, UserHandle_ALL);

}

void CDeviceStorageMonitorService::CancelNotification()
{
    if(localLOGV)
        Slogger::I(TAG, "Canceling low memory notification");
    AutoPtr<INotificationManager> mNotificationMgr;
    mContext->GetSystemService(
                    IContext::NOTIFICATION_SERVICE, (IInterface**)&mNotificationMgr);
    //cancel notification since memory has been freed
    AutoPtr<IUserHandle> UserHandle_ALL;
    CUserHandle::New(IUserHandle::USER_ALL, (IUserHandle**)&UserHandle_ALL);
    mNotificationMgr->CancelAsUser(String(NULL), LOW_MEMORY_NOTIFICATION_ID, UserHandle_ALL);

    mContext->RemoveStickyBroadcastAsUser(mStorageLowIntent, UserHandle_ALL);
    mContext->SendBroadcastAsUser(mStorageOkIntent, UserHandle_ALL);
}

void CDeviceStorageMonitorService::SendFullNotification()
{
    if(localLOGV)
        Slogger::I(TAG, "Sending memory full notification");
    AutoPtr<IUserHandle> UserHandle_ALL;
    CUserHandle::New(IUserHandle::USER_ALL, (IUserHandle**)&UserHandle_ALL);
    mContext->SendStickyBroadcastAsUser(mStorageFullIntent, UserHandle_ALL);
}

void CDeviceStorageMonitorService::CancelFullNotification()
{
    if(localLOGV)
        Slogger::I(TAG, "Canceling memory full notification");
    AutoPtr<IUserHandle> UserHandle_ALL;
    CUserHandle::New(IUserHandle::USER_ALL, (IUserHandle**)&UserHandle_ALL);
    mContext->RemoveStickyBroadcastAsUser(mStorageFullIntent, UserHandle_ALL);
    mContext->SendBroadcastAsUser(mStorageNotFullIntent, UserHandle_ALL);
}

ECode CDeviceStorageMonitorService::UpdateMemory()
{
    Int32 callingUid = Binder::GetCallingUid();
    if(callingUid != IProcess::SYSTEM_UID) {
        return E_NULL_POINTER_EXCEPTION;
    }
    // force an early check
    PostCheckMemoryMsg(TRUE, 0, 0, 0);
    return NOERROR;
}

ECode CDeviceStorageMonitorService::GetMemoryLowThreshold(
    /* [out] */ Int64* threshold)
{
    VALIDATE_NOT_NULL(threshold);
    *threshold = mMemLowThreshold;
    return NOERROR;
}

ECode CDeviceStorageMonitorService::IsMemoryLow(
    /* [out] */ Boolean* threshold)
{
    VALIDATE_NOT_NULL(threshold);
    *threshold = mLowMemFlag;
    return NOERROR;
}

CDeviceStorageMonitorService::CacheFileDeletedObserver::CacheFileDeletedObserver()
{
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> file;
    env->GetDownloadCacheDirectory((IFile**)&file);
    String path;
    file->GetAbsolutePath(&path);
    Init(path, IFileObserver::DELETE);
}

ECode CDeviceStorageMonitorService::CacheFileDeletedObserver::OnEvent(
    /* [in] */ Int32 event,
    /* [in] */ const String& path)
{
    //EventLogTags.writeCacheFileDeleted(path);
    return E_NOT_IMPLEMENTED;
}


}//namespace Server
}//namespace Droid
}//namespace Elastos
