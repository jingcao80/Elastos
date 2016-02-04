
#ifndef __ELASTOS_DROID_SERVER_AM_CSERVICERECORD_H__
#define __ELASTOS_DROID_SERVER_AM_CSERVICERECORD_H__

#include "_Elastos_Droid_Server_Am_CServiceRecord.h"
#define HASH_FOR_OS
#define HASH_FOR_CONTENT
#include "elastos/droid/ext/frameworkhash.h"
#include "elastos/droid/server/am/ConnectionRecord.h"
#include "Elastos.Droid.App.h"
#include <elastos/droid/os/Runnable.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::App::INotification;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilterComparison;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Internal::App::IServiceState;
using Elastos::Droid::Internal::Os::IBatteryStatsImplUidPkgServ;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Runnable;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class AppBindRecord;
class CActivityManagerService;
class NeededUriGrants;
class IntentBindRecord;
class ProcessRecord;
class UriPermissionOwner;

CarClass(CServiceRecord)
    , public Object
    , public IBinder
{
public:
    class StartItem : public Object
    {
    public:
        StartItem(
            /* [in] */ CServiceRecord* sr,
            /* [in] */ Boolean taskRemoved,
            /* [in] */ Int32 id,
            /* [in] */ IIntent* intent,
            /* [in] */ NeededUriGrants* neededGrants);

        CARAPI_(AutoPtr<UriPermissionOwner>) GetUriPermissionsLocked();

        CARAPI RemoveUriPermissionsLocked();

        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI_(String) ToString();

    public:
        CServiceRecord* mSr;
        Boolean mTaskRemoved;
        Int32 mId;
        AutoPtr<IIntent> mIntent;
        AutoPtr<NeededUriGrants> mNeededGrants;
        Int64 mDeliveredTime;
        Int32 mDeliveryCount;
        Int32 mDoneExecutingCount;
        AutoPtr<UriPermissionOwner> mUriPermissions;

        String mStringName;      // caching of toString
    };

private:
    class PostNotificationRunnable
        : public Runnable
    {
    public:
        PostNotificationRunnable(
            /* [in] */ CServiceRecord* host,
            /* [in] */ const String& localPackageName,
            /* [in] */ Int32 appUid,
            /* [in] */ Int32 appPid,
            /* [in] */ Int32 localForegroundId,
            /* [in] */ INotification* localForegroundNoti)
            : mHost(host)
            , mLocalPackageName(localPackageName)
            , mAppUid(appUid)
            , mAppPid(appPid)
            , mLocalForegroundId(localForegroundId)
            , mLocalForegroundNoti(localForegroundNoti)
        {}

        CARAPI Run();

    private:
        CServiceRecord* mHost;
        String mLocalPackageName;
        Int32 mAppUid;
        Int32 mAppPid;
        Int32 mLocalForegroundId;
        AutoPtr<INotification> mLocalForegroundNoti;
    };

    class CancelNotificationRunnable
        : public Runnable
    {
    public:
        CancelNotificationRunnable(
            /* [in] */ CServiceRecord* host,
            /* [in] */ const String& localPackageName,
            /* [in] */ Int32 localForegroundId)
            : mHost(host)
            , mLocalPackageName(localPackageName)
            , mLocalForegroundId(localForegroundId)
        {}

        CARAPI Run();

    private:
        CServiceRecord* mHost;
        String mLocalPackageName;
        Int32 mLocalForegroundId;
    };

    class StripForegroundServiceFlagNotificationRunnable
        : public Runnable
    {
    public:
        StripForegroundServiceFlagNotificationRunnable(
            /* [in] */ const String& localPackageName,
            /* [in] */ Int32 localUserId,
            /* [in] */ Int32 localForegroundId)
            : mLocalPackageName(localPackageName)
            , mLocalUserId(localUserId)
            , mLocalForegroundId(localForegroundId)
        {}

        CARAPI Run();

    private:
        String mLocalPackageName;
        Int32 mLocalUserId;
        Int32 mLocalForegroundId;
    };

public:
    CServiceRecord();

    virtual ~CServiceRecord();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ CActivityManagerService* ams,
        /* [in] */ IBatteryStatsImplUidPkgServ* servStats,
        /* [in] */ IComponentName* name,
        /* [in] */ IIntentFilterComparison* intent,
        /* [in] */ IServiceInfo* sInfo,
        /* [in] */ Boolean callerIsFg,
        /* [in] */ IRunnable* restarter);

    CARAPI_(void) DumpStartList(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ List<AutoPtr<StartItem> >* list,
        /* [in] */ Int32 now);

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI_(AutoPtr<IServiceState>) GetTracker();

    CARAPI_(void) ForceClearTracker();

    CARAPI_(void) MakeRestarting(
        /* [in] */ Int32 memFactor,
        /* [in] */ Int64 now);

    CARAPI_(AutoPtr<AppBindRecord>) RetrieveAppBindingLocked(
        /* [in] */ IIntent* intent,
        /* [in] */ ProcessRecord* app);

    CARAPI_(Boolean) HasAutoCreateConnections();

    CARAPI ResetRestartCounter();

    CARAPI_(AutoPtr<StartItem>) FindDeliveredStart(
        /* [in] */ Int32 id,
        /* [in] */ Boolean remove);

    CARAPI_(Int32) GetLastStartId();

    CARAPI_(Int32) MakeNextStartId();

    CARAPI PostNotification();

    CARAPI CancelNotification();

    CARAPI StripForegroundServiceFlagFromNotification();

    CARAPI ClearDeliveredStartsLocked();

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI_(String) ToString();

public:
    // Maximum number of delivery attempts before giving up.
    static const Int32 MAX_DELIVERY_COUNT = 3;

    // Maximum number of times it can fail during execution before giving up.
    static const Int32 MAX_DONE_EXECUTING_COUNT = 6;

    AutoPtr<CActivityManagerService> mAms;
    AutoPtr<IBatteryStatsImplUidPkgServ> mStats;
    AutoPtr<IComponentName> mName; // service component.
    String mShortName; // name.flattenToShortString().
    AutoPtr<IIntentFilterComparison> mIntent; // original intent used to find service.
    AutoPtr<IServiceInfo> mServiceInfo;  // all information about the service.
    AutoPtr<IApplicationInfo> mAppInfo; // information about service's app.
    Int32 mUserId;       // user that this service is running as
    String mPackageName; // the package implementing intent's component
    String mProcessName; // process where this component wants to run
    String mPermission; // permission needed to access service
    Boolean mExported; // from ServiceInfo.exported
    AutoPtr<IRunnable> mRestarter; // used to schedule retries of starting the service
    Int64 mCreateTime;  // when this service was created
    HashMap< AutoPtr<IIntentFilterComparison>, AutoPtr<IntentBindRecord> > mBindings; // All active bindings to the service.
    typedef HashMap< AutoPtr<IBinder>, AutoPtr<List< AutoPtr<ConnectionRecord> > > > ConnectionHashMap;
    typedef ConnectionHashMap::Iterator ConnectionIterator;
    ConnectionHashMap mConnections; // IBinder -> ConnectionRecord of all bound clients

    AutoPtr<ProcessRecord> mApp;  // where this service is running or null.
    AutoPtr<ProcessRecord> mIsolatedProc; // keep track of isolated process, if requested
    AutoPtr<IServiceState> mTracker; // tracking service execution, may be null
    AutoPtr<IServiceState> mRestartTracker; // tracking service restart
    Boolean mDelayed;        // are we waiting to start this service in the background?
    Boolean mIsForeground;   // is service currently in foreground mode?
    Int32 mForegroundId;       // Notification ID of last foreground req.
    AutoPtr<INotification> mForegroundNoti; // Notification record of foreground state.
    Int64 mLastActivity;      // last time there was some activity on the service.
    Int64 mStartingBgTimeout;  // time at which we scheduled this for a delayed start.
    Boolean mStartRequested; // someone explicitly called start?
    Boolean mDelayedStop;    // service has been stopped but is in a delayed start?
    Boolean mStopIfKilled;   // last onStart() said to stop if service killed?
    Boolean mCallStart;      // last onStart() has asked to alway be called on restart.
    Int32 mExecuteNesting;     // number of outstanding operations keeping foreground.
    Boolean mExecuteFg;      // should we be executing in the foreground?
    Int64 mExecutingStart;    // start time of last execute request.
    Boolean mCreatedFromFg;  // was this service last created due to a foreground process call?
    Int32 mCrashCount;         // number of times proc has crashed with service running
    Int32 mTotalRestartCount;  // number of times we have had to restart.
    Int32 mRestartCount;       // number of restarts performed in a row.
    Int64 mRestartDelay;      // delay until next restart attempt.
    Int64 mRestartTime;       // time of last restart.
    Int64 mNextRestartTime;   // time when restartDelay will expire.
    String mStringName;  // caching of toString
    List< AutoPtr<StartItem> > mDeliveredStarts;
                            // start() arguments which been delivered.
    List< AutoPtr<StartItem> > mPendingStarts;
                            // start() arguments that haven't yet been delivered.

private:
    Int32 mLastStartId;        // identifier of most recent start request.
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Am::CServiceRecord)

#endif //__ELASTOS_DROID_SERVER_AM_CSERVICERECORD_H__
