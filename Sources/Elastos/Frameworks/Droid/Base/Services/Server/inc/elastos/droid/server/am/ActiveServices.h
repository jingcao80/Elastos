
#ifndef __ELASTOS_DROID_SERVER_AM_ACTIVESERVICES_H__
#define __ELASTOS_DROID_SERVER_AM_ACTIVESERVICES_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/am/TaskRecord.h"
#include "elastos/droid/server/am/CServiceRecord.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::App::IIServiceConnection;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IActivityManagerRunningServiceInfo;
using Elastos::Droid::App::IApplicationThread;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

extern "C" const InterfaceID EIID_ServiceRestarter;

class CActivityManagerService;
class ProcessRecord;

typedef HashMap< AutoPtr<IComponentName>, AutoPtr<CServiceRecord> > IComponentNameCServiceRecordHashMap;
typedef HashMap<AutoPtr<IIntentFilterComparison>, AutoPtr<CServiceRecord> > IIntentFilterComparisonCServiceRecordHashMap;

class ActiveServices : public Object
{
public:
    class ServiceMap : public Handler
    {
        friend class ActiveServices;
    public:
        ServiceMap(
            /* [in] */ ILooper* looper,
            /* [in] */ Int32 userId,
            /* [in] */ ActiveServices* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI_(void) EnsureNotStartingBackground(
            /* [in] */ CServiceRecord* r);

        CARAPI_(void) RescheduleDelayedStarts();

    public:
        const Int32 mUserId;
        IComponentNameCServiceRecordHashMap mServicesByName;
        IIntentFilterComparisonCServiceRecordHashMap mServicesByIntent;

        List<AutoPtr<CServiceRecord> > mDelayedStartList;
        /* XXX eventually I'd like to have this based on processes instead of services.
         * That is, if we try to start two services in a row both running in the same
         * process, this should be one entry in mStartingBackground for that one process
         * that remains until all services in it are done.
        final ArrayMap<ProcessRecord, DelayingProcess> mStartingBackgroundMap
                = new ArrayMap<ProcessRecord, DelayingProcess>();
        final ArrayList<DelayingProcess> mStartingProcessList
                = new ArrayList<DelayingProcess>();
        */

        List<AutoPtr<CServiceRecord> > mStartingBackground;

        static const Int32 MSG_BG_START_TIMEOUT = 1;
    };

    class ServiceLookupResult : public Object
    {
    public:
        ServiceLookupResult(
            /* [in] */ CServiceRecord* record,
            /* [in] */ const String& permission);
    public:
        const AutoPtr<CServiceRecord> mRecord;
        const String mPermission;
    };

private:
    class ServiceRestarter
        : public Runnable
        , public IServiceRestarter
    {
    public:
        ServiceRestarter(
            /* [in] */ ActiveServices* owner);

        CAR_INTERFACE_DECL()

        CARAPI_(void) SetService(
            /* [in] */ CServiceRecord* service);

        CARAPI Run();

    private:
        CServiceRecord* mService;
        ActiveServices* mOwner;
    };

public:
    ActiveServices(
        /* [in] */ CActivityManagerService* service);

    ~ActiveServices();

    CARAPI_(AutoPtr<CServiceRecord>) GetServiceByName(
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 callingUser);

    CARAPI_(Boolean) HasBackgroundServices(
        /* [in] */ Int32 callingUser);

    CARAPI_(IComponentNameCServiceRecordHashMap&) GetServices(
        /* [in] */ Int32 callingUser);

    CARAPI StartServiceLocked(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IIntent* service,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 userId,
        /* [out] */ IComponentName** name);

    CARAPI_(AutoPtr<IComponentName>) StartServiceInnerLocked(
        /* [in] */ ServiceMap* smap,
        /* [in] */ IIntent* service,
        /* [in] */ CServiceRecord* r,
        /* [in] */ Boolean callerFg,
        /* [in] */ Boolean addToStarting);

    CARAPI StopServiceLocked(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IIntent* service,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI PeekServiceLocked(
        /* [in] */ IIntent* service,
        /* [in] */ const String& resolvedType,
        /* [out] */ IBinder** token);

    CARAPI StopServiceTokenLocked(
        /* [in] */ IComponentName* className,
        /* [in] */ IBinder* token,
        /* [in] */ Int32 startId,
        /* [out] */ Boolean* succeeded);

    CARAPI SetServiceForegroundLocked(
        /* [in] */ IComponentName* className,
        /* [in] */ IBinder* token,
        /* [in] */ Int32 id,
        /* [in] */ INotification* notification,
        /* [in] */ Boolean removeNotification);

    CARAPI UpdateServiceConnectionActivitiesLocked(
        /* [in] */ ProcessRecord* clientProc);

    CARAPI BindServiceLocked(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IBinder* token,
        /* [in] */ IIntent* service,
        /* [in] */ const String& resolvedType,
        /* [in] */ IIServiceConnection* connection,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI PublishServiceLocked(
        /* [in] */ CServiceRecord* r,
        /* [in] */ IIntent* intent,
        /* [in] */ IBinder* service);

    CARAPI UnbindServiceLocked(
        /* [in] */ IIServiceConnection* connection,
        /* [out] */ Boolean* result);

    CARAPI UnbindFinishedLocked(
        /* [in] */ CServiceRecord* r,
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean doRebind);

    CARAPI PerformServiceRestartLocked(
        /* [in] */ CServiceRecord* r);

    CARAPI RemoveConnectionLocked(
        /* [in] */ ConnectionRecord* c,
        /* [in] */ ProcessRecord* skipApp,
        /* [in] */ ActivityRecord* skipAct);

    CARAPI ServiceDoneExecutingLocked(
        /* [in] */ CServiceRecord* r,
        /* [in] */ Int32 type,
        /* [in] */ Int32 startId,
        /* [in] */ Int32 res);

    CARAPI AttachApplicationLocked(
        /* [in] */ ProcessRecord* proc,
        /* [in] */ const String& processName,
        /* [out] */ Boolean* result);

    CARAPI ProcessStartTimedOutLocked(
        /* [in] */ ProcessRecord* proc);

    CARAPI_(Boolean) ForceStopLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean evenPersistent,
        /* [in] */ Boolean doit);

    CARAPI CleanUpRemovedTaskLocked(
        /* [in] */ TaskRecord* tr,
        /* [in] */ IComponentName* component,
        /* [in] */ IIntent* baseIntent);

    CARAPI KillServicesLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Boolean allowRestart);

    CARAPI_(AutoPtr<IActivityManagerRunningServiceInfo>) MakeRunningServiceInfoLocked(
        /* [in] */ CServiceRecord* r);

    CARAPI_(AutoPtr<List<AutoPtr<IActivityManagerRunningServiceInfo> > >) GetRunningServiceInfoLocked(
        /* [in] */ Int32 maxNum,
        /* [in] */ Int32 flags);

    CARAPI GetRunningServiceControlPanelLocked(
        /* [in] */ IComponentName* name,
        /* [out] */ IPendingIntent** intent);

    CARAPI ServiceTimeout(
        /* [in] */ ProcessRecord* proc);

    CARAPI_(void) ScheduleServiceTimeoutLocked(
        /* [in] */ ProcessRecord* proc);

    CARAPI_(void) DumpServicesLocked(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int32 opti,
        /* [in] */ Boolean dumpAll,
        /* [in] */ Boolean dumpClient,
        /* [in] */ const String& dumpPackage);

    /**
     * There are three ways to call this:
     *  - no service specified: dump all the services
     *  - a flattened component name that matched an existing service was specified as the
     *    first arg: dump that one service
     *  - the first arg isn't the flattened component name of an existing service:
     *    dump all services whose component contains the first arg as a substring
     */
    CARAPI_(Boolean) DumpService(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int32 opti,
        /* [in] */ Boolean dumpAll);

private:
    CARAPI_(AutoPtr<ServiceMap>) GetServiceMap(
        /* [in] */ Int32 callingUser);

    CARAPI StopServiceLocked(
        /* [in] */ CServiceRecord* service);

    CARAPI UpdateServiceForegroundLocked(
        /* [in] */ ProcessRecord* proc,
        /* [in] */ Boolean oomAdj);

    CARAPI_(Boolean) UpdateServiceClientActivitiesLocked(
        /* [in] */ ProcessRecord* proc,
        /* [in] */ ConnectionRecord* modCr,
        /* [in] */ Boolean updateLru);

    CARAPI_(AutoPtr<CServiceRecord>) FindServiceLocked(
        /* [in] */ IComponentName* name,
        /* [in] */ IBinder* token,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<ServiceLookupResult>) RetrieveServiceLocked(
        /* [in] */ IIntent* service,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean createIfNeeded,
        /* [in] */ Boolean callingFromFg);

    CARAPI_(void) BumpServiceExecutingLocked(
        /* [in] */ CServiceRecord* r,
        /* [in] */ Boolean fg,
        /* [in] */ const String& why);

    CARAPI_(Boolean) RequestServiceBindingLocked(
        /* [in] */ CServiceRecord* r,
        /* [in] */ IntentBindRecord* i,
        /* [in] */ Boolean execInFg,
        /* [in] */ Boolean rebind);

    CARAPI_(Boolean) ScheduleServiceRestartLocked(
        /* [in] */ CServiceRecord* r,
        /* [in] */ Boolean allowCancel);

    CARAPI_(Boolean) UnscheduleServiceRestartLocked(
        /* [in] */ CServiceRecord* r);

    CARAPI_(void) ClearRestartingIfNeededLocked(
        /* [in] */ CServiceRecord* r);

    CARAPI_(String) BringUpServiceLocked(
        /* [in] */ CServiceRecord* r,
        /* [in] */ Int32 intentFlags,
        /* [in] */ Boolean execInFg,
        /* [in] */ Boolean whileRestarting);

    CARAPI RequestServiceBindingsLocked(
        /* [in] */ CServiceRecord* r,
        /* [in] */ Boolean execInFg);

    CARAPI RealStartServiceLocked(
        /* [in] */ CServiceRecord* r,
        /* [in] */ ProcessRecord* app,
        /* [in] */ Boolean execInFg);

    CARAPI SendServiceArgsLocked(
        /* [in] */ CServiceRecord* r,
        /* [in] */ Boolean execInFg,
        /* [in] */ Boolean oomAdjusted);

    CARAPI_(Boolean) IsServiceNeeded(
        /* [in] */ CServiceRecord* r,
        /* [in] */ Boolean knowConn,
        /* [in] */ Boolean hasConn);

    CARAPI BringDownServiceIfNeededLocked(
        /* [in] */ CServiceRecord* r,
        /* [in] */ Boolean knowConn,
        /* [in] */ Boolean hasConn);

    CARAPI BringDownServiceLocked(
        /* [in] */ CServiceRecord* r);

    CARAPI_(void) ServiceProcessGoneLocked(
        /* [in] */ CServiceRecord* r);

    CARAPI ServiceDoneExecutingLocked(
        /* [in] */ CServiceRecord* r,
        /* [in] */ Boolean inDestroying,
        /* [in] */ Boolean finishing);

    CARAPI_(Boolean) CollectForceStopServicesLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean evenPersistent,
        /* [in] */ Boolean doit,
        /* [in] */ IComponentNameCServiceRecordHashMap* services,
        /* [in] */ List<AutoPtr<CServiceRecord> >* result);

    /**
     * Invokes IApplicationThread.dumpService() on the thread of the specified service if
     * there is a thread associated with the service.
     */
    CARAPI_(void) DumpService(
        /* [in] */ const String& prefix,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ CServiceRecord* r,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Boolean dumpAll);

public:
    static const Boolean DEBUG_SERVICE;
    static const Boolean DEBUG_SERVICE_EXECUTING;
    static const Boolean DEBUG_DELAYED_SERVICE;
    static const Boolean DEBUG_DELAYED_STARTS;
    static const Boolean DEBUG_MU;
    static const Boolean LOG_SERVICE_START_STOP;
    static const String TAG;
    static const String TAG_MU;

    // How long we wait for a service to finish executing.
    static const Int32 SERVICE_TIMEOUT = 20*1000;

    // How long we wait for a service to finish executing.
    static const Int32 SERVICE_BACKGROUND_TIMEOUT = SERVICE_TIMEOUT * 10;

    // How long a service needs to be running until restarting its process
    // is no longer considered to be a relaunch of the service.
    static const Int32 SERVICE_RESTART_DURATION = 1*1000;

    // How long a service needs to be running until it will start back at
    // SERVICE_RESTART_DURATION after being killed.
    static const Int32 SERVICE_RESET_RUN_DURATION = 60*1000;

    // Multiplying factor to increase restart duration time by, for each time
    // a service is killed before it has run for SERVICE_RESET_RUN_DURATION.
    static const Int32 SERVICE_RESTART_DURATION_FACTOR = 4;

    // The minimum amount of time between restarting services that we allow.
    // That is, when multiple services are restarting, we won't allow each
    // to restart less than this amount of time from the last one.
    static const Int32 SERVICE_MIN_RESTART_TIME_BETWEEN = 10*1000;

    // Maximum amount of time for there to be no activity on a service before
    // we consider it non-essential and allow its process to go on the
    // LRU background list.
    static const Int32 MAX_SERVICE_INACTIVITY = 30*60*1000;

    // How long we wait for a background started service to stop itself before
    // allowing the next pending start to run.
    static const Int32 BG_START_TIMEOUT = 15*1000;

    CActivityManagerService* mAm; // weak-ref

    // Maximum number of services that we allow to start in the background
    // at the same time.
    const Int32 mMaxStartingBackground;

    HashMap<Int32, AutoPtr<ServiceMap> > mServiceMap;

    /**
     * All currently bound service connections.  Keys are the IBinder of
     * the client's IServiceConnection.
     */
    typedef List<AutoPtr<ConnectionRecord> > ConnectionRecordList;
    typedef ConnectionRecordList::Iterator ConnectionRecordListIterator;
    HashMap< AutoPtr<IBinder>, AutoPtr<ConnectionRecordList> > mServiceConnections;

    /**
     * List of services that we have been asked to start,
     * but haven't yet been able to.  It is used to hold start requests
     * while waiting for their corresponding application thread to get
     * going.
     */
    List< AutoPtr<CServiceRecord> > mPendingServices;

    /**
     * List of services that are scheduled to restart following a crash.
     */
    List< AutoPtr<CServiceRecord> > mRestartingServices;

    /**
     * List of services that are in the process of being destroyed.
     */
    List< AutoPtr<CServiceRecord> > mDestroyingServices;

    Object mLock;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__CACTIVESERVICES_H__
