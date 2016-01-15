
#ifndef __ELASTOS_DROID_SERVER_AM_CACTIVITYMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_AM_CACTIVITYMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

#define UN_FINISHED 1

#if UN_FINISHED
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/server/ProcessMap.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/server/am/ActiveServices.h"
//#include "elastos/droid/server/am/CBatteryStatsService.h"
#include "elastos/droid/server/am/CProcessStatsService.h"
#include "elastos/droid/server/am/TaskPersister.h"
#include "elastos/droid/server/am/UserStartedState.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::App::IApplicationThread;
using Elastos::Droid::App::IActivityController;
using Elastos::Droid::App::IProfilerInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Internal::Os::IProcessCpuTracker;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Server::ProcessMap;

using Elastos::Core::IInteger32;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class UriPermission;

//
// NeededUriGrants
//
class NeededUriGrants
    : public List< AutoPtr<IUri> >
{
public:
    NeededUriGrants(
        /* [in] */ const String& targetPkg,
        /* [in] */ Int32 targetUid,
        /* [in] */ Int32 flags);

public:
    String mTargetPkg;
    Int32 mTargetUid;
    Int32 mFlags;
};

class CActivityManagerService : public Object
{
public:
    static const String TAG;
    static const String TAG_MU;
    static const Boolean DEBUG = FALSE;
    static const Boolean localLOGV = DEBUG;
    static const Boolean DEBUG_BACKUP = localLOGV || FALSE;
    static const Boolean DEBUG_BROADCAST = localLOGV || FALSE;
    static const Boolean DEBUG_BROADCAST_LIGHT = DEBUG_BROADCAST || FALSE;
    static const Boolean DEBUG_BACKGROUND_BROADCAST = DEBUG_BROADCAST || FALSE;
    static const Boolean DEBUG_CLEANUP = localLOGV || FALSE;
    static const Boolean DEBUG_CONFIGURATION = localLOGV || FALSE;
    static const Boolean DEBUG_FOCUS = FALSE;
    static const Boolean DEBUG_IMMERSIVE = localLOGV || FALSE;
    static const Boolean DEBUG_MU = localLOGV || FALSE;
    static const Boolean DEBUG_OOM_ADJ = localLOGV || FALSE;
    static const Boolean DEBUG_LRU = localLOGV || FALSE;
    static const Boolean DEBUG_PAUSE = localLOGV || FALSE;
    static const Boolean DEBUG_POWER = localLOGV || FALSE;
    static const Boolean DEBUG_POWER_QUICK = DEBUG_POWER || FALSE;
    static const Boolean DEBUG_PROCESS_OBSERVERS = localLOGV || FALSE;
    static const Boolean DEBUG_PROCESSES = localLOGV || FALSE;
    static const Boolean DEBUG_PROVIDER = localLOGV || FALSE;
    static const Boolean DEBUG_RESULTS = localLOGV || FALSE;
    static const Boolean DEBUG_SERVICE = localLOGV || FALSE;
    static const Boolean DEBUG_SERVICE_EXECUTING = localLOGV || FALSE;
    static const Boolean DEBUG_STACK = localLOGV || FALSE;
    static const Boolean DEBUG_SWITCH = localLOGV || FALSE;
    static const Boolean DEBUG_TASKS = localLOGV || FALSE;
    static const Boolean DEBUG_THUMBNAILS = localLOGV || FALSE;
    static const Boolean DEBUG_TRANSITION = localLOGV || FALSE;
    static const Boolean DEBUG_URI_PERMISSION = localLOGV || FALSE;
    static const Boolean DEBUG_USER_LEAVING = localLOGV || FALSE;
    static const Boolean DEBUG_VISBILITY = localLOGV || FALSE;
    static const Boolean DEBUG_PSS = localLOGV || FALSE;
    static const Boolean DEBUG_LOCKSCREEN = localLOGV || FALSE;
    static const Boolean DEBUG_RECENTS = localLOGV || FALSE;
    static const Boolean VALIDATE_TOKENS = FALSE;
    static const Boolean SHOW_ACTIVITY_START_TIME = TRUE;

    // Control over CPU and battery monitoring.
    static const Int64 BATTERY_STATS_TIME;      // write battery stats every 30 minutes.
    static const Boolean MONITOR_CPU_USAGE;
    static const Int64 MONITOR_CPU_MIN_TIME;        // don't sample cpu less than every 5 seconds.
    static const Int64 MONITOR_CPU_MAX_TIME;    // wait possibly forever for next cpu sample.
    static const Boolean MONITOR_THREAD_CPU_USAGE;

    // The flags that are set for all calls we make to the package manager.
    static const Int32 STOCK_PM_FLAGS;

    static const Boolean IS_USER_BUILD;

    // Maximum number of recent tasks that we can remember.
    static const Int32 MAX_RECENT_TASKS = 20;

    // Amount of time after a call to stopAppSwitches() during which we will
    // prevent further untrusted switches from happening.
    static const Int64 APP_SWITCH_DELAY_TIME = 5 * 1000;

    // How long we wait for a launched process to attach to the activity manager
    // before we decide it's never going to come up for real.
    static const Int32 PROC_START_TIMEOUT = 10 * 1000;

    // How long we wait for a launched process to attach to the activity manager
    // before we decide it's never going to come up for real, when the process was
    // started with a wrapper for instrumentation (such as Valgrind) because it
    // could take much longer than usual.
    static const Int32 PROC_START_TIMEOUT_WITH_WRAPPER = 300 * 1000;

    // How long to wait after going idle before forcing apps to GC.
    static const Int32 GC_TIMEOUT = 5 * 1000;

    // The minimum amount of time between successive GC requests for a process.
    static const Int32 GC_MIN_INTERVAL = 60  *1000;

    // The rate at which we check for apps using excessive power -- 15 mins.
    static const Int32 POWER_CHECK_DELAY;

    // The minimum sample duration we will allow before deciding we have
    // enough data on wake locks to start killing things.
    static const Int32 WAKE_LOCK_MIN_CHECK_DURATION;

    // The minimum sample duration we will allow before deciding we have
    // enough data on CPU usage to start killing things.
    static const Int32 CPU_MIN_CHECK_DURATION;

    // How long we allow a receiver to run before giving up on it.
    static const Int32 BROADCAST_FG_TIMEOUT;
    static const Int32 BROADCAST_BG_TIMEOUT;

    // How long we wait until we timeout on key dispatching.
    static const Int32 KEY_DISPATCHING_TIMEOUT;

    // How long we wait until we timeout on key dispatching during instrumentation.
    static const Int32 INSTRUMENTATION_KEY_DISPATCHING_TIMEOUT;

    // Amount of time we wait for observers to handle a user switch before
    // giving up on them and unfreezing the screen.
    static const Int32 USER_SWITCH_TIMEOUT;

    // Maximum number of users we allow to be running at a time.
    static const Int32 MAX_RUNNING_USERS;

    static const Int32 MY_PID;

    static const AutoPtr< ArrayOf<String> > EMPTY_STRING_ARRAY;

    CARAPI RemoveUriPermissionIfNeededLocked(
        /* [in] */ UriPermission* perm)
    {
        return NOERROR;
    }

    CARAPI_(Boolean) IsSleeping()
    {
        return FALSE;
    }
//begin add leliang compile ActivityStack
    class ItemMatcher : public Object
    {
    public:
        ItemMatcher();

        ~ItemMatcher();

        CARAPI_(void) Build(
            /* [in] */ const String& name);

        CARAPI_(Int32) Build(
            /* [in] */ const ArrayOf<String>* args,
            /* [in] */ Int32 opti);

        CARAPI_(Boolean) Match(
            /* [in] */ IInterface* object,
            /* [in] */ IComponentName* comp);

    public:
        List< AutoPtr<IComponentName> > mComponents;
        List<String> mStrings;
        List< AutoPtr<IInteger32> > mObjects;
        Boolean mAll;
    };
    static const Int32 ALLOW_FULL_ONLY = 2;
    static const Int32 FIRST_SUPERVISOR_STACK_MSG = 100;
    static const Int32 FIRST_ACTIVITY_STACK_MSG = 100;
    static const Int32 CANCEL_HEAVY_NOTIFICATION_MSG = 25;
    Boolean mDidDexOpt;
    Boolean mBooting;
    Boolean mBooted;
    String mProfileApp;
    AutoPtr<ProcessRecord> mHeavyWeightProcess;
    AutoPtr<ProcessRecord> mHomeProcess;
    AutoPtr<ProcessRecord> mProfileProc;
    String mProfileFile;
    AutoPtr<IParcelFileDescriptor> mProfileFd;
    Int32 mSamplingInterval;
    Boolean mAutoStopProfiler;
    Int32 mProfileType;
    String mOpenGlTraceApp;
    static const Int32 POST_HEAVY_NOTIFICATION_MSG;
    AutoPtr<ActiveServices> mServices;
    AutoPtr<CProcessStatsService> mProcessStats;
    AutoPtr<IActivityController> mController;
    Boolean mDidAppSwitch;
    Int64 mAppSwitchesAllowedTime;
    Int64 mPreviousProcessVisibleTime;
    AutoPtr<ProcessRecord> mPreviousProcess;
    Boolean mShuttingDown;
    Int32 mCurrentUserId;
    Int32 mThumbnailWidth;
    Int32 mThumbnailHeight;
    Boolean mHasRecents;
    HashMap<Int32, AutoPtr<UserStartedState> > mStartedUsers;
    AutoPtr< ProcessMap< AutoPtr<ProcessRecord> > > mProcessNames;
    AutoPtr<List<AutoPtr<TaskRecord> > > mRecentTasks;
    AutoPtr<TaskPersister> mTaskPersister;
    AutoPtr<ArrayOf<Int32> > mCurrentProfileIds; // Accessed by ActivityStack
    AutoPtr<IInterface/*CBatteryStatsService*/> mBatteryStatsService;
    AutoPtr<IProcessCpuTracker> mProcessCpuTracker;
    AutoPtr<ActivityRecord> mFocusedActivity;
    CARAPI CancelIntentSenderLocked(
        /* [in] */ IIIntentSender* rec,
        /* [in] */ Boolean cleanActivity)
    {
        return NOERROR;
    }
    CARAPI ClearFocusedActivity(
        /* [in] */ ActivityRecord* r)
    {
        return NOERROR;
    }
    CARAPI NotifyTaskPersisterLocked(
        /* [in] */ TaskRecord* task,
        /* [in] */ Boolean flush)
    {
        return NOERROR;
    }
    CARAPI UpdateCpuStats()
    {
        return NOERROR;
    }
    CARAPI LogAppTooSlow(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Int64 startTime,
        /* [in] */ const String& msg)
    {
        return NOERROR;
    }
    CARAPI KillAppAtUsersRequest(
        /* [in] */ ProcessRecord* app,
        /* [in] */ IDialog* fromDialog)
    {
        return NOERROR;
    }
    CARAPI AddRecentTaskLocked(
        /* [in] */ TaskRecord* task)
    {
        return NOERROR;
    }
    CARAPI_(Boolean) ConvertFromTranslucent(
        /* [in] */ IBinder* token)
    {
        return FALSE;
    }
    CARAPI UpdateUsageStats(
        /* [in] */ ActivityRecord* resumedComponent,
        /* [in] */ Boolean resumed)
    {
        return NOERROR;
    }
    CARAPI GrantUriPermissionFromIntentLocked(
        /* [in] */ Int32 callingUid,
        /* [in] */ const String& targetPkg,
        /* [in] */ IIntent* intent,
        /* [in] */ UriPermissionOwner* owner,
        /* [in] */ Int32 targetUserId)
    {
        return NOERROR;
    }
    CARAPI PostFinishBooting(
        /* [in] */ Boolean,
        /* [in] */ Boolean)
    {
        return NOERROR;
    }
    CARAPI TrimApplications()
    {
        return NOERROR;
    }
    CARAPI FinishUserBoot (
        /* [in] */ UserStartedState* uss)
    {
        return NOERROR;
    }
    CARAPI FinishUserSwitch(
        /* [in] */ UserStartedState* uss)
    {
        return NOERROR;
    }
    CARAPI ScheduleAppGcsLocked()
    {
        return NOERROR;
    }
    CARAPI_(Boolean) CheckAppSwitchAllowedLocked(
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 realcallingPid,
        /* [in] */ Int32 realcallingUid,
        /* [in] */ const String& name)
    {
        return FALSE;
    }
    CARAPI_(Int32) CheckComponentPermission(
        /* [in] */ const String& permission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 owningUid,
        /* [in] */ Boolean exported)
    {
        return 0;
    }
    CARAPI CheckPermission(
        /* [in] */ const String& permission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [out] */ Int32* result)
    {
        return NOERROR;
    }
    CARAPI StartSetupActivityLocked()
    {
        return NOERROR;
    }
    CARAPI AppDiedLocked(
        /* [in] */ ProcessRecord* app)
    {
        return NOERROR;
    }
    CARAPI_(Boolean) IsNextTransitionForward()
    {
        return FALSE;
    }
    CARAPI_(AutoPtr<ICompatibilityInfo>) CompatibilityInfoForPackageLocked(
        /* [in] */ IApplicationInfo* ai)
    {
        return NULL;
    }
    CARAPI ShowAskCompatModeDialogLocked(
        /* [in] */ ActivityRecord* r)
    {
        return NOERROR;
    }
    CARAPI EnsurePackageDexOpt(
        /* [in] */ const String& packageName)
    {
        return NOERROR;
    }
    CARAPI_(Boolean) IsSleepingOrShuttingDown()
    {
        return FALSE;
    }
    CARAPI HandleIncomingUser(
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean allowAll,
        /* [in] */ Boolean requireFull,
        /* [in] */ const String& name,
        /* [in] */ const String& callerPackage,
        /* [out] */ Int32* result)
    {
        return NOERROR;
    }
    CARAPI GetProviderMimeType(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 userId,
        /* [out] */ String* type)
    {
        return NOERROR;
    }
    CARAPI EnforceNotIsolatedCaller(
        /* [in] */ const String& caller)
    {
        return NOERROR;
    }
    CARAPI_(void) LogLockScreen(
        /* [in] */ const String msg)
    {
    }
    CARAPI SetFocusedActivityLocked(
        /* [in] */ ActivityRecord* r)
    {
        return NOERROR;
    }
    CARAPI_(Boolean) StartHomeActivityLocked(
        /* [in] */ Int32 userId)
    {
        return FALSE;
    }
    CARAPI SetDebugApp(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean waitForDebugger,
        /* [in] */ Boolean persistent)
    {
        return NOERROR;
    }
    CARAPI SetOpenGlTraceApp(
        /* [in] */ IApplicationInfo* app,
        /* [in] */ const String& processName)
    {
        return NOERROR;
    }
    CARAPI SetProfileApp(
        /* [in] */ IApplicationInfo* app,
        /* [in] */ const String& processName,
        /* [in] */  IProfilerInfo* profilerInfo)
    {
        return NOERROR;
    }
    CARAPI_(AutoPtr<ProcessRecord>) GetRecordForAppLocked(
        /* [in] */ IApplicationThread* thread)
    {
        return NULL;
    }
    CARAPI GetIntentSenderLocked(
        /* [in] */ Int32 type,
        /* [in] */ const String& capsuleName,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 userId,
        /* [in] */ IBinder* token,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ ArrayOf<String>* resolvedTypes,
        /* [in] */ Int32 flags,
        /* [in] */ IBundle* options,
        /* [out] */ IIIntentSender** sender)
    {
        return NOERROR;
    }
    CARAPI_(AutoPtr<IActivityInfo>) GetActivityInfoForUser(
        /* [in] */ IActivityInfo* aInfo,
        /* [in] */ Int32 userId)
    {
        return NOERROR;
    }
    CARAPI EnforceCallingPermission(
        /* [in] */ const String& permission,
        /* [in] */ const String& func)
    {
        return NOERROR;
    }
    CARAPI_(Boolean) UpdateConfigurationLocked(
        /* [in] */ IConfiguration* values,
        /* [in] */ ActivityRecord* starting,
        /* [in] */ Boolean persistent,
        /* [in] */ Boolean initLocale)
    {
        return FALSE;
    }
    AutoPtr<ProcessRecord> GetProcessRecordLocked(
        /* [in] */ const String& processName,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean keepIfLarge)
    {
        return NULL;
    }
    AutoPtr<ProcessRecord> StartProcessLocked(
        /* [in] */ const String& processName,
        /* [in] */ IApplicationInfo* info,
        /* [in] */ Boolean knownToBeDead,
        /* [in] */ Int32 intentFlags,
        /* [in] */ const String& hostingType,
        /* [in] */ IComponentName* hostingName,
        /* [in] */ Boolean allowWhileBooting,
        /* [in] */ Boolean isolated,
        /* [in] */ Boolean keepIfLarge)
    {
        return NULL;
    }
    CARAPI UpdateLruProcessLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Boolean oomAdj,
        /* [in] */ ProcessRecord* client)
    {
        return NOERROR;
    }
    CARAPI UpdateOomAdjLocked()
    {
        return NOERROR;
    }
//end add leliang compile ActivityStack
public:
    AutoPtr<IConfiguration> mConfiguration;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IContext> mContext;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#else

#include "_Elastos_Droid_Server_Am_CActivityManagerService.h"
#include "ProcessMap.h"
#include "IntentResolver.h"
#include "wm/CWindowManagerService.h"
#include "elastos/droid/server/am/ContentProviderRecord.h"
#include "elastos/droid/server/am/CPendingAssistExtras.h"
#include "elastos/droid/server/am/CPendingIntentRecord.h"
#include "elastos/droid/server/am/CBatteryStatsService.h"
#include "elastos/droid/server/am/CUsageStatsService.h"
#include "elastos/droid/server/am/CoreSettingsObserver.h"
#include "elastos/droid/server/am/BackupRecord.h"
#include "elastos/droid/server/am/ProviderMap.h"
#include "elastos/droid/server/am/UriPermission.h"
#include "elastos/droid/server/am/UriPermissionOwner.h"
#include "elastos/droid/server/am/PendingThumbnailsRecord.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/server/am/ProcessList.h"
#include "elastos/droid/server/am/CompatModePackages.h"
#include "elastos/droid/server/pm/CUserManagerService.h"
#include <elastos/droid/os/FileObserver.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/os/Runnable.h>
// #include <binder/IPermissionController.h>
#include <elastos/core/StringBuilder.h>
#include <pthread.h>

using Elastos::Droid::App::IActivityManagerTaskThumbnails;
using Elastos::Droid::App::IActivityManagerMemoryInfo;
using Elastos::Droid::App::IActivityManagerRunningAppProcessInfo;
using Elastos::Droid::App::IActivityManagerProcessErrorStateInfo;
using Elastos::Droid::App::IActivityThread;
using Elastos::Droid::App::IActivityController;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IApplicationThread;
using Elastos::Droid::App::IApplicationErrorReport;
using Elastos::Droid::App::IApplicationErrorReportCrashInfo;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IActivityManagerWaitResult;
using Elastos::Droid::App::IContentProviderHolder;
using Elastos::Droid::App::IInstrumentationWatcher;
using Elastos::Droid::App::IIServiceConnection;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::IProcessObserver;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IStopUserCallback;
using Elastos::Droid::App::IThumbnailReceiver;
using Elastos::Droid::App::IUserSwitchObserver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::IIIntentSender;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IProviderInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IConfigurationInfo;
using Elastos::Droid::Content::Pm::IPackageDataObserver;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Internal::Os::IProcessStats;
using Elastos::Droid::Internal::Os::CProcessStats;
using Elastos::Droid::Internal::Os::IProcessStatsStats;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::FileObserver;
using Elastos::Droid::Os::IIUserManager;
using Elastos::Droid::Os::IDropBoxManager;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IDebugMemoryInfo;
using Elastos::Droid::Os::IStrictModeViolationInfo;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Server::ProcessMap;
using Elastos::Droid::Server::IntentResolver;
using Elastos::Droid::Server::Wm::CWindowManagerService;
using Elastos::Droid::Server::Pm::CUserManagerService;
using Elastos::Droid::View::IWindowManager;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::IO::IFile;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::ILocale;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger64;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

extern "C" const InterfaceID EIID_StringObjectHashMap;

class ActiveServices;
class ActivityStack;
class BackupRecord;
class BroadcastFilter;
class BroadcastQueue;
class BroadcastRecord;
//class CBatteryStatsService;
class CompatModePackages;
class CCompatModeDialog;
class ContentProviderRecord;
class CContentProviderConnection;
class CoreSettingsObserver;
class ProcessList;
class ProviderMap;
class ReceiverList;
class TaskRecord;
class UserStartedState;
class CUsageStatsService;
class CompatModeDialog;

class GrantUri : public Object
{
public:
    GrantUri(
        /* [in] */ Int32 sourceUserId,
        /* [in] */ IUri* uri,
        /* [in] */ Boolean prefix);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI_(Boolean) Equals(
        /* [in] */ GrantUri* other);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI_(String) ToString();

    CARAPI_(String) ToSafeString();

    static CARAPI_(AutoPtr<GrantUri>) Resolve(
        /* [in] */ Int32 defaultSourceUserHandle,
        /* [in] */ IUri* uri);

public:
    const Int32 mSourceUserId;
    const AutoPtr<IUri> mUri;
    Boolean mPrefix;
};

class NeededUriGrants
    : public List< AutoPtr<IUri> >
{
public:
    NeededUriGrants(
        /* [in] */ const String& targetPkg,
        /* [in] */ Int32 targetUid,
        /* [in] */ Int32 flags);

public:
    String mTargetPkg;
    Int32 mTargetUid;
    Int32 mFlags;
};

CarClass(CActivityManagerService)
{
    friend class ActivityRecord;
    friend class CStoppingReceiver;
    friend class CompatModePackages;
    friend class CoreSettingsObserver;

public:
    /**
     * Information about a process that is currently marked as bad.
     */
    class BadProcessInfo : public Object
    {
    public:
        BadProcessInfo(
            /* [in] */ Int64 time,
            /* [in] */ const String& shortMsg,
            /* [in] */ const String& longMsg,
            /* [in] */ const String& stack)
            : mTime(time)
            , mShortMsg(shortMsg)
            , mLongMsg(longMsg)
            , mStack(stack)
        {
        }

    public:
        const Int64 mTime;
        const String mShortMsg;
        const String mLongMsg;
        const String mStack;
    };

    class StringObjectHashMap
        : public IInterface
        , public HashMap<String, AutoPtr<IInterface> >
    {
    public:
        CAR_INTERFACE_DECL()
    };

    /**
     * All of the processes that have been forced to be foreground.  The key
     * is the pid of the caller who requested it (we hold a death
     * link on it).
     */
    class ForegroundToken
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        ForegroundToken();

        virtual CARAPI ProxyDied();

        CAR_INTERFACE_DECL()
    public:
        Int32 mPid;
        AutoPtr<IBinder> mToken;
    };

    class ProcessChangeItem : public Object
    {
    public:
        ProcessChangeItem();
    public:
        static const Int32 CHANGE_ACTIVITIES = 1<<0;
        static const Int32 CHANGE_PROCESS_STATE= 1<<1;
        Int32 mChanges;
        Int32 mUid;
        Int32 mPid;
        Int32 mProcessState;
        Boolean mForegroundActivities;
    };

    class MainHandler : public Handler
    {
    public:
        MainHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CActivityManagerService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CActivityManagerService* mHost;
    };

    class Lifecycle : public SystemService
    {
    public:
        Lifecycle(
            /* [in] */ IContext* context)
        {
            SystemService::constructor(context);
            mService = new ActivityManagerService(context);
        }

        // @Override
        CARAPI OnStart()
        {
            mService->Start();
            return NOERROR;
        }

        AutoPtr<CActivityManagerService> GetService()
        {
            return mService;
        }

    private:
        AutoPtr<CActivityManagerService> mService;
    };

    class mProcessCpuThread
        : public Thread
    {
    public:
        mProcessCpuThread(
            /* [in] */ CActivityManagerService* host);

        CARAPI Run();

    public:
        CActivityManagerService* mHost;
    };

    class ItemMatcher : public Object
    {
    public:
        ItemMatcher();

        ~ItemMatcher();

        CARAPI_(void) Build(
            /* [in] */ const String& name);

        CARAPI_(Int32) Build(
            /* [in] */ const ArrayOf<String>* args,
            /* [in] */ Int32 opti);

        CARAPI_(Boolean) Match(
            /* [in] */ IInterface* object,
            /* [in] */ IComponentName* comp);

    public:
        List< AutoPtr<IComponentName> > mComponents;
        List<String> mStrings;
        List<Int32> mObjects;
        Boolean mAll;
    };

    class MemItem : public Object
    {
    public:
        MemItem(
            /* [in] */ const String& label,
            /* [in] */ const String& shortLabel,
            /* [in] */ Int64 pss,
            /* [in] */ Int32 id);

        ~MemItem();

    public:
        const String mLabel;
        const String mShortLabel;
        const Int64 mPss;
        const Int32 mId;
        AutoPtr<List< AutoPtr<MemItem> > > mSubitems;
    };

    class ReceiverResolver
        : public IntentResolver<BroadcastFilter, BroadcastFilter>
    {
    protected:
        Boolean AllowFilterResult(
                /* [in] */ BroadcastFilter* filter,
                /* [in] */ List<AutoPtr<BroadcastFilter> >* dest);

        AutoPtr<BroadcastFilter> NewResult(
            /* [in] */ BroadcastFilter* filter,
            /* [in] */ Int32 match,
            /* [in] */ Int32 userId);

        AutoPtr< ArrayOf<BroadcastFilter*> > NewArray(
            /* [in] */ Int32 size);

        Boolean IsPackageForFilter(
            /* [in] */ const String& packageName,
            /* [in] */ BroadcastFilter* filter);
    };

    /**
     * Thread-local storage used to carry caller permissions over through
     * indirect content-provider access.
     * @see #ActivityManagerService.openContentUri()
     */
    class Identity : public Object
    {
    public:
        Identity(
            /* [in] */ Int32 pid,
            /* [in] */ Int32 uid);
    public:
        Int32 mPid;
        Int32 mUid;
    };

private:
    class AppDeathRecipient
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        AppDeathRecipient(
            /* [in] */ ProcessRecord* app,
            /* [in] */ Int32 pid,
            /* [in] */ IApplicationThread* thread,
            /* [in] */ CActivityManagerService* owner);

        CARAPI ProxyDied();

        CAR_INTERFACE_DECL()

    public:
        AutoPtr<ProcessRecord> mApp;
        Int32 mPid;
        AutoPtr<IApplicationThread> mAppThread;
        CActivityManagerService* mOwner;
    };

    class SwitchUserIntentReceiver
        : public Object
        , public IIntentReceiver
    {
    public:
        SwitchUserIntentReceiver(
            /* [in] */ CActivityManagerService* host,
            /* [in] */ UserStartedState* uss,
            /* [in] */ Int32 userId)
            : mHost(host)
            , mUss(uss)
            , mUserId(userId)
        {}

        CAR_INTERFACE_DECL()

        CARAPI PerformReceive(
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 resultCode,
            /* [in] */ const String& data,
            /* [in] */ IBundle* extras,
            /* [in] */ Boolean ordered,
            /* [in] */ Boolean sticky,
            /* [in] */ Int32 sendingUser);

    private:
        CActivityManagerService* mHost;
        UserStartedState* mUss;
        Int32 mUserId;
    };

    class NeedStartIntentReceiver
        : public Object
        , public IIntentReceiver
    {
    public:
        NeedStartIntentReceiver()
        {}

        CAR_INTERFACE_DECL()

        CARAPI PerformReceive(
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 resultCode,
            /* [in] */ const String& data,
            /* [in] */ IBundle* extras,
            /* [in] */ Boolean ordered,
            /* [in] */ Boolean sticky,
            /* [in] */ Int32 sendingUser)
        {
            return NOERROR;
        }
    };

    class ReportMemUsageThread
        : public Thread
    {
    private:
        class ProcessMemInfoComparator
            : public Object
            , public IComparator
        {
        public:
            CAR_INTERFACE_DECL()

            // @Override
            CARAPI Compare(
                /* [in] */ IInterface* _lhs,
                /* [in] */ IInterface* _rhs);
        };

    public:
        ReportMemUsageThread(
            /* [in] */ IList* memInfos,
            /* [in] */ CActivityManagerService* host);

        CARAPI Run();

    public:
        CActivityManagerService* mHost;
        AutoPtr<IList> mMemInfos;
    };

    class BgHandler : public Handler
    {
    public:
        BgHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CActivityManagerService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CActivityManagerService* mHost;
    };


    /**
     * Monitor for package changes and update our internal state.
     */
    class MyPackageMonitor : public PackageMonitor
    {
    public:
        MyPackageMonitor(
            /* [in] */ CActivityManagerService* host);

        // @Override
        CARAPI OnPackageRemoved(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

        // @Override
        CARAPI OnPackageChanged(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid,
            /* [in] */ ArrayOf<String>* components,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnPackageModified(
            /* [in] */ const String& packageName);

        // @Override
        CARAPI OnHandleForceStop(
            /* [in] */ IIntent* intent,
            /* [in] */ ArrayOf<String>* packages,
            /* [in] */ Int32 uid,
            /* [in] */ Boolean doit,
            /* [out] */ Boolean* result);

    private:
        CActivityManagerService* mHost;
    };

    class DumpStackTracesFileObserver
        : public FileObserver
        , public Object
    {
    public:
        DumpStackTracesFileObserver(
            /* [in] */ const String& path,
            /* [in] */ Int32 mask)
            : FileObserver(path, mask)
        {}

        CARAPI OnEvent(
            /* [in] */ Int32 event,
            /* [in] */ const String& path);
    };

    class ShowRunnable
        : public Runnable
    {
    public:
        ShowRunnable(
            /* [in] */ CActivityManagerService* host,
            /* [in] */ IDialog* dialog)
            : mHost(host)
            , mDialog(dialog)
        {}

        CARAPI Run();

    private:
        CActivityManagerService* mHost;
        AutoPtr<IDialog> mDialog;
    };

    class ShowLaunchWarningLockedRunnable
        : public Runnable
    {
    public:
        ShowLaunchWarningLockedRunnable(
            /* [in] */ CActivityManagerService* host,
            /* [in] */ ActivityRecord* cur,
            /* [in] */ ActivityRecord* next)
            : mHost(host)
            , mCur(cur)
            , mNext(next)
        {}

        CARAPI Run();

    private:
        CActivityManagerService* mHost;
        AutoPtr<ActivityRecord> mCur;
        AutoPtr<ActivityRecord> mNext;
    };

    class FinishBootingBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        FinishBootingBroadcastReceiver(
            /* [in] */ CActivityManagerService* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CActivityManagerService::FinishBootingBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CActivityManagerService* mHost;
    };

    class SetProcessForegroundToken
        : public ForegroundToken
    {
    public:
        SetProcessForegroundToken(
            /* [in] */ CActivityManagerService* host)
            : mHost(host)
        {}

        CARAPI ProxyDied();

    private:
        CActivityManagerService* mHost;
    };

    // class FinisherReceiver
    //     : public Object
    //     , public IIntentReceiver
    // {
    // public:
    //     FinisherReceiver(
    //         /* [in] */ CActivityManagerService* host,
    //         /* [in] */ List< AutoPtr<IComponentName> >* receivers,
    //         /* [in] */ IRunnable* goingCallback)
    //         : mHost(host)
    //         , mReceivers(receivers)
    //         , mCallback(goingCallback)
    //     {}

    //     CAR_INTERFACE_DECL()

    //     CARAPI PerformReceive(
    //        /* [in] */ IIntent* intent,
    //        /* [in] */ Int32 resultCode,
    //        /* [in] */ const String& data,
    //        /* [in] */ IBundle* extras,
    //        /* [in] */ Boolean ordered,
    //        /* [in] */ Boolean sticky,
    //        /* [in] */ Int32 sendingUser);

    // private:
    //     CActivityManagerService* mHost;
    //     AutoPtr<List< AutoPtr<IComponentName> > > mReceivers;
    //     AutoPtr<IRunnable> mCallback;
    // };

    // class PerformReceiveRunnable
    //     : public Object
    //     , public IRunnable
    // {
    // public:
    //     PerformReceiveRunnable(
    //         /* [in] */ CActivityManagerService* host,
    //         /* [in] */ List< AutoPtr<IComponentName> >* receivers,
    //         /* [in] */ IRunnable* callback)
    //         : mHost(host)
    //         , mCallback(callback)
    //         , mReceivers(receivers)
    //     {}

    //     CAR_INTERFACE_DECL()

    //     CARAPI Run();

    // private:
    //     CActivityManagerService* mHost;
    //     AutoPtr<List< AutoPtr<IComponentName> > > mReceivers;
    //     AutoPtr<IRunnable> mCallback;
    // };

    class ErrorMsgButtonOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        ErrorMsgButtonOnClickListener(
            /* [in] */ CActivityManagerService* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CActivityManagerService* mHost;
    };

    // class SystemBroadcastReceiver
    //     : public Object
    //     , public IIntentReceiver
    // {
    // public:
    //     SystemBroadcastReceiver()
    //     {}

    //     CAR_INTERFACE_DECL()

    //     CARAPI PerformReceive(
    //         /* [in] */ IIntent* intent,
    //         /* [in] */ Int32 resultCode,
    //         /* [in] */ const String& data,
    //         /* [in] */ IBundle* extras,
    //         /* [in] */ Boolean ordered,
    //         /* [in] */ Boolean sticky,
    //         /* [in] */ Int32 sendingUser);
    // };

    class DropBoxTagThread
        : public Thread
    {
    public:
        DropBoxTagThread(
            /* [in] */ CActivityManagerService* host,
            /* [in] */ StringBuilder* sb,
            /* [in] */ Object* lock,
            /* [in] */ IDropBoxManager* dbox,
            /* [in] */ const String& tag);

        CARAPI Run();

    public:
        CActivityManagerService* mHost;
        AutoPtr<StringBuilder> mSb;
        Object* mSbLock;
        AutoPtr<IDropBoxManager> mDbox;
        String mTag;
    };

    class DropBoxTagThread2
        : public Thread
    {
    public:
        DropBoxTagThread2(
            /* [in] */ CActivityManagerService* host,
            /* [in] */ IDropBoxManager* dbox,
            /* [in] */ const String& tag);

        CARAPI Run();

    public:
        CActivityManagerService* mHost;
        AutoPtr<IDropBoxManager> mDbox;
        String mTag;
    };

    class WorkerThread
        : public Thread
    {
    public:
        WorkerThread(
            /* [in] */ CActivityManagerService* host,
            /* [in] */ const String& dropboxTag,
            /* [in] */ const String& report,
            /* [in] */ StringBuilder* sb,
            /* [in] */ IFile* logFile,
            /* [in] */ IApplicationErrorReportCrashInfo* crashInfo,
            /* [in] */ IDropBoxManager* dbox);

        CARAPI Run();

    public:
        CActivityManagerService* mHost;
        String mDropboxTag;
        String mReport;
        AutoPtr<StringBuilder> mSb;
        AutoPtr<IFile> mLogFile;
        AutoPtr<IApplicationErrorReportCrashInfo> mCrashInfo;
        AutoPtr<IDropBoxManager> mDbox;
    };

    class StopUserLockedRunnable
        : public Runnable
    {
    public:
        StopUserLockedRunnable(
            /* [in] */ Int32 userId,
            /* [in] */ IStopUserCallback* callback)
            : mUserId(userId)
            , mCallback(callback)
        {}

        CARAPI Run();

    private:
        Int32 mUserId;
        AutoPtr<IStopUserCallback> mCallback;
    };

    class DispatchUserSwitchCallback
        : public Object
        , public IRemoteCallback
    {
    public:
        DispatchUserSwitchCallback(
            /* [in] */ CActivityManagerService* host,
            /* [in] */ Int32 n,
            /* [in] */ UserStartedState* uss,
            /* [in] */ Int32 oldUserId,
            /* [in] */ Int32 newUserId)
            : mHost(host)
            , mCount(0)
            , N(n)
            , mUss(uss)
            , mOldUserId(oldUserId)
            , mNewUserId(newUserId)
        {}

        CAR_INTERFACE_DECL()

        CARAPI SendResult(
            /* [in] */ IBundle* data);

    private:
        CActivityManagerService* mHost;
        Int32 mCount;
        Int32 N;
        AutoPtr<UserStartedState> mUss;
        Int32 mOldUserId;
        Int32 mNewUserId;
    };

public:
    CActivityManagerService();

    ~CActivityManagerService();

    CARAPI constructor(
        /* [in] */ IContext* systemContext);

    CARAPI GetSystemContext(
        /* [out] */ IContext** ctx);

    CARAPI SetSystemProcess();

    CARAPI_(void) SetWindowManager(
        /* [in] */ CWindowManagerService* wm);

    CARAPI_(void) SetUsageStatsManager(
        /* [in] */ IUsageStatsManagerInternal* usageStatsManager);

    CARAPI_(void) StartObservingNativeCrashes();

    CARAPI_(AutoPtr<CAppOpsService>) GetAppOpsService();

    CARAPI_(void) SetSystemServiceManager(
        /* [in] */ SystemServiceManager* mgr);

    CARAPI_(void) InitPowerManagement();

    //
    // interface IIActivityManager
    //
    CARAPI SetFocusedStack(
        /* [in] */ Int32 stackId);

    // @Override
    CARAPI NotifyActivityDrawn(
        /* [in] */ IBinder token);

    CARAPI StartActivity(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& callingPackage,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IBinder* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 startFlags,
        /* [in] */ const String& profilerInfo,
        /* [in] */ IBundle* options,
        /* [out] */ Int32* result);

    CARAPI StartActivityAsUser(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& callingPackage,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IBinder* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 flags,
        /* [in] */ const String& profilerInfo,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI StartActivityAndWait(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& callingPackage,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IBinder* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 flags,
        /* [in] */ const String& profilerInfo,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ IActivityManagerWaitResult** result);

    CARAPI StartActivityWithConfig(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& callingPackage,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IBinder* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 startFlags,
        /* [in] */ IConfiguration* newConfig,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI StartActivityIntentSender(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IIntentSender* intent,
        /* [in] */ IIntent* fillInIntent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IBinder* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 flagsMask,
        /* [in] */ Int32 flagsValues,
        /* [in] */ IBundle* options,
        /* [out] */ Int32* result);

    CARAPI StartVoiceActivity(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IIVoiceInteractionSession* session,
        /* [in] */ IIVoiceInteractor* interactor,
        /* [in] */ Int32 startFlags,
        /* [in] */ IProfilerInfo* profilerInfo,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI StartNextMatchingActivity(
        /* [in] */ IBinder* callingActivity,
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* options,
        /* [out] */ Boolean* result);

    CARAPI StartActivityFromRecents(
        /* [in] */ Int32 taskId,
        /* [in] */ IBundle* options,
        /* [out] */ Int32* result);

    CARAPI FinishActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 code,
        /* [in] */ IIntent* data,
        /* [in] */ Boolean finishTask,
        /* [out] */ Boolean* result);

    CARAPI FinishSubActivity(
        /* [in] */ IBinder* token,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode);

    CARAPI FinishActivityAffinity(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI FinishVoiceTask(
        /* [in] */ IIVoiceInteractionSession* session);

    CARAPI ReleaseActivityInstance(
        /* [in] */ IBinder* token);

    CARAPI ReleaseSomeActivities(
        /* [in] */ IApplicationThread* appInt);

    CARAPI WillActivityBeVisible(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* visible);

    CARAPI RegisterReceiver(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& callerPackage,
        /* [in] */ IIntentReceiver* receiver,
        /* [in] */ IIntentFilter* filter,
        /* [in] */ const String& requiredPermission,
        /* [in] */ Int32 userId,
        /* [out] */ IIntent** intent);

    CARAPI UnregisterReceiver(
        /* [in] */ IIntentReceiver* receiver);

    CARAPI BroadcastIntent(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IIntentReceiver* resultTo,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& resultData,
        /* [in] */ IBundle* map,
        /* [in] */ const String& requiredPermission,
        /* [in] */ Boolean serialized,
        /* [in] */ Boolean sticky,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI UnbroadcastIntent(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 userId);

    CARAPI FinishReceiver(
        /* [in] */ IBinder* who,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& resultData,
        /* [in] */ IBundle* map,
        /* [in] */ Boolean abortBroadcast);

    CARAPI AttachApplication(
        /* [in] */ IApplicationThread* app);

    CARAPI ActivityResumed(
        /* [in] */ IBinder* token);

    CARAPI ActivityIdle(
        /* [in] */ IBinder* token,
        /* [in] */ IConfiguration* config,
        /* [in] */ Boolean stopProfiling);

    CARAPI ActivityPaused(
        /* [in] */ IBinder* token);

    CARAPI ActivityStopped(
        /* [in] */ IBinder* token,
        /* [in] */ IBundle* state,
        /* [in] */ IBitmap* thumbnail,
        /* [in] */ ICharSequence* description);

    CARAPI ActivitySlept(
        /* [in] */ IBinder* token);

    CARAPI ActivityDestroyed(
        /* [in] */ IBinder* token);

    CARAPI GetCallingPackage(
        /* [in] */ IBinder* token,
        /* [out] */ String* pkg);

    CARAPI GetCallingActivity(
        /* [in] */ IBinder* token,
        /* [out] */ IComponentName** activity);

    CARAPI GetTasks(
        /* [in] */ Int32 maxNum,
        /* [in] */ Int32 flags,
        /* [in] */ IThumbnailReceiver* receiver,
        /* [out] */ IObjectContainer** tasks);

    CARAPI GetRecentTasks(
        /* [in] */ Int32 maxNum,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IObjectContainer** tasks);

    CARAPI GetTaskThumbnails(
        /* [in] */ Int32 taskId,
        /* [out] */ IActivityManagerTaskThumbnails** taskThumbnails);

    CARAPI GetTaskTopThumbnail(
        /* [in] */ Int32 taskId,
        /* [out] */ IBitmap** bitmap);

    CARAPI GetServices(
        /* [in] */ Int32 maxNum,
        /* [in] */ Int32 flags,
        /* [out] */ IObjectContainer** services);

    CARAPI GetProcessesInErrorState(
        /* [out] */ IObjectContainer** processes);

    CARAPI MoveTaskToFront(
        /* [in] */ Int32 task,
        /* [in] */ Int32 flags,
        /* [in] */ IBundle* options);

    CARAPI MoveTaskToBack(
        /* [in] */ Int32 task);

    CARAPI MoveActivityTaskToBack(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean nonRoot,
        /* [out] */ Boolean* completed);

    CARAPI MoveTaskBackwards(
        /* [in] */ Int32 task);

    CARAPI GetTaskForActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean onlyRoot,
        /* [out] */ Int32* task);

    /* oneway */
    CARAPI ReportThumbnail(
        /* [in] */ IBinder* token,
        /* [in] */ IBitmap* thumbnail,
        /* [in] */ ICharSequence* description);

    CARAPI GetContentProvider(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& name,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean stable,
        /* [out] */ IContentProviderHolder** contentProvider);

    CARAPI GetContentProviderExternal(
        /* [in] */ const String& name,
        /* [in] */ Int32 userId,
        /* [in] */ IBinder* token,
        /* [out] */ IContentProviderHolder** holder);

    CARAPI RemoveContentProvider(
        /* [in] */ IBinder* connection,
        /* [in] */ Boolean stable);

    CARAPI RemoveContentProviderExternal(
        /* [in] */ const String& name,
        /* [in] */ IBinder* token);

    CARAPI PublishContentProviders(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IObjectContainer* providers);

    CARAPI RefContentProvider(
        /* [in] */ IBinder* connection,
        /* [in] */ Int32 stableDelta,
        /* [in] */ Int32 unstableDelta,
        /* [out] */ Boolean* result);

    CARAPI UnstableProviderDied(
        /* [in] */ IBinder* connection);

    CARAPI GetRunningServiceControlPanel(
        /* [in] */ IComponentName* service,
        /* [out] */ IPendingIntent** pendingIntent);

    CARAPI StartService(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IIntent* service,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 userId,
        /* [out] */ IComponentName** componentName);

    CARAPI StopService(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IIntent* service,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI StopServiceToken(
        /* [in] */ IComponentName* className,
        /* [in] */ IBinder* token,
        /* [in] */ Int32 startId,
        /* [out] */ Boolean* result);

    CARAPI SetServiceForeground(
        /* [in] */ IComponentName* className,
        /* [in] */ IBinder* token,
        /* [in] */ Int32 id,
        /* [in] */ INotification* notification,
        /* [in] */ Boolean keepNotification);

    CARAPI BindService(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IBinder* token,
        /* [in] */ IIntent* service,
        /* [in] */ const String& resolvedType,
        /* [in] */ IIServiceConnection* connection,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI UnbindService(
        /* [in] */ IIServiceConnection* connection,
        /* [out] */ Boolean* result);

    CARAPI PublishService(
        /* [in] */ IBinder* token,
        /* [in] */ IIntent* intent,
        /* [in] */ IBinder* service);

    CARAPI UnbindFinished(
        /* [in] */ IBinder* token,
        /* [in] */ IIntent* service,
        /* [in] */ Boolean doRebind);

    /* oneway */
    CARAPI ServiceDoneExecuting(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 type,
        /* [in] */ Int32 startId,
        /* [in] */ Int32 res);

    CARAPI PeekService(
        /* [in] */ IIntent* service,
        /* [in] */ const String& resolvedType,
        /* [out] */ IBinder** binder);

    CARAPI BindBackupAgent(
        /* [in] */ IApplicationInfo* appInfo,
        /* [in] */ Int32 backupRestoreMode,
        /* [out] */ Boolean* result);

    CARAPI ClearPendingBackup();

    CARAPI BackupAgentCreated(
        /* [in] */ const String& packageName,
        /* [in] */ IBinder* agent);

    CARAPI UnbindBackupAgent(
        /* [in] */ IApplicationInfo* appInfo);

    CARAPI KillApplicationProcess(
        /* [in] */ const String& processName,
        /* [in] */ Int32 uid);

    CARAPI StartInstrumentation(
        /* [in] */ IComponentName* className,
        /* [in] */ const String& profileFile,
        /* [in] */ Int32 flags,
        /* [in] */ IBundle* arguments,
        /* [in] */ IInstrumentationWatcher* watcher,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI FinishInstrumentation(
        /* [in] */ IApplicationThread* target,
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

    CARAPI GetConfiguration(
        /* [out] */ IConfiguration** config);

    CARAPI UpdateConfiguration(
        /* [in] */ IConfiguration* values);

    CARAPI ReportActivityFullyDrawn(
        /* [in] */ IBinder token);

    CARAPI SetRequestedOrientation(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 requestedOrientation);

    CARAPI GetRequestedOrientation(
        /* [in] */ IBinder* token,
        /* [out] */ Int32* orientation);

    CARAPI GetActivityClassForToken(
        /* [in] */ IBinder* token,
        /* [out] */ IComponentName** klass);

    CARAPI GetPackageForToken(
        /* [in] */ IBinder* token,
        /* [out] */ String* pkg);

    CARAPI GetIntentSender(
        /* [in] */ Int32 type,
        /* [in] */ const String& packageName,
        /* [in] */ IBinder* token,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ ArrayOf<String>* resolvedTypes,
        /* [in] */ Int32 flags,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ IIIntentSender** intentSender);

    CARAPI CancelIntentSender(
        /* [in] */ IIIntentSender* sender);

    CARAPI ClearApplicationUserData(
        /* [in] */ const String& packageName,
        /* [in] */ IPackageDataObserver* observer,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI GetPackageForIntentSender(
        /* [in] */ IIIntentSender* sender,
        /* [out] */ String* pkg);

    CARAPI GetUidForIntentSender(
        /* [in] */ IIIntentSender* sender,
        /* [out] */ Int32* result);

    CARAPI HandleIncomingUser(
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean allowAll,
        /* [in] */ Boolean requireFull,
        /* [in] */ const String& name,
        /* [in] */ const String& callerPackage,
        /* [out] */ Int32* result);

    CARAPI GetIntentForIntentSender(
        /* [in] */ IIIntentSender* sender,
        /* [out] */ IIntent** intent);

    CARAPI SetProcessLimit(
        /* [in] */ Int32 max);

    CARAPI GetProcessLimit(
        /* [out] */ Int32* limit);

    CARAPI SetProcessForeground(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 pid,
        /* [in] */ Boolean isForeground);

    CARAPI CheckPermission(
        /* [in] */ const String& permission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [out] */ Int32* result);

    CARAPI CheckUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 mode,
        /* [out] */ Int32* result);

    CARAPI GrantUriPermission(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& targetPkg,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 mode);

    CARAPI RevokeUriPermission(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 mode);

    CARAPI ShowWaitingForDebugger(
        /* [in] */ IApplicationThread* who,
        /* [in] */ Boolean waiting);

    CARAPI GetMemoryInfo(
        /* [out] */ IActivityManagerMemoryInfo** outInfo);

    CARAPI KillBackgroundProcesses(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId);

    CARAPI KillAllBackgroundProcesses();

    CARAPI ForceStopPackage(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId);

    // Note: probably don't want to allow applications access to these.
    CARAPI GoingToSleep();

    CARAPI WakingUp();

    CARAPI SetLockScreenShown(
        /* [in] */ Boolean shown);

    CARAPI UnhandledBack();

    CARAPI OpenContentUri(
        /* [in] */ IUri* uri,
        /* [out] */ IParcelFileDescriptor** descriptor);

    CARAPI SetDebugApp(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean waitForDebugger,
        /* [in] */ Boolean persistent);

    CARAPI SetAlwaysFinish(
        /* [in] */ Boolean enabled);

    CARAPI SetActivityController(
        /* [in] */ IActivityController* watcher);

    CARAPI EnterSafeMode();

    CARAPI NoteWakeupAlarm(
        /* [in] */ IIIntentSender* sender);

    CARAPI KillPids(
        /* [in] */ ArrayOf<Int32>* pids,
        /* [in] */ const String& reason,
        /* [in] */ Boolean secure,
        /* [out] */ Boolean* result);

    CARAPI KillProcessesBelowForeground(
        /* [in] */ const String& reason,
        /* [out] */ Boolean* result);

    // Special low-level communication with activity manager.
    CARAPI StartRunning(
        /* [in] */ const String& pkg,
        /* [in] */ const String& cls,
        /* [in] */ const String& action,
        /* [in] */ const String& data);

    CARAPI HandleApplicationCrash(
        /* [in] */ IBinder* app,
        /* [in] */ IApplicationErrorReportCrashInfo* crashInfo);

    CARAPI HandleApplicationWtf(
        /* [in] */ IBinder* app,
        /* [in] */ const String& tag,
        /* [in] */ IApplicationErrorReportCrashInfo* crashInfo,
        /* [out] */ Boolean* result);

    // A StrictMode violation to be handled.  The violationMask is a
    // subset of the original StrictMode policy bitmask, with only the
    // bit violated and penalty bits to be executed by the
    // ActivityManagerService remaining set.
    CARAPI HandleApplicationStrictModeViolation(
        /* [in] */ IBinder* app,
        /* [in] */ Int32 violationMask,
        /* [in] */ IStrictModeViolationInfo* crashInfo);

    /*
     * This will deliver the specified signal to all the persistent processes. Currently only
     * SIGUSR1 is delivered. All others are ignored.
     */
    CARAPI SignalPersistentProcesses(
        /* [in] */ Int32 signal);

    // Retrieve running application processes in the system
    CARAPI GetRunningAppProcesses(
        /* [out] */ IObjectContainer** processes);

    // Retrieve info of applications installed on external media that are currently
    // running.
    CARAPI GetRunningExternalApplications(
        /* [out] */ IObjectContainer** applications);

    // Get memory information about the calling process.
    CARAPI GetMyMemoryState(
        /* [in] */ IActivityManagerRunningAppProcessInfo* outInfo);

    // Get device configuration
    CARAPI GetDeviceConfigurationInfo(
        /* [out] */ IConfigurationInfo** configInfo);

    // Turn on/off profiling in a particular process.
    CARAPI ProfileControl(
        /* [in] */ const String& process,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean start,
        /* [in] */ const String& path,
        /* [in] */ IParcelFileDescriptor* fd,
        /* [in] */ Int32 profileType,
        /* [out] */ Boolean* result);

    CARAPI Shutdown(
        /* [in] */ Int32 timeout,
        /* [out] */ Boolean* result);

    CARAPI StopAppSwitches();

    CARAPI ResumeAppSwitches();

    CARAPI AddPackageDependency(
        /* [in] */ const String& packageName);

    CARAPI KillApplicationWithAppId(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 appid,
        /* [in] */ const String& reason);

    CARAPI CloseSystemDialogs(
        /* [in] */ const String& reason);

    CARAPI GetProcessMemoryInfo(
        /* [in] */ ArrayOf<Int32>* pids,
        /* [out, callee] */ ArrayOf<IDebugMemoryInfo*>** memoryInfo);

    CARAPI OverridePendingTransition(
        /* [in] */ IBinder* token,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 enterAnim,
        /* [in] */ Int32 exitAnim);

    CARAPI IsUserAMonkey(
        /* [out] */ Boolean* result);

    CARAPI FinishHeavyWeightApp();

    CARAPI SetImmersive(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean immersive);

    CARAPI IsImmersive(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI IsTopActivityImmersive(
        /* [out] */ Boolean* result);

    CARAPI CrashApplication(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 initialPid,
        /* [in] */ const String& packageName,
        /* [in] */ const String& message);

    CARAPI GetProviderMimeType(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 userId,
        /* [out] */ String* type);

    CARAPI NewUriPermissionOwner(
        /* [in] */ const String& name,
        /* [out] */ IBinder** binder);

    CARAPI GrantUriPermissionFromOwner(
        /* [in] */ IBinder* owner,
        /* [in] */ Int32 fromUid,
        /* [in] */ const String& targetPkg,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 mode);

    CARAPI RevokeUriPermissionFromOwner(
        /* [in] */ IBinder* owner,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 mode);

    CARAPI CheckGrantUriPermission(
        /* [in] */ Int32 callingUid,
        /* [in] */ const String& targetPkg,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [out] */ Int32* result);

    // Cause the specified process to dump the specified heap.
    CARAPI DumpHeap(
        /* [in] */ const String& process,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean managed,
        /* [in] */ const String& path,
        /* [in] */ IParcelFileDescriptor* fd,
        /* [out] */ Boolean* result);

    CARAPI StartActivities(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& callingPackage,
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ ArrayOf<String>* resolvedTypes,
        /* [in] */ IBinder* resultTo,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI GetFrontActivityScreenCompatMode(
        /* [out] */ Int32* mode);

    CARAPI SetFrontActivityScreenCompatMode(
        /* [in] */ Int32 mode);

    CARAPI GetPackageScreenCompatMode(
        /* [in] */ const String& packageName,
        /* [out] */ Int32* mode);

    CARAPI SetPackageScreenCompatMode(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 mode);

    CARAPI GetPackageAskScreenCompat(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI SetPackageAskScreenCompat(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean ask);

    // Multi-user APIs
    CARAPI SwitchUser(
        /* [in] */ Int32 userid,
        /* [out] */ Boolean* swith);

    CARAPI StopUser(
        /* [in] */ Int32 userid,
        /* [in] */ IStopUserCallback* cb,
        /* [out] */ Int32* result);

    CARAPI GetCurrentUser(
        /* [out] */ IUserInfo** user);

    CARAPI IsUserRunning(
        /* [in] */ Int32 userid,
        /* [in] */ Boolean orStopping,
        /* [out] */ Boolean* running);

    CARAPI GetRunningUserIds(
        /* [out, callee] */ ArrayOf<Int32>** ids);

    CARAPI RemoveSubTask(
        /* [in] */ Int32 taskId,
        /* [in] */ Int32 subTaskIndex,
        /* [out] */ Boolean* removed);

    CARAPI RemoveTask(
        /* [in] */ Int32 taskId,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* removed);

    CARAPI RegisterProcessObserver(
        /* [in] */ IProcessObserver* observer);

    CARAPI UnregisterProcessObserver(
        /* [in] */ IProcessObserver* observer);

    CARAPI IsIntentSenderTargetedToPackage(
        /* [in] */ IIIntentSender* sender,
        /* [out] */ Boolean* result);

    CARAPI IsIntentSenderAnActivity(
        /* [in] */ IIIntentSender* sender,
        /* [out] */ Boolean* result);

    CARAPI UpdatePersistentConfiguration(
        /* [in] */ IConfiguration* values);

    CARAPI GetProcessPss(
        /* [in] */ ArrayOf<Int32>* pids,
        /* [out, callee] */ ArrayOf<Int64>** processPss);

    CARAPI ShowBootMessage(
        /* [in] */ ICharSequence* msg,
        /* [in] */ Boolean always);

    CARAPI DismissKeyguardOnNextActivity();

    CARAPI TargetTaskAffinityMatchesActivity(
        /* [in] */ IBinder* token,
        /* [in] */ const String& destAffinity,
        /* [out] */ Boolean* result);

    CARAPI NavigateUpTo(
        /* [in] */ IBinder* token,
        /* [in] */ IIntent* target,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* resultData,
        /* [out] */ Boolean* result);

    // This is not public because you need to be very careful in how you
    // manage your activity to make sure it is always the uid you expect.
    CARAPI GetLaunchedFromUid(
        /* [in] */ IBinder* activityToken,
        /* [out] */ Int32* result);

    CARAPI RegisterUserSwitchObserver(
        /* [in] */ IUserSwitchObserver* observer);

    CARAPI UnregisterUserSwitchObserver(
        /* [in] */ IUserSwitchObserver* observer);

    CARAPI RequestBugReport();

    CARAPI InputDispatchingTimedOut(
        /* [in] */ Int32 pid,
        /* [in] */ Boolean aboveSystem,
        /* [out] */ Int64* value);

    CARAPI TestIsSystemReady(
        /* [out] */ Boolean* isSystemReady);

public:
    CARAPI_(AutoPtr<BroadcastQueue>) BroadcastQueueForIntent(
        /* [in] */ IIntent* intent);

    CARAPI_(AutoPtr<BroadcastRecord>) BroadcastRecordForReceiverLocked(
        /* [in] */ IBinder* receiver);

    CARAPI UpdateCpuStats();

    CARAPI UpdateCpuStatsNow();

    CARAPI BatteryNeedsCpuUpdate();

    CARAPI BatteryPowerChanged(
        /* [in] */ Boolean onBattery);

    CARAPI SetFocusedActivityLocked(
        /* [in] */ ActivityRecord* r);

    CARAPI_(void) ClearFocusedActivity(
        /* [in] */ ActivityRecord* r);

    CARAPI_(void) ApplyUpdateLockStateLocked(
        /* [in] */ ActivityRecord* r);

    CARAPI_(void) ShowAskCompatModeDialogLocked(
        /* [in] */ ActivityRecord* r);

    CARAPI_(void) RemoveLruProcessLocked(
    /* [in] */ ProcessRecord* app);

    CARAPI_(void) UpdateLruProcessLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Boolean activityChange,
        /* [in] */ ProcessRecord* client);

    CARAPI_(AutoPtr<ProcessRecord>) GetProcessRecordLocked(
        /* [in] */ const String& processName,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean keepIfLarge);

    CARAPI EnsurePackageDexOpt(
        /* [in] */ const String& packageName);

    CARAPI_(Boolean) IsNextTransitionForward();

    CARAPI_(Int32) StartIsolatedProcess(
        /* [in] */ const String& entryPoint,
        /* [in] */ ArrayOf<String>* entryPointArgs,
        /* [in] */ const String& processName,
        /* [in] */ const String& abiOverride,
        /* [in] */ Int32 uid,
        /* [in] */ IRunnable* crashHandler);

    CARAPI_(AutoPtr<ProcessRecord>) StartProcessLocked(
        /* [in] */ const String& processName,
        /* [in] */ IApplicationInfo* info,
        /* [in] */ Boolean knownToBeDead,
        /* [in] */ Int32 intentFlags,
        /* [in] */ const String& hostingType,
        /* [in] */ IComponentName* hostingName,
        /* [in] */ Boolean allowWhileBooting,
        /* [in] */ Boolean isolated,
        /* [in] */ Boolean keepIfLarge);

    CARAPI_(AutoPtr<ProcessRecord>) StartProcessLocked(
        /* [in] */ const String& processName,
        /* [in] */ IApplicationInfo* info,
        /* [in] */ Boolean knownToBeDead,
        /* [in] */ Int32 intentFlags,
        /* [in] */ const String& hostingType,
        /* [in] */ IComponentName* hostingName,
        /* [in] */ Boolean allowWhileBooting,
        /* [in] */ Boolean isolated,
        /* [in] */ Int32 isolatedUid,
        /* [in] */ Boolean keepIfLarge,
        /* [in] */ const String& abiOverride,
        /* [in] */ const String& entryPoint,
        /* [in] */ ArrayOf<String>* entryPointArgs,
        /* [in] */ IRunnable* crashHandler);

    CARAPI_(Boolean) IsAllowedWhileBooting(
        /* [in] */ IApplicationInfo* ai);

    CARAPI UpdateUsageStats(
        /* [in] */ ActivityRecord* resumedComponent,
        /* [in] */ Boolean resumed);

    CARAPI_(AutoPtr<IIntent>) GetHomeIntent();

    CARAPI_(Boolean) StartHomeActivityLocked(
        /* [in] */ Int32 userId);

    CARAPI StartSetupActivityLocked();

    CARAPI_(AutoPtr<ICompatibilityInfo>) CompatibilityInfoForPackageLocked(
        /* [in] */ IApplicationInfo* ai);

    CARAPI EnforceNotIsolatedCaller(
        /* [in] */ const String& caller);

    CARAPI EnforceShellRestriction(
        /* [in] */ const String& restriction,
        /* [in] */ Int32 userHandle);

    CARAPI AddRecentTaskLocked(
        /* [in] */ TaskRecord* task);

    /**
     * If needed, remove oldest existing entries in recents that are for the same kind
     * of task as the given one.
     */
    CARAPI_(Int32) TrimRecentsForTask(
        /* [in] */ TaskRecord* task,
        /* [in] */ Boolean doTrim);

    CARAPI_(AutoPtr<ProcessRecord>) GetRecordForAppLocked(
        /* [in] */ IApplicationThread* thread);

    CARAPI_(void) DoLowMemReportIfNeededLocked(
        /* [in] */ ProcessRecord* dyingProc);

    CARAPI AppDiedLocked(
        /* [in] */ ProcessRecord* app);

    CARAPI AppDiedLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Int32 pid,
        /* [in] */ IApplicationThread* thread);

    /**
     * If a stack trace dump file is configured, dump process stack traces.
     * @param clearTraces causes the dump file to be erased prior to the new
     *    traces being written, if true; when false, the new traces will be
     *    appended to any existing file content.
     * @param firstPids of dalvik VM processes to dump stack traces for first
     * @param lastPids of dalvik VM processes to dump stack traces for last
     * @param nativeProcs optional list of native process names to dump stack crawls
     * @return file containing stack traces, or null if no dump file is configured
     */
    static CARAPI_(AutoPtr<IFile>) DumpStackTraces(
        /* [in] */ Boolean clearTraces,
        /* [in] */ List<Int32>* firstPids,
        /* [in] */ IProcessStats* processStats,
        /* [in] */ HashMap<Int32, Boolean>* lastPids,
        /* [in] */ ArrayOf<String>* nativeProcs);

    CARAPI LogAppTooSlow(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Int64 startTime,
        /* [in] */ const String& msg);

    CARAPI AppNotResponding(
        /* [in] */ ProcessRecord* app,
        /* [in] */ ActivityRecord* activity,
        /* [in] */ ActivityRecord* parent,
        /* [in] */ Boolean aboveSystem,
        /* [in] */ const String& annotation);

    CARAPI ShowLaunchWarningLocked(
        /* [in] */ ActivityRecord* cur,
        /* [in] */ ActivityRecord* next);

    CARAPI CloseSystemDialogsLocked(
        /* [in] */ const String& reason);

    CARAPI EnableScreenAfterBoot();

    CARAPI FinishBooting();

    CARAPI SendBootFastComplete();

    CARAPI EnsureBootCompleted();

    CARAPI GetIntentSenderLocked(
        /* [in] */ Int32 type,
        /* [in] */ const String& capsuleName,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 userId,
        /* [in] */ IBinder* token,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ ArrayOf<String>* resolvedTypes,
        /* [in] */ Int32 flags,
        /* [in] */ IBundle* options,
        /* [out] */ IIIntentSender** sender);

    CARAPI CancelIntentSenderLocked(
        /* [in] */ IIIntentSender* rec,
        /* [in] */ Boolean cleanActivity);

    CARAPI ForegroundTokenDied(
        /* [in] */ ForegroundToken* token);

    /**
     * This can be called with or without the global lock held.
     */
    CARAPI_(Int32) CheckComponentPermission(
        /* [in] */ const String& permission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 owningUid,
        /* [in] */ Boolean exported);

    /**
     * Binder IPC calls go through the public entry point.
     * This can be called with or without the global lock held.
     */
    CARAPI_(Int32) CheckCallingPermission(
        /* [in] */ const String& permission);

    /**
     * This can be called with or without the global lock held.
     */
    CARAPI EnforceCallingPermission(
        /* [in] */ const String& permission,
        /* [in] */ const String& func);

    CARAPI GrantUriPermissionUncheckedLocked(
        /* [in] */ Int32 targetUid,
        /* [in] */ const String& targetPkg,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [in] */ UriPermissionOwner* owner);

    CARAPI GrantUriPermissionLocked(
        /* [in] */ Int32 callingUid,
        /* [in] */ const String& targetPkg,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [in] */ UriPermissionOwner* owner);

    CARAPI_(AutoPtr<NeededUriGrants>) CheckGrantUriPermissionFromIntentLocked(
        /* [in] */ Int32 callingUid,
        /* [in] */ const String& targetPkg,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 mode,
        /* [in] */ NeededUriGrants* needed);

    CARAPI GrantUriPermissionUncheckedFromIntentLocked(
        /* [in] */ NeededUriGrants* needed,
        /* [in] */ UriPermissionOwner* owner);

    CARAPI GrantUriPermissionFromIntentLocked(
        /* [in] */ Int32 callingUid,
        /* [in] */ const String& targetPkg,
        /* [in] */ IIntent* intent,
        /* [in] */ UriPermissionOwner* owner);

    CARAPI RemoveUriPermissionIfNeededLocked(
        /* [in] */ UriPermission* perm);

    CARAPI_(Int32) GetTaskForActivityLocked(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean onlyRoot);

    CARAPI SendPendingThumbnail(
        /* [in] */ ActivityRecord* r,
        /* [in] */ IBinder* token,
        /* [in] */ IBitmap* thumbnail,
        /* [in] */ ICharSequence* description,
        /* [in] */ Boolean always);

    CARAPI GenerateApplicationProvidersLocked(
        /* [in] */ ProcessRecord* app,
        /* [out] */ IObjectContainer** providers);

    CARAPI_(AutoPtr<CContentProviderConnection>) IncProviderCountLocked(
        /* [in] */ ProcessRecord* r,
        /* [in] */ ContentProviderRecord* cpr,
        /* [in] */ IBinder* externalProcessToken,
        /* [in] */ Boolean stable);

    CARAPI_(Boolean) DecProviderCountLocked(
        /* [in] */ CContentProviderConnection* conn,
        /* [in] */ ContentProviderRecord* cpr,
        /* [in] */ IBinder* externalProcessToken,
        /* [in] */ Boolean stable);

    static CARAPI InstallSystemProviders();

    CARAPI_(AutoPtr<ProcessRecord>) NewProcessRecordLocked(
        /* [in] */ IApplicationThread* thread,
        /* [in] */ IApplicationInfo* info,
        /* [in] */ const String& customProcess,
        /* [in] */ Boolean isolated);

    CARAPI_(AutoPtr<ProcessRecord>) AddAppLocked(
        /* [in] */ IApplicationInfo* info,
        /* [in] */ Boolean isolated);

    CARAPI_(Boolean) IsSleeping();

    CARAPI_(Boolean) CheckAppSwitchAllowedLocked(
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid,
        /* [in] */ const String& name);

    CARAPI SetOpenGlTraceApp(
        /* [in] */ IApplicationInfo* app,
        /* [in] */ const String& processName);

    CARAPI SetProfileApp(
        /* [in] */ IApplicationInfo* app,
        /* [in] */ const String& processName,
        /* [in] */ const String& profileFile,
        /* [in] */ IParcelFileDescriptor* profileFd,
        /* [in] */ Boolean autoStopProfiler);

    CARAPI ShowSafeModeOverlay();

    CARAPI_(Boolean) TestIsSystemReady();

    CARAPI SystemReady(
        /* [in] */ IRunnable* goingCallback);

    CARAPI KillAppAtUsersRequest(
        /* [in] */ ProcessRecord* app,
        /* [in] */ IDialog* fromDialog);

    CARAPI StartAppProblemLocked(
        /* [in] */ ProcessRecord* app);

    CARAPI SkipCurrentReceiverLocked(
        /* [in] */ ProcessRecord* app);

    CARAPI AddErrorToDropBox(
        /* [in] */ const String& eventType,
        /* [in] */ ProcessRecord* process,
        /* [in] */ const String& processName,
        /* [in] */ ActivityRecord* activity,
        /* [in] */ ActivityRecord* parent,
        /* [in] */ const String& subject,
        /* [in] */ const String& report,
        /* [in] */ IFile* logFile,
        /* [in] */ IApplicationErrorReportCrashInfo* crashInfo);

    CARAPI_(AutoPtr<IIntent>) CreateAppErrorIntentLocked(
        /* [in] */ ProcessRecord* r,
        /* [in] */ Int64 timeMillis,
        /* [in] */ IApplicationErrorReportCrashInfo* crashInfo);

    static CARAPI_(Int32) OomAdjToImportance(
        /* [in] */ Int32 adj,
        /* [in] */ IActivityManagerRunningAppProcessInfo* currApp);

    CARAPI_(Boolean) DumpActivitiesLocked(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int32 opti,
        /* [in] */ Boolean dumpAll,
        /* [in] */ Boolean dumpClient,
        /* [in] */ const String& dumpPackage);

    CARAPI_(Boolean) DumpProcessesLocked(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int32 opti,
        /* [in] */ Boolean dumpAll,
        /* [in] */ const String& dumpPackage);

    CARAPI_(Boolean) DumpProcessesToGc(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int32 opti,
        /* [in] */ Boolean needSep,
        /* [in] */ Boolean dumpAll,
        /* [in] */ const String& dumpPackage);

    CARAPI_(Boolean) DumpOomLocked(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int32 opti,
        /* [in] */ Boolean dumpAll);

    CARAPI_(Boolean) DumpBroadcastsLocked(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int32 opti,
        /* [in] */ Boolean dumpAll,
        /* [in] */ const String& dumpPackage);

    CARAPI_(Boolean) DumpProvidersLocked(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int32 opti,
        /* [in] */ Boolean dumpAll,
        /* [in] */ const String& dumpPackage);

    CARAPI_(Boolean) DumpPendingIntentsLocked(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int32 opti,
        /* [in] */ Boolean dumpAll,
        /* [in] */ const String& dumpPackage);

    CARAPI DumpGraphicsHardwareUsage(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI DumpDbInfo(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    static CARAPI DumpMemItems(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ List< AutoPtr<MemItem> >* items,
        /* [in] */ Boolean sort);

    static CARAPI AppendMemBucket(
        /* [in] */ StringBuilder& out,
        /* [in] */ Int64 memKB,
        /* [in] */ const String& label,
        /* [in] */ Boolean stackLike);

    CARAPI DumpApplicationMemoryUsage(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Boolean brief,
        /* [in] */ IPrintWriter* categoryPw,
        /* [in] */ StringBuilder& outTag,
        /* [in] */ StringBuilder& outStack);

    CARAPI Monitor();

    CARAPI OnCoreSettingsChange(
        /* [in] */ IBundle* settings);

    CARAPI SendUserSwitchBroadcastsLocked(
        /* [in] */ Int32 oldUserId,
        /* [in] */ Int32 newUserId);

    CARAPI DispatchUserSwitch(
        /* [in] */ UserStartedState* uss,
        /* [in] */ Int32 oldUserId,
        /* [in] */ Int32 newUserId);

    CARAPI TimeoutUserSwitch(
        /* [in] */ UserStartedState* uss,
        /* [in] */ Int32 oldUserId,
        /* [in] */ Int32 newUserId);

    CARAPI SendContinueUserSwitchLocked(
        /* [in] */ UserStartedState* uss,
        /* [in] */ Int32 oldUserId,
        /* [in] */ Int32 newUserId);

    CARAPI UserInitialized(
        /* [in] */ UserStartedState* uss,
        /* [in] */ Int32 newUserId);

    CARAPI ContinueUserSwitch(
        /* [in] */ UserStartedState* uss,
        /* [in] */ Int32 oldUserId,
        /* [in] */ Int32 newUserId);

    CARAPI CompleteSwitchAndInitalize(
        /* [in] */ UserStartedState* uss,
        /* [in] */ Int32 newUserId,
        /* [in] */ Boolean clearInitializing,
        /* [in] */ Boolean clearSwitching);

    CARAPI FinishUserSwitch(
        /* [in] */ UserStartedState* uss);

    CARAPI FinishUserStop(
        /* [in] */ UserStartedState* uss);

    CARAPI_(Int32) GetCurrentUserIdLocked();

    CARAPI_(Boolean) IsUserRunningLocked(
        /* [in] */ Int32 userId,
        /* [in] */ Boolean orStopped);

    CARAPI_(AutoPtr< ArrayOf<Int32> >) GetUsersLocked();

    CARAPI_(AutoPtr<CUserManagerService>) GetUserManagerLocked();

    CARAPI_(AutoPtr<IApplicationInfo>) GetAppInfoForUser(
        /* [in] */ IApplicationInfo* aInfo,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<IActivityInfo>) GetActivityInfoForUser(
        /* [in] */ IActivityInfo* aInfo,
        /* [in] */ Int32 userId);

    CARAPI StartActivityFromRecentsInner(
        /* [in] */ Int32 taskId,
        /* [in] */ IBundle* options,
        /* [out] */ Int32* result);

    CARAPI StartActivityInPackage(
        /* [in] */ Int32 uid,
        /* [in] */ const String& callingPackage,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IBinder* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 startFlags,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [in] */ IActivityContainer* container,
        /* [in] */ TaskRecord* inTask
        /* [out] */ Int32* result);

    CARAPI StartActivitiesInPackage(
        /* [in] */ Int32 uid,
        /* [in] */ const String& callingPackage,
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ ArrayOf<String>* resolvedTypes,
        /* [in] */ IBinder* resultTo,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* status);

    /**
     * Update the recent tasks lists: make sure tasks should still be here (their
     * applications / activities still exist), update their availability, fixup ordering
     * of affiliations.
     */
    CARAPI_(void) CleanupRecentTasksLocked(
        /* [in] */ Int32 userId);

    CARAPI StartServiceInPackage(
        /* [in] */ Int32 uid,
        /* [in] */ IIntent* service,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 userId,
        /* [out] */ IComponentName** componentName);

    CARAPI BroadcastIntentInPackage(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IIntentReceiver* resultTo,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& resultData,
        /* [in] */ IBundle* map,
        /* [in] */ const String& requiredPermission,
        /* [in] */ Boolean serialized,
        /* [in] */ Boolean sticky,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* status);

    CARAPI_(Boolean) UpdateConfigurationLocked(
        /* [in] */ IConfiguration* values,
        /* [in] */ ActivityRecord* starting,
        /* [in] */ Boolean persistent,
        /* [in] */ Boolean initLocale);

    CARAPI_(Boolean) IsSingleton(
        /* [in] */ const String& componentProcessName,
        /* [in] */ IApplicationInfo* aInfo,
        /* [in] */ const String& className,
        /* [in] */ Int32 flags);

    /**
     * Schedule the execution of all pending app GCs.
     */
    CARAPI ScheduleAppGcsLocked();

    CARAPI_(Boolean) UpdateOomAdjLocked(
        /* [in] */ ProcessRecord* app);

    CARAPI UpdateOomAdjLocked();

    CARAPI TrimApplications();

    CARAPI_(void) HandleShowErrorMsg(
        /* [in] */ StringObjectHashMap* data);

    CARAPI_(void) HandleShowNotRespondingMsg(
        /* [in] */ StringObjectHashMap* data,
        /* [in] */ Int32 arg1);

    CARAPI_(void) HandleShowStrictModeViolationMsg(
        /* [in] */ StringObjectHashMap* map);

    CARAPI_(void) HandleShowFactoryErrorMsg(
        /* [in] */ ICharSequence* msg);

    CARAPI_(void) HandleUpdateConfigurationMsg(
        /* [in] */ IConfiguration* config);

    CARAPI_(void) HandleGCBackgroundProcessesMsg();

    CARAPI_(void) HandleWaitForDebuggerMsg(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Int32 arg1);

    CARAPI_(void) HandleServiceTimeoutMsg(
        /* [in] */ ProcessRecord* processRecord);

    CARAPI_(void) HandleUpdateTimeZone();

    CARAPI_(void) HandleClearDnsCache();

    CARAPI_(void) HandleUpdateHttpProxy(
        /* [in] */ IProxyInfo* proxy);

    CARAPI_(void) HandleShowUidErrorMsg();

    CARAPI_(void) HandleIMFeelingLuckyMsg();

    CARAPI_(void) HandleProcStartTimeoutMsg(
        /* [in] */ ProcessRecord* pr);

    CARAPI_(void) HandleDoPendingActivityLaunchesMsg();

    CARAPI_(void) HandleKillApplicationMsg(
        /* [in] */ Int32 appid,
        /* [in] */ Boolean restart,
        /* [in] */ IBundle* bundle);

    CARAPI_(void) HandlePostHeavyNotificationMsg(
        /* [in] */ ActivityRecord* activityRecord);

    CARAPI_(void) HandleCancelHeavyNotificationMsg(
        /* [in] */ Int32 id);

    CARAPI_(void) HandleCheckExcessiveWakeLocksMsg();

    CARAPI_(void) HandleShowCompatModeDialogMsg(
        /* [in] */ ActivityRecord* activityRecord);

    CARAPI_(void) HandleReportMemUsageMsg(
        /* [in] */ IList* memInfos);

    CARAPI_(void) HandleCollectPssBgMsg();

private:
    CARAPI_(void) Start();

    /**
     * Initialize the application bind args. These are passed to each
     * process when the bindApplication() IPC is sent to the process. They're
     * lazily setup to make sure the services are running when they're asked for.
     */
    CARAPI_(AutoPtr<IHashMap>) GetCommonServicesLocked();

    CARAPI_(Int32) UpdateLruProcessInternalLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Int64 now,
        /* [in] */ Int32 index,
        /* [in] */ const String& what,
        /* [in] */ IInterface* obj,
        /* [in] */ ProcessRecord* srcApp);

    CARAPI StartProcessLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ const String& hostingType,
        /* [in] */ const String& hostingNameStr);

    CARAPI StartProcessLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ const String& hostingType,
        /* [in] */ const String& hostingNameStr,
        /* [in] */ const String& abiOverride,
        /* [in] */ const String& entryPoint,
        /* [in] */ ArrayOf<String>* entryPointArgs);

    CARAPI_(AutoPtr<IActivityInfo>) ResolveActivityInfo(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId);

    CARAPI_(void) RemoveRecentTasksForUserLocked(
        /* [in] */ Int32 userId);

    // Sort by taskId
    CARAPI_(Int32) TaskRecordCompare(
        /* [in] */ TaskRecord* lhs,
        /* [in] */ TaskRecord* rhs);

    // Extract the affiliates of the chain containing mRecentTasks[start].
    CARAPI_(Int32) ProcessNextAffiliateChain(
        /* [in] */ Int32 start);

    CARAPI_(Boolean) MoveAffiliatedTasksToFront(
        /* [in] */ TaskRecord* task,
        /* [in] */ Int32 taskIndex);

    CARAPI DispatchProcessesChanged();

    CARAPI DispatchProcessDied(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid);

    /// patch for videoplayer crashed

    /**
     * Main function for removing an existing process from the activity manager
     * as a result of that process going away.  Clears out all connections
     * to the process.
     */
    CARAPI HandleAppDiedLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Boolean restarting,
        /* [in] */ Boolean allowRestart);

    CARAPI_(Int32) GetLRURecordIndexForAppLocked(
        /* [in] */ IApplicationThread* thread);

    static CARAPI_(void) DumpStackTraces(
        /* [in] */ const String& tracesPath,
        /* [in] */ List<Int32>* firstPids,
        /* [in] */ IProcessStats* processStats,
        /* [in] */ HashMap<Int32, Boolean>* lastPids,
        /* [in] */ ArrayOf<String>* nativeProcs);

    CARAPI ForceStopPackageLocked(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [in] */ const String& reason);

    CARAPI ForceStopUserLocked(
        /* [in] */ Int32 userId,
        /* [in] */ const String& reason);

    CARAPI_(Boolean) KillPackageProcessesLocked(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 appId,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 minOomAdj,
        /* [in] */ Boolean callerWillRestart,
        /* [in] */ Boolean allowRestart,
        /* [in] */ Boolean doit,
        /* [in] */ Boolean evenPersistent,
        /* [in] */ const String& reason);

    CARAPI_(Boolean) ForceStopPackageLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 appId,
        /* [in] */ Boolean callerWillRestart,
        /* [in] */ Boolean purgeCache,
        /* [in] */ Boolean doit,
        /* [in] */ Boolean evenPersistent,
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) RemoveProcessLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Boolean callerWillRestart,
        /* [in] */ Boolean allowRestart,
        /* [in] */ const String& reason);

    CARAPI ProcessStartTimedOutLocked(
        /* [in] */ ProcessRecord* app);

    CARAPI_(Boolean) AttachApplicationLocked(
        /* [in] */ IApplicationThread* thread,
        /* [in] */ Int32 pid);

    CARAPI_(AutoPtr<ActivityRecord>) GetCallingRecordLocked(
        /* [in] */ IBinder* token);

    CARAPI_(Boolean) CheckHoldingPermissionsLocked(
        /* [in] */ IIPackageManager* pm,
        /* [in] */ IProviderInfo* pi,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 modeFlags);

    CARAPI_(Boolean) CheckUriPermissionLocked(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 modeFlags);

    CARAPI CheckGrantUriPermissionLocked(
        /* [in] */ Int32 callingUid,
        /* [in] */ const String& targetPkg,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [in] */ Int32 lastTargetUid,
        /* [out] */ Int32* permission);

    CARAPI RevokeUriPermissionLocked(
        /* [in] */ Int32 callingUid,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags);

    CARAPI_(AutoPtr<TaskRecord>) TaskForIdLocked(
        /* [in] */ Int32 id);

    CARAPI CleanUpRemovedTaskLocked(
        /* [in] */ TaskRecord* tr,
        /* [in] */ Int32 flags);

    CARAPI_(Int32) FindAffinityTaskTopLocked(
        /* [in] */ Int32 startIndex,
        /* [in] */ const String& affinity);

    CARAPI MoveTaskBackwardsLocked(
        /* [in] */ Int32 task);

    CARAPI_(String) CheckContentProviderPermissionLocked(
        /* [in] */ IProviderInfo* cpi,
        /* [in] */ ProcessRecord* r);

    CARAPI GetContentProviderImpl(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& name,
        /* [in] */ IBinder* token,
        /* [in] */ Boolean stable,
        /* [in] */ Int32 userId,
        /* [out] */ IContentProviderHolder** providerHolder);

    CARAPI GetContentProviderExternalUnchecked(
        /* [in] */ const String& name,
        /* [in] */ IBinder* token,
        /* [in] */ Int32 userId,
        /* [out] */ IContentProviderHolder** providerHolder);

    CARAPI RemoveContentProviderExternalUnchecked(
        /* [in] */ const String& name,
        /* [in] */ IBinder* token,
        /* [in] */ Int32 userId);

    CARAPI ComeOutOfSleepIfNeededLocked();

    CARAPI UpdateEventDispatchingLocked();

    CARAPI_(Boolean) KillProcessesBelowAdj(
        /* [in] */ Int32 belowAdj,
        /* [in] */ const String& reason);

    CARAPI RetrieveSettings();

    static CARAPI_(AutoPtr<IFile>) GetCalledPreBootReceiversFile();

    static CARAPI_(AutoPtr<List< AutoPtr<IComponentName> > >) ReadLastDonePreBootReceivers();

    static CARAPI WriteLastDonePreBootReceivers(
        /* [in] */ const List< AutoPtr<IComponentName> >* list);

    CARAPI_(Boolean) MakeAppCrashingLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ const String& shortMsg,
        /* [in] */ const String& longMsg,
        /* [in] */ const String& stackTrace);

    CARAPI MakeAppNotRespondingLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ const String& activity,
        /* [in] */ const String& shortMsg,
        /* [in] */ const String& longMsg);

    CARAPI_(AutoPtr<IActivityManagerProcessErrorStateInfo>) GenerateProcessError(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Int32 condition,
        /* [in] */ const String& activity,
        /* [in] */ const String& shortMsg,
        /* [in] */ const String& longMsg,
        /* [in] */ const String& stackTrace);

    CARAPI_(Boolean) HandleAppCrashLocked(
        /* [in] */ ProcessRecord* app);

    CARAPI LogStrictModeViolationToDropBox(
        /* [in] */ ProcessRecord* process,
        /* [in] */ IStrictModeViolationInfo* info);

    /**
     * @param app object of some object (as stored in {@link com.android.internal.os.RuntimeInit})
     * @return the corresponding {@link ProcessRecord} object, or null if none could be found
     */
    CARAPI_(AutoPtr<ProcessRecord>) FindAppProcess(
        /* [in] */ IBinder* app,
        /* [in] */ const String& reason);

    /**
     * Utility function for addErrorToDropBox and handleStrictModeViolation's logging
     * to append various headers to the dropbox log text.
     */
    CARAPI AppendDropBoxProcessHeaders(
        /* [in] */ ProcessRecord* process,
        /* [in] */ const String& processName,
        /* [in] */ StringBuilder& sb);

    static CARAPI_(String) ProcessClass(
        /* [in] */ ProcessRecord* process);

    CARAPI CrashApplication(
        /* [in] */ ProcessRecord* r,
        /* [in] */ IApplicationErrorReportCrashInfo* crashInfo);

    CARAPI_(AutoPtr<IApplicationErrorReport>) CreateAppErrorReportLocked(
        /* [in] */ ProcessRecord* r,
        /* [in] */ Int64 timeMillis,
        /* [in] */ IApplicationErrorReportCrashInfo* crashInfo);

    CARAPI FillInProcMemInfo(
        /* [in] */ ProcessRecord* app,
        /* [in] */ IActivityManagerRunningAppProcessInfo* outInfo);

    static CARAPI DumpHistoryList(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ List< AutoPtr<ActivityRecord> >* list,
        /* [in] */ const String& prefix,
        /* [in] */ const String& label,
        /* [in] */ Boolean complete,
        /* [in] */ Boolean brief,
        /* [in] */ Boolean client,
        /* [in] */ const String& dumpPackage);

    static CARAPI_(String) BuildOomTag(
        /* [in] */ const String& prefix,
        /* [in] */ const String& space,
        /* [in] */ Int32 val,
        /* [in] */ Int32 base);

    static CARAPI_(Int32) DumpProcessList(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ CActivityManagerService* service,
        /* [in] */ List< AutoPtr<ProcessRecord> >* list,
        /* [in] */ const String& prefix,
        /* [in] */ const String& normalLabel,
        /* [in] */ const String& persistentLabel,
        /* [in] */ const String& dumpPackage);

    static CARAPI_(Boolean) DumpProcessOomList(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ CActivityManagerService* service,
        /* [in] */ List< AutoPtr<ProcessRecord> >* origList,
        /* [in] */ const String& prefix,
        /* [in] */ const String& normalLabel,
        /* [in] */ const String& persistentLabel,
        /* [in] */ Boolean inclDetails,
        /* [in] */ const String& dumpPackage);

    CARAPI_(AutoPtr< List< AutoPtr<ProcessRecord> > >) CollectProcesses(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 start,
        /* [in] */ ArrayOf<String>* args);

    CARAPI_(Boolean) CheckAppInLaunchingProvidersLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Boolean alwaysBad);

    CARAPI_(Boolean) IsPendingBroadcastProcessLocked(
        /* [in] */ Int32 pid);

    CARAPI_(void) SkipPendingBroadcastLocked(
        /* [in] */ Int32 pid);

    CARAPI_(Boolean) SendPendingBroadcastsLocked(
        /* [in] */ ProcessRecord* app);

    CARAPI_(void) RemoveReceiverLocked(
        /* [in] */ ReceiverList* rl);

    CARAPI VerifyBroadcastLocked(
        /* [in] */ IIntent* _intent,
        /* [out] */ IIntent** intent);

    CARAPI FinishInstrumentationLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

    CARAPI PerformAppGcLocked(
        /* [in] */ ProcessRecord* app);

    /**
     * Returns true if things are idle enough to perform GCs.
     */
    CARAPI_(Boolean) CanGcNowLocked();

    /**
     * Perform GCs on all processes that are waiting for it, but only
     * if things are idle.
     */
    CARAPI PerformAppGcsLocked();

    /**
     * If all looks good, perform GCs on all processes waiting for them.
     */
    CARAPI PerformAppGcsIfAppropriateLocked();

    CARAPI AddProcessToGcListLocked(
        /* [in] */ ProcessRecord* proc);

    CARAPI ScheduleAppGcLocked(
        /* [in] */ ProcessRecord* app);

    CARAPI CheckExcessivePowerUsageLocked(
        /* [in] */ Boolean doKills);

protected:
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI_(Boolean) DumpProvider(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int32 opti,
        /* [in] */ Boolean dumpAll);

    CARAPI_(Boolean) DumpActivity(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int32 opti,
        /* [in] */ Boolean dumpAll);

    CARAPI DumpActivity(
        /* [in] */ const String& prefix,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ActivityRecord* r,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Boolean dumpAll);

    static CARAPI_(Boolean) ScanArgs(
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ const String& value);

    CARAPI_(Boolean) RemoveDyingProviderLocked(
        /* [in] */ ProcessRecord* proc,
        /* [in] */ ContentProviderRecord* cpr,
        /* [in] */ Boolean always);

    ///**
    // * Main code for cleaning up a process when it has gone away.  This is
    // * called both as a result of the process dying, or directly when stopping
    // * a process when running in single process mode.
    // */
    CARAPI CleanUpApplicationRecordLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Boolean restarting,
        /* [in] */ Boolean allowRestart,
        /* [in] */ Int32 index);

    CARAPI_(AutoPtr< List< AutoPtr<IIntent> > >) GetStickiesLocked(
        /* [in] */ const String& action,
        /* [in] */ IIntentFilter* filter,
        /* [in] */ List< AutoPtr<IIntent> >* cur,
        /* [in] */ Int32 userId);

    CARAPI_(void) SendPackageBroadcastLocked(
        /* [in] */ Int32 cmd,
        /* [in] */ ArrayOf<String>* capsules,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<IList>) CollectReceiverComponents(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ ArrayOf<Int32>* users);

    CARAPI BroadcastIntentLocked(
        /* [in] */ ProcessRecord* callerApp,
        /* [in] */ const String& callerPackage,
        /* [in] */ IIntent *intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IIntentReceiver* resultTo,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& resultData,
        /* [in] */ IBundle* map,
        /* [in] */ const String& requiredPermission,
        /* [in] */ Boolean ordered,
        /* [in] */ Boolean sticky,
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI_(Boolean) FinishReceiverLocked(
        /* [in] */ IBinder* receiver,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& resultData,
        /* [in] */ IBundle* resultExtras,
        /* [in] */ Boolean resultAbort,
        /* [in] */ Boolean _explicit);

    CARAPI ReportStartInstrumentationFailure(
        /* [in] */ IInstrumentationWatcher* watcher,
        /* [in] */ IComponentName* cn,
        /* [in] */ const String& report);

    static CARAPI_(Boolean) ShouldShowDialogs(
        /* [in] */ IConfiguration* config);

    CARAPI SaveLocaleLocked(
        /* [in] */ ILocale* l,
        /* [in] */ Boolean isDiff,
        /* [in] */ Boolean isPersist);

    CARAPI_(AutoPtr<BroadcastQueue>) IsReceivingBroadcast(
        /* [in] */ ProcessRecord* app);

    CARAPI_(Int32) ComputeOomAdjLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Int32 hiddenAdj,
        /* [in] */ Int32 clientHiddenAdj,
        /* [in] */ Int32 emptyAdj,
        /* [in] */ ProcessRecord* TOP_APP,
        /* [in] */ Boolean recursed,
        /* [in] */ Boolean doingAll);

    CARAPI_(Boolean) UpdateOomAdjLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Int32 hiddenAdj,
        /* [in] */ Int32 clientHiddenAdj,
        /* [in] */ Int32 emptyAdj,
        /* [in] */ ProcessRecord* TOP_APP,
        /* [in] */ Boolean doingAll);

    CARAPI_(AutoPtr<ActivityRecord>) ResumedAppLocked();

    CARAPI StopProfilerLocked(
        /* [in] */ ProcessRecord* proc,
        /* [in] */ const String& path,
        /* [in] */ Int32 profileType);

    CARAPI ClearProfilerLocked();

    CARAPI_(AutoPtr<ProcessRecord>) FindProcessLocked(
        /* [in] */ const String& process,
        /* [in] */ Int32 userId,
        /* [in] */ const String& callName);

    CARAPI_(Int32) StopUserLocked(
        /* [in] */ Int32 userId,
        /* [in] */ IStopUserCallback* callback);

    CARAPI UpdateStartedUserArrayLocked();

    CARAPI_(Boolean) UserExists(
        /* [in] */ Int32 userId);

    CARAPI CheckValidCaller(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 userId);

    CARAPI_(Int32) ApplyUserId(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 userId);

    static CARAPI_(Int32) UserHandleGetUserId(
        /* [in] */ Int32 uid);

    static CARAPI_(Int32) UserHandleGetAppId(
        /* [in] */ Int32 uid);

    static CARAPI_(Int32) UserHandleGetCallingUserId();

public:
    // File that stores last updated system version and called preboot receivers
    static const String CALLED_PRE_BOOTS_FILENAME;

    static const String TAG;
    static const String TAG_MU;
    static const Boolean DEBUG;
    static const Boolean localLOGV;
    static const Boolean DEBUG_BACKUP;
    static const Boolean DEBUG_BROADCAST;
    static const Boolean DEBUG_BROADCAST_LIGHT;
    static const Boolean DEBUG_BACKGROUND_BROADCAST;
    static const Boolean DEBUG_CLEANUP;
    static const Boolean DEBUG_CONFIGURATION;
    static const Boolean DEBUG_FOCUS;
    static const Boolean DEBUG_IMMERSIVE;
    static const Boolean DEBUG_MU;
    static const Boolean DEBUG_OOM_ADJ;
    static const Boolean DEBUG_LRU;
    static const Boolean DEBUG_PAUSE;
    static const Boolean DEBUG_POWER;
    static const Boolean DEBUG_POWER_QUICK;
    static const Boolean DEBUG_PROCESS_OBSERVERS;
    static const Boolean DEBUG_PROCESSES;
    static const Boolean DEBUG_PROVIDER;
    static const Boolean DEBUG_RESULTS;
    static const Boolean DEBUG_SERVICE;
    static const Boolean DEBUG_SERVICE_EXECUTING;
    static const Boolean DEBUG_STACK;
    static const Boolean DEBUG_SWITCH;
    static const Boolean DEBUG_TASKS;
    static const Boolean DEBUG_THUMBNAILS;
    static const Boolean DEBUG_TRANSITION;
    static const Boolean DEBUG_URI_PERMISSION;
    static const Boolean DEBUG_USER_LEAVING;
    static const Boolean DEBUG_VISBILITY;
    static const Boolean DEBUG_PSS;
    static const Boolean DEBUG_LOCKSCREEN;
    static const Boolean DEBUG_RECENTS;
    static const Boolean VALIDATE_TOKENS;
    static const Boolean SHOW_ACTIVITY_START_TIME;

    // Control over CPU and battery monitoring.
    static const Int64 BATTERY_STATS_TIME;      // write battery stats every 30 minutes.
    static const Boolean MONITOR_CPU_USAGE;
    static const Int64 MONITOR_CPU_MIN_TIME;        // don't sample cpu less than every 5 seconds.
    static const Int64 MONITOR_CPU_MAX_TIME;    // wait possibly forever for next cpu sample.
    static const Boolean MONITOR_THREAD_CPU_USAGE;

    // The flags that are set for all calls we make to the package manager.
    static const Int32 STOCK_PM_FLAGS;

    static const Boolean IS_USER_BUILD;

    // Maximum number recent bitmaps to keep in memory.
    static const Int32 MAX_RECENT_BITMAPS = 5;

    // Amount of time after a call to stopAppSwitches() during which we will
    // prevent further untrusted switches from happening.
    static const Int64 APP_SWITCH_DELAY_TIME = 5 * 1000;

    // How long we wait for a launched process to attach to the activity manager
    // before we decide it's never going to come up for real.
    static const Int32 PROC_START_TIMEOUT = 10 * 1000;

    // How long we wait for a launched process to attach to the activity manager
    // before we decide it's never going to come up for real, when the process was
    // started with a wrapper for instrumentation (such as Valgrind) because it
    // could take much longer than usual.
    static const Int32 PROC_START_TIMEOUT_WITH_WRAPPER = 1200 * 1000;

    // How long to wait after going idle before forcing apps to GC.
    static const Int32 GC_TIMEOUT = 5 * 1000;

    // The minimum amount of time between successive GC requests for a process.
    static const Int32 GC_MIN_INTERVAL = 60  *1000;

    // The minimum amount of time between successive PSS requests for a process.
    static const Int32 FULL_PSS_MIN_INTERVAL = 10*60*1000;

    // The minimum amount of time between successive PSS requests for a process
    // when the request is due to the memory state being lowered.
    static const Int32 FULL_PSS_LOWERED_INTERVAL = 2*60*1000;

    // The rate at which we check for apps using excessive power -- 15 mins.
    static const Int32 POWER_CHECK_DELAY;

    // The minimum sample duration we will allow before deciding we have
    // enough data on wake locks to start killing things.
    static const Int32 WAKE_LOCK_MIN_CHECK_DURATION;

    // The minimum sample duration we will allow before deciding we have
    // enough data on CPU usage to start killing things.
    static const Int32 CPU_MIN_CHECK_DURATION;

    // How long we allow a receiver to run before giving up on it.
    static const Int32 BROADCAST_FG_TIMEOUT;
    static const Int32 BROADCAST_BG_TIMEOUT;

    // How long we wait until we timeout on key dispatching.
    static const Int32 KEY_DISPATCHING_TIMEOUT;

    // How long we wait until we timeout on key dispatching during instrumentation.
    static const Int32 INSTRUMENTATION_KEY_DISPATCHING_TIMEOUT;

    // Amount of time we wait for observers to handle a user switch before
    // giving up on them and unfreezing the screen.
    static const Int32 USER_SWITCH_TIMEOUT;

    // Maximum number of users we allow to be running at a time.
    static const Int32 MAX_RUNNING_USERS;

    // How long to wait in getAssistContextExtras for the activity and foreground services
    // to respond with the result.
    static const Int32 PENDING_ASSIST_EXTRAS_TIMEOUT = 500;

    // Maximum number of persisted Uri grants a package is allowed
    static const Int32 MAX_PERSISTED_URI_GRANTS = 128;

    static const Int32 MY_PID;

    static const AutoPtr< ArrayOf<String> > EMPTY_STRING_ARRAY;

    // How many bytes to write into the dropbox log before truncating
    static const Int32 DROPBOX_MAX_SIZE = 256 * 1024;

    // Access modes for handleIncomingUser.
    static const Int32 ALLOW_NON_FULL = 0;
    static const Int32 ALLOW_NON_FULL_IN_PROFILE = 1;
    static const Int32 ALLOW_FULL_ONLY = 2;

    static const Int32 LAST_PREBOOT_DELIVERED_FILE_VERSION = 10000;

    /** All system services */
    AutoPtr<SystemServiceManager> mSystemServiceManager;

    /** Run all ActivityStacks through this */
    AutoPtr<ActivityStackSupervisor> mStackSupervisor;

    AutoPtr<IntentFirewall> mIntentFirewall;

    AutoPtr<BroadcastQueue> mFgBroadcastQueue;
    AutoPtr<BroadcastQueue> mBgBroadcastQueue;
    // Convenient for easy iteration over the queues. Foreground is first
    // so that dispatch of foreground broadcasts gets precedence.
    AutoPtr< ArrayOf<BroadcastQueue*> > mBroadcastQueues;// new BroadcastQueue[2];

    /**
     * Activity we have told the window manager to have key focus.
     */
    AutoPtr<ActivityRecord> mFocusedActivity;

    /**
     * List of intents that were used to start the most recent tasks.
     */
    AutoPtr<List<AutoPtr<TaskRecord> > > mRecentTasks;
    List<AutoPtr<TaskRecord> > mTmpRecents;

    /**
     * For addAppTask: cached of the last activity component that was added.
     */
    AutoPtr<IComponentName> mLastAddedTaskComponent;

    /**
     * For addAppTask: cached of the last activity uid that was added.
     */
    Int32 mLastAddedTaskUid;

    /**
     * For addAppTask: cached of the last ActivityInfo that was added.
     */
    AutoPtr<IActivityInfo> mLastAddedTaskActivity;

    List<AutoPtr<CPendingAssistExtras> > mPendingAssistExtras;

    /**
     * Process management.
     */
    AutoPtr<ProcessList> mProcessList;

    /**
     * All of the applications we currently have running organized by name.
     * The keys are strings of the application package name (as
     * returned by the package manager), and the keys are ApplicationRecord
     * objects.
     */
    AutoPtr< ProcessMap< AutoPtr<ProcessRecord> > > mProcessNames;

    /**
     * Tracking long-term execution of processes to look for abuse and other
     * bad app behavior.
     */
    AutoPtr<CProcessStatsService> mProcessStats;

    /**
     * The currently running isolated processes.
     */
    HashMap<Int32, AutoPtr<ProcessRecord> > mIsolatedProcesses;

    /**
     * Counter for assigning isolated process uids, to avoid frequently reusing the
     * same ones.
     */
    Int32 mNextIsolatedProcessUid;

    /**
     * The currently running heavy-weight process, if any.
     */
    AutoPtr<ProcessRecord> mHeavyWeightProcess;

    /**
     * The last time that various processes have crashed.
     */
    AutoPtr< ProcessMap<Int64> > mProcessCrashTimes;

    /**
     * Set of applications that we consider to be bad, and will reject
     * incoming broadcasts from (which the user has no control over).
     * Processes are added to this set when they have crashed twice within
     * a minimum amount of time; they are removed from it when they are
     * later restarted (hopefully due to some user action).  The value is the
     * time it was added to the list.
     */
    AutoPtr<ProcessMap<AutoPtr<BadProcessInfo> > > mBadProcesses;

    /**
     * All of the processes we currently have running organized by pid.
     * The keys are the pid running the application.
     *
     * <p>NOTE: This object is protected by its own lock, NOT the global
     * activity manager lock!
     */
    HashMap<Int32, AutoPtr<ProcessRecord> > mPidsSelfLocked;
    Object mPidsSelfLockedLock;

    HashMap<Int32, AutoPtr<ForegroundToken> > mForegroundProcesses;

    /**
     * List of records for processes that someone had tried to start before the
     * system was ready.  We don't start them at that point, but ensure they
     * are started by the time booting is complete.
     */
    List< AutoPtr<ProcessRecord> > mProcessesOnHold;

    /**
     * List of records for processes that we have started and are waiting
     * for them to call back.  This is really only needed when running in
     * single processes mode, in which case we do not have a unique pid for
     * each process.
     */
    List<AutoPtr<ProcessRecord> > mStartingProcesses;

    /**
     * List of persistent applications that are in the process
     * of being started.
     */
    List< AutoPtr<ProcessRecord> > mPersistentStartingProcesses;

    /**
     * Processes that are being forcibly torn down.
     */
    List< AutoPtr<ProcessRecord> > mRemovedProcesses;

    /**
     * List of running applications, sorted by recent usage.
     * The first entry in the list is the least recently used.
     */
    List< AutoPtr<ProcessRecord> > mLruProcesses;

    /**
     * Where in mLruProcesses that the processes hosting activities start.
     */
    Int32 mLruProcessActivityStart;

    /**
     * Where in mLruProcesses that the processes hosting services start.
     * This is after (lower index) than mLruProcessesActivityStart.
     */
    Int32 mLruProcessServiceStart;

    /**
     * List of processes that should gc as soon as things are idle.
     */
    List< AutoPtr<ProcessRecord> > mProcessesToGc;

    /**
     * Processes we want to collect PSS data from.
     */
    List<AutoPtr<ProcessRecord> > mPendingPssProcesses;

    /**
     * Last time we requested PSS data of all processes.
     */
    Int64 mLastFullPssTime;

    /**
     * If set, the next time we collect PSS data we should do a full collection
     * with data from native processes and the kernel.
     */
    Boolean mFullPssPending;

    /**
     * This is the process holding what we currently consider to be
     * the "home" activity.
     */
    AutoPtr<ProcessRecord> mHomeProcess;

    /**
     * This is the process holding the activity the user last visited that
     * is in a different process from the one they are currently in.
     */
    AutoPtr<ProcessRecord> mPreviousProcess;

    /**
     * The time at which the previous process was last visible.
     */
    Int64 mPreviousProcessVisibleTime;

    /**
     * Which uses have been started, so are allowed to run code.
     */
    HashMap<Int32, AutoPtr<UserStartedState> > mStartedUsers;

    /**
     * LRU list of history of current users.  Most recently current is at the end.
     */
    List<Int32> mUserLru;

    /**
     * Constant array of the users that are currently started.
     */
    AutoPtr< ArrayOf<Int32> > mStartedUserArray;

    /**
     * Registered observers of the user switching mechanics.
     */
    //AutoPtr< RemoteCallbackList<IUserSwitchObserver> > mUserSwitchObservers;
    AutoPtr<IRemoteCallbackList> mUserSwitchObservers;

    /**
     * Currently active user switch.
     */
    AutoPtr<IInterface> mCurUserSwitchCallback;

    /**
     * Packages that the user has asked to have run in screen size
     * compatibility mode instead of filling the screen.
     */
    AutoPtr<CompatModePackages> mCompatModePackages;

    /**
     * Set of IntentSenderRecord objects that are currently active.
     */
    typedef HashMap< AutoPtr<CPendingIntentRecord::Key>, AutoPtr<IWeakReference> > PendingIntentRecordHashMap;
    PendingIntentRecordHashMap mIntentSenderRecords;

     /**
     * Keeps track of all IIntentReceivers that have been registered for
     * broadcasts.  Hash keys are the receiver IBinder, hash value is
     * a ReceiverList.
     */
    HashMap<AutoPtr<IBinder>, AutoPtr<ReceiverList> > mRegisteredReceivers;

    /**
     * Resolver for broadcast intents to registered receivers.
     * Holds BroadcastFilter (subclass of IntentFilter).
     */
    AutoPtr<ReceiverResolver> mReceiverResolver;

    /**
     * State of all active sticky broadcasts per user.  Keys are the action of the
     * sticky Intent, values are an ArrayList of all broadcasted intents with
     * that action (which should usually be one).  The SparseArray is keyed
     * by the user ID the sticky is for, and can include UserHandle.USER_ALL
     * for stickies that are sent to all users.
     */
    typedef List< AutoPtr<IIntent> > StringIntentList;
    typedef HashMap<String, AutoPtr<StringIntentList> > StringIntentMap;
    typedef HashMap<Int32, AutoPtr<StringIntentMap> > StickyBroadcastHashMap;
    typedef typename StickyBroadcastHashMap::Iterator StickyBroadcastIterator;
    StickyBroadcastHashMap mStickyBroadcasts;

    AutoPtr<ActiveServices> mServices;

    /**
     * Backup/restore process management
     */
    String mBackupAppName;
    AutoPtr<BackupRecord> mBackupTarget;

    AutoPtr<ProviderMap> mProviderMap;

    /**
     * List of content providers who have clients waiting for them.  The
     * application is currently being launched and the provider will be
     * removed from this list once it is published.
     */
    List< AutoPtr<ContentProviderRecord> > mLaunchingProviders;

    AutoPtr<CoreSettingsObserver> mCoreSettingsObserver;

    /**
     * All information we have collected about the runtime performance of
     * any user id that can impact battery performance.
     */
    AutoPtr<CBatteryStatsService> mBatteryStatsService;

    /**
     * information about component usage
     */
    AutoPtr<IUsageStatsManagerInternal> mUsageStatsService;

    /**
     * Information about and control over application operations
     */
    AutoPtr<CAppOpsService> mAppOpsService;

    /**
     * Save recent tasks information across reboots.
     */
    AutoPtr<TaskPersister> mTaskPersister;

    /**
     * Current configuration information.  HistoryRecord objects are given
     * a reference to this object to indicate which configuration they are
     * currently running in, so this object must be kept immutable.
     */
    AutoPtr<IConfiguration> mConfiguration;

    /**
     * Current sequencing integer of the configuration, for skipping old
     * configurations.
     */
    Int32 mConfigurationSeq;

    /**
     * Hardware-reported OpenGLES version.
     */
    Int32 GL_ES_VERSION;

    /**
     * List of initialization arguments to pass to all processes when binding applications to them.
     * For example, references to the commonly used services.
     */
    AutoPtr<HashMap<String, AutoPtr<IInterface> > > mAppBindArgs;

    /**
     * Temporary to avoid allocations.  Protected by main lock.
     */
    AutoPtr<StringBuilder> mStringBuilder;

    /**
     * Used to control how we initialize the service.
     */
    AutoPtr<IComponentName> mTopComponent;
    String mTopAction;
    String mTopData;
    Boolean mProcessesReady;
    Boolean mSystemReady;
    Boolean mBooting;
    Boolean mCallFinishBooting;
    Boolean mBootAnimationComplete;
    Boolean mWaitingUpdate;
    Boolean mDidUpdate;
    Boolean mOnBattery;
    Boolean mLaunchWarningShown;

    AutoPtr<IContext> mContext;
    //todo: sSystemContext maybe not its memeber
    static AutoPtr<IContext> sSystemContext;

    Int32 mFactoryTest;

    Boolean mCheckedForSetup;

    /**
     * The time at which we will allow normal application switches again,
     * after a call to {@link #stopAppSwitches()}.
     */
    Int64 mAppSwitchesAllowedTime;

    /**
     * This is set to true after the first switch after mAppSwitchesAllowedTime
     * is set; any switches after that will clear the time.
     */
    Boolean mDidAppSwitch;

    /**
     * Last time (in realtime) at which we checked for power usage.
     */
    Int64 mLastPowerCheckRealtime;

    /**
     * Last time (in uptime) at which we checked for power usage.
     */
    Int64 mLastPowerCheckUptime;

    /**
     * Set if we are shutting down the system, similar to sleeping.
     */
    Boolean mShuttingDown;

    /**
     * Current sequence id for oom_adj computation traversal.
     */
    Int32 mAdjSeq;

    /**
     * Current sequence id for process LRU updating.
     */
    Int32 mLruSeq;

    /**
     * Keep track of the non-cached/empty process we last found, to help
     * determine how to distribute cached/empty processes next time.
     */
    Int32 mNumNonCachedProcs;

    /**
     * Keep track of the number of cached hidden procs, to balance oom adj
     * distribution between those and empty procs.
     */
    Int32 mNumCachedHiddenProcs;

    /**
     * Keep track of the number of service processes we last found, to
     * determine on the next iteration which should be B services.
     */
    Int32 mNumServiceProcs;
    Int32 mNewNumAServiceProcs;
    Int32 mNewNumServiceProcs;

    /**
     * Allow the current computed overall memory level of the system to go down?
     * This is set to false when we are killing processes for reasons other than
     * memory management, so that the now smaller process list will not be taken as
     * an indication that memory is tighter.
     */
    Boolean mAllowLowerMemLevel;

    /**
     * The last computed memory level, for holding when we are in a state that
     * processes are going away for other reasons.
     */
    Int32 mLastMemoryLevel;

    /**
     * The last total number of process we have, to determine if changes actually look
     * like a shrinking number of process due to lower RAM.
     */
    Int32 mLastNumProcesses;

    /**
     * The uptime of the last time we performed idle maintenance.
     */
    Int64 mLastIdleTime;

    /**
     * Total time spent with RAM that has been added in the past since the last idle time.
     */
    Int64 mLowRamTimeSinceLastIdle;

    /**
     * If RAM is currently low, when that horrible situation started.
     */
    Int64 mLowRamStartTime;

    /**
     * For reporting to battery stats the apps currently running foreground
     * service.  The ProcessMap is package/uid tuples; each of these contain
     * an array of the currently foreground processes.
     */
    AutoPtr<ProcessMap<AutoPtr<List<ProcessRecord> > > > mForegroundPackages;

    /**
     * This is set if we had to do a delayed dexopt of an app before launching
     * it, to increase the ANR timeouts in that case.
     */
    Boolean mDidDexOpt;

     /**
     * Set if the systemServer made a call to enterSafeMode.
     */
    Boolean mSafeMode;

    String mDebugApp;
    Boolean mWaitForDebugger;
    Boolean mDebugTransient;
    String mOrigDebugApp;
    Boolean mOrigWaitForDebugger;
    Boolean mAlwaysFinishActivities;
    AutoPtr<IActivityController> mController;
    String mProfileApp;
    AutoPtr<ProcessRecord> mProfileProc;
    String mProfileFile;
    AutoPtr<IParcelFileDescriptor> mProfileFd;
    Int32 mSamplingInterval;
    Boolean mAutoStopProfiler;
    Int32 mProfileType;
    String mOpenGlTraceApp;

    AutoPtr<IRemoteCallbackList> mProcessObservers;
//            = new RemoteCallbackList<IProcessObserver>();
    AutoPtr< ArrayOf<ProcessChangeItem*> > mActiveProcessChanges;

    List< AutoPtr<ProcessChangeItem> > mPendingProcessChanges;
    List< AutoPtr<ProcessChangeItem> > mAvailProcessChanges;

    /**
     * Runtime CPU use collection thread.  This object's lock is used to
     * perform synchronization with the thread (notifying it to run).
     */
    AutoPtr<IThread> mProcessCpuThread;

    /**
     * Used to collect per-process CPU use for ANRs, battery stats, etc.
     * Must acquire this object's lock when accessing it.
     * NOTE: this lock will be held while doing long operations (trawling
     * through all processes in /proc), so it should never be acquired by
     * any critical paths such as when holding the main activity manager lock.
     */
    AutoPtr<IProcessCpuTracker> mProcessCpuTracker;
    AutoPtr<IAtomicInteger64> mLastCpuTime;
    AutoPtr<IAtomicBoolean> mProcessCpuMutexFree;

    Int64 mLastWriteTime;

    /**
     * Used to retain an update lock when the foreground activity is in
     * immersive mode.
     */
    AutoPtr<IUpdateLock> mUpdateLock;

    /**
     * Set to true after the system has finished booting.
     */
    Boolean mBooted;

    Int32 mProcessLimit;
    Int32 mProcessLimitOverride;

    AutoPtr<CWindowManagerService> mWindowManager;

    AutoPtr<IActivityThread> mSystemThread;

    // Holds the current foreground user's id
    Int32 mCurrentUserId;
    // Holds the target user's id during a user switch
    Int32 mTargetUserId;
    // If there are multiple profiles for the current user, their ids are here
    // Currently only the primary user can have managed profiles
    AutoPtr<ArrayOf<Int32> > mCurrentProfileIds; // Accessed by ActivityStack

    /**
     * Mapping from each known user ID to the profile group ID it is associated with.
     */
    HashMap<Int32, Int32> mUserProfileGroupIdsSelfLocked;

    static const Int32 SHOW_ERROR_MSG;
    static const Int32 SHOW_NOT_RESPONDING_MSG;
    static const Int32 SHOW_FACTORY_ERROR_MSG;
    static const Int32 UPDATE_CONFIGURATION_MSG;
    static const Int32 GC_BACKGROUND_PROCESSES_MSG;
    static const Int32 WAIT_FOR_DEBUGGER_MSG;
    static const Int32 SERVICE_TIMEOUT_MSG;
    static const Int32 UPDATE_TIME_ZONE;
    static const Int32 SHOW_UID_ERROR_MSG;
    static const Int32 IM_FEELING_LUCKY_MSG;
    static const Int32 PROC_START_TIMEOUT_MSG;
    static const Int32 DO_PENDING_ACTIVITY_LAUNCHES_MSG;
    static const Int32 KILL_APPLICATION_MSG;
    static const Int32 FINALIZE_PENDING_INTENT_MSG;
    static const Int32 POST_HEAVY_NOTIFICATION_MSG;
    static const Int32 CANCEL_HEAVY_NOTIFICATION_MSG;
    static const Int32 SHOW_STRICT_MODE_VIOLATION_MSG;
    static const Int32 CHECK_EXCESSIVE_WAKE_LOCKS_MSG;
    static const Int32 CLEAR_DNS_CACHE_MSG;
    static const Int32 UPDATE_HTTP_PROXY_MSG;
    static const Int32 SHOW_COMPAT_MODE_DIALOG_MSG;
    static const Int32 DISPATCH_PROCESSES_CHANGED;
    static const Int32 DISPATCH_PROCESS_DIED;
    static const Int32 REPORT_MEM_USAGE_MSG;
    static const Int32 REPORT_USER_SWITCH_MSG;
    static const Int32 CONTINUE_USER_SWITCH_MSG;
    static const Int32 USER_SWITCH_TIMEOUT_MSG;
    static const Int32 IMMERSIVE_MODE_LOCK_MSG;
    static const Int32 PERSIST_URI_GRANTS_MSG;
    static const Int32 REQUEST_ALL_PSS_MSG;
    static const Int32 START_PROFILES_MSG;
    static const Int32 UPDATE_TIME;
    static const Int32 SYSTEM_USER_START_MSG;
    static const Int32 SYSTEM_USER_CURRENT_MSG;
    static const Int32 ENTER_ANIMATION_COMPLETE_MSG;
    static const Int32 FINISH_BOOTING_MSG;
    static const Int32 START_USER_SWITCH_MSG;
    static const Int32 SEND_LOCALE_TO_MOUNT_DAEMON_MSG;

    static const Int32 FIRST_ACTIVITY_STACK_MSG;
    static const Int32 FIRST_BROADCAST_QUEUE_MSG;
    static const Int32 FIRST_COMPAT_MODE_MSG;
    static const Int32 FIRST_SUPERVISOR_STACK_MSG = 100;

    AutoPtr<IAlertDialog> mUidAlert;
    AutoPtr<CompatModeDialog> mCompatModeDialog;
    Int64 mLastMemUsageReportTime;

    /** Flag whether the device has a Recents UI */
    Boolean mHasRecents;

    /** The dimensions of the thumbnails in the Recents UI. */
    Int32 mThumbnailWidth;
    Int32 mThumbnailHeight;

    AutoPtr<ServiceThread> mHandlerThread;
    AutoPtr<MainHandler> mHandler;

    static const Int32 COLLECT_PSS_BG_MSG;

    AutoPtr<Handler> mBgHandler;

    AutoPtr<PackageMonitor> mPackageMonitor;

    static const Int32 LAST_DONE_VERSION;

    // These are in KB.
    static const AutoPtr< ArrayOf<Int64> > DUMP_MEM_BUCKETS;

    static const AutoPtr< ArrayOf<Int32> > DUMP_MEM_OOM_ADJ;
    static const AutoPtr< ArrayOf<String> > DUMP_MEM_OOM_LABEL;

private:
    static const String USER_DATA_DIR;

    static const String SYSTEM_DEBUGGABLE;

    // Whether we should show our dialogs (ANR, crash, etc) or just perform their
    // default actuion automatically.  Important for devices without direct input
    // devices.
    Boolean mShowDialogs;

    /**
     * Fingerprints (hashCode()) of stack traces that we've
     * already logged DropBox entries for.  Guarded by itself.  If
     * something (rogue user app) forces this over
     * MAX_DUP_SUPPRESSED_STACKS entries, the contents are cleared.
     */
    HashSet<Int32> mAlreadyLoggedViolatedStacks;
    Object mAlreadyLoggedViolatedStacksLock;

    static const Int32 MAX_DUP_SUPPRESSED_STACKS;

    /**
     * Strict Mode background batched logging state.
     *
     * The string buffer is guarded by itself, and its lock is also
     * used to determine if another batched write is already
     * in-flight.
     */
    AutoPtr<StringBuilder> mStrictModeBuffer;


    /**
     * File storing persisted {@link #mGrantedUriPermissions}.
     */
    AutoPtr<IAtomicFile> mGrantFile;

    /** XML constants used in {@link #mGrantFile} */
    static const String TAG_URI_GRANTS = "uri-grants";
    static const String TAG_URI_GRANT = "uri-grant";
    static const String ATTR_USER_HANDLE = "userHandle";
    static const String ATTR_SOURCE_USER_ID = "sourceUserId";
    static const String ATTR_TARGET_USER_ID = "targetUserId";
    static const String ATTR_SOURCE_PKG = "sourcePkg";
    static const String ATTR_TARGET_PKG = "targetPkg";
    static const String ATTR_URI = "uri";
    static const String ATTR_MODE_FLAGS = "modeFlags";
    static const String ATTR_CREATED_TIME = "createdTime";
    static const String ATTR_PREFIX = "prefix";

    /**
     * Global set of specific {@link Uri} permissions that have been granted.
     * This optimized lookup structure maps from {@link UriPermission#targetUid}
     * to {@link UriPermission#uri} to {@link UriPermission}.
     */
    // @GuardedBy("this")
    typedef HashMap<AutoPtr<GrantUri>, AutoPtr<IUriPermission> > GrantUriPermissionMap;
    typedef HashMap<Int32, AutoPtr<GrantUriPermissionMap> > GrantUriPermissionMapMap;
    GrantUriPermissionMapMap mGrantedUriPermissions;

    /**
     * Global set of specific Uri permissions that have been granted.
     */
    typedef HashMap<AutoPtr<IUri>, AutoPtr<UriPermission> > PermissionMap;
    typedef HashMap<Int32, AutoPtr<PermissionMap> > UriPermissionHashMap;
    typedef typename UriPermissionHashMap::ValueType UriPermissionValueType;
    typedef typename UriPermissionHashMap::Iterator UriPermissionIterator;
    UriPermissionHashMap mGrantedUriPermissions;

    static pthread_key_t sCallerIdentity;


    /**
     * Set while we are wanting to sleep, to prevent any
     * activities from being started/resumed.
     */
    Boolean mSleeping;

    /**
     * Set while we are running a voice interaction.  This overrides
     * sleeping while it is active.
     */
    Boolean mRunningVoice;

    /**
     * State of external calls telling us if the device is asleep.
     */
    Boolean mWentToSleep;

    /**
     * State of external call telling us if the lock screen is shown.
     */
    Boolean mLockScreenShown;

    /**
     * For reporting to battery stats the current top application.
     */
    String mCurResumedPackage;
    Int32 mCurResumedUid;

    AutoPtr<CUserManagerService> mUserManager;

    AutoPtr<LockToAppRequestDialog> mLockToAppRequest;

    /**
     * Flag whether the current user is a "monkey", i.e. whether
     * the UI is driven by a UI automation tool.
     */
    Boolean mUserIsMonkey;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
#endif

#endif // __ELASTOS_DROID_SERVER_AM_CACTIVITYMANAGERSERVICE_H__
