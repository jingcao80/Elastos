
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/app/CApplicationThread.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/net/Proxy.h"
#include "elastos/droid/utility/CPairHelper.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Net::Proxy;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::IPairHelper;
using Elastos::Droid::Utility::CPairHelper;

using Elastos::Core::CString;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Text::IDateFormatHelper;
using Elastos::Text::CDateFormatHelper;
using Elastos::Net::IInetAddressHelper;
using Elastos::Net::CInetAddressHelper;
using Libcore::Net::Event::INetworkEventDispatcher;
using Libcore::Net::Event::INetworkEventDispatcherHelper;
using Libcore::Net::Event::CNetworkEventDispatcherHelper;

namespace Elastos {
namespace Droid {
namespace App {

const String CApplicationThread::ONE_COUNT_COLUMN("%21s %8d");
const String CApplicationThread::TWO_COUNT_COLUMNS("%21s %8d %21s %8d");
const String CApplicationThread::DB_INFO_FORMAT("  %8s %8s %14s %14s  %s");

CAR_INTERFACE_IMPL_2(CApplicationThread, Object, IApplicationThread, IBinder)

CAR_OBJECT_IMPL(CApplicationThread)

CApplicationThread::CApplicationThread()
    : mLastProcessState(-1)
{
}

ECode CApplicationThread::constructor()
{
    return NOERROR;
}

void CApplicationThread::UpdatePendingConfiguration(
    /* [in] */ IConfiguration* config)
{
    AutoLock lock(mAThread->mResourcesManager);

    Boolean isNewer;
    if (mAThread->mPendingConfiguration == NULL ||
        (mAThread->mPendingConfiguration->IsOtherSeqNewer(config, &isNewer), isNewer)) {
        mAThread->mPendingConfiguration = config;
    }
}

ECode CApplicationThread::SchedulePauseActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean finished,
    /* [in] */ Boolean userLeaving,
    /* [in] */ Int32 configChanges,
    /* [in] */ Boolean dontReport)
{
    return mAThread->SendMessage(
        finished ? CActivityThread::H::PAUSE_ACTIVITY_FINISHING : CActivityThread::H::PAUSE_ACTIVITY,
        token, (userLeaving ? 1 : 0) | (dontReport ? 2 : 0), configChanges);
}

ECode CApplicationThread::ScheduleStopActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean showWindow,
    /* [in] */ Int32 configChanges)
{
    return mAThread->SendMessage(
        showWindow ? CActivityThread::H::STOP_ACTIVITY_SHOW : CActivityThread::H::STOP_ACTIVITY_HIDE,
        token, 0, configChanges);
}

ECode CApplicationThread::ScheduleWindowVisibility(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean showWindow)
{
    return mAThread->SendMessage(
        showWindow ? CActivityThread::H::SHOW_WINDOW : CActivityThread::H::HIDE_WINDOW, token);
}

ECode CApplicationThread::ScheduleSleeping(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean sleeping)
{
    return mAThread->SendMessage(CActivityThread::H::SLEEPING,
        token, sleeping ? 1 : 0);
}

ECode CApplicationThread::ScheduleResumeActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 processState,
    /* [in] */ Boolean isForward,
    /* [in] */ IBundle* resumeArgs)
{
    UpdateProcessState(processState, FALSE);
    return mAThread->SendMessage(CActivityThread::H::RESUME_ACTIVITY,
        token, isForward ? 1 : 0);
}

ECode CApplicationThread::ScheduleSendResult(
    /* [in] */ IBinder* token,
    /* [in] */ IList* results)
{
    AutoPtr<CActivityThread::ResultData> res = new CActivityThread::ResultData();
    res->mToken = token;
    res->mResults = results;
    return mAThread->SendMessage(CActivityThread::H::SEND_RESULT, TO_IINTERFACE(res));
}

ECode CApplicationThread::ScheduleLaunchActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ IBinder* token,
    /* [in] */ Int32 ident,
    /* [in] */ IActivityInfo* info,
    /* [in] */ IConfiguration* curConfig,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ IIVoiceInteractor* voiceInteractor,
    /* [in] */ Int32 procState,
    /* [in] */ IBundle* state,
    /* [in] */ IPersistableBundle* persistentState,
    /* [in] */ IList* pendingResults,   //List<ResultInfo>
    /* [in] */ IList* pendingNewIntents, //List<Intent>
    /* [in] */ Boolean notResumed,
    /* [in] */ Boolean isForward,
    /* [in] */ IProfilerInfo* profilerInfo)
{
    UpdateProcessState(procState, FALSE);

    AutoPtr<CActivityThread::ActivityClientRecord> r = new CActivityThread::ActivityClientRecord();
    r->mToken = token;
    r->mIdent = ident;
    r->mIntent = intent;
    r->mVoiceInteractor = voiceInteractor;
    r->mActivityInfo = info;
    r->mCompatInfo = compatInfo;
    r->mState = (CBundle*)state;
    r->mPersistentState = persistentState;

    r->mPendingResults = pendingResults;
    r->mPendingIntents = pendingNewIntents;

    r->mStartsNotResumed = notResumed;
    r->mIsForward = isForward;

    r->mProfilerInfo = profilerInfo;

    UpdatePendingConfiguration(curConfig);

    return mAThread->SendMessage(CActivityThread::H::LAUNCH_ACTIVITY, TO_IINTERFACE(r));
}

ECode CApplicationThread::ScheduleRelaunchActivity(
    /* [in] */ IBinder* token,
    /* [in] */ IList* pendingResults,
    /* [in] */ IList* pendingNewIntents,
    /* [in] */ Int32 configChanges,
    /* [in] */ Boolean notResumed,
    /* [in] */ IConfiguration* config)
{
    return mAThread->RequestRelaunchActivity(token, pendingResults, pendingNewIntents,
            configChanges, notResumed, config, TRUE);
}

ECode CApplicationThread::ScheduleNewIntent(
    /* [in] */ IList* intents,
    /* [in] */ IBinder *token)
{
    AutoPtr<CActivityThread::NewIntentData> data = new CActivityThread::NewIntentData();
    data->mIntents = intents;
    data->mToken = token;

    return mAThread->SendMessage(CActivityThread::H::NEW_INTENT, TO_IINTERFACE(data));
}

ECode CApplicationThread::ScheduleDestroyActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean finishing,
    /* [in] */ Int32 configChanges)
{
    return mAThread->SendMessage(CActivityThread::H::DESTROY_ACTIVITY,
        token, finishing ? 1 : 0, configChanges);
}

ECode CApplicationThread::ScheduleReceiver(
    /* [in] */ IIntent* intent,
    /* [in] */ IActivityInfo* info,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean sync,
    /* [in] */ Int32 sendingUser,
    /* [in] */ Int32 processState)
{
    UpdateProcessState(processState, FALSE);
    AutoPtr<CActivityThread::ReceiverData> r = new CActivityThread::ReceiverData(intent, resultCode, data, extras,
        sync, FALSE, IBinder::Probe((IApplicationThread*)mAThread->mAppThread.Get()), sendingUser);
    r->mInfo = info;
    r->mCompatInfo = (CCompatibilityInfo*)compatInfo;

    return mAThread->SendMessage(CActivityThread::H::RECEIVER, TO_IINTERFACE(r));
}

ECode CApplicationThread::ScheduleCreateBackupAgent(
    /* [in] */ IApplicationInfo* app,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ Int32 backupMode)
{
    AutoPtr<CActivityThread::CreateBackupAgentData> d = new CActivityThread::CreateBackupAgentData();
    d->mAppInfo = app;
    d->mCompatInfo = (CCompatibilityInfo*)compatInfo;
    d->mBackupMode = backupMode;

    return mAThread->SendMessage(CActivityThread::H::CREATE_BACKUP_AGENT, TO_IINTERFACE(d));
}

ECode CApplicationThread::ScheduleDestroyBackupAgent(
    /* [in] */ IApplicationInfo* app,
    /* [in] */ ICompatibilityInfo* compatInfo)
{
    AutoPtr<CActivityThread::CreateBackupAgentData> d = new CActivityThread::CreateBackupAgentData();
    d->mAppInfo = app;
    d->mCompatInfo = (CCompatibilityInfo*)compatInfo;

    return mAThread->SendMessage(CActivityThread::H::DESTROY_BACKUP_AGENT, TO_IINTERFACE(d));
}

ECode CApplicationThread::ScheduleCreateService(
    /* [in] */ IBinder* token,
    /* [in] */ IServiceInfo* info,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ Int32 processState)
{
    UpdateProcessState(processState, FALSE);
    AutoPtr<CActivityThread::CreateServiceData> s = new CActivityThread::CreateServiceData();
    s->mToken = token;
    s->mInfo = info;
    s->mCompatInfo = compatInfo;

    return mAThread->SendMessage(CActivityThread::H::CREATE_SERVICE, TO_IINTERFACE(s));
}

ECode CApplicationThread::ScheduleBindService(
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean rebind,
    /* [in] */ Int32 processState)
{
    UpdateProcessState(processState, FALSE);
    AutoPtr<CActivityThread::BindServiceData> s = new CActivityThread::BindServiceData();
    s->mToken = token;
    s->mIntent = intent;
    s->mRebind = rebind;

    if (CActivityThread::DEBUG_SERVICE) {
        Slogger::V(CActivityThread::TAG, "scheduleBindService token=%p intent=%p uid=%d pid=%d"
            , token, intent, Binder::GetCallingUid(), Binder::GetCallingPid());
    }

    return mAThread->SendMessage(CActivityThread::H::BIND_SERVICE, TO_IINTERFACE(s));
}

ECode CApplicationThread::ScheduleUnbindService(
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* intent)
{
    AutoPtr<CActivityThread::BindServiceData> s = new CActivityThread::BindServiceData();
    s->mToken = token;
    s->mIntent = intent;

    return mAThread->SendMessage(CActivityThread::H::UNBIND_SERVICE, TO_IINTERFACE(s));
}

ECode CApplicationThread::ScheduleServiceArgs(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean taskRemoved,
    /* [in] */ Int32 startId,
    /* [in] */ Int32 flags,
    /* [in] */ IIntent* args)
{
    AutoPtr<CActivityThread::ServiceArgsData> s = new CActivityThread::ServiceArgsData();
    s->mToken = token;
    s->mTaskRemoved = taskRemoved;
    s->mStartId = startId;
    s->mFlags = flags;
    s->mArgs = args;

    return mAThread->SendMessage(CActivityThread::H::SERVICE_ARGS, TO_IINTERFACE(s));
}

ECode CApplicationThread::ScheduleStopService(
    /* [in] */ IBinder* token)
{
    return mAThread->SendMessage(CActivityThread::H::STOP_SERVICE, token);
}

ECode CApplicationThread::BindApplication(
    /* [in] */ const String& processName,
    /* [in] */ IApplicationInfo* appInfo,
    /* [in] */ IList* providers,
    /* [in] */ IComponentName* instrumentationName,
    /* [in] */ IProfilerInfo* profilerInfo,
    /* [in] */ IBundle* instrumentationArgs,
    /* [in] */ IInstrumentationWatcher* instrumentationWatcher,
    /* [in] */ IIUiAutomationConnection* instrumentationUiConnection,
    /* [in] */ Int32 debugMode,
    /* [in] */ Boolean enableOpenGlTrace,
    /* [in] */ Boolean isRestrictedBackupMode,
    /* [in] */ Boolean persistent,
    /* [in] */ IConfiguration* config,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ IMap* services,
    /* [in] */ IBundle* coreSettings)
{
    if (services != NULL) {
        // Setup the service cache in the ServiceManager
        AutoPtr<CServiceManager> serviceManager;
        CServiceManager::AcquireSingletonByFriend((CServiceManager**)&serviceManager);
        serviceManager->InitServiceCache(services);
    }
    SetCoreSettings(coreSettings);

    /*
     * Two possible indications that this package could be
     * sharing its runtime with other packages:
     *
     * 1.) the sharedUserId attribute is set in the manifest,
     *     indicating a request to share a VM with other
     *     packages with the same sharedUserId.
     *
     * 2.) the application element of the manifest has an
     *     attribute specifying a non-default process name,
     *     indicating the desire to run in another packages VM.
     *
     * If sharing is enabled we do not have a unique application
     * in a process and therefore cannot rely on the package
     * name inside the runtime.
     */
    String packageName;
    IPackageItemInfo::Probe(appInfo)->GetPackageName(&packageName);
    AutoPtr<IIPackageManager> pm = CActivityThread::GetPackageManager();
    AutoPtr<IPackageInfo> pi;
    pm->GetPackageInfo(packageName, 0, UserHandle::GetMyUserId(), (IPackageInfo**)&pi);

    if (pi != NULL) {
        String uid;
        pi->GetSharedUserId(&uid);
        AutoPtr<IApplicationInfo> ai;
        pi->GetApplicationInfo((IApplicationInfo**)&ai);
        Boolean sharedUserIdSet = !uid.IsNull();
        Boolean processNameNotDefault = FALSE;
        if (ai != NULL) {
            String processName;
            ai->GetProcessName(&processName);
            processNameNotDefault = !packageName.Equals(processName);
        }
        Boolean sharable = (sharedUserIdSet || processNameNotDefault);

        // Tell the VMRuntime about the application, unless it is shared
        // inside a process.
        if (!sharable) {
            // VMRuntime.registerAppInfo(packageName, appInfo.dataDir,
            //                         appInfo.processName);
        }
    }

    AutoPtr<CActivityThread::AppBindData> data = new CActivityThread::AppBindData();
    data->mProcessName = processName;
    data->mAppInfo = appInfo;
    data->mProviders = providers;
    data->mInstrumentationName = instrumentationName;
    data->mInstrumentationArgs = instrumentationArgs;
    data->mInstrumentationWatcher = instrumentationWatcher;
    data->mInstrumentationUiAutomationConnection = instrumentationUiConnection;
    data->mDebugMode = debugMode;
    data->mEnableOpenGlTrace = enableOpenGlTrace;
    data->mRestrictedBackupMode = isRestrictedBackupMode;
    data->mPersistent = persistent;
    data->mConfig = config;
    data->mCompatInfo = compatInfo;
    data->mInitProfilerInfo = profilerInfo;
    return mAThread->SendMessage(CActivityThread::H::BIND_APPLICATION, TO_IINTERFACE(data));
}

ECode CApplicationThread::ScheduleExit()
{
    return mAThread->SendMessage(CActivityThread::H::EXIT_APPLICATION, NULL);
}

ECode CApplicationThread::ScheduleSuicide()
{
    return mAThread->SendMessage(CActivityThread::H::SUICIDE, NULL);
}

ECode CApplicationThread::ScheduleConfigurationChanged(
    /* [in] */ IConfiguration* config)
{
    UpdatePendingConfiguration(config);

    return mAThread->SendMessage(CActivityThread::H::CONFIGURATION_CHANGED, config);
}

ECode CApplicationThread::UpdateTimeZone()
{
    AutoPtr<ITimeZoneHelper> helper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
    return helper->SetDefault(NULL);
}

ECode CApplicationThread::ClearDnsCache()
{
    // a non-standard API to get this to libcore
    AutoPtr<IInetAddressHelper> helper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
    helper->ClearDnsCache();

    // Allow libcore to perform the necessary actions as it sees fit upon a network
    // configuration change.
    AutoPtr<INetworkEventDispatcherHelper> nedHelper;
    CNetworkEventDispatcherHelper::AcquireSingleton((INetworkEventDispatcherHelper**)&nedHelper);
    AutoPtr<INetworkEventDispatcher> ned;
    nedHelper->GetInstance((INetworkEventDispatcher**)&ned);
    return ned->OnNetworkConfigurationChanged();
}

ECode CApplicationThread::SetHttpProxy(
    /* [in] */ const String& host,
    /* [in] */ const String& port,
    /* [in] */ const String& exclList,
    /* [in] */ IUri* pacFileUrl)
{
    return Proxy::SetHttpProxySystemProperty(host, port, exclList, pacFileUrl);
}

ECode CApplicationThread::ProcessInBackground()
{
    mAThread->mH->RemoveMessages(CActivityThread::H::GC_WHEN_IDLE);
    AutoPtr<IMessage> msg;
    mAThread->mH->ObtainMessage(CActivityThread::H::GC_WHEN_IDLE, (IMessage**)&msg);
    Boolean bval;
    return mAThread->mH->SendMessage(msg, &bval);
}

ECode CApplicationThread::DumpService(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IBinder* servicetoken,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<CActivityThread::DumpComponentInfo> data = new CActivityThread::DumpComponentInfo();
//     try {
//         data.fd = ParcelFileDescriptor.dup(fd);
    data->mToken = servicetoken;
    data->mArgs->Copy(args);

    return mAThread->SendMessage(CActivityThread::H::DUMP_SERVICE, TO_IINTERFACE(data),
        0, 0, TRUE /*async*/);
//     } catch (IOException e) {
//         Slog.w(TAG, "dumpService failed", e);
//     }
}

ECode CApplicationThread::ScheduleRegisteredReceiver(
    /* [in] */ IIntentReceiver* receiver,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& dataStr,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser,
    /* [in] */ Int32 processState)
{
    UpdateProcessState(processState, false);
    return receiver->PerformReceive(intent, resultCode, dataStr,
            extras, ordered, sticky, sendingUser);
}

ECode CApplicationThread::ScheduleLowMemory()
{
    return mAThread->SendMessage(CActivityThread::H::LOW_MEMORY, NULL);
}

ECode CApplicationThread::ScheduleActivityConfigurationChanged(
    /* [in] */ IBinder* token)
{
    return mAThread->SendMessage(CActivityThread::H::ACTIVITY_CONFIGURATION_CHANGED, token);
}

ECode CApplicationThread::ProfilerControl(
    /* [in] */ Boolean start,
    /* [in] */ IProfilerInfo* profilerInfo,
    /* [in] */ Int32 profileType)
{
    return mAThread->SendMessage(CActivityThread::H::PROFILER_CONTROL,
        profilerInfo, start ? 1 : 0, profileType);
}

ECode CApplicationThread::DumpHeap(
    /* [in] */ Boolean managed,
    /* [in] */ const String& path,
    /* [in] */ IParcelFileDescriptor* fd)
{
    AutoPtr<CActivityThread::DumpHeapData> dhd = new CActivityThread::DumpHeapData();
    dhd->mPath = path;
    dhd->mFd = fd;

    return mAThread->SendMessage(CActivityThread::H::DUMP_HEAP, TO_IINTERFACE(dhd),
        managed ? 1 : 0, 0, true /*async*/);
}

ECode CApplicationThread::SetSchedulingGroup(
    /* [in] */ Int32 group)
{
    // Note: do this immediately, since going into the foreground
    // should happen regardless of what pending work we have to do
    // and the activity manager will wait for us to report back that
    // we are done before sending us to the background.
//     try {
    Process::SetProcessGroup(Process::MyPid(), group);
//     } catch (Exception e) {
//         Slog.w(TAG, "Failed setting process group to " + group, e);
//     }
    return NOERROR;
}

ECode CApplicationThread::DispatchPackageBroadcast(
    /* [in] */ Int32 cmd,
    /* [in] */ ArrayOf<String>* packages)
{
    AutoPtr<IArrayList> container;

    if (packages != NULL) {
        CArrayList::New((IArrayList**)&container);
        for (Int32 i = 0; i < packages->GetLength(); ++i) {
            AutoPtr<ICharSequence> seq = CoreUtils::Convert((*packages)[i]);
            container->Add(seq.Get());
        }
    }

    return mAThread->SendMessage(CActivityThread::H::DISPATCH_PACKAGE_BROADCAST,
        container, cmd);
}

ECode CApplicationThread::ScheduleCrash(
    /* [in] */ const String& msg)
{
    AutoPtr<ICharSequence> seq;
    CString::New(msg, (ICharSequence**)&seq);

    return mAThread->SendMessage(CActivityThread::H::SCHEDULE_CRASH, seq);
}

ECode CApplicationThread::DumpActivity(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IBinder* activitytoken,
    /* [in] */ const String& prefix,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<CActivityThread::DumpComponentInfo> data = new CActivityThread::DumpComponentInfo();
//     try {
//         data.fd = ParcelFileDescriptor.dup(fd);
    data->mToken = activitytoken;
    data->mPrefix = prefix;
    data->mArgs->Copy(args);

    return mAThread->SendMessage(CActivityThread::H::DUMP_ACTIVITY, TO_IINTERFACE(data),
        0, 0, TRUE /*async*/);
//     } catch (IOException e) {
//         Slog.w(TAG, "dumpActivity failed", e);
//     }
}

ECode CApplicationThread::DumpProvider(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IBinder* providertoken,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<CActivityThread::DumpComponentInfo> data = new CActivityThread::DumpComponentInfo();
//     try {
//         data.fd = ParcelFileDescriptor.dup(fd);
    data->mToken = providertoken;
    data->mArgs->Copy(args);

    return mAThread->SendMessage(CActivityThread::H::DUMP_PROVIDER, TO_IINTERFACE(data),
        0, 0, TRUE /*async*/);
//     } catch (IOException e) {
//         Slog.w(TAG, "dumpProvider failed", e);
//     }
}

ECode CApplicationThread::DumpMemInfo(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IDebugMemoryInfo* mem,
    /* [in] */ Boolean checkin,
    /* [in] */ Boolean dumpFullInfo,
    /* [in] */ Boolean dumpDalvik,
    /* [in] */ ArrayOf<String>* args)
{
//     FileOutputStream fout = new FileOutputStream(fd);
//     PrintWriter pw = new PrintWriter(fout);
//     try {
//         return dumpMemInfo(pw, checkin, all);
//     } finally {
//         pw.flush();
//     }
    return E_NOT_IMPLEMENTED;
}

ECode CApplicationThread::DumpMemInfo(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ IDebugMemoryInfo* mem,
    /* [in] */ Boolean checkin,
    /* [in] */ Boolean dumpFullInfo,
    /* [in] */ Boolean dumpDalvik)
{
//     long nativeMax = Debug.getNativeHeapSize() / 1024;
//     long nativeAllocated = Debug.getNativeHeapAllocatedSize() / 1024;
//     long nativeFree = Debug.getNativeHeapFreeSize() / 1024;

//     Runtime runtime = Runtime.getRuntime();

//     long dalvikMax = runtime.totalMemory() / 1024;
//     long dalvikFree = runtime.freeMemory() / 1024;
//     long dalvikAllocated = dalvikMax - dalvikFree;
//     long viewInstanceCount = ViewDebug.getViewInstanceCount();
//     long viewRootInstanceCount = ViewDebug.getViewRootImplCount();
//     long appContextInstanceCount = Debug.countInstancesOfClass(ContextImpl.class);
//     long activityInstanceCount = Debug.countInstancesOfClass(Activity.class);
//     int globalAssetCount = AssetManager.getGlobalAssetCount();
//     int globalAssetManagerCount = AssetManager.getGlobalAssetManagerCount();
//     int binderLocalObjectCount = Debug.getBinderLocalObjectCount();
//     int binderProxyObjectCount = Debug.getBinderProxyObjectCount();
//     int binderDeathObjectCount = Debug.getBinderDeathObjectCount();
//     long openSslSocketCount = Debug.countInstancesOfClass(OpenSSLSocketImpl.class);
//     SQLiteDebug.PagerStats stats = SQLiteDebug.getDatabaseInfo();

        // dumpMemInfoTable(pw, memInfo, checkin, dumpFullInfo, dumpDalvik, Process.myPid(),
        //         (mBoundApplication != NULL) ? mBoundApplication.processName : "unknown",
        //         nativeMax, nativeAllocated, nativeFree,
        //         dalvikMax, dalvikAllocated, dalvikFree);

//     if (checkin) {
//         // NOTE: if you change anything significant below, also consider changing
//         // ACTIVITY_THREAD_CHECKIN_VERSION.

//         // Object counts
//         pw.print(viewInstanceCount); pw.print(',');
//         pw.print(viewRootInstanceCount); pw.print(',');
//         pw.print(appContextInstanceCount); pw.print(',');
//         pw.print(activityInstanceCount); pw.print(',');

//         pw.print(globalAssetCount); pw.print(',');
//         pw.print(globalAssetManagerCount); pw.print(',');
//         pw.print(binderLocalObjectCount); pw.print(',');
//         pw.print(binderProxyObjectCount); pw.print(',');

//         pw.print(binderDeathObjectCount); pw.print(',');
//         pw.print(openSslSocketCount); pw.print(',');

//         // SQL
//         pw.print(stats.memoryUsed / 1024); pw.print(',');
//         pw.print(stats.memoryUsed / 1024); pw.print(',');
//         pw.print(stats.pageCacheOverflow / 1024); pw.print(',');
//         pw.print(stats.largestMemAlloc / 1024);
//         for (int i = 0; i < stats.dbStats.size(); i++) {
//             DbStats dbStats = stats.dbStats.get(i);
//             pw.print(','); pw.print(dbStats.dbName);
//             pw.print(','); pw.print(dbStats.pageSize);
//             pw.print(','); pw.print(dbStats.dbSize);
//             pw.print(','); pw.print(dbStats.lookaside);
//             pw.print(','); pw.print(dbStats.cache);
//             pw.print(','); pw.print(dbStats.cache);
//         }
//         pw.println();

//         return memInfo;
//     }

//     pw.println(" ");
//     pw.println(" Objects");
//     printRow(pw, TWO_COUNT_COLUMNS, "Views:", viewInstanceCount, "ViewRootImpl:",
//             viewRootInstanceCount);

//     printRow(pw, TWO_COUNT_COLUMNS, "AppContexts:", appContextInstanceCount,
//             "Activities:", activityInstanceCount);

//     printRow(pw, TWO_COUNT_COLUMNS, "Assets:", globalAssetCount,
//             "AssetManagers:", globalAssetManagerCount);

//     printRow(pw, TWO_COUNT_COLUMNS, "Local Binders:", binderLocalObjectCount,
//             "Proxy Binders:", binderProxyObjectCount);
//     printRow(pw, ONE_COUNT_COLUMN, "Death Recipients:", binderDeathObjectCount);

//     printRow(pw, ONE_COUNT_COLUMN, "OpenSSL Sockets:", openSslSocketCount);

//     // SQLite mem info
//     pw.println(" ");
//     pw.println(" SQL");
//     printRow(pw, ONE_COUNT_COLUMN, "MEMORY_USED:", stats.memoryUsed / 1024);
//     printRow(pw, TWO_COUNT_COLUMNS, "PAGECACHE_OVERFLOW:",
//             stats.pageCacheOverflow / 1024, "MALLOC_SIZE:", stats.largestMemAlloc / 1024);
//     pw.println(" ");
//     int N = stats.dbStats.size();
//     if (N > 0) {
//         pw.println(" DATABASES");
//         printRow(pw, "  %8s %8s %14s %14s  %s", "pgsz", "dbsz", "Lookaside(b)", "cache",
//                 "Dbname");
//         for (int i = 0; i < N; i++) {
//             DbStats dbStats = stats.dbStats.get(i);
//             printRow(pw, DB_INFO_FORMAT,
//                     (dbStats.pageSize > 0) ? String.valueOf(dbStats.pageSize) : " ",
//                     (dbStats.dbSize > 0) ? String.valueOf(dbStats.dbSize) : " ",
//                     (dbStats.lookaside > 0) ? String.valueOf(dbStats.lookaside) : " ",
//                     dbStats.cache, dbStats.dbName);
//         }
//     }

//     // Asset details.
//     String assetAlloc = AssetManager.getAssetAllocations();
//     if (assetAlloc != NULL) {
//         pw.println(" ");
//         pw.println(" Asset Allocations");
//         pw.print(assetAlloc);
//     }
    return NOERROR;
}

ECode CApplicationThread::DumpGfxInfo(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<String>* args)
{
//     dumpGraphicsInfo(fd);
//     WindowManagerGlobal.getInstance().dumpGfxInfo(fd);
    return E_NOT_IMPLEMENTED;
}

ECode CApplicationThread::DumpDbInfo(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<String>* args)
{
//     PrintWriter pw = new PrintWriter(new FileOutputStream(fd));
//     PrintWriterPrinter printer = new FastPrintWriter(pw);
//     SQLiteDebug.dump(printer, args);
//     pw.flush();
    return E_NOT_IMPLEMENTED;
}

ECode CApplicationThread::UnstableProviderDied(
    /* [in] */ IBinder* provider)
{
    return mAThread->SendMessage(CActivityThread::H::UNSTABLE_PROVIDER_DIED, provider);
}

ECode CApplicationThread::RequestAssistContextExtras(
    /* [in] */ IBinder* activityToken,
    /* [in] */ IBinder* requestToken,
    /* [in] */ Int32 requestType)
{
    AutoPtr<CActivityThread::RequestAssistContextExtras> cmd = new CActivityThread::RequestAssistContextExtras();
    cmd->mActivityToken = activityToken;
    cmd->mRequestToken = requestToken;
    cmd->mRequestType = requestType;
    return mAThread->SendMessage(CActivityThread::H::REQUEST_ASSIST_CONTEXT_EXTRAS, TO_IINTERFACE(cmd));
}

ECode CApplicationThread::SetCoreSettings(
    /* [in] */ IBundle* coreSettings)
{
    return mAThread->SendMessage(CActivityThread::H::SET_CORE_SETTINGS, coreSettings);
}

ECode CApplicationThread::UpdatePackageCompatibilityInfo(
    /* [in] */ const String& pkg,
    /* [in] */ ICompatibilityInfo* info)
{
    AutoPtr<CActivityThread::UpdateCompatibilityData> ucd = new CActivityThread::UpdateCompatibilityData();
    ucd->mPkg = pkg;
    ucd->mInfo = info;

    return mAThread->SendMessage(CActivityThread::H::UPDATE_PACKAGE_COMPATIBILITY_INFO, TO_IINTERFACE(ucd));
}

ECode CApplicationThread::ScheduleTrimMemory(
    /* [in] */ Int32 level)
{
    return mAThread->SendMessage(CActivityThread::H::TRIM_MEMORY, NULL, level);
}

ECode CApplicationThread::ScheduleTranslucentConversionComplete(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean drawComplete)
{
    return mAThread->SendMessage(CActivityThread::H::TRANSLUCENT_CONVERSION_COMPLETE, token, drawComplete ? 1 : 0);
}

ECode CApplicationThread::ScheduleOnNewActivityOptions(
    /* [in] */ IBinder* token,
    /* [in] */ IActivityOptions* options)
{
    AutoPtr<IPairHelper> helper;
    CPairHelper::AcquireSingleton((IPairHelper**)&helper);
    AutoPtr<IPair> pair;
    helper->Create(token, options, (IPair**)&pair);
    return mAThread->SendMessage(CActivityThread::H::ON_NEW_ACTIVITY_OPTIONS, pair);
}

ECode CApplicationThread::SetProcessState(
    /* [in] */ Int32 state)
{
    return UpdateProcessState(state, TRUE);
}

ECode CApplicationThread::UpdateProcessState(
    /* [in] */ Int32 processState,
    /* [in] */ Boolean fromIpc)
{
    synchronized(this) {
        if (mLastProcessState != processState) {
            mLastProcessState = processState;
            // Update Dalvik state based on ActivityManager.PROCESS_STATE_* constants.
            Int32 DALVIK_PROCESS_STATE_JANK_PERCEPTIBLE = 0;
            Int32 DALVIK_PROCESS_STATE_JANK_IMPERCEPTIBLE = 1;
            Int32 dalvikProcessState = DALVIK_PROCESS_STATE_JANK_IMPERCEPTIBLE;
            // TODO: Tune this since things like gmail sync are important background but not jank perceptible.
            if (processState <= IActivityManager::PROCESS_STATE_IMPORTANT_FOREGROUND) {
                dalvikProcessState = DALVIK_PROCESS_STATE_JANK_PERCEPTIBLE;
            }
            // TODO:
            // VMRuntime.getRuntime().UpdateProcessState(dalvikProcessState);
            if (FALSE) {
                Slogger::I("CApplicationThread", "******************* PROCESS STATE CHANGED TO: %d %s",
                    processState ,(fromIpc ? " (from ipc": ""));
            }
        }
    }
    return NOERROR;
}

ECode CApplicationThread::ScheduleInstallProvider(
    /* [in] */ IProviderInfo* provider)
{
    return mAThread->SendMessage(CActivityThread::H::INSTALL_PROVIDER, provider);
}

ECode CApplicationThread::UpdateTimePrefs(
    /* [in] */ Boolean is24Hour)
{
    AutoPtr<IDateFormatHelper> helper;
    CDateFormatHelper::AcquireSingleton((IDateFormatHelper**)&helper);
    return helper->Set24HourTimePref(is24Hour);
}

ECode CApplicationThread::ScheduleCancelVisibleBehind(
    /* [in] */ IBinder* token)
{
    return mAThread->SendMessage(CActivityThread::H::CANCEL_VISIBLE_BEHIND, token);
}

ECode CApplicationThread::ScheduleBackgroundVisibleBehindChanged(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean visible)
{
    return mAThread->SendMessage(CActivityThread::H::BACKGROUND_VISIBLE_BEHIND_CHANGED, token, visible ? 1 : 0);
}

ECode CApplicationThread::ScheduleEnterAnimationComplete(
    /* [in] */ IBinder* token)
{
    return mAThread->SendMessage(CActivityThread::H::ENTER_ANIMATION_COMPLETE, token);
}

ECode CApplicationThread::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);
    String info("CApplicationThread:");
    info.AppendFormat(" %p, activity thread: %p", this, mAThread);
    *string = info;
    return NOERROR;
}


} // namespace App
} // namespace Droid
} // namespace Elastos

