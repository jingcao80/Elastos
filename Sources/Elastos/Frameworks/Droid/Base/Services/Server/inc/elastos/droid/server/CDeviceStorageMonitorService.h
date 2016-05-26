#ifndef __ELASTOS_DROID_SERVER_CDEVICESTORAGEMONITORSERVICE_H__
#define __ELASTOS_DROID_SERVER_CDEVICESTORAGEMONITORSERVICE_H__

#include "_Elastos_Droid_Server_CDeviceStorageMonitorService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/FileObserver.h"
#include "elastos/droid/os/HandlerBase.h"
#include "Elastos.Core.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageDataObserver;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::FileObserver;
using Libcore::IO::IStructStatFs;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CDeviceStorageMonitorService)
{
private:
    class MyHandler : public HandlerBase
    {
    public:
        TO_STRING_IMPL("CDeviceStorageMonitorService::MyHandler")

        MyHandler(
            /* [in] */ CDeviceStorageMonitorService* service)
            : mHost(service)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        CDeviceStorageMonitorService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDeviceStorageMonitorService();

    CARAPI ToString(
        /* [out] */ String* res);

    /**
    * Constructor to run service. initializes the disk space threshold value
    * and posts an empty message to kickstart the process.
    */
    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI UpdateMemory();

    /**
     * Callable from other things in the system service to obtain the low memory
     * threshold.
     *
     * @return low memory threshold in bytes
     */
    CARAPI GetMemoryLowThreshold(
        /* [out] */ Int64* threshold);

    /**
     * Callable from other things in the system process to check whether memory
     * is low.
     *
     * @return true is memory is low
     */
    CARAPI IsMemoryLow(
        /* [out] */ Boolean* threshold);

private:
    CARAPI_(void) RestatDataDir();

    CARAPI_(void) ClearCache();

    CARAPI_(void) CheckMemory(
        /* [in] */ Boolean checkCache);

    CARAPI_(void) PostCheckMemoryMsg(
        /* [in] */ Boolean clearCache,
        /* [in] */ Int64 delay,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1);

    /*
     * just query settings to retrieve the memory threshold.
     * Preferred this over using a ContentObserver since Settings.Secure caches the value
     * any way
     */
    CARAPI_(Int64) GetMemThreshold();

    /*
     * just query settings to retrieve the memory full threshold.
     * Preferred this over using a ContentObserver since Settings.Secure caches the value
     * any way
     */
    CARAPI_(Int32) GetMemFullThreshold();

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

public:
    class CachePackageDataObserver
        : public ElRefBase
        , public IPackageDataObserver
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CachePackageDataObserver(
            /* [in] */ CDeviceStorageMonitorService* host);

        CARAPI OnRemoveCompleted(
            /* [in] */ const String& packageName,
            /* [in] */ Boolean succeeded);

        CARAPI GetHashCode(
            /* [out] */ Int32* hash);

        CARAPI ToString(
            /* [out] */ String* res);

    private:
        AutoPtr<CDeviceStorageMonitorService> mHost;
    };

    class CacheFileDeletedObserver
        : public FileObserver
    {
    public:
        CacheFileDeletedObserver();

        //@Override
        CARAPI OnEvent(
            /* [in] */ Int32 event,
            /* [in] */ const String& path);
    };

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean localLOGV;
    static const Int32 DEVICE_MEMORY_WHAT = 1;
    static const Int32 MONITOR_INTERVAL = 1; //in minutes
    static const Int32 LOW_MEMORY_NOTIFICATION_ID = 1;
    static const Int32 DEFAULT_THRESHOLD_PERCENTAGE = 10;
    static const Int32 DEFAULT_THRESHOLD_MAX_BYTES = 500*1024*1024; // 500MB
    static const Int32 DEFAULT_FREE_STORAGE_LOG_INTERVAL_IN_MINUTES = 12*60; //in minutes
    static const Int64 DEFAULT_DISK_FREE_CHANGE_REPORTING_THRESHOLD; // 2MB
    static const Int64 DEFAULT_CHECK_INTERVAL;
    static const Int32 DEFAULT_FULL_THRESHOLD_BYTES = 1024*1024; // 1MB
    Int64 mFreeMem;  // on /data
    Int64 mFreeMemAfterLastCacheClear;  // on /data
    Int64 mLastReportedFreeMem;
    Int64 mLastReportedFreeMemTime;
    Boolean mLowMemFlag;
    Boolean mMemFullFlag;
    AutoPtr<IContext> mContext;
    AutoPtr<IContentResolver> mContentResolver;
    Int64 mTotalMemory;  // on /data
    AutoPtr<IStructStatFs> mDataFileStats;
    AutoPtr<IStructStatFs> mSystemFileStats;
    AutoPtr<IStructStatFs> mCacheFileStats;
    static const String DATA_PATH;
    static const String SYSTEM_PATH;
    static const String CACHE_PATH;
    Int64 mThreadStartTime;
    Boolean mClearSucceeded;
    Boolean mClearingCache;
    AutoPtr<IIntent> mStorageLowIntent;
    AutoPtr<IIntent> mStorageOkIntent;
    AutoPtr<IIntent> mStorageFullIntent;
    AutoPtr<IIntent> mStorageNotFullIntent;
    AutoPtr<CachePackageDataObserver> mClearCacheObserver;
    AutoPtr<CacheFileDeletedObserver> mCacheFileDeletedObserver;
    static const Int32 _TRUE = 1;
    static const Int32 _FALSE = 0;
    // This is the raw threshold that has been set at which we consider
    // storage to be low.
    Int64 mMemLowThreshold;
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
    Int32 mMemFullThreshold;

public:
    /**
     * This string is used for ServiceManager access to this class.
     */
    static const String SERVICE;

    AutoPtr<IHandler> mHandler;
};

}//namespace Server
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CDEVICESTORAGEMONITORSERVICE_H__
