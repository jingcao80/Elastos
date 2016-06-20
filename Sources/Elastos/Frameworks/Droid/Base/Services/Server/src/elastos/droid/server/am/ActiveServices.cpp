
#include "elastos/droid/server/am/ActiveServices.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/server/am/AppBindRecord.h"
#include "elastos/droid/server/am/ConnectionRecord.h"
#include "elastos/droid/server/am/IntentBindRecord.h"
#include <elastos/droid/app/AppGlobals.h>
#include <elastos/droid/os/Build.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/utility/TimeUtils.h>
#include <elastos/droid/Manifest.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/logging/Logger.h>
#include <utils/CallStack.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerRunningServiceInfo;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::IService;
using Elastos::Droid::Content::CIntentFilterComparison;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::CServiceInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Internal::Os::CTransferPipe;
using Elastos::Droid::Internal::Os::ITransferPipe;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::Utility::CParcelableList;
using Elastos::Core::StringUtils;
using Elastos::IO::IFlushable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

const Int32 ActiveServices::ServiceMap::MSG_BG_START_TIMEOUT;

ActiveServices::ServiceMap::ServiceMap(
    /* [in] */ ILooper* looper,
    /* [in] */ Int32 userId,
    /* [in] */ ActiveServices* host)
    : Handler(looper)
    , mUserId(userId)
    , mHost(host)
{
}

ECode ActiveServices::ServiceMap::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_BG_START_TIMEOUT:
        {
            AutoLock lock(mHost->mAm);
            RescheduleDelayedStarts();
        }
        break;
    }
    return NOERROR;
}

void ActiveServices::ServiceMap::EnsureNotStartingBackground(
    /* [in] */ CServiceRecord* r)
{
    List<AutoPtr<CServiceRecord> >::Iterator find = Find(mStartingBackground.Begin(),
        mStartingBackground.End(), AutoPtr<CServiceRecord>(r));
    if (find != mStartingBackground.End()) {
        mStartingBackground.Erase(find);
        if (DEBUG_DELAYED_STARTS)
            Slogger::V(TAG, "No longer background starting: %s", TO_CSTR(r));
        RescheduleDelayedStarts();
    }

    find = Find(mDelayedStartList.Begin(), mDelayedStartList.End(), AutoPtr<CServiceRecord>(r));
    if (find != mDelayedStartList.End()) {
        mDelayedStartList.Erase(find);
        if (DEBUG_DELAYED_STARTS)
            Slogger::V(TAG, "No longer delaying start: %s", TO_CSTR(r));
    }
}

void ActiveServices::ServiceMap::RescheduleDelayedStarts()
{
    RemoveMessages(MSG_BG_START_TIMEOUT);
    Int64 now = SystemClock::GetUptimeMillis();
    List<AutoPtr<CServiceRecord> >::Iterator it = mStartingBackground.Begin();
    while (it != mStartingBackground.End()) {
        AutoPtr<CServiceRecord> r = *it;
        if (r->mStartingBgTimeout <= now) {
            Slogger::I(TAG, "Waited long enough for: %s", TO_CSTR(r));
            it = mStartingBackground.Erase(it);
        }
        else
            ++it;
    }
    while (mDelayedStartList.GetSize() > 0
        && (Int32)mStartingBackground.GetSize() < mHost->mMaxStartingBackground) {
        AutoPtr<CServiceRecord> r = mDelayedStartList.GetFront();
        mDelayedStartList.PopFront();
        if (DEBUG_DELAYED_STARTS)
            Slogger::V(TAG, "REM FR DELAY LIST (exec next): %s", TO_CSTR(r));
        if (r->mPendingStarts.GetSize() <= 0) {
            Slogger::W(TAG, "**** NO PENDING STARTS! %s startReq=%d delayedStop=%d",
                TO_CSTR(r), r->mStartRequested, r->mDelayedStop);
        }
        if (DEBUG_DELAYED_SERVICE) {
            if (mDelayedStartList.GetSize() > 0) {
                Slogger::V(TAG, "Remaining delayed list:");
                it = mDelayedStartList.Begin();
                for (Int32 i = 0; it != mDelayedStartList.End(); ++it, i++) {
                    Slogger::V(TAG, "  #%d: %s", i, TO_CSTR(*it));
                }
            }
        }
        r->mDelayed = FALSE;
        mHost->StartServiceInnerLocked(this, (*r->mPendingStarts.Begin())->mIntent, r, FALSE, TRUE);
    }
    if (mStartingBackground.GetSize() > 0) {
        AutoPtr<CServiceRecord> next = mStartingBackground.GetFront();
        Int64 when = next->mStartingBgTimeout > now ? next->mStartingBgTimeout : now;
        if (DEBUG_DELAYED_SERVICE)
            Slogger::V(TAG, "Top bg start is %s, can delay others up to %lld",
                TO_CSTR(next), when);
        AutoPtr<IMessage> msg;
        ObtainMessage(MSG_BG_START_TIMEOUT, (IMessage**)&msg);
        Boolean res;
        SendMessageAtTime(msg, when, &res);
    }
    if ((Int32)mStartingBackground.GetSize() < mHost->mMaxStartingBackground) {
        mHost->mAm->BackgroundServicesFinishedLocked(mUserId);
    }
}

ActiveServices::ServiceLookupResult::ServiceLookupResult(
    /* [in] */ CServiceRecord* record,
    /* [in] */ const String& permission)
    : mRecord(record)
    , mPermission(permission)
{}

CAR_INTERFACE_IMPL(ActiveServices::ServiceRestarter, Runnable, IServiceRestarter)

ActiveServices::ServiceRestarter::ServiceRestarter(
    /* [in] */ ActiveServices* owner)
    : mService(NULL)
    , mOwner(owner)
{}

void ActiveServices::ServiceRestarter::SetService(
    /* [in] */ CServiceRecord* service)
{
    mService = service;
}

ECode ActiveServices::ServiceRestarter::Run()
{
    AutoLock lock(mOwner->mAm);
    return mOwner->PerformServiceRestartLocked(mService);
}

const String ActiveServices::TAG("ActiveServices");
const String ActiveServices::TAG_MU("ActiveServicesMU");
const Boolean ActiveServices::DEBUG_SERVICE = TRUE; //CActivityManagerService::DEBUG_SERVICE;
const Boolean ActiveServices::DEBUG_SERVICE_EXECUTING = TRUE; //CActivityManagerService::DEBUG_SERVICE_EXECUTING;
const Boolean ActiveServices::DEBUG_DELAYED_SERVICE = TRUE; //CActivityManagerService::DEBUG_SERVICE;
const Boolean ActiveServices::DEBUG_DELAYED_STARTS = DEBUG_DELAYED_SERVICE;
const Boolean ActiveServices::DEBUG_MU = CActivityManagerService::DEBUG_MU;
const Boolean ActiveServices::LOG_SERVICE_START_STOP = FALSE;

const Int32 ActiveServices::SERVICE_TIMEOUT;
const Int32 ActiveServices::SERVICE_BACKGROUND_TIMEOUT;
const Int32 ActiveServices::SERVICE_RESTART_DURATION;
const Int32 ActiveServices::SERVICE_RESET_RUN_DURATION;
const Int32 ActiveServices::SERVICE_RESTART_DURATION_FACTOR;
const Int32 ActiveServices::SERVICE_MIN_RESTART_TIME_BETWEEN;
const Int32 ActiveServices::MAX_SERVICE_INACTIVITY;
const Int32 ActiveServices::BG_START_TIMEOUT;

ActiveServices::ActiveServices(
    /* [in] */ CActivityManagerService* service)
    : mAm(service)
{
    AutoPtr<ISystemProperties> spHelper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&spHelper);
    String value;
    spHelper->Get(String("ro.config.max_starting_bg"), String("0"), &value);
    Int32 maxBg = StringUtils::ParseInt32(value);
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    Boolean result;
    mMaxStartingBackground = maxBg > 0 ? maxBg :
        (amHelper->IsLowRamDeviceStatic(&result), result) ? 1 : 8;
}

ActiveServices::~ActiveServices()
{
}

AutoPtr<CServiceRecord> ActiveServices::GetServiceByName(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 callingUser)
{
    // TODO: Deal with global services
    if (DEBUG_MU) {
        String str;
        Slogger::V(TAG_MU, "getServiceByName(%s), callingUser = %d", TO_CSTR(name), callingUser);
    }
    IComponentNameCServiceRecordHashMap& servicesByName = GetServiceMap(callingUser)->mServicesByName;
    IComponentNameCServiceRecordHashMap::Iterator find =  servicesByName.Find(name);
    return find != servicesByName.End() ? find->mSecond : NULL;
}

Boolean ActiveServices::HasBackgroundServices(
    /* [in] */ Int32 callingUser)
{
    HashMap<Int32, AutoPtr<ServiceMap> >::Iterator find = mServiceMap.Find(callingUser);
    AutoPtr<ServiceMap> smap = find != mServiceMap.End() ? find->mSecond : NULL;
    return smap != NULL ? (Int32)smap->mStartingBackground.GetSize() >= mMaxStartingBackground : FALSE;
}

AutoPtr<ActiveServices::ServiceMap> ActiveServices::GetServiceMap(
    /* [in] */ Int32 callingUser)
{
    AutoPtr<ServiceMap> smap = mServiceMap[callingUser];
    if (smap == NULL) {
        AutoPtr<ILooper> looper;
        mAm->mHandler->GetLooper((ILooper**)&looper);
        smap = new ServiceMap(looper, callingUser, this);
        mServiceMap[callingUser] = smap;
    }

    return smap;
}

IComponentNameCServiceRecordHashMap& ActiveServices::GetServices(
    /* [in] */ Int32 callingUser)
{
    return GetServiceMap(callingUser)->mServicesByName;
}

ECode ActiveServices::StartServiceLocked(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IIntent* service,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId,
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);
    *name = NULL;

    AutoPtr<IBundle> extras;
    service->GetExtras((IBundle**)&extras);
    if (DEBUG_DELAYED_STARTS) {
        Slogger::V(TAG, "startService: %s type=%s args=%s",
            TO_CSTR(service), resolvedType.string(), TO_CSTR(extras));
    }

    Boolean callerFg;
    if (caller != NULL) {
        AutoPtr<ProcessRecord> callerApp = mAm->GetRecordForAppLocked(caller);
        if (callerApp == NULL) {
            Int32 pid = Binder::GetCallingPid();
            Slogger::E(TAG, "Unable to find app for caller %s (pid=%d) when starting service %s",
                TO_CSTR(caller), pid, TO_CSTR(service));
            return E_SECURITY_EXCEPTION;
        }
        callerFg = callerApp->mSetSchedGroup != IProcess::THREAD_GROUP_BG_NONINTERACTIVE;
    }
    else {
        callerFg = TRUE;
    }

    AutoPtr<ServiceLookupResult> res =
        RetrieveServiceLocked(service, resolvedType,
                callingPid, callingUid, userId, TRUE, callerFg);
    if (res == NULL) {
        return NOERROR;
    }
    if (res->mRecord == NULL) {
        return CComponentName::New(String("!"), (!res->mPermission.IsNull())
                ? res->mPermission : String("private to package"), name);
    }

    AutoPtr<CServiceRecord> r = res->mRecord;

    if (!mAm->GetUserManagerLocked()->Exists(r->mUserId)) {
        Slogger::D(TAG, "Trying to start service with non-existent user! %d", r->mUserId);
        return NOERROR;
    }

    Int32 flags;
    service->GetFlags(&flags);
    AutoPtr<NeededUriGrants> neededGrants = mAm->CheckGrantUriPermissionFromIntentLocked(
            callingUid, r->mPackageName, service, flags, NULL, r->mUserId);
    if (UnscheduleServiceRestartLocked(r, callingUid, FALSE)) {
        if (DEBUG_SERVICE)
            Slogger::V(TAG, "START SERVICE WHILE RESTART PENDING: %s", TO_CSTR(r));
    }
    r->mLastActivity = SystemClock::GetUptimeMillis();
    r->mStartRequested = TRUE;
    r->mDelayedStop = FALSE;
    AutoPtr<CServiceRecord::StartItem> item = new CServiceRecord::StartItem(
        r, FALSE, r->MakeNextStartId(), service, neededGrants);
    r->mPendingStarts.PushBack(item);

    AutoPtr<ServiceMap> smap = GetServiceMap(r->mUserId);
    Boolean addToStarting = FALSE;
    if (!callerFg && r->mApp == NULL && mAm->mStartedUsers.Find(r->mUserId) != mAm->mStartedUsers.End()) {
        Int32 uid;
        r->mAppInfo->GetUid(&uid);
        AutoPtr<ProcessRecord> proc = mAm->GetProcessRecordLocked(r->mProcessName, uid, FALSE);
        if (proc == NULL || proc->mCurProcState > IActivityManager::PROCESS_STATE_RECEIVER) {
            // If this is not coming from a foreground caller, then we may want
            // to delay the start if there are already other background services
            // that are starting.  This is to avoid process start spam when lots
            // of applications are all handling things like connectivity broadcasts.
            // We only do this for cached processes, because otherwise an application
            // can have assumptions about calling startService() for a service to run
            // in its own process, and for that process to not be killed before the
            // service is started.  This is especially the case for receivers, which
            // may start a service in onReceive() to do some additional work and have
            // initialized some global state as part of that.
            if (DEBUG_DELAYED_SERVICE) Slogger::V(TAG, "Potential start delay of %s in %s",
                    TO_CSTR(r), TO_CSTR(proc));
            if (r->mDelayed) {
                // This service is already scheduled for a delayed start; just leave
                // it still waiting.
                if (DEBUG_DELAYED_STARTS) Slogger::V(TAG, "Continuing to delay: %s", TO_CSTR(r));
                *name = r->mName;
                REFCOUNT_ADD(*name)
                return NOERROR;
            }
            if ((Int32)smap->mStartingBackground.GetSize() >= mMaxStartingBackground) {
                // Something else is starting, delay!
                Slogger::I(TAG, "Delaying start of: %s", TO_CSTR(r));
                smap->mDelayedStartList.PushBack(r);
                r->mDelayed = TRUE;
                *name = r->mName;
                REFCOUNT_ADD(*name)
                return NOERROR;
            }
            if (DEBUG_DELAYED_STARTS) Slogger::V(TAG, "Not delaying: %s", TO_CSTR(r));
            addToStarting = TRUE;
        }
        else if (proc->mCurProcState >= IActivityManager::PROCESS_STATE_SERVICE) {
            // We slightly loosen when we will enqueue this new service as a background
            // starting service we are waiting for, to also include processes that are
            // currently running other services or receivers.
            addToStarting = TRUE;
            if (DEBUG_DELAYED_STARTS)
                Slogger::V(TAG, "Not delaying, but counting as bg: %s", TO_CSTR(r));
        }
        else if (DEBUG_DELAYED_STARTS) {
            StringBuilder sb(128);
            sb.Append("Not potential delay (state=");
            sb.Append(proc->mCurProcState);
            sb.AppendChar(' ');
            sb.Append(proc->mAdjType);
            String reason = proc->MakeAdjReason();
            if (reason != NULL) {
                sb.AppendChar(' ');
                sb.Append(reason);
            }
            sb.Append("): ");
            sb.Append(r->ToString());
            Slogger::V(TAG, sb.ToString());
        }
    }
    else if (DEBUG_DELAYED_STARTS) {
        if (callerFg) {
            Slogger::V(TAG, "Not potential delay (callerFg=%d uid=%d pid=%d): ",
                callerFg, callingUid, callingPid, TO_CSTR(r));
        }
        else if (r->mApp != NULL) {
            Slogger::V(TAG, "Not potential delay (cur app=%s): %s",
                TO_CSTR(r->mApp), TO_CSTR(r));
        }
        else {
            Slogger::V(TAG, "Not potential delay (user %d not started): %s",
                r->mUserId, TO_CSTR(r));
        }
    }

    AutoPtr<IComponentName> retName = StartServiceInnerLocked(smap, service, r, callerFg, addToStarting);
    *name = retName;
    REFCOUNT_ADD(*name);
    return NOERROR;
}

AutoPtr<IComponentName> ActiveServices::StartServiceInnerLocked(
    /* [in] */ ServiceMap* smap,
    /* [in] */ IIntent* service,
    /* [in] */ CServiceRecord* r,
    /* [in] */ Boolean callerFg,
    /* [in] */ Boolean addToStarting)
{
    AutoPtr<IServiceState> stracker = r->GetTracker();
    if (stracker != NULL) {
        stracker->SetStarted(TRUE, mAm->mProcessStats->GetMemFactorLocked(), r->mLastActivity);
    }
    r->mCallStart = FALSE;
    if (r->mStats != NULL) {
        AutoPtr<IBatteryStatsImpl> stats;
        r->mStats->GetBatteryStats((IBatteryStatsImpl**)&stats);
        {    AutoLock syncLock(stats);
            r->mStats->StartRunningLocked();
        }
    }

    Int32 flags;
    service->GetFlags(&flags);
    String error = BringUpServiceLocked(r, flags, callerFg, FALSE);
    if (error != NULL) {
        AutoPtr<IComponentName> name;
        CComponentName::New(String("!!"), error, (IComponentName**)&name);
        return name;
    }

    if (r->mStartRequested && addToStarting) {
        Boolean first = smap->mStartingBackground.GetSize() == 0;
        smap->mStartingBackground.PushBack(r);
        r->mStartingBgTimeout = SystemClock::GetUptimeMillis() + BG_START_TIMEOUT;
        if (DEBUG_DELAYED_SERVICE) {
            // RuntimeException here = new RuntimeException("here");
            // here.fillInStackTrace();
            Slogger::V(TAG, "Starting background (first=%d): %s", first, TO_CSTR(r));
        }
        else if (DEBUG_DELAYED_STARTS) {
            Slogger::V(TAG, "Starting background (first=%d): %s", first, TO_CSTR(r));
        }
        if (first) {
            smap->RescheduleDelayedStarts();
        }
    }
    else if (callerFg) {
        smap->EnsureNotStartingBackground(r);
    }

    return r->mName;
}

ECode ActiveServices::StopServiceLocked(
    /* [in] */ CServiceRecord* service)
{
    if (service->mDelayed) {
        // If service isn't actually running, but is is being held in the
        // delayed list, then we need to keep it started but note that it
        // should be stopped once no longer delayed.
        if (DEBUG_DELAYED_STARTS)
            Slogger::V(TAG, "Delaying stop of pending: %s", TO_CSTR(service));
        service->mDelayedStop = TRUE;
        return NOERROR;
    }

    if (service->mStats != NULL) {
        AutoPtr<IBatteryStatsImpl> stats;
        service->mStats->GetBatteryStats((IBatteryStatsImpl**)&stats);
        {    AutoLock syncLock(stats);
            service->mStats->StopRunningLocked();
        }
    }

    service->mStartRequested = FALSE;
    if (service->mTracker != NULL) {
        service->mTracker->SetStarted(FALSE, mAm->mProcessStats->GetMemFactorLocked(),
            SystemClock::GetUptimeMillis());
    }
    service->mCallStart = FALSE;
    BringDownServiceIfNeededLocked(service, FALSE, FALSE);
    return NOERROR;
}

ECode ActiveServices::StopServiceLocked(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IIntent* service,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (DEBUG_SERVICE) Slogger::V(TAG, "stopService: %s type=%s",
        TO_CSTR(service), resolvedType.string());

    Int32 pid = Binder::GetCallingPid();
    Int32 uid = Binder::GetCallingUid();
    AutoPtr<ProcessRecord> callerApp = mAm->GetRecordForAppLocked(caller);
    if (caller != NULL && callerApp == NULL) {
        Logger::E(TAG, "Unable to find app for caller %s (pid=%d) when stopping service %s",
            TO_CSTR(caller), pid, TO_CSTR(service));
       return E_SECURITY_EXCEPTION;
    }

    // If this service is active, make sure it is stopped.
    AutoPtr<ServiceLookupResult> r = RetrieveServiceLocked(service, resolvedType,
            pid, uid, userId, FALSE, FALSE);
    if (r != NULL) {
        if (r->mRecord != NULL) {
            Int64 origId = Binder::ClearCallingIdentity();
            ECode ec = StopServiceLocked(r->mRecord);
            Binder::RestoreCallingIdentity(origId);
            FAIL_RETURN(ec)
            *result = 1;
            return NOERROR;
        }
        *result = -1;
        return NOERROR;
    }

    *result = 0;
    return NOERROR;
}

ECode ActiveServices::PeekServiceLocked(
    /* [in] */ IIntent* service,
    /* [in] */ const String& resolvedType,
    /* [out] */ IBinder** token)
{
    VALIDATE_NOT_NULL(token);
    Int32 pid = Binder::GetCallingPid();
    Int32 uid = Binder::GetCallingUid();
    Int32 userId = UserHandle::GetCallingUserId();
    AutoPtr<ServiceLookupResult> r = RetrieveServiceLocked(service, resolvedType,
           pid, uid, userId, FALSE, FALSE);

    AutoPtr<IBinder> ret;
    if (r != NULL) {
        // r.record is NULL if findServiceLocked() failed the caller permission check
        if (r->mRecord == NULL) {
            Logger::D(TAG, "Permission Denial: Accessing service %s from pid=%d, uid=%d requires %s",
                TO_CSTR(r->mRecord->mName), pid, uid, r->mPermission.string());
           return E_SECURITY_EXCEPTION;
       }

      HashMap< AutoPtr<IIntentFilterComparison>, AutoPtr<IntentBindRecord> >::Iterator it =
                r->mRecord->mBindings.Find(r->mRecord->mIntent);
      if (it != r->mRecord->mBindings.End()) {
          AutoPtr<IntentBindRecord> ib = it->mSecond;
          if (ib != NULL) {
              ret = ib->mBinder;
          }
      }
   }

   *token = ret;
   REFCOUNT_ADD(*token);
   return NOERROR;
}

ECode ActiveServices::StopServiceTokenLocked(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* token,
    /* [in] */ Int32 startId,
    /* [out] */ Boolean* succeeded)
{
    if (DEBUG_SERVICE) Slogger::V(TAG, "stopServiceToken: %s %s startId=%d",
            TO_CSTR(className), TO_CSTR(token), startId);

    Int32 userId = UserHandle::GetCallingUserId();
    AutoPtr<CServiceRecord> r = FindServiceLocked(className, token, userId);
    if (r != NULL) {
        if (startId >= 0) {
            // Asked to only stop if done with all work.  Note that
            // to avoid leaks, we will take this as dropping all
            // start items up to and including this one.
            AutoPtr<CServiceRecord::StartItem> si = r->FindDeliveredStart(startId, FALSE);
            if (si != NULL) {
                while (!r->mDeliveredStarts.IsEmpty()) {
                    AutoPtr<CServiceRecord::StartItem> cur = r->mDeliveredStarts.GetFront();
                    r->mDeliveredStarts.PopFront();
                    cur->RemoveUriPermissionsLocked();
                    if (cur == si) {
                        break;
                    }
                }
            }

            if (r->GetLastStartId() != startId) {
                *succeeded = FALSE;
                return NOERROR;
            }

            if (r->mDeliveredStarts.IsEmpty() == FALSE) {
                Slogger::W(TAG, "stopServiceToken startId %d is last, but have %d remaining args", startId, r->mDeliveredStarts.GetSize());
            }
        }

        if (r->mStats != NULL) {
            AutoPtr<IBatteryStatsImpl> stats;
            r->mStats->GetBatteryStats((IBatteryStatsImpl**)&stats);
            {    AutoLock syncLock(stats);
                r->mStats->StopRunningLocked();
            }
        }

        r->mStartRequested = FALSE;
        if (r->mTracker != NULL) {
            r->mTracker->SetStarted(FALSE, mAm->mProcessStats->GetMemFactorLocked(),
                    SystemClock::GetUptimeMillis());
        }
        r->mCallStart = FALSE;

        Int64 origId = Binder::ClearCallingIdentity();
        BringDownServiceIfNeededLocked(r, FALSE, FALSE);
        Binder::RestoreCallingIdentity(origId);
        *succeeded = TRUE;
        return NOERROR;
    }
    *succeeded = FALSE;
    return NOERROR;
}

ECode ActiveServices::SetServiceForegroundLocked(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* token,
    /* [in] */ Int32 id,
    /* [in] */ INotification* notification,
    /* [in] */ Boolean removeNotification)
{
    Int32 userId = UserHandle::GetCallingUserId();
    Int64 origId = Binder::ClearCallingIdentity();
//     try {
    AutoPtr<CServiceRecord> r = FindServiceLocked(className, token, userId);
    if (r != NULL) {
        if (id != 0) {
            if (notification == NULL) {
                Binder::RestoreCallingIdentity(origId);
                Slogger::E(TAG, "NULL notification");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            if (r->mForegroundId != id) {
                r->CancelNotification();
                r->mForegroundId = id;
            }
            Int32 flags;
            notification->GetFlags(&flags);
            notification->SetFlags(flags|INotification::FLAG_FOREGROUND_SERVICE);
            r->mForegroundNoti = notification;
            r->mIsForeground = TRUE;
            r->PostNotification();
            if (r->mApp != NULL) {
                UpdateServiceForegroundLocked(r->mApp, TRUE);
            }
            GetServiceMap(r->mUserId)->EnsureNotStartingBackground(r);
        }
        else {
            if (r->mIsForeground) {
                r->mIsForeground = FALSE;
                if (r->mApp != NULL) {
                    mAm->UpdateLruProcessLocked(r->mApp, FALSE, NULL);
                    UpdateServiceForegroundLocked(r->mApp, TRUE);
                }
            }
            Int32 version;
            if (removeNotification) {
                r->CancelNotification();
                r->mForegroundId = 0;
                r->mForegroundNoti = NULL;
            }
            else if (r->mAppInfo->GetTargetSdkVersion(&version), version >= Build::VERSION_CODES::LOLLIPOP) {
                r->StripForegroundServiceFlagFromNotification();
            }
        }
    }
 //     } finally {
    Binder::RestoreCallingIdentity(origId);
 //     }
    return NOERROR;
}

ECode ActiveServices::UpdateServiceForegroundLocked(
    /* [in] */ ProcessRecord* proc,
    /* [in] */ Boolean oomAdj)
{
   Boolean anyForeground = FALSE;
   HashSet< AutoPtr<CServiceRecord> >::Iterator it;
   for (it = proc->mServices.Begin(); it != proc->mServices.End(); ++it) {
       AutoPtr<CServiceRecord> sr = *it;
       if (sr->mIsForeground) {
           anyForeground = TRUE;
           break;
       }
   }
   mAm->UpdateProcessForegroundLocked(proc, anyForeground, oomAdj);
   return NOERROR;
}

ECode ActiveServices::UpdateServiceConnectionActivitiesLocked(
    /* [in] */ ProcessRecord* clientProc)
{
    AutoPtr<HashSet<AutoPtr<ProcessRecord> > > updatedProcesses;
    HashSet< AutoPtr<ConnectionRecord> >::Iterator it = clientProc->mConnections.Begin();
    for (; it != clientProc->mConnections.End(); ++it) {
        AutoPtr<ConnectionRecord> conn = *it;
        AutoPtr<ProcessRecord> proc = conn->mBinding->mService->mApp;
        if (proc == NULL || proc.Get() == clientProc) {
            continue;
        }
        else if (updatedProcesses == NULL) {
            updatedProcesses = new HashSet<AutoPtr<ProcessRecord> >;
        }
        else if (updatedProcesses->Find(proc) != updatedProcesses->End()) {
            continue;
        }
        updatedProcesses->Insert(proc);
        UpdateServiceClientActivitiesLocked(proc, NULL, FALSE);
    }
    return NOERROR;
}

Boolean ActiveServices::UpdateServiceClientActivitiesLocked(
    /* [in] */ ProcessRecord* proc,
    /* [in] */ ConnectionRecord* modCr,
    /* [in] */ Boolean updateLru)
{
    if (modCr != NULL && modCr->mBinding->mClient != NULL) {
        if (modCr->mBinding->mClient->mActivities.GetSize() <= 0) {
            // This connection is from a client without activities, so adding
            // and removing is not interesting.
            return FALSE;
        }
    }

    Boolean anyClientActivities = FALSE;
    HashSet< AutoPtr<CServiceRecord> >::Iterator it = proc->mServices.Begin();
    for (; it != proc->mServices.End() && !anyClientActivities; ++it) {
        AutoPtr<CServiceRecord> sr = *it;
        CServiceRecord::ConnectionIterator connIt = sr->mConnections.Begin();
        for (; connIt != sr->mConnections.End() && !anyClientActivities; ++connIt) {
            AutoPtr<List<AutoPtr<ConnectionRecord> > > clist = connIt->mSecond;
            List< AutoPtr<ConnectionRecord> >::Iterator crit;
            for (crit = clist->Begin(); crit != clist->End(); ++crit) {
                AutoPtr<ConnectionRecord> cr = *crit;
                if (cr->mBinding->mClient == NULL || cr->mBinding->mClient == proc) {
                    // Binding to ourself is not interesting.
                    continue;
                }
                if (cr->mBinding->mClient->mActivities.GetSize() > 0) {
                    anyClientActivities = TRUE;
                    break;
                }
            }
        }
    }
    if (anyClientActivities != proc->mHasClientActivities) {
        proc->mHasClientActivities = anyClientActivities;
        if (updateLru) {
            mAm->UpdateLruProcessLocked(proc, anyClientActivities, NULL);
        }
        return TRUE;
    }
    return FALSE;
}

ECode ActiveServices::BindServiceLocked(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* inService,
    /* [in] */ const String& resolvedType,
    /* [in] */ IIServiceConnection* connection,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<IBinder> binder = IBinder::Probe(connection);
    if (DEBUG_SERVICE) {
        Slogger::V(TAG, "bindService: %s type=%s conn=%s flags=0x%x",
            TO_CSTR(inService), resolvedType.string(), TO_CSTR(binder), flags);
    }
    AutoPtr<ProcessRecord> callerApp = mAm->GetRecordForAppLocked(caller);
    if (callerApp == NULL) {
        Int32 pid = Binder::GetCallingPid();
        Slogger::E(TAG, "Unable to find app for caller %s (pid=%d) when binding service %s",
            TO_CSTR(caller), pid, TO_CSTR(inService));
        return E_SECURITY_EXCEPTION;
    }

    AutoPtr<ActivityRecord> activity;
    if (token != NULL) {
        activity = ActivityRecord::IsInStackLocked(token);
        if (activity == NULL) {
            Slogger::W(TAG, "Binding with unknown activity: %s", TO_CSTR(token));
            return NOERROR;
        }
    }

    AutoPtr<IIntent> service = inService;
    Int32 clientLabel = 0;
    AutoPtr<IPendingIntent> clientIntent;

    Int32 uid;
    callerApp->mInfo->GetUid(&uid);
    if (uid == IProcess::SYSTEM_UID) {
        // Hacky kind of thing -- allow system stuff to tell us
        // what they are, so we can report this elsewhere for
        // others to know why certain services are running.
//         try {
        AutoPtr<IParcelable> parcel;
        service->GetParcelableExtra(IIntent::EXTRA_CLIENT_INTENT, (IParcelable**)&parcel);
        clientIntent = IPendingIntent::Probe(parcel);
//         } catch (RuntimeException e) {
//         }
        if (clientIntent != NULL) {
            service->GetInt32Extra(IIntent::EXTRA_CLIENT_LABEL, 0, &clientLabel);
            if (clientLabel != 0) {
                // There are no useful extras in the intent, trash them.
                // System code calling with this stuff just needs to know
                // this will happen.
                service = NULL;
                inService->CloneFilter((IIntent**)&service);
            }
        }
    }

    if ((flags & IContext::BIND_TREAT_LIKE_ACTIVITY) != 0) {
        FAIL_RETURN(mAm->EnforceCallingPermission(Manifest::permission::MANAGE_ACTIVITY_STACKS,
                String("BIND_TREAT_LIKE_ACTIVITY")));
    }

    Boolean callerFg = callerApp->mSetSchedGroup != IProcess::THREAD_GROUP_BG_NONINTERACTIVE;

    Int32 pid = Binder::GetCallingPid();
    Int32 cuid = Binder::GetCallingPid();
    AutoPtr<ServiceLookupResult> res = RetrieveServiceLocked(
        service, resolvedType, pid, cuid, userId, TRUE, callerFg);
    if (res == NULL) {
        *result = 0;
        return NOERROR;
    }
    if (res->mRecord == NULL) {
        *result = -1;
        return NOERROR;
    }
    AutoPtr<CServiceRecord> s = res->mRecord;

    Int64 origId = Binder::ClearCallingIdentity();

//     try {
    if (UnscheduleServiceRestartLocked(s, uid, FALSE)) {
        if (DEBUG_SERVICE) Slogger::V(TAG, "BIND SERVICE WHILE RESTART PENDING: %s", TO_CSTR(s));
    }

    if ((flags & IContext::BIND_AUTO_CREATE) != 0) {
        s->mLastActivity = SystemClock::GetUptimeMillis();
        if (!s->HasAutoCreateConnections()) {
            // This is the first binding, let the tracker know.
            AutoPtr<IServiceState> stracker = s->GetTracker();
            if (stracker != NULL) {
                stracker->SetBound(TRUE, mAm->mProcessStats->GetMemFactorLocked(),
                    s->mLastActivity);
            }
        }
    }

    AutoPtr<AppBindRecord> b = s->RetrieveAppBindingLocked(service, callerApp);
    AutoPtr<ConnectionRecord> c = new ConnectionRecord(b, activity,
            connection, flags, clientLabel, clientIntent);

    AutoPtr<ConnectionRecordList> clist = s->mConnections[binder];
    if (clist == NULL) {
        clist = new ConnectionRecordList();
        s->mConnections[binder] = clist;
    }
    clist->PushBack(c);
    b->mConnections.Insert(c);
    if (activity != NULL) {
        if (activity->mConnections == NULL) {
            activity->mConnections = new HashSet<AutoPtr<ConnectionRecord> >();
        }
        activity->mConnections->Insert(c);
    }
    b->mClient->mConnections.Insert(c);
    if ((c->mFlags & IContext::BIND_ABOVE_CLIENT) != 0) {
        b->mClient->mHasAboveClient = TRUE;
    }
    if (s->mApp != NULL) {
        UpdateServiceClientActivitiesLocked(s->mApp, c, TRUE);
    }
    clist = mServiceConnections[binder];
    if (clist == NULL) {
        clist = new ConnectionRecordList();
        mServiceConnections[binder] = clist;
    }
    clist->PushBack(c);

    if ((flags & IContext::BIND_AUTO_CREATE) != 0) {
        s->mLastActivity = SystemClock::GetUptimeMillis();
        Int32 flags;
        service->GetFlags(&flags);
        if (!BringUpServiceLocked(s, flags, callerFg, FALSE).IsNull()) {
            *result = 0;
            return NOERROR;
        }
    }

    if (s->mApp != NULL) {
        if ((flags & IContext::BIND_TREAT_LIKE_ACTIVITY) != 0) {
            s->mApp->mTreatLikeActivity = true;
        }
        // This could have made the service more important.
        mAm->UpdateLruProcessLocked(s->mApp, s->mApp->mHasClientActivities
                || s->mApp->mTreatLikeActivity, b->mClient);
        mAm->UpdateOomAdjLocked(s->mApp);
    }

    if (DEBUG_SERVICE) {
        Slogger::V(TAG, "Bind %s with %s: received=%d apps=%d doRebind=%d",
            TO_CSTR(s), TO_CSTR(b), b->mIntent->mReceived,
            b->mIntent->mApps.GetSize(), b->mIntent->mDoRebind);
    }

    if (s->mApp != NULL && b->mIntent->mReceived) {
        // Service is already running, so we can immediately
        // publish the connection.
//        try {
        c->mConn->Connected(s->mName, b->mIntent->mBinder);
//        } catch (Exception e) {
//            Slogger::W(TAG, "Failure sending service " + s.shortName
//                    + " to connection " + c.conn.asBinder()
//                    + " (in " + c.binding.client.processName + ")", e);
//        }

        // If this is the first app connected back to this binding,
        // and the service had previously asked to be told when
        // rebound, then do so.
        if (b->mIntent->mApps.GetSize() == 1 && b->mIntent->mDoRebind) {
            RequestServiceBindingLocked(s, b->mIntent, callerFg, TRUE);
        }
    }
    else if (!b->mIntent->mRequested) {
        RequestServiceBindingLocked(s, b->mIntent, callerFg, FALSE);
    }

    GetServiceMap(s->mUserId)->EnsureNotStartingBackground(s);
//     } finally {
    Binder::RestoreCallingIdentity(origId);
//     }

    *result = 1;
    return NOERROR;
}

ECode ActiveServices::PublishServiceLocked(
    /* [in] */ CServiceRecord* r,
    /* [in] */ IIntent* intent,
    /* [in] */ IBinder* service)
{
    Int64 origId = Binder::ClearCallingIdentity();
    // try {
    if (DEBUG_SERVICE) {
        Slogger::V(TAG, "PUBLISHING %s %s: %s",
            TO_CSTR(r), TO_CSTR(intent), TO_CSTR(service));
    }
    if (r != NULL) {
        AutoPtr<IIntentFilterComparison> filter;
        CIntentFilterComparison::New(intent, (IIntentFilterComparison**)&filter);
        AutoPtr<IntentBindRecord> b;
        HashMap< AutoPtr<IIntentFilterComparison>, AutoPtr<IntentBindRecord> >::Iterator it =
                r->mBindings.Find(filter);
        if (it != r->mBindings.End()) {
            b = it->mSecond;
        }
        if (b != NULL && !b->mReceived) {
            b->mBinder = service;
            b->mRequested = TRUE;
            b->mReceived = TRUE;
            HashMap< AutoPtr<IBinder>, AutoPtr<ConnectionRecordList> >::Iterator cit;
            for (cit = r->mConnections.Begin(); cit != r->mConnections.End(); ++cit) {
                ConnectionRecordListIterator crit;
                for (crit = cit->mSecond->Begin(); crit != cit->mSecond->End(); ++crit) {
                    AutoPtr<ConnectionRecord> c = *crit;
                    Boolean isEqual;
                    filter->Equals(c->mBinding->mIntent->mIntent, &isEqual);
                    if (!isEqual) {
                        if (DEBUG_SERVICE) Slogger::V(
                                TAG, "Not publishing to: %s", TO_CSTR(c));
                        if (DEBUG_SERVICE) Slogger::V(
                                TAG, "Bound intent: %s", TO_CSTR(c->mBinding->mIntent->mIntent));
                        if (DEBUG_SERVICE) Slogger::V(
                                TAG, "Published intent: %s", TO_CSTR(intent));
                        continue;
                    }
                    if (DEBUG_SERVICE) Slogger::V(TAG, "Publishing to: %s", TO_CSTR(c));
                    if (FAILED(c->mConn->Connected(r->mName, service))) {
                        Slogger::W(TAG, "Failure sending service %s to connection %s (in %s)",
                            TO_CSTR(r->mName), TO_CSTR(c->mConn), c->mBinding->mClient->mProcessName.string());
                    }
                }
            }
        }

        List< AutoPtr<CServiceRecord> >::Iterator sit =
                Find(mDestroyingServices.Begin(), mDestroyingServices.End(), AutoPtr<CServiceRecord>(r));
        ServiceDoneExecutingLocked(r, sit != mDestroyingServices.End(), FALSE);
    }
    // } finally {
    Binder::RestoreCallingIdentity(origId);
    // }
    return NOERROR;
}

ECode ActiveServices::UnbindServiceLocked(
    /* [in] */ IIServiceConnection* connection,
    /* [out] */ Boolean* result)
{
    AutoPtr<IBinder> binder = IBinder::Probe(connection);
    if (DEBUG_SERVICE) Slogger::V(TAG, "unbindService: conn=%s", TO_CSTR(binder));
    AutoPtr<ConnectionRecordList> clist;
    HashMap< AutoPtr<IBinder>, AutoPtr<ConnectionRecordList> >::Iterator it =
            mServiceConnections.Find(binder);
    if (it != mServiceConnections.End()) {
        clist = it->mSecond;
    }
    if (clist == NULL) {
        Slogger::W(TAG, "Unbind failed: could not find connection for %s", TO_CSTR(binder));
        *result = FALSE;
        return NOERROR;
    }

    Int64 origId = Binder::ClearCallingIdentity();
//     try {
    while (clist->IsEmpty() == FALSE) {
        AutoPtr<ConnectionRecord> r = clist->GetFront();
        RemoveConnectionLocked(r, NULL, NULL);
        if (clist->GetSize() > 0 && clist->GetFront() == r) {
            // In case it didn't get removed above, do it now.
            Slogger::W/*wtf*/(TAG, "Connection %s not removed for binder %s", TO_CSTR(r), TO_CSTR(binder));
            clist->PopFront();
        }

        if (r->mBinding->mService->mApp != NULL) {
            // This could have made the service less important.
            if ((r->mFlags & IContext::BIND_TREAT_LIKE_ACTIVITY) != 0) {
                r->mBinding->mService->mApp->mTreatLikeActivity = TRUE;
                mAm->UpdateLruProcessLocked(r->mBinding->mService->mApp,
                    r->mBinding->mService->mApp->mHasClientActivities
                    || r->mBinding->mService->mApp->mTreatLikeActivity, NULL);
            }
            mAm->UpdateOomAdjLocked(r->mBinding->mService->mApp);
        }
    }
//     } finally {
    Binder::RestoreCallingIdentity(origId);
//     }

    *result = TRUE;
    return NOERROR;
}

ECode ActiveServices::UnbindFinishedLocked(
    /* [in] */ CServiceRecord* r,
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean doRebind)
{
    Int64 origId = Binder::ClearCallingIdentity();
//     try {
    if (r != NULL) {
        AutoPtr<IIntentFilterComparison> filter;
        CIntentFilterComparison::New(intent, (IIntentFilterComparison**)&filter);
        AutoPtr<IntentBindRecord> b = r->mBindings[filter];
        if (DEBUG_SERVICE) Slogger::V(TAG, "unbindFinished in %s at %s: apps=%d", TO_CSTR(r)
                , TO_CSTR(b), (b != NULL ? b->mApps.GetSize() : 0));

        Boolean inDestroying = FALSE;
        List< AutoPtr<CServiceRecord> >::Iterator it =
            Find(mDestroyingServices.Begin(), mDestroyingServices.End(), AutoPtr<CServiceRecord>(r));
        if (it != mDestroyingServices.End())
            inDestroying = TRUE;
        if (b != NULL) {
            if (b->mApps.IsEmpty() == FALSE && !inDestroying) {
               // Applications have already bound since the last
                // unbind, so just rebind right here.
                Boolean inFg = FALSE;
                HashMap< AutoPtr<ProcessRecord>, AutoPtr<AppBindRecord> >::Iterator it;
                for (it = b->mApps.Begin(); it != b->mApps.End(); ++it) {
                    ProcessRecord* client = it->mSecond->mClient;
                    if (client != NULL && client->mSetSchedGroup
                        != IProcess::THREAD_GROUP_BG_NONINTERACTIVE) {
                        inFg = TRUE;
                        break;
                    }
                }
                RequestServiceBindingLocked(r, b, inFg, TRUE);
            }
            else {
                // Note to tell the service the next time there is
                // a new client.
                b->mDoRebind = TRUE;
            }
        }

        ServiceDoneExecutingLocked(r, inDestroying, FALSE);
    }
//     } finally {
    Binder::RestoreCallingIdentity(origId);
//     }
    return NOERROR;
}

AutoPtr<CServiceRecord> ActiveServices::FindServiceLocked(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* token,
    /* [in] */ Int32 userId)
{
    AutoPtr<CServiceRecord> r = GetServiceByName(name, userId);
    return IBinder::Probe(r) == token ? r : NULL;
}

AutoPtr<ActiveServices::ServiceLookupResult> ActiveServices::RetrieveServiceLocked(
    /* [in] */ IIntent* service,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean createIfNeeded,
    /* [in] */ Boolean callingFromFg)
{
    AutoPtr<CServiceRecord> r;
    if (DEBUG_SERVICE) Slogger::V(TAG, "retrieveServiceLocked: %s type=%s callingUid=%d",
        TO_CSTR(service), resolvedType.string(), callingUid);

    mAm->HandleIncomingUser(callingPid, callingUid, userId,
        FALSE, CActivityManagerService::ALLOW_NON_FULL_IN_PROFILE,
        String("service"), String(NULL), &userId);

    AutoPtr<ServiceMap> smap = GetServiceMap(userId);
    AutoPtr<IComponentName> comp;
    service->GetComponent((IComponentName**)&comp);
    String serviceName;
    if (comp != NULL) {
        comp->FlattenToString(&serviceName);
        IComponentNameCServiceRecordHashMap::Iterator find = smap->mServicesByName.Find(comp);
        r = find != smap->mServicesByName.End() ? find->mSecond : NULL;
    }
    if (r == NULL) {
        AutoPtr<IIntentFilterComparison> filter;
        CIntentFilterComparison::New(service, (IIntentFilterComparison**)&filter);
        IIntentFilterComparisonCServiceRecordHashMap::Iterator find = smap->mServicesByIntent.Find(filter);
        r = find != smap->mServicesByIntent.End() ? find->mSecond : NULL;
    }
    if (r == NULL) {
//         try {
        AutoPtr<IResolveInfo> rInfo;
        AppGlobals::GetPackageManager()->ResolveService(
            service, resolvedType, CActivityManagerService::STOCK_PM_FLAGS,
            userId, (IResolveInfo**)&rInfo);
        AutoPtr<IServiceInfo> sInfo;
        if (rInfo != NULL) {
            rInfo->GetServiceInfo((IServiceInfo**)&sInfo);
        }
        if (sInfo == NULL) {
            Slogger::W(TAG, "Unable to start service %s[%s] U=%d: not found",
                serviceName.string(), TO_CSTR(service), userId);
            return NULL;
        }
        AutoPtr<IComponentInfo> comInfo = IComponentInfo::Probe(sInfo);
        AutoPtr<IPackageItemInfo> pkgInfo = IPackageItemInfo::Probe(sInfo);
        AutoPtr<IApplicationInfo> appInfo;
        comInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
        String spName, sname;
        IPackageItemInfo::Probe(appInfo)->GetPackageName(&spName);
        pkgInfo->GetName(&sname);
        AutoPtr<IComponentName> name;
        CComponentName::New(spName, sname, (IComponentName**)&name);
        if (userId > 0) {
            String pName;
            comInfo->GetProcessName(&pName);
            Int32 flags, uid;
            sInfo->GetFlags(&flags);
            appInfo->GetUid(&uid);
            Boolean isSingleton;
            mAm->IsSingleton(pName, appInfo, sname, flags, &isSingleton);
            if (isSingleton && mAm->IsValidSingletonCall(callingUid, uid)) {
                userId = 0;
                smap = GetServiceMap(0);
            }
            AutoPtr<IServiceInfo> temp;
            CServiceInfo::New(sInfo, (IServiceInfo**)&temp);
            sInfo = temp;
            comInfo = IComponentInfo::Probe(sInfo);
            pkgInfo = IPackageItemInfo::Probe(sInfo);
            comInfo->SetApplicationInfo(mAm->GetAppInfoForUser(appInfo, userId));
        }
        IComponentNameCServiceRecordHashMap::Iterator find = smap->mServicesByName.Find(name);
        r = find != smap->mServicesByName.End() ? find->mSecond : NULL;
        if (r == NULL && createIfNeeded) {
            AutoPtr<IIntent> ci;
            service->CloneFilter((IIntent**)&ci);
            AutoPtr<IIntentFilterComparison> filter;
            CIntentFilterComparison::New(ci, (IIntentFilterComparison**)&filter);
            AutoPtr<ServiceRestarter> res = new ServiceRestarter(this);
            AutoPtr<IBatteryStatsImplUidPkgServ> ss;
            AutoPtr<IBatteryStatsImpl> stats = mAm->mBatteryStatsService->GetActiveStatistics();
            {
                AutoLock syncLock(stats);
                AutoPtr<IApplicationInfo> appInfo;
                comInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
                Int32 sInfoUid;
                appInfo->GetUid(&sInfoUid);
                String sInfoPkgName, sInfoName;
                pkgInfo->GetPackageName(&sInfoPkgName);
                pkgInfo->GetName(&sInfoName);
                stats->GetServiceStatsLocked(sInfoUid, sInfoPkgName, sInfoName,
                    (IBatteryStatsImplUidPkgServ**)&ss);
            }
            CServiceRecord::NewByFriend((CServiceRecord**)&r);
            r->constructor(mAm, ss, name, filter, sInfo, callingFromFg, res);
            res->SetService(r);
            smap->mServicesByName[name] = r;
            smap->mServicesByIntent[filter] = r;

            // Make sure this component isn't in the pending list.
            List< AutoPtr<CServiceRecord> >::Iterator it = mPendingServices.Begin();
            while (it != mPendingServices.End()) {
                AutoPtr<CServiceRecord> pr = *it;
                AutoPtr<IApplicationInfo> appInfo;
                IComponentInfo::Probe(pr->mServiceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
                Int32 aUid, sUid;
                appInfo->GetUid(&aUid);
                AutoPtr<IApplicationInfo> sappInfo;
                comInfo->GetApplicationInfo((IApplicationInfo**)&sappInfo);
                sappInfo->GetUid(&sUid);
                if (aUid == sUid && Object::Equals(pr->mName, name)) {
                    it = mPendingServices.Erase(it);
                }
                else {
                    ++it;
                }
            }
        }
//         } catch (RemoteException ex) {
//             // pm is in same process, this will never happen.
//         }
    }
    if (r != NULL) {
        Int32 uid = 0;
        r->mAppInfo->GetUid(&uid);
        AutoPtr<ServiceLookupResult> result;
        if (mAm->CheckComponentPermission(r->mPermission,
            callingPid, callingUid, uid, r->mExported) != IPackageManager::PERMISSION_GRANTED) {
            if (!r->mExported) {
                Slogger::W(TAG, "Permission [%s] Denial: Accessing service %s from pid=%d, uid=%d that is not exported from uid %d",
                    r->mPermission.string(), TO_CSTR(r->mName), callingPid, callingUid, uid);
                StringBuilder sb("not exported from uid ");
                sb += uid;
                result = new ServiceLookupResult(NULL, sb.ToString());
                return result;
            }
            Slogger::W(TAG, "Permission Denial: Accessing service %s from pid=%d, uid=%d requires %s",
                TO_CSTR(r->mName), callingPid, callingUid, r->mPermission.string());
            result = new ServiceLookupResult(NULL, r->mPermission);
            return result;
        }
        // assert(0 && "TODO");
        Logger::I(TAG, "TODO: mIntentFirewall not ready.");
        // if (!mAm->mIntentFirewall->CheckService(r->mName, service, callingUid, callingPid,
        //         resolvedType, r->mAppInfo)) {
        //     return NULL;
        // }
        result = new ServiceLookupResult(r, String(NULL));
        return result;
    }
    return NULL;
}

void ActiveServices::BumpServiceExecutingLocked(
    /* [in] */ CServiceRecord* r,
    /* [in] */ Boolean fg,
    /* [in] */ const String& why)
{
    if (DEBUG_SERVICE)
        Logger::V(TAG, ">>> EXECUTING %s of %s in app %s", why.string(), TO_CSTR(r), TO_CSTR(r->mApp));
    else if (DEBUG_SERVICE_EXECUTING)
        Logger::V(TAG, ">>> EXECUTING %s of %s", why.string(), r->mShortName.string());
    Int64 now = SystemClock::GetUptimeMillis();
    if (r->mExecuteNesting == 0) {
        r->mExecuteFg = fg;
        AutoPtr<IServiceState> stracker = r->GetTracker();
        if (stracker != NULL) {
            stracker->SetExecuting(TRUE, mAm->mProcessStats->GetMemFactorLocked(), now);
        }
        if (r->mApp != NULL) {
            r->mApp->mExecutingServices.Insert(r);
            r->mApp->mExecServicesFg |= fg;
            if (r->mApp->mExecutingServices.GetSize() == 1) {
                ScheduleServiceTimeoutLocked(r->mApp);
            }
        }
    }
    else if (r->mApp != NULL && fg && !r->mApp->mExecServicesFg) {
        r->mApp->mExecServicesFg = TRUE;
        ScheduleServiceTimeoutLocked(r->mApp);
    }
    r->mExecuteFg |= fg;
    r->mExecuteNesting++;
    r->mExecutingStart = now;
}

Boolean ActiveServices::RequestServiceBindingLocked(
    /* [in] */ CServiceRecord* r,
    /* [in] */ IntentBindRecord* i,
    /* [in] */ Boolean execInFg,
    /* [in] */ Boolean rebind)
{
    if (r->mApp == NULL || r->mApp->mThread == NULL) {
        // If service is not currently running, can't yet bind.
        return FALSE;
    }
    if ((!i->mRequested || rebind) && i->mApps.IsEmpty() == FALSE) {
        BumpServiceExecutingLocked(r, execInFg, String("bind"));
        r->mApp->ForceProcessStateUpTo(IActivityManager::PROCESS_STATE_SERVICE);
        AutoPtr<IIntent> intent;
        i->mIntent->GetIntent((IIntent**)&intent);
        ECode ec = r->mApp->mThread->ScheduleBindService(r, intent, rebind, r->mApp->mRepProcState);
        if (SUCCEEDED(ec)) {
            if (!rebind) {
                i->mRequested = TRUE;
            }
            i->mHasBound = TRUE;
            i->mDoRebind = FALSE;
        }
        else {
            if (DEBUG_SERVICE) {
                Slogger::V(TAG, "Crashed while binding %s", TO_CSTR(r));
            }
            return FALSE;
        }
    }
    return TRUE;
}

Boolean ActiveServices::ScheduleServiceRestartLocked(
    /* [in] */ CServiceRecord* r,
    /* [in] */ Boolean allowCancel)
{
    Boolean canceled = FALSE;

    AutoPtr<ServiceMap> smap = GetServiceMap(r->mUserId);
    IComponentNameCServiceRecordHashMap::Iterator find = smap->mServicesByName.Find(r->mName);
    AutoPtr<CServiceRecord> cur = find != smap->mServicesByName.End() ? find->mSecond : NULL;
    if (cur.Get() != r) {
        Slogger::W/*wtf*/(TAG, "Attempting to schedule restart of %s when found in map: %s",
            TO_CSTR(r), TO_CSTR(cur));
        return FALSE;
    }

    const Int64 now = SystemClock::GetUptimeMillis();

    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(r->mServiceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 flags;
    appInfo->GetFlags(&flags);
    if ((flags &IApplicationInfo::FLAG_PERSISTENT) == 0) {
        Int64 minDuration = SERVICE_RESTART_DURATION;
        Int64 resetTime = SERVICE_RESET_RUN_DURATION;

        // Any delivered but not yet finished starts should be put back
        // on the pending list.
        if (!r->mDeliveredStarts.IsEmpty()) {
            List< AutoPtr<CServiceRecord::StartItem> >::ReverseIterator rit;
            for (rit = r->mDeliveredStarts.RBegin(); rit != r->mDeliveredStarts.REnd(); ++rit) {
                AutoPtr<CServiceRecord::StartItem> si = *rit;
                si->RemoveUriPermissionsLocked();
                if (si->mIntent == NULL) {
                    // We'll generate this again if needed.
                }
                else if (!allowCancel || (si->mDeliveryCount < CServiceRecord::MAX_DELIVERY_COUNT
                        && si->mDoneExecutingCount < CServiceRecord::MAX_DONE_EXECUTING_COUNT)) {
                    r->mPendingStarts.Insert(0, si);
                    Int64 dur = SystemClock::GetUptimeMillis() - si->mDeliveredTime;
                    dur *= 2;
                    if (minDuration < dur) minDuration = dur;
                    if (resetTime < dur) resetTime = dur;
                }
                else {
                    Slogger::W(TAG, "Canceling start item %s in service %s", TO_CSTR(si->mIntent), TO_CSTR(r->mName));
                    canceled = TRUE;
                }
            }
            r->mDeliveredStarts.Clear();
        }

        r->mTotalRestartCount++;
        if (r->mRestartDelay == 0) {
            r->mRestartCount++;
            r->mRestartDelay = minDuration;
        }
        else {
            // If it has been a "reasonably long time" since the service
            // was started, then reset our restart duration back to
            // the beginning, so we don't infinitely increase the duration
            // on a service that just occasionally gets killed (which is
            // a normal case, due to process being killed to reclaim memory).
            if (now > (r->mRestartTime + resetTime)) {
                r->mRestartCount = 1;
                r->mRestartDelay = minDuration;
            }
            else {
                r->mRestartDelay *= SERVICE_RESTART_DURATION_FACTOR;
                if (r->mRestartDelay < minDuration) {
                    r->mRestartDelay = minDuration;
                }
            }
        }

        r->mNextRestartTime = now + r->mRestartDelay;

        // Make sure that we don't end up restarting a bunch of services
        // all at the same time.
        Boolean repeat;
        do {
            repeat = FALSE;
            List< AutoPtr<CServiceRecord> >::ReverseIterator rit;
            for (rit = mRestartingServices.RBegin(); rit != mRestartingServices.REnd(); ++rit) {
                AutoPtr<CServiceRecord> r2 = *rit;
                if (r2.Get() != r && r->mNextRestartTime
                        >= (r2->mNextRestartTime - SERVICE_MIN_RESTART_TIME_BETWEEN)
                        && r->mNextRestartTime
                        < (r2->mNextRestartTime + SERVICE_MIN_RESTART_TIME_BETWEEN)) {
                    r->mNextRestartTime = r2->mNextRestartTime + SERVICE_MIN_RESTART_TIME_BETWEEN;
                    r->mRestartDelay = r->mNextRestartTime - now;
                    repeat = TRUE;
                    break;
                }
            }
        } while (repeat);

    }
    else {
        // Persistent processes are immediately restarted, so there is no
        // reason to hold of on restarting their services.
        r->mTotalRestartCount++;
        r->mRestartCount = 0;
        r->mRestartDelay = 0;
        r->mNextRestartTime = now;
    }

    AutoPtr<CServiceRecord> sr = r;
    if (Find(mRestartingServices.Begin(), mRestartingServices.End(), sr)
            == mRestartingServices.End()) {
        r->mCreatedFromFg = FALSE;
        mRestartingServices.PushBack(r);
        r->MakeRestarting(mAm->mProcessStats->GetMemFactorLocked(), now);
    }

    r->CancelNotification();

    mAm->mHandler->RemoveCallbacks(r->mRestarter);
    Boolean result;
    mAm->mHandler->PostAtTime(r->mRestarter, r->mNextRestartTime, &result);
    r->mNextRestartTime = SystemClock::GetUptimeMillis() + r->mRestartDelay;
    Slogger::W(TAG, "Scheduling restart of crashed service %s in %lldms"
            , r->mShortName.string(), r->mRestartDelay);
//     EventLog.writeEvent(EventLogTags.AM_SCHEDULE_SERVICE_RESTART,
//             r.userId, r.shortName, r.restartDelay);

    return canceled;
}

ECode ActiveServices::PerformServiceRestartLocked(
    /* [in] */ CServiceRecord* r)
{
    if (Find(mRestartingServices.Begin(), mRestartingServices.End(), AutoPtr<CServiceRecord>(r))
            == mRestartingServices.End()) {
        return NOERROR;
    }
    AutoPtr<IIntent> intent;
    r->mIntent->GetIntent((IIntent**)&intent);
    Int32 flags;
    intent->GetFlags(&flags);
    BringUpServiceLocked(r, flags, r->mCreatedFromFg, TRUE);
    return NOERROR;
}

Boolean ActiveServices::UnscheduleServiceRestartLocked(
    /* [in] */ CServiceRecord* r,
    /* [in] */ Int32 callingUid,
    /* [in] */ Boolean force)
{
    if (!force && r->mRestartDelay == 0) {
        return FALSE;
    }
    // Remove from the restarting list; if the service is currently on the
    // restarting list, or the call is coming from another app, then this
    // service has become of much more interest so we reset the restart interval.
    List< AutoPtr<CServiceRecord> >::Iterator find = Find(
        mRestartingServices.Begin(), mRestartingServices.End(), AutoPtr<CServiceRecord>(r));
    Boolean removed = find != mRestartingServices.End();
    if (removed)
        mRestartingServices.Erase(find);
    Int32 uid;
    if (removed || callingUid != (r->mAppInfo->GetUid(&uid), uid)) {
        r->ResetRestartCounter();
    }
    if (removed) {
        ClearRestartingIfNeededLocked(r);
    }
    mAm->mHandler->RemoveCallbacks(r->mRestarter);
    return TRUE;
}

void ActiveServices::ClearRestartingIfNeededLocked(
    /* [in] */ CServiceRecord* r)
{
    if (r->mRestartTracker != NULL) {
        // If this is the last restarting record with this tracker, then clear
        // the tracker's restarting state.
        Boolean stillTracking = FALSE;
        List< AutoPtr<CServiceRecord> >::Iterator it;
        for (it = mRestartingServices.Begin(); it != mRestartingServices.End(); ++it) {
            if ((*it)->mRestartTracker == r->mRestartTracker) {
                stillTracking = TRUE;
                break;
            }
        }
        if (!stillTracking) {
            r->mRestartTracker->SetRestarting(FALSE, mAm->mProcessStats->GetMemFactorLocked(),
                SystemClock::GetUptimeMillis());
            r->mRestartTracker = NULL;
        }
    }
}

String ActiveServices::BringUpServiceLocked(
    /* [in] */ CServiceRecord* r,
    /* [in] */ Int32 intentFlags,
    /* [in] */ Boolean execInFg,
    /* [in] */ Boolean whileRestarting)
{
    //Slog.i(TAG, "Bring up service:");
    //r.dump("  ");

    if (r->mApp != NULL && r->mApp->mThread != NULL) {
        SendServiceArgsLocked(r, execInFg, FALSE);
        return String(NULL);
    }

    if (!whileRestarting && r->mRestartDelay > 0) {
        // If waiting for a restart, then do nothing.
        return String(NULL);
    }

    if (DEBUG_SERVICE) Slogger::V(TAG, "Bringing up %s %s", TO_CSTR(r), TO_CSTR(r->mIntent));

    // We are now bringing the service up, so no longer in the
    // restarting state.
    List< AutoPtr<CServiceRecord> >::Iterator find = Find(
        mRestartingServices.Begin(), mRestartingServices.End(), AutoPtr<CServiceRecord>(r));
    if (find != mRestartingServices.End()) {
        mRestartingServices.Erase(find);
        ClearRestartingIfNeededLocked(r);
    }

    // Make sure this service is no longer considered delayed, we are starting it now.
    if (r->mDelayed) {
        if (DEBUG_DELAYED_STARTS) Slogger::V(TAG, "REM FR DELAY LIST (bring up): ", TO_CSTR(r));
        GetServiceMap(r->mUserId)->mDelayedStartList.Remove(r);
        r->mDelayed = FALSE;
    }

    // Make sure that the user who owns this service is started.  If not,
    // we don't want to allow it to run.
    if (mAm->mStartedUsers.Find(r->mUserId) == mAm->mStartedUsers.End()) {
        StringBuilder msg("Unable to launch app ");
        String pName;
        IPackageItemInfo::Probe(r->mAppInfo)->GetPackageName(&pName);
        msg += pName;
        msg += "/";
        Int32 uid;
        r->mAppInfo->GetUid(&uid);
        msg += uid;
        msg += " for service ";
        AutoPtr<IIntent> intent;
        r->mIntent->GetIntent((IIntent**)&intent);
        msg += intent;
        msg += ": user ";
        msg += r->mUserId;
        msg += " is stopped";
        Slogger::W(TAG, msg.ToString().string());
        BringDownServiceLocked(r);
        return msg.ToString();
    }

    // Service is now being launched, its package can't be stopped.
//     try {
    ECode ec = AppGlobals::GetPackageManager()->SetPackageStoppedState(
        r->mPackageName, FALSE, r->mUserId);
    if (FAILED(ec))
        Slogger::W(TAG, "Failed trying to unstop package %s: %d", r->mPackageName.string(), ec);
//     } catch (RemoteException e) {
//     } catch (IllegalArgumentException e) {
//         Slog.w(TAG, "Failed trying to unstop package "
//                 + r.packageName + ": " + e);
//     }

    Int32 siFlags;
    r->mServiceInfo->GetFlags(&siFlags);
    const Boolean isolated = (siFlags & IServiceInfo::FLAG_ISOLATED_PROCESS) != 0;
    const String procName = r->mProcessName;
    AutoPtr<ProcessRecord> app;

    if (!isolated) {
        Int32 rUid;
        r->mAppInfo->GetUid(&rUid);
        app = mAm->GetProcessRecordLocked(procName, rUid, FALSE);
        if (DEBUG_MU) {
            Slogger::V(TAG_MU, "bringUpServiceLocked: appInfo.uid=%d app=%s", rUid, TO_CSTR(app));
        }
        if (app != NULL && app->mThread != NULL) {
//             try {
            String packageName;
            IPackageItemInfo::Probe(r->mAppInfo)->GetPackageName(&packageName);
            Int32 versionCode;
            r->mAppInfo->GetVersionCode(&versionCode);
            app->AddPackage(packageName, versionCode, mAm->mProcessStats);
            if (SUCCEEDED(RealStartServiceLocked(r, app, execInFg)))
                return String(NULL);
            Slogger::W(TAG, "Exception when starting service %s", r->mShortName.string());

            // If a dead object exception was thrown -- fall through to
            // restart the application.
        }
    }
    else {
        // If this service runs in an isolated process, then each time
        // we call startProcessLocked() we will get a new isolated
        // process, starting another process if we are currently waiting
        // for a previous process to come up.  To deal with this, we store
        // in the service any current isolated process it is running in or
        // waiting to have come up.
        app = r->mIsolatedProc;
    }

    // Not running -- get it started, and enqueue this service record
    // to be executed when the app comes up.
    if (app == NULL) {
        if ((app = mAm->StartProcessLocked(procName, r->mAppInfo, TRUE, intentFlags,
                String("service"), r->mName, FALSE, isolated, FALSE)) == NULL) {
            StringBuilder msg("Unable to launch app ");
            String pName;
            IPackageItemInfo::Probe(r->mAppInfo)->GetPackageName(&pName);
            msg += pName;
            msg += "/";
            Int32 uid;
            r->mAppInfo->GetUid(&uid);
            msg += uid;
            msg += " for service ";
            AutoPtr<IIntent> intent;
            r->mIntent->GetIntent((IIntent**)&intent);
            msg += intent;
            msg += ": process is bad";
            Slogger::W(TAG, msg.ToString().string());
            BringDownServiceLocked(r);
            return msg.ToString();
        }
        if (isolated) {
            r->mIsolatedProc = app;
        }
    }

    if (Find(mPendingServices.Begin(), mPendingServices.End(), AutoPtr<CServiceRecord>(r))
            == mPendingServices.End()) {
        mPendingServices.PushBack(r);
    }

    if (r->mDelayedStop) {
        // Oh and hey we've already been asked to stop!
        r->mDelayedStop = FALSE;
        if (r->mStartRequested) {
            if (DEBUG_DELAYED_STARTS)
                Slogger::V(TAG, "Applying delayed stop (in bring up): %s", TO_CSTR(r));
            StopServiceLocked(r);
        }
    }

    return String(NULL);
}

ECode ActiveServices::RequestServiceBindingsLocked(
    /* [in] */ CServiceRecord* r,
    /* [in] */ Boolean execInFg)
{
    HashMap< AutoPtr<IIntentFilterComparison>, AutoPtr<IntentBindRecord> >::Iterator it;
    for (it = r->mBindings.Begin(); it != r->mBindings.End(); ++it) {
        AutoPtr<IntentBindRecord> i = it->mSecond;
        if (!RequestServiceBindingLocked(r, i, execInFg, FALSE)) {
            break;
        }
    }
    return NOERROR;
}

ECode ActiveServices::RealStartServiceLocked(
    /* [in] */ CServiceRecord* r,
    /* [in] */ ProcessRecord* app,
    /* [in] */ Boolean execInFg)
{
    if (app->mThread == NULL) {
        return E_REMOTE_EXCEPTION;
    }
    if (DEBUG_MU) {
        Int32 uid;
        r->mAppInfo->GetUid(&uid);
        Slogger::V(TAG_MU, "realStartServiceLocked, CServiceRecord.uid = %d, ProcessRecord.uid = %d", uid
                , app->mUid);
    }
    r->mApp = app;
    r->mRestartTime = r->mLastActivity = SystemClock::GetUptimeMillis();

    app->mServices.Insert(r);
    BumpServiceExecutingLocked(r, execInFg, String("create"));
    mAm->UpdateLruProcessLocked(app, FALSE, NULL);
    mAm->UpdateOomAdjLocked();

    Boolean created = FALSE;
//     try {
    Int32 lastPeriod = r->mShortName.LastIndexOf('.');
    String nameTerm = lastPeriod >= 0 ? r->mShortName.Substring(lastPeriod) : r->mShortName;
    // if (LOG_SERVICE_START_STOP) {
    //     EventLogTags.writeAmCreateService(
    //             r->mUserId, System.identityHashCode(r), nameTerm, r->mApp.uid, r->mApp.pid);
    // }
    if (r->mStats != NULL) {
        AutoPtr<IBatteryStatsImpl> stats;
        r->mStats->GetBatteryStats((IBatteryStatsImpl**)&stats);
        {    AutoLock syncLock(stats);
            r->mStats->StartLaunchedLocked();
        }
    }

    String pkgName;
    IPackageItemInfo::Probe(r->mServiceInfo)->GetPackageName(&pkgName);
    mAm->EnsurePackageDexOpt(pkgName);
    app->ForceProcessStateUpTo(IActivityManager::PROCESS_STATE_SERVICE);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(r->mServiceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    if (FAILED(app->mThread->ScheduleCreateService(r, r->mServiceInfo,
        mAm->CompatibilityInfoForPackageLocked(appInfo), app->mRepProcState))) {
        Slogger::W(TAG, "Application dead when creating service %s", TO_CSTR(r));
        mAm->AppDiedLocked(app);
    }
    else {
        r->PostNotification();
        created = TRUE;
    }

    if (!created) {
        app->mServices.Erase(r);
        r->mApp = NULL;
        ScheduleServiceRestartLocked(r, FALSE);
        return NOERROR;
    }

    RequestServiceBindingsLocked(r, execInFg);

    UpdateServiceClientActivitiesLocked(app, NULL, TRUE);

    // If the service is in the started state, and there are no
    // pending arguments, then fake up one so its onStartCommand() will
    // be called.
    if (r->mStartRequested && r->mCallStart && r->mPendingStarts.IsEmpty()) {
        AutoPtr<CServiceRecord::StartItem> item = new CServiceRecord::StartItem(r, FALSE, r->MakeNextStartId(),
                NULL, NULL);
        r->mPendingStarts.PushBack(item);
    }

    SendServiceArgsLocked(r, execInFg, TRUE);

    if (r->mDelayed) {
        if (DEBUG_DELAYED_STARTS)
            Slogger::V(TAG, "REM FR DELAY LIST (new proc): %s", TO_CSTR(r));
        GetServiceMap(r->mUserId)->mDelayedStartList.Remove(r);
        r->mDelayed = FALSE;
    }

    if (r->mDelayedStop) {
        // Oh and hey we've already been asked to stop!
        r->mDelayedStop = FALSE;
        if (r->mStartRequested) {
            if (DEBUG_DELAYED_STARTS)
                Slogger::V(TAG, "Applying delayed stop (from start): %s", TO_CSTR(r));
            StopServiceLocked(r);
        }
    }

    return NOERROR;
}

ECode ActiveServices::SendServiceArgsLocked(
    /* [in] */ CServiceRecord* r,
    /* [in] */ Boolean execInFg,
    /* [in] */ Boolean oomAdjusted)
{
    const Int32 N = r->mPendingStarts.GetSize();
    if (N == 0) {
        return NOERROR;
    }

   while (r->mPendingStarts.IsEmpty() == FALSE) {
//         try {
        AutoPtr<CServiceRecord::StartItem> si = r->mPendingStarts[0];
        r->mPendingStarts.Remove(0);
        if (DEBUG_SERVICE) Slogger::V(TAG, "Sending arguments to: %s %s args=%s"
                , TO_CSTR(r), TO_CSTR(r->mIntent), TO_CSTR(si->mIntent));
        if (si->mIntent == NULL && N > 1) {
            // If somehow we got a dummy NULL intent in the middle,
            // then skip it.  DO NOT skip a NULL intent when it is
            // the only one in the list -- this is to support the
            // onStartCommand(NULL) case.
            continue;
        }
        si->mDeliveredTime = SystemClock::GetUptimeMillis();
        r->mDeliveredStarts.PushBack(si);
        si->mDeliveryCount++;
        if (si->mNeededGrants != NULL) {
            mAm->GrantUriPermissionUncheckedFromIntentLocked(si->mNeededGrants,
                    si->GetUriPermissionsLocked());
        }
        BumpServiceExecutingLocked(r, execInFg, String("start"));
        if (!oomAdjusted) {
            oomAdjusted = TRUE;
            mAm->UpdateOomAdjLocked(r->mApp);
        }
        Int32 flags = 0;
        if (si->mDeliveryCount > 1) {
            flags |= IService::START_FLAG_RETRY;
        }
        if (si->mDoneExecutingCount > 0) {
            flags |= IService::START_FLAG_REDELIVERY;
        }
        ECode ec = r->mApp->mThread->ScheduleServiceArgs(r, si->mTaskRemoved, si->mId, flags, si->mIntent);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            // Remote process gone...  we'll let the normal cleanup take
            // care of this.
            if (DEBUG_SERVICE) Slogger::V(TAG, "Crashed while scheduling start: %s", TO_CSTR(r));
            break;
        }
        else if (FAILED(ec)) {
            Slogger::W(TAG, "Unexpected exception 0x%08x", ec);
            break;
       }
   }
   return NOERROR;
}

Boolean ActiveServices::IsServiceNeeded(
    /* [in] */ CServiceRecord* r,
    /* [in] */ Boolean knowConn,
    /* [in] */ Boolean hasConn)
{
    // Are we still explicitly being asked to run?
    if (r->mStartRequested) {
        return TRUE;
    }

    // Is someone still bound to us keepign us running?
    if (!knowConn) {
        hasConn = r->HasAutoCreateConnections();
    }
    if (hasConn) {
        return TRUE;
    }

    return FALSE;
}

ECode ActiveServices::BringDownServiceIfNeededLocked(
    /* [in] */ CServiceRecord* r,
    /* [in] */ Boolean knowConn,
    /* [in] */ Boolean hasConn)
{
    //Slog.i(TAG, "Bring down service:");
    //r.dump("  ");

    if (IsServiceNeeded(r, knowConn, hasConn)) {
        return NOERROR;
    }

    // Are we in the process of launching?
    if (Find(mPendingServices.Begin(), mPendingServices.End(), AutoPtr<CServiceRecord>(r))
            != mPendingServices.End()) {
        return NOERROR;
    }

    return BringDownServiceLocked(r);
}

ECode ActiveServices::BringDownServiceLocked(
    /* [in] */ CServiceRecord* r)
{
   //Slog.i(TAG, "Bring down service:");
   //r.dump("  ");

    // Report to all of the connections that the service is no longer
    // available.
    HashMap< AutoPtr<IBinder>, AutoPtr<ConnectionRecordList> >::Iterator it;
    for (it = r->mConnections.Begin(); it != r->mConnections.End(); ++it) {
        // Report to all of the connections that the service is no longer
        // available.
        AutoPtr<ConnectionRecordList>  c = it->mSecond;
        ConnectionRecordListIterator cit;
        for (cit = c->Begin(); cit != c->End(); ++cit) {
            AutoPtr<ConnectionRecord> cr = *cit;
            // There is still a connection to the service that is
            // being brought down.  Mark it as dead.
            cr->mServiceDead = TRUE;
            ECode ec = cr->mConn->Connected(r->mName, NULL);
            if (FAILED(ec))
                Slogger::W(TAG, "Failure disconnecting service %s to connection %s (in %s) 0x%08x",
                    TO_CSTR(r->mName), TO_CSTR(cr->mConn), cr->mBinding->mClient->mProcessName.string(), ec);
        }
    }

    // Tell the service that it has been unbound.
    if (r->mApp != NULL && r->mApp->mThread != NULL) {
        HashMap< AutoPtr<IIntentFilterComparison>, AutoPtr<IntentBindRecord> >::Iterator it;
        for (it = r->mBindings.Begin(); it != r->mBindings.End(); ++it) {
            AutoPtr<IntentBindRecord> ibr = it->mSecond;
            if (DEBUG_SERVICE) Slogger::V(TAG, "Bringing down binding %s: hasBound=%d",
                TO_CSTR(ibr), ibr->mHasBound);
            if (ibr->mHasBound) {
                BumpServiceExecutingLocked(r, FALSE, String("bring down unbind"));
                mAm->UpdateOomAdjLocked(r->mApp);
                ibr->mHasBound = FALSE;
                AutoPtr<IIntent> intent;
                ibr->mIntent->GetIntent((IIntent**)&intent);
                if (FAILED(r->mApp->mThread->ScheduleUnbindService(r, intent))) {
                    Slogger::W(TAG, "Exception when unbinding service %s", r->mShortName.string());
                    ServiceProcessGoneLocked(r);
                }
            }
        }
    }

    if (DEBUG_SERVICE) Slogger::V(TAG, "Bringing down %s %s", TO_CSTR(r), TO_CSTR(r->mIntent));
    // if (LOG_SERVICE_START_STOP) {
    //     EventLogTags.writeAmDestroyService(
    //             r.userId, System.identityHashCode(r), (r.app != NULL) ? r.app.pid : -1);
    // }

    AutoPtr<ServiceMap> smap = GetServiceMap(r->mUserId);
    smap->mServicesByName.Erase(r->mName);
    smap->mServicesByIntent.Erase(r->mIntent);
    r->mTotalRestartCount = 0;
    UnscheduleServiceRestartLocked(r, 0, TRUE);

    // Also make sure it is not on the pending list.
    List< AutoPtr<CServiceRecord> >::Iterator pIt;
    for (pIt = mPendingServices.Begin(); pIt != mPendingServices.End();) {
        if ((*pIt).Get() == r) {
            pIt = mPendingServices.Erase(pIt);
            if (DEBUG_SERVICE) Slogger::V(TAG, "Removed pending: %s", TO_CSTR(r));
        }
        else {
          ++pIt;
        }
    }

    r->CancelNotification();
    r->mIsForeground = FALSE;
    r->mForegroundId = 0;
    r->mForegroundNoti = NULL;

    // Clear start entries.
    r->ClearDeliveredStartsLocked();
    r->mPendingStarts.Clear();

    if (r->mApp != NULL) {
        if (r->mStats != NULL) {
            AutoPtr<IBatteryStatsImpl> stats;
            r->mStats->GetBatteryStats((IBatteryStatsImpl**)&stats);
            {    AutoLock syncLock(stats);
                r->mStats->StopLaunchedLocked();
            }
        }

        r->mApp->mServices.Erase(r);
        if (r->mApp->mThread != NULL) {
            UpdateServiceForegroundLocked(r->mApp, FALSE);
            BumpServiceExecutingLocked(r, FALSE, String("destroy"));
            mDestroyingServices.PushBack(r);
            mAm->UpdateOomAdjLocked(r->mApp);
            ECode ec = r->mApp->mThread->ScheduleStopService(r);
            if (FAILED(ec)) {
                Slogger::W(TAG, "Exception when destroying service %s ecode: 0x%08x"
                      , r->mShortName.string(), ec);
                ServiceProcessGoneLocked(r);
            }
        }
        else {
            if (DEBUG_SERVICE)
                Slogger::V(TAG, "Removed service that has no process: %s", TO_CSTR(r));
        }
    }
    else {
        if (DEBUG_SERVICE)
            Slogger::V(TAG, "Removed service that is not running: %s", TO_CSTR(r));
    }

    if (r->mBindings.IsEmpty() == FALSE) {
        r->mBindings.Clear();
    }

    if (IServiceRestarter::Probe(r->mRestarter) != NULL) {
        ServiceRestarter* sr = (ServiceRestarter*)IServiceRestarter::Probe(r->mRestarter);
        sr->SetService(NULL);
    }

    Int32 memFactor = mAm->mProcessStats->GetMemFactorLocked();
    Int64 now = SystemClock::GetUptimeMillis();
    if (r->mTracker != NULL) {
        r->mTracker->SetStarted(FALSE, memFactor, now);
        r->mTracker->SetBound(FALSE, memFactor, now);
        if (r->mExecuteNesting == 0) {
            r->mTracker->ClearCurrentOwner(r->Probe(EIID_IInterface), FALSE);
            r->mTracker = NULL;
        }
    }

    smap->EnsureNotStartingBackground(r);

    return NOERROR;
}

ECode ActiveServices::RemoveConnectionLocked(
    /* [in] */ ConnectionRecord* c,
    /* [in] */ ProcessRecord* skipApp,
    /* [in] */ ActivityRecord* skipAct)
{
    AutoPtr<IBinder> binder = IBinder::Probe(c->mConn);
    AutoPtr<AppBindRecord> b = c->mBinding;
    AutoPtr<CServiceRecord> s = b->mService;
    AutoPtr<ConnectionRecordList>  clist;
    HashMap< AutoPtr<IBinder>, AutoPtr<ConnectionRecordList> >::Iterator it =
            s->mConnections.Find(binder);
    if (it != s->mConnections.End()) {
        clist = it->mSecond;
    }
    if (clist != NULL) {
        clist->Remove(c);
        if (clist->IsEmpty()) {
            s->mConnections.Erase(it);
        }
    }
    b->mConnections.Erase(c);
    if (c->mActivity != NULL && c->mActivity != skipAct) {
        if (c->mActivity->mConnections != NULL) {
            c->mActivity->mConnections->Erase(c);
        }
    }
    if (b->mClient != skipApp) {
        b->mClient->mConnections.Erase(c);
        if ((c->mFlags&IContext::BIND_ABOVE_CLIENT) != 0) {
            b->mClient->UpdateHasAboveClientLocked();
        }
        if (s->mApp != NULL) {
            UpdateServiceClientActivitiesLocked(s->mApp, c, TRUE);
        }
    }
    HashMap< AutoPtr<IBinder>, AutoPtr<ConnectionRecordList> >::Iterator cit =
            mServiceConnections.Find(binder);
    if (cit != mServiceConnections.End()) {
        clist = cit->mSecond;
    }
    if (clist != NULL) {
        clist->Remove(c);
        if (clist->IsEmpty()) {
            mServiceConnections.Erase(cit);
        }
    }

    if (b->mConnections.IsEmpty()) {
        b->mIntent->mApps.Erase(b->mClient);
    }

    if (!c->mServiceDead) {
        if (DEBUG_SERVICE) Slogger::V(TAG, "Disconnecting binding %s: shouldUnbind=%d",
            TO_CSTR(b->mIntent), b->mIntent->mHasBound);
        if (s->mApp != NULL && s->mApp->mThread != NULL && b->mIntent->mApps.IsEmpty()
                && b->mIntent->mHasBound) {
            BumpServiceExecutingLocked(s, FALSE, String("unbind"));
            if (b->mClient != s->mApp && (c->mFlags & IContext::BIND_WAIVE_PRIORITY) == 0
                && s->mApp->mSetProcState <= IActivityManager::PROCESS_STATE_RECEIVER) {
                // If this service's process is not already in the cached list,
                // then update it in the LRU list here because this may be causing
                // it to go down there and we want it to start out near the top.
                mAm->UpdateLruProcessLocked(s->mApp, FALSE, NULL);
            }
            mAm->UpdateOomAdjLocked(s->mApp);
            b->mIntent->mHasBound = FALSE;
            // Assume the client doesn't want to know about a rebind;
            // we will deal with that later if it asks for one.
            b->mIntent->mDoRebind = FALSE;
            AutoPtr<IIntent> bIntent;
            ECode ec = b->mIntent->mIntent->GetIntent((IIntent**)&bIntent);
            FAIL_GOTO(ec, __EXIT__)

            ec = s->mApp->mThread->ScheduleUnbindService(s, bIntent);
            FAIL_GOTO(ec, __EXIT__)

__EXIT__:
            if (FAILED(ec)) {
                Slogger::W(TAG, "Exception when unbinding service %s", s->mShortName.string());
                ServiceProcessGoneLocked(s);
            }
        }

        if ((c->mFlags & IContext::BIND_AUTO_CREATE) != 0) {
            Boolean hasAutoCreate = s->HasAutoCreateConnections();
            if (!hasAutoCreate) {
                if (s->mTracker != NULL) {
                    s->mTracker->SetBound(FALSE, mAm->mProcessStats->GetMemFactorLocked(),
                        SystemClock::GetUptimeMillis());
                }
            }
            BringDownServiceIfNeededLocked(s, TRUE, hasAutoCreate);
        }
    }
    return NOERROR;
}

ECode ActiveServices::ServiceDoneExecutingLocked(
    /* [in] */ CServiceRecord* r,
    /* [in] */ Int32 type,
    /* [in] */ Int32 startId,
    /* [in] */ Int32 res)
{
    Boolean inDestroying = FALSE;
    List< AutoPtr<CServiceRecord> >::Iterator it = Find(
        mDestroyingServices.Begin(), mDestroyingServices.End(), AutoPtr<CServiceRecord>(r));
    if (it != mDestroyingServices.End())
        inDestroying = TRUE;

    if (r != NULL) {
        if (type == 1) {
            // This is a call from a service start...  take care of
            // book-keeping.
            r->mCallStart = TRUE;
            switch (res) {
                case IService::START_STICKY_COMPATIBILITY:
                case IService::START_STICKY: {
                    // We are done with the associated start arguments.
                    r->FindDeliveredStart(startId, TRUE);
                    // Don't stop if killed.
                    r->mStopIfKilled = FALSE;
                    break;
                }
                case IService::START_NOT_STICKY: {
                    // We are done with the associated start arguments.
                    r->FindDeliveredStart(startId, TRUE);
                    if (r->GetLastStartId() == startId) {
                        // There is no more work, and this service
                        // doesn't want to hang around if killed.
                        r->mStopIfKilled = TRUE;
                    }
                    break;
                }
                case IService::START_REDELIVER_INTENT: {
                    // We'll keep this item until they explicitly
                    // call stop for it, but keep track of the fact
                    // that it was delivered.
                    AutoPtr<CServiceRecord::StartItem> si = r->FindDeliveredStart(startId, FALSE);
                    if (si != NULL) {
                        si->mDeliveryCount = 0;
                        si->mDoneExecutingCount++;
                        // Don't stop if killed.
                        r->mStopIfKilled = TRUE;
                    }
                    break;
                }
                case IService::START_TASK_REMOVED_COMPLETE: {
                    // Special processing for onTaskRemoved().  Don't
                    // impact normal onStartCommand() processing.
                    r->FindDeliveredStart(startId, TRUE);
                    break;
                }
                default:
                    Slogger::E(TAG, "Unknown service start result: %d", res);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            if (res == IService::START_STICKY_COMPATIBILITY) {
                r->mCallStart = FALSE;
            }
        }
        Int64 origId = Binder::ClearCallingIdentity();
        ServiceDoneExecutingLocked(r, inDestroying, inDestroying);
        Binder::RestoreCallingIdentity(origId);
    }
    else {
        Int32 pid = Binder::GetCallingPid();
        Slogger::W(TAG, "Done executing unknown service from pid %d", pid);
    }
    return NOERROR;
}

void ActiveServices::ServiceProcessGoneLocked(
    /* [in] */ CServiceRecord* r)
{
    if (r->mTracker != NULL) {
        Int32 memFactor = mAm->mProcessStats->GetMemFactorLocked();
        Int64 now = SystemClock::GetUptimeMillis();
        r->mTracker->SetExecuting(FALSE, memFactor, now);
        r->mTracker->SetBound(FALSE, memFactor, now);
        r->mTracker->SetStarted(FALSE, memFactor, now);
    }
    ServiceDoneExecutingLocked(r, TRUE, TRUE);
}

ECode ActiveServices::ServiceDoneExecutingLocked(
    /* [in] */ CServiceRecord* r,
    /* [in] */ Boolean inDestroying,
    /* [in] */ Boolean finishing)
{
    if (DEBUG_SERVICE) Slogger::V(TAG, "<<< DONE EXECUTING %s: nesting=%d, inDestroying=%d, app=%s",
        TO_CSTR(r), r->mExecuteNesting, inDestroying, TO_CSTR(r->mApp));
    else if (DEBUG_SERVICE_EXECUTING)
        Slogger::V(TAG, "<<< DONE EXECUTING %s", r->mShortName.string());
    r->mExecuteNesting--;
    if (r->mExecuteNesting <= 0) {
        if (r->mApp != NULL) {
            if (DEBUG_SERVICE)
                Slogger::V(TAG, "Nesting at 0 of %s", r->mShortName.string());

            r->mApp->mExecServicesFg = FALSE;
            r->mApp->mExecutingServices.Erase(r);
            if (r->mApp->mExecutingServices.IsEmpty()) {
                if (DEBUG_SERVICE || DEBUG_SERVICE_EXECUTING) Slogger::V(TAG,
                        "No more executingServices of %s", r->mShortName.string());

                mAm->mHandler->RemoveMessages(
                    CActivityManagerService::SERVICE_TIMEOUT_MSG,
                    r->mApp->Probe(EIID_IInterface));
            }
            else if (r->mExecuteFg) {
                // Need to re-evaluate whether the app still needs to be in the foreground.
                HashSet< AutoPtr<CServiceRecord> >::Iterator it = r->mApp->mExecutingServices.Begin();
                for (; it != r->mApp->mExecutingServices.End(); ++it) {
                    if ((*it)->mExecuteFg) {
                        r->mApp->mExecServicesFg = TRUE;
                        break;
                    }
                }
            }
            if (inDestroying) {
                if (DEBUG_SERVICE)
                    Slogger::V(TAG, "doneExecuting remove stopping %s", TO_CSTR(r));
                mDestroyingServices.Remove(r);
                r->mBindings.Clear();
            }
            mAm->UpdateOomAdjLocked(r->mApp);
        }
        r->mExecuteFg = FALSE;
        if (r->mTracker != NULL) {
            r->mTracker->SetExecuting(FALSE, mAm->mProcessStats->GetMemFactorLocked(),
                    SystemClock::GetUptimeMillis());
            if (finishing) {
                r->mTracker->ClearCurrentOwner(r->Probe(EIID_IInterface), FALSE);
                r->mTracker = NULL;
            }
        }
        if (finishing) {
            if (r->mApp != NULL && !r->mApp->mPersistent) {
                r->mApp->mServices.Erase(r);
            }
            r->mApp = NULL;
        }
    }
    return NOERROR;
}

ECode ActiveServices::AttachApplicationLocked(
    /* [in] */ ProcessRecord* proc,
    /* [in] */ const String& processName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean didSomething = FALSE;
    // Collect any services that are waiting for this process to come up.
    if (!mPendingServices.IsEmpty()) {
        AutoPtr<CServiceRecord> sr;
        List<AutoPtr<CServiceRecord> >::Iterator it = mPendingServices.Begin();
        while (it != mPendingServices.End()) {
            sr = *it;
            Int32 srUid;
            sr->mAppInfo->GetUid(&srUid);
            if (proc != sr->mIsolatedProc && (proc->mUid != srUid
                    || !processName.Equals(sr->mProcessName))) {
                ++it;
                continue;
            }

            it = mPendingServices.Erase(it);
            String packageName;
            IPackageItemInfo::Probe(sr->mAppInfo)->GetPackageName(&packageName);
            Int32 versionCode;
            sr->mAppInfo->GetVersionCode(&versionCode);
            proc->AddPackage(packageName, versionCode, mAm->mProcessStats);
            ECode ec = RealStartServiceLocked(sr, proc, sr->mCreatedFromFg);
            if (FAILED(ec)) {
                Slogger::W(TAG, "Exception in new application when starting service %s, ec = 0x%08x",
                    sr->mShortName.string(), ec);
                return ec;
            }
            didSomething = TRUE;
        }
    }
    // Also, if there are any services that are waiting to restart and
    // would run in this process, now is a good time to start them.  It would
    // be weird to bring up the process but arbitrarily not let the services
    // run at this point just because their restart time hasn't come up.
    if (!mRestartingServices.IsEmpty()) {
        AutoPtr<CServiceRecord> sr;
        List< AutoPtr<CServiceRecord> >::Iterator it;
        for (it = mRestartingServices.Begin(); it != mRestartingServices.End(); ++it) {
            sr = *it;
            Int32 srUid;
            sr->mAppInfo->GetUid(&srUid);
            if (proc != sr->mIsolatedProc && (proc->mUid != srUid
                    || !processName.Equals(sr->mProcessName))) {
                continue;
            }

            mAm->mHandler->RemoveCallbacks(sr->mRestarter);
            Boolean result;
            mAm->mHandler->Post(sr->mRestarter, &result);
        }
    }
    *result = didSomething;
    return NOERROR;
}

ECode ActiveServices::ProcessStartTimedOutLocked(
    /* [in] */ ProcessRecord* proc)
{
    List< AutoPtr<CServiceRecord> >::Iterator it;
    for (it = mPendingServices.Begin(); it != mPendingServices.End();) {
        AutoPtr<CServiceRecord> sr = *it;
        Int32 appUid = 0;
        sr->mAppInfo->GetUid(&appUid);
        if ((proc->mUid == appUid
                && proc->mProcessName.Equals(sr->mProcessName))
                || sr->mIsolatedProc.Get() == proc) {
            Slogger::W(TAG, "Forcing bringing down service: %s", TO_CSTR(sr));
            sr->mIsolatedProc = NULL;
            it = mPendingServices.Erase(it);
            BringDownServiceLocked(sr);
        }
        else {
          ++it;
        }
    }
    return NOERROR;
}

Boolean ActiveServices::CollectForceStopServicesLocked(
    /* [in] */ const String& name,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean evenPersistent,
    /* [in] */ Boolean doit,
    /* [in] */ IComponentNameCServiceRecordHashMap& services,
    /* [in] */ List<AutoPtr<CServiceRecord> >* result)
{
    Boolean didSomething = FALSE;
    IComponentNameCServiceRecordHashMap::Iterator it;
    for (it = services.Begin(); it != services.End(); ++it) {
        AutoPtr<CServiceRecord> service = it->mSecond;
        if ((name == NULL || service->mPackageName.Equals(name))
            && (service->mApp == NULL || evenPersistent || !service->mApp->mPersistent)) {
            if (!doit) {
                return TRUE;
            }
            didSomething = TRUE;
            Slogger::I(TAG, "  Force stopping service %s", TO_CSTR(service));
            if (service->mApp != NULL) {
                service->mApp->mRemoved = TRUE;
                if (!service->mApp->mPersistent) {
                    service->mApp->mServices.Erase(service);
                }
            }
            service->mApp = NULL;
            service->mIsolatedProc = NULL;
            result->PushBack(service);
        }
    }
    return didSomething;
}

Boolean ActiveServices::ForceStopLocked(
    /* [in] */ const String& name,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean evenPersistent,
    /* [in] */ Boolean doit)
{
    Boolean didSomething = FALSE;
    List<AutoPtr<CServiceRecord> > services;
    if (userId == IUserHandle::USER_ALL) {
        HashMap<Int32, AutoPtr<ServiceMap> >::Iterator it;
        for (it = mServiceMap.Begin(); it != mServiceMap.End(); ++it) {
            didSomething |= CollectForceStopServicesLocked(name, userId, evenPersistent,
                    doit, it->mSecond->mServicesByName, &services);
            if (!doit && didSomething) {
                return TRUE;
            }
        }
    }
    else {
        HashMap<Int32, AutoPtr<ServiceMap> >::Iterator it = mServiceMap.Find(userId);
        if (it != mServiceMap.End()) {
            AutoPtr<ServiceMap> smap = it->mSecond;
            if (smap != NULL) {
                didSomething = CollectForceStopServicesLocked(name, userId, evenPersistent,
                        doit, smap->mServicesByName, &services);
            }
        }
    }

    List<AutoPtr<CServiceRecord> >::Iterator sit;
    for (sit = services.Begin(); sit != services.End(); ++sit) {
        BringDownServiceLocked(*sit);
    }
    return didSomething;
}

ECode ActiveServices::CleanUpRemovedTaskLocked(
    /* [in] */ TaskRecord* tr,
    /* [in] */ IComponentName* component,
    /* [in] */ IIntent* baseIntent)
{
    List<AutoPtr<CServiceRecord> > services;
    String pkgName;
    component->GetPackageName(&pkgName);
    IComponentNameCServiceRecordHashMap& alls = GetServices(tr->mUserId);
    IComponentNameCServiceRecordHashMap::Iterator it;
    for (it = alls.Begin(); it != alls.End(); ++it) {
        AutoPtr<CServiceRecord> sr = it->mSecond;
        if (sr->mPackageName.Equals(pkgName)) {
            services.PushBack(sr);
        }
    }

    // Take care of any running services associated with the app.
    List<AutoPtr<CServiceRecord> >::Iterator sit;
    for (sit = services.Begin(); sit != services.End(); ++sit) {
        AutoPtr<CServiceRecord> sr = *sit;
        if (sr->mStartRequested) {
            Int32 flags;
            sr->mServiceInfo->GetFlags(&flags);
            if ((flags&IServiceInfo::FLAG_STOP_WITH_TASK) != 0) {
                Slogger::I(TAG, "Stopping service %s: remove task", sr->mShortName.string());
                StopServiceLocked(sr);
            }
            else {
                sr->mPendingStarts.PushBack(new CServiceRecord::StartItem(sr, TRUE,
                        sr->MakeNextStartId(), baseIntent, NULL));
                if (sr->mApp != NULL && sr->mApp->mThread != NULL) {
                    // We always run in the foreground, since this is called as
                    // part of the "remove task" UI operation.
                    SendServiceArgsLocked(sr, TRUE, FALSE);
                }
            }
        }
    }
    return NOERROR;
}

ECode ActiveServices::KillServicesLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Boolean allowRestart)
{
    // Report disconnected services.
    if (FALSE) {
        // XXX we are letting the client link to the service for
        // death notifications.
        if (app->mServices.IsEmpty() == FALSE) {
            HashSet< AutoPtr<CServiceRecord> >::Iterator it;
            for (it = app->mServices.Begin(); it != app->mServices.End(); ++it) {
                AutoPtr<CServiceRecord> r = *it;
                CServiceRecord::ConnectionHashMap::Iterator rit;
                for(rit = r->mConnections.Begin(); rit != r->mConnections.End(); ++rit) {
                    AutoPtr<ConnectionRecordList> cl = rit->mSecond;
                    ConnectionRecordListIterator clit;
                    for (clit = cl->Begin(); clit != cl->End(); ++clit) {
                        AutoPtr<ConnectionRecord> c = *clit;
                        if (c->mBinding->mClient != app) {
                            // try {
                            //     // c.conn.connected(r.className, NULL);
                            // } catch (Exception e) {
                            //     // todo: this should be asynchronous!
                            //     Slog.w(TAG, "Exception thrown disconnected servce "
                            //           + r.shortName
                            //           + " from app " + app.processName, e);
                            // }
                        }
                    }
                }
            }
        }
    }

    // First clear app state from services.
    HashSet< AutoPtr<CServiceRecord> >::Iterator it;
    for (it = app->mServices.Begin(); it != app->mServices.End(); ++it) {
        AutoPtr<CServiceRecord> sr = *it;
        AutoPtr<IBatteryStatsImpl> stats;
        sr->mStats->GetBatteryStats((IBatteryStatsImpl**)&stats);
        {
            AutoLock lock(stats);
            sr->mStats->StopLaunchedLocked();
        }

        if (sr->mApp.Get() != app && sr->mApp != NULL && !sr->mApp->mPersistent) {
            sr->mApp->mServices.Erase(sr);
        }
        sr->mApp = NULL;
        sr->mIsolatedProc = NULL;
        sr->mExecuteNesting = 0;
        sr->ForceClearTracker();

        List< AutoPtr<CServiceRecord> >::Iterator find = Find(
            mDestroyingServices.Begin(), mDestroyingServices.End(), AutoPtr<CServiceRecord>(sr));
        if (find != mDestroyingServices.End()) {
            mDestroyingServices.Erase(find);
            if (DEBUG_SERVICE)
                Slogger::V(TAG, "killServices remove destroying %s", TO_CSTR(sr));
        }

        HashMap< AutoPtr<IIntentFilterComparison>, AutoPtr<IntentBindRecord> >::Iterator ibit;
        for (ibit = sr->mBindings.Begin(); ibit != sr->mBindings.End(); ++ibit) {
            AutoPtr<IntentBindRecord> b = ibit->mSecond;
            if (DEBUG_SERVICE) Slogger::V(TAG, "Killing binding %s: shouldUnbind=%d",
                TO_CSTR(b), b->mHasBound);
            b->mBinder = NULL;
            b->mRequested = b->mReceived = b->mHasBound = FALSE;
            // If this binding is coming from a cached process and is asking to keep
            // the service created, then we'll kill the cached process as well -- we
            // don't want to be thrashing around restarting processes that are only
            // there to be cached.
            HashMap< AutoPtr<ProcessRecord>, AutoPtr<AppBindRecord> >::Iterator appsit;
            for (appsit = b->mApps.Begin(); appsit != b->mApps.End(); ++appsit) {
                AutoPtr<ProcessRecord> proc = appsit->mFirst;
                // If the process is already gone, skip it.
                if (proc->mKilledByAm || proc->mThread == NULL) {
                    continue;
                }
                // Only do this for processes that have an auto-create binding;
                // otherwise the binding can be left, because it won't cause the
                // service to restart.
                AutoPtr<AppBindRecord> abind = appsit->mSecond;
                Boolean hasCreate = FALSE;
                HashSet< AutoPtr<ConnectionRecord> >::Iterator connIt;
                for (connIt = abind->mConnections.Begin(); connIt != abind->mConnections.End(); ++connIt) {
                    AutoPtr<ConnectionRecord> conn = *connIt;
                    if ((conn->mFlags & (IContext::BIND_AUTO_CREATE | IContext::BIND_ALLOW_OOM_MANAGEMENT
                        | IContext::BIND_WAIVE_PRIORITY)) == IContext::BIND_AUTO_CREATE) {
                        hasCreate = TRUE;
                        break;
                    }
                }
                if (!hasCreate) {
                    continue;
                }
                // XXX turned off for now until we have more time to get a better policy.
                if (FALSE && proc != NULL && !proc->mPersistent && proc->mThread != NULL
                    && proc->mPid != 0 && proc->mPid != CActivityManagerService::MY_PID
                    && proc->mSetProcState >= IActivityManager::PROCESS_STATE_LAST_ACTIVITY) {
                    String name;
                    sr->mName->FlattenToShortString(&name);
                    StringBuilder sb;
                    sb += "bound to service ";
                    sb += name;
                    sb += " in dying proc ";
                    sb += app != NULL ? app->mProcessName : "??";
                    proc->Kill(sb.ToString(), TRUE);
                }
            }
        }
    }

    // Clean up any connections this application has to other services.
    HashSet< AutoPtr<ConnectionRecord> >::Iterator connIt;
    for (connIt = app->mConnections.Begin(); connIt != app->mConnections.End(); ++connIt) {
        RemoveConnectionLocked(*connIt, app, NULL);
    }
    UpdateServiceConnectionActivitiesLocked(app);
    app->mConnections.Clear();

    AutoPtr<ServiceMap> smap = GetServiceMap(app->mUserId);

    // Now do remaining service cleanup.
    it = app->mServices.Begin();
    while (it != app->mServices.End()) {
        AutoPtr<CServiceRecord> sr = *it;

        // Unless the process is persistent, this process record is going away,
        // so make sure the service is cleaned out of it.
        if (!app->mPersistent) {
            app->mServices.Erase(it++);
        }
        else ++it;

        // Sanity check: if the service listed for the app is not one
        // we actually are maintaining, just let it drop.
        IComponentNameCServiceRecordHashMap::Iterator find = smap->mServicesByName.Find(sr->mName);
        AutoPtr<CServiceRecord> curRec = find != smap->mServicesByName.End() ? find->mSecond : NULL;
        if (curRec != sr) {
            if (curRec != NULL) {
                Slogger::W/*wtf*/(TAG, "Service %s in process %s not same as in map: %s",
                    TO_CSTR(sr), TO_CSTR(app), TO_CSTR(curRec));
            }
            continue;
        }

        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(sr->mServiceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 flags;
        appInfo->GetFlags(&flags);
        if (allowRestart && sr->mCrashCount >= 2 && (flags & IApplicationInfo::FLAG_PERSISTENT) == 0) {
            Slogger::W(TAG, "Service crashed %d times, stopping: %s", sr->mCrashCount, TO_CSTR(sr));
//                 EventLog.writeEvent(EventLogTags.AM_SERVICE_CRASHED_TOO_MUCH,
//                         sr->mUserId, sr.crashCount, sr.shortName, app.pid);
            BringDownServiceLocked(sr);
        }
        else if (!allowRestart) {
            BringDownServiceLocked(sr);
        }
        else {
            Boolean canceled = ScheduleServiceRestartLocked(sr, TRUE);

            // Should the service remain running?  Note that in the
            // extreme case of so many attempts to deliver a command
            // that it failed we also will stop it here.
            if (sr->mStartRequested && (sr->mStopIfKilled || canceled)) {
                if (sr->mPendingStarts.IsEmpty()) {
                    sr->mStartRequested = FALSE;
                    if (sr->mTracker != NULL) {
                        sr->mTracker->SetStarted(FALSE, mAm->mProcessStats->GetMemFactorLocked(),
                            SystemClock::GetUptimeMillis());
                    }
                    if (!sr->HasAutoCreateConnections()) {
                        // Whoops, no reason to restart!
                        BringDownServiceLocked(sr);
                    }
                }
            }
        }
    }

    if (!allowRestart) {
        app->mServices.Clear();

        // Make sure there are no more restarting services for this process.
        List< AutoPtr<CServiceRecord> >::ReverseIterator rit;
        for (rit = mRestartingServices.RBegin(); rit != mRestartingServices.REnd();) {
            AutoPtr<CServiceRecord> r = *rit;
            if (r->mProcessName.Equals(app->mProcessName)) {
                AutoPtr<IApplicationInfo> appInfo;
                IComponentInfo::Probe(r->mServiceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
                Int32 uid, appUid;
                appInfo->GetUid(&uid);
                app->mInfo->GetUid(&appUid);
                if (uid == appUid) {
                    List< AutoPtr<CServiceRecord> >::ReverseIterator tmpRit(rit);
                    mRestartingServices.Erase((++tmpRit).GetBase());
                    ClearRestartingIfNeededLocked(r);
                    continue;
                }
            }
            ++rit;
        }
        for (rit = mPendingServices.RBegin(); rit != mPendingServices.REnd();) {
            AutoPtr<CServiceRecord> r = *rit;
            if (r->mProcessName.Equals(app->mProcessName)) {
                AutoPtr<IApplicationInfo> appInfo;
                IComponentInfo::Probe(r->mServiceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
                Int32 uid, appUid;
                appInfo->GetUid(&uid);
                app->mInfo->GetUid(&appUid);
                if (uid == appUid) {
                    List< AutoPtr<CServiceRecord> >::ReverseIterator tmpRit(rit);
                    mPendingServices.Erase((++tmpRit).GetBase());
                    continue;
                }
            }
            ++rit;
        }
    }

    // Make sure we have no more records on the stopping list.
    List< AutoPtr<CServiceRecord> >::ReverseIterator sit = mDestroyingServices.RBegin();
    while (sit != mDestroyingServices.REnd()) {
        AutoPtr<CServiceRecord> sr = *sit;
        if (sr->mApp.Get() == app) {
            List< AutoPtr<CServiceRecord> >::ReverseIterator tmpRIt(sit);
            mDestroyingServices.Erase((++tmpRIt).GetBase());
            if (DEBUG_SERVICE) Slogger::V(TAG, "killServices remove stopping %s", TO_CSTR(sr));
        }
        else {
          ++sit;
        }
    }

    app->mExecutingServices.Clear();
    return NOERROR;
}

AutoPtr<IActivityManagerRunningServiceInfo> ActiveServices::MakeRunningServiceInfoLocked(
    /* [in] */ CServiceRecord* r)
{
    AutoPtr<IActivityManagerRunningServiceInfo> info;
    ASSERT_SUCCEEDED(CActivityManagerRunningServiceInfo::New((IActivityManagerRunningServiceInfo**)&info));
    info->SetService(r->mName);
    if (r->mApp != NULL) {
        info->SetPid(r->mApp->mPid);
    }
    Int32 aUid;
    r->mAppInfo->GetUid(&aUid);
    info->SetUid(aUid);
    info->SetProcess(r->mProcessName);
    info->SetForeground(r->mIsForeground);
    info->SetActiveSince(r->mCreateTime);
    info->SetStarted(r->mStartRequested);
    info->SetClientCount(r->mConnections.GetSize());
    info->SetCrashCount(r->mCrashCount);
    info->SetLastActivityTime(r->mLastActivity);
    Int32 flags;
    info->GetFlags(&flags);
    if (r->mIsForeground) {
        flags |= IActivityManagerRunningServiceInfo::FLAG_FOREGROUND;
    }
    if (r->mStartRequested) {
        flags |= IActivityManagerRunningServiceInfo::FLAG_STARTED;
    }
    if (r->mApp != NULL && r->mApp->mPid == CActivityManagerService::MY_PID) {
        flags |= IActivityManagerRunningServiceInfo::FLAG_SYSTEM_PROCESS;
    }
    if (r->mApp != NULL && r->mApp->mPersistent) {
        flags |= IActivityManagerRunningServiceInfo::FLAG_PERSISTENT_PROCESS;
    }
    info->SetFlags(flags);

    CServiceRecord::ConnectionHashMap::Iterator it;
    for (it = r->mConnections.Begin(); it != r->mConnections.End(); ++it) {
        AutoPtr<ConnectionRecordList> connl = it->mSecond;
        ConnectionRecordListIterator cit;
        for (cit = connl->Begin(); cit != connl->End(); ++cit) {
            AutoPtr<ConnectionRecord> conn = *cit;
            if (conn->mClientLabel != 0) {
                String pkgName;
                IPackageItemInfo::Probe(conn->mBinding->mClient->mInfo)->GetPackageName(&pkgName);
                info->SetClientPackage(pkgName);
                info->SetClientLabel(conn->mClientLabel);
                return info;
            }
        }
    }
    return info;
}

AutoPtr<IList> ActiveServices::GetRunningServiceInfoLocked(
    /* [in] */ Int32 maxNum,
    /* [in] */ Int32 flags)
{
    AutoPtr<IList> res;
    CParcelableList::New((IList**)&res);

    Int32 uid = Binder::GetCallingUid();
    Int64 ident = Binder::ClearCallingIdentity();
//     try {
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    Int32 result;
    amHelper->CheckUidPermission(
                Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL,
                uid, &result);
    if (result == IPackageManager::PERMISSION_GRANTED) {
        AutoPtr< ArrayOf<Int32> > users = mAm->GetUsersLocked();
        Int32 size;
        for (Int32 ui = 0; ui < users->GetLength() && (res->GetSize(&size), size < maxNum); ui++) {
            IComponentNameCServiceRecordHashMap::Iterator it;
            IComponentNameCServiceRecordHashMap& alls = GetServices((*users)[ui]);
            for (it = alls.Begin(); it != alls.End() && (res->GetSize(&size), size < maxNum); ++it) {
                AutoPtr<CServiceRecord> r = it->mSecond;
                res->Add(MakeRunningServiceInfoLocked(r));
            }
        }

        List< AutoPtr<CServiceRecord> >::Iterator srit = mRestartingServices.Begin();
        for (; srit != mRestartingServices.End() && (res->GetSize(&size), size < maxNum); ++srit) {
            AutoPtr<CServiceRecord> r = *srit;
            AutoPtr<IActivityManagerRunningServiceInfo> info =
                    MakeRunningServiceInfoLocked(r);
            info->SetRestarting(r->mNextRestartTime);
            res->Add(info);
        }
    }
    else {
        Int32 userId = UserHandle::GetUserId(uid);
        IComponentNameCServiceRecordHashMap::Iterator it;
        IComponentNameCServiceRecordHashMap& alls = GetServices(userId);
        Int32 size;
        for (it = alls.Begin(); it != alls.End() && (res->GetSize(&size), size < maxNum); ++it) {
            AutoPtr<CServiceRecord> r = it->mSecond;
            res->Add(MakeRunningServiceInfoLocked(r));
        }

        List< AutoPtr<CServiceRecord> >::Iterator srit = mRestartingServices.Begin();
        for (; srit != mRestartingServices.End() && (res->GetSize(&size), size < maxNum); ++srit) {
            AutoPtr<CServiceRecord> r = *srit;
            if (r->mUserId == userId) {
                AutoPtr<IActivityManagerRunningServiceInfo> info =
                        MakeRunningServiceInfoLocked(r);
                info->SetRestarting(r->mNextRestartTime);
                res->Add(info);
            }
        }
    }
//     } finally {
    Binder::RestoreCallingIdentity(ident);
//     }

    return res;
}

ECode ActiveServices::GetRunningServiceControlPanelLocked(
    /* [in] */ IComponentName* name,
    /* [out] */ IPendingIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    Int32 uid = Binder::GetCallingUid();
    Int32 userId = UserHandle::GetUserId(uid);
    AutoPtr<CServiceRecord> r = GetServiceByName(name, userId);
    if (r != NULL) {
        CServiceRecord::ConnectionHashMap::Iterator it;
        for (it = r->mConnections.Begin(); it != r->mConnections.End(); ++it) {
            AutoPtr<ConnectionRecordList> conn = it->mSecond;
            ConnectionRecordListIterator cit;
            for (cit = conn->Begin(); cit != conn->End(); ++cit) {
                if ((*cit)->mClientIntent != NULL) {
                    *intent = (*cit)->mClientIntent;
                    REFCOUNT_ADD(*intent);
                    return NOERROR;
                }
            }
        }
    }
    *intent = NULL;
    return NOERROR;
}

ECode ActiveServices::ServiceTimeout(
    /* [in] */ ProcessRecord* proc)
{
    String anrMessage;

    {
        AutoLock lock(mAm);
        if (proc->mExecutingServices.IsEmpty() || proc->mThread == NULL) {
            return NOERROR;
        }
        Int64 maxTime = SystemClock::GetUptimeMillis() -
            (proc->mExecServicesFg ? SERVICE_TIMEOUT : SERVICE_BACKGROUND_TIMEOUT);
        AutoPtr<CServiceRecord> timeout;
        Int64 nextTime = 0;
        HashSet< AutoPtr<CServiceRecord> >::Iterator it;
        for (it = proc->mExecutingServices.Begin(); it != proc->mExecutingServices.End(); ++it) {
            AutoPtr<CServiceRecord> sr = *it;
            if (sr->mExecutingStart < maxTime) {
                timeout = sr;
                break;
            }
            if (sr->mExecutingStart > nextTime) {
                nextTime = sr->mExecutingStart;
            }
        }
        List< AutoPtr<ProcessRecord> >::Iterator pIt = Find(mAm->mLruProcesses.Begin()
                , mAm->mLruProcesses.End(), AutoPtr<ProcessRecord>(proc));
        if (timeout != NULL && pIt != mAm->mLruProcesses.End()) {
            Slogger::W(TAG, "Timeout executing service: %s", TO_CSTR(timeout));
            anrMessage = String("Executing service ") + timeout->mShortName;
        }
        else {
            AutoPtr<IMessage> msg;
            mAm->mHandler->ObtainMessage(CActivityManagerService::SERVICE_TIMEOUT_MSG, (IMessage**)&msg);
            msg->SetObj(proc->Probe(EIID_IInterface));
            Boolean result;
            mAm->mHandler->SendMessageAtTime(msg, proc->mExecServicesFg
                ? (nextTime + SERVICE_TIMEOUT) : (nextTime + SERVICE_BACKGROUND_TIMEOUT), &result);
        }
    }

    if (!anrMessage.IsNull()) {
        mAm->AppNotResponding(proc, NULL, NULL, FALSE, anrMessage);
    }
    return NOERROR;
}

void ActiveServices::ScheduleServiceTimeoutLocked(
    /* [in] */ ProcessRecord* proc)
{
    if (proc->mExecutingServices.IsEmpty() || proc->mThread == NULL) {
        return;
    }
    Int64 now = SystemClock::GetUptimeMillis();
    AutoPtr<IMessage> msg;
    mAm->mHandler->ObtainMessage(CActivityManagerService::SERVICE_TIMEOUT_MSG,
        (IMessage**)&msg);
    msg->SetObj(proc->Probe(EIID_IInterface));
    Boolean result;
    mAm->mHandler->SendMessageAtTime(msg, proc->mExecServicesFg
        ? (now + SERVICE_TIMEOUT) : (now + SERVICE_BACKGROUND_TIMEOUT), &result);
}

/**
 * Prints a list of ServiceRecords (dumpsys activity services)
 */
void ActiveServices::DumpServicesLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll,
    /* [in] */ Boolean dumpClient,
    /* [in] */ const String& dumpPackage)
{
    Boolean needSep = FALSE;
    Boolean printedAnything = FALSE;

    AutoPtr<CActivityManagerService::ItemMatcher> matcher = new CActivityManagerService::ItemMatcher();
    matcher->Build(args, opti);

    pw->Println(String("ACTIVITY MANAGER SERVICES (dumpsys activity services)"));
//     try {
        AutoPtr< ArrayOf<Int32> > users = mAm->GetUsersLocked();
        for (Int32 i = 0; i < users->GetLength(); ++i) {
            Int32 user = (*users)[i];
            AutoPtr<ServiceMap> smap = GetServiceMap(user);
            Boolean printed = FALSE;
            IComponentNameCServiceRecordHashMap::Iterator it;
            if (smap->mServicesByName.IsEmpty() == FALSE) {
                Int64 nowReal = SystemClock::GetElapsedRealtime();
                needSep = FALSE;
                for (it = smap->mServicesByName.Begin(); it != smap->mServicesByName.End(); ++it) {
                    AutoPtr<CServiceRecord> r = it->mSecond;
                    if (!matcher->Match(r->Probe(EIID_IInterface), r->mName)) {
                        continue;
                    }
                    String pkgName;
                    IPackageItemInfo::Probe(r->mAppInfo)->GetPackageName(&pkgName);
                    if (dumpPackage != NULL && !dumpPackage.Equals(pkgName)) {
                        continue;
                    }
                    if (!printed) {
                        if (printedAnything) {
                            pw->Println();
                        }
                        StringBuilder sb("  User ");
                        sb += user;
                        sb += " active services:";
                        pw->Println(sb.ToString());
                        printed = TRUE;
                    }
                    printedAnything = TRUE;
                    if (needSep) {
                        pw->Println();
                    }
                    pw->Print(String("  * "));
                    pw->Println(r->Probe(EIID_IInterface));
                    if (dumpAll) {
                        r->Dump(pw, String("    "));
                        needSep = TRUE;
                    }
                    else {
                        pw->Print(String("    app="));
                        pw->Println(r->mApp->ToString());
                        pw->Print(String("    created="));
                        TimeUtils::FormatDuration(r->mCreateTime, nowReal, pw);
                        pw->Print(String(" started="));
                        pw->Print(r->mStartRequested);
                        pw->Print(String(" connections="));
                        pw->Println((Int32)r->mConnections.GetSize());
                        if (r->mConnections.IsEmpty() == FALSE) {
                            pw->Println(String("    Connections:"));
                            HashMap< AutoPtr<IBinder>, AutoPtr<ConnectionRecordList> >::Iterator connIt;
                            for (connIt = r->mConnections.Begin(); connIt != r->mConnections.End(); ++connIt) {
                                AutoPtr<ConnectionRecordList> clist = connIt->mSecond;
                                ConnectionRecordListIterator cit;
                                for (cit = clist->Begin(); cit != clist->End(); ++cit) {
                                    AutoPtr<ConnectionRecord> conn = *cit;
                                    pw->Print(String("      "));
                                    AutoPtr<IIntent> intent;
                                    conn->mBinding->mIntent->mIntent->GetIntent((IIntent**)&intent);
                                    String intentString;
                                    intent->ToShortString(FALSE, FALSE, FALSE, FALSE, &intentString);
                                    pw->Print(intentString);
                                    pw->Print(String(" -> "));
                                    AutoPtr<ProcessRecord> proc = conn->mBinding->mClient;
                                    pw->Println(proc != NULL ? proc->ToShortString() : String("NULL"));
                                }
                            }
                        }
                    }
                    if (dumpClient && r->mApp != NULL && r->mApp->mThread != NULL) {
                        pw->Println(String("    Client:"));
                        IFlushable::Probe(pw)->Flush();
                        // try {
                        AutoPtr<ITransferPipe> tp;
                        CTransferPipe::New((ITransferPipe**)&tp);
                            // try {
                        AutoPtr<IParcelFileDescriptor> writeFd;
                        tp->GetWriteFd((IParcelFileDescriptor**)&writeFd);
                        AutoPtr<IFileDescriptor> fileDes;
                        writeFd->GetFileDescriptor((IFileDescriptor**)&fileDes);
                        r->mApp->mThread->DumpService(fileDes, r, args);
                        tp->SetBufferPrefix(String("      "));
                        // Short timeout, since blocking here can
                        // deadlock with the application.
                        tp->Go(fd, 2000);
                            // } finally {
                        tp->Kill();
                        //     }
                        // } catch (IOException e) {
                        //     pw.println("      Failure while dumping the service: " + e);
                        // } catch (RemoteException e) {
                        //     pw.println("      Got a RemoteException while dumping the service");
                        // }
                        needSep = TRUE;
                    }
                }
                needSep |= printed;
            }
            printed = FALSE;
            List<AutoPtr<CServiceRecord> >::Iterator srit = smap->mDelayedStartList.Begin();
            for (; srit != smap->mDelayedStartList.End(); ++srit) {
                AutoPtr<CServiceRecord> r = *srit;
                if (!matcher->Match(r->Probe(EIID_IInterface), r->mName)) {
                    continue;
                }
                String packageName;
                IPackageItemInfo::Probe(r->mAppInfo)->GetPackageName(&packageName);
                if (dumpPackage != NULL && !dumpPackage.Equals(packageName)) {
                    continue;
                }
                if (!printed) {
                    if (printedAnything) {
                        pw->Println();
                    }
                    pw->Print(String("  User "));
                    pw->Print(user);
                    pw->Println(String(" delayed start services:"));
                    printed = TRUE;
                }
                printedAnything = TRUE;
                pw->Print(String("  * Delayed start "));
                pw->Println(r->ToString());
            }
            printed = FALSE;
            for (srit = smap->mStartingBackground.Begin(); srit != smap->mStartingBackground.End(); ++srit) {
                AutoPtr<CServiceRecord> r = *srit;
                if (!matcher->Match(r->Probe(EIID_IInterface), r->mName)) {
                    continue;
                }
                String packageName;
                IPackageItemInfo::Probe(r->mAppInfo)->GetPackageName(&packageName);
                if (dumpPackage != NULL && !dumpPackage.Equals(packageName)) {
                    continue;
                }
                if (!printed) {
                    if (printedAnything) {
                        pw->Println();
                    }
                    pw->Print(String("  User "));
                    pw->Print(user);
                    pw->Println(String(" starting in background:"));
                    printed = TRUE;
                }
                printedAnything = TRUE;
                pw->Print(String("  * Starting bg "));
                pw->Println(r->ToString());
            }
        }
    // } catch (Exception e) {
    //     Slog.w(TAG, "Exception in dumpServicesLocked: " + e);
    // }

    if (mPendingServices.IsEmpty() == FALSE) {
        Boolean printed = FALSE;
        List< AutoPtr<CServiceRecord> >::Iterator it;
        for (it = mPendingServices.Begin(); it != mPendingServices.End(); ++it) {
            AutoPtr<CServiceRecord> r = *it;
            if (!matcher->Match(r->Probe(EIID_IInterface), r->mName)) {
                continue;
            }
            String pkgName;
            IPackageItemInfo::Probe(r->mAppInfo)->GetPackageName(&pkgName);
            if (dumpPackage != NULL && !dumpPackage.Equals(pkgName)) {
                continue;
            }
            printedAnything = TRUE;
            if (!printed) {
                if (needSep) pw->Println();
                needSep = TRUE;
                pw->Println(String("  Pending services:"));
                printed = TRUE;
            }
            pw->Print(String("  * Pending "));
            pw->Println(r->ToString());
            r->Dump(pw, String("    "));
        }
        needSep = TRUE;
    }

    if (!mRestartingServices.IsEmpty()) {
        Boolean printed = FALSE;
        List< AutoPtr<CServiceRecord> >::Iterator it;
        for (it = mRestartingServices.Begin(); it != mRestartingServices.End(); ++it) {
            AutoPtr<CServiceRecord> r = *it;
            if (!matcher->Match(r->Probe(EIID_IInterface), r->mName)) {
                continue;
            }
            String pkgName;
            IPackageItemInfo::Probe(r->mAppInfo)->GetPackageName(&pkgName);
            if (dumpPackage != NULL && !dumpPackage.Equals(pkgName)) {
                continue;
            }
            printedAnything = TRUE;
            if (!printed) {
                if (needSep) pw->Println(String(" "));
                needSep = TRUE;
                pw->Println(String("  Restarting services:"));
                printed = TRUE;
            }
            pw->Print(String("  * Restarting "));
            pw->Println(r->ToString());
            r->Dump(pw, String("    "));
        }
        needSep = TRUE;
    }

    if (mDestroyingServices.IsEmpty() == FALSE) {
        Boolean printed = FALSE;
        List< AutoPtr<CServiceRecord> >::Iterator it;
        for (it = mDestroyingServices.Begin(); it != mDestroyingServices.End(); ++it) {
            AutoPtr<CServiceRecord> r = *it;
            if (!matcher->Match(r->Probe(EIID_IInterface), r->mName)) {
                continue;
            }
            String pkgName;
            IPackageItemInfo::Probe(r->mAppInfo)->GetPackageName(&pkgName);
            if (dumpPackage != NULL && !dumpPackage.Equals(pkgName)) {
                continue;
            }
            printedAnything = TRUE;
            if (!printed) {
                if (needSep) pw->Println();
                needSep = TRUE;
                pw->Println(String("  Destroying services:"));
                printed = TRUE;
            }
            pw->Print(String("  * Destroy "));
            pw->Println(r->ToString());
            r->Dump(pw, String("    "));
        }
        needSep = TRUE;
    }

    if (dumpAll) {
        Boolean printed = FALSE;
        HashMap< AutoPtr<IBinder>, AutoPtr<ConnectionRecordList> >::Iterator it;
        for (it = mServiceConnections.Begin(); it != mServiceConnections.End(); ++it) {
            AutoPtr<ConnectionRecordList> r = it->mSecond;
            ConnectionRecordListIterator rit;
            for (rit = r->Begin(); rit != r->End(); ++rit) {
                AutoPtr<ConnectionRecord> cr = *rit;
                if (!matcher->Match(cr->mBinding->mService->Probe(EIID_IInterface), cr->mBinding->mService->mName)) {
                    continue;
                }
                String pName;
                if (cr->mBinding->mClient != NULL) {
                    IPackageItemInfo::Probe(cr->mBinding->mClient->mInfo)->GetPackageName(&pName);
                }
                if (dumpPackage != NULL && !dumpPackage.Equals(pName)) {
                    continue;
                }
                printedAnything = TRUE;
                if (!printed) {
                    if (needSep) pw->Println();
                    needSep = TRUE;
                    pw->Println(String("  Connection bindings to services:"));
                    printed = TRUE;
                }
                pw->Print(String("  * "));
                pw->Println(cr->ToString());
                cr->Dump(pw, String("    "));
            }
        }
    }

    if (!printedAnything) {
        pw->Println(String("  (nothing)"));
    }
}

Boolean ActiveServices::DumpService(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll)
{
    List<AutoPtr<CServiceRecord> > services;

    {
        AutoLock lock(mAm);
        AutoPtr< ArrayOf<Int32> > users = mAm->GetUsersLocked();
        if (String("all").Equals(name)) {
            for (Int32 i = 0; i < users->GetLength(); ++i) {
                Int32 user = (*users)[i];
                AutoPtr<ServiceMap> smap;
                HashMap<Int32, AutoPtr<ServiceMap> >::Iterator find = mServiceMap.Find(user);
                if (find != mServiceMap.End()) {
                    smap = find->mSecond;
                }
                if (smap == NULL) {
                    continue;
                }
                IComponentNameCServiceRecordHashMap& alls = smap->mServicesByName;
                IComponentNameCServiceRecordHashMap::Iterator it;
                for (it = alls.Begin(); it != alls.End(); ++it) {
                    AutoPtr<CServiceRecord> r1 = it->mSecond;
                    services.PushBack(r1);
                }
            }
        }
        else {
            AutoPtr<IComponentName> componentName;
            if (name != NULL) {
                AutoPtr<IComponentNameHelper> cnHelper;
                CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&cnHelper);
                cnHelper->UnflattenFromString(name, (IComponentName**)&componentName);
            }
            Int32 objectId = 0;
            String lname = name;
            if (componentName == NULL) {
                // Not a '/' separated full component name; maybe an object ID?
                // try {
                    objectId = StringUtils::ParseInt32(name, 16);
                    lname = String(NULL);
                    componentName = NULL;
                // } catch (RuntimeException e) {
                // }
            }

            for (Int32 i = 0; i < users->GetLength(); ++i) {
                Int32 user = (*users)[i];
                AutoPtr<ServiceMap> smap;
                HashMap<Int32, AutoPtr<ServiceMap> >::Iterator find = mServiceMap.Find(user);
                if (find != mServiceMap.End()) {
                    smap = find->mSecond;
                }
                if (smap == NULL) {
                    continue;
                }
                IComponentNameCServiceRecordHashMap& alls = smap->mServicesByName;
                IComponentNameCServiceRecordHashMap::Iterator it;
                for (it = alls.Begin(); it != alls.End(); ++it) {
                    AutoPtr<CServiceRecord> r1 = it->mSecond;
                    if (componentName != NULL) {
                        Boolean equal = FALSE;
                        IObject::Probe(r1->mName)->Equals(componentName, &equal);
                        if (equal) {
                            services.PushBack(r1);
                        }
                    }
                    else if (lname != NULL) {
                        String nString;
                        r1->mName->FlattenToString(&nString);
                        if (nString.Contains(lname)) {
                            services.PushBack(r1);
                        }
                    }
                    else if ((Int32)r1.Get() == objectId) {
                       services.PushBack(r1);
                    }
                }
            }
        }
    }

    if (services.IsEmpty()) {
        return FALSE;
    }

    Boolean needSep = FALSE;
    List<AutoPtr<CServiceRecord> >::Iterator it;
    for (it = services.Begin(); it != services.End(); ++it) {
        if (needSep) {
            pw->Println();
        }
        needSep = TRUE;
        DumpService(String(""), fd, pw, *it, args, dumpAll);
    }
    return TRUE;
}

void ActiveServices::DumpService(
    /* [in] */ const String& prefix,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ CServiceRecord* r,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Boolean dumpAll)
{
    String innerPrefix = prefix + "  ";
    {
        AutoLock lock(mAm);
        pw->Print(prefix);
        pw->Print(String("SERVICE "));
        pw->Print(r->mShortName);
        pw->Print(String(" "));
        pw->Print((Int32)r);
        pw->Print(String(" pid="));
        if (r->mApp != NULL) pw->Println(r->mApp->mPid);
        else pw->Println(String("(not running)"));
        if (dumpAll) {
            r->Dump(pw, innerPrefix);
        }
    }
    if (r->mApp != NULL && r->mApp->mThread != NULL) {
        pw->Print(prefix); pw->Println(String("  Client:"));
        IFlushable::Probe(pw)->Flush();
//         try {
            AutoPtr<ITransferPipe> tp;
            CTransferPipe::New((ITransferPipe**)&tp);
//             try {
            AutoPtr<IParcelFileDescriptor> writeFd;
            tp->GetWriteFd((IParcelFileDescriptor**)&writeFd);
            AutoPtr<IFileDescriptor> fileDes;
            writeFd->GetFileDescriptor((IFileDescriptor**)&fileDes);
            r->mApp->mThread->DumpService(fileDes, r, args);
            tp->SetBufferPrefix(prefix + "    ");
            tp->Go(fd);
//             } finally {
            tp->Kill();
//             }
//         } catch (IOException e) {
//             pw->Println(prefix + "    Failure while dumping the service: " + e);
//         } catch (RemoteException e) {
//             pw->Println(prefix + "    Got a RemoteException while dumping the service");
//         }
    }
}

}
}
}
}
