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

#ifndef __ELASTOS_DROID_SERVER_STORAGE_CDEVICESTORAGEMONITORSERVICE_H__
#define __ELASTOS_DROID_SERVER_STORAGE_CDEVICESTORAGEMONITORSERVICE_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Server_Storage_CDeviceStorageMonitorService.h"
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/os/FileObserver.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Server::SystemService;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageDataObserver;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::FileObserver;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IStatFs;
using Elastos::IO::IFile;
using Elastos::IO::IPrintWriter;
// using Elastos::IO::IFileDescriptor;
// using dalvik.system.VMRuntime;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Storage {

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
CarClass(CDeviceStorageMonitorService)
    , public SystemService
{
public:
    class CachePackageDataObserver
        : public Object
        , public IIPackageDataObserver
        , public IBinder
    {
    public:
        TO_STRING_IMPL("CDeviceStorageMonitorService::CachePackageDataObserver")

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ISystemService* host);

        CARAPI OnRemoveCompleted(
            /* [in] */ const String& packageName,
            /* [in] */ Boolean succeeded);

    private:
        CDeviceStorageMonitorService* mHost;
    };

private:
    class CacheFileDeletedObserver
        : public FileObserver
    {
    public:
        TO_STRING_IMPL("CDeviceStorageMonitorService::CacheFileDeletedObserver")

        CARAPI constructor();

        //@Override
        CARAPI OnEvent(
            /* [in] */ Int32 event,
            /* [in] */ const String& path);
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CDeviceStorageMonitorService::MyHandler")

        CARAPI constructor(
            /* [in] */ CDeviceStorageMonitorService* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CDeviceStorageMonitorService* mHost;
    };

    class MyDeviceStorageMonitorInternal
        : public Object
        , public IDeviceStorageMonitorInternal
    {
    public:
        TO_STRING_IMPL("CDeviceStorageMonitorService::MyDeviceStorageMonitorInternal")

        CAR_INTERFACE_DECL()

        MyDeviceStorageMonitorInternal(
            /* [in] */ CDeviceStorageMonitorService* host);

        //@Override
        CARAPI CheckMemory();

        //@Override
        CARAPI IsMemoryLow(
            /* [out] */ Boolean* result);

        //@Override
        CARAPI GetMemoryLowThreshold(
            /* [out] */ Int64* result);

    private:
        CDeviceStorageMonitorService* mHost;
    };

public:
    TO_STRING_IMPL("CDeviceStorageMonitorService")

    CAR_OBJECT_DECL()

    CDeviceStorageMonitorService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
    * Initializes the disk space threshold value and posts an empty message to
    * kickstart the process.
    */
    //@Override
    CARAPI OnStart();

protected:
    CARAPI_(void) CheckMemory(
        /* [in] */ Boolean checkCache);

    CARAPI_(void) PostCheckMemoryMsg(
        /* [in] */ Boolean clearCache,
        /* [in] */ Int64 delay);

    CARAPI_(void) DumpImpl(
        /* [in] */ IPrintWriter* pw);

private:
    CARAPI_(void) RestatDataDir();

    CARAPI_(void) ClearCache();

    static CARAPI_(Boolean) IsBootImageOnDisk();


    /**
    * This method sends a notification to NotificationManager to display
    * an error dialog indicating low disk space and launch the Installer
    * application
    */
    CARAPI_(void) SendNotification();

    /**
     * Cancels low storage notification and sends OK intent.
     */
    CARAPI_(void) CancelNotification();

    /**
     * Send a notification when storage is full.
     */
    CARAPI_(void) SendFullNotification();

    /**
     * Cancels memory full notification and sends "not full" intent.
     */
    CARAPI_(void) CancelFullNotification();

protected:
    static const String TAG;

    static const Boolean DEBUG;
    static const Boolean localLOGV;

    static const Int32 DEVICE_MEMORY_WHAT;

    Boolean mLowMemFlag;
    Boolean mClearSucceeded;
    Boolean mClearingCache;

    // This is the raw threshold that has been set at which we consider
    // storage to be low.
    Int64 mMemLowThreshold;

    /**
     * This string is used for ServiceManager access to this class.
     */
    static const String SERVICE;

private:
    static const Int32 MONITOR_INTERVAL; //in minutes
    static const Int32 LOW_MEMORY_NOTIFICATION_ID;

    static const Int32 DEFAULT_FREE_STORAGE_LOG_INTERVAL_IN_MINUTES; //in minutes
    static const Int64 DEFAULT_DISK_FREE_CHANGE_REPORTING_THRESHOLD; // 2MB
    static const Int64 DEFAULT_CHECK_INTERVAL;

    Int64 mFreeMem;  // on /data
    Int64 mFreeMemAfterLastCacheClear;  // on /data
    Int64 mLastReportedFreeMem;
    Int64 mLastReportedFreeMemTime;
    Boolean mMemFullFlag;
    Boolean mIsBootImageOnDisk;
    AutoPtr<IContentResolver> mResolver;
    Int64 mTotalMemory;  // on /data
    AutoPtr<IStatFs> mDataFileStats;
    AutoPtr<IStatFs> mSystemFileStats;
    AutoPtr<IStatFs> mCacheFileStats;

    static const AutoPtr<IFile> DATA_PATH;
    static const AutoPtr<IFile> SYSTEM_PATH;
    static const AutoPtr<IFile> CACHE_PATH;

    Int64 mThreadStartTime;
    AutoPtr<IIntent> mStorageLowIntent;
    AutoPtr<IIntent> mStorageOkIntent;
    AutoPtr<IIntent> mStorageFullIntent;
    AutoPtr<IIntent> mStorageNotFullIntent;
    AutoPtr<IIPackageDataObserver> mClearCacheObserver;
    AutoPtr<CacheFileDeletedObserver> mCacheFileDeletedObserver;
    static const Int32 _TRUE;
    static const Int32 _FALSE;
    // This is the threshold at which we start trying to flush caches
    // to get below the low threshold limit.  It is less than the low
    // threshold; we will allow storage to get a bit beyond the limit
    // before flushing and checking if we are actually low.
    Int64 mMemCacheStartTrimThreshold;
    // This is the threshold that we try to get to when deleting cache
    // files.  This is greater than the low threshold so that we will flush
    // more files than absolutely needed, to reduce the frequency that
    // flushing takes place.
    Int64 mMemCacheTrimToThreshold;
    Int64 mMemFullThreshold;

    /**
    * Handler that checks the amount of disk space on the device and sends a
    * notification if the device runs low on disk space
    */
    AutoPtr<MyHandler> mHandler;

    AutoPtr<IDeviceStorageMonitorInternal> mLocalService;

    // TODO:
    // IBinder mRemoteService = new Binder() {
    //     //@Override
    //     protected void Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
    //         if (GetContext()->CheckCallingOrSelfPermission(Manifest::permission::DUMP)
    //                 != PackageManager.PERMISSION_GRANTED) {

    //             pw->Println("Permission Denial: can't dump " + SERVICE + " from from pid="
    //                     + Binder->GetCallingPid()
    //                     + ", uid=" + Binder->GetCallingUid());
    //             return;
    //         }

    //         mHost->DumpImpl(pw);
    //     }
    // };
};

}//namespace Storage
}//namespace Server
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_SERVER_STORAGE_CDEVICESTORAGEMONITORSERVICE_H__