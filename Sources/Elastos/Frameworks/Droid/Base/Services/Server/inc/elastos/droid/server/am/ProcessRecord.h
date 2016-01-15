
#ifndef __ELASTOS_DROID_SERVER_AM_PROCESSRECORD_H__
#define __ELASTOS_DROID_SERVER_AM_PROCESSRECORD_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/server/am/ReceiverList.h"
#include "elastos/droid/server/am/ConnectionRecord.h"
#include "elastos/droid/server/am/ContentProviderRecord.h"
#include "elastos/droid/server/am/CServiceRecord.h"
#include "elastos/droid/server/am/ActivityRecord.h"
#include "elastos/droid/server/am/BroadcastRecord.h"

using Elastos::Droid::App::IApplicationThread;
using Elastos::Droid::App::IActivityManagerProcessErrorStateInfo;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IInstrumentationWatcher;
using Elastos::Droid::App::IIUiAutomationConnection;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Internal::App::IProcessState;
using Elastos::Droid::Internal::App::IProcessStateHolder;
using Elastos::Droid::Internal::Os::IBatteryStatsImpl;
using Elastos::Droid::Os::IBatteryStatsUidProc;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Core::IRunnable;
using Elastos::Core::StringBuilder;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class CProcessStatsService;

class ProcessRecord
    : public Object
    , public IProcessRecord
{
public:
    ProcessRecord(
        /* [in] */ IBatteryStatsImpl* batteryStats,
        /* [in] */ IApplicationInfo* info,
        /* [in] */ const String& processName,
        /* [in] */ Int32 uid);

    ~ProcessRecord();

    CAR_INTERFACE_DECL()

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI SetPid(
        /* [in] */ Int32 pid);

    CARAPI_(void) MakeActive(
        /* [in] */ IApplicationThread* thread,
        /* [in] */ CProcessStatsService* tracker);

    CARAPI_(void) MakeInactive(
        /* [in] */ CProcessStatsService* tracker);

    /**
     * This method returns true if any of the activities within the process record are interesting
     * to the user. See HistoryRecord.isInterestingToUserLocked()
     */
    CARAPI_(Boolean) IsInterestingToUserLocked();

    CARAPI StopFreezingAllLocked();

    CARAPI UnlinkDeathRecipient();

    CARAPI UpdateHasAboveClientLocked();

    CARAPI_(Int32) ModifyRawOomAdj(
        /* [in] */ Int32 adj);

    CARAPI_(void) Kill(
        /* [in] */ const String& reason,
        /* [in] */ Boolean noisy);

    CARAPI_(String) ToShortString();

    CARAPI ToShortString(
        /* [in] */ StringBuilder& sb);

    CARAPI_(String) ToString();

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI_(String) MakeAdjReason();

    /*
     *  Return true if package has been added false if not
     */
    CARAPI_(Boolean) AddPackage(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 versionCode,
        /* [in] */ CProcessStatsService* tracker);

    CARAPI_(Int32) GetSetAdjWithServices();

    CARAPI_(void) ForceProcessStateUpTo(
        /* [in] */ Int32 newState);

    /*
     *  Delete all packages from list except the package indicated in info
     */
    CARAPI_(void) ResetPackageList(
        /* [in] */ CProcessStatsService* tracker);

    CARAPI_(AutoPtr< ArrayOf<String> >) GetPackageList();

public:
    AutoPtr<IBatteryStatsImpl> mBatteryStats; // where to collect runtime statistics
    AutoPtr<IApplicationInfo> mInfo; // all about the first app in the process
    Boolean mIsolated;      // true if this is a special isolated process
    Int32 mUid;             // uid of process; may be different from 'info' if isolated
    Int32 mUserId;          // user of process.
    String mProcessName;    // name of the process
    // List of packages running in the process
    AutoPtr<IArrayMap> mPkgList;
    AutoPtr<HashSet<String> > mPkgDeps;   // additional packages we have a dependency on
    AutoPtr<IApplicationThread> mThread; // the actual proc...  may be null only if
                                // 'persistent' is true (in which case we
                                // are in the process of launching the app)
    AutoPtr<IProcessState> mBaseProcessTracker;
    AutoPtr<IBatteryStatsUidProc> mCurProcBatteryStats;
    Int32 mPid;
    Boolean mStarting;            // True if the process is being started
    Int64 mLastActivityTime;      // For managing the LRU list
    Int64 mLastPssTime;           // Last time we retrieved PSS data
    Int64 mNextPssTime;           // Next time we want to request PSS data
    Int64 mLastStateTime;         // Last time setProcState changed
    Int64 mInitialIdlePss;        // Initial memory pss of process for idle maintenance.
    Int64 mLastPss;               // Last computed memory pss.
    Int64 mLastCachedPss;         // Last computed pss when in cached state.
    Int32 mMaxAdj;                  // Maximum OOM adjustment for this process
    Int32 mCurRawAdj;               // Current OOM unlimited adjustment for this process
    Int32 mSetRawAdj;               // Last set OOM unlimited adjustment for this process
    Int32 mCurAdj;                  // Current OOM adjustment for this process
    Int32 mSetAdj;                  // Last set OOM adjustment for this process
    Int32 mCurSchedGroup;           // Currently desired scheduling class
    Int32 mSetSchedGroup;           // Last set to background scheduling class
    Int32 mTrimMemoryLevel;        // Last selected memory trimming level
    Int32 mCurProcState;        // Currently computed process state: ActivityManager.PROCESS_STATE_*
    Int32 mRepProcState;        // Last reported process state
    Int32 mSetProcState;        // Last set process state in process tracker
    Int32 mPssProcState;        // The proc state we are currently requesting pss for
    Boolean mServiceb;           // Process currently is on the service B list
    Boolean mServiceHighRam;     // We are forcing to service B list due to its RAM use
    Boolean mSetIsForeground;       // Running foreground UI when last set?
    Boolean mNotCachedSinceIdle; // Has this process not been in a cached state since last idle?
    Boolean mHasClientActivities;  // Are there any client services with activities?
    Boolean mHasStartedServices; // Are there any started services running in this process?
    Boolean mForegroundServices;    // Running any services that are foreground?
    Boolean mForegroundActivities; // Running any activities that are foreground?
    Boolean mRepForegroundActivities; // Last reported foreground activities.
    Boolean mSystemNoUi;         // This is a system process, but not currently showing UI.
    Boolean mHasShownUi;         // Has UI been shown in this process since it was started?
    Boolean mPendingUiClean;     // Want to clean up resources from showing UI?
    Boolean mHasAboveClient;     // Bound using BIND_ABOVE_CLIENT, so want to be lower
    Boolean mTreatLikeActivity;  // Bound using BIND_TREAT_LIKE_ACTIVITY
    Boolean mBad;                   // True if disabled in the bad process list
    Boolean mKilledByAm;         // True when proc has been killed by activity manager, not for RAM
    Boolean mKilled;             // True once we know the process has been killed
    Boolean mProcStateChanged;   // Keep track of whether we changed 'setAdj'.
    String mWaitingToKill;       // Process is waiting to be killed when in the bg, and reason
    AutoPtr<IBinder> mForcingToForeground; // Token that is forcing this process to be foreground
    Int32 mAdjSeq;                 // Sequence id for identifying oom_adj assignment cycles
    Int32 mLruSeq;                  // Sequence id for identifying LRU update cycles
    AutoPtr<ICompatibilityInfo> mCompat;   // last used compatibility mode
    AutoPtr<IProxyDeathRecipient> mDeathRecipient; // Who is watching for the death.
    AutoPtr<IComponentName> mInstrumentationClass;// class installed to instrument app
    AutoPtr<IApplicationInfo> mInstrumentationInfo; // the application being instrumented
    String mInstrumentationProfileFile; // where to save profiling
    AutoPtr<IInstrumentationWatcher> mInstrumentationWatcher; // who is waiting
    AutoPtr<IIUiAutomationConnection> mInstrumentationUiAutomationConnection; // Connection to use the UI introspection APIs.
    AutoPtr<IBundle> mInstrumentationArguments; // as given to us
    AutoPtr<IComponentName> mInstrumentationResultClass;// copy of instrumentationClass
    Boolean mUsingWrapper;       // Set to true when process was launched with a wrapper attached
    AutoPtr<BroadcastRecord> mCurReceiver; // receiver currently running in the app
    Int64 mLastWakeTime;          // How long proc held wake lock at last check
    Int64 mLastCpuTime;           // How long proc has run CPU at last check
    Int64 mCurCpuTime;            // How long proc has run CPU most recently
    Int64 mLastRequestedGc; // When we last asked the app to do a gc
    Int64 mLastLowMemory;   // When we last told the app that memory is low
    Boolean mReportLowMemory;    // Set to true when waiting to report low mem
    Boolean mEmpty;              // Is this an empty background process?
    Boolean mCached;             // Is this a cached process?
    String mAdjType;             // Debugging: primary thing impacting oom_adj.
    Int32 mAdjTypeCode;            // Debugging: adj code to report to app.
    AutoPtr<IInterface> mAdjSource;     // Debugging: option dependent object.
    Int32 mAdjSourceProcState;          // Debugging: proc state of adjSource's process.
    AutoPtr<IInterface> mAdjTarget;     // Debugging: target component impacting oom_adj.
    AutoPtr<IRunnable> mCrashHandler;   // Optional local handler to be invoked in the process crash.

    // contains HistoryRecord objects
    List< AutoPtr<ActivityRecord> > mActivities;
    // all ServiceRecord running in this process
    HashSet< AutoPtr<CServiceRecord> > mServices;
    // services that are currently executing code (need to remain foreground).
    HashSet< AutoPtr<CServiceRecord> > mExecutingServices;
    // All ConnectionRecord this process holds
    HashSet< AutoPtr<ConnectionRecord> > mConnections;
    // all IIntentReceivers that are registered from this process.
    HashSet< AutoPtr<ReceiverList> > mReceivers;
    // class (String) -> ContentProviderRecord
    HashMap<String, AutoPtr<ContentProviderRecord> > mPubProviders;
    // All ContentProviderRecord process is using
    List< AutoPtr<CContentProviderConnection> > mConProviders;

    Boolean mExecServicesFg;     // do we need to be executing services in the foreground?
    Boolean mPersistent;         // always keep this application running?
    Boolean mCrashing;           // are we in the process of crashing?
    AutoPtr<IDialog> mCrashDialog;         // dialog being displayed due to crash.
    Boolean mForceCrashReport;   // suppress normal auto-dismiss of crash dialog & report UI?
    Boolean mNotResponding;      // does the app have a not responding dialog?
    AutoPtr<IDialog> mAnrDialog;           // dialog being displayed due to app not resp.
    Boolean mRemoved;            // has app package been removed from device?
    Boolean mDebugging;          // was app launched for debugging?
    Boolean mWaitedForDebugger;  // has process show wait for debugger dialog?
    AutoPtr<IDialog> mWaitDialog;          // current wait for debugger dialog

    String mShortStringName;     // caching of toShortString() result.
    String mStringName;          // caching of toString() result.

    // These reports are generated & stored when an app gets into an error condition.
    // They will be "null" when all is OK.
    AutoPtr<IActivityManagerProcessErrorStateInfo> mCrashingReport;
    AutoPtr<IActivityManagerProcessErrorStateInfo> mNotRespondingReport;

    // Who will be notified of the error. This is usually an activity in the
    // app that installed the package.
    AutoPtr<IComponentName> mErrorReportReceiver;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Am::ProcessRecord)

#endif //__ELASTOS_DROID_SERVER_AM_PROCESSRECORD_H__
