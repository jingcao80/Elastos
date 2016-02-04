#ifndef __MONKEY_H__
#define __MONKEY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Commands.Monkey.h"
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>

using Elastos::Utility::Etl::HashSet;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Utility::Etl::List;
using Elastos::Droid::App::IIActivityController;
using Elastos::Core::IRandom;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class Monkey : public ElLightRefBase
{
public:
    Monkey();

    virtual ~Monkey();

    static void Main(
        /* [in] */ const ArrayOf<String>& args);

    CARAPI_(Int32) GetVerbose();

    Boolean CheckEnteringPackage(
        /* [in] */ const String& pkg);

private:
    static Boolean LoadPackageListFromFile(
        /* [in] */ const String& fileName,
        /* [in] */ HashSet<String> *list);

    void ReportProcRank();

    void ReportAnrTraces();

    void ReportDumpsysMemInfo();

    void CommandLineReport(
        /* [in] */ const String& reportName,
        /* [in] */ const String& command);

    void WriteScriptLog(
        /* [in] */ Int32 count);

    void GetBugreport(
        /* [in] */ const String& reportName);

    Int32 Run(
        /* [in] */ ArrayOf<String> *args);

    Boolean ProcessOptions();

    Boolean LoadPackageLists();

    Boolean CheckInternalConfiguration();

    Boolean GetSystemInterfaces();

    Boolean GetMainApps();

    Int32 RunMonkeyCycles();

    void SignalPersistentProcesses();

    Boolean CheckNativeCrashes();

    String NextOption();

    String NextOptionData();

    Int64 NextOptionLong(
        /* [in] */ const char *opt);

    String NextArg();

    void ShowUsage();

public:
    // information on the current activity.
    static AutoPtr<IIntent> currentIntent;

    static String currentPackage;

    static AutoPtr<IFile> TOMBSTONES_PATH;

    /** The delay between event inputs **/
    Int64 mThrottle;

    /** Whether to randomize each throttle (0-mThrottle ms) inserted between events. */
    Boolean mRandomizeThrottle;

    /** The number of iterations **/
    Int32 mCount;

    /** The random number seed **/
    Int64 mSeed;

    /** The random number generator **/
    AutoPtr<IRandom> mRandom;

    /** Dropped-event statistics **/
    Int64 mDroppedKeyEvents;

    Int64 mDroppedPointerEvents;

    Int64 mDroppedTrackballEvents;

    Int64 mDroppedFlipEvents;

    Int64 mDroppedRotationEvents;

    /** The delay between user actions. This is for the scripted monkey. **/
    Int64 mProfileWaitTime;

    /** Device idle time. This is for the scripted monkey. **/
    Int64 mDeviceSleepTime;

    Boolean mRandomizeScript;

    Boolean mScriptLog;

    AutoPtr<ArrayOf<Float> > mFactors;

    AutoPtr<IMonkeyEventSource> mEventSource;

    Object mLockLock;

    AutoPtr<IIActivityManager> mAm;

    AutoPtr<IIWindowManager> mWm;

    AutoPtr<IIPackageManager> mPm;

    /** Command line arguments */
    AutoPtr<ArrayOf<String> > mArgs;

    /** Current argument being parsed */
    Int32 mNextArg;

    /** Data of current argument */
    String mCurArgData;

    /** Running in verbose output mode? 1= verbose, 2=very verbose */
    Int32 mVerbose;

    /** Ignore any application crashes while running? */
    Boolean mIgnoreCrashes;

    /** Ignore any not responding timeouts while running? */
    Boolean mIgnoreTimeouts;

    /** Ignore security exceptions when launching activities */
    /** (The activity launch still fails, but we keep pluggin' away) */
    Boolean mIgnoreSecurityExceptions;

    /** Monitor /data/tombstones and stop the monkey if new files appear. */
    Boolean mMonitorNativeCrashes;

    /** Ignore any native crashes while running? */
    Boolean mIgnoreNativeCrashes;

    /** Send no events. Use with long throttle-time to watch user operations */
    Boolean mSendNoEvents;

    /** This is set when we would like to abort the running of the monkey. */
    Boolean mAbort;

    /**
     * Count each event as a cycle. Set to false for scripts so that each time
     * through the script increments the count.
     */
    Boolean mCountEvents;

    /**
     * This is set by the ActivityController thread to request collection of ANR
     * trace files
     */
    Boolean mRequestAnrTraces;

    /**
     * This is set by the ActivityController thread to request a
     * "dumpsys meminfo"
     */
    Boolean mRequestDumpsysMemInfo;

    /**
     * This is set by the ActivityController thread to request a
     * bugreport after ANR
     */
    Boolean mRequestAnrBugreport;

    /**
     * This is set by the ActivityController thread to request a
     * bugreport after java application crash
     */
    Boolean mRequestAppCrashBugreport;

    /**Request the bugreport based on the mBugreportFrequency. */
    Boolean mGetPeriodicBugreport;

    /**
     * Request the bugreport based on the mBugreportFrequency.
     */
    Boolean mRequestPeriodicBugreport;

    /** Bugreport frequency. */
    Int64 mBugreportFrequency;

    /** Failure process name */
    String mReportProcessName;

    /**
     * This is set by the ActivityController thread to request a "procrank"
     */
    Boolean mRequestProcRank;

    /** Kill the process after a timeout or crash. */
    Boolean mKillProcessAfterError;

    /** Generate hprof reports before/after monkey runs */
    Boolean mGenerateHprof;

    /** Package blacklist file. */
    String mPkgBlacklistFile;

    /** Package whitelist file. */
    String mPkgWhitelistFile;

    /** Packages we are allowed to run, or empty if no restriction. */
    AutoPtr< HashSet<String> > mValidPackages;

    /** Packages we are not allowed to run. */
    AutoPtr< HashSet<String> > mInvalidPackages;

    /** Categories we are allowed to launch **/
    AutoPtr<List<String> > mMainCategories;

    /** Applications we can switch to. */
    AutoPtr<List<AutoPtr<IComponentName> > > mMainApps;

    /** Capture bugreprot whenever there is a crash. **/
    Boolean mRequestBugreport;

    /** a filename to the setup script (if any) */
    String mSetupFileName;

    /** filenames of the script (if any) */
    AutoPtr<ArrayOf<String> > mScriptFileNames;

    /** a TCP port to listen on for remote commands. */
    Int32 mServerPort;

    AutoPtr<HashSet<String> > mTombstones;

    AutoPtr<IMonkeyNetworkMonitor> mNetworkMonitor;

private:
    Object mLock;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __MONKEY_H__
