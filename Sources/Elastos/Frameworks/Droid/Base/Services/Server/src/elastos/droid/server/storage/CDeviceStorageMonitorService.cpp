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

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/server/storage/CDeviceStorageMonitorService.h"
#include "elastos/droid/server/storage/CCachePackageDataObserver.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/format/Formatter.h"
#include "elastos/droid/utility/TimeUtils.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::INotificationStyle;
using Elastos::Droid::App::INotificationBigTextStyle;
using Elastos::Droid::App::CNotificationBigTextStyle;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::EIID_IIPackageDataObserver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::CStatFs;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Os::Storage::IStorageManagerHelper;
using Elastos::Droid::Os::Storage::CStorageManagerHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Text::Format::Formatter;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::R;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Storage {

//===============================================================================
//                  CDeviceStorageMonitorService::CachePackageDataObserver
//===============================================================================
CAR_INTERFACE_IMPL_2(CDeviceStorageMonitorService::CachePackageDataObserver, Object, IIPackageDataObserver, IBinder)

ECode CDeviceStorageMonitorService::CachePackageDataObserver::constructor(
    /* [in] */ ISystemService* host)
{
    mHost = (CDeviceStorageMonitorService*)host;
    return NOERROR;
}

ECode CDeviceStorageMonitorService::CachePackageDataObserver::OnRemoveCompleted(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean succeeded)
{
    mHost->mClearSucceeded = succeeded;
    mHost->mClearingCache = FALSE;
    if (localLOGV) {
        Slogger::I(TAG, " Clear succeeded:%d, mClearingCache:%d Forcing memory check",
                mHost->mClearSucceeded, mHost->mClearingCache);
    }
    mHost->PostCheckMemoryMsg(FALSE, 0);
    return NOERROR;
}


//===============================================================================
//                  CDeviceStorageMonitorService::CacheFileDeletedObserver
//===============================================================================
ECode CDeviceStorageMonitorService::CacheFileDeletedObserver::constructor()
{
    AutoPtr<IEnvironment> environment;
    CEnvironment::AcquireSingleton((IEnvironment**)&environment);
    AutoPtr<IFile> dir;
    environment->GetDownloadCacheDirectory((IFile**)&dir);
    String path;
    dir->GetAbsolutePath(&path);
    return FileObserver::constructor(path, FileObserver::DELETE);
}

ECode CDeviceStorageMonitorService::CacheFileDeletedObserver::OnEvent(
    /* [in] */ Int32 event,
    /* [in] */ const String& path)
{
    Slogger::E(TAG, "EventLogTags is TODO");
    // EventLogTags->WriteCacheFileDeleted(path);
    return E_NOT_IMPLEMENTED;
}


//===============================================================================
//                  CDeviceStorageMonitorService::MyHandler
//===============================================================================
ECode CDeviceStorageMonitorService::MyHandler::constructor(
    /* [in] */ CDeviceStorageMonitorService* host)
{
    mHost = host;
    return Handler::constructor();
}

ECode CDeviceStorageMonitorService::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    //don't handle an invalid message
    Int32 what;
    msg->GetWhat(&what);
    if (what != DEVICE_MEMORY_WHAT) {
        Slogger::E(TAG, "Will not process invalid message");
        return NOERROR;
    }
    Int32 arg1;
    msg->GetArg1(&arg1);
    mHost->CheckMemory(arg1 == _TRUE);
    return NOERROR;
}


//===============================================================================
//                  CDeviceStorageMonitorService::MyDeviceStorageMonitorInternal
//===============================================================================
CAR_INTERFACE_IMPL(CDeviceStorageMonitorService::MyDeviceStorageMonitorInternal, Object, IDeviceStorageMonitorInternal)

CDeviceStorageMonitorService::MyDeviceStorageMonitorInternal::MyDeviceStorageMonitorInternal(
    /* [in] */ CDeviceStorageMonitorService* host)
    : mHost(host)
{}

ECode CDeviceStorageMonitorService::MyDeviceStorageMonitorInternal::CheckMemory()
{
    // force an early check
    mHost->PostCheckMemoryMsg(TRUE, 0);
    return NOERROR;
}

ECode CDeviceStorageMonitorService::MyDeviceStorageMonitorInternal::IsMemoryLow(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->mLowMemFlag || !mHost->mIsBootImageOnDisk;
    return NOERROR;
}

ECode CDeviceStorageMonitorService::MyDeviceStorageMonitorInternal::GetMemoryLowThreshold(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->mMemLowThreshold;
    return NOERROR;
}


//===============================================================================
//                  CDeviceStorageMonitorService
//===============================================================================
const String CDeviceStorageMonitorService::TAG("CDeviceStorageMonitorService");

const Boolean CDeviceStorageMonitorService::DEBUG = FALSE;
const Boolean CDeviceStorageMonitorService::localLOGV = FALSE;

const Int32 CDeviceStorageMonitorService::DEVICE_MEMORY_WHAT = 1;
const String CDeviceStorageMonitorService::SERVICE("devicestoragemonitor");
const Int32 CDeviceStorageMonitorService::MONITOR_INTERVAL = 1; //in minutes
const Int32 CDeviceStorageMonitorService::LOW_MEMORY_NOTIFICATION_ID = 1;

const Int32 CDeviceStorageMonitorService::DEFAULT_FREE_STORAGE_LOG_INTERVAL_IN_MINUTES = 12*60; //in minutes
const Int64 CDeviceStorageMonitorService::DEFAULT_DISK_FREE_CHANGE_REPORTING_THRESHOLD = 2 * 1024 * 1024; // 2MB
const Int64 CDeviceStorageMonitorService::DEFAULT_CHECK_INTERVAL = MONITOR_INTERVAL*60*1000;

static AutoPtr<IFile> InitEnvironmentDirectory(
    /* [in] */ const String& str)
{
    AutoPtr<IEnvironment> environment;
    CEnvironment::AcquireSingleton((IEnvironment**)&environment);
    AutoPtr<IFile> file;
    if (str.Equals("Data")) {
        environment->GetDataDirectory((IFile**)&file);
    }
    else if (str.Equals("Root")) {
        environment->GetRootDirectory((IFile**)&file);
    }
    else if (str.Equals("DownloadCache")) {
        environment->GetDownloadCacheDirectory((IFile**)&file);
    }
    else {
        Slogger::D("CDeviceStorageMonitorService", "don't come here");
    }
    return file;
}

const AutoPtr<IFile> CDeviceStorageMonitorService::DATA_PATH = InitEnvironmentDirectory(String("Data"));
const AutoPtr<IFile> CDeviceStorageMonitorService::SYSTEM_PATH = InitEnvironmentDirectory(String("Root"));
const AutoPtr<IFile> CDeviceStorageMonitorService::CACHE_PATH = InitEnvironmentDirectory(String("DownloadCache"));

const Int32 CDeviceStorageMonitorService::_TRUE = 1;
const Int32 CDeviceStorageMonitorService::_FALSE = 0;

CAR_OBJECT_IMPL(CDeviceStorageMonitorService)

CDeviceStorageMonitorService::CDeviceStorageMonitorService()
    : mLowMemFlag(FALSE)
    , mClearSucceeded(FALSE)
    , mClearingCache(FALSE)
    , mMemLowThreshold(0)
    , mFreeMem(0)
    , mFreeMemAfterLastCacheClear(0)
    , mLastReportedFreeMem(0)
    , mLastReportedFreeMemTime(0)
    , mMemFullFlag(FALSE)
    , mIsBootImageOnDisk(FALSE)
    , mTotalMemory(0)
    , mThreadStartTime(-1)
    , mMemCacheStartTrimThreshold(0)
    , mMemCacheTrimToThreshold(0)
    , mMemFullThreshold(0)
{}

ECode CDeviceStorageMonitorService::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);

    mHandler = new MyHandler();
    mHandler->constructor(this);

    mLocalService = new MyDeviceStorageMonitorInternal(this);

    // mRemoteService = ?

    mLastReportedFreeMemTime = 0;
    context->GetContentResolver((IContentResolver**)&mResolver);
    mIsBootImageOnDisk = IsBootImageOnDisk();
    //create StatFs object
    String path;
    DATA_PATH->GetAbsolutePath(&path);
    CStatFs::New(path, (IStatFs**)&mDataFileStats);
    SYSTEM_PATH->GetAbsolutePath(&path);
    CStatFs::New(path, (IStatFs**)&mSystemFileStats);
    CACHE_PATH->GetAbsolutePath(&path);
    CStatFs::New(path, (IStatFs**)&mCacheFileStats);
    //initialize total storage on device
    Int64 count, size;
    mDataFileStats->GetBlockCount(&count);
    mDataFileStats->GetBlockSize(&size);
    mTotalMemory = (Int64) count * size;
    CIntent::New(IIntent::ACTION_DEVICE_STORAGE_LOW, (IIntent**)&mStorageLowIntent);
    mStorageLowIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    CIntent::New(IIntent::ACTION_DEVICE_STORAGE_OK, (IIntent**)&mStorageOkIntent);
    mStorageOkIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    CIntent::New(IIntent::ACTION_DEVICE_STORAGE_FULL, (IIntent**)&mStorageFullIntent);
    mStorageFullIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    CIntent::New(IIntent::ACTION_DEVICE_STORAGE_NOT_FULL, (IIntent**)&mStorageNotFullIntent);
    mStorageNotFullIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    return NOERROR;
}

void CDeviceStorageMonitorService::RestatDataDir()
{
    // try {
    String path;
    DATA_PATH->GetAbsolutePath(&path);
    ECode ec = mDataFileStats->Restat(path);
    if (SUCCEEDED(ec)) {
        Int64 blocks, size;
        mDataFileStats->GetAvailableBlocks(&blocks);
        mDataFileStats->GetBlockSize(&size);
        mFreeMem = (Int64) blocks * size;
    }
    // } catch (IllegalArgumentException e) {
    //     // use the old value of mFreeMem
    // }
    // Allow freemem to be overridden by debug.freemem for testing
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String debugFreeMem;
    sp->Get(String("debug.freemem"), &debugFreeMem);
    if (!String("").Equals(debugFreeMem)) {
        mFreeMem = StringUtils::ParseInt64(debugFreeMem);
    }
    // Read the log interval from secure settings
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int64 data;
    global->GetInt64(mResolver, ISettingsGlobal::SYS_FREE_STORAGE_LOG_INTERVAL,
            DEFAULT_FREE_STORAGE_LOG_INTERVAL_IN_MINUTES, &data);
    Int64 freeMemLogInterval = data * 60 * 1000;
    //log the amount of free memory in event log
    Int64 currTime = SystemClock::GetElapsedRealtime();
    if ((mLastReportedFreeMemTime == 0) ||
       (currTime-mLastReportedFreeMemTime) >= freeMemLogInterval) {
        mLastReportedFreeMemTime = currTime;
        Int64 mFreeSystem = -1, mFreeCache = -1;
        // try {
        SYSTEM_PATH->GetAbsolutePath(&path);
        ec = mSystemFileStats->Restat(path);
        if (SUCCEEDED(ec)) {
            Int64 blocks, size;
            mSystemFileStats->GetAvailableBlocks(&blocks);
            mSystemFileStats->GetBlockSize(&size);
            mFreeSystem = (Int64) blocks * size;
        }
        // } catch (IllegalArgumentException e) {
        //     // ignore; report -1
        // }
        // try {
        CACHE_PATH->GetAbsolutePath(&path);
        ec = mCacheFileStats->Restat(path);
        if (SUCCEEDED(ec)) {
            Int64 blocks, size;
            mCacheFileStats->GetAvailableBlocks(&blocks);
            mCacheFileStats->GetBlockSize(&size);
            mFreeCache = (Int64) blocks * size;
        }
        // } catch (IllegalArgumentException e) {
        //     // ignore; report -1
        // }
        Slogger::E(TAG, "EventLog is TODO");
        // EventLog->WriteEvent(EventLogTags.FREE_STORAGE_LEFT,
        //                     mFreeMem, mFreeSystem, mFreeCache);
    }
    // Read the reporting threshold from secure settings
    Int64 threshold;
    global->GetInt64(mResolver, ISettingsGlobal::DISK_FREE_CHANGE_REPORTING_THRESHOLD,
            DEFAULT_DISK_FREE_CHANGE_REPORTING_THRESHOLD, &threshold);
    // If mFree changed significantly log the new value
    Int64 delta = mFreeMem - mLastReportedFreeMem;
    if (delta > threshold || delta < -threshold) {
        mLastReportedFreeMem = mFreeMem;
        Slogger::E(TAG, "EventLog is TODO");
        // EventLog->WriteEvent(EventLogTags.FREE_STORAGE_CHANGED, mFreeMem);
    }
}

void CDeviceStorageMonitorService::ClearCache()
{
    if (mClearCacheObserver == NULL) {
        // Lazy instantiation
        CCachePackageDataObserver::New((ISystemService*)this, (IIPackageDataObserver**)&mClearCacheObserver);
    }
    mClearingCache = TRUE;
    if (localLOGV) Slogger::I(TAG, "Clearing cache");
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> obj;
    ECode ec = sm->GetService(String("package"), (IInterface**)&obj);
    if (ec == (ECode) E_REMOTE_EXCEPTION) {
        Slogger::W(TAG, "Failed to get handle for PackageManger Exception: 0x%08x", ec);
        mClearingCache = FALSE;
        mClearSucceeded = FALSE;
    }
    else {
        IIPackageManager::Probe(obj)->FreeStorageAndNotify(mMemCacheTrimToThreshold, mClearCacheObserver);
    }
}

void CDeviceStorageMonitorService::CheckMemory(
    /* [in] */ Boolean checkCache)
{
    //if the thread that was started to clear cache is still running do nothing till its
    //finished clearing cache. Ideally this flag could be modified by clearCache
    // and should be accessed via a lock but even if it does this test will fail now and
    //hopefully the next time this flag will be set to the correct value.
    if (mClearingCache) {
        if (localLOGV) Slogger::I(TAG, "Thread already running just skip");
        //make sure the thread is not hung for too Int64
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        Int64 current;
        sys->GetCurrentTimeMillis(&current);
        Int64 diffTime = current - mThreadStartTime;
        if (diffTime > (10*60*1000)) {
            Slogger::W(TAG, "Thread that clears cache file seems to run for ever");
        }
    }
    else {
        RestatDataDir();
        if (localLOGV)  Slogger::V(TAG, "freeMemory=%ld", mFreeMem);

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
                    if ((mFreeMemAfterLastCacheClear - mFreeMem)
                            >= ((mMemLowThreshold-mMemCacheStartTrimThreshold)/4)) {
                        // See if clearing cache helps
                        // Note that clearing cache is asynchronous and so we do a
                        // memory check again once the cache has been cleared.
                        AutoPtr<ISystem> sys;
                        CSystem::AcquireSingleton((ISystem**)&sys);
                        sys->GetCurrentTimeMillis(&mThreadStartTime);
                        mClearSucceeded = FALSE;
                        ClearCache();
                    }
                }
            }
            else {
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
                else {
                    if (localLOGV) Slogger::V(TAG, "Running low on memory notification already sent. do nothing");
                }
            }
        }
        else {
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
        }
        else {
            if (mMemFullFlag) {
                CancelFullNotification();
                mMemFullFlag = FALSE;
            }
        }
    }
    if (localLOGV) Slogger::I(TAG, "Posting Message again");
    //keep posting messages to itself periodically
    PostCheckMemoryMsg(TRUE, DEFAULT_CHECK_INTERVAL);
}

void CDeviceStorageMonitorService::PostCheckMemoryMsg(
    /* [in] */ Boolean clearCache,
    /* [in] */ Int64 delay)
{
    // Remove queued messages
    mHandler->RemoveMessages(DEVICE_MEMORY_WHAT);
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(DEVICE_MEMORY_WHAT, clearCache ?_TRUE : _FALSE, 0, (IMessage**)&message);
    Boolean res;
    mHandler->SendMessageDelayed(message, delay, &res);
}

Boolean CDeviceStorageMonitorService::IsBootImageOnDisk()
{
    Slogger::D(TAG, "CDeviceStorageMonitorService::IsBootImageOnDisk is TODO");
    // AutoPtr< ArrayOf<String> > array;
    // CPackageManagerService::GetAllDexCodeInstructionSets((ArrayOf<String>**)&array);
    // for (Int32 i = 0; i < array->GetLength(); i++) {
    //     String instructionSet = (*array)[i];
    //     assert(0 && "TODO");
    //     Boolean res;
    //     // VMRuntime::IsBootClassPathOnDisk(instructionSet);
    //     if (!res) {
    //         return FALSE;
    //     }
    // }
    return TRUE;
}

ECode CDeviceStorageMonitorService::OnStart()
{
    // cache storage thresholds
    AutoPtr<IStorageManagerHelper> helper;
    CStorageManagerHelper::AcquireSingleton((IStorageManagerHelper**)&helper);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IStorageManager> sm;
    helper->From(context, (IStorageManager**)&sm);
    sm->GetStorageLowBytes(DATA_PATH, &mMemLowThreshold);
    sm->GetStorageFullBytes(DATA_PATH, &mMemFullThreshold);

    mMemCacheStartTrimThreshold = ((mMemLowThreshold * 3) + mMemFullThreshold) / 4;
    mMemCacheTrimToThreshold = mMemLowThreshold
            + ((mMemLowThreshold-mMemCacheStartTrimThreshold) * 2);
    mFreeMemAfterLastCacheClear = mTotalMemory;
    CheckMemory(TRUE);

    mCacheFileDeletedObserver = new CacheFileDeletedObserver();
    mCacheFileDeletedObserver->constructor();
    mCacheFileDeletedObserver->StartWatching();

    Slogger::D(TAG, "mRemoteService is TODO");
    // PublishBinderService(SERVICE, mRemoteService);
    PublishLocalService(EIID_IDeviceStorageMonitorInternal, mLocalService);
    return NOERROR;
}

void CDeviceStorageMonitorService::DumpImpl(
    /* [in] */ IPrintWriter* pw)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    pw->Println(String("Current DeviceStorageMonitor state:"));

    pw->Print(String("  mFreeMem="));
    pw->Print(Formatter::FormatFileSize(context, mFreeMem));
    pw->Print(String(" mTotalMemory="));
    pw->Println(Formatter::FormatFileSize(context, mTotalMemory));

    pw->Print(String("  mFreeMemAfterLastCacheClear="));
    pw->Println(Formatter::FormatFileSize(context, mFreeMemAfterLastCacheClear));

    pw->Print(String("  mLastReportedFreeMem="));
    pw->Print(Formatter::FormatFileSize(context, mLastReportedFreeMem));
    pw->Print(String(" mLastReportedFreeMemTime="));
    TimeUtils::FormatDuration(mLastReportedFreeMemTime, SystemClock::GetElapsedRealtime(), pw);
    pw->Println();

    pw->Print(String("  mLowMemFlag="));
    pw->Print(mLowMemFlag);
    pw->Print(String(" mMemFullFlag="));
    pw->Println(mMemFullFlag);
    pw->Print(String(" mIsBootImageOnDisk="));
    pw->Print(mIsBootImageOnDisk);

    pw->Print(String("  mClearSucceeded="));
    pw->Print(mClearSucceeded);
    pw->Print(String(" mClearingCache="));
    pw->Println(mClearingCache);

    pw->Print(String("  mMemLowThreshold="));
    pw->Print(Formatter::FormatFileSize(context, mMemLowThreshold));
    pw->Print(String(" mMemFullThreshold="));
    pw->Println(Formatter::FormatFileSize(context, mMemFullThreshold));

    pw->Print(String("  mMemCacheStartTrimThreshold="));
    pw->Print(Formatter::FormatFileSize(context, mMemCacheStartTrimThreshold));
    pw->Print(String(" mMemCacheTrimToThreshold="));
    pw->Println(Formatter::FormatFileSize(context, mMemCacheTrimToThreshold));
}

void CDeviceStorageMonitorService::SendNotification()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    if (localLOGV) Slogger::I(TAG, "Sending low memory notification");
    //log the event to event log with the amount of free Storage(in bytes) left on the device
    Slogger::E(TAG, "EventLog is TODO");
    // EventLog->WriteEvent(EventLogTags.LOW_STORAGE, mFreeMem);

    //  Pack up the values and broadcast them to everyone
    AutoPtr<IEnvironment> environment;
    CEnvironment::AcquireSingleton((IEnvironment**)&environment);
    Boolean res;
    environment->IsExternalStorageEmulated(&res);
    AutoPtr<IIntent> lowMemIntent;
    CIntent::New(res ? ISettings::ACTION_INTERNAL_STORAGE_SETTINGS
            : IIntent::ACTION_MANAGE_PACKAGE_STORAGE, (IIntent**)&lowMemIntent);
    lowMemIntent->PutExtra(String("memory"), mFreeMem);
    lowMemIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
    INotificationManager* mNotificationMgr = INotificationManager::Probe(obj);
    AutoPtr<ICharSequence> title;
    context->GetText(R::string::low_internal_storage_view_title, (ICharSequence**)&title);
    AutoPtr<ICharSequence> details;
    context->GetText(mIsBootImageOnDisk
            ? R::string::low_internal_storage_view_text
            : R::string::low_internal_storage_view_text_no_boot, (ICharSequence**)&details);
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> intent;
    helper->GetActivityAsUser(context, 0,  lowMemIntent, 0,
            NULL, UserHandle::CURRENT, (IPendingIntent**)&intent);

    AutoPtr<INotificationBuilder> builder;
    CNotificationBuilder::New(context, (INotificationBuilder**)&builder);
    builder->SetSmallIcon(R::drawable::stat_notify_disk_full);
    builder->SetTicker(title);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Int32 color;
    resources->GetColor(R::color::system_notification_accent_color, &color);
    builder->SetColor(color);
    builder->SetContentTitle(title);
    builder->SetContentText(details);
    builder->SetContentIntent(intent);
    AutoPtr<INotificationBigTextStyle> bs;
    CNotificationBigTextStyle::New((INotificationBigTextStyle**)&bs);
    bs->BigText(details);
    builder->SetStyle(INotificationStyle::Probe(bs));
    builder->SetVisibility(INotification::VISIBILITY_PUBLIC);
    builder->SetCategory(INotification::CATEGORY_SYSTEM);
    AutoPtr<INotification> notification;
    builder->Build((INotification**)&notification);

    Int32 flags;
    notification->GetFlags(&flags);
    flags |= INotification::FLAG_NO_CLEAR;
    notification->SetFlags(flags);
    mNotificationMgr->NotifyAsUser(String(NULL), LOW_MEMORY_NOTIFICATION_ID, notification, UserHandle::ALL);
    context->SendStickyBroadcastAsUser(mStorageLowIntent, UserHandle::ALL);
}

void CDeviceStorageMonitorService::CancelNotification()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    if (localLOGV) Slogger::I(TAG, "Canceling low memory notification");
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
    INotificationManager* mNotificationMgr = INotificationManager::Probe(obj);
    //cancel notification since memory has been freed
    mNotificationMgr->CancelAsUser(String(NULL), LOW_MEMORY_NOTIFICATION_ID, UserHandle::ALL);

    context->RemoveStickyBroadcastAsUser(mStorageLowIntent, UserHandle::ALL);
    context->SendBroadcastAsUser(mStorageOkIntent, UserHandle::ALL);
}

void CDeviceStorageMonitorService::SendFullNotification()
{
    if (localLOGV) Slogger::I(TAG, "Sending memory full notification");
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->SendStickyBroadcastAsUser(mStorageFullIntent, UserHandle::ALL);
}

void CDeviceStorageMonitorService::CancelFullNotification()
{
    if (localLOGV) Slogger::I(TAG, "Canceling memory full notification");
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->RemoveStickyBroadcastAsUser(mStorageFullIntent, UserHandle::ALL);
    context->SendBroadcastAsUser(mStorageNotFullIntent, UserHandle::ALL);
}

}//namespace Storage
}//namespace Server
}//namespace Droid
}//namespace Elastos
