
#ifndef __ELASTOS_DROID_SERVER_AM_CUSAGESTATSSERVICE_H__
#define __ELASTOS_DROID_SERVER_AM_CUSAGESTATSSERVICE_H__

#include "_Elastos_Droid_Server_Am_CUsageStatsService.h"
#include "elastos/droid/content/PackageMonitor.h"
#include <ext/frameworkext.h>
#include <Elastos.Droid.Core.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::IO::IFile;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileInputStream;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::ICalendar;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Internal::Os::IPkgUsageStats;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Internal::Content::IPackageMonitor;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger64;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CarClass(CUsageStatsService)
{
public:
    class TimeStats
        : public Object
    {
    public:
        TimeStats();

        TimeStats(
            /* [in] */ IParcel* in);

        CARAPI_(void) IncCount();

        CARAPI_(void) Add(
            /* [in] */ Int32 val);

        CARAPI WriteToParcel(
            /* [in] */ IParcel* out);

    public:
        Int32 mCount;
        AutoPtr<ArrayOf<Int32> > mTimes;
    };

private:
    class PkgUsageStatsExtended
        : public Object
    {
    public:
        PkgUsageStatsExtended()
            : mLaunchCount(0)
            , mUsageTime(0)
            , mPausedTime(0)
            , mResumedTime(0)
        {}

        PkgUsageStatsExtended(
            /* [in] */ IParcel* in);

        CARAPI_(void) UpdateResume(
            /* [in] */ const String& comp,
            /* [in] */ Boolean launched);

        CARAPI_(void) UpdatePause();

        CARAPI_(void) AddLaunchCount(
            /* [in] */ const String& comp);

        CARAPI_(void) AddLaunchTime(
            /* [in] */ const String& comp,
            /* [in] */ Int32 millis);

        CARAPI_(void) WriteToParcel(
            /* [in] */ IParcel* out);

        CARAPI_(void) Clear();

    public:
        HashMap<String, AutoPtr<TimeStats> > mLaunchTimes;
        //         = new HashMap<String, TimeStats>();
        Int32 mLaunchCount;
        Int64 mUsageTime;
        Int64 mPausedTime;
        Int64 mResumedTime;
    };

    class WriteStatsToFileThread
        : public ThreadBase
    {
    public:
        WriteStatsToFileThread(
            /* [in] */ CUsageStatsService* host);

        CARAPI Run();

    private:
        CUsageStatsService* mHost;
    };

    class LocalPackageMonitor
        : public PackageMonitor
    {
    public:
        LocalPackageMonitor(
            /* [in] */ CUsageStatsService* host)
            : mHost(host)
        {}

        CARAPI OnPackageRemovedAllUsers(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

    private:
        CUsageStatsService* mHost;
    };

public:

    CUsageStatsService()
        : mLastResumedPkg(String(NULL))
        , mLastResumedComp(String(NULL))
        , mIsResumed(FALSE)
        , mFileLeaf(String(NULL))
    {}

    ~CUsageStatsService();

    CARAPI constructor(
        /* [in] */ const String& dir);

    CARAPI Publish(
        /* [in] */ IContext* context);

    CARAPI MonitorPackages();

    CARAPI Shutdown();

    static AutoPtr<IIUsageStats> GetService();

    CARAPI NoteResumeComponent(
        /* [in] */ IComponentName* componentName);

    CARAPI NotePauseComponent(
        /* [in] */ IComponentName* componentName);

    CARAPI NoteLaunchTime(
        /* [in] */ IComponentName* componentName,
        /* [in] */ Int32 millis);

    CARAPI EnforceCallingPermission();

    CARAPI GetPkgUsageStats(
        /* [in] */ IComponentName* componentName,
        /* [out] */ IPkgUsageStats** pkgUsageStats);

    CARAPI GetAllPkgUsageStats(
        /* [out, callee] */ ArrayOf<IPkgUsageStats*>** stats);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) ReadFully(
        /* [in] */ IFileInputStream* stream);

private:
    CARAPI_(String) GetCurrentDateStr(
        /* [in] */ const String& prefix);

    CARAPI_(AutoPtr<IParcel>) GetParcelForFile(
        /* [in] */ IFile* file);

    CARAPI_(void) ReadStatsFromFile();

    CARAPI_(void) ReadStatsFLOCK(
        /* [in] */ IFile* file);

    CARAPI_(void) ReadHistoryStatsFromFile();

    CARAPI ReadHistoryStatsFLOCK(
        /* [in] */ IAtomicFile* file);

    CARAPI_(AutoPtr<ArrayOf<String> >) GetUsageStatsFileListFLOCK();

    CARAPI_(void) CheckFileLimitFLOCK();

    CARAPI_(void) WriteStatsToFile(
        /* [in] */ Boolean force,
        /* [in] */ Boolean forceWriteHistoryStats);

    CARAPI_(void) WriteStatsFLOCK(
        /* [in] */ IFile* file);

    CARAPI_(void) WriteStatsToParcelFLOCK(
        /* [in] */ IParcel* out);

    CARAPI_(void) FilterHistoryStats();

    CARAPI_(void) WriteHistoryStatsFLOCK(
        /* [in] */ IAtomicFile* historyFile);

    CARAPI_(void) CollectDumpInfoFLOCK(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Boolean isCompactOutput,
        /* [in] */ Boolean deleteAfterPrint,
        /* [in] */ ArrayOf<String>* packages);

    CARAPI_(void) CollectDumpInfoFromParcelFLOCK(
        /* [in] */ IParcel* in,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& date,
        /* [in] */ Boolean isCompactOutput,
        /* [in] */ ArrayOf<String>* packages);

    static CARAPI_(Boolean) ScanArgs(
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ const String& value);

    static CARAPI_(String) ScanArgsData(
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ const String& value);

public:
    static const String SERVICE_NAME;
    static const Boolean mLocalLOGV = FALSE;
    static const Boolean REPORT_UNEXPECTED = FALSE;
    static const String TAG;

    // Current on-disk Parcel version
    static const Int32 VERSION = 1007;
    static const Int32 CHECKIN_VERSION = 4;
    static const String FILE_PREFIX;
    static const String FILE_HISTORY;
    static const Int32 FILE_WRITE_INTERVAL = 30*60*1000; //ms
    static const Int32 MAX_NUM_FILES = 5;

    static const Int32 NUM_LAUNCH_TIME_BINS = 10;
    static const AutoPtr< ArrayOf<Int32> > LAUNCH_TIME_BINS;

    static AutoPtr<IIUsageStats> sService;

    AutoPtr<IContext> mContext;

    // Maintains the last time any component was resumed, for all time.
    typedef HashMap<String, Int64> StringInt64Map;
    typedef typename StringInt64Map::Iterator StringInt64MapIterator;
    typedef HashMap<String, AutoPtr<StringInt64Map> > LastResumeTimeMap;
    typedef typename HashMap<String, AutoPtr<StringInt64Map> >::Iterator LastResumeTimeMapIterator;
    LastResumeTimeMap mLastResumeTimes;

    // To remove last-resume time stats when a pacakge is removed.
    AutoPtr<IPackageMonitor> mPackageMonitor;

    // Lock to update package stats. Methods suffixed by SLOCK should invoked with
    // this lock held
    Object mStatsLock;
    // Lock to write to file. Methods suffixed by FLOCK should invoked with
    // this lock held.
    Object mFileLock;

private:
    // structure used to maintain statistics since the last checkin.
    HashMap<String, AutoPtr<PkgUsageStatsExtended> > mStats;

    // Order of locks is mFileLock followed by mStatsLock to avoid deadlocks
    String mLastResumedPkg;
    String mLastResumedComp;
    Boolean mIsResumed;
    AutoPtr<IFile> mFile;
    AutoPtr<IAtomicFile> mHistoryFile;
    String mFileLeaf;
    AutoPtr<IFile> mDir;

    AutoPtr<ICalendar> mCal; // guarded by itself

    Object mCalLock;

    AutoPtr<IAtomicInteger32> mLastWriteDay;
    AutoPtr<IAtomicInteger64> mLastWriteElapsedTime;
    AutoPtr<IAtomicBoolean> mUnforcedDiskWriteRunning;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_CUSAGESTATSSERVICE_H__
