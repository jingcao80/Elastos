
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/am/CProcessStatsService.h"
#include "elastos/droid/server/am/ProcessList.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/droid/os/Process.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Internal::App::CProcessStateHolder;
using Elastos::Droid::Internal::App::IProcessState;
using Elastos::Droid::Internal::App::IProcessStats;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL(ProcessRecord, Object, IProcessRecord)

ProcessRecord::ProcessRecord(
    /* [in] */ IBatteryStatsImpl* batteryStats,
    /* [in] */ IApplicationInfo* info,
    /* [in] */ const String& processName,
    /* [in] */ Int32 uid)
    : mBatteryStats(batteryStats)
    , mInfo(info)
    , mIsolated(FALSE)
    , mUid(uid)
    , mUserId(0)
    , mProcessName(processName)
    , mPid(0)
    , mStarting(FALSE)
    , mLastActivityTime(0)
    , mLastPssTime(SystemClock::GetUptimeMillis())
    , mNextPssTime(mLastPssTime)
    , mLastStateTime(mLastPssTime)
    , mInitialIdlePss(0)
    , mLastPss(0)
    , mLastCachedPss(0)
    , mMaxAdj(ProcessList::UNKNOWN_ADJ)
    , mCurRawAdj(-100)
    , mSetRawAdj(-100)
    , mCurAdj(-100)
    , mSetAdj(-100)
    , mCurSchedGroup(0)
    , mTrimMemoryLevel(0)
    , mCurProcState(-1)
    , mRepProcState(-1)
    , mSetProcState(-1)
    , mPssProcState(-1)
    , mServiceb(FALSE)
    , mServiceHighRam(FALSE)
    , mSetIsForeground(FALSE)
    , mNotCachedSinceIdle(FALSE)
    , mHasClientActivities(FALSE)
    , mHasStartedServices(FALSE)
    , mForegroundServices(FALSE)
    , mForegroundActivities(FALSE)
    , mRepForegroundActivities(FALSE)
    , mSystemNoUi(FALSE)
    , mHasShownUi(FALSE)
    , mPendingUiClean(FALSE)
    , mHasAboveClient(FALSE)
    , mTreatLikeActivity(FALSE)
    , mBad(FALSE)
    , mKilledByAm(FALSE)
    , mKilled(FALSE)
    , mProcStateChanged(FALSE)
    , mAdjSeq(0)
    , mLruSeq(0)
    , mUsingWrapper(FALSE)
    , mLastWakeTime(0)
    , mLastCpuTime(0)
    , mCurCpuTime(0)
    , mLastRequestedGc(0)
    , mLastLowMemory(0)
    , mReportLowMemory(FALSE)
    , mEmpty(FALSE)
    , mCached(FALSE)
    , mAdjTypeCode(0)
    , mAdjSourceProcState(0)
    , mExecServicesFg(FALSE)
    , mPersistent(FALSE)
    , mCrashing(FALSE)
    , mForceCrashReport(FALSE)
    , mNotResponding(FALSE)
    , mRemoved(FALSE)
    , mDebugging(FALSE)
    , mWaitedForDebugger(FALSE)
{
    assert(info != NULL && "IApplicationInfo can not be NULL when creating ProcessRecord.");

    Int32 appUid;
    info->GetUid(&appUid);
    String pkgName;
    IPackageItemInfo::Probe(info)->GetPackageName(&pkgName);
    Int32 versionCode;
    info->GetVersionCode(&versionCode);

    mIsolated = appUid != mUid;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetUserId(uid, &mUserId);
    CArrayMap::New((IArrayMap**)&mPkgList);
    AutoPtr<IProcessStateHolder> procStateHolder;
    CProcessStateHolder::New(versionCode, (IProcessStateHolder**)&procStateHolder);
    mPkgList->Put(CoreUtils::Convert(pkgName), procStateHolder);
}

ProcessRecord::~ProcessRecord()
{
    Slogger::D("ProcessRecord", " =========== ~ProcessRecord(): %s", ToString().string());
}

void ProcessRecord::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    // Int64 now = SystemClock::GetUptimeMillis();

    // pw->Print(prefix);
    // pw->Print("user #");
    // pw->Print(userId);
    // pw->Print(" uid=");
    // pw->Print(info.uid);
    // if (uid != info.uid) {
    //     pw->Print(" ISOLATED uid=");
    //     pw->Print(uid);
    // }
    // pw->Println();
    // if (info.className != null) {
    //     pw->Print(prefix);
    //     pw->Print("class=");
    //     pw->Println(info.className);
    // }
    // if (info.manageSpaceActivityName != null) {
    //     pw->Print(prefix);
    //     pw->Print("manageSpaceActivityName=");
    //     pw->Println(info.manageSpaceActivityName);
    // }
    // pw->Print(prefix);
    // pw->Print("dir=");
    // pw->Print(info.sourceDir);
    // pw->Print(" publicDir=");
    // pw->Print(info.publicSourceDir);
    // pw->Print(" data=");
    // pw->Println(info.dataDir);
    // pw->Print(prefix);
    // pw->Print("packageList={");
    // for (int i=0; i<pkgList.size(); i++) {
    //     if (i > 0) pw->Print(", ");
    //     pw->Print(pkgList.keyAt(i));
    // }
    // pw->Println("}");
    // if (pkgDeps != null) {
    //     pw->Print(prefix);
    //     pw->Print("packageDependencies={");
    //     for (int i=0; i<pkgDeps.size(); i++) {
    //         if (i > 0) pw->Print(", ");
    //         pw->Print(pkgDeps.valueAt(i));
    //     }
    //     pw->Println("}");
    // }
    // pw->Print(prefix);
    // pw->Print("compat=");
    // pw->Println(compat);
    // if (instrumentationClass != null || instrumentationProfileFile != null
    //         || instrumentationArguments != null) {
    //     pw->Print(prefix);
    //     pw->Print("instrumentationClass=");
    //     pw->Print(instrumentationClass);
    //     pw->Print(" instrumentationProfileFile=");
    //     pw->Println(instrumentationProfileFile);
    //     pw->Print(prefix);
    //     pw->Print("instrumentationArguments=");
    //     pw->Println(instrumentationArguments);
    //     pw->Print(prefix);
    //     pw->Print("instrumentationInfo=");
    //     pw->Println(instrumentationInfo);
    //     if (instrumentationInfo != null) {
    //         instrumentationInfo.dump(new PrintWriterPrinter(pw), prefix + "  ");
    //     }
    // }
    // pw->Print(prefix);
    // pw->Print("thread=");
    // pw->Println(thread);
    // pw->Print(prefix);
    // pw->Print("pid=");
    // pw->Print(pid);
    // pw->Print(" starting=");
    // pw->Println(starting);
    // pw->Print(prefix);
    // pw->Print("lastActivityTime=");
    // TimeUtils::FormatDuration(lastActivityTime, now, pw);
    // pw->Print(" lastPssTime=");
    // TimeUtils::FormatDuration(lastPssTime, now, pw);
    // pw->Print(" nextPssTime=");
    // TimeUtils::FormatDuration(nextPssTime, now, pw);
    // pw->Println();
    // pw->Print(prefix);
    // pw->Print("adjSeq=");
    // pw->Print(adjSeq);
    // pw->Print(" lruSeq=");
    // pw->Print(lruSeq);
    // pw->Print(" lastPss=");
    // pw->Print(lastPss);
    // pw->Print(" lastCachedPss=");
    // pw->Println(lastCachedPss);
    // pw->Print(prefix);
    // pw->Print("cached=");
    // pw->Print(cached);
    // pw->Print(" empty=");
    // pw->Println(empty);
    // if (serviceb) {
    //     pw->Print(prefix);
    //     pw->Print("serviceb=");
    //     pw->Print(serviceb);
    //     pw->Print(" serviceHighRam=");
    //     pw->Println(serviceHighRam);
    // }
    // if (notCachedSinceIdle) {
    //     pw->Print(prefix);
    //     pw->Print("notCachedSinceIdle=");
    //     pw->Print(notCachedSinceIdle);
    //     pw->Print(" initialIdlePss=");
    //     pw->Println(initialIdlePss);
    // }
    // pw->Print(prefix);
    // pw->Print("oom: max=");
    // pw->Print(maxAdj);
    // pw->Print(" curRaw=");
    // pw->Print(curRawAdj);
    // pw->Print(" setRaw=");
    // pw->Print(setRawAdj);
    // pw->Print(" cur=");
    // pw->Print(curAdj);
    // pw->Print(" set=");
    // pw->Println(setAdj);
    // pw->Print(prefix);
    // pw->Print("curSchedGroup=");
    // pw->Print(curSchedGroup);
    // pw->Print(" setSchedGroup=");
    // pw->Print(setSchedGroup);
    // pw->Print(" systemNoUi=");
    // pw->Print(systemNoUi);
    // pw->Print(" trimMemoryLevel=");
    // pw->Println(trimMemoryLevel);
    // pw->Print(prefix);
    // pw->Print("curProcState=");
    // pw->Print(curProcState);
    // pw->Print(" repProcState=");
    // pw->Print(repProcState);
    // pw->Print(" pssProcState=");
    // pw->Print(pssProcState);
    // pw->Print(" setProcState=");
    // pw->Print(setProcState);
    // pw->Print(" lastStateTime=");
    // TimeUtils.formatDuration(lastStateTime, now, pw);
    // pw->Println();
    // if (hasShownUi || pendingUiClean || hasAboveClient || treatLikeActivity) {
    //     pw->Print(prefix);
    //     pw->Print("hasShownUi=");
    //     pw->Print(hasShownUi);
    //     pw->Print(" pendingUiClean=");
    //     pw->Print(pendingUiClean);
    //     pw->Print(" hasAboveClient=");
    //     pw->Print(hasAboveClient);
    //     pw->Print(" treatLikeActivity=");
    //     pw->Println(treatLikeActivity);
    // }
    // if (setIsForeground || foregroundServices || forcingToForeground != null) {
    //     pw->Print(prefix);
    //     pw->Print("setIsForeground=");
    //     pw->Print(setIsForeground);
    //     pw->Print(" foregroundServices=");
    //     pw->Print(foregroundServices);
    //     pw->Print(" forcingToForeground=");
    //     pw->Println(forcingToForeground);
    // }
    // if (persistent || removed) {
    //     pw->Print(prefix);
    //     pw->Print("persistent=");
    //     pw->Print(persistent);
    //     pw->Print(" removed=");
    //     pw->Println(removed);
    // }
    // if (hasClientActivities || foregroundActivities || repForegroundActivities) {
    //     pw->Print(prefix);
    //     pw->Print("hasClientActivities=");
    //     pw->Print(hasClientActivities);
    //     pw->Print(" foregroundActivities=");
    //     pw->Print(foregroundActivities);
    //     pw->Print(" (rep=");
    //     pw->Print(repForegroundActivities);
    //     pw->Println(")");
    // }
    // if (hasStartedServices) {
    //     pw->Print(prefix);
    //     pw->Print("hasStartedServices=");
    //     pw->Println(hasStartedServices);
    // }
    // if (setProcState >= ActivityManager.PROCESS_STATE_SERVICE) {
    //     long wtime;
    //     synchronized (mBatteryStats) {
    //         wtime = mBatteryStats.getProcessWakeTime(info.uid,
    //                 pid, SystemClock.elapsedRealtime());
    //     }
    //     pw->Print(prefix);
    //     pw->Print("lastWakeTime=");
    //     pw->Print(lastWakeTime);
    //     pw->Print(" timeUsed=");
    //     TimeUtils.formatDuration(wtime-lastWakeTime, pw);
    //     pw->Println("");
    //     pw->Print(prefix);
    //     pw->Print("lastCpuTime=");
    //     pw->Print(lastCpuTime);
    //     pw->Print(" timeUsed=");
    //     TimeUtils.formatDuration(curCpuTime-lastCpuTime, pw);
    //     pw->Println("");
    // }
    // pw->Print(prefix);
    // pw->Print("lastRequestedGc=");
    // TimeUtils.formatDuration(lastRequestedGc, now, pw);
    // pw->Print(" lastLowMemory=");
    // TimeUtils.formatDuration(lastLowMemory, now, pw);
    // pw->Print(" reportLowMemory=");
    // pw->Println(reportLowMemory);
    // if (killed || killedByAm || waitingToKill != null) {
    //     pw->Print(prefix);
    //     pw->Print("killed=");
    //     pw->Print(killed);
    //     pw->Print(" killedByAm=");
    //     pw->Print(killedByAm);
    //     pw->Print(" waitingToKill=");
    //     pw->Println(waitingToKill);
    // }
    // if (debugging || crashing || crashDialog != null || notResponding
    //         || anrDialog != null || bad) {
    //     pw->Print(prefix);
    //     pw->Print("debugging=");
    //     pw->Print(debugging);
    //     pw->Print(" crashing=");
    //     pw->Print(crashing);
    //     pw->Print(" ");
    //     pw->Print(crashDialog);
    //     pw->Print(" notResponding=");
    //     pw->Print(notResponding);
    //     pw->Print(" " );
    //     pw->Print(anrDialog);
    //     pw->Print(" bad=");
    //     pw->Print(bad);

    //     // crashing or notResponding is always set before errorReportReceiver
    //     if (errorReportReceiver != null) {
    //         pw->Print(" errorReportReceiver=");
    //         pw->Print(errorReportReceiver.flattenToShortString());
    //     }
    //     pw->Println();
    // }
    // if (activities.size() > 0) {
    //     pw->Print(prefix);
    //     pw->Println("Activities:");
    //     for (int i=0; i<activities.size(); i++) {
    //         pw->Print(prefix);
    //         pw->Print("  - ");
    //         pw->Println(activities.get(i));
    //     }
    // }
    // if (services.size() > 0) {
    //     pw->Print(prefix);
    //     pw->Println("Services:");
    //     for (int i=0; i<services.size(); i++) {
    //         pw->Print(prefix);
    //         pw->Print("  - ");
    //         pw->Println(services.valueAt(i));
    //     }
    // }
    // if (executingServices.size() > 0) {
    //     pw->Print(prefix);
    //     pw->Print("Executing Services (fg=");
    //     pw->Print(execServicesFg);
    //     pw->Println(")");
    //     for (int i=0; i<executingServices.size(); i++) {
    //         pw->Print(prefix);
    //         pw->Print("  - ");
    //         pw->Println(executingServices.valueAt(i));
    //     }
    // }
    // if (connections.size() > 0) {
    //     pw->Print(prefix);
    //     pw->Println("Connections:");
    //     for (int i=0; i<connections.size(); i++) {
    //         pw->Print(prefix);
    //         pw->Print("  - ");
    //         pw->Println(connections.valueAt(i));
    //     }
    // }
    // if (pubProviders.size() > 0) {
    //     pw->Print(prefix);
    //     pw->Println("Published Providers:");
    //     for (int i=0; i<pubProviders.size(); i++) {
    //         pw->Print(prefix);
    //         pw->Print("  - ");
    //         pw->Println(pubProviders.keyAt(i));
    //         pw->Print(prefix);
    //         pw->Print("    -> ");
    //         pw->Println(pubProviders.valueAt(i));
    //     }
    // }
    // if (conProviders.size() > 0) {
    //     pw->Print(prefix);
    //     pw->Println("Connected Providers:");
    //     for (int i=0; i<conProviders.size(); i++) {
    //         pw->Print(prefix);
    //         pw->Print("  - ");
    //         pw->Println(conProviders.get(i).toShortString());
    //     }
    // }
    // if (curReceiver != null) {
    //     pw->Print(prefix);
    //     pw->Print("curReceiver=");
    //     pw->Println(curReceiver);
    // }
    // if (receivers.size() > 0) {
    //     pw->Print(prefix);
    //     pw->Println("Receivers:");
    //     for (int i=0; i<receivers.size(); i++) {
    //         pw->Print(prefix);
    //         pw->Print("  - ");
    //         pw->Println(receivers.valueAt(i));
    //     }
    // }
}

ECode ProcessRecord::SetPid(
    /* [in] */ Int32 pid)
{
    mPid = pid;
    mShortStringName = NULL;
    mStringName = NULL;
    return NOERROR;
}

void ProcessRecord::MakeActive(
    /* [in] */ IApplicationThread* thread,
    /* [in] */ CProcessStatsService* tracker)
{
    if (mThread == NULL) {
        AutoPtr<IProcessState> origBase = mBaseProcessTracker;
        if (origBase != NULL) {
            origBase->SetState(IProcessStats::STATE_NOTHING,
                tracker->GetMemFactorLocked(), SystemClock::GetUptimeMillis(), mPkgList);
            origBase->MakeInactive();
        }
        Int32 uid;
        mInfo->GetUid(&uid);
        String packageName;
        IPackageItemInfo::Probe(mInfo)->GetPackageName(&packageName);
        Int32 versionCode;
        mInfo->GetVersionCode(&versionCode);
        mBaseProcessTracker = tracker->GetProcessStateLocked(packageName, uid,
            versionCode, mProcessName);
        mBaseProcessTracker->MakeActive();
        Int32 size;
        mPkgList->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> value;
            mPkgList->GetValueAt(i, (IInterface**)&value);
            AutoPtr<IProcessStateHolder> holder = IProcessStateHolder::Probe(value);
            AutoPtr<IProcessState> state;
            holder->GetState((IProcessState**)&state);
            if (state != NULL && state != origBase) {
                state->MakeInactive();
            }
            AutoPtr<IInterface> key;
            mPkgList->GetKeyAt(i, (IInterface**)&key);
            String strKey;
            ICharSequence::Probe(key)->ToString(&strKey);
            state = tracker->GetProcessStateLocked(strKey, uid,
                    versionCode, mProcessName);
            holder->SetState(state);
            if (state != mBaseProcessTracker) {
                state->MakeActive();
            }
        }
    }
    mThread = thread;
}

void ProcessRecord::MakeInactive(
    /* [in] */ CProcessStatsService* tracker)
{
    mThread = NULL;
    AutoPtr<IProcessState> origBase = mBaseProcessTracker;
    if (origBase != NULL) {
        if (origBase != NULL) {
            origBase->SetState(IProcessStats::STATE_NOTHING,
                    tracker->GetMemFactorLocked(), SystemClock::GetUptimeMillis(), mPkgList);
            origBase->MakeInactive();
        }
        mBaseProcessTracker = NULL;
        Int32 size;
        mPkgList->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> value;
            mPkgList->GetValueAt(i, (IInterface**)&value);
            AutoPtr<IProcessStateHolder> holder = IProcessStateHolder::Probe(value);
            AutoPtr<IProcessState> state;
            holder->GetState((IProcessState**)&state);
            if (state != NULL && state != origBase) {
                state->MakeInactive();
            }
            holder->SetState(NULL);
        }
    }
}

Boolean ProcessRecord::IsInterestingToUserLocked()
{
    List< AutoPtr<ActivityRecord> >::Iterator iter;
    for (iter = mActivities.Begin(); iter != mActivities.End(); ++iter) {
        AutoPtr<ActivityRecord> r = *iter;
        if (r->IsInterestingToUserLocked()) {
            return TRUE;
        }
    }

    return FALSE;
}

ECode ProcessRecord::StopFreezingAllLocked()
{
    List< AutoPtr<ActivityRecord> >::ReverseIterator rit;
    for (rit = mActivities.RBegin(); rit != mActivities.REnd(); ++rit) {
        AutoPtr<ActivityRecord> r = *rit;
        r->StopFreezingScreenLocked(TRUE);
    }
    return NOERROR;
}

ECode ProcessRecord::UnlinkDeathRecipient()
{
    if (mDeathRecipient != NULL && mThread != NULL) {
        AutoPtr<IProxy> proxy = (IProxy*)mThread->Probe(EIID_IProxy);
        assert(proxy != NULL);
        Boolean res;
        proxy->UnlinkToDeath(mDeathRecipient, 0, &res);
    }
    mDeathRecipient = NULL;
    return NOERROR;
}

ECode ProcessRecord::UpdateHasAboveClientLocked()
{
    mHasAboveClient = FALSE;
    if (mConnections.IsEmpty() == FALSE) {
        HashSet< AutoPtr<ConnectionRecord> >::Iterator it = mConnections.Begin();
        for (; it != mConnections.End(); ++it) {
            AutoPtr<ConnectionRecord> cr = *it;
            if ((cr->mFlags & IContext::BIND_ABOVE_CLIENT) != 0) {
                mHasAboveClient = TRUE;
                break;
            }
        }
    }
    return NOERROR;
}

Int32 ProcessRecord::ModifyRawOomAdj(
    /* [in] */ Int32 adj)
{
    if (mHasAboveClient) {
        // If this process has bound to any services with BIND_ABOVE_CLIENT,
        // then we need to drop its adjustment to be lower than the service's
        // in order to honor the request.  We want to drop it by one adjustment
        // level...  but there is special meaning applied to various levels so
        // we will skip some of them.
        if (adj < ProcessList::FOREGROUND_APP_ADJ) {
            // System process will not get dropped, ever
        }
        else if (adj < ProcessList::VISIBLE_APP_ADJ) {
            adj = ProcessList::VISIBLE_APP_ADJ;
        }
        else if (adj < ProcessList::PERCEPTIBLE_APP_ADJ) {
            adj = ProcessList::PERCEPTIBLE_APP_ADJ;
        }
        else if (adj < ProcessList::CACHED_APP_MIN_ADJ) {
            adj = ProcessList::CACHED_APP_MIN_ADJ;
        }
        else if (adj < ProcessList::CACHED_APP_MAX_ADJ) {
            adj++;
        }
    }
    return adj;
}

void ProcessRecord::Kill(
    /* [in] */ const String& reason,
    /* [in] */ Boolean noisy)
{
    if (!mKilledByAm) {
        if (noisy) {
            Slogger::I(CActivityManagerService::TAG, "Killing %s (adj %d): %s",
                ToShortString().string(), mSetAdj, reason.string());
        }
        // EventLog.writeEvent(EventLogTags.AM_KILL, userId, pid, processName, setAdj, reason);
        Process::KillProcessQuiet(mPid);
        Int32 uid;
        mInfo->GetUid(&uid);
        Process::KillProcessGroup(uid, mPid);
        if (!mPersistent) {
            mKilled = TRUE;
            mKilledByAm = TRUE;
        }
    }
}

String ProcessRecord::ToShortString()
{
    if (!mShortStringName.IsNull()) {
        return mShortStringName;
    }
    StringBuilder sb;
    ToShortString(sb);
    mShortStringName = sb.ToString();
    return mShortStringName;
}

ECode ProcessRecord::ToShortString(
    /* [in] */ StringBuilder& sb)
{
    sb.Append(mPid);
    sb.AppendChar(':');
    sb.Append(mProcessName);
    sb.AppendChar('/');
    Int32 uid;
    mInfo->GetUid(&uid);
    if (uid < IProcess::FIRST_APPLICATION_UID) {
        sb.Append(uid);
    }
    else {
        sb.AppendChar('u');
        sb.Append(mUserId);
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        Int32 appId;
        helper->GetAppId(uid, &appId);
        if (appId >= IProcess::FIRST_APPLICATION_UID) {
            sb.AppendChar('a');
            sb.Append(appId - IProcess::FIRST_APPLICATION_UID);
        }
        else {
            sb.AppendChar('s');
            sb.Append(appId);
        }
        if (mUid != uid) {
            sb.AppendChar('i');
            helper->GetAppId(mUid, &appId);
            sb.Append(appId - IProcess::FIRST_ISOLATED_UID);
        }
    }
    return NOERROR;
}

String ProcessRecord::ToString()
{
    if (!mStringName.IsNull()) {
        return mStringName;
    }
    StringBuilder sb(128);
    sb += ("ProcessRecord{");
    sb += StringUtils::ToHexString((Int32)this);
    sb += (' ');
    ToShortString(sb);
    sb += ('}');
    return mStringName = sb.ToString();
}

ECode ProcessRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = ToString();
    return NOERROR;
}

String ProcessRecord::MakeAdjReason()
{
    if (mAdjSource != NULL || mAdjTarget != NULL) {
        StringBuilder sb(128);
        sb.AppendChar(' ');
        if (IComponentName::Probe(mAdjTarget)) {
            String str;
            IComponentName::Probe(mAdjTarget)->FlattenToShortString(&str);
            sb.Append(str);
        }
        else if (mAdjTarget != NULL) {
            sb.Append(mAdjTarget);
        }
        else {
            sb.Append("{NULL}");
        }
        sb.Append("<=");
        if (IProcessRecord::Probe(mAdjSource)) {
            sb.Append("Proc{");
            sb.Append(((ProcessRecord*)IProcessRecord::Probe(mAdjSource))->ToShortString());
            sb.Append("}");
        }
        else if (mAdjSource != NULL) {
            sb.Append(mAdjSource);
        }
        else {
            sb.Append("{NULL}");
        }
        return sb.ToString();
    }
    return String(NULL);
}

Boolean ProcessRecord::AddPackage(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 versionCode,
    /* [in] */ CProcessStatsService* tracker)
{
    Boolean containsKey;
    mPkgList->ContainsKey(CoreUtils::Convert(pkg), &containsKey);
    if (!containsKey) {
        AutoPtr<IProcessStateHolder> holder;
        CProcessStateHolder::New(versionCode, (IProcessStateHolder**)&holder);
        if (mBaseProcessTracker != NULL) {
            Int32 uid;
            mInfo->GetUid(&uid);
            AutoPtr<IProcessState> state = tracker->GetProcessStateLocked(
                    pkg, uid, versionCode, mProcessName);
            holder->SetState(state);
            mPkgList->Put(CoreUtils::Convert(pkg), holder);
            if (state != mBaseProcessTracker) {
                state->MakeActive();
            }
        }
        else {
            mPkgList->Put(CoreUtils::Convert(pkg), holder);
        }
        return TRUE;
    }
    return FALSE;
}

Int32 ProcessRecord::GetSetAdjWithServices()
{
    if (mSetAdj >= ProcessList::CACHED_APP_MIN_ADJ) {
        if (mHasStartedServices) {
            return ProcessList::SERVICE_B_ADJ;
        }
    }
    return mSetAdj;
}

void ProcessRecord::ForceProcessStateUpTo(
    /* [in] */ Int32 newState)
{
    if (mRepProcState > newState) {
        mCurProcState = mRepProcState = newState;
    }
}

void ProcessRecord::ResetPackageList(
    /* [in] */ CProcessStatsService* tracker)
{
    Int32 N;
    mPkgList->GetSize(&N);
    if (mBaseProcessTracker != NULL) {
        Int64 now = SystemClock::GetUptimeMillis();
        mBaseProcessTracker->SetState(IProcessStats::STATE_NOTHING,
                tracker->GetMemFactorLocked(), now, mPkgList);
        if (N != 1) {
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IInterface> value;
                mPkgList->GetValueAt(i, (IInterface**)&value);
                AutoPtr<IProcessStateHolder> holder = IProcessStateHolder::Probe(value);
                AutoPtr<IProcessState> state;
                holder->GetState((IProcessState**)&state);
                if (state != NULL && state != mBaseProcessTracker) {
                    state->MakeInactive();
                }
            }
            mPkgList->Clear();
            Int32 uid;
            mInfo->GetUid(&uid);
            String packageName;
            IPackageItemInfo::Probe(mInfo)->GetPackageName(&packageName);
            Int32 versionCode;
            mInfo->GetVersionCode(&versionCode);
            AutoPtr<IProcessState> ps = tracker->GetProcessStateLocked(
                    packageName, uid, versionCode, mProcessName);
            AutoPtr<IProcessStateHolder> holder;
            CProcessStateHolder::New(versionCode, (IProcessStateHolder**)&holder);
            holder->SetState(ps);
            mPkgList->Put(CoreUtils::Convert(packageName), holder);
            if (ps != mBaseProcessTracker) {
                ps->MakeActive();
            }
        }
    }
    else if (N != 1) {
        mPkgList->Clear();
        String packageName;
        IPackageItemInfo::Probe(mInfo)->GetPackageName(&packageName);
        Int32 versionCode;
        mInfo->GetVersionCode(&versionCode);
        AutoPtr<IProcessStateHolder> holder;
        CProcessStateHolder::New(versionCode, (IProcessStateHolder**)&holder);
        mPkgList->Put(CoreUtils::Convert(packageName), holder);
    }
}

AutoPtr<ArrayOf<String> > ProcessRecord::GetPackageList()
{
    Int32 size;
    mPkgList->GetSize(&size);
    if (size == 0) {
        return NULL;
    }

    AutoPtr< ArrayOf<String> > list = ArrayOf<String>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> key;
        mPkgList->GetKeyAt(i, (IInterface**)&key);
        String strKey;
        ICharSequence::Probe(key)->ToString(&strKey);
        (*list)[i] = strKey;
    }
    return list;
}


} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
