
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Core.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/AppGlobals.h"
//TODO #include "elastos/droid/internal/os/BatteryStatsImpl.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/server/am/ActivityRecord.h"
#include "elastos/droid/server/am/ActivityStack.h"
#include "elastos/droid/server/am/ActivityState.h"
#include "elastos/droid/server/wm/AppTransition.h"
#include "elastos/droid/server/wm/TaskGroup.h"
#include "elastos/droid/server/Watchdog.h"

#include "elastos/core/AutoLock.h"
#include "elastos/core/StringUtils.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::CActivityManager;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerRunningTaskInfo;
using Elastos::Droid::App::CActivityOptions;
using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::CResultInfo;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IIActivityContainer;
using Elastos::Droid::App::IIActivityController;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerRunningTaskInfo;
using Elastos::Droid::App::IActivityOptionsHelper;
using Elastos::Droid::App::IResultInfo;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIIntentSender;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Internal::Os::IBatteryStatsImplUidProc;
//TODO using Elastos::Droid::Internal::Os::BatteryStatsImpl;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::ITrace;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Server::Am::IActivityRecord;
using Elastos::Droid::Server::Watchdog;
using Elastos::Droid::Server::Wm::AppTransition;
using Elastos::Droid::Server::Wm::TaskGroup;
using Elastos::Droid::View::IDisplay;

using Elastos::Core::AutoLock;
using Elastos::Core::CSystem;
using Elastos::Core::IComparable;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

//=====================================================================
//                  ActivityStack::ScheduleDestroyArgs
//=====================================================================
ActivityStack::ScheduleDestroyArgs::ScheduleDestroyArgs(
    /* [in] */ ProcessRecord* owner,
    /* [in] */ const String& reason)
{
    // ==================before translated======================
    // mOwner = owner;
    // mReason = reason;
}

//=====================================================================
//                 ActivityStack::ActivityStackHandler
//=====================================================================
ActivityStack::ActivityStackHandler::ActivityStackHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ ActivityStack* owner)
    :Handler(looper)
    , mOwner(owner)
{
}

ECode ActivityStack::ActivityStackHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case PAUSE_TIMEOUT_MSG: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ActivityRecord* r = (ActivityRecord*)(IObject::Probe(obj));
            // We don't at this point know if the activity is fullscreen,
            // so we need to be conservative and assume it isn't.
            Slogger::W("ActivityStack::ActivityStackHandler::HandleMessage", "Activity pause timeout for %s", r->ToString().string());
            {
                AutoLock lock(mOwner->mService);
                if (r->mApp != NULL) {
                    mOwner->mService->LogAppTooSlow(r->mApp, r->mPauseTime, String("pausing ") + r->ToString());
                }
                mOwner->ActivityPausedLocked(IBinder::Probe(r->mAppToken), TRUE);
            }
        } break;
        case LAUNCH_TICK_MSG: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ActivityRecord* r = (ActivityRecord*)(IObject::Probe(obj));
            {
                AutoLock lock(mOwner->mService);
                if (r->ContinueLaunchTickingLocked()) {
                    mOwner->mService->LogAppTooSlow(r->mApp, r->mLaunchTickTime, String("launching ") + r->ToString());
                }
            }
        } break;
        case DESTROY_TIMEOUT_MSG: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ActivityRecord* r = (ActivityRecord*)(IObject::Probe(obj));
            // We don't at this point know if the activity is fullscreen,
            // so we need to be conservative and assume it isn't.
            Slogger::W("ActivityStack::ActivityStackHandler::HandleMessage",
                    "Activity destroy timeout for %s", r->ToString().string());
            {
                AutoLock lock(mOwner->mService);
                mOwner->ActivityDestroyedLocked(r != NULL? IBinder::Probe(r->mAppToken) : NULL);
            }
        } break;
        case STOP_TIMEOUT_MSG: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ActivityRecord* r = (ActivityRecord*)(IObject::Probe(obj));
            // We don't at this point know if the activity is fullscreen,
            // so we need to be conservative and assume it isn't.
            Slogger::W("ActivityStack::ActivityStackHandler::HandleMessage",
                    "Activity stop timeout for %s", r->ToString().string());
            {
                AutoLock lock(mOwner->mService);
                if (r->IsInHistory()) {
                    mOwner->ActivityStoppedLocked(r, NULL, NULL, NULL);
                }
            }
        } break;
        case DESTROY_ACTIVITIES_MSG: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ScheduleDestroyArgs* args = (ScheduleDestroyArgs*)(IObject::Probe(obj));
            {
                AutoLock lock(mOwner->mService);
                mOwner->DestroyActivitiesLocked(args->mOwner, args->mReason);
            }
        } break;
        case TRANSLUCENT_TIMEOUT_MSG: {
            {
                AutoLock lock(mOwner->mService);
                mOwner->NotifyActivityDrawnLocked(NULL);
            }
        } break;
        case RELEASE_BACKGROUND_RESOURCES_TIMEOUT_MSG: {
            {
                AutoLock lock(mOwner->mService);
                AutoPtr<ActivityRecord> r = mOwner->GetVisibleBehindActivity();
                Slogger::E("ActivityStack::ActivityStackHandler::HandleMessage",
                        "Timeout waiting for cancelVisibleBehind player=%s", r->ToString().string());
                if (r != NULL) {
                    mOwner->mService->KillAppAtUsersRequest(r->mApp, NULL);
                }
            }
        } break;
    }
    return NOERROR;
}

//=====================================================================
//                            ActivityStack
//=====================================================================
static Boolean Init_SCREENSHOT_FORCE_565()
{
    Boolean res;
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    helper->IsLowRamDeviceStatic(&res);
    return res;
}

const Int32 ActivityStack::LAUNCH_TICK;
const Int32 ActivityStack::PAUSE_TIMEOUT;
const Int32 ActivityStack::STOP_TIMEOUT;
const Int32 ActivityStack::DESTROY_TIMEOUT;
const Int64 ActivityStack::ACTIVITY_INACTIVE_RESET_TIME;
const Int64 ActivityStack::START_WARN_TIME;
const Boolean ActivityStack::SHOW_APP_STARTING_PREVIEW = TRUE;
const Int64 ActivityStack::TRANSLUCENT_CONVERSION_TIMEOUT;
const Boolean ActivityStack::SCREENSHOT_FORCE_565 = Init_SCREENSHOT_FORCE_565();
const Int32 ActivityStack::PAUSE_TIMEOUT_MSG;// = CActivityManagerService::FIRST_ACTIVITY_STACK_MSG + 1;
const Int32 ActivityStack::DESTROY_TIMEOUT_MSG;// = CActivityManagerService::FIRST_ACTIVITY_STACK_MSG + 2;
const Int32 ActivityStack::LAUNCH_TICK_MSG;//  = CActivityManagerService::FIRST_ACTIVITY_STACK_MSG + 3;
const Int32 ActivityStack::STOP_TIMEOUT_MSG;//  = CActivityManagerService::FIRST_ACTIVITY_STACK_MSG + 4;
const Int32 ActivityStack::DESTROY_ACTIVITIES_MSG;//  = CActivityManagerService::FIRST_ACTIVITY_STACK_MSG + 5;
const Int32 ActivityStack::TRANSLUCENT_TIMEOUT_MSG;//  = CActivityManagerService::FIRST_ACTIVITY_STACK_MSG + 6;
const Int32 ActivityStack::RELEASE_BACKGROUND_RESOURCES_TIMEOUT_MSG;//  = CActivityManagerService::FIRST_ACTIVITY_STACK_MSG + 7;
const Int32 ActivityStack::FINISH_IMMEDIATELY;
const Int32 ActivityStack::FINISH_AFTER_PAUSE;
const Int32 ActivityStack::FINISH_AFTER_VISIBLE;

ActivityStack::ActivityStack(
    /* [in] */ ActivityStackSupervisor::ActivityContainer* activityContainer)
    : mConfigWillChange(FALSE)
    , mLaunchStartTime(0)
    , mFullyDrawnStartTime(0)
    , mCurrentUser(0)
    , mDisplayId(0)
    , mInResumeTopActivity(FALSE)
{
    mActivityContainer = activityContainer;
    mStackSupervisor = activityContainer->GetOuter();
    mService = mStackSupervisor->mService;
    AutoPtr<ILooper> looper;
    mService->mHandler->GetLooper((ILooper**)&looper);
    mHandler = new ActivityStackHandler(looper, this);
    mWindowManager = mService->mWindowManager;
    mStackId = activityContainer->mStackId;
    mCurrentUser = mService->mCurrentUserId;//TODO friend this class in ActivityManagerService

    CArrayList::New((IArrayList**)&mTaskHistory);
    CArrayList::New((IArrayList**)&mValidateAppTokens);
    CArrayList::New((IArrayList**)&mLRUActivities);
    CArrayList::New((IArrayList**)&mNoAnimActivities);
    CArrayList::New((IArrayList**)&mUndrawnActivitiesBelowTopTranslucent);
}

Int32 ActivityStack::NumActivities()
{
    Int32 count = 0;
    Int32 size;
    mTaskHistory->GetSize(&size);
    for (Int32 taskNdx = size - 1; taskNdx >= 0; --taskNdx) {
        AutoPtr<IInterface> obj;
        mTaskHistory->Get(taskNdx, (IInterface**)&obj);
        TaskRecord* tr = (TaskRecord*)(IObject::Probe(obj));
        count += tr->mActivities->GetSize();
    }
    return count;
}

Boolean ActivityStack::OkToShowLocked(
    /* [in] */ ActivityRecord* r)
{
    Int32 flags;
    return IsCurrentProfileLocked(r->mUserId)
            || ((r->mInfo->GetFlags(&flags), flags) & IActivityInfo::FLAG_SHOW_ON_LOCK_SCREEN) != 0;
}

AutoPtr<ActivityRecord> ActivityStack::TopRunningActivityLocked(
    /* [in] */ ActivityRecord* notTop)
{
    Int32 size;
    mTaskHistory->GetSize(&size);
    for (Int32 taskNdx = size - 1; taskNdx >= 0; --taskNdx) {
        AutoPtr<IInterface> obj;
        mTaskHistory->Get(taskNdx, (IInterface**)&obj);
        AutoPtr<TaskRecord> tr = (TaskRecord*)(IObject::Probe(obj));
        AutoPtr<ActivityRecord> r = tr->TopRunningActivityLocked(notTop);
        if (r != NULL) {
            return r;
        }
    }
    return NULL;
}

AutoPtr<ActivityRecord> ActivityStack::TopRunningNonDelayedActivityLocked(
    /* [in] */ ActivityRecord* notTop)
{
    Int32 size;
    mTaskHistory->GetSize(&size);
    for (Int32 taskNdx = size - 1; taskNdx >= 0; --taskNdx) {
        AutoPtr<IInterface> obj;
        mTaskHistory->Get(taskNdx, (IInterface**)&obj);
        AutoPtr<TaskRecord> task = (TaskRecord*)(IObject::Probe(obj));
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            AutoPtr<ActivityRecord> r = (*activities)[activityNdx];
            if (!r->mFinishing && !r->mDelayedResume && r.Get() != notTop && OkToShowLocked(r)) {
                return r;
            }
        }
    }
    return NULL;
}

AutoPtr<ActivityRecord> ActivityStack::TopRunningActivityLocked(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 taskId)
{
    Int32 size;
    mTaskHistory->GetSize(&size);
    for (Int32 taskNdx = size - 1; taskNdx >= 0; --taskNdx) {
        AutoPtr<IInterface> obj;
        mTaskHistory->Get(taskNdx, (IInterface**)&obj);
        AutoPtr<TaskRecord> task = (TaskRecord*)(IObject::Probe(obj));
        if (task->mTaskId == taskId) {
            continue;
        }
        //ArrayList<ActivityRecord> activities = task.mActivities;
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        //List<AutoPtr<ActivityRecord> >::Iterator iterator = activities.RBegin();
        for (Int32 i = activities->GetSize() - 1; i >= 0; --i) {
            AutoPtr<ActivityRecord> r = (*activities)[i];
            // Note: the taskId check depends on real taskId fields being non-zero
            if (!r->mFinishing && (token != IBinder::Probe(r->mAppToken)) && OkToShowLocked(r)) {
                return r;
            }
        }
    }
    return NULL;
}

AutoPtr<ActivityRecord> ActivityStack::TopActivity()
{
    Int32 size;
    mTaskHistory->GetSize(&size);
    for (Int32 taskNdx = size - 1; taskNdx >= 0; --taskNdx) {
        AutoPtr<IInterface> obj;
        mTaskHistory->Get(taskNdx, (IInterface**)&obj);
        AutoPtr<TaskRecord> tr = (TaskRecord*)(IObject::Probe(obj));
        //ArrayList<ActivityRecord> activities = tr->mActivities;
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = tr->mActivities;
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            //final ActivityRecord r = activities.get(activityNdx);
            AutoPtr<ActivityRecord> r = (*activities)[activityNdx];
            if (!r->mFinishing) {
                return r;
            }
        }
    }
    return NULL;
}

AutoPtr<TaskRecord> ActivityStack::TopTask()
{
    Int32 size;
    mTaskHistory->GetSize(&size);
    if (size > 0) {
        AutoPtr<IInterface> obj;
        mTaskHistory->Get(size - 1, (IInterface**)&obj);
        AutoPtr<TaskRecord> tr = (TaskRecord*)(IObject::Probe(obj));
        return tr;
    }
    return NULL;
}

AutoPtr<TaskRecord> ActivityStack::TaskForIdLocked(
    /* [in] */ Int32 id)
{
    Int32 size;
    mTaskHistory->GetSize(&size);
    for (Int32 taskNdx = size - 1; taskNdx >= 0; --taskNdx) {
        //final TaskRecord task = mTaskHistory.get(taskNdx);
        AutoPtr<IInterface> obj;
        mTaskHistory->Get(taskNdx, (IInterface**)&obj);
        AutoPtr<TaskRecord> task = (TaskRecord*)(IObject::Probe(obj));
        if (task->mTaskId == id) {
            return task;
        }
    }
    return NULL;
}

AutoPtr<ActivityRecord> ActivityStack::IsInStackLocked(
    /* [in] */ IBinder* token)
{
    AutoPtr<ActivityRecord> r = ActivityRecord::ForToken(token);
    if (r != NULL) {
        AutoPtr<TaskRecord> task = r->mTask;
        if (task != NULL) {
            Boolean taskHistoryContainsTask;
            mTaskHistory->Contains(TO_IINTERFACE(task), &taskHistoryContainsTask);
            Boolean activitiesContainsTask = FALSE;
            AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
            List<AutoPtr<ActivityRecord> >::Iterator iterator = activities->Begin();
            while(iterator != activities->End()) {
                if ((*iterator).Get() == r.Get()) {
                    activitiesContainsTask = TRUE;
                    break;
                }
            }

            if (activitiesContainsTask && taskHistoryContainsTask) {
                if (task->mStack != this)
                    Slogger::W(CActivityManagerService::TAG, "Illegal state! task does not point to stack it is in.");
                return r;
            }
        }
    }
    return NULL;
}

Boolean ActivityStack::UpdateLRUListLocked(
    /* [in] */ ActivityRecord* r)
{
    Boolean hadit;
    mLRUActivities->Remove(TO_IINTERFACE(r), &hadit);
    mLRUActivities->Add(TO_IINTERFACE(r));
    return hadit;
}

Boolean ActivityStack::IsHomeStack()
{
    return mStackId == ActivityStackSupervisor::HOME_STACK_ID;
}

Boolean ActivityStack::IsOnHomeDisplay()
{
    return IsAttached() &&
            mActivityContainer->mActivityDisplay->mDisplayId == IDisplay::DEFAULT_DISPLAY;
}

void ActivityStack::MoveToFront()
{
    if (IsAttached()) {
        if (IsOnHomeDisplay()) {
            mStackSupervisor->MoveHomeStack(IsHomeStack());
        }
        mStacks->Remove(TO_IINTERFACE(this));
        mStacks->Add(TO_IINTERFACE(this));
    }
}

Boolean ActivityStack::IsAttached()
{
    return mStacks != NULL;
}

AutoPtr<ActivityRecord> ActivityStack::FindTaskLocked(
    /* [in] */ ActivityRecord* target)
{
    AutoPtr<IIntent> intent = target->mIntent;
    AutoPtr<IComponentName> cls;
    intent->GetComponent((IComponentName**)&cls);
    String sTargetActivity;
    AutoPtr<IActivityInfo> info = target->mInfo;
    info->GetTargetActivity(&sTargetActivity);
    if (!sTargetActivity.IsNull()) {
        String sPackagename;
        IPackageItemInfo::Probe(info)->GetPackageName(&sPackagename);
        //cls = new ComponentName(info.packageName, info.targetActivity);
        CComponentName::New(sPackagename, sTargetActivity, (IComponentName**)&cls);
    }
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(info)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 uid;
    appInfo->GetUid(&uid);
    Int32 userId;
    helper->GetUserId(uid, &userId);

    Boolean isDocument = FALSE;
    if (intent != NULL) {
        intent->IsDocument(&isDocument);
    }
    // If documentData is non-NULL then it must match the existing task data.
    AutoPtr<IUri> documentData;
    if (isDocument) {
        intent->GetData((IUri**)&documentData);
    }

    //if (CActivityManagerService::DEBUG_TASKS) Slogger::D(CActivityManagerService::TAG, "Looking for task of " + target + " in " + this);
    Int32 size;
    mTaskHistory->GetSize(&size);
    for (Int32 taskNdx = size - 1; taskNdx >= 0; --taskNdx) {
        AutoPtr<IInterface> obj;
        mTaskHistory->Get(taskNdx, (IInterface**)&obj);
        AutoPtr<TaskRecord> task = (TaskRecord*)(IObject::Probe(obj));
        if (task->mVoiceSession != NULL) {
            // We never match voice sessions; those always run independently.
            if (CActivityManagerService::DEBUG_TASKS) Slogger::D(CActivityManagerService::TAG, "Skipping %s: voice session", task->ToString().string());
            continue;
        }
        if (task->mUserId != userId) {
            // Looking for a different task.
            if (CActivityManagerService::DEBUG_TASKS) Slogger::D(CActivityManagerService::TAG, "Skipping %s: different user", task->ToString().string());
            continue;
        }
        AutoPtr<ActivityRecord> r = task->GetTopActivity();
        if (r == NULL || r->mFinishing || r->mUserId != userId ||
                r->mLaunchMode == IActivityInfo::LAUNCH_SINGLE_INSTANCE) {
            if (CActivityManagerService::DEBUG_TASKS) Slogger::D(CActivityManagerService::TAG, "Skipping %s: mismatch root %s", task->ToString().string(), r->ToString().string());
            continue;
        }

        AutoPtr<IIntent> taskIntent = task->mIntent;
        AutoPtr<IIntent> affinityIntent = task->mAffinityIntent;
        Boolean taskIsDocument = FALSE, affinityIsDocument = FALSE;
        AutoPtr<IUri> taskDocumentData;
        if (taskIntent != NULL) {
            taskIntent->IsDocument(&taskIsDocument);
        }
        if (affinityIntent != NULL) {
            affinityIntent->IsDocument(&affinityIsDocument);
        }
        if (taskIntent != NULL && taskIsDocument) {
            taskIsDocument = TRUE;
            taskIntent->GetData((IUri**)&taskDocumentData);
        } else if (affinityIntent != NULL && affinityIsDocument
            ) {
            taskIsDocument = TRUE;
            affinityIntent->GetData((IUri**)&taskDocumentData);
        } else {
            taskIsDocument = FALSE;
        }

        if (CActivityManagerService::DEBUG_TASKS) Slogger::D(CActivityManagerService::TAG, "Comparing existing cls=");
        //        + taskIntent.getComponent().flattenToShortString()
        //        + "/aff=" + r.task.rootAffinity + " to new cls="
        //        + intent.getComponent().flattenToShortString() + "/aff=" + info.taskAffinity);
        if (!isDocument && !taskIsDocument && (!task->mRootAffinity.IsNull())) {
            if (task->mRootAffinity.Equals(target->mTaskAffinity)) {
                if (CActivityManagerService::DEBUG_TASKS) Slogger::D(CActivityManagerService::TAG, "Found matching affinity!");
                return r;
            }
            return NULL;
        }
        AutoPtr<IComponentName> clstmp;
        if (taskIntent != NULL && (taskIntent->GetComponent((IComponentName**)&clstmp), clstmp) != NULL) {
            Int32 result;
            IComparable::Probe(clstmp)->CompareTo(TO_IINTERFACE(cls), &result);
            Boolean equals;
            IObject::Probe(documentData)->Equals(TO_IINTERFACE(taskDocumentData), &equals);
            if (result == 0 && equals) {
                if (CActivityManagerService::DEBUG_TASKS) Slogger::D(CActivityManagerService::TAG, "Found matching class!");
                //dump();
                //if (CActivityManagerService::DEBUG_TASKS) Slogger::D(CActivityManagerService::TAG, "For Intent " + intent + " bringing to top: " + r.intent);
                return r;
            }
        }

        clstmp = NULL;
        if (affinityIntent != NULL && (affinityIntent->GetComponent((IComponentName**)&clstmp), clstmp)  != NULL) {
            Int32 result;
            IComparable::Probe(clstmp)->CompareTo(TO_IINTERFACE(cls), &result);
            Boolean equals;
            IObject::Probe(documentData)->Equals(TO_IINTERFACE(taskDocumentData), &equals);
            if (result == 0 && equals) {
                if (CActivityManagerService::DEBUG_TASKS) Slogger::D(CActivityManagerService::TAG, "Found matching class!!");
                //dump();
                //if (CActivityManagerService::DEBUG_TASKS) Slogger::D(CActivityManagerService::TAG, "For Intent " + intent + " bringing to top: " + r.intent);
                return r;
            }
        }
        if (CActivityManagerService::DEBUG_TASKS) {
            Slogger::D(CActivityManagerService::TAG, "Not a match: %s", task->ToString().string());
        }
    }

    return NULL;
}

AutoPtr<ActivityRecord> ActivityStack::FindActivityLocked(
    /* [in] */ IIntent* intent,
    /* [in] */ IActivityInfo* info)
{
    AutoPtr<IComponentName> cls;
    intent->GetComponent((IComponentName**)&cls);
    String sTargetActivity;
    info->GetTargetActivity(&sTargetActivity);
    if (!sTargetActivity.IsNull()) {
        String sPackagename;
        IPackageItemInfo::Probe(info)->GetPackageName(&sPackagename);
        CComponentName::New(sPackagename, sTargetActivity, (IComponentName**)&cls);
    }
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(info)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 uid;
    appInfo->GetUid(&uid);
    Int32 userId;
    helper->GetUserId(uid, &userId);

    Int32 size;
    mTaskHistory->GetSize(&size);
    for (Int32 taskNdx = size - 1; taskNdx >= 0; --taskNdx) {
        AutoPtr<IInterface> obj;
        mTaskHistory->Get(taskNdx, (IInterface**)&obj);
        AutoPtr<TaskRecord> task = (TaskRecord*)(IObject::Probe(obj));
        if (!IsCurrentProfileLocked(task->mUserId)) {
            return NULL;
        }
        //final ArrayList<ActivityRecord> activities = task->mActivities;
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            AutoPtr<ActivityRecord> r = (*activities)[activityNdx];

            if (!r->mFinishing) {
                AutoPtr<IComponentName> clstmp;
                r->mIntent->GetComponent((IComponentName**)&clstmp);
                Boolean equals;
                IObject::Probe(clstmp)->Equals(TO_IINTERFACE(cls), &equals);
                if (equals && r->mUserId == userId) {
                    //Slog.i(CActivityManagerService::TAG, "Found matching class!");
                    //dump();
                    //Slog.i(CActivityManagerService::TAG, "For Intent " + intent + " bringing to top: " + r.intent);
                    return r;
                }
            }
        }
    }

    return NULL;
}

void ActivityStack::SwitchUserLocked(
    /* [in] */ Int32 userId)
{
    if (mCurrentUser == userId) {
        return;
    }
    mCurrentUser = userId;

    // Move userId's tasks to the top.
    Int32 index;
    mTaskHistory->GetSize(&index);
    for (Int32 i = 0; i < index; ) {
        AutoPtr<IInterface> obj;
        mTaskHistory->Get(i, (IInterface**)&obj);
        AutoPtr<TaskRecord> task = (TaskRecord*)(IObject::Probe(obj));
        if (IsCurrentProfileLocked(task->mUserId)) {
            //if (CActivityManagerService::DEBUG_TASKS) Slogger::D(CActivityManagerService::TAG, "switchUserLocked: stack=" + getStackId() + " moving " + task + " to top");
            mTaskHistory->Remove(i);
            mTaskHistory->Add(TO_IINTERFACE(task));
            --index;
            // Use same value for i.
        } else {
            ++i;
        }
    }
    if (CActivityManagerService::VALIDATE_TOKENS) {
        ValidateAppTokensLocked();
    }
}

void ActivityStack::MinimalResumeActivityLocked(
    /* [in] */ ActivityRecord* r)
{
    r->mState = ActivityState_RESUMED;
    //if (ActivityStackSupervisor::DEBUG_STATES) Slog.v(CActivityManagerService::TAG, "Moving to RESUMED: " + r + " (starting new instance)");
    r->mStopped = FALSE;
    mResumedActivity = r;
    r->mTask->TouchActiveTime();
    mService->AddRecentTaskLocked(r->mTask);
    CompleteResumeLocked(r);
    mStackSupervisor->CheckReadyForSleepLocked();
    SetLaunchTime(r);
    //if (ActivityStackSupervisor::DEBUG_SAVED_STATE)
    //    Slogger::I(CActivityManagerService::TAG, "Launch completed; removing icicle of " + r->mIcicle->ToString().string());
}

void ActivityStack::SetLaunchTime(
    /* [in] */ ActivityRecord* r)
{
    if (r->mDisplayStartTime == 0) {
        r->mFullyDrawnStartTime = r->mDisplayStartTime = SystemClock::GetUptimeMillis();
        if (mLaunchStartTime == 0) {
            StartLaunchTraces();
            mLaunchStartTime = mFullyDrawnStartTime = r->mDisplayStartTime;
        }
    } else if (mLaunchStartTime == 0) {
        StartLaunchTraces();
        mLaunchStartTime = mFullyDrawnStartTime = SystemClock::GetUptimeMillis();
    }
}

void ActivityStack::ClearLaunchTime(
    /* [in] */ ActivityRecord* r)
{
    // // Make sure that there is no activity waiting for this to launch.
    Boolean result;
    if (mStackSupervisor->mWaitingActivityLaunched->IsEmpty(&result), result) {
        r->mDisplayStartTime = r->mFullyDrawnStartTime = 0;
    } else {
        mStackSupervisor->RemoveTimeoutsForActivityLocked(r);
        mStackSupervisor->ScheduleIdleTimeoutLocked(r);
    }
}

void ActivityStack::AwakeFromSleepingLocked()
{
    // Ensure activities are no longer sleeping.
    Int32 size;
    mTaskHistory->GetSize(&size);
    for (Int32 taskNdx = size - 1; taskNdx >= 0; --taskNdx) {
        AutoPtr<IInterface> obj;
        mTaskHistory->Get(taskNdx, (IInterface**)&obj);
        AutoPtr<TaskRecord> tr = (TaskRecord*)(IObject::Probe(obj));
        //ArrayList<ActivityRecord> activities = mTaskHistory.get(taskNdx).mActivities;
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = tr->mActivities;
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            //activities.get(activityNdx).setSleeping(false);
            (*activities)[activityNdx]->SetSleeping(FALSE);
        }
    }
}

Boolean ActivityStack::CheckReadyForSleepLocked()
{
    if (mResumedActivity != NULL) {
        // Still have something resumed; can't sleep until it is paused.
        if (CActivityManagerService::DEBUG_PAUSE)
            Slogger::V(CActivityManagerService::TAG, "Sleep needs to pause %s", mResumedActivity->ToString().string());
        if (CActivityManagerService::DEBUG_USER_LEAVING)
            Slogger::V(CActivityManagerService::TAG, "Sleep => pause with userLeaving=false");
        StartPausingLocked(FALSE, TRUE, FALSE, FALSE);
        return TRUE;
    }
    if (mPausingActivity != NULL) {
        // Still waiting for something to pause; can't sleep yet.
        if (CActivityManagerService::DEBUG_PAUSE)
            Slogger::V(CActivityManagerService::TAG, "Sleep still waiting to pause %s", mPausingActivity->ToString().string());
        return TRUE;
    }

    return FALSE;
}

void ActivityStack::GoToSleep()
{
    EnsureActivitiesVisibleLocked(NULL, 0);

    // Make sure any stopped but visible activities are now sleeping.
    // This ensures that the activity's onStop() is called.
    Int32 size;
    mTaskHistory->GetSize(&size);
    for (Int32 taskNdx = size - 1; taskNdx >= 0; --taskNdx) {
        //final ArrayList<ActivityRecord> activities = mTaskHistory.get(taskNdx).mActivities;
        AutoPtr<IInterface> obj;
        mTaskHistory->Get(taskNdx, (IInterface**)&obj);
        AutoPtr<TaskRecord> tr = (TaskRecord*)(IObject::Probe(obj));
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = tr->mActivities;
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            //final ActivityRecord r = activities.get(activityNdx);
            AutoPtr<ActivityRecord> r = (*activities)[activityNdx];
            if (r->mState == ActivityState_STOPPING || r->mState == ActivityState_STOPPED) {
                r->SetSleeping(TRUE);
            }
        }
    }
}

AutoPtr<IBitmap> ActivityStack::ScreenshotActivities(
    /* [in] */ ActivityRecord* who)
{
    if (ActivityStackSupervisor::DEBUG_SCREENSHOTS) Slogger::D(CActivityManagerService::TAG, "screenshotActivities: %s", who->ToString().string());
    if (who->mNoDisplay) {
        if (ActivityStackSupervisor::DEBUG_SCREENSHOTS) Slogger::D(CActivityManagerService::TAG, "\tNo display");
        return NULL;
    }

    if (IsHomeStack()) {
        // This is an optimization -- since we never show Home or Recents within Recents itself,
        // we can just go ahead and skip taking the screenshot if this is the home stack.  In
        // the case where the most recent task is not the task that was supplied, then the stack
        // has changed, so invalidate the last screenshot().
        InvalidateLastScreenshot();
        if (ActivityStackSupervisor::DEBUG_SCREENSHOTS) Slogger::D(CActivityManagerService::TAG, "\tIs Home stack? %d", IsHomeStack());
        return NULL;
    }

    Int32 w = mService->mThumbnailWidth;
    Int32 h = mService->mThumbnailHeight;
    Int32 width, height;
    if (w > 0) {
        if (who != mLastScreenshotActivity || mLastScreenshotBitmap == NULL
                || mLastScreenshotActivity->mState == ActivityState_RESUMED
                || (mLastScreenshotBitmap->GetWidth(&width), width) != w
                || (mLastScreenshotBitmap->GetHeight(&height), height) != h) {
            if (ActivityStackSupervisor::DEBUG_SCREENSHOTS) Slogger::D(CActivityManagerService::TAG, "\tUpdating screenshot");
            mLastScreenshotActivity = who;
            //TODO mLastScreenshotBitmap = mWindowManager->ScreenshotApplications(
            //        IBinder::Probe(who->mAppToken), IDisplay::DEFAULT_DISPLAY, w, h, SCREENSHOT_FORCE_565);
        }
        if (mLastScreenshotBitmap != NULL) {
            if (ActivityStackSupervisor::DEBUG_SCREENSHOTS) Slogger::D(CActivityManagerService::TAG, "\tReusing last screenshot");
            BitmapConfig config;
            mLastScreenshotBitmap->GetConfig(&config);
            AutoPtr<IBitmap> bitmap;
            mLastScreenshotBitmap->Copy(config, TRUE, (IBitmap**)&bitmap);
            return bitmap;
        }
    }
    Slogger::E(CActivityManagerService::TAG, "Invalid thumbnail dimensions: %d x %d", w, h);
    return NULL;
}

Boolean ActivityStack::StartPausingLocked(
    /* [in] */ Boolean userLeaving,
    /* [in] */ Boolean uiSleeping,
    /* [in] */ Boolean resuming,
    /* [in] */ Boolean dontWait)
{
    if (mPausingActivity != NULL) {
        //Slogger::Wtf(CActivityManagerService::TAG, "Going to pause when pause is already pending for " + mPausingActivity);
        CompletePauseLocked(FALSE);
    }
    AutoPtr<ActivityRecord> prev = mResumedActivity;
    if (prev == NULL) {
        if (!resuming) {
            //Slogger::Wtf(CActivityManagerService::TAG, "Trying to pause when nothing is resumed");
            mStackSupervisor->ResumeTopActivitiesLocked();
        }
        return FALSE;
    }

    if (mActivityContainer->mParentActivity == NULL) {
        // Top level stack, not a child. Look for child stacks.
        mStackSupervisor->PauseChildStacks(prev, userLeaving, uiSleeping, resuming, dontWait);
    }

    if (ActivityStackSupervisor::DEBUG_STATES)
        Slogger::V(CActivityManagerService::TAG, "Moving to PAUSING: %s", prev->ToString().string());
    else if (CActivityManagerService::DEBUG_PAUSE)
        Slogger::V(CActivityManagerService::TAG, "Start pausing: %s", prev->ToString().string());
    mResumedActivity = NULL;
    mPausingActivity = prev;
    mLastPausedActivity = prev;
    Int32 flags;
    prev->mIntent->GetFlags(&flags);
    Int32 infoFlags;
    prev->mInfo->GetFlags(&infoFlags);
    mLastNoHistoryActivity = (flags & IIntent::FLAG_ACTIVITY_NO_HISTORY) != 0
            || (infoFlags & IActivityInfo::FLAG_NO_HISTORY) != 0 ? prev : NULL;
    prev->mState = ActivityState_PAUSING;
    prev->mTask->TouchActiveTime();
    ClearLaunchTime(prev);
    AutoPtr<ActivityRecord> next = mStackSupervisor->TopRunningActivityLocked();
    if (mService->mHasRecents && (next == NULL|| next->mNoDisplay || next->mTask != prev->mTask)) {
        prev->UpdateThumbnail(ScreenshotActivities(prev), NULL);
    }
    StopFullyDrawnTraceIfNeeded();

    mService->UpdateCpuStats();

    if (prev->mApp != NULL && prev->mApp->mThread != NULL) {
        if (CActivityManagerService::DEBUG_PAUSE)
            Slogger::V(CActivityManagerService::TAG, "Enqueueing pending pause: %s", prev->ToString().string());
        //try {
            //TODO EventLog.writeEvent(EventLogTags.AM_PAUSE_ACTIVITY, prev.userId, System.identityHashCode(prev), prev.shortComponentName);
            mService->UpdateUsageStats(prev, FALSE);
            prev->mApp->mThread->SchedulePauseActivity(IBinder::Probe(prev->mAppToken), prev->mFinishing,
                    userLeaving, prev->mConfigChangeFlags, dontWait);
        //} catch (Exception e) {
        //    // Ignore exception, if process died other code will cleanup.
        //    Slogger::W(CActivityManagerService::TAG, "Exception thrown during pause", e);
        //    mPausingActivity = NULL;
        //    mLastPausedActivity = NULL;
        //    mLastNoHistoryActivity = NULL;
        //}
    } else {
        mPausingActivity = NULL;
        mLastPausedActivity = NULL;
        mLastNoHistoryActivity = NULL;
    }

    // If we are not going to sleep, we want to ensure the device is
    // awake until the next activity is started.
    if (!mService->IsSleepingOrShuttingDown()) {
        mStackSupervisor->AcquireLaunchWakelock();
    }

    if (mPausingActivity != NULL) {
        // Have the window manager pause its key dispatching until the new
        // activity has started.  If we're pausing the activity just because
        // the screen is being turned off and the UI is sleeping, don't interrupt
        // key dispatch; the same activity will pick it up again on wakeup.
        if (!uiSleeping) {
            prev->PauseKeyDispatchingLocked();
        } else {
            if (CActivityManagerService::DEBUG_PAUSE)
                Slogger::V(CActivityManagerService::TAG, "Key dispatch not paused for screen off");
        }

        if (dontWait) {
            // If the caller said they don't want to wait for the pause, then complete
            // the pause now.
            CompletePauseLocked(FALSE);
            return FALSE;

        } else {
            // Schedule a pause timeout in case the app doesn't respond.
            // We don't give it much time because this directly impacts the
            // responsiveness seen by the user.
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(PAUSE_TIMEOUT_MSG, (IMessage**)&msg);
            msg->SetObj(TO_IINTERFACE(prev));
            prev->mPauseTime = SystemClock::GetUptimeMillis();
            Boolean result;
            mHandler->SendMessageDelayed(msg, PAUSE_TIMEOUT, &result);
            if (CActivityManagerService::DEBUG_PAUSE)
                Slogger::V(CActivityManagerService::TAG, "Waiting for pause to complete...");
            return TRUE;
        }

    } else {
        // This activity failed to schedule the
        // pause, so just treat it as being paused now.
        if (CActivityManagerService::DEBUG_PAUSE)
            Slogger::V(CActivityManagerService::TAG, "Activity not running, resuming next.");
        if (!resuming) {
            mStackSupervisor->GetFocusedStack()->ResumeTopActivityLocked(NULL);
        }
        return FALSE;
    }
    return FALSE;
}

void ActivityStack::ActivityPausedLocked(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean timeout)
{
    //if (CActivityManagerService::DEBUG_PAUSE)
    //  Slogger::V( CActivityManagerService::TAG, "Activity paused: token=" + token + ", timeout=" + timeout);

    AutoPtr<ActivityRecord> r = IsInStackLocked(token);
    if (r != NULL) {
        mHandler->RemoveMessages(PAUSE_TIMEOUT_MSG, TO_IINTERFACE(r));
        if (mPausingActivity == r) {
            //if (ActivityStackSupervisor::DEBUG_STATES) Slog.v(CActivityManagerService::TAG, "Moving to PAUSED: " + r + (timeout ? " (due to timeout)" : " (pause complete)"));
            CompletePauseLocked(TRUE);
        } else {
            //TODO EventLog.writeEvent(EventLogTags.AM_FAILED_TO_PAUSE,
            //        r.userId, System.identityHashCode(r), r.shortComponentName,
            //        mPausingActivity != NULL
            //            ? mPausingActivity.shortComponentName : "(none)");
        }
    }
}

void ActivityStack::ActivityStoppedLocked(
    /* [in] */ ActivityRecord* r,
    /* [in] */ IBundle* icicle,
    /* [in] */ IPersistableBundle* persistentState,
    /* [in] */ ICharSequence* description)
{
    if (r->mState != ActivityState_STOPPING) {
        //Slog.i(CActivityManagerService::TAG, "Activity reported stop, but no longer stopping: " + r);
        mHandler->RemoveMessages(STOP_TIMEOUT_MSG, TO_IINTERFACE(r));
        return;
    }
    if (persistentState != NULL) {
        r->mPersistentState = persistentState;
        mService->NotifyTaskPersisterLocked(r->mTask, FALSE);
    }
    //if (ActivityStackSupervisor::DEBUG_SAVED_STATE) Slog.i(CActivityManagerService::TAG, "Saving icicle of " + r + ": " + icicle);
    if (icicle != NULL) {
        // If icicle is NULL, this is happening due to a timeout, so we
        // haven't really saved the state.
        r->mIcicle = icicle;
        r->mHaveState = TRUE;
        r->mLaunchCount = 0;
        r->UpdateThumbnail(NULL, description);
    }
    if (!r->mStopped) {
        //if (ActivityStackSupervisor::DEBUG_STATES) Slog.v(CActivityManagerService::TAG, "Moving to STOPPED: " + r + " (stop complete)");
        mHandler->RemoveMessages(STOP_TIMEOUT_MSG, TO_IINTERFACE(r));
        r->mStopped = TRUE;
        r->mState = ActivityState_STOPPED;
        if (mActivityContainer->mActivityDisplay->mVisibleBehindActivity.Get() == r) {
            mStackSupervisor->RequestVisibleBehindLocked(r, FALSE);
        }
        if (r->mFinishing) {
            r->ClearOptionsLocked();
        } else {
            if (r->mConfigDestroy) {
                DestroyActivityLocked(r, TRUE, String("stop-config"));
                mStackSupervisor->ResumeTopActivitiesLocked();
            } else {
                mStackSupervisor->UpdatePreviousProcessLocked(r);
            }
        }
    }
}

AutoPtr<ActivityRecord> ActivityStack::FindNextTranslucentActivity(
    /* [in] */ ActivityRecord* r)
{
    AutoPtr<TaskRecord> task = r->mTask;
    if (task == NULL) {
        return NULL;
    }

    AutoPtr<ActivityStack> stack = task->mStack;
    if (stack == NULL) {
        return NULL;
    }

    Int32 stackNdx;
    mStacks->IndexOf(TO_IINTERFACE(stack), &stackNdx);

    AutoPtr<IArrayList> tasks = stack->mTaskHistory;
    Int32 taskNdx;
    tasks->IndexOf(TO_IINTERFACE(task), &taskNdx);

    //ArrayList<ActivityRecord> activities = task.mActivities;
    AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
    //activities.indexOf(r) + 1;
    Int32 activityNdx = -1;//IndexOf
    for(UInt32 i = 0; i < activities->GetSize(); ++i) {
        if (((*activities)[i]).Get() == r) {
            activityNdx = i;
            break;
        }
    }
    ++activityNdx;

    Int32 numStacks;
    mStacks->GetSize(&numStacks);
    while (stackNdx < numStacks) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        ActivityStack* as = (ActivityStack*)(IObject::Probe(obj));
        tasks = as->mTaskHistory;
        Int32 numTasks;
        tasks->GetSize(&numTasks);
        while (taskNdx < numTasks) {
            AutoPtr<IInterface> taskobj;
            tasks->Get(taskNdx, (IInterface**)&taskobj);
            //activities = tasks.get(taskNdx).mActivities;
            activities = ((TaskRecord*)(IObject::Probe(taskobj)))->mActivities;
            Int32 numActivities = activities->GetSize();
            while (activityNdx < numActivities) {
                AutoPtr<ActivityRecord> activity = (*activities)[activityNdx];
                if (!activity->mFinishing) {
                    return activity->mFullscreen ? NULL: activity;
                }
                ++activityNdx;
            }
            activityNdx = 0;
            ++taskNdx;
        }
        taskNdx = 0;
        ++stackNdx;
    }

    return NULL;
}

void ActivityStack::EnsureActivitiesVisibleLocked(
    /* [in] */ ActivityRecord* starting,
    /* [in] */ Int32 configChanges)
{
    AutoPtr<ActivityRecord> top = TopRunningActivityLocked(NULL);
    if (top == NULL) {
        return;
    }
    //if (CActivityManagerService::DEBUG_VISBILITY) Slog.v(
    //        CActivityManagerService::TAG, "ensureActivitiesVisible behind " + top
    //        + " configChanges=0x" + Integer.toHexString(configChanges));

    if (mTranslucentActivityWaiting != top) {
        mUndrawnActivitiesBelowTopTranslucent->Clear();
        if (mTranslucentActivityWaiting != NULL) {
            // Call the callback with a timeout indication.
            NotifyActivityDrawnLocked(NULL);
            mTranslucentActivityWaiting = NULL;
        }
        mHandler->RemoveMessages(TRANSLUCENT_TIMEOUT_MSG);
    }

    // If the top activity is not fullscreen, then we need to
    // make sure any activities under it are now visible.
    Boolean aboveTop = TRUE;
    Boolean behindFullscreen = !IsStackVisible();

    Int32 taskSize;
    mTaskHistory->GetSize(&taskSize);
    for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        //ArrayList<ActivityRecord> activities = task.mActivities;
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            AutoPtr<ActivityRecord> r = (*activities)[activityNdx];
            if (r->mFinishing) {
                continue;
            }
            if (aboveTop && r != top) {
                continue;
            }
            aboveTop = FALSE;
            // mLaunchingBehind: Activities launching behind are at the back of the task stack
            // but must be drawn initially for the animation as though they were visible.
            if (!behindFullscreen || r->mLaunchTaskBehind) {
                //if (CActivityManagerService::DEBUG_VISBILITY) Slog.v(
                //        CActivityManagerService::TAG, "Make visible? " + r + " finishing=" + r.finishing
                //        + " state=" + r.state);

                // First: if this is not the current activity being started, make
                // sure it matches the current configuration.
                if (r.Get() != starting) {
                    EnsureActivityConfigurationLocked(r, 0);
                }

                if (r->mApp == NULL || r->mApp->mThread == NULL) {
                    // This activity needs to be visible, but isn't even
                    // running...  get it started, but don't resume it
                    // at this point.
                    //if (CActivityManagerService::DEBUG_VISBILITY)
                    //  Slog.v(CActivityManagerService::TAG, "Start and freeze screen for " + r);
                    if (r.Get() != starting) {
                        r->StartFreezingScreenLocked(r->mApp, configChanges);
                    }
                    if (!r->mVisible || r->mLaunchTaskBehind) {
                        //if (CActivityManagerService::DEBUG_VISBILITY)
                        //  Slog.v(CActivityManagerService::TAG, "Starting and making visible: " + r);
                        SetVisibile(r, TRUE);
                    }
                    if (r.Get() != starting) {
                        mStackSupervisor->StartSpecificActivityLocked(r, FALSE, FALSE);
                    }

                } else if (r->mVisible) {
                    // If this activity is already visible, then there is nothing
                    // else to do here.
                    //if (CActivityManagerService::DEBUG_VISBILITY)
                    //  Slog.v(CActivityManagerService::TAG, "Skipping: already visible at " + r);
                    r->StopFreezingScreenLocked(FALSE);
                    //try {
                    if (r->mReturningOptions != NULL) {
                        r->mApp->mThread->ScheduleOnNewActivityOptions(IBinder::Probe(r->mAppToken),
                                    r->mReturningOptions);
                    }
                    //} catch(RemoteException e) {
                    //}
                } else {
                    // This activity is not currently visible, but is running.
                    // Tell it to become visible.
                    r->mVisible = TRUE;
                    if (r->mState != ActivityState_RESUMED && r.Get() != starting) {
                        // If this activity is paused, tell it
                        // to now show its window.
                        //if (CActivityManagerService::DEBUG_VISBILITY)
                        //  Slog.v(CActivityManagerService::TAG, "Making visible and scheduling visibility: " + r);
                        //try {
                        if (mTranslucentActivityWaiting != NULL) {
                            r->UpdateOptionsLocked(r->mReturningOptions);
                            mUndrawnActivitiesBelowTopTranslucent->Add(TO_IINTERFACE(r));
                        }
                        SetVisibile(r, TRUE);
                        r->mSleeping = FALSE;
                        r->mApp->mPendingUiClean = TRUE;
                        r->mApp->mThread->ScheduleWindowVisibility(IBinder::Probe(r->mAppToken), TRUE);
                        r->StopFreezingScreenLocked(FALSE);
                        //} catch (Exception e) {
                        //    // Just skip on any failure; we'll make it
                        //    // visible when it next restarts.
                        //    Slogger::W(CActivityManagerService::TAG, "Exception thrown making visibile: "
                        //            + r.intent.getComponent(), e);
                        //}
                    }
                }

                // Aggregate current change flags.
                configChanges |= r->mConfigChangeFlags;

                if (r->mFullscreen) {
                    // At this point, nothing else needs to be shown
                    //if (CActivityManagerService::DEBUG_VISBILITY)
                    //  Slog.v(CActivityManagerService::TAG, "Fullscreen: at " + r);
                    behindFullscreen = TRUE;
                } else if (!IsHomeStack() && r->mFrontOfTask && task->IsOverHomeStack()) {
                    //if (CActivityManagerService::DEBUG_VISBILITY)
                    //  Slog.v(CActivityManagerService::TAG, "Showing home: at " + r);
                    behindFullscreen = TRUE;
                }
            } else {
                //if (CActivityManagerService::DEBUG_VISBILITY)
                //  Slog.v(
                //    CActivityManagerService::TAG, "Make invisible? " + r + " finishing=" + r.finishing
                //    + " state=" + r.state
                //    + " behindFullscreen=" + behindFullscreen);
                // Now for any activities that aren't visible to the user, make
                // sure they no longer are keeping the screen frozen.
                if (r->mVisible) {
                    //if (CActivityManagerService::DEBUG_VISBILITY)
                    //  Slog.v(CActivityManagerService::TAG, "Making invisible: " + r);
                    //try {
                    SetVisibile(r, FALSE);
                    switch (r->mState) {
                        case ActivityState_STOPPING:
                        case ActivityState_STOPPED:
                            if (r->mApp != NULL && r->mApp->mThread != NULL) {
                                //if (CActivityManagerService::DEBUG_VISBILITY)
                                //  Slog.v(CActivityManagerService::TAG, "Scheduling invisibility: " + r);
                                r->mApp->mThread->ScheduleWindowVisibility(IBinder::Probe(r->mAppToken), FALSE);
                            }
                            break;

                        case ActivityState_INITIALIZING:
                        case ActivityState_RESUMED:
                        case ActivityState_PAUSING:
                        case ActivityState_PAUSED:
                            // This case created for transitioning activities from
                            // translucent to opaque {@link Activity#convertToOpaque}.
                            if (GetVisibleBehindActivity() == r) {
                                ReleaseBackgroundResources();
                            } else {
                                Boolean result;
                                mStackSupervisor->mStoppingActivities->Contains(TO_IINTERFACE(r), &result);
                                if (!result) {
                                    mStackSupervisor->mStoppingActivities->Add(TO_IINTERFACE(r));
                                }
                                mStackSupervisor->ScheduleIdleLocked();
                            }
                            break;

                        default:
                            break;
                    }
                    //} catch (Exception e) {
                    //    // Just skip on any failure; we'll make it
                    //    // visible when it next restarts.
                    //    Slogger::W(CActivityManagerService::TAG, "Exception thrown making hidden: "
                    //            + r.intent.getComponent(), e);
                    //}
                } else {
                    if (CActivityManagerService::DEBUG_VISBILITY)
                        Slogger::V(CActivityManagerService::TAG, "Already invisible: %s", r->ToString().string());
                }
            }
        }
    }

    Boolean isEmpty = FALSE;
    if (mTranslucentActivityWaiting != NULL &&
            (mUndrawnActivitiesBelowTopTranslucent->IsEmpty(&isEmpty), isEmpty)) {
        // Nothing is getting drawn or everything was already visible, don't wait for timeout.
        NotifyActivityDrawnLocked(NULL);
    }
}

void ActivityStack::ConvertToTranslucent(
    /* [in] */ ActivityRecord* r)
{
    mTranslucentActivityWaiting = r;
    mUndrawnActivitiesBelowTopTranslucent->Clear();
    Boolean res;
    mHandler->SendEmptyMessageDelayed(TRANSLUCENT_TIMEOUT_MSG, TRANSLUCENT_CONVERSION_TIMEOUT, &res);
}

void ActivityStack::NotifyActivityDrawnLocked(
    /* [in] */ ActivityRecord* r)
{
    mActivityContainer->SetDrawn();
    if (r == NULL){
        Boolean remove, isEmpty;
        mUndrawnActivitiesBelowTopTranslucent->Remove(TO_IINTERFACE(r), &remove);
        mUndrawnActivitiesBelowTopTranslucent->IsEmpty(&isEmpty);
        if(remove && isEmpty) {
            // The last undrawn activity below the top has just been drawn. If there is an
            // opaque activity at the top, notify it that it can become translucent safely now.
            AutoPtr<ActivityRecord> waitingActivity = mTranslucentActivityWaiting;
            mTranslucentActivityWaiting = NULL;
            mUndrawnActivitiesBelowTopTranslucent->Clear();
            mHandler->RemoveMessages(TRANSLUCENT_TIMEOUT_MSG);

            if (waitingActivity != NULL) {
                //TODO mWindowManager->SetWindowOpaque(IBinder::Probe(waitingActivity->mAppToken), FALSE);
                if (waitingActivity->mApp != NULL && waitingActivity->mApp->mThread != NULL) {
                    //try {
                    waitingActivity->mApp->mThread->ScheduleTranslucentConversionComplete(
                                IBinder::Probe(waitingActivity->mAppToken), r != NULL);
                    //} catch (RemoteException e) {
                    //}
                }
            }
        }
    }
}

void ActivityStack::CancelInitializingActivities()
{
    AutoPtr<ActivityRecord> topActivity = TopRunningActivityLocked(NULL);
    Boolean aboveTop = TRUE;
    Int32 taskSize;
    mTaskHistory->GetSize(&taskSize);
    for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
        //final ArrayList<ActivityRecord> activities = mTaskHistory.get(taskNdx).mActivities;
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            //final ActivityRecord r = activities.get(activityNdx);
            AutoPtr<ActivityRecord> r = (*activities)[activityNdx];
            if (aboveTop) {
                if (r == topActivity) {
                    aboveTop = FALSE;
                }
                continue;
            }

            if (r->mState == ActivityState_INITIALIZING && r->mStartingWindowShown) {
                if (CActivityManagerService::DEBUG_VISBILITY)
                    Slogger::W(CActivityManagerService::TAG, "Found orphaned starting window %s", r->ToString().string());
                r->mStartingWindowShown = FALSE;
                //TODO mWindowManager->RemoveAppStartingWindow(IBinder::Probe(r->mAppToken));
            }
        }
    }
}

Boolean ActivityStack::ResumeTopActivityLocked(
    /* [in] */ ActivityRecord* prev)
{
    return ResumeTopActivityLocked(prev, NULL);
}

Boolean ActivityStack::ResumeTopActivityLocked(
    /* [in] */ ActivityRecord* prev,
    /* [in] */ IBundle* options)
{
    if (mInResumeTopActivity) {
        // Don't even start recursing.
        return FALSE;
    }

    Boolean result = FALSE;
    //try {
    // Protect against recursion.
    mInResumeTopActivity = TRUE;
    result = ResumeTopActivityInnerLocked(prev, options);
    //} finally {
    //    mInResumeTopActivity = false;
    //}
    return result;
}

Boolean ActivityStack::ResumeTopActivityInnerLocked(
    /* [in] */ ActivityRecord* prev,
    /* [in] */ IBundle* options)
{
    if (CActivityManagerService::DEBUG_LOCKSCREEN) mService->LogLockScreen(String(""));

    if (!mService->mBooting && !mService->mBooted) {
        // Not ready yet!
        return FALSE;
    }

    AutoPtr<ActivityRecord> parent = mActivityContainer->mParentActivity;
    if ((parent != NULL && parent->mState != ActivityState_RESUMED) ||
            !mActivityContainer->IsAttachedLocked()) {
        // Do not resume this stack if its parent is not resumed.
        // TODO: If in a loop, make sure that parent stack resumeTopActivity is called 1st.
        return FALSE;
    }

    CancelInitializingActivities();

    // Find the first activity that is not finishing.
    AutoPtr<ActivityRecord> next = TopRunningActivityLocked(NULL);

    // Remember how we'll process this pause/resume situation, and ensure
    // that the state is reset however we wind up proceeding.
    Boolean userLeaving = mStackSupervisor->mUserLeaving;
    mStackSupervisor->mUserLeaving = FALSE;

    AutoPtr<TaskRecord> prevTask = prev != NULL ? prev->mTask : NULL;
    AutoPtr<IActivityOptionsHelper> aoHelper;
    CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoHelper);
    if (next == NULL) {
        // There are no more activities!  Let's just start up the
        // Launcher...
        aoHelper->Abort(options);
        if (ActivityStackSupervisor::DEBUG_STATES)
            Slogger::D(CActivityManagerService::TAG, "resumeTopActivityLocked: No more activities go home");
        if (CActivityManagerService::DEBUG_STACK)
            mStackSupervisor->ValidateTopActivitiesLocked();
        // Only resume home if on home display
        Int32 returnTaskType = prevTask == NULL || !prevTask->IsOverHomeStack() ?
            ActivityRecord::HOME_ACTIVITY_TYPE : prevTask->GetTaskToReturnTo();
        return IsOnHomeDisplay() &&
            mStackSupervisor->ResumeHomeStackTask(returnTaskType, prev);
    }

    next->mDelayedResume = FALSE;

    // If the top activity is the resumed one, nothing to do.
    if (mResumedActivity == next && next->mState == ActivityState_RESUMED &&
            mStackSupervisor->AllResumedActivitiesComplete()) {
        // Make sure we have executed any pending transitions, since there
        // should be nothing left to do at this point.
        mWindowManager->ExecuteAppTransition();
        mNoAnimActivities->Clear();
        aoHelper->Abort(options);
        if (ActivityStackSupervisor::DEBUG_STATES)
            Slogger::D(CActivityManagerService::TAG, "resumeTopActivityLocked: Top activity resumed %s", next->ToString().string());
        if (CActivityManagerService::DEBUG_STACK)
            mStackSupervisor->ValidateTopActivitiesLocked();

        // Make sure to notify Keyguard as well if it is waiting for an activity to be drawn.
        mStackSupervisor->NotifyActivityDrawnForKeyguard();
        return FALSE;
    }

    AutoPtr<TaskRecord> nextTask = next->mTask;
    if (prevTask != NULL && prevTask->mStack == this &&
            prevTask->IsOverHomeStack() && prev->mFinishing && prev->mFrontOfTask) {
        if (CActivityManagerService::DEBUG_STACK)
            mStackSupervisor->ValidateTopActivitiesLocked();
        if (prevTask == nextTask) {
            prevTask->SetFrontOfTask();
        } else if (prevTask != TopTask()) {
            // This task is going away but it was supposed to return to the home stack.
            // Now the task above it has to return to the home task instead.
            Int32 taskNdx = 0;
            mTaskHistory->IndexOf(TO_IINTERFACE(prevTask), &taskNdx);
            ++taskNdx;
            AutoPtr<IInterface> taskobj;
            mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
            ((TaskRecord*)(IObject::Probe(taskobj)))->SetTaskToReturnTo(ActivityRecord::HOME_ACTIVITY_TYPE);
        } else {
            if (ActivityStackSupervisor::DEBUG_STATES && IsOnHomeDisplay())
                Slogger::D(CActivityManagerService::TAG, "resumeTopActivityLocked: Launching home next");
            // Only resume home if on home display
            Int32 returnTaskType = prevTask == NULL || !prevTask->IsOverHomeStack() ?
                ActivityRecord::HOME_ACTIVITY_TYPE : prevTask->GetTaskToReturnTo();
            return IsOnHomeDisplay() &&
                mStackSupervisor->ResumeHomeStackTask(returnTaskType, prev);
        }
    }

    // If we are sleeping, and there is no resumed activity, and the top
    // activity is paused, well that is the state we want.
    if (mService->IsSleepingOrShuttingDown()
            && mLastPausedActivity == next
            && mStackSupervisor->AllPausedActivitiesComplete()) {
        // Make sure we have executed any pending transitions, since there
        // should be nothing left to do at this point.
        mWindowManager->ExecuteAppTransition();
        mNoAnimActivities->Clear();
        aoHelper->Abort(options);
        if (ActivityStackSupervisor::DEBUG_STATES) Slogger::D(CActivityManagerService::TAG, "resumeTopActivityLocked: Going to sleep and all paused");
        if (CActivityManagerService::DEBUG_STACK)
            mStackSupervisor->ValidateTopActivitiesLocked();
        return FALSE;
    }

    // Make sure that the user who owns this activity is started.  If not,
    // we will just leave it as is because someone should be bringing
    // another user's activities to the top of the stack.
    if (mService->mStartedUsers[next->mUserId] == NULL) {
        //Slogger::W(CActivityManagerService::TAG, "Skipping resume of top activity " + next
        //        + ": user " + next.userId + " is stopped");
        if (CActivityManagerService::DEBUG_STACK)
            mStackSupervisor->ValidateTopActivitiesLocked();
        return FALSE;
    }

    // The activity may be waiting for stop, but that is no longer
    // appropriate for it.
    mStackSupervisor->mStoppingActivities->Remove(TO_IINTERFACE(next));
    mStackSupervisor->mGoingToSleepActivities->Remove(TO_IINTERFACE(next));
    next->mSleeping = FALSE;
    mStackSupervisor->mWaitingVisibleActivities->Remove(TO_IINTERFACE(next));

    if (CActivityManagerService::DEBUG_SWITCH)
        Slogger::V(CActivityManagerService::TAG, "Resuming %s", next->ToString().string());

    // If we are currently pausing an activity, then don't do anything
    // until that is done.
    if (!mStackSupervisor->AllPausedActivitiesComplete()) {
        if (CActivityManagerService::DEBUG_SWITCH || CActivityManagerService::DEBUG_PAUSE || ActivityStackSupervisor::DEBUG_STATES)
            Slogger::V(CActivityManagerService::TAG,
                "resumeTopActivityLocked: Skip resume: some activity pausing.");
        if (CActivityManagerService::DEBUG_STACK)
            mStackSupervisor->ValidateTopActivitiesLocked();
        return FALSE;
    }

    // Okay we are now going to start a switch, to 'next'.  We may first
    // have to pause the current activity, but this is an important point
    // where we have decided to go to 'next' so keep track of that.
    // XXX "App Redirected" dialog is getting too many false positives
    // at this point, so turn off for now.
    //if (FALSE) {
    //    if (mLastStartedActivity != NULL && !mLastStartedActivity.finishing) {
    //        long now = SystemClock.uptimeMillis();
    //        final boolean inTime = mLastStartedActivity.startTime != 0
    //            && (mLastStartedActivity.startTime + START_WARN_TIME) >= now;
    //        final int lastUid = mLastStartedActivity.info.applicationInfo.uid;
    //        final int nextUid = next.info.applicationInfo.uid;
    //        if (inTime && lastUid != nextUid
    //                && lastUid != next.launchedFromUid
    //                && mService.checkPermission(
    //                    android.Manifest.permission.STOP_APP_SWITCHES,
    //                    -1, next.launchedFromUid)
    //                != PackageManager.PERMISSION_GRANTED) {
    //            mService.showLaunchWarningLocked(mLastStartedActivity, next);
    //        } else {
    //            next.startTime = now;
    //            mLastStartedActivity = next;
    //        }
    //    } else {
    //        next.startTime = SystemClock.uptimeMillis();
    //        mLastStartedActivity = next;
    //    }
    //}

    // We need to start pausing the current activity so the top one
    // can be resumed...
    Int32 infoFlags;
    next->mInfo->GetFlags(&infoFlags);
    Boolean dontWaitForPause = (infoFlags&IActivityInfo::FLAG_RESUME_WHILE_PAUSING) != 0;
    Boolean pausing = mStackSupervisor->PauseBackStacks(userLeaving, TRUE, dontWaitForPause);
    if (mResumedActivity != NULL) {
        pausing |= StartPausingLocked(userLeaving, FALSE, TRUE, dontWaitForPause);
        if (ActivityStackSupervisor::DEBUG_STATES) Slogger::D(CActivityManagerService::TAG, "resumeTopActivityLocked: Pausing %s", mResumedActivity->ToString().string());
    }
    if (pausing) {
        if (CActivityManagerService::DEBUG_SWITCH || ActivityStackSupervisor::DEBUG_STATES)
            Slogger::V(CActivityManagerService::TAG,
                "resumeTopActivityLocked: Skip resume: need to start pausing");
        // At this point we want to put the upcoming activity's process
        // at the top of the LRU list, since we know we will be needing it
        // very soon and it would be a waste to let it get killed if it
        // happens to be sitting towards the end.
        if (next->mApp != NULL && next->mApp->mThread != NULL) {
            mService->UpdateLruProcessLocked(next->mApp, TRUE, NULL);
        }
        if (CActivityManagerService::DEBUG_STACK)
            mStackSupervisor->ValidateTopActivitiesLocked();
        return TRUE;
    }

    // If the most recent activity was noHistory but was only stopped rather
    // than stopped+finished because the device went to sleep, we need to make
    // sure to finish it as we're making a new activity topmost.
    if (mService->IsSleeping() && mLastNoHistoryActivity != NULL &&
            !mLastNoHistoryActivity->mFinishing) {
        if (ActivityStackSupervisor::DEBUG_STATES) Slogger::D(CActivityManagerService::TAG, "no-history finish of %s on new resume", mLastNoHistoryActivity->ToString().string());
        RequestFinishActivityLocked(IBinder::Probe(mLastNoHistoryActivity->mAppToken), IActivity::RESULT_CANCELED,
                NULL, String("no-history"), FALSE);
        mLastNoHistoryActivity = NULL;
    }

    if (prev != NULL && prev != next) {
        if (!prev->mWaitingVisible && next != NULL && !next->mNowVisible) {
            prev->mWaitingVisible = TRUE;
            mStackSupervisor->mWaitingVisibleActivities->Add(TO_IINTERFACE(prev));
            if (CActivityManagerService::DEBUG_SWITCH) Slogger::V(
                    CActivityManagerService::TAG, "Resuming top, waiting visible to hide: %s", prev->ToString().string());
        } else {
            // The next activity is already visible, so hide the previous
            // activity's windows right now so we can show the new one ASAP.
            // We only do this if the previous is finishing, which should mean
            // it is on top of the one being resumed so hiding it quickly
            // is good.  Otherwise, we want to do the normal route of allowing
            // the resumed activity to be shown so we can decide if the
            // previous should actually be hidden depending on whether the
            // new one is found to be full-screen or not.
            if (prev->mFinishing) {
                //TODO mWindowManager->SetAppVisibility(IBinder::Probe(prev->mAppToken), FALSE);
                if (CActivityManagerService::DEBUG_SWITCH)
                    Slogger::V(CActivityManagerService::TAG, "Not waiting for visible to hide: ");
                //        + prev + ", waitingVisible="
                //        + (prev != NULL ? prev.waitingVisible : NULL)
                //        + ", nowVisible=" + next.nowVisible);
            } else {
                if (CActivityManagerService::DEBUG_SWITCH)
                    Slogger::V(CActivityManagerService::TAG, "Previous already visible but still waiting to hide: ");
                //        + prev + ", waitingVisible="
                //        + (prev != NULL ? prev.waitingVisible : NULL)
                //        + ", nowVisible=" + next.nowVisible);
            }
        }
    }

    // Launching this app's activity, make sure the app is no longer
    // considered stopped.
    //try {
    AppGlobals::GetPackageManager()->SetPackageStoppedState(
                next->mPackageName, FALSE, next->mUserId); /* TODO: Verify if correct userid */
    //} catch (RemoteException e1) {
    //} catch (IllegalArgumentException e) {
    //    Slogger::W(CActivityManagerService::TAG, "Failed trying to unstop package "
    //            + next.packageName + ": " + e);
    //}

    // We are starting up the next activity, so tell the window manager
    // that the previous one will be hidden soon.  This way it can know
    // to ignore it when computing the desired screen orientation.
    Boolean anim = true;
    if (prev != NULL) {
        if (prev->mFinishing) {
            if (CActivityManagerService::DEBUG_TRANSITION)
                Slogger::V(CActivityManagerService::TAG,
                    "Prepare close transition: prev=%s", prev->ToString().string());
            Boolean bTemp = FALSE;
            if (mNoAnimActivities->Contains(TO_IINTERFACE(prev), &bTemp), bTemp) {
                anim = FALSE;
                mWindowManager->PrepareAppTransition(AppTransition::TRANSIT_NONE, FALSE);
            } else {
                mWindowManager->PrepareAppTransition(prev->mTask == next->mTask
                       ? AppTransition::TRANSIT_ACTIVITY_CLOSE
                       :  AppTransition::TRANSIT_TASK_CLOSE, FALSE);
            }
            mWindowManager->SetAppWillBeHidden(IBinder::Probe(prev->mAppToken));
            mWindowManager->SetAppVisibility(IBinder::Probe(prev->mAppToken), FALSE);
        } else {
            if (CActivityManagerService::DEBUG_TRANSITION)
                Slogger::V(CActivityManagerService::TAG, "Prepare open transition: prev=%s", prev->ToString().string());
            Boolean bTemp;
            if (mNoAnimActivities->Contains(TO_IINTERFACE(next), &bTemp), bTemp) {
                anim = FALSE;
                mWindowManager->PrepareAppTransition(AppTransition::TRANSIT_NONE, FALSE);
            } else {
                mWindowManager->PrepareAppTransition(prev->mTask == next->mTask
                       ? AppTransition::TRANSIT_ACTIVITY_OPEN
                       : next->mLaunchTaskBehind
                       ? AppTransition::TRANSIT_TASK_OPEN_BEHIND
                       : AppTransition::TRANSIT_TASK_OPEN, FALSE);
            }
        }
        if (FALSE) {
            mWindowManager->SetAppWillBeHidden(IBinder::Probe(prev->mAppToken));
            mWindowManager->SetAppVisibility(IBinder::Probe(prev->mAppToken), FALSE);
        }
    } else {
        if (CActivityManagerService::DEBUG_TRANSITION)
            Slogger::V(CActivityManagerService::TAG, "Prepare open transition: no previous");
        Boolean bTemp;
        if (mNoAnimActivities->Contains(TO_IINTERFACE(next), &bTemp), bTemp) {
            anim = FALSE;
            mWindowManager->PrepareAppTransition(AppTransition::TRANSIT_NONE, FALSE);
        } else {
            mWindowManager->PrepareAppTransition(AppTransition::TRANSIT_ACTIVITY_OPEN, FALSE);
        }
    }

    AutoPtr<IBundle> resumeAnimOptions = NULL;
    if (anim) {
        AutoPtr<IActivityOptions> opts = next->GetOptionsForTargetActivityLocked();
        if (opts != NULL) {
            //resumeAnimOptions = opts.toBundle();
            opts->ToBundle((IBundle**)&resumeAnimOptions);
        }
        next->ApplyOptionsLocked();
    } else {
        next->ClearOptionsLocked();
    }

    AutoPtr<ActivityStack> lastStack = mStackSupervisor->GetLastStack();
    if (next->mApp != NULL && next->mApp->mThread != NULL) {
        if (CActivityManagerService::DEBUG_SWITCH)
            Slogger::V(CActivityManagerService::TAG, "Resume running: %s", next->ToString().string());

        // This activity is now becoming visible.
        mWindowManager->SetAppVisibility(IBinder::Probe(next->mAppToken), TRUE);

        // schedule launch ticks to collect information about slow apps.
        next->StartLaunchTickingLocked();

        AutoPtr<ActivityRecord> lastResumedActivity =
            lastStack == NULL ? NULL :lastStack->mResumedActivity;
        ActivityState lastState = next->mState;

        mService->UpdateCpuStats();

        if (ActivityStackSupervisor::DEBUG_STATES) Slogger::V(CActivityManagerService::TAG, "Moving to RESUMED: %s (in existing)", next->ToString().string());
        next->mState = ActivityState_RESUMED;
        mResumedActivity = next;
        next->mTask->TouchActiveTime();
        mService->AddRecentTaskLocked(next->mTask);
        mService->UpdateLruProcessLocked(next->mApp, TRUE, NULL);
        UpdateLRUListLocked(next);
        mService->UpdateOomAdjLocked();

        // Have the window manager re-evaluate the orientation of
        // the screen based on the new activity order.
        Boolean notUpdated = TRUE;
        if (mStackSupervisor->IsFrontStack(this)) {
            AutoPtr<IConfiguration> config;
            mWindowManager->UpdateOrientationFromAppTokens(
                   mService->mConfiguration,
                   next->MayFreezeScreenLocked(next->mApp) ? IBinder::Probe(next->mAppToken) : NULL,
                   (IConfiguration**)&config);
            if (config != NULL) {
                next->mFrozenBeforeDestroy = TRUE;
            }
            notUpdated = !mService->UpdateConfigurationLocked(config, next, FALSE, FALSE);
        }

        if (notUpdated) {
            // The configuration update wasn't able to keep the existing
            // instance of the activity, and instead started a new one.
            // We should be all done, but let's just make sure our activity
            // is still at the top and schedule another run if something
            // weird happened.
            AutoPtr<ActivityRecord> nextNext = TopRunningActivityLocked(NULL);
            //if (CActivityManagerService::DEBUG_SWITCH || ActivityStackSupervisor::DEBUG_STATES)
            //  Slog.i(CActivityManagerService::TAG,
            //        "Activity config changed during resume: " + next
            //        + ", new next: " + nextNext);
            if (nextNext != next) {
                // Do over!
                mStackSupervisor->ScheduleResumeTopActivities();
            }
            if (mStackSupervisor->ReportResumedActivityLocked(next)) {
                mNoAnimActivities->Clear();
                if (CActivityManagerService::DEBUG_STACK)
                    mStackSupervisor->ValidateTopActivitiesLocked();
                return TRUE;
            }
            if (CActivityManagerService::DEBUG_STACK)
                mStackSupervisor->ValidateTopActivitiesLocked();
            return FALSE;
        }

        //try {
            // Deliver all pending results.
            //ArrayList<ResultInfo> a = next.results;
            AutoPtr< List< AutoPtr<ActivityResult> > > a = next->mResults;
            if (a != NULL) {
                Int32 N = a->GetSize();
                if (!next->mFinishing && N > 0) {
                    //if (CActivityManagerService::DEBUG_RESULTS) Slog.v(
                    //        CActivityManagerService::TAG, "Delivering results to " + next
                    //        + ": " + a);
                    AutoPtr<IList> list;
                    CArrayList::New((IList**)&list);
                    for (UInt32 i = 0; i < a->GetSize(); ++i) {
                        AutoPtr<ActivityResult> ar = (*a)[i];
                        list->Add(TO_IINTERFACE(ar));
                    }
                    next->mApp->mThread->ScheduleSendResult(IBinder::Probe(next->mAppToken), list);
                }
            }

            if (next->mNewIntents != NULL) {
                AutoPtr<IList> list;
                CArrayList::New((IList**)&list);
                for (UInt32 i = 0; i < next->mNewIntents->GetSize(); ++i) {
                    AutoPtr<IIntent> intent = (*(next->mNewIntents))[i];
                    list->Add(TO_IINTERFACE(intent));
                }
                next->mApp->mThread->ScheduleNewIntent(list, IBinder::Probe(next->mAppToken));
            }

            //EventLog.writeEvent(EventLogTags.AM_RESUME_ACTIVITY,
            //        next.userId, System.identityHashCode(next),
            //        next.task.taskId, next.shortComponentName);

            next->mSleeping = FALSE;
            mService->ShowAskCompatModeDialogLocked(next);
            next->mApp->mPendingUiClean = TRUE;
            next->mApp->ForceProcessStateUpTo(IActivityManager::PROCESS_STATE_TOP);
            next->ClearOptionsLocked();
            next->mApp->mThread->ScheduleResumeActivity(IBinder::Probe(next->mAppToken), next->mApp->mRepProcState,
                    mService->IsNextTransitionForward(), resumeAnimOptions);

            mStackSupervisor->CheckReadyForSleepLocked();

            if (ActivityStackSupervisor::DEBUG_STATES) Slogger::D(CActivityManagerService::TAG, "resumeTopActivityLocked: Resumed %s", next->ToString().string());
            assert(lastState);//TODO remove
        //TODO } catch (Exception e) {
        //    // Whoops, need to restart this activity!
        //    if (ActivityStackSupervisor::DEBUG_STATES) Slog.v(CActivityManagerService::TAG, "Resume failed; resetting state to "
        //            + lastState + ": " + next);
        //    next->mState = lastState;
        //    if (lastStack != NULL) {
        //        lastStack->mResumedActivity = lastResumedActivity;
        //    }
        //    Slog.i(CActivityManagerService::TAG, "Restarting because process died: " + next);
        //    if (!next->mHasBeenLaunched) {
        //        next->mHasBeenLaunched = TRUE;
        //    } else  if (SHOW_APP_STARTING_PREVIEW && lastStack != NULL &&
        //            mStackSupervisor->IsFrontStack(lastStack)) {
        //        AutoPtr<IApplicationInfo> appInfo;
        //        IComponentInfo::Probe(next->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        //        mWindowManager->SetAppStartingWindow(
        //                IBinder::Probe(next->mAppToken), next->mPackageName, next->mTheme,
        //                mService->CompatibilityInfoForPackageLocked(appInfo),
        //                next->mNonLocalizedLabel, next->mLabelRes, next->mIcon, next->mLogo,
        //                next->mWindowFlags, NULL, TRUE);
        //    }
        //    mStackSupervisor->StartSpecificActivityLocked(next, TRUE, FALSE);
        //    if (CActivityManagerService::DEBUG_STACK)
        //      mStackSupervisor->ValidateTopActivitiesLocked();
        //    return TRUE;
        //}

        // From this point on, if something goes wrong there is no way
        // to recover the activity.
        //try {
            next->mVisible = TRUE;
            CompleteResumeLocked(next);
        //} catch (Exception e) {
        //    // If any exception gets thrown, toss away this
        //    // activity and try the next one.
        //    Slogger::W(CActivityManagerService::TAG, "Exception thrown during resume of " + next, e);
        //    RequestFinishActivityLocked(IBinder::Probe(next->mAppToken), IActivity::RESULT_CANCELED, NULL,
        //            String("resume-exception"), TRUE);
        //    if (CActivityManagerService::DEBUG_STACK)
        //    mStackSupervisor->ValidateTopActivitiesLocked();
        //    return TRUE;
        //}
        next->mStopped = FALSE;

    } else {
        // Whoops, need to restart this activity!
        if (!next->mHasBeenLaunched) {
            next->mHasBeenLaunched = TRUE;
        } else {
            if (SHOW_APP_STARTING_PREVIEW) {
                AutoPtr<IApplicationInfo> appInfo;
                IComponentInfo::Probe(next->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
                mWindowManager->SetAppStartingWindow(
                       IBinder::Probe(next->mAppToken), next->mPackageName, next->mTheme,
                       mService->CompatibilityInfoForPackageLocked(
                           appInfo),
                       next->mNonLocalizedLabel,
                       next->mLabelRes, next->mIcon, next->mLogo, next->mWindowFlags,
                       NULL, TRUE);
            }
            if (CActivityManagerService::DEBUG_SWITCH)
                Slogger::V(CActivityManagerService::TAG, "Restarting: %s", next->ToString().string());
        }
        if (ActivityStackSupervisor::DEBUG_STATES) Slogger::D(CActivityManagerService::TAG, "resumeTopActivityLocked: Restarting %s", next->ToString().string());
        mStackSupervisor->StartSpecificActivityLocked(next, TRUE, TRUE);
    }

    if (CActivityManagerService::DEBUG_STACK)
        mStackSupervisor->ValidateTopActivitiesLocked();
    return TRUE;
}

void ActivityStack::StartActivityLocked(
    /* [in] */ ActivityRecord* r,
    /* [in] */ Boolean newTask,
    /* [in] */ Boolean doResume,
    /* [in] */ Boolean keepCurTransition,
    /* [in] */ IBundle* options)
{
    AutoPtr<TaskRecord> rTask = r->mTask;
    Int32 taskId = rTask->mTaskId;
    // mLaunchTaskBehind tasks get placed at the back of the task stack.
    if (!r->mLaunchTaskBehind && (TaskForIdLocked(taskId) == NULL || newTask)) {
        // Last activity in task had been removed or ActivityManagerService is reusing task.
        // Insert or replace.
        // Might not even be in.
        InsertTaskAtTop(rTask);
        mWindowManager->MoveTaskToTop(taskId);
    }
    AutoPtr<TaskRecord> task = NULL;
    if (!newTask) {
        // If starting in an existing task, find where that is...
        Boolean startIt = TRUE;
        Int32 taskSize;
        mTaskHistory->GetSize(&taskSize);
        for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
            AutoPtr<IInterface> taskobj;
            mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
            task = (TaskRecord*)(IObject::Probe(taskobj));
            if (task->GetTopActivity() == NULL) {
                // All activities in task are finishing.
                continue;
            }
            if (task == r->mTask) {
                // Here it is!  Now, if this is not yet visible to the
                // user, then just add it without starting; it will
                // get started when the user navigates back to it.
                if (!startIt) {
                    //if (ActivityStackSupervisor::DEBUG_ADD_REMOVE) Slog.i(CActivityManagerService::TAG, "Adding activity " + r + " to task "
                    //        + task, new RuntimeException("here").fillInStackTrace());
                    task->AddActivityToTop(r);
                    r->PutInHistory();
                    //task.mActivities.indexOf(r)
                    AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
                    Int32 activityNdx = -1;//IndexOf
                    for(UInt32 i = 0; i < activities->GetSize(); ++i) {
                        if (((*activities)[i]).Get() == r) {
                            activityNdx = i;
                            break;
                        }
                    }
                    AutoPtr<IActivityInfo> info = r->mInfo;
                    Int32 screenOrientation, flags, configChanges;
                    info->GetScreenOrientation(&screenOrientation);
                    info->GetFlags(&flags);
                    info->GetConfigChanges(&configChanges);
                    assert(activityNdx);//TODO remove
                    mWindowManager->AddAppToken(activityNdx, r->mAppToken,
                           r->mTask->mTaskId, mStackId, screenOrientation, r->mFullscreen,
                           (flags & IActivityInfo::FLAG_SHOW_ON_LOCK_SCREEN) != 0,
                           r->mUserId, configChanges, task->mVoiceSession != NULL,
                           r->mLaunchTaskBehind);
                    if (CActivityManagerService::VALIDATE_TOKENS) {
                        ValidateAppTokensLocked();
                    }
                    AutoPtr<IActivityOptionsHelper> aoHelper;
                    CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoHelper);
                    aoHelper->Abort(options);
                    return;
                }
                break;
            } else if (task->mNumFullscreen > 0) {
                startIt = FALSE;
            }
        }
    }

    // Place a new activity at top of stack, so it is next to interact
    // with the user.

    // If we are not placing the new activity frontmost, we do not want
    // to deliver the onUserLeaving callback to the actual frontmost
    // activity
    Int32 taskSize;
    mTaskHistory->GetSize(&taskSize);
    Int32 taskIndex;
    mTaskHistory->IndexOf(TO_IINTERFACE(task), &taskIndex);
    if (task == r->mTask && taskIndex != (taskSize - 1)) {
        mStackSupervisor->mUserLeaving = FALSE;
        if (CActivityManagerService::DEBUG_USER_LEAVING)
            Slogger::V(CActivityManagerService::TAG,
                "startActivity() behind front, mUserLeaving=false");
    }

    task = r->mTask;

    // Slot the activity into the history stack and proceed
    //if (ActivityStackSupervisor::DEBUG_ADD_REMOVE) Slog.i(CActivityManagerService::TAG, "Adding activity " + r + " to stack to task " + task,
    //        new RuntimeException("here").fillInStackTrace());
    task->AddActivityToTop(r);
    task->SetFrontOfTask();

    r->PutInHistory();
    if (!IsHomeStack() || NumActivities() > 0) {
        // We want to show the starting preview window if we are
        // switching to a new task, or the next activity's process is
        // not currently running.
        Boolean showStartingIcon = newTask;
        AutoPtr<ProcessRecord> proc = r->mApp;
        if (proc == NULL) {
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(r->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 uid;
            appInfo->GetUid(&uid);
            proc = mService->mProcessNames->Get(r->mProcessName, uid);//TODO ProcessNames
        }
        if (proc == NULL || proc->mThread == NULL) {
            showStartingIcon = TRUE;
        }
        if (CActivityManagerService::DEBUG_TRANSITION)
            Slogger::V(CActivityManagerService::TAG,
                "Prepare open transition: starting %s", r->ToString().string());
        Int32 intentFlags;
        if (((r->mIntent->GetFlags(&intentFlags), intentFlags)&IIntent::FLAG_ACTIVITY_NO_ANIMATION) != 0) {
            mWindowManager->PrepareAppTransition(0/*TODO AppTransition::TRANSIT_NONE*/, keepCurTransition);
            mNoAnimActivities->Add(TO_IINTERFACE(r));
        } else {
            mWindowManager->PrepareAppTransition(newTask
                   ? r->mLaunchTaskBehind
                   ? 16/*TODO AppTransition::TRANSIT_TASK_OPEN_BEHIND */
                   : 8/*TODO AppTransition::TRANSIT_TASK_OPEN*/
                   : 6/*TODO AppTransition::TRANSIT_ACTIVITY_OPEN*/, keepCurTransition);
            mNoAnimActivities->Remove(TO_IINTERFACE(r));
        }

        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        Int32 activityNdx = -1;//IndexOf
        for(UInt32 i = 0; i < activities->GetSize(); ++i) {
            if (((*activities)[i]).Get() == r) {
                activityNdx = i;
                break;
            }
        }
        AutoPtr<IActivityInfo> info = r->mInfo;
        //r.info.screenOrientation
        Int32 screenOrientation;
        info->GetScreenOrientation(&screenOrientation);
        //r.info.flags
        Int32 flags;
        info->GetFlags(&flags);
        //r.info.configChanges
        Int32 configChanges;
        info->GetConfigChanges(&configChanges);
        assert(activityNdx);//TODO remove
        mWindowManager->AddAppToken(activityNdx,
               r->mAppToken, r->mTask->mTaskId, mStackId, screenOrientation, r->mFullscreen,
               (flags & IActivityInfo::FLAG_SHOW_ON_LOCK_SCREEN) != 0, r->mUserId,
               configChanges, task->mVoiceSession != NULL, r->mLaunchTaskBehind);
        Boolean doShow = TRUE;
        if (newTask) {
            // Even though this activity is starting fresh, we still need
            // to reset it to make sure we apply affinities to move any
            // existing activities from other tasks in to it.
            // If the caller has requested that the target task be
            // reset, then do so.
            Int32 intentFlags;
            if (((r->mIntent->GetFlags(&intentFlags),intentFlags) & IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED) != 0) {
                ResetTaskIfNeededLocked(r, r);
                doShow = TopRunningNonDelayedActivityLocked(NULL).Get() == r;
            }
        } else if (options != NULL) {
            AutoPtr<IActivityOptions> ao;
            CActivityOptions::New(options, (IActivityOptions**)&ao);
            Int32 type;
            ao->GetAnimationType(&type);
            if(type == IActivityOptions::ANIM_SCENE_TRANSITION) {
                doShow = FALSE;
            }
        }

        if (r->mLaunchTaskBehind) {
            // Don't do a starting window for mLaunchTaskBehind. More importantly make sure we
            // tell WindowManager that r is visible even though it is at the back of the stack.
            mWindowManager->SetAppVisibility(IBinder::Probe(r->mAppToken), TRUE);
            EnsureActivitiesVisibleLocked(NULL, 0);
        } else if (SHOW_APP_STARTING_PREVIEW && doShow) {
            // Figure out if we are transitioning from another activity that is
            // "has the same starting icon" as the next one.  This allows the
            // window manager to keep the previous window it had previously
            // created, if it still had one.
            AutoPtr<ActivityRecord> prev = mResumedActivity;
            if (prev != NULL) {
                // We don't want to reuse the previous starting preview if:
                // (1) The current activity is in a different task.
                if (prev->mTask != r->mTask) {
                    prev = NULL;
                }
                // (2) The current activity is already displayed.
                else if (prev->mNowVisible) {
                    prev = NULL;
                }
            }
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(r->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            assert(showStartingIcon);//TODO remove
            mWindowManager->SetAppStartingWindow(
                   IBinder::Probe(r->mAppToken), r->mPackageName, r->mTheme,
                   mService->CompatibilityInfoForPackageLocked(appInfo),
                   r->mNonLocalizedLabel,
                   r->mLabelRes, r->mIcon, r->mLogo, r->mWindowFlags,
                   prev != NULL ? IBinder::Probe(prev->mAppToken) : NULL, showStartingIcon);
            r->mStartingWindowShown = TRUE;
        }
    } else {
        // If this is the first activity, don't do any fancy animations,
        // because there is nothing for it to animate on top of.
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        Int32 activityNdx = -1;//IndexOf
        for(UInt32 i = 0; i < activities->GetSize(); ++i) {
            if (((*activities)[i]).Get() == r) {
                activityNdx = i;
                break;
            }
        }
        AutoPtr<IActivityInfo> info = r->mInfo;
        //r.info.screenOrientation
        Int32 screenOrientation;
        info->GetScreenOrientation(&screenOrientation);
        //r.info.flags
        Int32 flags;
        info->GetFlags(&flags);
        //r.info.configChanges
        Int32 configChanges;
        info->GetConfigChanges(&configChanges);
        assert(activityNdx);//TODO remove
        mWindowManager->AddAppToken(activityNdx, r->mAppToken,
               r->mTask->mTaskId, mStackId, screenOrientation, r->mFullscreen,
               (flags & IActivityInfo::FLAG_SHOW_ON_LOCK_SCREEN) != 0, r->mUserId,
               configChanges, task->mVoiceSession != NULL, r->mLaunchTaskBehind);
        AutoPtr<IActivityOptionsHelper> aoHelper;
        CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoHelper);
        aoHelper->Abort(options);
        options = NULL;
    }
    if (CActivityManagerService::VALIDATE_TOKENS) {
        ValidateAppTokensLocked();
    }

    if (doResume) {
        mStackSupervisor->ResumeTopActivitiesLocked(this, r, options);
    }
}

void ActivityStack::ValidateAppTokensLocked()
{
    mValidateAppTokens->Clear();
    mValidateAppTokens->EnsureCapacity(NumActivities());
    Int32 numTasks;
    mTaskHistory->GetSize(&numTasks);
    for (Int32 taskNdx = 0; taskNdx < numTasks; ++taskNdx) {
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        //ArrayList<ActivityRecord> activities = task.mActivities;
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        //if (activities.isEmpty())
        if (activities->IsEmpty()) {
            continue;
        }

        AutoPtr<TaskGroup> group = new TaskGroup();
        group->mTaskId = task->mTaskId;
        mValidateAppTokens->Add(TO_IINTERFACE(group));
        Int32 numActivities = activities->GetSize();
        for (Int32 activityNdx = 0; activityNdx < numActivities; ++activityNdx) {
            AutoPtr<ActivityRecord> r = (*activities)[activityNdx];//->get(activityNdx);
            group->mTokens.PushBack(r->mAppToken);
        }
    }
    mWindowManager->ValidateAppTokens(mStackId, mValidateAppTokens);
}

AutoPtr<IActivityOptions> ActivityStack::ResetTargetTaskIfNeededLocked(
    /* [in] */ TaskRecord* task,
    /* [in] */ Boolean forceReset)
{
    AutoPtr<IActivityOptions> topOptions = NULL;

    Int32 replyChainEnd = -1;
    Boolean canMoveOptions = TRUE;

    // We only do this for activities that are not the root of the task (since if we finish
    // the root, we may no longer have the task!).
    //ArrayList<ActivityRecord> activities = task.mActivities;
    AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
    Int32 numActivities = activities->GetSize();
    Int32 rootActivityNdx = task->FindEffectiveRootIndex();
    for (Int32 i = numActivities - 1; i > rootActivityNdx; --i ) {
        AutoPtr<ActivityRecord> target = (*activities)[i];
        if (target->mFrontOfTask)
            break;

        AutoPtr<IActivityInfo> info = target->mInfo;
        Int32 flags;
        info->GetFlags(&flags);
        Boolean finishOnTaskLaunch =
                (flags & IActivityInfo::FLAG_FINISH_ON_TASK_LAUNCH) != 0;
        Boolean allowTaskReparenting =
                (flags & IActivityInfo::FLAG_ALLOW_TASK_REPARENTING) != 0;
        Int32 intentFlags;
        target->mIntent->GetFlags(&intentFlags);
        Boolean clearWhenTaskReset =
                (intentFlags & IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET) != 0;

        if (!finishOnTaskLaunch
                && !clearWhenTaskReset
                && target->mResultTo != NULL) {
            // If this activity is sending a reply to a previous
            // activity, we can't do anything with it now until
            // we reach the start of the reply chain.
            // XXX note that we are assuming the result is always
            // to the previous activity, which is almost always
            // the case but we really shouldn't count on.
            if (replyChainEnd < 0) {
                replyChainEnd = i;
            }
        } else if (!finishOnTaskLaunch
                && !clearWhenTaskReset
                && allowTaskReparenting
                //&& target->mTaskAffinity != NULL
                && !target->mTaskAffinity.Equals(task->mAffinity)) {
            // If this activity has an affinity for another
            // task, then we need to move it out of here.  We will
            // move it as far out of the way as possible, to the
            // bottom of the activity stack.  This also keeps it
            // correctly ordered with any activities we previously
            // moved.
            AutoPtr<TaskRecord> targetTask;
            AutoPtr<ActivityRecord> bottom;
            Boolean isEmpty;
            mTaskHistory->IsEmpty(&isEmpty);
            if (!isEmpty) {
                AutoPtr<IInterface> taskobj;
                mTaskHistory->Get(0, (IInterface**)&taskobj);
                TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
                AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
                if (!activities->IsEmpty()) {
                    bottom = (*(activities->Begin()));
                }
            }

            if (bottom != NULL
                    //&& target.taskAffinity != NULL
                    && target->mTaskAffinity.Equals(bottom->mTask->mAffinity)) {
                // If the activity currently at the bottom has the
                // same task affinity as the one we are moving,
                // then merge it into the same task.
                targetTask = bottom->mTask;
                //if (CActivityManagerService::DEBUG_TASKS) Slog.v(CActivityManagerService::TAG, "Start pushing activity " + target
                //        + " out to bottom task " + bottom.task);
            } else {
                targetTask = CreateTaskRecord(mStackSupervisor->GetNextTaskId(), target->mInfo,
                        NULL, NULL, NULL, FALSE);
                targetTask->mAffinityIntent = target->mIntent;
                //if (CActivityManagerService::DEBUG_TASKS) Slog.v(CActivityManagerService::TAG, "Start pushing activity " + target
                //        + " out to new task " + target.task);
            }

            Int32 targetTaskId = targetTask->mTaskId;
            assert(targetTaskId);//TODO remove
            mWindowManager->SetAppGroupId(IBinder::Probe(target->mAppToken), targetTaskId);

            Boolean noOptions = canMoveOptions;
            Int32 start = replyChainEnd < 0 ? i : replyChainEnd;
            for (Int32 srcPos = start; srcPos >= i; --srcPos) {
                AutoPtr<ActivityRecord> p = (*activities)[srcPos];
                if (p->mFinishing) {
                    continue;
                }

                canMoveOptions = FALSE;
                if (noOptions && topOptions == NULL) {
                    topOptions = p->TakeOptionsLocked();
                    if (topOptions != NULL) {
                        noOptions = FALSE;
                    }
                }
                //if (ActivityStackSupervisor::DEBUG_ADD_REMOVE) Slog.i(CActivityManagerService::TAG, "Removing activity " + p + " from task="
                //        + task + " adding to task=" + targetTask
                //        + " Callers=" + Debug.getCallers(4));
                //if (CActivityManagerService::DEBUG_TASKS) Slog.v(CActivityManagerService::TAG, "Pushing next activity " + p
                //        + " out to target's task " + target.task);
                p->SetTask(targetTask, NULL);
                targetTask->AddActivityAtBottom(p);

                mWindowManager->SetAppGroupId(IBinder::Probe(p->mAppToken), targetTaskId);
            }

            mWindowManager->MoveTaskToBottom(targetTaskId);
            if (CActivityManagerService::VALIDATE_TOKENS) {
                ValidateAppTokensLocked();
            }

            replyChainEnd = -1;
        } else if (forceReset || finishOnTaskLaunch || clearWhenTaskReset) {
            // If the activity should just be removed -- either
            // because it asks for it, or the task should be
            // cleared -- then finish it and anything that is
            // part of its reply chain.
            Int32 end;
            if (clearWhenTaskReset) {
                // In this case, we want to finish this activity
                // and everything above it, so be sneaky and pretend
                // like these are all in the reply chain.
                end = numActivities - 1;
            } else if (replyChainEnd < 0) {
                end = i;
            } else {
                end = replyChainEnd;
            }
            Boolean noOptions = canMoveOptions;
            for (Int32 srcPos = i; srcPos <= end; srcPos++) {
                AutoPtr<ActivityRecord> p = (*activities)[srcPos];
                if (p->mFinishing) {
                    continue;
                }
                canMoveOptions = FALSE;
                if (noOptions && topOptions == NULL) {
                    topOptions = p->TakeOptionsLocked();
                    if (topOptions != NULL) {
                        noOptions = FALSE;
                    }
                }
                if (CActivityManagerService::DEBUG_TASKS) Slogger::W(CActivityManagerService::TAG, "resetTaskIntendedTask: calling finishActivity on %s", p->ToString().string());
                if (FinishActivityLocked(p, IActivity::RESULT_CANCELED, NULL, String("reset"), FALSE)) {
                    end--;
                    srcPos--;
                }
            }
            replyChainEnd = -1;
        } else {
            // If we were in the middle of a chain, well the
            // activity that started it all doesn't want anything
            // special, so leave it all as-is.
            replyChainEnd = -1;
        }
    }

    return topOptions;
}

AutoPtr<ActivityRecord> ActivityStack::ResetTaskIfNeededLocked(
    /* [in] */ ActivityRecord* taskTop,
    /* [in] */ ActivityRecord* newActivity)
{
    AutoPtr<IActivityInfo> info = newActivity->mInfo;
    Int32 flags;
    info->GetFlags(&flags);
    Boolean forceReset =
            (flags & IActivityInfo::FLAG_CLEAR_TASK_ON_LAUNCH) != 0;
    if (ACTIVITY_INACTIVE_RESET_TIME > 0
            && taskTop->mTask->GetInactiveDuration() > ACTIVITY_INACTIVE_RESET_TIME) {
        if ((flags & IActivityInfo::FLAG_ALWAYS_RETAIN_TASK_STATE) == 0) {
            forceReset = TRUE;
        }
    }

    AutoPtr<TaskRecord> task = taskTop->mTask;

    /** False until we evaluate the TaskRecord associated with taskTop. Switches to true
     * for remaining tasks. Used for later tasks to reparent to task. */
    Boolean taskFound = FALSE;

    /** If ActivityOptions are moved out and need to be aborted or moved to taskTop. */
    AutoPtr<IActivityOptions> topOptions = NULL;

    // Preserve the location for reparenting in the new task.
    Int32 reparentInsertionPoint = -1;

    Int32 historySize;
    mTaskHistory->GetSize(&historySize);
    for (Int32 i = historySize - 1; i >= 0; --i) {
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(i, (IInterface**)&taskobj);
        TaskRecord* targetTask = (TaskRecord*)(IObject::Probe(taskobj));

        if (targetTask == task.Get()) {
            topOptions = ResetTargetTaskIfNeededLocked(task, forceReset);
            taskFound = TRUE;
        } else {
            reparentInsertionPoint = ResetAffinityTaskIfNeededLocked(targetTask, task,
                    taskFound, forceReset, reparentInsertionPoint);
        }
    }

    Int32 taskNdx;// = mTaskHistory.IndexOf(task);
    mTaskHistory->IndexOf(TO_IINTERFACE(task), &taskNdx);
    do {
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx--, (IInterface**)&taskobj);
        TaskRecord* tr = (TaskRecord*)(IObject::Probe(taskobj));
        taskTop = tr->GetTopActivity();
    } while (taskTop == NULL && taskNdx >= 0);

    if (topOptions != NULL) {
        // If we got some ActivityOptions from an activity on top that
        // was removed from the task, propagate them to the new real top.
        if (taskTop != NULL) {
            taskTop->UpdateOptionsLocked(topOptions);
        } else {
            topOptions->Abort();
        }
    }

    return taskTop;
}

void ActivityStack::SendActivityResultLocked(
    /* [in] */ Int32 callingUid,
    /* [in] */ ActivityRecord* r,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    if (callingUid > 0) {
        mService->GrantUriPermissionFromIntentLocked(callingUid, r->mPackageName,
                data, r->GetUriPermissionsLocked(), r->mUserId);
    }

    //if (CActivityManagerService::DEBUG_RESULTS)
    //  Slog.v(CActivityManagerService::TAG, "Send activity result to " + r
    //        + " : who=" + resultWho + " req=" + requestCode
    //        + " res=" + resultCode + " data=" + data);
    if (mResumedActivity.Get() == r && r->mApp != NULL && r->mApp->mThread != NULL) {
        //try {
            //ArrayList<ResultInfo> list = new ArrayList<ResultInfo>();
            AutoPtr<IArrayList> list;
            CArrayList::New((IArrayList**)&list);
            AutoPtr<IResultInfo> resultInfo;
            CResultInfo::New(resultWho, requestCode, resultCode, data, (IResultInfo**)&resultInfo);
            list->Add(TO_IINTERFACE(resultInfo));
            r->mApp->mThread->ScheduleSendResult(IBinder::Probe(r->mAppToken), IList::Probe(list));
            return;
        //} catch (Exception e) {
        //    Slogger::W(CActivityManagerService::TAG, "Exception thrown sending result to " + r, e);
        //}
    }

    r->AddResultLocked(NULL, resultWho, requestCode, resultCode, data);
}

void ActivityStack::StopActivityLocked(
    /* [in] */ ActivityRecord* r)
{
    if (CActivityManagerService::DEBUG_SWITCH)
        Slogger::D(CActivityManagerService::TAG, "Stopping: %s", r->ToString().string());
    Int32 intentFlags;
    r->mIntent->GetFlags(&intentFlags);
    Int32 infoFlags;
    r->mInfo->GetFlags(&infoFlags);
    if ((intentFlags&IIntent::FLAG_ACTIVITY_NO_HISTORY) != 0
            || (infoFlags&IActivityInfo::FLAG_NO_HISTORY) != 0) {
        if (!r->mFinishing) {
            if (!mService->IsSleeping()) {
                if (ActivityStackSupervisor::DEBUG_STATES) {
                    Slogger::D(CActivityManagerService::TAG, "no-history finish of %s", r->ToString().string());
                }
                RequestFinishActivityLocked(IBinder::Probe(r->mAppToken), IActivity::RESULT_CANCELED, NULL,
                        String("no-history"), FALSE);
            } else {
                if (ActivityStackSupervisor::DEBUG_STATES)
                    Slogger::D(CActivityManagerService::TAG, "Not finishing noHistory %s on stop because we're just sleeping", r->ToString().string());
            }
        }
    }

    if (r->mApp != NULL && r->mApp->mThread != NULL) {
        AdjustFocusedActivityLocked(r);
        r->ResumeKeyDispatchingLocked();
        //try {
            r->mStopped = FALSE;
            //if (ActivityStackSupervisor::DEBUG_STATES)
            //  Slog.v(CActivityManagerService::TAG, "Moving to STOPPING: " + r + " (stop requested)");
            r->mState = ActivityState_STOPPING;
            //if (CActivityManagerService::DEBUG_VISBILITY)
            //  Slog.v(CActivityManagerService::TAG, "Stopping visible=" + r.visible + " for " + r);
            if (!r->mVisible) {
                mWindowManager->SetAppVisibility(IBinder::Probe(r->mAppToken), FALSE);
            }
            r->mApp->mThread->ScheduleStopActivity(IBinder::Probe(r->mAppToken), r->mVisible, r->mConfigChangeFlags);
            if (mService->IsSleepingOrShuttingDown()) {
                r->SetSleeping(TRUE);
            }
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(STOP_TIMEOUT_MSG, TO_IINTERFACE(r), (IMessage**)&msg);
            Boolean bTemp;
            mHandler->SendMessageDelayed(msg, STOP_TIMEOUT, &bTemp);
        //} catch (Exception e) {
        //TODO
        //    // Maybe just ignore exceptions here...  if the process
        //    // has crashed, our death notification will clean things
        //    // up.
        //    //Slogger::W(CActivityManagerService::TAG, "Exception thrown during pause", e);
        //    // Just in case, assume it to be stopped.
        //    r->mStopped = TRUE;
        //    //if (ActivityStackSupervisor::DEBUG_STATES) Slog.v(CActivityManagerService::TAG, "Stop failed; moving to STOPPED: " + r);
        //    r->mState = ActivityState_STOPPED;
        //    if (r->mConfigDestroy) {
        //        DestroyActivityLocked(r, TRUE, String("stop-except"));
        //    }
        //}
    }
}

Boolean ActivityStack::RequestFinishActivityLocked(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* resultData,
    /* [in] */ const String& reason,
    /* [in] */ Boolean oomAdj)
{
    AutoPtr<ActivityRecord> r = IsInStackLocked(token);
    //if (CActivityManagerService::DEBUG_RESULTS || ActivityStackSupervisor::DEBUG_STATES)
    //  Slog.v(
    //        CActivityManagerService::TAG, "Finishing activity token=" + token + " r="
    //        + ", result=" + resultCode + ", data=" + resultData
    //        + ", reason=" + reason);
    if (r == NULL) {
        return FALSE;
    }

    FinishActivityLocked(r, resultCode, resultData, reason, oomAdj);
    return TRUE;
}

void ActivityStack::FinishSubActivityLocked(
    /* [in] */ ActivityRecord* self,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode)
{
    Int32 historySize;
    mTaskHistory->GetSize(&historySize);
    for (Int32 taskNdx = historySize - 1; taskNdx >= 0; --taskNdx) {
        //ArrayList<ActivityRecord> activities = mTaskHistory.get(taskNdx).mActivities;
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            AutoPtr<ActivityRecord> r = (*activities)[activityNdx];
            if (r->mResultTo.Get() == self && r->mRequestCode == requestCode) {
                if ((r->mResultWho == NULL && resultWho == NULL) ||
                    (//r->mResultWho != NULL &&
                     r->mResultWho.Equals(resultWho))) {
                    FinishActivityLocked(r, IActivity::RESULT_CANCELED, NULL, String("request-sub"),
                            FALSE);
                }
            }
        }
    }
    mService->UpdateOomAdjLocked();
}

void ActivityStack::FinishTopRunningActivityLocked(
    /* [in] */ ProcessRecord* app)
{
    AutoPtr<ActivityRecord> r = TopRunningActivityLocked(NULL);
    if (r != NULL && r->mApp.Get() == app) {
        // If the top running activity is from this crashing
        // process, then terminate it to avoid getting in a loop.
        //Slogger::W(CActivityManagerService::TAG, "  Force finishing activity " + r.intent.getComponent().flattenToShortString());
        Int32 taskNdx;
        mTaskHistory->IndexOf(TO_IINTERFACE(r->mTask), &taskNdx);
        //int activityNdx = r.task.mActivities.IndexOf(r);
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = r->mTask->mActivities;
        Int32 activityNdx = -1;//IndexOf
        for(UInt32 i = 0; i < activities->GetSize(); ++i) {
            if ((*activities)[i] == r) {
                activityNdx = i;
                break;
            }
        }
        FinishActivityLocked(r, IActivity::RESULT_CANCELED, NULL, String("crashed"), FALSE);
        // Also terminate any activities below it that aren't yet
        // stopped, to avoid a situation where one will get
        // re-start our crashing activity once it gets resumed again.
        --activityNdx;
        if (activityNdx < 0) {
            do {
                --taskNdx;
                if (taskNdx < 0) {
                    break;
                }
                AutoPtr<IInterface> taskobj;
                mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
                TaskRecord* tr = (TaskRecord*)(IObject::Probe(taskobj));
                activityNdx = tr->mActivities->GetSize() - 1;
            } while (activityNdx < 0);
        }

        if (activityNdx >= 0) {
            AutoPtr<IInterface> taskobj;
            mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
            TaskRecord* tr = (TaskRecord*)(IObject::Probe(taskobj));
            AutoPtr<List<AutoPtr<ActivityRecord> > > activities = tr->mActivities;
            r = (*activities)[activityNdx];
            if (r->mState == ActivityState_RESUMED
                    || r->mState == ActivityState_PAUSING
                    || r->mState == ActivityState_PAUSED) {
                if (!r->IsHomeActivity() || mService->mHomeProcess != r->mApp) {
                    //Slogger::W(CActivityManagerService::TAG, "  Force finishing activity "
                    //        + r.intent.getComponent().flattenToShortString());
                    FinishActivityLocked(r, IActivity::RESULT_CANCELED, NULL, String("crashed"), FALSE);
                }
            }
        }
    }
}

void ActivityStack::FinishVoiceTask(
    /* [in] */ IIVoiceInteractionSession* session)
{
    //IBinder sessionBinder = session.asBinder();
    AutoPtr<IBinder> sessionBinder = IBinder::Probe(session);
    Boolean didOne = FALSE;
    Int32 taskSize;
    mTaskHistory->GetSize(&taskSize);
    for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* tr = (TaskRecord*)(IObject::Probe(taskobj));
        if (tr->mVoiceSession != NULL) {
            AutoPtr<IBinder> trSessionBinder = IBinder::Probe(tr->mVoiceSession);
            if (trSessionBinder == sessionBinder) {
                AutoPtr<List<AutoPtr<ActivityRecord> > > activities = tr->mActivities;
                for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
                    AutoPtr<ActivityRecord> r = (*activities)[activityNdx];
                    if (!r->mFinishing) {
                        FinishActivityLocked(r, IActivity::RESULT_CANCELED, NULL, String("finish-voice"),
                                FALSE);
                        didOne = TRUE;
                    }
                }
            }
        }
    }
    if (didOne) {
        mService->UpdateOomAdjLocked();
    }
}

Boolean ActivityStack::FinishActivityAffinityLocked(
    /* [in] */ ActivityRecord* r)
{
    //ArrayList<ActivityRecord> activities = r.task.mActivities;
    AutoPtr<List<AutoPtr<ActivityRecord> > > activities = r->mTask->mActivities;
    Int32 activityNdx = -1;//IndexOf
    for(UInt32 i = 0; i < activities->GetSize(); ++i) {
        if (((*activities)[i]).Get() == r) {
            activityNdx = i;
            break;
        }
    }
    for (Int32 index = activityNdx; index >= 0; --index) {
        AutoPtr<ActivityRecord> cur = (*activities)[index];
        //TODO the java semantic here,
        // cur->mTaskAffinity and r->mTaskAffinity should be the same object
        // mTaskAffinity which got from IActivityInfo
        //TODO if (!IObject::Equals(cur->mTaskAffinity, r->mTaskAffinity))
        if (cur->mTaskAffinity.Equals(r->mTaskAffinity)) {
            break;
        }
        FinishActivityLocked(cur, IActivity::RESULT_CANCELED, NULL, String("request-affinity"), TRUE);
    }
    return TRUE;
}

void ActivityStack::FinishActivityResultsLocked(
    /* [in] */ ActivityRecord* r,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* resultData)
{
    // send the result
    AutoPtr<ActivityRecord> resultTo = r->mResultTo;
    if (resultTo != NULL) {
        //if (CActivityManagerService::DEBUG_RESULTS)
        //  Slog.v(CActivityManagerService::TAG, "Adding result to " + resultTo
        //        + " who=" + r.resultWho + " req=" + r.requestCode
        //        + " res=" + resultCode + " data=" + resultData);
        if (resultTo->mUserId != r->mUserId) {
            if (resultData != NULL) {
                resultData->SetContentUserHint(r->mUserId);
            }
        }
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(r->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 uid;
        appInfo->GetUid(&uid);

        if (uid > 0) {
            mService->GrantUriPermissionFromIntentLocked(uid,
                    resultTo->mPackageName, resultData,
                    resultTo->GetUriPermissionsLocked(), resultTo->mUserId);
        }
        resultTo->AddResultLocked(r, r->mResultWho, r->mRequestCode, resultCode,
                                 resultData);
        r->mResultTo = NULL;
    }
    else if (CActivityManagerService::DEBUG_RESULTS)
        Slogger::V(CActivityManagerService::TAG, "No result destination from %s", r->ToString().string());

    // Make sure this HistoryRecord is not holding on to other resources,
    // because clients have remote IPC references to this object so we
    // can't assume that will go away and want to avoid circular IPC refs.
    r->mResults = NULL;
    r->mPendingResults = NULL;
    r->mNewIntents = NULL;
    r->mIcicle = NULL;
}

Boolean ActivityStack::FinishActivityLocked(
    /* [in] */ ActivityRecord* r,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* resultData,
    /* [in] */ const String& reason,
    /* [in] */ Boolean oomAdj)
{
    if (r->mFinishing) {
        Slogger::W(CActivityManagerService::TAG, "Duplicate finish request for %s", r->ToString().string());
        return FALSE;
    }

    r->MakeFinishing();
    AutoPtr<TaskRecord> task = r->mTask;
    //TODO begin
    //EventLog.writeEvent(EventLogTags.AM_FINISH_ACTIVITY,
    //        r.userId, System.identityHashCode(r),
    //        task.taskId, r.shortComponentName, reason);
    //TODO end
    //final ArrayList<ActivityRecord> activities = task.mActivities;
    AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
    //final int index = activities.IndexOf(r);
    Int32 index = -1;//IndexOf
    for(UInt32 i = 0; i < activities->GetSize(); ++i) {
        if (((*activities)[i]).Get() == r) {
            index = i;
            break;
        }
    }
    if (index < (Int32)(activities->GetSize() - 1)) {
        task->SetFrontOfTask();
        Int32 flags;
        r->mIntent->GetFlags(&flags);
        if ((flags & IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET) != 0) {
            // If the caller asked that this activity (and all above it)
            // be cleared when the task is reset, don't lose that information,
            // but propagate it up to the next activity.
            AutoPtr<ActivityRecord> next = (*activities)[index+1];
            next->mIntent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET);
        }
    }

    r->PauseKeyDispatchingLocked();

    AdjustFocusedActivityLocked(r);

    FinishActivityResultsLocked(r, resultCode, resultData);

    if (mResumedActivity.Get() == r) {
        Boolean endTask = index <= 0;
        //if (CActivityManagerService::DEBUG_VISBILITY || CActivityManagerService::DEBUG_TRANSITION)
        //  Slog.v(CActivityManagerService::TAG,
        //        "Prepare close transition: finishing " + r);
        mWindowManager->PrepareAppTransition(endTask
               ? 9/*TODO AppTransition::TRANSIT_TASK_CLOSE*/
               : 7/*TODO AppTransition::TRANSIT_ACTIVITY_CLOSE*/, FALSE);

        // Tell window manager to prepare for this one to be removed.
        mWindowManager->SetAppVisibility(IBinder::Probe(r->mAppToken), FALSE);

        if (mPausingActivity == NULL) {
            if (CActivityManagerService::DEBUG_PAUSE)
                Slogger::V(CActivityManagerService::TAG, "Finish needs to pause: %s", r->ToString().string());
            if (CActivityManagerService::DEBUG_USER_LEAVING)
                Slogger::V(CActivityManagerService::TAG, "finish() => pause with userLeaving=false");
            StartPausingLocked(FALSE, FALSE, FALSE, FALSE);
        }

        if (endTask) {
            mStackSupervisor->EndLockTaskModeIfTaskEnding(task);
        }
    } else if (r->mState != ActivityState_PAUSING) {
        // If the activity is PAUSING, we will complete the finish once
        // it is done pausing; else we can just directly finish it here.
        if (CActivityManagerService::DEBUG_PAUSE)
            Slogger::V(CActivityManagerService::TAG, "Finish not pausing: %s", r->ToString().string());
        return FinishCurrentActivityLocked(r, FINISH_AFTER_PAUSE, oomAdj) == NULL;
    } else {
        if (CActivityManagerService::DEBUG_PAUSE)
            Slogger::V(CActivityManagerService::TAG, "Finish waiting for pause of: %s", r->ToString().string());
    }

    return FALSE;
}

AutoPtr<ActivityRecord> ActivityStack::FinishCurrentActivityLocked(
    /* [in] */ ActivityRecord* r,
    /* [in] */ Int32 mode,
    /* [in] */ Boolean oomAdj)
{
    // First things first: if this activity is currently visible,
    // and the resumed activity is not yet visible, then hold off on
    // finishing until the resumed one becomes visible.
    if (mode == FINISH_AFTER_VISIBLE && r->mNowVisible) {
        Boolean contains;
        mStackSupervisor->mStoppingActivities->Contains(TO_IINTERFACE(r), &contains);
        if (!contains) {
            mStackSupervisor->mStoppingActivities->Add(TO_IINTERFACE(r));
            Int32 stoppingActivitySize;
            mStackSupervisor->mStoppingActivities->GetSize(&stoppingActivitySize);
            Int32 taskSize;
            mTaskHistory->GetSize(&taskSize);
            if (stoppingActivitySize > 3
                    || (r->mFrontOfTask && taskSize <= 1)) {
                // If we already have a few activities waiting to stop,
                // then give up on things going idle and start clearing
                // them out. Or if r is the last of activity of the last task the stack
                // will be empty and must be cleared immediately.
                mStackSupervisor->ScheduleIdleLocked();
            } else {
                mStackSupervisor->CheckReadyForSleepLocked();
            }
        }
        //if (ActivityStackSupervisor::DEBUG_STATES) Slog.v(CActivityManagerService::TAG, "Moving to STOPPING: " + r
        //        + " (finish requested)");
        r->mState = ActivityState_STOPPING;
        if (oomAdj) {
            mService->UpdateOomAdjLocked();
        }
        return r;
    }

    // make sure the record is cleaned out of other places.
    mStackSupervisor->mStoppingActivities->Remove(TO_IINTERFACE(r));
    mStackSupervisor->mGoingToSleepActivities->Remove(TO_IINTERFACE(r));
    mStackSupervisor->mWaitingVisibleActivities->Remove(TO_IINTERFACE(r));
    if (mResumedActivity.Get() == r) {
        mResumedActivity = NULL;
    }
    ActivityState prevState = r->mState;
    if (ActivityStackSupervisor::DEBUG_STATES) Slogger::V(CActivityManagerService::TAG, "Moving to FINISHING: %s", r->ToString().string());
    r->mState = ActivityState_FINISHING;

    if (mode == FINISH_IMMEDIATELY
            || prevState == ActivityState_STOPPED
            || prevState == ActivityState_INITIALIZING) {
        // If this activity is already stopped, we can just finish
        // it right now.
        r->MakeFinishing();
        Boolean activityRemoved = DestroyActivityLocked(r, TRUE, String("finish-imm"));
        if (activityRemoved) {
            mStackSupervisor->ResumeTopActivitiesLocked();
        }
        //if (ActivityStackSupervisor::DEBUG_CONTAINERS) Slogger::D(CActivityManagerService::TAG,
        //        "destroyActivityLocked: finishCurrentActivityLocked r=" + r +
        //        " destroy returned removed=" + activityRemoved);
        return activityRemoved ? NULL : r;
    }

    // Need to go through the full pause cycle to get this
    // activity into the stopped state and then finish it.
    if (CActivityManagerService::localLOGV) Slogger::V(CActivityManagerService::TAG, "Enqueueing pending finish: %s", r->ToString().string());
    mStackSupervisor->mFinishingActivities->Add(TO_IINTERFACE(r));
    r->ResumeKeyDispatchingLocked();
    mStackSupervisor->GetFocusedStack()->ResumeTopActivityLocked(NULL);
    return r;
}

void ActivityStack::FinishAllActivitiesLocked(
    /* [in] */ Boolean immediately)
{
    Boolean noActivitiesInStack = TRUE;
    Int32 taskSize;
    mTaskHistory->GetSize(&taskSize);
    for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
        //final ArrayList<ActivityRecord> activities = mTaskHistory.get(taskNdx).mActivities;
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            AutoPtr<ActivityRecord> r = (*activities)[activityNdx];
            noActivitiesInStack = FALSE;
            if (r->mFinishing && !immediately) {
                continue;
            }
            //Slogger::D(CActivityManagerService::TAG, "finishAllActivitiesLocked: finishing " + r + " immediately");
            FinishCurrentActivityLocked(r, FINISH_IMMEDIATELY, FALSE);
        }
    }
    if (noActivitiesInStack) {
        mActivityContainer->OnTaskListEmptyLocked();
    }
}

Boolean ActivityStack::ShouldUpRecreateTaskLocked(
    /* [in] */ ActivityRecord* srec,
    /* [in] */ const String& destAffinity)
{
    // Basic case: for simple app-centric recents, we need to recreate
    // the task if the affinity has changed.
    if (srec == NULL
            //|| srec->mTask->mAffinity == NULL
            || !srec->mTask->mAffinity.Equals(destAffinity)) {
        return TRUE;
    }
    // Document-centric case: an app may be split in to multiple documents;
    // they need to re-create their task if this current activity is the root
    // of a document, unless simply finishing it will return them to the the
    // correct app behind.
    Boolean bTemp = FALSE;
    assert(bTemp);//TODO remove
    if (srec->mFrontOfTask && srec->mTask != NULL && srec->mTask->GetBaseIntent() != NULL
            && FALSE/*TODO (((CIntent*)srec->mTask->GetBaseIntent().Get())->IsDocument(&bTemp), bTemp)*/) {
        // Okay, this activity is at the root of its task.  What to do, what to do...
        if (srec->mTask->GetTaskToReturnTo() != ActivityRecord::APPLICATION_ACTIVITY_TYPE) {
            // Finishing won't return to an application, so we need to recreate.
            return TRUE;
        }
        // We now need to get the task below it to determine what to do.
        Int32 taskIdx;
        mTaskHistory->IndexOf(TO_IINTERFACE(srec->mTask), &taskIdx);
        if (taskIdx <= 0) {
            //Slogger::W(CActivityManagerService::TAG, "shouldUpRecreateTask: task not in history for " + srec);
            return FALSE;
        }
        if (taskIdx == 0) {
            // At the bottom of the stack, nothing to go back to.
            return TRUE;
        }
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskIdx, (IInterface**)&taskobj);
        TaskRecord* prevTask = (TaskRecord*)(IObject::Probe(taskobj));//mTaskHistory.get(taskIdx);
        if (!srec->mTask->mAffinity.Equals(prevTask->mAffinity)) {
            // These are different apps, so need to recreate.
            return TRUE;
        }
    }
    return FALSE;
}

Boolean ActivityStack::NavigateUpToLocked(
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* destIntent,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* resultData)
{
    AutoPtr<ActivityRecord> srec = ActivityRecord::ForToken(token);
    AutoPtr<TaskRecord> task = srec->mTask;
    //final ArrayList<ActivityRecord> activities = task.mActivities;
    AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
    //final int start = activities.indexOf(srec);
    Int32 start = -1;//IndexOf
    for(UInt32 i = 0; i < activities->GetSize(); ++i) {
        if ((*activities)[i] == srec) {
            start = i;
            break;
        }
    }
    Boolean contains;
    mTaskHistory->Contains(TO_IINTERFACE(task), &contains);
    if (!contains || (start < 0)) {
        return FALSE;
    }
    Int32 finishTo = start - 1;
    AutoPtr<ActivityRecord> parent = finishTo < 0 ? NULL : (*activities)[finishTo];
    Boolean foundParentInTask = FALSE;
    //final ComponentName dest = destIntent.GetComponent();
    AutoPtr<IComponentName> dest;
    destIntent->GetComponent((IComponentName**)&dest);

    if (start > 0 && dest != NULL) {
        for (Int32 i = finishTo; i >= 0; i--) {
            AutoPtr<ActivityRecord> r = (*activities)[i];
            String packageName;
            String name;
            IPackageItemInfo::Probe(r->mInfo)->GetPackageName(&packageName);
            IPackageItemInfo::Probe(r->mInfo)->GetName(&name);
            String pName;
            String cName;
            dest->GetPackageName(&pName);
            dest->GetClassName(&cName);
            if (packageName.Equals(pName) &&
                    name.Equals(cName)) {
                finishTo = i;
                parent = r;
                foundParentInTask = TRUE;
                break;
            }
        }
    }

    AutoPtr<IIActivityController> controller = mService->mController;
    if (controller != NULL) {
        AutoPtr<ActivityRecord> next = TopRunningActivityLocked(IBinder::Probe(srec->mAppToken), 0);
        if (next != NULL) {
            // ask watcher if this is allowed
            Boolean resumeOK = TRUE;
            ECode ec;
            //try {
                ec = controller->ActivityResuming(next->mPackageName, &resumeOK);
            //} catch (RemoteException e) {
            if (FAILED(ec)) {
                mService->mController = NULL;
                Watchdog::GetInstance()->SetActivityController(NULL);
            }
            //}

            if (!resumeOK) {
                return FALSE;
            }
        }
    }
    Int64 origId;
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    binderHelper->ClearCallingIdentity(&origId);
    for (Int32 i = start; i > finishTo; i--) {
        AutoPtr<ActivityRecord> r = (*activities)[i];
        RequestFinishActivityLocked(IBinder::Probe(r->mAppToken), resultCode, resultData, String("navigate-up"), TRUE);
        // Only return the supplied result for the first activity finished
        resultCode = IActivity::RESULT_CANCELED;
        resultData = NULL;
    }

    if (parent != NULL && foundParentInTask) {
        Int32 parentLaunchMode;
        parent->mInfo->GetLaunchMode(&parentLaunchMode);
        Int32 destIntentFlags;
        destIntent->GetFlags(&destIntentFlags);
        if (parentLaunchMode == IActivityInfo::LAUNCH_SINGLE_INSTANCE ||
                parentLaunchMode == IActivityInfo::LAUNCH_SINGLE_TASK ||
                parentLaunchMode == IActivityInfo::LAUNCH_SINGLE_TOP ||
                (destIntentFlags & IIntent::FLAG_ACTIVITY_CLEAR_TOP) != 0) {
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(srec->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 uid;
            appInfo->GetUid(&uid);
            parent->DeliverNewIntentLocked(uid, destIntent);
        } else {
            //try {
                AutoPtr<IComponentName> cn;
                destIntent->GetComponent((IComponentName**)&cn);
                AutoPtr<IActivityInfo> aInfo;
                AppGlobals::GetPackageManager()->GetActivityInfo(
                        cn, 0, srec->mUserId, (IActivityInfo**)&aInfo);
                Int32 res;
                mStackSupervisor->StartActivityLocked(srec->mApp->mThread, destIntent,
                        String(NULL), aInfo, NULL, NULL, IBinder::Probe(parent->mAppToken), String(NULL),
                        0, -1, parent->mLaunchedFromUid, parent->mLaunchedFromPackage,
                        -1, parent->mLaunchedFromUid, 0, NULL, TRUE, NULL, NULL, NULL, &res);
                foundParentInTask = res == IActivityManager::START_SUCCESS;
            //} catch (RemoteException e) {
                //TODO foundParentInTask = FALSE;
            //}
            RequestFinishActivityLocked(IBinder::Probe(parent->mAppToken), resultCode,
                    resultData, String("navigate-up"), TRUE);
        }
    }
    binderHelper->RestoreCallingIdentity(origId);
    return foundParentInTask;
}

void ActivityStack::CleanUpActivityLocked(
    /* [in] */ ActivityRecord* r,
    /* [in] */ Boolean cleanServices,
    /* [in] */ Boolean setState)
{
    if (mResumedActivity.Get() == r) {
        mResumedActivity = NULL;
    }
    if (mPausingActivity.Get() == r) {
        mPausingActivity = NULL;
    }
    mService->ClearFocusedActivity(r);

    r->mConfigDestroy = FALSE;
    r->mFrozenBeforeDestroy = FALSE;

    if (setState) {
        //if (ActivityStackSupervisor::DEBUG_STATES) Slog.v(CActivityManagerService::TAG, "Moving to DESTROYED: " + r + " (cleaning up)");
        r->mState = ActivityState_DESTROYED;
        //if (ActivityStackSupervisor::DEBUG_APP) Slog.v(CActivityManagerService::TAG, "Clearing app during cleanUp for activity " + r);
        r->mApp = NULL;
    }

    // Make sure this record is no longer in the pending finishes list.
    // This could happen, for example, if we are trimming activities
    // down to the max limit while they are still waiting to finish.
    mStackSupervisor->mFinishingActivities->Remove(TO_IINTERFACE(r));
    mStackSupervisor->mWaitingVisibleActivities->Remove(TO_IINTERFACE(r));

    // Remove any pending results.
    if (r->mFinishing && r->mPendingResults != NULL) {
        //for (WeakReference<PendingIntentRecord> apr : r.pendingResults) {
        //    PendingIntentRecord rec = apr.get();
        //    if (rec != NULL) {
        //        mService.cancelIntentSenderLocked(rec, false);
        //    }
        //}
        HashSet<AutoPtr<IWeakReference> >::Iterator iter = r->mPendingResults->Begin();
        while(iter != r->mPendingResults->End()) {
            AutoPtr<IWeakReference> weakRef = *iter;
            AutoPtr<IInterface> ws;
            weakRef->Resolve(EIID_IInterface, (IInterface**)&ws);
            AutoPtr<IIIntentSender> rec = IIIntentSender::Probe(ws);
            mService->CancelIntentSenderLocked(rec, FALSE);
            ++iter;
        }
        r->mPendingResults = NULL;
    }

    if (cleanServices) {
        CleanUpActivityServicesLocked(r);
    }

    // Get rid of any pending idle timeouts.
    RemoveTimeoutsForActivityLocked(r);
    if (GetVisibleBehindActivity().Get() == r) {
        mStackSupervisor->RequestVisibleBehindLocked(r, FALSE);
    }
}

void ActivityStack::CleanUpActivityServicesLocked(
    /* [in] */ ActivityRecord* r)
{
    // Throw away any services that have been bound by this activity.
    if (r->mConnections != NULL) {
        //Iterator<ConnectionRecord> it = r.connections.iterator();
        HashSet<AutoPtr<ConnectionRecord> >::Iterator iter = r->mConnections->Begin();
        //while (it.hasNext()) {
        //    ConnectionRecord c = it.next();
        //    mService.mServices.removeConnectionLocked(c, NULL, r);
        //}
        while (iter != r->mConnections->End()) {
            AutoPtr<ConnectionRecord> cr = *iter;
            mService->mServices->RemoveConnectionLocked(cr, NULL, r);
            ++iter;
        }
        r->mConnections = NULL;
    }
}

void ActivityStack::ScheduleDestroyActivities(
    /* [in] */ ProcessRecord* owner,
    /* [in] */ const String& reason)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(DESTROY_ACTIVITIES_MSG, (IMessage**)&msg);
    AutoPtr<ScheduleDestroyArgs> sda = new ScheduleDestroyArgs(owner, reason);
    msg->SetObj(TO_IINTERFACE(sda));
    Boolean res;
    mHandler->SendMessage(msg, &res);
}

void ActivityStack::DestroyActivitiesLocked(
    /* [in] */ ProcessRecord* owner,
    /* [in] */ const String& reason)
{
    Boolean lastIsOpaque = FALSE;
    Boolean activityRemoved = FALSE;
    Int32 taskSize;
    mTaskHistory->GetSize(&taskSize);
    for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        //final ArrayList<ActivityRecord> activities = mTaskHistory.get(taskNdx).mActivities;
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            AutoPtr<ActivityRecord> r = (*activities)[activityNdx];
            if (r->mFinishing) {
                continue;
            }
            if (r->mFullscreen) {
                lastIsOpaque = TRUE;
            }
            if (owner != NULL && r->mApp.Get() != owner) {
                continue;
            }
            if (!lastIsOpaque) {
                continue;
            }
            if (r->IsDestroyable()) {
                //if (CActivityManagerService::DEBUG_SWITCH)
                //  Slog.v(CActivityManagerService::TAG, "Destroying " + r + " in state " + r.state
                //        + " resumed=" + mResumedActivity
                //        + " pausing=" + mPausingActivity + " for reason " + reason);
                if (DestroyActivityLocked(r, TRUE, reason)) {
                    activityRemoved = TRUE;
                }
            }
        }
    }
    if (activityRemoved) {
        mStackSupervisor->ResumeTopActivitiesLocked();
    }
}

Boolean ActivityStack::SafelyDestroyActivityLocked(
    /* [in] */ ActivityRecord* r,
    /* [in] */ const String& reason)
{
    if (r->IsDestroyable()) {
        //if (CActivityManagerService::DEBUG_SWITCH)
        //  Slog.v(CActivityManagerService::TAG, "Destroying " + r + " in state " + r.state
        //        + " resumed=" + mResumedActivity
        //        + " pausing=" + mPausingActivity + " for reason " + reason);
        return DestroyActivityLocked(r, TRUE, reason);
    }
    return FALSE;
}

Int32 ActivityStack::ReleaseSomeActivitiesLocked(
    /* [in] */ ProcessRecord* app,
    ///* [in] */ IArraySet<TaskRecord*>* tasks,
    /* [in] */ IArraySet* tasks,
    /* [in] */ const String& reason)
{
    // Iterate over tasks starting at the back (oldest) first.
    //if (ActivityStackSupervisor::DEBUG_RELEASE) Slogger::D(CActivityManagerService::TAG, "Trying to release some activities in " + app);
    Int32 taskSize;
    ISet::Probe(tasks)->GetSize(&taskSize);
    Int32 maxTasks = taskSize / 4;
    if (maxTasks < 1) {
        maxTasks = 1;
    }
    Int32 numReleased = 0;
    Int32 historySize;
    mTaskHistory->GetSize(&historySize);
    for (Int32 taskNdx = 0; taskNdx < historySize && maxTasks > 0; taskNdx++) {
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        Boolean result;
        ISet::Probe(tasks)->Contains(TO_IINTERFACE(task), &result);
        if (!result) {
            continue;
        }
        //if (ActivityStackSupervisor::DEBUG_RELEASE) Slogger::D(CActivityManagerService::TAG, "Looking for activities to release in " + task);
        Int32 curNum = 0;
        //final ArrayList<ActivityRecord> activities = task.mActivities;
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        for (UInt32 actNdx = 0; actNdx < activities->GetSize(); actNdx++) {
            AutoPtr<ActivityRecord> activity = (*activities)[actNdx];
            if (activity->mApp.Get() == app && activity->IsDestroyable()) {
                //if (ActivityStackSupervisor::DEBUG_RELEASE) Slog.v(CActivityManagerService::TAG, "Destroying " + activity
                //        + " in state " + activity.state + " resumed=" + mResumedActivity
                //        + " pausing=" + mPausingActivity + " for reason " + reason);
                DestroyActivityLocked(activity, TRUE, reason);
                if ((*activities)[actNdx] != activity) {
                    // Was removed from list, back up so we don't miss the next one.
                    actNdx--;
                }
                curNum++;
            }
        }
        if (curNum > 0) {
            numReleased += curNum;
            maxTasks--;
            AutoPtr<IInterface> taskobj2;
            mTaskHistory->Get(taskNdx, (IInterface**)&taskobj2);
            TaskRecord* tr = (TaskRecord*)(IObject::Probe(taskobj2));
            if (tr != task) {
                // The entire task got removed, back up so we don't miss the next one.
                taskNdx--;
            }
        }
    }
    //if (ActivityStackSupervisor::DEBUG_RELEASE) Slogger::D(CActivityManagerService::TAG, "Done releasing: did " + numReleased + " activities");
    return numReleased;
}

Boolean ActivityStack::DestroyActivityLocked(
    /* [in] */ ActivityRecord* r,
    /* [in] */ Boolean removeFromApp,
    /* [in] */ const String& reason)
{
    //if (CActivityManagerService::DEBUG_SWITCH || CActivityManagerService::DEBUG_CLEANUP)
    //  Slog.v(
    //    CActivityManagerService::TAG, "Removing activity from " + reason + ": token=" + r
    //      + ", app=" + (r.app != NULL ? r.app.processName : "(NULL)"));
    //TODO EventLog.writeEvent(EventLogTags.AM_DESTROY_ACTIVITY,
    //        r.userId, System.identityHashCode(r),
    //        r.task.taskId, r.shortComponentName, reason);

    Boolean removedFromHistory = FALSE;

    CleanUpActivityLocked(r, FALSE, FALSE);

    const Boolean hadApp = r->mApp != NULL;

    if (hadApp) {
        if (removeFromApp) {
            AutoPtr<ActivityRecord> ar(r);//TODO use the AutoPtr type which is the type of elements in mActivities.
            r->mApp->mActivities.Remove(ar);
            if (mService->mHeavyWeightProcess == r->mApp && r->mApp->mActivities.GetSize() <= 0) {
                mService->mHeavyWeightProcess = NULL;
                Boolean result;
                mService->mHandler->SendEmptyMessage(
                        CActivityManagerService::CANCEL_HEAVY_NOTIFICATION_MSG, &result);
            }
            if (r->mApp->mActivities.IsEmpty()) {
                // Update any services we are bound to that might care about whether
                // their client may have activities.
                mService->mServices->UpdateServiceConnectionActivitiesLocked(r->mApp);
                // No longer have activities, so update LRU list and oom adj.
                mService->UpdateLruProcessLocked(r->mApp, FALSE, NULL);
                mService->UpdateOomAdjLocked();
            }
        }

        Boolean skipDestroy = FALSE;

        //try {
            if (CActivityManagerService::DEBUG_SWITCH)
                Slogger::I(CActivityManagerService::TAG, "Destroying: %s", r->ToString().string());
            ECode ec = r->mApp->mThread->ScheduleDestroyActivity(IBinder::Probe(r->mAppToken), r->mFinishing,
                    r->mConfigChangeFlags);
        //} catch (Exception e) {
        if (FAILED(ec)) {
            // We can just ignore exceptions here...  if the process
            // has crashed, our death notification will clean things
            // up.
            //Slogger::W(CActivityManagerService::TAG, "Exception thrown during finish", e);
            if (r->mFinishing) {
                RemoveActivityFromHistoryLocked(r);
                removedFromHistory = TRUE;
                skipDestroy = TRUE;
            }
        }
        //}

        r->mNowVisible = FALSE;

        // If the activity is finishing, we need to wait on removing it
        // from the list to give it a chance to do its cleanup.  During
        // that time it may make calls back with its token so we need to
        // be able to find it on the list and so we don't want to remove
        // it from the list yet.  Otherwise, we can just immediately put
        // it in the destroyed state since we are not removing it from the
        // list.
        if (r->mFinishing && !skipDestroy) {
            //if (ActivityStackSupervisor::DEBUG_STATES) Slog.v(CActivityManagerService::TAG, "Moving to DESTROYING: " + r
            //        + " (destroy requested)");
            r->mState = ActivityState_DESTROYING;
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(DESTROY_TIMEOUT_MSG, TO_IINTERFACE(r), (IMessage**)&msg);
            Boolean result;
            mHandler->SendMessageDelayed(msg, DESTROY_TIMEOUT, &result);
        } else {
            //if (ActivityStackSupervisor::DEBUG_STATES) Slog.v(CActivityManagerService::TAG, "Moving to DESTROYED: " + r + " (destroy skipped)");
            r->mState = ActivityState_DESTROYED;
            if (ActivityStackSupervisor::DEBUG_APP) Slogger::V(CActivityManagerService::TAG, "Clearing app during destroy for activity %s", r->ToString().string());
            r->mApp = NULL;
        }
    } else {
        // remove this record from the history.
        if (r->mFinishing) {
            RemoveActivityFromHistoryLocked(r);
            removedFromHistory = TRUE;
        } else {
            //if (ActivityStackSupervisor::DEBUG_STATES) Slog.v(CActivityManagerService::TAG, "Moving to DESTROYED: " + r + " (no app)");
            r->mState = ActivityState_DESTROYED;
            if (ActivityStackSupervisor::DEBUG_APP) Slogger::V(CActivityManagerService::TAG, "Clearing app during destroy for activity %s", r->ToString().string());
            r->mApp = NULL;
        }
    }

    r->mConfigChangeFlags = 0;

    Boolean removed;
    mLRUActivities->Remove(TO_IINTERFACE(r), &removed);
    if (!removed && hadApp) {
        Slogger::W(CActivityManagerService::TAG, "Activity %s being finished, but not in LRU list", r->ToString().string());
    }

    return removedFromHistory;
}

void ActivityStack::ActivityDestroyedLocked(
    /* [in] */ IBinder* token)
{
    Int64 origId;
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    binderHelper->ClearCallingIdentity(&origId);
    //try {
        AutoPtr<ActivityRecord> r = ActivityRecord::ForToken(token);
        if (r != NULL) {
            mHandler->RemoveMessages(DESTROY_TIMEOUT_MSG, TO_IINTERFACE(r));
        }
        if (ActivityStackSupervisor::DEBUG_CONTAINERS) Slogger::D(CActivityManagerService::TAG, "activityDestroyedLocked: r=%s", r->ToString().string());

        if (IsInStackLocked(token) != NULL) {
            if (r->mState == ActivityState_DESTROYING) {
                CleanUpActivityLocked(r, TRUE, FALSE);
                RemoveActivityFromHistoryLocked(r);
            }
        }
        mStackSupervisor->ResumeTopActivitiesLocked();
    //} finally {
        binderHelper->RestoreCallingIdentity(origId);
    //}
}

void ActivityStack::ReleaseBackgroundResources()
{
    Boolean hasMessages;
    if (HasVisibleBehindActivity() &&
            !(mHandler->HasMessages(RELEASE_BACKGROUND_RESOURCES_TIMEOUT_MSG, &hasMessages), hasMessages)) {
        AutoPtr<ActivityRecord> r = GetVisibleBehindActivity();
        if (r == TopRunningActivityLocked(NULL)) {
            // Don't release the top activity if it has requested to run behind the next
            // activity.
            return;
        }
        //if (ActivityStackSupervisor::DEBUG_STATES) Slogger::D(CActivityManagerService::TAG, "releaseBackgroundResources activtyDisplay=" +
        //        mActivityContainer.mActivityDisplay + " visibleBehind=" + r + " app=" + r.app +
        //        " thread=" + r.app.thread);
        if (r != NULL && r->mApp != NULL && r->mApp->mThread != NULL) {
            //try {
                r->mApp->mThread->ScheduleCancelVisibleBehind(IBinder::Probe(r->mAppToken));
            //} catch (RemoteException e) {
            //}
            Boolean res;
            mHandler->SendEmptyMessageDelayed(RELEASE_BACKGROUND_RESOURCES_TIMEOUT_MSG, 500, &res);
        } else {
            //Slog.e(CActivityManagerService::TAG, "releaseBackgroundResources: activity " + r + " no longer running");
            BackgroundResourcesReleased(IBinder::Probe(r->mAppToken));
        }
    }
}

void ActivityStack::BackgroundResourcesReleased(
    /* [in] */ IBinder* token)
{
    mHandler->RemoveMessages(RELEASE_BACKGROUND_RESOURCES_TIMEOUT_MSG);
    AutoPtr<ActivityRecord> r = GetVisibleBehindActivity();
    if (r != NULL) {
        mStackSupervisor->mStoppingActivities->Add(TO_IINTERFACE(r));
        SetVisibleBehindActivity(NULL);
    }
    mStackSupervisor->ResumeTopActivitiesLocked();
}

Boolean ActivityStack::HasVisibleBehindActivity()
{
    return IsAttached() && mActivityContainer->mActivityDisplay->HasVisibleBehindActivity();
}

void ActivityStack::SetVisibleBehindActivity(
    /* [in] */ ActivityRecord* r)
{
    if (IsAttached()) {
        mActivityContainer->mActivityDisplay->SetVisibleBehindActivity(r);
    }
}

AutoPtr<ActivityRecord> ActivityStack::GetVisibleBehindActivity()
{
    return IsAttached() ? mActivityContainer->mActivityDisplay->mVisibleBehindActivity : NULL;
}

Boolean ActivityStack::RemoveHistoryRecordsForAppLocked(
    /* [in] */ ProcessRecord* app)
{
    RemoveHistoryRecordsForAppLocked(mLRUActivities, app, String("mLRUActivities"));
    RemoveHistoryRecordsForAppLocked(mStackSupervisor->mStoppingActivities, app,
            String("mStoppingActivities"));
    RemoveHistoryRecordsForAppLocked(mStackSupervisor->mGoingToSleepActivities, app,
            String("mGoingToSleepActivities"));
    RemoveHistoryRecordsForAppLocked(mStackSupervisor->mWaitingVisibleActivities, app,
            String("mWaitingVisibleActivities"));
    RemoveHistoryRecordsForAppLocked(mStackSupervisor->mFinishingActivities, app,
            String("mFinishingActivities"));

    Boolean hasVisibleActivities = FALSE;

    // Clean out the history list.
    Int32 i = NumActivities();
    //if (CActivityManagerService::DEBUG_CLEANUP) Slog.v(
    //    CActivityManagerService::TAG, "Removing app " + app + " from history with " + i + " entries");
    Int32 taskSize;
    mTaskHistory->GetSize(&taskSize);
    for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
        //final ArrayList<ActivityRecord> activities = mTaskHistory.get(taskNdx).mActivities;
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            AutoPtr<ActivityRecord> r = (*activities)[activityNdx];
            --i;
            //if (CActivityManagerService::DEBUG_CLEANUP) Slog.v(
            //    CActivityManagerService::TAG, "Record #" + i + " " + r + ": app=" + r.app);
            if (r->mApp.Get() == app) {
                Boolean remove;
                if ((!r->mHaveState && !r->mStateNotNeeded) || r->mFinishing) {
                    // Don't currently have state for the activity, or
                    // it is finishing -- always remove it.
                    remove = TRUE;
                } else if (r->mLaunchCount > 2 &&
                        r->mLastLaunchTime > (SystemClock::GetUptimeMillis()-60000)) {
                    // We have launched this activity too many times since it was
                    // able to run, so give up and remove it.
                    remove = TRUE;
                } else {
                    // The process may be gone, but the activity lives on!
                    remove = FALSE;
                }
                if (remove) {
                    //if (ActivityStackSupervisor::DEBUG_ADD_REMOVE || CActivityManagerService::DEBUG_CLEANUP) {
                    //    RuntimeException here = new RuntimeException("here");
                    //    here.fillInStackTrace();
                    //    Slog.i(CActivityManagerService::TAG, "Removing activity " + r + " from stack at " + i
                    //            + ": haveState=" + r.haveState
                    //            + " stateNotNeeded=" + r.stateNotNeeded
                    //            + " finishing=" + r.finishing
                    //            + " state=" + r.state, here);
                    //}
                    if (!r->mFinishing) {
                        //Slogger::W(CActivityManagerService::TAG, "Force removing " + r + ": app died, no saved state");
                        //TODO EventLog.writeEvent(EventLogTags.AM_FINISH_ACTIVITY,
                        //        r.userId, System.identityHashCode(r),
                        //        r.task.taskId, r.shortComponentName,
                        //        "proc died without state saved");
                        if (r->mState == ActivityState_RESUMED) {
                            mService->UpdateUsageStats(r, FALSE);
                        }
                    }
                    RemoveActivityFromHistoryLocked(r);

                } else {
                    // We have the current state for this activity, so
                    // it can be restarted later when needed.
                    if (CActivityManagerService::localLOGV) Slogger::V( CActivityManagerService::TAG, "Keeping entry, setting app to NULL");
                    if (r->mVisible) {
                        hasVisibleActivities = TRUE;
                    }
                    if (ActivityStackSupervisor::DEBUG_APP) Slogger::V(CActivityManagerService::TAG, "Clearing app during removeHistory for activity %s", r->ToString().string());
                    r->mApp = NULL;
                    r->mNowVisible = FALSE;
                    if (!r->mHaveState) {
                        if (ActivityStackSupervisor::DEBUG_SAVED_STATE)
                            Slogger::I(CActivityManagerService::TAG, "App died, clearing saved state of %s", r->ToString().string());
                        r->mIcicle = NULL;
                    }
                }

                CleanUpActivityLocked(r, TRUE, TRUE);
            }
        }
    }

    return hasVisibleActivities;
}

void ActivityStack::UpdateTransitLocked(
    /* [in] */ Int32 transit,
    /* [in] */ IBundle* options)
{
    if (options != NULL) {
        AutoPtr<ActivityRecord> r = TopRunningActivityLocked(NULL);
        if (r != NULL && r->mState != ActivityState_RESUMED) {
            r->UpdateOptionsLocked(options);
        } else {
            AutoPtr<IActivityOptionsHelper> aoHelper;
            CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoHelper);
            aoHelper->Abort(options);
        }
    }
    mWindowManager->PrepareAppTransition(transit, FALSE);
}

void ActivityStack::UpdateTaskMovement(
    /* [in] */ TaskRecord* task,
    /* [in] */ Boolean toFront)
{
    if (task->mIsPersistable) {
        //task->mLastTimeMoved = System::CurrentTimeMillis();
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&(task->mLastTimeMoved));
        // Sign is used to keep tasks sorted when persisted. Tasks sent to the bottom most
        // recently will be most negative, tasks sent to the bottom before that will be less
        // negative. Similarly for recent tasks moved to the top which will be most positive.
        if (!toFront) {
            task->mLastTimeMoved *= -1;
        }
    }
}

void ActivityStack::MoveHomeStackTaskToTop(
    /* [in] */ Int32 homeStackTaskType)
{
    Int32 size;
    mTaskHistory->GetSize(&size);
    Int32 top = size - 1;
    for (Int32 taskNdx = top; taskNdx >= 0; --taskNdx) {
        //TaskRecord task = mTaskHistory.get(taskNdx);
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        if (task->mTaskType == homeStackTaskType) {
            //if (CActivityManagerService::DEBUG_TASKS || CActivityManagerService::DEBUG_STACK)
            //    Slogger::D(CActivityManagerService::TAG, "moveHomeStackTaskToTop: moving " + task);
            mTaskHistory->Remove(taskNdx);
            mTaskHistory->Add(top, TO_IINTERFACE(task));
            UpdateTaskMovement(task, TRUE);
            mWindowManager->MoveTaskToTop(task->mTaskId);
            return;
        }
    }
}

void ActivityStack::MoveTaskToFrontLocked(
    /* [in] */ TaskRecord* tr,
    /* [in] */ ActivityRecord* reason,
    /* [in] */ IBundle* options)
{
    //if (CActivityManagerService::DEBUG_SWITCH)
    //  Slog.v(CActivityManagerService::TAG, "moveTaskToFront: " + tr);

    Int32 numTasks;
    mTaskHistory->GetSize(&numTasks);
    Int32 index;
    mTaskHistory->IndexOf(TO_IINTERFACE(tr), &index);
    if (numTasks == 0 || index < 0)  {
        // nothing to do!
        Int32 flags;
        if (reason != NULL &&
                ((reason->mIntent->GetFlags(&flags), flags)&IIntent::FLAG_ACTIVITY_NO_ANIMATION) != 0) {
            //ActivityOptions.Abort(options);
            AutoPtr<IActivityOptionsHelper> aoHelper;
            CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoHelper);
            aoHelper->Abort(options);
        } else {
            UpdateTransitLocked(10/*TODO AppTransition::TRANSIT_TASK_TO_FRONT*/, options);
        }
        return;
    }

    MoveToFront();

    // Shift all activities with this task up to the top
    // of the stack, keeping them in the same internal order.
    InsertTaskAtTop(tr);

    //if (CActivityManagerService::DEBUG_TRANSITION)
    //  Slog.v(CActivityManagerService::TAG, "Prepare to front transition: task=" + tr);
    Int32 flags;
    if (reason != NULL &&
            ((reason->mIntent->GetFlags(&flags), flags)&IIntent::FLAG_ACTIVITY_NO_ANIMATION) != 0) {
        mWindowManager->PrepareAppTransition(0/*TODO AppTransition::TRANSIT_NONE*/, FALSE);
        AutoPtr<ActivityRecord> r = TopRunningActivityLocked(NULL);
        if (r != NULL) {
            mNoAnimActivities->Add(TO_IINTERFACE(r));
        }
        //ActivityOptions.abort(options);
        AutoPtr<IActivityOptionsHelper> aoHelper;
        CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoHelper);
        aoHelper->Abort(options);
    } else {
        UpdateTransitLocked(10/*TODO AppTransition::TRANSIT_TASK_TO_FRONT*/, options);
    }

    mWindowManager->MoveTaskToTop(tr->mTaskId);

    mStackSupervisor->ResumeTopActivitiesLocked();
    //TODO EventLog.writeEvent(EventLogTags.AM_TASK_TO_FRONT, tr.userId, tr.taskId);

    if (CActivityManagerService::VALIDATE_TOKENS) {
        ValidateAppTokensLocked();
    }
}

Boolean ActivityStack::MoveTaskToBackLocked(
    /* [in] */ Int32 taskId,
    /* [in] */ ActivityRecord* reason)
{
    AutoPtr<TaskRecord> tr = TaskForIdLocked(taskId);
    if (tr == NULL) {
        Slogger::I(CActivityManagerService::TAG, "moveTaskToBack: bad taskId=%d", taskId);
        return FALSE;
    }

    Slogger::I(CActivityManagerService::TAG, "moveTaskToBack: %s", tr->ToString().string());

    mStackSupervisor->EndLockTaskModeIfTaskEnding(tr);

    // If we have a watcher, preflight the move before committing to it.  First check
    // for *other* available tasks, but if none are available, then try again allowing the
    // current task to be selected.
    if (mStackSupervisor->IsFrontStack(this) && mService->mController != NULL) {
        AutoPtr<ActivityRecord> next = TopRunningActivityLocked(NULL, taskId);
        if (next == NULL) {
            next = TopRunningActivityLocked(NULL, 0);
        }
        if (next != NULL) {
            // ask watcher if this is allowed
            Boolean moveOK = TRUE;
            //try {
                ECode ec = mService->mController->ActivityResuming(next->mPackageName, &moveOK);
            //} catch (RemoteException e) {
            if (FAILED(ec)) {
                mService->mController = NULL;
                Watchdog::GetInstance()->SetActivityController(NULL);
            }
            //}
            if (!moveOK) {
                return FALSE;
            }
        }
    }

    if (CActivityManagerService::DEBUG_TRANSITION)
        Slogger::V(CActivityManagerService::TAG, "Prepare to back transition: task=%d", taskId);

    mTaskHistory->Remove(TO_IINTERFACE(tr));
    mTaskHistory->Add(0, TO_IINTERFACE(tr));
    UpdateTaskMovement(tr, FALSE);

    // There is an assumption that moving a task to the back moves it behind the home activity.
    // We make sure here that some activity in the stack will launch home.
    Int32 numTasks;
    mTaskHistory->GetSize(&numTasks);
    for (Int32 taskNdx = numTasks - 1; taskNdx >= 1; --taskNdx) {
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        if (task->IsOverHomeStack()) {
            break;
        }
        if (taskNdx == 1) {
            // Set the last task before tr to go to home.
            task->SetTaskToReturnTo(ActivityRecord::HOME_ACTIVITY_TYPE);
        }
    }

    Int32 flags;
    if (reason != NULL &&
            ((reason->mIntent->GetFlags(&flags), flags) & IIntent::FLAG_ACTIVITY_NO_ANIMATION) != 0) {
        mWindowManager->PrepareAppTransition(0/*TODO AppTransition::TRANSIT_NONE*/, FALSE);
        AutoPtr<ActivityRecord> r = TopRunningActivityLocked(NULL);
        if (r != NULL) {
            mNoAnimActivities->Add(TO_IINTERFACE(r));
        }
    } else {
        mWindowManager->PrepareAppTransition(11/*TODO AppTransition::TRANSIT_TASK_TO_BACK*/, FALSE);
    }
    mWindowManager->MoveTaskToBottom(taskId);

    if (CActivityManagerService::VALIDATE_TOKENS) {
        ValidateAppTokensLocked();
    }

    AutoPtr<TaskRecord> task = mResumedActivity != NULL ? mResumedActivity->mTask : NULL;
    if ((task == tr && tr->IsOverHomeStack()) || (numTasks <= 1 && IsOnHomeDisplay())) {
        if (!mService->mBooting && !mService->mBooted) {
            // Not ready yet!
            return FALSE;
        }
        Int32 taskToReturnTo = tr->GetTaskToReturnTo();
        tr->SetTaskToReturnTo(ActivityRecord::APPLICATION_ACTIVITY_TYPE);
        return mStackSupervisor->ResumeHomeStackTask(taskToReturnTo, NULL);
    }

    mStackSupervisor->ResumeTopActivitiesLocked();
    return TRUE;
}

void ActivityStack::LogStartActivity(
    /* [in] */ Int32 tag,
    /* [in] */ ActivityRecord* r,
    /* [in] */ TaskRecord* task)
{
    AutoPtr<IUri> data;
    r->mIntent->GetData((IUri**)&data);
    String strData;// = data != NULL ? data->ToSafeString() : NULL;
    if (data != NULL) {
        data->ToSafeString(&strData);
    }

    //TODO EventLog.writeEvent(tag,
    //         r.userId, System.identityHashCode(r), task.taskId,
    //         r.shortComponentName, r.intent.getAction(),
    //         r.intent.getType(), strData, r.intent.getFlags());
}

Boolean ActivityStack::EnsureActivityConfigurationLocked(
    /* [in] */ ActivityRecord* r,
    /* [in] */ Int32 globalChanges)
{
    if (mConfigWillChange) {
        //if (CActivityManagerService::DEBUG_SWITCH || CActivityManagerService::DEBUG_CONFIGURATION)
        //  Slog.v(CActivityManagerService::TAG,
        //        "Skipping config check (will change): " + r);
        return TRUE;
    }

    if (CActivityManagerService::DEBUG_SWITCH || CActivityManagerService::DEBUG_CONFIGURATION)
        Slogger::V(CActivityManagerService::TAG, "Ensuring correct configuration: %s", r->ToString().string());

    // Short circuit: if the two configurations are the exact same
    // object (the common case), then there is nothing to do.
    AutoPtr<IConfiguration> newConfig = mService->mConfiguration;
    if (r->mConfiguration == newConfig && !r->mForceNewConfig) {
        //if (CActivityManagerService::DEBUG_SWITCH || CActivityManagerService::DEBUG_CONFIGURATION)
        //  Slog.v(CActivityManagerService::TAG, "Configuration unchanged in " + r);
        return TRUE;
    }

    // We don't worry about activities that are finishing.
    if (r->mFinishing) {
        //if (CActivityManagerService::DEBUG_SWITCH || CActivityManagerService::DEBUG_CONFIGURATION)
        //  Slog.v(CActivityManagerService::TAG, "Configuration doesn't matter in finishing " + r);
        r->StopFreezingScreenLocked(FALSE);
        return TRUE;
    }

    // Okay we now are going to make this activity have the new config.
    // But then we need to figure out how it needs to deal with that.
    AutoPtr<IConfiguration> oldConfig = r->mConfiguration;
    r->mConfiguration = newConfig;

    // Determine what has changed.  May be nothing, if this is a config
    // that has come back from the app after going idle.  In that case
    // we just want to leave the official config object now in the
    // activity and do nothing else.
    Int32 changes;
    oldConfig->Diff(newConfig, &changes);
    if (changes == 0 && !r->mForceNewConfig) {
        //if (CActivityManagerService::DEBUG_SWITCH || CActivityManagerService::DEBUG_CONFIGURATION)
        //  Slog.v(CActivityManagerService::TAG, "Configuration no differences in " + r);
        return TRUE;
    }

    // If the activity isn't currently running, just leave the new
    // configuration and it will pick that up next time it starts.
    if (r->mApp == NULL || r->mApp->mThread == NULL) {
        //if (CActivityManagerService::DEBUG_SWITCH || CActivityManagerService::DEBUG_CONFIGURATION)
        //  Slog.v(CActivityManagerService::TAG,
        //        "Configuration doesn't matter not running " + r);
        r->StopFreezingScreenLocked(FALSE);
        r->mForceNewConfig = FALSE;
        return TRUE;
    }

    // Figure out how to handle the changes between the configurations.
    //if (CActivityManagerService::DEBUG_SWITCH || CActivityManagerService::DEBUG_CONFIGURATION) {
    //    Slog.v(CActivityManagerService::TAG, "Checking to restart " + r.info.name + ": changed=0x"
    //            + Integer.toHexString(changes) + ", handles=0x"
    //            + Integer.toHexString(r.info.getRealConfigChanged())
    //            + ", newConfig=" + newConfig);
    //}
    Int32 realConfigChanged;
    r->mInfo->GetRealConfigChanged(&realConfigChanged);
    if ((changes&(~realConfigChanged)) != 0 || r->mForceNewConfig) {
        // Aha, the activity isn't handling the change, so DIE DIE DIE.
        r->mConfigChangeFlags |= changes;
        r->StartFreezingScreenLocked(r->mApp, globalChanges);
        r->mForceNewConfig = FALSE;
        if (r->mApp == NULL || r->mApp->mThread == NULL) {
            //if (CActivityManagerService::DEBUG_SWITCH || CActivityManagerService::DEBUG_CONFIGURATION)
            //  Slog.v(CActivityManagerService::TAG,
            //        "Config is destroying non-running " + r);
            DestroyActivityLocked(r, TRUE, String("config"));
        } else if (r->mState == ActivityState_PAUSING) {
            // A little annoying: we are waiting for this activity to
            // finish pausing.  Let's not do anything now, but just
            // flag that it needs to be restarted when done pausing.
            //if (CActivityManagerService::DEBUG_SWITCH || CActivityManagerService::DEBUG_CONFIGURATION)
            //  Slog.v(CActivityManagerService::TAG,
            //        "Config is skipping already pausing " + r);
            r->mConfigDestroy = TRUE;
            return TRUE;
        } else if (r->mState == ActivityState_RESUMED) {
            // Try to optimize this case: the configuration is changing
            // and we need to restart the top, resumed activity.
            // Instead of doing the normal handshaking, just say
            // "restart!".
            //if (CActivityManagerService::DEBUG_SWITCH || CActivityManagerService::DEBUG_CONFIGURATION)
            //  Slog.v(CActivityManagerService::TAG,
            //        "Config is relaunching resumed " + r);
            RelaunchActivityLocked(r, r->mConfigChangeFlags, TRUE);
            r->mConfigChangeFlags = 0;
        } else {
            //if (CActivityManagerService::DEBUG_SWITCH || CActivityManagerService::DEBUG_CONFIGURATION)
            //  Slog.v(CActivityManagerService::TAG,
            //        "Config is relaunching non-resumed " + r);
            RelaunchActivityLocked(r, r->mConfigChangeFlags, FALSE);
            r->mConfigChangeFlags = 0;
        }

        // All done...  tell the caller we weren't able to keep this
        // activity around.
        return FALSE;
    }

    // Default case: the activity can handle this new configuration, so
    // hand it over.  Note that we don't need to give it the new
    // configuration, since we always send configuration changes to all
    // process when they happen so it can just use whatever configuration
    // it last got.
    if (r->mApp != NULL && r->mApp->mThread != NULL) {
        //try {
            //if (CActivityManagerService::DEBUG_CONFIGURATION) Slog.v(CActivityManagerService::TAG, "Sending new config to " + r);
            r->mApp->mThread->ScheduleActivityConfigurationChanged(IBinder::Probe(r->mAppToken));
        //} catch (RemoteException e) {
            // If process died, whatever.
        //}
    }
    r->StopFreezingScreenLocked(FALSE);

    return TRUE;
}

Boolean ActivityStack::WillActivityBeVisibleLocked(
    /* [in] */ IBinder* token)
{
    Int32 taskSize;
    mTaskHistory->GetSize(&taskSize);
    for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        //final ArrayList<ActivityRecord> activities = mTaskHistory.get(taskNdx).mActivities;
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            //final ActivityRecord r = activities.get(activityNdx);
            AutoPtr<ActivityRecord> r = (*activities)[activityNdx];
            if (IBinder::Probe(r->mAppToken) == token) {
                return TRUE;
            }
            if (r->mFullscreen && !r->mFinishing) {
                return FALSE;
            }
        }
    }
    AutoPtr<ActivityRecord> r = ActivityRecord::ForToken(token);
    if (r == NULL) {
        return FALSE;
    }
    //if (r->mFinishing) Slog.e(CActivityManagerService::TAG, "willActivityBeVisibleLocked: Returning false,"
    //        + " would have returned true for r=" + r);
    return !r->mFinishing;
}

void ActivityStack::CloseSystemDialogsLocked()
{
    Int32 taskSize;
    mTaskHistory->GetSize(&taskSize);
    for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        //final ArrayList<ActivityRecord> activities = mTaskHistory.get(taskNdx).mActivities;
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            AutoPtr<ActivityRecord> r = (*activities)[activityNdx];
            Int32 flags;
            r->mInfo->GetFlags(&flags);
            if ((flags&IActivityInfo::FLAG_FINISH_ON_CLOSE_SYSTEM_DIALOGS) != 0) {
                FinishActivityLocked(r, IActivity::RESULT_CANCELED, NULL, String("close-sys"), TRUE);
            }
        }
    }
}

Boolean ActivityStack::ForceStopPackageLocked(
    /* [in] */ const String& name,
    /* [in] */ Boolean doit,
    /* [in] */ Boolean evenPersistent,
    /* [in] */ Int32 userId)
{
    Boolean didSomething = FALSE;
    AutoPtr<TaskRecord> lastTask = NULL;
    AutoPtr<IComponentName> homeActivity = NULL;
    Int32 taskSize;
    mTaskHistory->GetSize(&taskSize);
    for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
        //final ArrayList<ActivityRecord> activities = mTaskHistory.get(taskNdx).mActivities;
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        Int32 numActivities = activities->GetSize();
        for (Int32 activityNdx = 0; activityNdx < numActivities; ++activityNdx) {
            AutoPtr<ActivityRecord> r = (*activities)[activityNdx];
            Boolean samePackage = r->mPackageName.Equals(name)
                    || (name == NULL && r->mUserId == userId);
            if ((userId == IUserHandle::USER_ALL || r->mUserId == userId)
                    && (samePackage || r->mTask == lastTask)
                    && (r->mApp == NULL || evenPersistent || !r->mApp->mPersistent)) {
                if (!doit) {
                    if (r->mFinishing) {
                        // If this activity is just finishing, then it is not
                        // interesting as far as something to stop.
                        continue;
                    }
                    return TRUE;
                }
                if (r->IsHomeActivity()) {
                    Int32 result;
                    //if (homeActivity != null && homeActivity.equals(r.realActivity))
                    if (homeActivity != NULL
                        && (IComparable::Probe(homeActivity)->CompareTo(TO_IINTERFACE(r->mRealActivity), &result), result == 0)) {
                        Slogger::I(CActivityManagerService::TAG, "Skip force-stop again %s", r->ToString().string());
                        continue;
                    } else {
                        homeActivity = r->mRealActivity;
                    }
                }
                didSomething = TRUE;
                Slogger::I(CActivityManagerService::TAG, "  Force finishing activity %s", r->ToString().string());
                if (samePackage) {
                    if (r->mApp != NULL) {
                        r->mApp->mRemoved = TRUE;
                    }
                    r->mApp = NULL;
                }
                lastTask = r->mTask;
                if (FinishActivityLocked(r, IActivity::RESULT_CANCELED, NULL, String("force-stop"),
                        TRUE)) {
                    // r has been deleted from mActivities, accommodate.
                    --numActivities;
                    --activityNdx;
                }
            }
        }
    }
    return didSomething;
}

void ActivityStack::GetTasksLocked(
    /* [in] */ IList* list,
    /* [in] */ Int32 callingUid,
    /* [in] */ Boolean allowed)
{
    Int32 taskSize;
    mTaskHistory->GetSize(&taskSize);
    for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
        //final TaskRecord task = mTaskHistory.get(taskNdx);
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        AutoPtr<ActivityRecord> r = NULL;
        AutoPtr<ActivityRecord> top = NULL;
        Int32 numActivities = 0;
        Int32 numRunning = 0;
        //final ArrayList<ActivityRecord> activities = task.mActivities;
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        if (activities->IsEmpty()) {
            continue;
        }
        if (!allowed && !task->IsHomeTask() && task->mEffectiveUid != callingUid) {
            continue;
        }
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            r = (*activities)[activityNdx];

            // Initialize state for next task if needed.
            if (top == NULL || (top->mState == ActivityState_INITIALIZING)) {
                top = r;
                numActivities = numRunning = 0;
            }

            // Add 'r' into the current task.
            numActivities++;
            if (r->mApp != NULL && r->mApp->mThread != NULL) {
                numRunning++;
            }

            //if (CActivityManagerService::localLOGV) Slog.v(
            //    CActivityManagerService::TAG, r.intent.getComponent().flattenToShortString()
            //    + ": task=" + r.task);
        }

        AutoPtr<IActivityManagerRunningTaskInfo> ci;
        CActivityManagerRunningTaskInfo::New((IActivityManagerRunningTaskInfo**)&ci);
        ci->SetId(task->mTaskId);
        AutoPtr<IComponentName> bActivity;
        r->mIntent->GetComponent((IComponentName**)&bActivity);
        ci->SetBaseActivity(bActivity);
        AutoPtr<IComponentName> tActivity;
        top->mIntent->GetComponent((IComponentName**)&tActivity);
        ci->SetTopActivity(tActivity);
        ci->SetLastActiveTime(task->mLastActiveTime);

        if (top->mTask != NULL) {
            ci->SetDescription(top->mTask->mLastDescription);
        }
        ci->SetNumActivities(numActivities);
        ci->SetNumRunning(numRunning);
        //System.out.println(
        //    "#" + maxNum + ": " + " descr=" + ci.description);
        list->Add(TO_IINTERFACE(ci));
    }
}

void ActivityStack::UnhandledBackLocked()
{
    Int32 top;
    mTaskHistory->GetSize(&top);
    --top;
    //if (CActivityManagerService::DEBUG_SWITCH)
    //  Slogger::D(
    //    CActivityManagerService::TAG, "Performing unhandledBack(): top activity at " + top);
    if (top >= 0) {
        //final ArrayList<ActivityRecord> activities = mTaskHistory.get(top).mActivities;
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(top, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        Int32 activityTop = activities->GetSize() - 1;
        if (activityTop > 0) {
            FinishActivityLocked((*activities)[activityTop], IActivity::RESULT_CANCELED, NULL,
                    String("unhandled-back"), TRUE);
        }
    }
}

Boolean ActivityStack::HandleAppDiedLocked(
    /* [in] */ ProcessRecord* app)
{
    if (mPausingActivity != NULL && mPausingActivity->mApp.Get() == app) {
        //if (CActivityManagerService::DEBUG_PAUSE || CActivityManagerService::DEBUG_CLEANUP)
        //  Slog.v(CActivityManagerService::TAG,
        //        "App died while pausing: " + mPausingActivity);
        mPausingActivity = NULL;
    }
    if (mLastPausedActivity != NULL && mLastPausedActivity->mApp.Get() == app) {
        mLastPausedActivity = NULL;
        mLastNoHistoryActivity = NULL;
    }

    return RemoveHistoryRecordsForAppLocked(app);
}

void ActivityStack::HandleAppCrashLocked(
    /* [in] */ ProcessRecord* app)
{
    Int32 taskSize;
    mTaskHistory->GetSize(&taskSize);
    for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
        //final ArrayList<ActivityRecord> activities = mTaskHistory.get(taskNdx).mActivities;
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            AutoPtr<ActivityRecord> r = (*activities)[activityNdx];
            if (r->mApp.Get() == app) {
                //Slogger::W(CActivityManagerService::TAG, "  Force finishing activity "
                //        + r.intent.getComponent().flattenToShortString());
                // Force the destroy to skip right to removal.
                r->mApp = NULL;
                FinishCurrentActivityLocked(r, FINISH_IMMEDIATELY, FALSE);
            }
        }
    }
}

Boolean ActivityStack::DumpActivitiesLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Boolean dumpAll,
    /* [in] */ Boolean dumpClient,
    /* [in] */ const String& dumpPackage,
    /* [in] */ Boolean needSep,
    /* [in] */ const String& _header)
{
    String header = _header;
    Boolean printed = false;
    Int32 taskSize;
    mTaskHistory->GetSize(&taskSize);
    for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
        //final TaskRecord task = mTaskHistory.get(taskNdx);
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        AutoPtr<IList> list;
        CArrayList::New((IList**)&list);
        AutoPtr<List<AutoPtr<ActivityRecord> > > ars = task->mActivities;
        for (UInt32 activityNdx = 0; activityNdx < ars->GetSize(); ++activityNdx) {
            AutoPtr<ActivityRecord> ar = (*ars)[activityNdx];
            list->Add(TO_IINTERFACE(ar.Get()));
        }
        printed |= ActivityStackSupervisor::DumpHistoryList(fd, pw,
                list, String("    "), String("Hist"), TRUE, !dumpAll,
                dumpClient, dumpPackage, needSep, header,
                String("    Task id #") + StringUtils::ToString(task->mTaskId));
        if (printed) {
            header = NULL;
        }
    }
    return printed;
}

AutoPtr<IArrayList> ActivityStack::GetDumpActivitiesLocked(
    /* [in] */ const String& name)
{
    //ArrayList<ActivityRecord> activities = new ArrayList<ActivityRecord>();
    AutoPtr<IArrayList> activities;
    CArrayList::New((IArrayList**)&activities);

    if (String("all").Equals(name)) {
        Int32 taskSize;
        mTaskHistory->GetSize(&taskSize);
        for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
            AutoPtr<IInterface> taskobj;
            mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
            TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
            AutoPtr<List<AutoPtr<ActivityRecord> > > ars = task->mActivities;
            for (Int32 activityNdx = ars->GetSize() - 1; activityNdx >= 0; --activityNdx) {
                AutoPtr<ActivityRecord> r = (*ars)[activityNdx];
                activities->Add(TO_IINTERFACE(r));
            }
        }
    } else if (String("top").Equals(name)) {
        Int32 top;
        mTaskHistory->GetSize(&top);
        --top;
        if (top >= 0) {
            //final ArrayList<ActivityRecord> list = mTaskHistory.get(top).mActivities;
            AutoPtr<IInterface> taskobj;
            mTaskHistory->Get(top, (IInterface**)&taskobj);
            TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
            AutoPtr<List<AutoPtr<ActivityRecord> > > list = task->mActivities;
            Int32 listTop = list->GetSize() - 1;
            if (listTop >= 0) {
                AutoPtr<ActivityRecord> r = (*list)[listTop];
                activities->Add(TO_IINTERFACE(r));
            }
        }
    } else {
        AutoPtr<CActivityManagerService::ItemMatcher> matcher = new CActivityManagerService::ItemMatcher();
        matcher->Build(name);

        Int32 taskSize;
        mTaskHistory->GetSize(&taskSize);
        for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
            //for (ActivityRecord r1 : mTaskHistory.get(taskNdx).mActivities) {
            //    if (matcher.match(r1, r1.intent.getComponent())) {
            //        activities.add(r1);
            //    }
            //}
            AutoPtr<IInterface> taskobj;
            mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
            TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
            AutoPtr<List<AutoPtr<ActivityRecord> > > ars = task->mActivities;
            for (Int32 activityNdx = ars->GetSize() - 1; activityNdx >= 0; --activityNdx) {
                AutoPtr<ActivityRecord> r = (*ars)[activityNdx];
                AutoPtr<IComponentName> cn;
                r->mIntent->GetComponent((IComponentName**)&cn);
                if (matcher->Match(TO_IINTERFACE(r), cn)) {
                    activities->Add(TO_IINTERFACE(r));
                }
            }
        }
    }
    return activities;
}

AutoPtr<ActivityRecord> ActivityStack::RestartPackage(
    /* [in] */ const String& packageName)
{
    AutoPtr<ActivityRecord> starting = TopRunningActivityLocked(NULL);

    // All activities that came from the package must be
    // restarted as if there was a config change.
    Int32 taskSize;
    mTaskHistory->GetSize(&taskSize);
    for (Int32 taskNdx = taskSize - 1; taskNdx >= 0; --taskNdx) {
        //final ArrayList<ActivityRecord> activities = mTaskHistory.get(taskNdx).mActivities;
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
        for (Int32 activityNdx = activities->GetSize() - 1; activityNdx >= 0; --activityNdx) {
            //final ActivityRecord a = activities.get(activityNdx);
            AutoPtr<ActivityRecord> a = (*activities)[activityNdx];
            String pn;
            IPackageItemInfo::Probe(a->mInfo)->GetPackageName(&pn);
            if (pn.Equals(packageName)) {
                a->mForceNewConfig = TRUE;
                if (starting != NULL && a == starting && a->mVisible) {
                    a->StartFreezingScreenLocked(starting->mApp,
                            IActivityInfo::CONFIG_SCREEN_LAYOUT);
                }
            }
        }
    }

    return starting;
}

void ActivityStack::RemoveTask(
    /* [in] */ TaskRecord* task)
{
    mStackSupervisor->EndLockTaskModeIfTaskEnding(task);
    mWindowManager->RemoveTask(task->mTaskId);
    AutoPtr<ActivityRecord> r = mResumedActivity;
    if (r != NULL && r->mTask.Get() == task) {
        mResumedActivity = NULL;
    }

    Int32 taskNdx;
    mTaskHistory->IndexOf(TO_IINTERFACE(task), &taskNdx);
    Int32 topTaskNdx;
    mTaskHistory->GetSize(&topTaskNdx);
    --topTaskNdx;
    if (task->IsOverHomeStack() && taskNdx < topTaskNdx) {
        AutoPtr<IInterface> taskobj;
        mTaskHistory->Get(taskNdx + 1, (IInterface**)&taskobj);
        TaskRecord* nextTask = (TaskRecord*)(IObject::Probe(taskobj));
        if (!nextTask->IsOverHomeStack()) {
            nextTask->SetTaskToReturnTo(ActivityRecord::HOME_ACTIVITY_TYPE);
        }
    }
    mTaskHistory->Remove(TO_IINTERFACE(task));
    UpdateTaskMovement(task, TRUE);

    if (task->mActivities->IsEmpty()) {
        Boolean isVoiceSession = task->mVoiceSession != NULL;
        if (isVoiceSession) {
            //try {
                task->mVoiceSession->TaskFinished(task->mIntent, task->mTaskId);
            //} catch (RemoteException e) {
            //}
        }
        if (task->AutoRemoveFromRecents() || isVoiceSession) {
            // Task creator asked to remove this when done, or this task was a voice
            // interaction, so it should not remain on the recent tasks list.
            mService->mRecentTasks->Remove(task);
            task->RemovedFromRecents(mService->mTaskPersister);
        }
    }

    Boolean bTemp;
    if ((mTaskHistory->IsEmpty(&bTemp), bTemp)) {
        //if (CActivityManagerService::DEBUG_STACK)
        //    Slog.i(CActivityManagerService::TAG, "removeTask: moving to back stack=" + this);
        if (IsOnHomeDisplay()) {
            mStackSupervisor->MoveHomeStack(!IsHomeStack());
        }
        if (mStacks != NULL) {
            mStacks->Remove(TO_IINTERFACE(this));
            mStacks->Add(0, TO_IINTERFACE(this));
        }
        mActivityContainer->OnTaskListEmptyLocked();
    }
}

AutoPtr<TaskRecord> ActivityStack::CreateTaskRecord(
    /* [in] */ Int32 taskId,
    /* [in] */ IActivityInfo* info,
    /* [in] */ IIntent* intent,
    /* [in] */ IIVoiceInteractionSession* voiceSession,
    /* [in] */ IIVoiceInteractor* voiceInteractor,
    /* [in] */ Boolean toTop)
{
    AutoPtr<TaskRecord> task = new TaskRecord();
    task->constructor(mService, taskId, info, intent, voiceSession,
            voiceInteractor);
    AddTask(task, toTop, FALSE);
    return task;
}

AutoPtr<IArrayList> ActivityStack::GetAllTasks()
{
    AutoPtr<IArrayList> result;
    CArrayList::New(ICollection::Probe(mTaskHistory), (IArrayList**)&result);
    return result;
}

void ActivityStack::AddTask(
    /* [in] */ TaskRecord* task,
    /* [in] */ Boolean toTop,
    /* [in] */ Boolean moving)
{
    task->mStack = this;
    if (toTop) {
        InsertTaskAtTop(task);
    } else {
        mTaskHistory->Add(0, TO_IINTERFACE(task));
        UpdateTaskMovement(task, FALSE);
    }
    if (!moving && task->mVoiceSession != NULL) {
        //try {
        task->mVoiceSession->TaskStarted(task->mIntent, task->mTaskId);
        //} catch (RemoteException e) {
        //}
    }
}

Int32 ActivityStack::GetStackId()
{
    return mStackId;
}

ECode ActivityStack::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ToString();
    return NOERROR;
}

String ActivityStack::ToString()
{
    Int32 hash;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->IdentityHashCode(TO_IINTERFACE(this), &hash);

    Int32 taskSize;
    mTaskHistory->GetSize(&taskSize);
    return String("ActivityStack{") + StringUtils::ToHexString(hash)
             + String(" stackId=") + StringUtils::ToString(mStackId) + String(", ")
             + StringUtils::ToString(taskSize) + String(" tasks}");
}

Boolean ActivityStack::IsCurrentProfileLocked(
    /* [in] */ Int32 userId)
{
    if (userId == mCurrentUser) return TRUE;
    for (Int32 i = 0; i < mService->mCurrentProfileIds->GetLength(); ++i) {
        if ((*mService->mCurrentProfileIds)[i] == userId) return TRUE;
    }
    return FALSE;
}

void ActivityStack::StartLaunchTraces()
{
    AutoPtr<ITrace> trace;
    //TODO CTrace::AcquireSingleton((ITrace**)&trace);
    if (mFullyDrawnStartTime != 0)  {
        trace->AsyncTraceEnd(1L << 6/*TODO ITrace::TRACE_TAG_ACTIVITY_MANAGER*/, String("drawing"), 0);
    }
    trace->AsyncTraceBegin(1L << 6/*TODO ITrace::TRACE_TAG_ACTIVITY_MANAGER*/, String("launching"), 0);
    trace->AsyncTraceBegin(1L << 6/*TODO ITrace::TRACE_TAG_ACTIVITY_MANAGER*/, String("drawing"), 0);
}

void ActivityStack::StopFullyDrawnTraceIfNeeded()
{
    if (mFullyDrawnStartTime != 0 && mLaunchStartTime == 0) {
        AutoPtr<ITrace> trace;
        //TODO CTrace::AcquireSingleton((ITrace**)&trace);
        trace->AsyncTraceEnd(1L << 6/*TODO ITrace::TRACE_TAG_ACTIVITY_MANAGER*/, String("drawing"), 0);
        mFullyDrawnStartTime = 0;
    }
}

void ActivityStack::InvalidateLastScreenshot()
{
    mLastScreenshotActivity = NULL;
    if (mLastScreenshotBitmap != NULL) {
        mLastScreenshotBitmap->Recycle();
    }
    mLastScreenshotBitmap = NULL;
}

void ActivityStack::CompletePauseLocked(
    /* [in] */ Boolean resumeNext)
{
    AutoPtr<ActivityRecord> prev = mPausingActivity;
    if (CActivityManagerService::DEBUG_PAUSE)
        Slogger::V(CActivityManagerService::TAG, "Complete pause: %s", prev->ToString().string());

    if (prev != NULL) {
        prev->mState = ActivityState_PAUSED;
        if (prev->mFinishing) {
            if (CActivityManagerService::DEBUG_PAUSE)
                Slogger::V(CActivityManagerService::TAG, "Executing finish of activity: %s", prev->ToString().string());
            prev = FinishCurrentActivityLocked(prev, FINISH_AFTER_VISIBLE, FALSE);
        } else if (prev->mApp != NULL) {
            if (CActivityManagerService::DEBUG_PAUSE)
                Slogger::V(CActivityManagerService::TAG, "Enqueueing pending stop: %s", prev->ToString().string());
            if (prev->mWaitingVisible) {
                prev->mWaitingVisible = FALSE;
                mStackSupervisor->mWaitingVisibleActivities->Remove(TO_IINTERFACE(prev));
                if (CActivityManagerService::DEBUG_SWITCH || CActivityManagerService::DEBUG_PAUSE)
                    Slogger::V(
                        CActivityManagerService::TAG, "Complete pause, no longer waiting: %s", prev->ToString().string());
            }
            if (prev->mConfigDestroy) {
                // The previous is being paused because the configuration
                // is changing, which means it is actually stopping...
                // To juggle the fact that we are also starting a new
                // instance right now, we need to first completely stop
                // the current instance before starting the new one.
                if (CActivityManagerService::DEBUG_PAUSE)
                    Slogger::V(CActivityManagerService::TAG, "Destroying after pause: %s", prev->ToString().string());
                DestroyActivityLocked(prev, TRUE, String("pause-config"));
            } else if (!HasVisibleBehindActivity()) {
                // If we were visible then resumeTopActivities will release resources before
                // stopping.
                mStackSupervisor->mStoppingActivities->Add(TO_IINTERFACE(prev));
                Int32 stoppingActivitiesSize = 0;
                Int32 taskHistorySize = 0;
                mStackSupervisor->mStoppingActivities->GetSize(&stoppingActivitiesSize);
                mTaskHistory->GetSize(&taskHistorySize);
                if (stoppingActivitiesSize > 3 || (prev->mFrontOfTask && taskHistorySize <= 1)) {
                    // If we already have a few activities waiting to stop,
                    // then give up on things going idle and start clearing
                    // them out. Or if r is the last of activity of the last task the stack
                    // will be empty and must be cleared immediately.
                    if (CActivityManagerService::DEBUG_PAUSE)
                        Slogger::V(CActivityManagerService::TAG, "To many pending stops, forcing idle");
                    mStackSupervisor->ScheduleIdleLocked();
                } else {
                    mStackSupervisor->CheckReadyForSleepLocked();
                }
            }
        } else {
            if (CActivityManagerService::DEBUG_PAUSE)
                Slogger::V(CActivityManagerService::TAG, "App died during pause, not stopping: %s", prev->ToString().string());
            prev = NULL;
        }
        mPausingActivity = NULL;
    }

    if (resumeNext) {
        AutoPtr<ActivityStack> topStack = mStackSupervisor->GetFocusedStack();
        if (!mService->IsSleepingOrShuttingDown()) {
            mStackSupervisor->ResumeTopActivitiesLocked(topStack, prev, NULL);
        } else {
            mStackSupervisor->CheckReadyForSleepLocked();
            AutoPtr<ActivityRecord> top = topStack->TopRunningActivityLocked(NULL);
            if (top == NULL || (prev != NULL && top != prev)) {
                // If there are no more activities available to run,
                // do resume anyway to start something.  Also if the top
                // activity on the stack is not the just paused activity,
                // we need to go ahead and resume it to ensure we complete
                // an in-flight app switch.
                mStackSupervisor->ResumeTopActivitiesLocked(topStack, NULL, NULL);
            }
        }
    }

    if (prev != NULL) {
        prev->ResumeKeyDispatchingLocked();

        if (prev->mApp != NULL && prev->mCpuTimeAtResume > 0
                && FALSE/*TODO mService->mBatteryStatsService->IsOnBattery()*/) {
            Int64 diff;
            mService->mProcessCpuTracker->GetCpuTimeForPid(prev->mApp->mPid, &diff);
            diff -= prev->mCpuTimeAtResume;
            if (diff > 0) {
                //TODO AutoPtr<BatteryStatsImpl> bsi = mService->mBatteryStatsService->GetActiveStatistics();
                {
                    //TODO AutoLock lock(bsi);
                    AutoPtr<IBatteryStatsImplUidProc> ps;
                    AutoPtr<IUserHandleHelper> helper;
                    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
                    AutoPtr<IApplicationInfo> appInfo;
                    IComponentInfo::Probe(prev->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
                    Int32 uid;
                    appInfo->GetUid(&uid);
                    Int32 userId;
                    helper->GetUserId(uid, &userId);
                    String packageName;
                    IPackageItemInfo::Probe(prev->mInfo)->GetPackageName(&packageName);
                    //TODO bsi->GetProcessStatsLocked(userId, packageName, (IBatteryStatsImplUidProc**)&ps);
                    if (ps != NULL) {
                        ps->AddForegroundTimeLocked(diff);
                    }
                }
            }
        }
        prev->mCpuTimeAtResume = 0; // reset it
    }
}

void ActivityStack::CompleteResumeLocked(
    /* [in] */ ActivityRecord* next)
{
    next->mIdle = FALSE;
    next->mResults = NULL;
    next->mNewIntents = NULL;

    if (next->IsHomeActivity() && next->IsNotResolverActivity()) {
        AutoPtr<List<AutoPtr<ActivityRecord> > > activities = next->mTask->mActivities;
        AutoPtr<ProcessRecord> app = ((*activities)[0])->mApp;
        if (app != NULL && app != mService->mHomeProcess) {
            mService->mHomeProcess = app;
        }
    }

    if (next->mNowVisible) {
        // We won't get a call to reportActivityVisibleLocked() so dismiss lockscreen now.
        mStackSupervisor->NotifyActivityDrawnForKeyguard();
    }

    // schedule an idle timeout in case the app doesn't do it for us.
    mStackSupervisor->ScheduleIdleTimeoutLocked(next);

    mStackSupervisor->ReportResumedActivityLocked(next);

    next->ResumeKeyDispatchingLocked();
    mNoAnimActivities->Clear();

    // Mark the point when the activity is resuming
    // TODO: To be more accurate, the mark should be before the onCreate,
    //       not after the onResume. But for subsequent starts, onResume is fine.
    if (next->mApp != NULL) {
        mService->mProcessCpuTracker->GetCpuTimeForPid(next->mApp->mPid, &(next->mCpuTimeAtResume));
    } else {
        next->mCpuTimeAtResume = 0; // Couldn't get the cpu time of process
    }

    // If we are resuming the activity that we had last screenshotted, then we know it will be
    // updated, so invalidate the last screenshot to ensure we take a fresh one when requested
    if (next == mLastScreenshotActivity) {
        InvalidateLastScreenshot();
    }
    next->mReturningOptions = NULL;

    if (mActivityContainer->mActivityDisplay->mVisibleBehindActivity.Get() == next) {
        // When resuming an activity, require it to call requestVisibleBehind() again.
        mActivityContainer->mActivityDisplay->SetVisibleBehindActivity(NULL);
    }
}

void ActivityStack::SetVisibile(
    /* [in] */ ActivityRecord* r,
    /* [in] */ Boolean visible)
{
    r->mVisible = visible;
    mWindowManager->SetAppVisibility(IBinder::Probe(r->mAppToken), visible);
    //final ArrayList<ActivityContainer> containers = r.mChildContainers;
    List<AutoPtr<IIActivityContainer> > containers = r->mChildContainers;
    for (Int32 containerNdx = containers.GetSize() - 1; containerNdx >= 0; --containerNdx) {
        AutoPtr<IIActivityContainer> icontainer = containers[containerNdx];
        ActivityStackSupervisor::ActivityContainer* container = (ActivityStackSupervisor::ActivityContainer*)(icontainer.Get());
        container->SetVisible(visible);
    }
}

Boolean ActivityStack::IsStackVisible()
{
    if (!IsAttached()) {
        return FALSE;
    }

    if (mStackSupervisor->IsFrontStack(this)) {
        return TRUE;
    }

    /**
     * Start at the task above this one and go up, looking for a visible
     * fullscreen activity, or a translucent activity that requested the
     * wallpaper to be shown behind it.
     */
    Int32 size;
    mStacks->GetSize(&size);
    Int32 stackNdx;
    mStacks->IndexOf(TO_IINTERFACE(this), &stackNdx);
    for (Int32 i = stackNdx + 1; i < size; ++i) {
        //final ArrayList<TaskRecord> tasks = mStacks.get(i).GetAllTasks();
        AutoPtr<IInterface> obj;
        mStacks->Get(i, (IInterface**)&obj);
        ActivityStack* as = (ActivityStack*)(IObject::Probe(obj));
        AutoPtr<IArrayList> tasks = as->GetAllTasks();
        Int32 tasksSize;
        tasks->GetSize(&tasksSize);
        for (Int32 taskNdx = 0; taskNdx < tasksSize; taskNdx++) {
            AutoPtr<IInterface> taskobj;
            tasks->Get(taskNdx, (IInterface**)&taskobj);
            TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
            //ArrayList<ActivityRecord> activities = task.mActivities;
            AutoPtr<List<AutoPtr<ActivityRecord> > > activities = task->mActivities;
            for (UInt32 activityNdx = 0; activityNdx < activities->GetSize(); activityNdx++) {
                AutoPtr<ActivityRecord> r = (*activities)[activityNdx];

                // Conditions for an activity to obscure the stack we're
                // examining:
                // 1. Not Finishing AND Visible AND:
                // 2. Either:
                // - Full Screen Activity OR
                // - On top of Home and our stack is NOT home
                if (!r->mFinishing && r->mVisible && (r->mFullscreen ||
                        (!IsHomeStack() && r->mFrontOfTask && task->IsOverHomeStack()))) {
                    return FALSE;
                }
            }
        }
    }

    return TRUE;
}

void ActivityStack::InsertTaskAtTop(
    /* [in] */ TaskRecord* task)
{
    // If this is being moved to the top by another activity or being launched from the home
    // activity, set mOnTopOfHome accordingly.
    if (IsOnHomeDisplay()) {
        AutoPtr<ActivityStack> lastStack = mStackSupervisor->GetLastStack();
        Boolean fromHome = lastStack->IsHomeStack();
        if (!IsHomeStack() && (fromHome || TopTask().Get() != task)) {
            task->SetTaskToReturnTo(fromHome
                    ? lastStack->TopTask() == NULL
                            ? ActivityRecord::HOME_ACTIVITY_TYPE
                            : lastStack->TopTask()->mTaskType
                    : ActivityRecord::APPLICATION_ACTIVITY_TYPE);
        }
    } else {
        task->SetTaskToReturnTo(ActivityRecord::APPLICATION_ACTIVITY_TYPE);
    }

    mTaskHistory->Remove(TO_IINTERFACE(task));
    // Now put task at top.
    Int32 taskNdx;
    mTaskHistory->GetSize(&taskNdx);
    if (!IsCurrentProfileLocked(task->mUserId)) {
        // Put non-current user tasks below current user tasks.
        while (--taskNdx >= 0) {
            AutoPtr<IInterface> taskobj;
            mTaskHistory->Get(taskNdx, (IInterface**)&taskobj);
            TaskRecord* tr = (TaskRecord*)(IObject::Probe(taskobj));
            if (!IsCurrentProfileLocked(tr->mUserId)) {
                break;
            }
        }
        ++taskNdx;
    }
    mTaskHistory->Add(taskNdx, TO_IINTERFACE(task));
    UpdateTaskMovement(task, TRUE);
}

Int32 ActivityStack::ResetAffinityTaskIfNeededLocked(
    /* [in] */ TaskRecord* affinityTask,
    /* [in] */ TaskRecord* task,
    /* [in] */ Boolean topTaskIsHigher,
    /* [in] */ Boolean forceReset,
    /* [in] */ Int32 taskInsertionPoint)
{
    Int32 replyChainEnd = -1;
    Int32 taskId = task->mTaskId;
    String taskAffinity = task->mAffinity;

    //final ArrayList<ActivityRecord> activities = affinityTask.mActivities;
    AutoPtr<List<AutoPtr<ActivityRecord> > > activities = affinityTask->mActivities;
    Int32 numActivities = activities->GetSize();
    Int32 rootActivityNdx = affinityTask->FindEffectiveRootIndex();

    // Do not operate on or below the effective root Activity.
    for (Int32 i = numActivities - 1; i > rootActivityNdx; --i) {
        AutoPtr<ActivityRecord> target = (*activities)[i];
        if (target->mFrontOfTask)
            break;

        Int32 flags;
        target->mInfo->GetFlags(&flags);
        Boolean finishOnTaskLaunch = (flags & IActivityInfo::FLAG_FINISH_ON_TASK_LAUNCH) != 0;
        Boolean allowTaskReparenting = (flags & IActivityInfo::FLAG_ALLOW_TASK_REPARENTING) != 0;

        if (target->mResultTo != NULL) {
            // If this activity is sending a reply to a previous
            // activity, we can't do anything with it now until
            // we reach the start of the reply chain.
            // XXX note that we are assuming the result is always
            // to the previous activity, which is almost always
            // the case but we really shouldn't count on.
            if (replyChainEnd < 0) {
                replyChainEnd = i;
            }
        } else if (topTaskIsHigher
                && allowTaskReparenting
                //&& taskAffinity != NULL
                && taskAffinity.Equals(target->mTaskAffinity)) {
            // This activity has an affinity for our task. Either remove it if we are
            // clearing or move it over to our task.  Note that
            // we currently punt on the case where we are resetting a
            // task that is not at the top but who has activities above
            // with an affinity to it...  this is really not a normal
            // case, and we will need to later pull that task to the front
            // and usually at that point we will do the reset and pick
            // up those remaining activities.  (This only happens if
            // someone starts an activity in a new task from an activity
            // in a task that is not currently on top.)
            if (forceReset || finishOnTaskLaunch) {
                Int32 start = replyChainEnd >= 0 ? replyChainEnd : i;
                //if (CActivityManagerService::DEBUG_TASKS) Slog.v(CActivityManagerService::TAG, "Finishing task at index " + start + " to " + i);
                for (Int32 srcPos = start; srcPos >= i; --srcPos) {
                    AutoPtr<ActivityRecord> p = (*activities)[srcPos];
                    if (p->mFinishing) {
                        continue;
                    }
                    FinishActivityLocked(p, IActivity::RESULT_CANCELED, NULL, String("reset"), FALSE);
                }
            } else {
                if (taskInsertionPoint < 0) {
                    taskInsertionPoint = task->mActivities->GetSize();

                }

                Int32 start = replyChainEnd >= 0 ? replyChainEnd : i;
                //if (CActivityManagerService::DEBUG_TASKS) Slog.v(CActivityManagerService::TAG, "Reparenting from task=" + affinityTask + ":"
                //        + start + "-" + i + " to task=" + task + ":" + taskInsertionPoint);
                for (Int32 srcPos = start; srcPos >= i; --srcPos) {
                    AutoPtr<ActivityRecord> p = (*activities)[srcPos];
                    p->SetTask(task, NULL);
                    task->AddActivityAtIndex(taskInsertionPoint, p);

                    //if (ActivityStackSupervisor::DEBUG_ADD_REMOVE) Slog.i(CActivityManagerService::TAG, "Removing and adding activity " + p
                    //        + " to stack at " + task,
                    //        new RuntimeException("here").fillInStackTrace());
                    //if (CActivityManagerService::DEBUG_TASKS) Slog.v(CActivityManagerService::TAG, "Pulling activity " + p + " from " + srcPos
                    //        + " in to resetting task " + task);
                    mWindowManager->SetAppGroupId(IBinder::Probe(p->mAppToken), taskId);
                }
                assert(taskId);//TODO remove
                mWindowManager->MoveTaskToTop(taskId);
                if (CActivityManagerService::VALIDATE_TOKENS) {
                    ValidateAppTokensLocked();
                }

                // Now we've moved it in to place...  but what if this is
                // a singleTop activity and we have put it on top of another
                // instance of the same activity?  Then we drop the instance
                // below so it remains singleTop.
                AutoPtr<IActivityInfo> info = target->mInfo;
                Int32 launchMode;
                info->GetLaunchMode(&launchMode);
                if (launchMode == IActivityInfo::LAUNCH_SINGLE_TOP) {
                    //ArrayList<ActivityRecord> taskActivities = task.mActivities;
                    AutoPtr<List<AutoPtr<ActivityRecord> > > taskActivities = task->mActivities;
                    //int targetNdx = taskActivities.IndexOf(target);
                    Int32 targetNdx = -1;//IndexOf
                    for(UInt32 i = 0; i < taskActivities->GetSize(); ++i) {
                        if (((*activities)[i]).Get() == target) {
                            targetNdx = i;
                            break;
                        }
                    }

                    if (targetNdx > 0) {
                        AutoPtr<ActivityRecord> p = (*taskActivities)[targetNdx - 1];
                        AutoPtr<IIntent> pIntent = p->mIntent;
                        AutoPtr<IComponentName> pcls;
                        pIntent->GetComponent((IComponentName**)&pcls);
                        AutoPtr<IIntent> tIntent = target->mIntent;
                        AutoPtr<IComponentName> tcls;
                        tIntent->GetComponent((IComponentName**)&tcls);
                        Int32 cInt32;
                        IComparable::Probe(pcls)->CompareTo(TO_IINTERFACE(tcls), &cInt32);
                        Boolean bTemp = cInt32 == 0;
                        if (bTemp) {
                            FinishActivityLocked(p, IActivity::RESULT_CANCELED, NULL, String("replace"),
                                    FALSE);
                        }
                    }
                }
            }

            replyChainEnd = -1;
        }
    }
    return taskInsertionPoint;
}

void ActivityStack::AdjustFocusedActivityLocked(
    /* [in] */ ActivityRecord* r)
{
    if (mStackSupervisor->IsFrontStack(this) && mService->mFocusedActivity.Get() == r) {
        AutoPtr<ActivityRecord> next = TopRunningActivityLocked(NULL);
        if (next.Get() != r) {
            AutoPtr<TaskRecord> task = r->mTask;
            if (r->mFrontOfTask && task == TopTask() && task->IsOverHomeStack()) {
                mStackSupervisor->MoveHomeStackTaskToTop(task->GetTaskToReturnTo());
            }
        }
        AutoPtr<ActivityRecord> top = mStackSupervisor->TopRunningActivityLocked();
        if (top != NULL) {
            mService->SetFocusedActivityLocked(top);
        }
    }
}

void ActivityStack::RemoveTimeoutsForActivityLocked(
    /* [in] */ ActivityRecord* r)
{
    mStackSupervisor->RemoveTimeoutsForActivityLocked(r);
    mHandler->RemoveMessages(PAUSE_TIMEOUT_MSG, TO_IINTERFACE(r));
    mHandler->RemoveMessages(STOP_TIMEOUT_MSG, TO_IINTERFACE(r));
    mHandler->RemoveMessages(DESTROY_TIMEOUT_MSG, TO_IINTERFACE(r));
    r->FinishLaunchTickingLocked();
}

void ActivityStack::RemoveActivityFromHistoryLocked(
    /* [in] */ ActivityRecord* r)
{
    mStackSupervisor->RemoveChildActivityContainers(r);
    FinishActivityResultsLocked(r, IActivity::RESULT_CANCELED, NULL);
    r->MakeFinishing();
    //if (ActivityStackSupervisor::DEBUG_ADD_REMOVE) {
    //    RuntimeException here = new RuntimeException("here");
    //    here.fillInStackTrace();
    //    Slog.i(CActivityManagerService::TAG, "Removing activity " + r + " from stack");
    //}
    r->TakeFromHistory();
    RemoveTimeoutsForActivityLocked(r);
    //if (ActivityStackSupervisor::DEBUG_STATES) Slog.v(CActivityManagerService::TAG, "Moving to DESTROYED: " + r + " (removed from history)");
    r->mState = ActivityState_DESTROYED;
    //if (ActivityStackSupervisor::DEBUG_APP) Slog.v(CActivityManagerService::TAG, "Clearing app during remove for activity " + r);
    r->mApp = NULL;
    mWindowManager->RemoveAppToken(IBinder::Probe(r->mAppToken));
    if (CActivityManagerService::VALIDATE_TOKENS) {
        ValidateAppTokensLocked();
    }
    AutoPtr<TaskRecord> task = r->mTask;
    if (task != NULL && task->RemoveActivity(r)) {
        //if (CActivityManagerService::DEBUG_STACK)
        //  Slog.i(CActivityManagerService::TAG, "removeActivityFromHistoryLocked: last activity removed from " + this);
        if (mStackSupervisor->IsFrontStack(this) && task == TopTask() &&
                task->IsOverHomeStack()) {
            mStackSupervisor->MoveHomeStackTaskToTop(task->GetTaskToReturnTo());
        }
        RemoveTask(task);
    }
    CleanUpActivityServicesLocked(r);
    r->RemoveUriPermissionsLocked();
}

void ActivityStack::RemoveHistoryRecordsForAppLocked(
    /* [in] */ IArrayList* list,
    /* [in] */ ProcessRecord* app,
    /* [in] */ const String& listName)
{
    Int32 i;
    list->GetSize(&i);
    //if (CActivityManagerService::DEBUG_CLEANUP) Slog.v(
    //    CActivityManagerService::TAG, "Removing app " + app + " from list " + listName
    //    + " with " + i + " entries");
    while (i > 0) {
        i--;
        AutoPtr<IInterface> activityobj;
        list->Get(i, (IInterface**)&activityobj);
        ActivityRecord* r = (ActivityRecord*)(IObject::Probe(activityobj));
        if (CActivityManagerService::DEBUG_CLEANUP)
            Slogger::V(CActivityManagerService::TAG, "Record #%d %s",  i, r->ToString().string());
        if (r->mApp.Get() == app) {
            if (CActivityManagerService::DEBUG_CLEANUP)
                Slogger::V(CActivityManagerService::TAG, "---> REMOVING this entry!");
            list->Remove(i);
            RemoveTimeoutsForActivityLocked(r);
        }
    }
}

Boolean ActivityStack::RelaunchActivityLocked(
    /* [in] */ ActivityRecord* r,
    /* [in] */ Int32 changes,
    /* [in] */ Boolean andResume)
{
    //List<ResultInfo> results = NULL;
    //List<Intent> newIntents = NULL;
    AutoPtr< List< AutoPtr<ActivityResult> > > results;
    AutoPtr< List< AutoPtr<IIntent> > > newIntents;
    if (andResume) {
        results = r->mResults;
        newIntents = r->mNewIntents;
    }
    //if (CActivityManagerService::DEBUG_SWITCH)
    //  Slog.v(CActivityManagerService::TAG, "Relaunching: " + r
    //        + " with results=" + results + " newIntents=" + newIntents
    //        + " andResume=" + andResume);
    //TODO EventLog.writeEvent(andResume ? EventLogTags.AM_RELAUNCH_RESUME_ACTIVITY
    //        : EventLogTags.AM_RELAUNCH_ACTIVITY, r.userId, System.identityHashCode(r),
    //        r.task.taskId, r.shortComponentName);

    r->StartFreezingScreenLocked(r->mApp, 0);

    mStackSupervisor->RemoveChildActivityContainers(r);
    AutoPtr<IList> iResults;
    AutoPtr<IList> iNewIntents;
    CArrayList::New((IList**)&iResults);
    CArrayList::New((IList**)&iNewIntents);
    List< AutoPtr<ActivityResult> >::Iterator resultIter = results->Begin();
    while (resultIter != results->End()) {
        AutoPtr<ActivityResult> ar = *resultIter;
        iResults->Add(TO_IINTERFACE(ar));
        ++resultIter;
    }
    List< AutoPtr<IIntent> >::Iterator newIntentsIter = newIntents->Begin();
    while (newIntentsIter != newIntents->End()) {
        AutoPtr<IIntent> intent = *newIntentsIter;
        iNewIntents->Add(TO_IINTERFACE(intent));
        ++newIntentsIter;
    }

    //try {
        //if (CActivityManagerService::DEBUG_SWITCH || ActivityStackSupervisor::DEBUG_STATES)
        //  Slog.i(CActivityManagerService::TAG,
        //        (andResume ? "Relaunching to RESUMED " : "Relaunching to PAUSED ")
        //        + r);
        r->mForceNewConfig = FALSE;
        AutoPtr<IConfiguration> config;
        CConfiguration::New(mService->mConfiguration, (IConfiguration**)&config);
        r->mApp->mThread->ScheduleRelaunchActivity(IBinder::Probe(r->mAppToken), iResults, iNewIntents,
                changes, !andResume, config);
        // Note: don't need to call pauseIfSleepingLocked() here, because
        // the caller will only pass in 'andResume' if this activity is
        // currently resumed, which implies we aren't sleeping.
    //} catch (RemoteException e) {
        //if (CActivityManagerService::DEBUG_SWITCH || ActivityStackSupervisor::DEBUG_STATES)
        //  Slog.i(CActivityManagerService::TAG, "Relaunch failed", e);
    //}

    if (andResume) {
        r->mResults = NULL;
        r->mNewIntents = NULL;
        r->mState = ActivityState_RESUMED;
    } else {
        mHandler->RemoveMessages(PAUSE_TIMEOUT_MSG, TO_IINTERFACE(r));
        r->mState = ActivityState_PAUSED;
    }

    return TRUE;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

