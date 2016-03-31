
#include "elastos/droid/server/am/BroadcastQueue.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::EIID_IResolveInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

static Boolean IsLowRamDeviceStatic()
{
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    Boolean result;
    amHelper->IsLowRamDeviceStatic(&result);
    return result;
}

const String BroadcastQueue::TAG("BroadcastQueue");
const String BroadcastQueue::TAG_MU("BroadcastQueueMU");
const Boolean BroadcastQueue::DEBUG_BROADCAST = FALSE;
const Boolean BroadcastQueue::DEBUG_BROADCAST_LIGHT = FALSE;
const Boolean BroadcastQueue::DEBUG_MU = FALSE;
const Int32 BroadcastQueue::MAX_BROADCAST_HISTORY = IsLowRamDeviceStatic() ? 10 : 50;
const Int32 BroadcastQueue::MAX_BROADCAST_SUMMARY_HISTORY = IsLowRamDeviceStatic() ? 25 : 300;
const Int32 BroadcastQueue::BROADCAST_INTENT_MSG = 200;//CActivityManagerService.FIRST_BROADCAST_QUEUE_MSG;
const Int32 BroadcastQueue::BROADCAST_TIMEOUT_MSG = 201;//ActivityManagerService.FIRST_BROADCAST_QUEUE_MSG + 1;
AutoPtr<List<String> > BroadcastQueue::mQuickbootWhiteList;

ECode BroadcastQueue::BroadcastHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case BROADCAST_INTENT_MSG:
            if (DEBUG_BROADCAST)
                Slogger::V(TAG, "Received BROADCAST_INTENT_MSG");
            mHost->ProcessNextBroadcast(TRUE);
            break;
        case BROADCAST_TIMEOUT_MSG: {
            AutoLock lock(mHost->mService);
            mHost->BroadcastTimeoutLocked(TRUE);
            break;
        }
    }

    return NOERROR;
}

BroadcastQueue::AppNotResponding::AppNotResponding(
    /* [in] */ ProcessRecord* app,
    /* [in] */ const String& annotation,
    /* [in] */ BroadcastQueue* host)
    : mApp(app)
    , mAnnotation(annotation)
    , mHost(host)
{}

ECode BroadcastQueue::AppNotResponding::Run()
{
    return mHost->mService->AppNotResponding(mApp, NULL, NULL, FALSE, mAnnotation);
}

BroadcastQueue::BroadcastQueue(
    /* [in] */ CActivityManagerService* service,
    /* [in] */ IHandler* handler,
    /* [in] */ const String& name,
    /* [in] */ Int64 timeoutPeriod,
    /* [in] */ Boolean allowDelayBehindServices)
    : mService(service)
    , mQueueName(name)
    , mTimeoutPeriod(timeoutPeriod)
    , mDelayBehindServices(allowDelayBehindServices)
    , mBroadcastsScheduled(FALSE)
    , mPendingBroadcastTimeoutMessage(FALSE)
    , mPendingBroadcastRecvIndex(0)
{
    mBroadcastHistory = ArrayOf<BroadcastRecord*>::Alloc(MAX_BROADCAST_HISTORY);
    mBroadcastSummaryHistory = ArrayOf<IIntent*>::Alloc(MAX_BROADCAST_SUMMARY_HISTORY);

    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);
    mHandler = new BroadcastHandler(looper, this);
}

Boolean BroadcastQueue::IsPendingBroadcastProcessLocked(
    /* [in] */ Int32 pid)
{
    return mPendingBroadcast != NULL && mPendingBroadcast->mCurApp->mPid == pid;
}

void BroadcastQueue::EnqueueParallelBroadcastLocked(
    /* [in] */ BroadcastRecord* r)
{
    mParallelBroadcasts.PushBack(r);
}

void BroadcastQueue::EnqueueOrderedBroadcastLocked(
    /* [in] */ BroadcastRecord* r)
{
    mOrderedBroadcasts.PushBack(r);
}

Boolean BroadcastQueue::ReplaceParallelBroadcastLocked(
    /* [in] */ BroadcastRecord* r)
{
    List< AutoPtr<BroadcastRecord> >::ReverseIterator it = mParallelBroadcasts.RBegin();
    for(; it != mParallelBroadcasts.REnd(); ++it) {
        Boolean b;
        r->mIntent->FilterEquals((*it)->mIntent, &b);
        if (b) {
            if (DEBUG_BROADCAST) {
                Slogger::V(TAG, "***** DROPPING PARALLEL [%s]:%s", mQueueName.string(), TO_CSTR(r->mIntent));
            }
            *it = r;
            return TRUE;
        }
    }
    return FALSE;
}

Boolean BroadcastQueue::ReplaceOrderedBroadcastLocked(
    /* [in] */ BroadcastRecord* r)
{
    List< AutoPtr<BroadcastRecord> >::ReverseIterator it = mOrderedBroadcasts.RBegin();
    for(; it != mOrderedBroadcasts.REnd(); ++it) {
        Boolean b;
        r->mIntent->FilterEquals((*it)->mIntent, &b);
        if (b) {
            if (DEBUG_BROADCAST) {
                String str;
                IObject::Probe(r->mIntent)->ToString(&str);
                Slogger::V(TAG, "***** DROPPING ORDERED [%s]:%s", mQueueName.string(), str.string());
            }
            *it = r;
            return TRUE;
        }
    }
    return FALSE;
}

ECode BroadcastQueue::ProcessCurBroadcastLocked(
    /* [in] */ BroadcastRecord* r,
    /* [in] */ ProcessRecord* app)
{
    if (DEBUG_BROADCAST) {
        String rs;
        if (r) {
            rs = r->ToString();
        }

        Slogger::V(TAG, "Process cur broadcast %s for app %s", rs.string(), app->ToString().string());
    }

    if (app->mThread == NULL) {
        // throw new RemoteException();
        return E_REMOTE_EXCEPTION;
    }
    r->mReceiver = IBinder::Probe(app->mThread.Get());
    r->mCurApp = app;
    app->mCurReceiver = r;
    app->ForceProcessStateUpTo(IActivityManager::PROCESS_STATE_RECEIVER);
    mService->UpdateLruProcessLocked(app, FALSE, NULL);
    mService->UpdateOomAdjLocked();

    // Tell the application to launch this receiver.
    r->mIntent->SetComponent(r->mCurComponent);

    Boolean started = FALSE;
    // try {
    if (DEBUG_BROADCAST_LIGHT) {
        String str;
        IObject::Probe(r->mCurComponent)->ToString(&str);
        Slogger::V(TAG, "Delivering to component %s: %s", str.string(), TO_CSTR(r));
    }
    AutoPtr<IComponentName> name;
    r->mIntent->GetComponent((IComponentName**)&name);
    String pkgName;
    name->GetPackageName(&pkgName);
    mService->EnsurePackageDexOpt(pkgName);
    AutoPtr<IIntent> intent;
    CIntent::New(r->mIntent, (IIntent**)&intent);
    AutoPtr<IApplicationInfo> info;
    IComponentInfo::Probe(r->mCurReceiver)->GetApplicationInfo((IApplicationInfo**)&info);
    if (FAILED(app->mThread->ScheduleReceiver(intent, r->mCurReceiver,
            mService->CompatibilityInfoForPackageLocked(info), r->mResultCode,
            r->mResultData, r->mResultExtras, r->mOrdered, r->mUserId, app->mRepProcState))) {
        goto failed;
    }
    if (DEBUG_BROADCAST)
        Slogger::V(TAG, "Process cur broadcast %s DELIVERED for app ",
            TO_CSTR(r), app->ToString().string());
    started = TRUE;
    // } finally {
failed:
    if (!started) {
        if (DEBUG_BROADCAST)
            Slogger::V(TAG, "Process cur broadcast %s: NOT STARTED!",
                TO_CSTR(r));
        r->mReceiver = NULL;
        r->mCurApp = NULL;
        app->mCurReceiver = NULL;
    }
    // }
    return NOERROR;
}

ECode BroadcastQueue::SendPendingBroadcastsLocked(
    /* [in] */ ProcessRecord* app,
    /* [out] */ Boolean* result)
{
    AutoPtr<BroadcastRecord> br = mPendingBroadcast;
    if (br != NULL && br->mCurApp->mPid == app->mPid) {
        // try {
        mPendingBroadcast = NULL;
        ECode ec = ProcessCurBroadcastLocked(br, app);
        if(FAILED(ec)) {
            String str;
            br->mCurComponent->FlattenToShortString(&str);
            Slogger::W(TAG, "Exception in new application when starting receiver %s 0x%08x", str.string(), ec);
            LogBroadcastReceiverDiscardLocked(br);
            FinishReceiverLocked(br, br->mResultCode, br->mResultData,
                    br->mResultExtras, br->mResultAbort, FALSE);
            ScheduleBroadcastsLocked();
            // We need to reset the state if we failed to start the receiver.
            br->mState = BroadcastRecord::IDLE;
            // throw new RuntimeException(e.getMessage());
            *result = FALSE;
            return E_RUNTIME_EXCEPTION;
        }
    // }catch (Exception e) {
    // }
    }
    *result = TRUE;
    return NOERROR;
}

void BroadcastQueue::SkipPendingBroadcastLocked(
    /* [in] */ Int32 pid)
{
    AutoPtr<BroadcastRecord> br = mPendingBroadcast;
    if (br != NULL && br->mCurApp->mPid == pid) {
        br->mState = BroadcastRecord::IDLE;
        br->mNextReceiver = mPendingBroadcastRecvIndex;
        mPendingBroadcast = NULL;
        ScheduleBroadcastsLocked();
    }
}

void BroadcastQueue::SkipCurrentReceiverLocked(
    /* [in] */ ProcessRecord* app)
{
    Boolean reschedule = FALSE;
    AutoPtr<BroadcastRecord> r = app->mCurReceiver;
    if (r != NULL && r->mQueue == this) {
        // The current broadcast is waiting for this app's receiver
        // to be finished.  Looks like that's not going to happen, so
        // let the broadcast continue.
        LogBroadcastReceiverDiscardLocked(r);
        FinishReceiverLocked(r, r->mResultCode, r->mResultData,
                r->mResultExtras, r->mResultAbort, FALSE);
        reschedule = TRUE;
    }

    r = mPendingBroadcast;
    if (r != NULL && r->mCurApp.Get() == app) {
        if (DEBUG_BROADCAST)
            Slogger::V(TAG, "[%s] skip & discard pending app %s", mQueueName.string(), TO_CSTR(r));
        LogBroadcastReceiverDiscardLocked(r);
        FinishReceiverLocked(r, r->mResultCode, r->mResultData,
                r->mResultExtras, r->mResultAbort, FALSE);
        reschedule = TRUE;
    }
    if (reschedule) {
        ScheduleBroadcastsLocked();
    }
}

void BroadcastQueue::ScheduleBroadcastsLocked()
{
    if (DEBUG_BROADCAST)
        Slogger::V(TAG, "Schedule broadcasts [%s]: current=%d",
            mQueueName.string(), mBroadcastsScheduled);

    if (mBroadcastsScheduled) {
        return;
    }

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(BROADCAST_INTENT_MSG, TO_IINTERFACE(this), (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);

    mBroadcastsScheduled = TRUE;
}

AutoPtr<BroadcastRecord> BroadcastQueue::GetMatchingOrderedReceiver(
    /* [in] */ IBinder* receiver)
{
    if (!mOrderedBroadcasts.IsEmpty()) {
        AutoPtr<BroadcastRecord> r = *mOrderedBroadcasts.Begin();
        if (r != NULL && r->mReceiver.Get() == receiver) {
            return r;
        }
    }
    return NULL;
}

Boolean BroadcastQueue::FinishReceiverLocked(
    /* [in] */ BroadcastRecord* r,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* resultExtras,
    /* [in] */ Boolean resultAbort,
    /* [in] */ Boolean waitForServices)
{
    Int32 state = r->mState;
    IActivityInfo* receiver = r->mCurReceiver;
    r->mState = BroadcastRecord::IDLE;
    if (state == BroadcastRecord::IDLE) {
        Slogger::W(TAG, "finishReceiver [%s] called but state is IDLE", mQueueName.string());
    }
    r->mReceiver = NULL;
    r->mIntent->SetComponent(NULL);
    if (r->mCurApp != NULL && r->mCurApp->mCurReceiver.Get() == r) {
        r->mCurApp->mCurReceiver = NULL;
    }
    if (r->mCurFilter != NULL) {
        r->mCurFilter->mReceiverList->mCurBroadcast = NULL;
    }
    r->mCurFilter = NULL;
    r->mCurReceiver = NULL;
    r->mCurApp = NULL;
    mPendingBroadcast = NULL;

    r->mResultCode = resultCode;
    r->mResultData = resultData;
    r->mResultExtras = resultExtras;
    Int32 flags;
    if (resultAbort && (r->mIntent->GetFlags(&flags), flags & IIntent::FLAG_RECEIVER_NO_ABORT) == 0) {
        r->mResultAbort = resultAbort;
    }
    else {
        r->mResultAbort = FALSE;
    }

    if (waitForServices && r->mCurComponent != NULL && r->mQueue->mDelayBehindServices
            && r->mQueue->mOrderedBroadcasts.GetSize() > 0
            && r->mQueue->mOrderedBroadcasts.Begin()->Get() == r) {
        AutoPtr<IActivityInfo> nextReceiver;
        Int32 size;
        r->mReceivers->GetSize(&size);
        if (r->mNextReceiver < size) {
            AutoPtr<IInterface> obj;
            r->mReceivers->Get(r->mNextReceiver, (IInterface**)&obj);
            nextReceiver = IActivityInfo::Probe(obj) ? IActivityInfo::Probe(obj) : NULL;
        }
        else {
            nextReceiver = NULL;
        }
        // Don't do this if the next receive is in the same process as the current one.
        Int32 uid, nextUid;
        String processName, nextProcessName;
        if (receiver != NULL && nextReceiver != NULL) {
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(receiver)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            appInfo->GetUid(&uid);
            IComponentInfo::Probe(receiver)->GetProcessName(&processName);
            appInfo = NULL;
            IComponentInfo::Probe(nextReceiver)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            appInfo->GetUid(&nextUid);
            IComponentInfo::Probe(nextReceiver)->GetProcessName(&nextProcessName);
        }
        if (receiver == NULL || nextReceiver == NULL || uid != nextUid
            || !processName.Equals(nextProcessName)) {
            // In this case, we are ready to process the next receiver for the current broadcast,
            // but are on a queue that would like to wait for services to finish before moving
            // on.  If there are background services currently starting, then we will go into a
            // special state where we hold off on continuing this broadcast until they are done.
            if (mService->mServices->HasBackgroundServices(r->mUserId)) {
                String str;
                r->mCurComponent->FlattenToShortString(&str);
                Slogger::I(CActivityManagerService::TAG, "Delay finish: %s", str.string());
                r->mState = BroadcastRecord::WAITING_SERVICES;
                return FALSE;
            }
        }
    }

    r->mCurComponent = NULL;

    // We will process the next receiver right now if this is finishing
    // an app receiver (which is always asynchronous) or after we have
    // come back from calling a receiver.
    return state == BroadcastRecord::APP_RECEIVE
            || state == BroadcastRecord::CALL_DONE_RECEIVE;
}

void BroadcastQueue::BackgroundServicesFinishedLocked(
    /* [in] */ Int32 userId)
{
    if (mOrderedBroadcasts.GetSize() > 0) {
        AutoPtr<BroadcastRecord> br = *mOrderedBroadcasts.Begin();
        if (br->mUserId == userId && br->mState == BroadcastRecord::WAITING_SERVICES) {
            Slogger::I(CActivityManagerService::TAG, "Resuming delayed broadcast");
            br->mCurComponent = NULL;
            br->mState = BroadcastRecord::IDLE;
            ProcessNextBroadcast(FALSE);
        }
    }
}

ECode BroadcastQueue::PerformReceiveLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ IIntentReceiver* receiver,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    // Send the intent to the receiver asynchronously using one-way binder calls.
    if (app != NULL) {
        if (app->mThread != NULL) {
            // If we have an app thread, do the call through that so it is
            // correctly ordered with other one-way calls.
            return app->mThread->ScheduleRegisteredReceiver(receiver, intent, resultCode,
                    data, extras, ordered, sticky, sendingUser, app->mRepProcState);
        }
        else {
            Slogger::E(TAG, "app.thread must not be null");
            return E_REMOTE_EXCEPTION;
        }
    }
    else {
        return receiver->PerformReceive(intent, resultCode, data, extras, ordered, sticky, sendingUser);
    }
}

void BroadcastQueue::DeliverToRegisteredReceiverLocked(
    /* [in] */ BroadcastRecord* r,
    /* [in] */ BroadcastFilter* filter,
    /* [in] */ Boolean ordered)
{
    // TODO: delete???
    if (filter->mReceiverList == NULL) {
        if (ordered) {
            r->mReceiver = NULL;
            r->mCurFilter = NULL;
            r->mState = BroadcastRecord::CALL_IN_RECEIVE;
        }
        return;
    }

    if (DEBUG_BROADCAST) {
        String rs, fs;
        if (r) rs = r->ToString();
        if (filter) fs = filter->ToString();
        Slogger::W(TAG, "DeliverToRegisteredReceiverLocked: \n >>> record:[%s]\n >>> filter:[%s]",
            rs.string(), fs.string());
    }

    Boolean skip = FALSE;
    if (!filter->mRequiredPermission.IsNull()) {
        Int32 perm = mService->CheckComponentPermission(filter->mRequiredPermission,
                r->mCallingPid, r->mCallingUid, -1, TRUE);
        if (perm != IPackageManager::PERMISSION_GRANTED) {
            String str;
            r->mIntent->ToString(&str);
            Slogger::W(TAG, "Permission Denial: broadcasting %s from %s (pid=%d, uid=%d)"
                " requires %s due to registered receiver x"
                , str.string(), r->mCallerPackage.string(),  r->mCallingPid, r->mCallingUid
                , filter->mRequiredPermission.string(), TO_CSTR(r));
            skip = TRUE;
        }
    }
    if (!skip && !r->mRequiredPermission.IsNull()) {
        Int32 perm = mService->CheckComponentPermission(r->mRequiredPermission,
                filter->mReceiverList->mPid, filter->mReceiverList->mUid, -1, TRUE);
        if (perm != IPackageManager::PERMISSION_GRANTED) {
            String str;
            r->mIntent->ToString(&str);
            Slogger::W(TAG, "Permission Denial: receiving %s to %s (pid=%d, uid=%d)"
                " requires %s due to sender %s (uid %d)", str.string(),
                filter->mReceiverList->mApp->ToString().string(), filter->mReceiverList->mPid,
                filter->mReceiverList->mUid, r->mRequiredPermission.string(),
                r->mCallerPackage.string(), r->mCallingUid);
            skip = TRUE;
        }
    }

    if (r->mAppOp != IAppOpsManager::OP_NONE) {
        Int32 mode;
        mService->mAppOpsService->NoteOperation(r->mAppOp,
                filter->mReceiverList->mUid, filter->mPackageName, &mode);
        if (mode != IAppOpsManager::MODE_ALLOWED) {
            if (DEBUG_BROADCAST)
                Slogger::V(TAG, "App op %d not allowed for broadcast to uid %d, pkg %s",
                    filter->mReceiverList->mUid, r->mAppOp, filter->mPackageName.string());
            skip = TRUE;
        }
    }
    if (!skip) {
        //assert(0 && TODO);
        // skip = !mService->mIntentFirewall->CheckBroadcast(r->mIntent, r->mCallingUid,
        //         r->mCallingPid, r->mResolvedType, filter->mReceiverList->mUid);
    }

    if (filter->mReceiverList->mApp == NULL || filter->mReceiverList->mApp->mCrashing) {
        Slogger::W(TAG, "Skipping deliver [%s] %s to %s: process crashing",
            mQueueName.string(), TO_CSTR(r), TO_CSTR(filter->mReceiverList));
        skip = TRUE;
    }

    if (!skip) {
        // If this is not being sent as an ordered broadcast, then we
        // don't want to touch the fields that keep track of the current
        // state of ordered broadcasts.
        if (ordered) {
            r->mReceiver = IBinder::Probe(filter->mReceiverList->mReceiver.Get());
            r->mCurFilter = filter;
            filter->mReceiverList->mCurBroadcast = r;
            r->mState = BroadcastRecord::CALL_IN_RECEIVE;
            if (filter->mReceiverList->mApp != NULL) {
                // Bump hosting application to no longer be in background
                // scheduling class.  Note that we can't do that if there
                // isn't an app...  but we can only be in that case for
                // things that directly call the IActivityManager API, which
                // are already core system stuff so don't matter for this.
                r->mCurApp = filter->mReceiverList->mApp;
                filter->mReceiverList->mApp->mCurReceiver = r;
                mService->UpdateOomAdjLocked(r->mCurApp);
            }
        }

        // try {
        if (DEBUG_BROADCAST_LIGHT) {
            Int32 seq;
            r->mIntent->GetInt32Extra(String("seq"), -1, &seq);
            Slogger::I(TAG, "Delivering to %s (seq=%d): %s", TO_CSTR(r), seq, TO_CSTR(r));
        }

        AutoPtr<IIntent> newIntent;
        CIntent::New(r->mIntent, (IIntent**)&newIntent);
        ECode ec = PerformReceiveLocked(filter->mReceiverList->mApp, filter->mReceiverList->mReceiver,
            newIntent, r->mResultCode, r->mResultData,
            r->mResultExtras, r->mOrdered, r->mInitialSticky, r->mUserId);
        if (FAILED(ec)) {
            Slogger::W(TAG, "Failure sending broadcast %s, ec=0x%08x", TO_CSTR(r->mIntent), ec);
            if (ordered) {
                r->mReceiver = NULL;
                r->mCurFilter = NULL;
                filter->mReceiverList->mCurBroadcast = NULL;
                if (filter->mReceiverList->mApp != NULL) {
                    filter->mReceiverList->mApp->mCurReceiver = NULL;
                }
            }
            return;
        }

        if (ordered) {
            r->mState = BroadcastRecord::CALL_DONE_RECEIVE;
        }
        return;
    }
}

AutoPtr<BroadcastRecord> BroadcastQueue::GetProcessingBroadcast()
{
    return mCurrentBroadcast;
}

ECode BroadcastQueue::ProcessNextBroadcast(
    /* [in] */ Boolean fromMsg)
{
    AutoLock lock(mService);
    AutoPtr<BroadcastRecord> r;
    if (DEBUG_BROADCAST) {
        Slogger::V(TAG, "processNextBroadcast [%s]: %d broadcasts, %d ordered broadcasts"
                , mQueueName.string(), mParallelBroadcasts.GetSize(), mOrderedBroadcasts.GetSize());
    }

    mService->UpdateCpuStats();

    if (fromMsg) {
        mBroadcastsScheduled = FALSE;
    }

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    // First, deliver any non-serialized broadcasts right away.
    while (!mParallelBroadcasts.IsEmpty()) {
        r = mParallelBroadcasts.GetFront();
        mParallelBroadcasts.PopFront();
        r->mDispatchTime = SystemClock::GetUptimeMillis();
        system->GetCurrentTimeMillis(&r->mDispatchClockTime);
        mCurrentBroadcast = r;
        Int32 N;
        r->mReceivers->GetSize(&N);
        if (DEBUG_BROADCAST_LIGHT) {
            Slogger::V(TAG, "Processing parallel broadcast [%s] %s", mQueueName.string(), TO_CSTR(r));
        }

        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> target;
            r->mReceivers->Get(i, (IInterface**)&target);
            if (DEBUG_BROADCAST) {
                String str;
                IObject::Probe(target)->ToString(&str);
                Slogger::V(TAG, "Delivering non-ordered on [%s] to registered %s: %s"
                        , mQueueName.string(), str.string(), TO_CSTR(r));
            }
            DeliverToRegisteredReceiverLocked(r, (BroadcastFilter*)IBroadcastFilter::Probe(target), FALSE);
        }
        AddBroadcastToHistoryLocked(r);
        mCurrentBroadcast = NULL;
        if (DEBUG_BROADCAST_LIGHT) {
            Slogger::V(TAG, "Done with parallel broadcast [%s] %s",
                mQueueName.string(), TO_CSTR(r));
        }
    }

    // Now take care of the next serialized one...

    // If we are waiting for a process to come up to handle the next
    // broadcast, then do nothing at this point.  Just in case, we
    // check that the process we're waiting for still exists.
    if (mPendingBroadcast != NULL) {
        if (DEBUG_BROADCAST_LIGHT) {
            Slogger::V(TAG, "processNextBroadcast [%s]: waiting for %s",
                mQueueName.string(), mPendingBroadcast->mCurApp->ToString().string());
        }

        Boolean isDead;
        {
            AutoLock lock(mService->mPidsSelfLockedLock);
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it =
                mService->mPidsSelfLocked.Find(mPendingBroadcast->mCurApp->mPid);
            AutoPtr<ProcessRecord> proc;
            if (it != mService->mPidsSelfLocked.End())
                proc = it->mSecond;
            isDead = proc == NULL || proc->mCrashing;
        }
        if (!isDead) {
            // It's still alive, so keep waiting
            return NOERROR;
        }
        else {
            Slogger::W(TAG, "pending app  [%s]%s died before responding to broadcast"
                    , mQueueName.string(), mPendingBroadcast->mCurApp->ToString().string());
            mPendingBroadcast->mState = BroadcastRecord::IDLE;
            mPendingBroadcast->mNextReceiver = mPendingBroadcastRecvIndex;
            mPendingBroadcast = NULL;
        }
    }

    Boolean looped = FALSE;

    do {
        if (mOrderedBroadcasts.IsEmpty()) {
            // No more broadcasts pending, so all done!
            mService->ScheduleAppGcsLocked();
            if (looped) {
                // If we had finished the last ordered broadcast, then
                // make sure all processes have correct oom and sched
                // adjustments.
                mService->UpdateOomAdjLocked();
            }
            return NOERROR;
        }
        r = mOrderedBroadcasts.GetFront();
        mCurrentBroadcast = r;
        assert(r != NULL);

        Boolean forceReceive = FALSE;

        // Ensure that even if something goes awry with the timeout
        // detection, we catch "hung" broadcasts here, discard them,
        // and continue to make progress.
        //
        // This is only done if the system is ready so that PRE_BOOT_COMPLETED
        // receivers don't get executed with timeouts. They're intended for
        // one time heavy lifting after system upgrades and can take
        // significant amounts of time.
        Int32 numReceivers = 0;
        if (r->mReceivers != NULL)
            r->mReceivers->GetSize(&numReceivers);
        if (mService->mProcessesReady && r->mDispatchTime > 0) {
            Int64 now = SystemClock::GetUptimeMillis();
            if ((numReceivers > 0) &&
                    (now > r->mDispatchTime + (2 * mTimeoutPeriod * numReceivers))) {
                String str;
                IObject::Probe(r->mIntent)->ToString(&str);
                Slogger::W(TAG, "Hung broadcast [%s] discarded after timeout failure: now=%lld"
                    " dispatchTime=%lld startTime=%lld intent=%s numReceivers=%d nextReceiver=%d state=%d"
                    , mQueueName.string(), now, r->mDispatchTime, r->mReceiverTime, str.string()
                    , numReceivers, r->mNextReceiver, r->mState);
                BroadcastTimeoutLocked(FALSE); // forcibly finish this broadcast
                forceReceive = TRUE;
                r->mState = BroadcastRecord::IDLE;
            }
        }

        if (r->mState != BroadcastRecord::IDLE) {
            if (DEBUG_BROADCAST) {
                Slogger::D(TAG, "processNextBroadcast(%s) called when not idle (state=%d)"
                        , mQueueName.string(), r->mState);
            }
            return NOERROR;
        }

        if (r->mReceivers == NULL || r->mNextReceiver >= numReceivers || r->mResultAbort || forceReceive) {
            // No more receivers for this broadcast!  Send the final
            // result if requested...
            if (r->mResultTo != NULL) {
                // try {
                if (DEBUG_BROADCAST) {
                    Int32 seq = -1;
                    r->mIntent->GetInt32Extra(String("seq"), -1, &seq);
                    String action;
                    r->mIntent->GetAction(&action);
                    Slogger::I(TAG, "Finishing broadcast [%s] %s seq=%d app=%s",
                        mQueueName.string(), action.string(), seq, TO_CSTR(r->mCallerApp));
                }
                AutoPtr<IIntent> newIntent;
                CIntent::New(r->mIntent, (IIntent**)&newIntent);
                ECode ec = PerformReceiveLocked(r->mCallerApp, r->mResultTo, newIntent, r->mResultCode,
                        r->mResultData, r->mResultExtras, FALSE, FALSE, r->mUserId);
                // Set this to NULL so that the reference
                // (local and remote) isn't kept in the mBroadcastHistory.
                r->mResultTo = NULL;
                if (FAILED(ec)) {
                    Slogger::W(TAG, "Failure [%s] sending broadcast result of %s, ec=0x%08x",
                        mQueueName.string(), TO_CSTR(r->mIntent), ec);
                }
            }

            if (DEBUG_BROADCAST) Slogger::V(TAG, "Cancelling BROADCAST_TIMEOUT_MSG");
            CancelBroadcastTimeoutLocked();

            if (DEBUG_BROADCAST_LIGHT)
                Slogger::V(TAG, "Finished with ordered broadcast %s", TO_CSTR(r));

            // ... and on to the next...
            AddBroadcastToHistoryLocked(r);
            mOrderedBroadcasts.PopFront();
            mCurrentBroadcast = NULL;
            r = NULL;
            looped = TRUE;
            continue;
        }
    } while (r == NULL);

    // Get the next receiver...
    Int32 recIdx = r->mNextReceiver++;

    // Keep track of when this receiver started, and make sure there
    // is a timeout message pending to kill it if need be.
    r->mReceiverTime = SystemClock::GetUptimeMillis();
    if (recIdx == 0) {
        r->mDispatchTime = r->mReceiverTime;
        system->GetCurrentTimeMillis(&r->mDispatchClockTime);
        if (DEBUG_BROADCAST_LIGHT) {
            Slogger::V(TAG, "Processing ordered broadcast [%s] %s", mQueueName.string(), TO_CSTR(r));
        }
    }
    if (!mPendingBroadcastTimeoutMessage) {
        Int64 timeoutTime = r->mReceiverTime + mTimeoutPeriod;
        if (DEBUG_BROADCAST) {
            Slogger::V(TAG, "Submitting BROADCAST_TIMEOUT_MSG [%s] for %s at %d",
                mQueueName.string(), TO_CSTR(r), timeoutTime);
        }
        SetBroadcastTimeoutLocked(timeoutTime);
    }

    AutoPtr<IInterface> nextReceiver;
    r->mReceivers->Get(recIdx, (IInterface**)&nextReceiver);
    IBroadcastFilter* bf = IBroadcastFilter::Probe(nextReceiver);
    if (bf != NULL) {
        // Simple case: this is a registered receiver who gets
        // a direct call.
        AutoPtr<BroadcastFilter> filter = (BroadcastFilter*)bf;
        if (DEBUG_BROADCAST)  {
            Slogger::V(TAG, "Delivering ordered [%s] to registered %s: %s",
                mQueueName.string(), TO_CSTR(filter), TO_CSTR(r));
        }
        DeliverToRegisteredReceiverLocked(r, filter, r->mOrdered);
        if (r->mReceiver == NULL || !r->mOrdered) {
            // The receiver has already finished, so schedule to
            // process the next one.
            if (DEBUG_BROADCAST) {
                Slogger::V(TAG, "Quick finishing [%s]: ordered=%d receiver=%s",
                    mQueueName.string(), r->mOrdered, TO_CSTR(r->mReceiver));
            }
            r->mState = BroadcastRecord::IDLE;
            ScheduleBroadcastsLocked();
        }
        return NOERROR;
    }

    // Hard case: need to instantiate the receiver, possibly
    // starting its application process to host it.

    AutoPtr<IResolveInfo> info = IResolveInfo::Probe(nextReceiver);
    AutoPtr<IActivityInfo> aInfo;
    info->GetActivityInfo((IActivityInfo**)&aInfo);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    String permission;
    aInfo->GetPermission(&permission);
    String processName;
    IComponentInfo::Probe(aInfo)->GetProcessName(&processName);
    String packageName;
    IPackageItemInfo::Probe(aInfo)->GetPackageName(&packageName);
    String name;
    IPackageItemInfo::Probe(aInfo)->GetName(&name);
    Int32 flags;
    aInfo->GetFlags(&flags);
    Boolean exported;
    IComponentInfo::Probe(aInfo)->GetExported(&exported);
    Int32 appUid;
    appInfo->GetUid(&appUid);
    String appPackageName;
    IPackageItemInfo::Probe(appInfo)->GetPackageName(&appPackageName);

    AutoPtr<IComponentName> component;
    CComponentName::New(appPackageName, name, (IComponentName**)&component);

    Boolean skip = FALSE;
    Int32 perm = mService->CheckComponentPermission(permission,
            r->mCallingPid, r->mCallingUid, appUid, exported);
    if (perm != IPackageManager::PERMISSION_GRANTED) {
        if (!exported) {
            String str;
            r->mIntent->ToString(&str);
            String str2;
            component->FlattenToShortString(&str2);
            Slogger::W(TAG, "Permission Denial: broadcasting %s from %s (pid=%d, uid=%d)"
                " is not exported from uid %d due to receiver %s", str.string(),
                r->mCallerPackage.string(), r->mCallingPid, r->mCallingUid, appUid, str2.string());
        }
        else {
            String str;
            r->mIntent->ToString(&str);
            String str2;
            component->FlattenToShortString(&str2);
            Slogger::W(TAG, "Permission Denial: broadcasting %s from %s (pid=%d, uid=%d)"
                " requires %s due to receiver ", str.string(), r->mCallerPackage.string(),
                r->mCallingPid, r->mCallingUid, permission.string(), str2.string());
        }
        skip = TRUE;
    }
    if (appUid != IProcess::SYSTEM_UID && !r->mRequiredPermission.IsNull()) {
        // try {
        if (FAILED(AppGlobals::GetPackageManager()->CheckPermission(r->mRequiredPermission, appPackageName, &perm))) {
            perm = IPackageManager::PERMISSION_DENIED;
        }
        // } catch (RemoteException e) {
        //     perm = IPackageManager::PERMISSION_DENIED;
        // }
        if (perm != IPackageManager::PERMISSION_GRANTED) {
            String str, str2;
            IObject::Probe(r->mIntent)->ToString(&str);
            component->FlattenToShortString(&str2);
            Slogger::W(TAG, "Permission Denial: receiving %s to %s requires %s due to sender %s (uid %d)",
                str.string(), str2.string(), r->mRequiredPermission.string(),
                r->mCallerPackage.string(), r->mCallingUid);
            skip = TRUE;
        }
    }
    if (r->mAppOp != IAppOpsManager::OP_NONE) {
        Int32 mode;
        mService->mAppOpsService->NoteOperation(r->mAppOp, appUid, appPackageName, &mode);
        if (mode != IAppOpsManager::MODE_ALLOWED) {
            if (DEBUG_BROADCAST)
                Slogger::V(TAG, "App op %d not allowed for broadcast to uid %d pkg %s",
                    r->mAppOp, appUid, appPackageName.string());
            skip = TRUE;
        }
    }
    if (!skip) {
        //assert(0 && TODO);
        // skip = !mService->mIntentFirewall->CheckBroadcast(r->mIntent, r->mCallingUid,
        //     r->mCallingPid, r->mResolvedType, appUid);
    }
    Boolean isSingleton = FALSE;
    if (FAILED(mService->IsSingleton(processName, appInfo, name, flags, &isSingleton))) {
        Slogger::W(TAG, "mService->IsSingleton failed");
        skip = TRUE;
    }
    if ((flags & IActivityInfo::FLAG_SINGLE_USER) != 0) {
        AutoPtr<IActivityManagerHelper> helper;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
        Int32 result;
        helper->CheckUidPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS, appUid, &result);
        if (result != IPackageManager::PERMISSION_GRANTED) {
            String str;
            component->FlattenToShortString(&str);
            Slogger::W(TAG, "Permission Denial: Receiver %s requests FLAG_SINGLE_USER,"
                " but app does not hold ", str.string());
            skip = TRUE;
        }
    }
    if (r->mCurApp != NULL && r->mCurApp->mCrashing) {
        // If the target process is crashing, just skip it.
        Slogger::W(TAG, "Skipping deliver ordered [%s] %s to %s: process crashing"
            , mQueueName.string(), TO_CSTR(r), r->mCurApp->ToString().string());
        skip = TRUE;
    }
    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    if (!skip) {
        Int32 userId;
        uhHelper->GetUserId(appUid, &userId);
        Boolean isAvailable = FALSE;
        if (FAILED(AppGlobals::GetPackageManager()->IsPackageAvailable(
                appPackageName, userId, &isAvailable))) {
            // all such failures mean we skip this receiver
            Slogger::W(TAG, "Exception getting recipient info for %s", appPackageName.string());
        }
        if (!isAvailable) {
            if (DEBUG_BROADCAST) {
                Slogger::V(TAG, "Skipping delivery to %s / %d : package no longer available",
                    appPackageName.string(), appUid);
            }
            skip = TRUE;
        }
    }

    if (skip) {
        if (DEBUG_BROADCAST)  {
            Slogger::V(TAG, "Skipping delivery of ordered [%s] %s for whatever reason",
                mQueueName.string(), TO_CSTR(r));
        }
        r->mReceiver = NULL;
        r->mCurFilter = NULL;
        r->mState = BroadcastRecord::IDLE;
        ScheduleBroadcastsLocked();
        return NOERROR;
    }

    r->mState = BroadcastRecord::APP_RECEIVE;
    String targetProcess = processName;
    r->mCurComponent = component;
    Int32 receiverUid = appUid;
    // If it's a singleton, it needs to be the same app or a special app
    if (r->mCallingUid != IProcess::SYSTEM_UID && isSingleton
        && mService->IsValidSingletonCall(r->mCallingUid, receiverUid)) {
        aInfo = mService->GetActivityInfoForUser(aInfo, 0);
        info->SetActivityInfo(aInfo);
    }
    r->mCurReceiver = aInfo;
    if (DEBUG_MU && r->mCallingUid > IUserHandle::PER_USER_RANGE) {
        String str;
        IObject::Probe(aInfo)->ToString(&str);
        Slogger::V(TAG_MU, "Updated broadcast record activity info for secondary user,"
            " %s, callingUid = %d, uid = %d", str.string(), r->mCallingUid,  appUid);
    }

    // Broadcast is being executed, its package can't be stopped.
    // try {
    String pkgName;
    r->mCurComponent->GetPackageName(&pkgName);
    Int32 userId;
    uhHelper->GetUserId(r->mCallingUid, &userId);
    if (FAILED(AppGlobals::GetPackageManager()->SetPackageStoppedState(pkgName, FALSE, userId))) {
        Slogger::W(TAG, "Failed trying to unstop package %s", pkgName.string());
    }

    // Is this receiver's application already running?
    AutoPtr<ProcessRecord> app = mService->GetProcessRecordLocked(targetProcess, appUid, FALSE);
    if (app != NULL && app->mThread != NULL) {
        // try {
        Int32 versionCode;
        appInfo->GetVersionCode(&versionCode);
        app->AddPackage(packageName, versionCode, mService->mProcessStats);
        ECode ec = ProcessCurBroadcastLocked(r, app);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            String str;
            IObject::Probe(r->mCurComponent)->ToString(&str);
            Slogger::W(TAG, "Exception when sending broadcast to %s", str.string());
        }
        else if (ec == (ECode)E_RUNTIME_EXCEPTION) {
            String str, str2;
            IObject::Probe(r->mCurComponent)->ToString(&str);
            IObject::Probe(r->mIntent)->ToString(&str2);
            Slogger::W/*wtf*/(TAG, "Failed sending broadcast to %s with %s",
                str.string(), str2.string());
            // If some unexpected exception happened, just skip
            // this broadcast.  At this point we are not in the call
            // from a client, so throwing an exception out from here
            // will crash the entire system instead of just whoever
            // sent the broadcast.
            LogBroadcastReceiverDiscardLocked(r);
            FinishReceiverLocked(r, r->mResultCode, r->mResultData,
                    r->mResultExtras, r->mResultAbort, FALSE);
            ScheduleBroadcastsLocked();
            // We need to reset the state if we failed to start the receiver.
            r->mState = BroadcastRecord::IDLE;
            return NOERROR;
        }
        else {
            return NOERROR;
        }

        // If a dead object exception was thrown -- fall through to
        // restart the application.
    }

    // Not running -- get it started, to be executed when the app comes up.
    if (DEBUG_BROADCAST) {
        Slogger::V(TAG, "Need to start app [%s] %s for broadcast %s"
                , mQueueName.string(), targetProcess.string(), TO_CSTR(r));
    }
    AutoPtr<ISystemProperties> systemProperties;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&systemProperties);
    AutoPtr<List<String> > whiteList = GetWhiteList();
    Int32 v;
    Int32 intentFlags;
    r->mIntent->GetFlags(&intentFlags);
    if (((systemProperties->GetInt32(String("sys.quickboot.enable"), 0, &v), v == 1) &&
            (systemProperties->GetInt32(String("sys.quickboot.poweron"), 0, &v), v == 0) &&
            Find(whiteList->Begin(), whiteList->End(), appPackageName) == whiteList->End())
        || (r->mCurApp = mService->StartProcessLocked(targetProcess,
            appInfo, TRUE, intentFlags | IIntent::FLAG_FROM_BACKGROUND,
            String("broadcast"), r->mCurComponent,
            (intentFlags & IIntent::FLAG_RECEIVER_BOOT_UPGRADE) != 0, FALSE, FALSE))
                    == NULL) {
        // Ah, this recipient is unavailable.  Finish it if necessary,
        // and mark the broadcast record as ready for the next.
        String str;
        IObject::Probe(r->mIntent)->ToString(&str);
        Slogger::W(TAG, "Unable to launch app %s/%d for broadcast %s: process is bad"
                , appPackageName.string(), appUid, str.string());
        LogBroadcastReceiverDiscardLocked(r);
        FinishReceiverLocked(r, r->mResultCode, r->mResultData,
                r->mResultExtras, r->mResultAbort, FALSE);
        ScheduleBroadcastsLocked();
        r->mState = BroadcastRecord::IDLE;
        return NOERROR;
    }

    mPendingBroadcast = r;
    mPendingBroadcastRecvIndex = recIdx;

    return NOERROR;
}

AutoPtr<List<String> > BroadcastQueue::GetWhiteList()
{
    if (mQuickbootWhiteList == NULL) {
        mQuickbootWhiteList = new List<String>();
        // allow deskclock app to be launched
        mQuickbootWhiteList->PushBack(String("com.android.deskclock"));
        mQuickbootWhiteList->PushBack(String("com.qapp.quickboot"));
    }
    return mQuickbootWhiteList;
}

void BroadcastQueue::SetBroadcastTimeoutLocked(
    /* [in] */ Int64 timeoutTime)
{
    if (!mPendingBroadcastTimeoutMessage) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(BROADCAST_TIMEOUT_MSG, TO_IINTERFACE(this), (IMessage**)&msg);
        Boolean result;
        mHandler->SendMessageAtTime(msg, timeoutTime, &result);

        mPendingBroadcastTimeoutMessage = TRUE;
    }
}

void BroadcastQueue::CancelBroadcastTimeoutLocked()
{
    if (mPendingBroadcastTimeoutMessage) {
        mHandler->RemoveMessages(BROADCAST_TIMEOUT_MSG, TO_IINTERFACE(this));
        mPendingBroadcastTimeoutMessage = FALSE;
    }
}

void BroadcastQueue::BroadcastTimeoutLocked(
    /* [in] */ Boolean fromMsg)
{
    if (fromMsg) {
        mPendingBroadcastTimeoutMessage = FALSE;
    }

    if (mOrderedBroadcasts.IsEmpty()) {
        return;
    }

    Int64 now = SystemClock::GetUptimeMillis();
    AutoPtr<BroadcastRecord> r = mOrderedBroadcasts.GetFront();
    if (fromMsg) {
        if (mService->mDidDexOpt) {
            // Delay timeouts until dexopt finishes.
            mService->mDidDexOpt = FALSE;
            Int64 timeoutTime = SystemClock::GetUptimeMillis() + mTimeoutPeriod;
            SetBroadcastTimeoutLocked(timeoutTime);
            return;
        }
        if (!mService->mProcessesReady) {
            // Only process broadcast timeouts if the system is ready. That way
            // PRE_BOOT_COMPLETED broadcasts can't timeout as they are intended
            // to do heavy lifting for system up.
            return;
        }

        Int64 timeoutTime = r->mReceiverTime + mTimeoutPeriod;
        if (timeoutTime > now) {
            // We can observe premature timeouts because we do not cancel and reset the
            // broadcast timeout message after each receiver finishes.  Instead, we set up
            // an initial timeout then kick it down the road a little further as needed
            // when it expires.
            if (DEBUG_BROADCAST) {
                Slogger::V(TAG, "Premature timeout [%s] @ %d: resetting BROADCAST_TIMEOUT_MSG for %d"
                        , mQueueName.string(), now, timeoutTime);
            }
            SetBroadcastTimeoutLocked(timeoutTime);
            return;
        }
    }

    AutoPtr<BroadcastRecord> br = mOrderedBroadcasts.GetFront();
    if (br->mState == BroadcastRecord::WAITING_SERVICES) {
        // In this case the broadcast had already finished, but we had decided to wait
        // for started services to finish as well before going on.  So if we have actually
        // waited long enough time timeout the broadcast, let's give up on the whole thing
        // and just move on to the next.
        String str("(null)");
        if (br->mCurComponent != NULL)
            br->mCurComponent->FlattenToShortString(&str);
        Slogger::I(CActivityManagerService::TAG, "Waited long enough for: %s", str.string());
        br->mCurComponent = NULL;
        br->mState = BroadcastRecord::IDLE;
        ProcessNextBroadcast(FALSE);
        return;
    }

    String record = r->ToString();
    String receiver;
    if (r->mReceiver) {
        r->mReceiver->ToString(&receiver);
    }

    Slogger::W(TAG, "Timeout of broadcast %s - receiver=%s, started %d ms ago"
            , record.string(), receiver.string(), (now - r->mReceiverTime));

    r->mReceiverTime = now;
    r->mAnrCount++;

    // Current receiver has passed its expiration date.
    if (r->mNextReceiver <= 0) {
        Slogger::W(TAG, "Timeout on receiver with nextReceiver <= 0");
        return;
    }

    AutoPtr<ProcessRecord> app;
    String anrMessage;

    AutoPtr<IInterface> curReceiver;
    r->mReceivers->Get(r->mNextReceiver - 1, (IInterface**)&curReceiver);
    String str;
    IObject::Probe(curReceiver)->ToString(&str);
    Slogger::W(TAG, "Receiver during timeout: %s", str.string());
    LogBroadcastReceiverDiscardLocked(r);
    if (IBroadcastFilter::Probe(curReceiver) != NULL) {
        AutoPtr<BroadcastFilter> bf = (BroadcastFilter*)IBroadcastFilter::Probe(curReceiver);
        if (bf->mReceiverList->mPid != 0
                && bf->mReceiverList->mPid != CActivityManagerService::MY_PID) {
            AutoLock lock(mService->mPidsSelfLockedLock);
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it =
                mService->mPidsSelfLocked.Find(bf->mReceiverList->mPid);
            if (it != mService->mPidsSelfLocked.End())
                app = it->mSecond;
        }
    }
    else {
        app = r->mCurApp;
    }

    if (app != NULL) {
        String str;
        r->mIntent->ToString(&str);
        anrMessage.AppendFormat("Broadcast of %s", str.string());
    }

    if (mPendingBroadcast == r) {
        mPendingBroadcast = NULL;
    }

    // Move on to the next receiver.
    FinishReceiverLocked(r, r->mResultCode, r->mResultData,
            r->mResultExtras, r->mResultAbort, TRUE);
    ScheduleBroadcastsLocked();

    if (!anrMessage.IsNull()) {
        // Post the ANR to the handler since we do not want to process ANRs while
        // potentially holding our lock.

        AutoPtr<IRunnable> anr = new AppNotResponding(app, anrMessage, this);
        Boolean result;
        mHandler->Post(anr, &result);
    }
}

void BroadcastQueue::AddBroadcastToHistoryLocked(
    /* [in] */ BroadcastRecord* r)
{
    if (r->mCallingUid < 0) {
        // This was from a registerReceiver() call; ignore it.
        return;
    }

    for (Int32 i = MAX_BROADCAST_HISTORY - 1; i > 0; i--) {
        mBroadcastHistory->Set(i, (*mBroadcastHistory)[i - 1]);
    }
    r->mFinishTime = SystemClock::GetUptimeMillis();
    mBroadcastHistory->Set(0, r);

    for (Int32 i = MAX_BROADCAST_SUMMARY_HISTORY - 1; i > 0; i--) {
        mBroadcastSummaryHistory->Set(i, (*mBroadcastSummaryHistory)[i - 1]);
    }
    mBroadcastSummaryHistory->Set(0, r->mIntent);
}

void BroadcastQueue::LogBroadcastReceiverDiscardLocked(
    /* [in] */ BroadcastRecord* r)
{
    if (r->mNextReceiver > 0) {
        AutoPtr<IInterface> curReceiver;
        r->mReceivers->Get(r->mNextReceiver-1, (IInterface**)&curReceiver);
        AutoPtr<BroadcastFilter> bFilter = (BroadcastFilter*)IBroadcastFilter::Probe(curReceiver);
        if (bFilter != NULL) {
            String filter = bFilter->ToString();
            String record = r->ToString();
            Slogger::W(TAG, "BroadcastRecord: %s, BroadcastFilter: %s", record.string(), filter.string());
    //         EventLog.writeEvent(EventLogTags.AM_BROADCAST_DISCARD_FILTER,
    //                 bf.owningUserId, System.identityHashCode(r),
    //                 r->mIntent.getAction(),
    //                 r->mNextReceiver - 1,
    //                 System.identityHashCode(bf));
        }
        else {
            AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(curReceiver);
            String record = r->ToString();
            String resolve;
            if (ri) {
                ri->ToString(&resolve);
            }
            Slogger::W(TAG, "BroadcastRecord: %s, ResolveInfo: %s", record.string(), resolve.string());

    //         EventLog.writeEvent(EventLogTags.AM_BROADCAST_DISCARD_APP,
    //                 UserHandle.getUserId(ri.activityInfo.applicationInfo.uid),
    //                 System.identityHashCode(r), r->mIntent.getAction(),
    //                 r->mNextReceiver - 1, ri.toString());
        }
    }
    else {
        Slogger::W(TAG, "Discarding broadcast before first receiver is invoked: %s",
                TO_CSTR(r));
    //     EventLog.writeEvent(EventLogTags.AM_BROADCAST_DISCARD_APP,
    //             -1, System.identityHashCode(r),
    //             r->mIntent.getAction(),
    //             r->mNextReceiver,
    //             "NONE");
    }
}

Boolean BroadcastQueue::DumpLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll,
    /* [in] */ const String& dumpPackage,
    /* [in] */ Boolean needSep)
{
    if (!mParallelBroadcasts.IsEmpty() || !mOrderedBroadcasts.IsEmpty() || mPendingBroadcast != NULL) {
        Boolean printed = FALSE;
        Int32 i = mParallelBroadcasts.GetSize() - 1;
        List< AutoPtr<BroadcastRecord> >::ReverseIterator rit;
        for (rit = mParallelBroadcasts.RBegin(); rit != mParallelBroadcasts.REnd(); ++rit, --i) {
            AutoPtr<BroadcastRecord> br = *rit;
            if (!dumpPackage.IsNull() && !dumpPackage.Equals(br->mCallerPackage)) {
                continue;
            }
            if (!printed) {
                if (needSep) {
                    pw->Println();
                }
                needSep = TRUE;
                printed = TRUE;
                pw->Println(String("  Active broadcasts [") + mQueueName + "]:");
            }
            StringBuilder sb("  Active Broadcast ");
            sb += mQueueName;
            sb += " #";
            sb += i;
            sb += ":";
            pw->Println(sb.ToString());
            br->Dump(pw, String("    "));
        }
        printed = FALSE;
        needSep = TRUE;
        i = mOrderedBroadcasts.GetSize() - 1;
        for (rit = mOrderedBroadcasts.RBegin(); rit != mOrderedBroadcasts.REnd(); ++rit, --i) {
            AutoPtr<BroadcastRecord> br = *rit;
            if (!dumpPackage.IsNull() && !dumpPackage.Equals(br->mCallerPackage)) {
                continue;
            }
            if (!printed) {
                if (needSep) {
                    pw->Println();
                }
                needSep = TRUE;
                printed = TRUE;
                pw->Println(String("  Active ordered broadcasts [") + mQueueName + "]:");
            }
            StringBuilder sb("  Active Ordered Broadcast ");
            sb += mQueueName;
            sb += " #";
            sb += i;
            sb += ":";
            pw->Println(sb.ToString());
            br->Dump(pw, String("    "));
        }
        if (dumpPackage.IsNull() || (mPendingBroadcast != NULL
                && dumpPackage.Equals(mPendingBroadcast->mCallerPackage))) {
            if (needSep) {
                pw->Println();
            }
            pw->Println(String("  Pending broadcast [") + mQueueName + "]:");
            if (mPendingBroadcast != NULL) {
                mPendingBroadcast->Dump(pw, String("    "));
            }
            else {
                pw->Println(String("    (null)"));
            }
            needSep = TRUE;
        }
    }

    Int32 i;
    Boolean printed = FALSE;
    for (i = 0; i < MAX_BROADCAST_HISTORY; i++) {
        AutoPtr<BroadcastRecord> r = (*mBroadcastHistory)[i];
        if (r == NULL) {
            break;
        }
        if (dumpPackage != NULL && !dumpPackage.Equals(r->mCallerPackage)) {
            continue;
        }
        if (!printed) {
            if (needSep) {
                pw->Println();
            }
            needSep = TRUE;
            pw->Println(String("  Historical broadcasts [") + mQueueName + "]:");
            printed = TRUE;
        }
        if (dumpAll) {
            pw->Print(String("  Historical Broadcast ") + mQueueName + " #");
            pw->Print(i);
            pw->Println(String(":"));
            r->Dump(pw, String("    "));
        }
        else {
            pw->Print(String("  #"));
            pw->Print(i);
            pw->Print(String(": "));
            pw->Println(r->ToString());
            pw->Print(String("    "));
            String intentStr;
            r->mIntent->ToShortString(FALSE, TRUE, TRUE, FALSE, &intentStr);
            pw->Println(intentStr);
            AutoPtr<IComponentName> name;
            r->mIntent->GetComponent((IComponentName**)&name);
            if (r->mTargetComp != NULL && r->mTargetComp != name) {
                pw->Print(String("    targetComp: "));
                String str;
                r->mTargetComp->ToShortString(&str);
                pw->Println(str);
            }
            AutoPtr<IBundle> bundle;
            r->mIntent->GetExtras((IBundle**)&bundle);
            if (bundle != NULL) {
                pw->Print(String("    extras: "));
                pw->Println(bundle);
            }
        }
    }

    if (dumpPackage.IsNull()) {
        if (dumpAll) {
            i = 0;
            printed = FALSE;
        }
        for (; i < MAX_BROADCAST_SUMMARY_HISTORY; i++) {
            AutoPtr<IIntent> intent = (*mBroadcastSummaryHistory)[i];
            if (intent == NULL) {
                break;
            }
            if (!printed) {
                if (needSep) {
                    pw->Println();
                }
                needSep = TRUE;
                pw->Println(String("  Historical broadcasts summary [") + mQueueName + "]:");
                printed = TRUE;
            }
            if (!dumpAll && i >= 50) {
                pw->Println(String("  ..."));
                break;
            }
            pw->Print(String("  #"));
            pw->Print(i);
            pw->Print(String(": "));
            String iStr;
            intent->ToShortString(FALSE, TRUE, TRUE, FALSE, &iStr);
            pw->Println(iStr);
            AutoPtr<IBundle> bundle;
            intent->GetExtras((IBundle**)&bundle);
            if (bundle != NULL) {
                pw->Print(String("    extras: "));
                pw->Println(bundle);
            }
        }
    }

    return needSep;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
