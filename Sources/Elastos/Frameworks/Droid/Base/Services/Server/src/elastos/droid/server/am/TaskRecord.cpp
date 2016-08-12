
#include "elastos/droid/server/am/TaskRecord.h"
#include "elastos/droid/server/am/ActivityRecord.h"
#include "elastos/droid/server/am/ActivityStack.h"
#include "elastos/droid/server/am/ActivityStackSupervisor.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/am/TaskPersister.h"
#include <elastos/droid/app/AppGlobals.h>
#include <elastos/droid/internal/utility/XmlUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerTaskDescription;
using Elastos::Droid::App::CActivityManagerTaskThumbnail;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::IO::CFile;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

#define TO_ActivityRecord(obj) \
    ((ActivityRecord*)IActivityRecord::Probe(obj))

const String TaskRecord::TAG("TaskRecord");
const String TaskRecord::ATTR_TASKID("task_id");
const String TaskRecord::TAG_INTENT("intent");
const String TaskRecord::TAG_AFFINITYINTENT("affinity_intent");
const String TaskRecord::ATTR_REALACTIVITY("real_activity");
const String TaskRecord::ATTR_ORIGACTIVITY("orig_activity");
const String TaskRecord::TAG_ACTIVITY("activity");
const String TaskRecord::ATTR_AFFINITY("affinity");
const String TaskRecord::ATTR_ROOT_AFFINITY("root_affinity");
const String TaskRecord::ATTR_ROOTHASRESET("root_has_reset");
const String TaskRecord::ATTR_AUTOREMOVERECENTS("auto_remove_recents");
const String TaskRecord::ATTR_ASKEDCOMPATMODE("asked_compat_mode");
const String TaskRecord::ATTR_USERID("user_id");
const String TaskRecord::ATTR_EFFECTIVE_UID("effective_uid");
const String TaskRecord::ATTR_TASKTYPE("task_type");
const String TaskRecord::ATTR_FIRSTACTIVETIME("first_active_time");
const String TaskRecord::ATTR_LASTACTIVETIME("last_active_time");
const String TaskRecord::ATTR_LASTDESCRIPTION("last_description");
const String TaskRecord::ATTR_LASTTIMEMOVED("last_time_moved");
const String TaskRecord::ATTR_NEVERRELINQUISH("never_relinquish_identity");
const String TaskRecord::ATTR_TASK_AFFILIATION("task_affiliation");
const String TaskRecord::ATTR_PREV_AFFILIATION("prev_affiliation");
const String TaskRecord::ATTR_NEXT_AFFILIATION("next_affiliation");
const String TaskRecord::ATTR_TASK_AFFILIATION_COLOR("task_affiliation_color");
const String TaskRecord::ATTR_CALLING_UID("calling_uid");
const String TaskRecord::ATTR_CALLING_PACKAGE("calling_package");
const String TaskRecord::TASK_THUMBNAIL_SUFFIX("_task_thumbnail");

const Boolean TaskRecord::IGNORE_RETURN_TO_RECENTS;

TaskRecord::TaskRecord()
    : mTaskId(0)
    , mEffectiveUid(0)
    , mFirstActiveTime(0)
    , mLastActiveTime(0)
    , mInRecents(FALSE)
    , mIsAvailable(FALSE)
    , mRootWasReset(FALSE)
    , mAutoRemoveRecents(FALSE)
    , mAskedCompatMode(FALSE)
    , mHasBeenVisible(FALSE)
    , mUserId(0)
    , mCreatorUid(0)
    , mStack(NULL)
    , mTaskType(0)
    , mIsPersistable(FALSE)
    , mMaxRecents(0)
    , mLastTimeMoved(0)
    , mNeverRelinquishIdentity(TRUE)
    , mReuseTask(FALSE)
    , mAffiliatedTaskId(0)
    , mAffiliatedTaskColor(0)
    , mPrevAffiliateTaskId(-1)
    , mNextAffiliateTaskId(-1)
    , mCallingUid(0)
    , mService(NULL)
    , mTaskToReturnTo(ActivityRecord::APPLICATION_ACTIVITY_TYPE)
{
    CActivityManagerTaskDescription::New((IActivityManagerTaskDescription**)&mLastTaskDescription);
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mLastTimeMoved);
}

ECode TaskRecord::constructor(
    /* [in] */ CActivityManagerService* service,
    /* [in] */ Int32 taskId,
    /* [in] */ IActivityInfo* info,
    /* [in] */ IIntent* intent,
    /* [in] */ IIVoiceInteractionSession* voiceSession,
    /* [in] */ IIVoiceInteractor* voiceInteractor)
{
    mService = service;
    mFilename = StringUtils::ToString(taskId) + TASK_THUMBNAIL_SUFFIX + TaskPersister::IMAGE_EXTENSION;
    CFile::New(TaskPersister::sImagesDir, mFilename, (IFile**)&mLastThumbnailFile);
    mTaskId = taskId;
    mAffiliatedTaskId = taskId;
    mVoiceSession = voiceSession;
    mVoiceInteractor = voiceInteractor;
    mIsAvailable = TRUE;
    CArrayList::New((IArrayList**)&mActivities);
    SetIntent(intent, info);
    return NOERROR;
}

ECode TaskRecord::constructor(
    /* [in] */ CActivityManagerService* service,
    /* [in] */ Int32 taskId,
    /* [in] */ IActivityInfo* info,
    /* [in] */ IIntent* intent,
    /* [in] */ IActivityManagerTaskDescription* taskDescription)
{
    mService = service;
    mFilename = StringUtils::ToString(taskId) + TASK_THUMBNAIL_SUFFIX + TaskPersister::IMAGE_EXTENSION;
    CFile::New(TaskPersister::sImagesDir, mFilename, (IFile**)&mLastThumbnailFile);
    mTaskId = taskId;
    mAffiliatedTaskId = taskId;
    mVoiceSession = NULL;
    mVoiceInteractor = NULL;
    mIsAvailable = TRUE;
    CArrayList::New((IArrayList**)&mActivities);
    SetIntent(intent, info);

    mTaskType = ActivityRecord::APPLICATION_ACTIVITY_TYPE;
    mIsPersistable = TRUE;
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(info)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    appInfo->GetUid(&mCallingUid);
    IPackageItemInfo::Probe(info)->GetPackageName(&mCallingPackage);
    // Clamp to [1, max].
    Int32 maxRecents;
    info->GetMaxRecents(&maxRecents);
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    Int32 maxRecentsLimit;
    amHelper->GetMaxAppRecentsLimitStatic(&maxRecentsLimit);
    mMaxRecents = Elastos::Core::Math::Min(Elastos::Core::Math::Max(maxRecents, 1),
            maxRecentsLimit);

    mTaskType = ActivityRecord::APPLICATION_ACTIVITY_TYPE;
    mTaskToReturnTo = ActivityRecord::HOME_ACTIVITY_TYPE;
    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    uhHelper->GetUserId(mCallingUid, &mUserId);
    mLastTaskDescription = taskDescription;

    return NOERROR;
}

ECode TaskRecord::constructor(
    /* [in] */ CActivityManagerService* service,
    /* [in] */ Int32 taskId,
    /* [in] */ IIntent* intent,
    /* [in] */ IIntent* affinityIntent,
    /* [in] */ const String& affinity,
    /* [in] */ const String& rootAffinity,
    /* [in] */ IComponentName* realActivity,
    /* [in] */ IComponentName* origActivity,
    /* [in] */ Boolean rootWasReset,
    /* [in] */ Boolean autoRemoveRecents,
    /* [in] */ Boolean askedCompatMode,
    /* [in] */ Int32 taskType,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 effectiveUid,
    /* [in] */ const String& lastDescription,
    /* [in] */ IArrayList* activities,
    /* [in] */ Int64 firstActiveTime,
    /* [in] */ Int64 lastActiveTime,
    /* [in] */ Int64 lastTimeMoved,
    /* [in] */ Boolean neverRelinquishIdentity,
    /* [in] */ IActivityManagerTaskDescription* lastTaskDescription,
    /* [in] */ Int32 taskAffiliation,
    /* [in] */ Int32 prevTaskId,
    /* [in] */ Int32 nextTaskId,
    /* [in] */ Int32 taskAffiliationColor,
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& callingPackage)
{
    mService = service;
    mFilename = StringUtils::ToString(taskId) + TASK_THUMBNAIL_SUFFIX +
            TaskPersister::IMAGE_EXTENSION;
    CFile::New(TaskPersister::sImagesDir, mFilename, (IFile**)&mLastThumbnailFile);
    mTaskId = taskId;
    mIntent = intent;
    mAffinityIntent = affinityIntent;
    mAffinity = affinity;
    mRootAffinity = affinity;
    mRealActivity = realActivity;
    mOrigActivity = origActivity;
    mRootWasReset = rootWasReset;
    mIsAvailable = TRUE;
    mAutoRemoveRecents = autoRemoveRecents;
    mAskedCompatMode = askedCompatMode;
    mTaskType = taskType;
    mTaskToReturnTo = ActivityRecord::HOME_ACTIVITY_TYPE;
    mUserId = userId;
    mEffectiveUid = effectiveUid;
    mFirstActiveTime = firstActiveTime;
    mLastActiveTime = lastActiveTime;
    mLastDescription = CoreUtils::Convert(lastDescription);
    mActivities = activities;
    mLastTimeMoved = lastTimeMoved;
    mNeverRelinquishIdentity = neverRelinquishIdentity;
    mLastTaskDescription = lastTaskDescription;
    mAffiliatedTaskId = taskAffiliation;
    mAffiliatedTaskColor = taskAffiliationColor;
    mPrevAffiliateTaskId = prevTaskId;
    mNextAffiliateTaskId = nextTaskId;
    mCallingUid = callingUid;
    mCallingPackage = callingPackage;

    return NOERROR;
}

void TaskRecord::TouchActiveTime()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mLastActiveTime);
    if (mFirstActiveTime == 0) {
        mFirstActiveTime = mLastActiveTime;
    }
}

Int64 TaskRecord::GetInactiveDuration()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    return now - mLastActiveTime;
}

/** Sets the original intent, and the calling uid and package. */
void TaskRecord::SetIntent(
    /* [in] */ ActivityRecord* r)
{
    SetIntent(r->mIntent, r->mInfo);
    mCallingUid = r->mLaunchedFromUid;
    mCallingPackage = r->mLaunchedFromPackage;
}

void TaskRecord::SetIntent(
    /* [in] */ IIntent* _intent,
    /* [in] */ IActivityInfo* info)
{
    AutoPtr<IIntent> intent = _intent;
    if (mIntent == NULL) {
        Int32 flags;
        info->GetFlags(&flags);
        mNeverRelinquishIdentity =
                (flags & IActivityInfo::FLAG_RELINQUISH_TASK_IDENTITY) == 0;
    }
    else if (mNeverRelinquishIdentity) {
        return;
    }

    info->GetTaskAffinity(&mAffinity);
    if (mIntent == NULL) {
        // If this task already has an intent associated with it, don't set the root
        // affinity -- we don't want it changing after initially set, but the initially
        // set value may be NULL.
        mRootAffinity = mAffinity;
    }
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(info)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 uid;
    appInfo->GetUid(&uid);
    mEffectiveUid = uid;
    mStringName = NULL;

    String targetActivity;
    info->GetTargetActivity(&targetActivity);
    if (targetActivity == NULL) {
        if (intent != NULL) {
            // If this Intent has a selector, we want to clear it for the
            // recent task since it is not relevant if the user later wants
            // to re-launch the app.
            AutoPtr<IIntent> selector;
            intent->GetSelector((IIntent**)&selector);
            AutoPtr<IRect> bounds;
            if (selector != NULL || (intent->GetSourceBounds((IRect**)&bounds), bounds != NULL)) {
                AutoPtr<IIntent> newIntent;
                CIntent::New(intent, (IIntent**)&newIntent);
                intent = newIntent;
                intent->SetSelector(NULL);
                intent->SetSourceBounds(NULL);
            }
        }
        if (CActivityManagerService::DEBUG_TASKS) {
            String strIntent;
            IObject::Probe(intent)->ToString(&strIntent);
            Slogger::V(TAG, "Setting Intent of %s to %s", ToString().string(), strIntent.string());
        }
        mIntent = intent;
        mRealActivity = NULL;
        if (intent != NULL)
            intent->GetComponent((IComponentName**)&mRealActivity);
        mOrigActivity = NULL;
    }
    else {
        String packageName;
        IPackageItemInfo::Probe(info)->GetPackageName(&packageName);
        AutoPtr<IComponentName> targetComponent;
        CComponentName::New(packageName, targetActivity, (IComponentName**)&targetComponent);
        if (intent != NULL) {
            AutoPtr<IIntent> targetIntent;
            CIntent::New(intent, (IIntent**)&targetIntent);
            targetIntent->SetComponent(targetComponent);
            targetIntent->SetSelector(NULL);
            targetIntent->SetSourceBounds(NULL);
            if (CActivityManagerService::DEBUG_TASKS) {
                String strIntent;
                IObject::Probe(targetIntent)->ToString(&strIntent);
                Slogger::V(TAG, "Setting Intent of %s to target %s", ToString().string(), strIntent.string());
            }
            mIntent = targetIntent;
            mRealActivity = targetComponent;
            mOrigActivity = NULL;
            intent->GetComponent((IComponentName**)&mOrigActivity);
        }
        else {
            mIntent = NULL;
            mRealActivity = targetComponent;
            String name;
            IPackageItemInfo::Probe(info)->GetName(&name);
            mOrigActivity = NULL;
            CComponentName::New(packageName, name, (IComponentName**)&mOrigActivity);
        }
    }

    Int32 intentFlags = 0;
    if (mIntent != NULL)
        mIntent->GetFlags(&intentFlags);
    if ((intentFlags & IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED) != 0) {
        // Once we are set to an Intent with this flag, we count this
        // task as having a TRUE root activity.
        mRootWasReset = TRUE;
    }

    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    uhHelper->GetUserId(uid, &mUserId);
    Int32 flags;
    info->GetFlags(&flags);
    if ((flags & IActivityInfo::FLAG_AUTO_REMOVE_FROM_RECENTS) != 0) {
        // If the activity itself has requested auto-remove, then just always do it.
        mAutoRemoveRecents = TRUE;
    }
    else if ((intentFlags & (IIntent::FLAG_ACTIVITY_NEW_DOCUMENT
        /*| IIntent::FLAG_ACTIVITY_RETAIN_IN_RECENTS*/)) == IIntent::FLAG_ACTIVITY_NEW_DOCUMENT) {
        // If the caller has not asked for the document to be retained, then we may
        // want to turn on auto-remove, depending on whether the target has set its
        // own document launch mode.
        Int32 documentLaunchMode;
        info->GetDocumentLaunchMode(&documentLaunchMode);
        if (documentLaunchMode != IActivityInfo::DOCUMENT_LAUNCH_NONE) {
            mAutoRemoveRecents = FALSE;
        }
        else {
            mAutoRemoveRecents = TRUE;
        }
    }
    else {
        mAutoRemoveRecents = FALSE;
    }
}

void TaskRecord::SetTaskToReturnTo(
    /* [in] */ Int32 taskToReturnTo)
{
    if (IGNORE_RETURN_TO_RECENTS && taskToReturnTo == ActivityRecord::RECENTS_ACTIVITY_TYPE) {
        taskToReturnTo = ActivityRecord::HOME_ACTIVITY_TYPE;
    }
    mTaskToReturnTo = taskToReturnTo;
}

Int32 TaskRecord::GetTaskToReturnTo()
{
    return mTaskToReturnTo;
}

void TaskRecord::SetPrevAffiliate(
    /* [in] */ TaskRecord* prevAffiliate)
{
    mPrevAffiliate = prevAffiliate;
    mPrevAffiliateTaskId = prevAffiliate == NULL ? -1 : prevAffiliate->mTaskId;
}

void TaskRecord::SetNextAffiliate(
    /* [in] */ TaskRecord* nextAffiliate)
{
    mNextAffiliate = nextAffiliate;
    mNextAffiliateTaskId = nextAffiliate == NULL ? -1 : nextAffiliate->mTaskId;
}

// Close up recents linked list.
void TaskRecord::CloseRecentsChain()
{
    if (mPrevAffiliate != NULL) {
        mPrevAffiliate->SetNextAffiliate(mNextAffiliate);
    }
    if (mNextAffiliate != NULL) {
        mNextAffiliate->SetPrevAffiliate(mPrevAffiliate);
    }
    SetPrevAffiliate(NULL);
    SetNextAffiliate(NULL);
}

void TaskRecord::RemovedFromRecents(
    /* [in] */ TaskPersister* persister)
{
    DisposeThumbnail();
    CloseRecentsChain();
    if (mInRecents) {
        mInRecents = FALSE;
        persister->Wakeup(this, FALSE);
    }
}

void TaskRecord::SetTaskToAffiliateWith(
    /* [in] */ TaskRecord* _taskToAffiliateWith)
{
    AutoPtr<TaskRecord> taskToAffiliateWith = _taskToAffiliateWith;
    CloseRecentsChain();
    mAffiliatedTaskId = taskToAffiliateWith->mAffiliatedTaskId;
    mAffiliatedTaskColor = taskToAffiliateWith->mAffiliatedTaskColor;
    // Find the end
    while (taskToAffiliateWith->mNextAffiliate != NULL) {
        AutoPtr<TaskRecord> nextRecents = taskToAffiliateWith->mNextAffiliate;
        if (nextRecents->mAffiliatedTaskId != mAffiliatedTaskId) {
            Slogger::E(TAG, "setTaskToAffiliateWith: nextRecents=%s affilTaskId=%d should be %d",
                nextRecents->ToString().string(), nextRecents->mAffiliatedTaskId, mAffiliatedTaskId);
            if (nextRecents->mPrevAffiliate == taskToAffiliateWith) {
                nextRecents->SetPrevAffiliate(NULL);
            }
            taskToAffiliateWith->SetNextAffiliate(NULL);
            break;
        }
        taskToAffiliateWith = nextRecents;
    }
    taskToAffiliateWith->SetNextAffiliate(this);
    SetPrevAffiliate(taskToAffiliateWith);
    SetNextAffiliate(NULL);
}

Boolean TaskRecord::SetLastThumbnail(
    /* [in] */ IBitmap* thumbnail)
{
    if (mLastThumbnail.Get() != thumbnail) {
        mLastThumbnail = thumbnail;
        if (thumbnail == NULL) {
            if (mLastThumbnailFile != NULL) {
                mLastThumbnailFile->Delete();
            }
        }
        else {
            mService->mTaskPersister->SaveImage(thumbnail, mFilename);
        }
        return TRUE;
    }
    return FALSE;
}

void TaskRecord::GetLastThumbnail(
    /* [in] */ IActivityManagerTaskThumbnail* thumbs)
{
    thumbs->SetMainThumbnail(mLastThumbnail);
    thumbs->SetThumbnailFileDescriptor(NULL);
    if (mLastThumbnail == NULL) {
        thumbs->SetMainThumbnail(mService->mTaskPersister->GetImageFromWriteQueue(mFilename));
    }
    // Only load the thumbnail file if we don't have a thumbnail
    AutoPtr<IBitmap> mainThumbnail;
    thumbs->GetMainThumbnail((IBitmap**)&mainThumbnail);
    Boolean exists;
    if (mainThumbnail == NULL && (mLastThumbnailFile->Exists(&exists), exists)) {
        AutoPtr<IParcelFileDescriptorHelper> pfdHelper;
        CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&pfdHelper);
        AutoPtr<IParcelFileDescriptor> thumbnailFileDescriptor;
        pfdHelper->Open(mLastThumbnailFile, IParcelFileDescriptor::MODE_READ_ONLY,
            (IParcelFileDescriptor**)&thumbnailFileDescriptor);
        thumbs->SetThumbnailFileDescriptor(thumbnailFileDescriptor);
    }
}

void TaskRecord::FreeLastThumbnail()
{
    mLastThumbnail = NULL;
}

void TaskRecord::DisposeThumbnail()
{
    mLastThumbnail = NULL;
    mLastDescription = NULL;
}

/** Returns the intent for the root activity for this task */
AutoPtr<IIntent> TaskRecord::GetBaseIntent()
{
    return mIntent != NULL ? mIntent : mAffinityIntent;
}

/** Returns the first non-finishing activity from the root. */
AutoPtr<ActivityRecord> TaskRecord::GetRootActivity()
{
    Int32 N;
    mActivities->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mActivities->Get(i, (IInterface**)&obj);
        ActivityRecord* r = TO_ActivityRecord(obj);
        if (r->mFinishing) {
            continue;
        }
        return r;
    }
    return NULL;
}

AutoPtr<ActivityRecord> TaskRecord::GetTopActivity()
{
    Int32 N;
    mActivities->GetSize(&N);
    for (Int32 i = N - 1; i >= 0; --i) {
        AutoPtr<IInterface> obj;
        mActivities->Get(i, (IInterface**)&obj);
        ActivityRecord* r = TO_ActivityRecord(obj);
        if (r->mFinishing) {
            continue;
        }
        return r;
    }
    return NULL;
}

AutoPtr<ActivityRecord> TaskRecord::TopRunningActivityLocked(
    /* [in] */ ActivityRecord* notTop)
{
    Int32 N;
    mActivities->GetSize(&N);
    for (Int32 activityNdx = N - 1; activityNdx >= 0; --activityNdx) {
        AutoPtr<IInterface> obj;
        mActivities->Get(activityNdx, (IInterface**)&obj);
        ActivityRecord* r = TO_ActivityRecord(obj);
        if (!r->mFinishing && r != notTop && mStack->OkToShowLocked(r)) {
            return r;
        }
    }
    return NULL;
}

/** Call after activity movement or finish to make sure that frontOfTask is set correctly */
void TaskRecord::SetFrontOfTask()
{
    Boolean foundFront = FALSE;
    Int32 numActivities;
    mActivities->GetSize(&numActivities);
    for (Int32 activityNdx = 0; activityNdx < numActivities; ++activityNdx) {
        AutoPtr<IInterface> obj;
        mActivities->Get(activityNdx, (IInterface**)&obj);
        ActivityRecord* r = TO_ActivityRecord(obj);
        if (foundFront || r->mFinishing) {
            r->mFrontOfTask = FALSE;
        }
        else {
            r->mFrontOfTask = TRUE;
            // Set frontOfTask FALSE for every following activity.
            foundFront = TRUE;
        }
    }
    if (!foundFront && numActivities > 0) {
        // All activities of this task are finishing. As we ought to have a frontOfTask
        // activity, make the bottom activity front.
        AutoPtr<IInterface> obj;
        mActivities->Get(0, (IInterface**)&obj);
        ActivityRecord* r = TO_ActivityRecord(obj);
        r->mFrontOfTask = TRUE;
    }
}

/**
 * Reorder the history stack so that the passed activity is brought to the front.
 */
void TaskRecord::MoveActivityToFrontLocked(
    /* [in] */ ActivityRecord* newTop)
{
    if (ActivityStackSupervisor::DEBUG_ADD_REMOVE)
        Slogger::I(TAG, "Removing and adding activity %s to stack at top",
            newTop->ToString().string()/*, new RuntimeException("here").fillInStackTrace()*/);

    mActivities->Remove((IActivityRecord*)newTop);
    mActivities->Add((IActivityRecord*)newTop);
    UpdateEffectiveIntent();

    SetFrontOfTask();
}

void TaskRecord::AddActivityAtBottom(
    /* [in] */ ActivityRecord* r)
{
    AddActivityAtIndex(0, r);
}

void TaskRecord::AddActivityToTop(
    /* [in] */ ActivityRecord* r)
{
    Int32 N;
    mActivities->GetSize(&N);
    AddActivityAtIndex(N, r);
}

void TaskRecord::AddActivityAtIndex(
    /* [in] */ Int32 index,
    /* [in] */ ActivityRecord* r)
{
    // Remove r first, and if it wasn't already in the list and it's fullscreen, count it.
    Boolean result;
    if ((mActivities->Remove((IActivityRecord*)r, &result), !result) && r->mFullscreen) {
        // Was not previously in list.
        mNumFullscreen++;
    }
    // Only set this based on the first activity
    if (mActivities->IsEmpty(&result), result) {
        mTaskType = r->mActivityType;
        mIsPersistable = r->IsPersistable();
        mCallingUid = r->mLaunchedFromUid;
        mCallingPackage = r->mLaunchedFromPackage;
        // Clamp to [1, max].
        Int32 maxRecents;
        r->mInfo->GetMaxRecents(&maxRecents);
        AutoPtr<IActivityManagerHelper> amHelper;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
        Int32 maxRecentsLimit;
        amHelper->GetMaxAppRecentsLimitStatic(&maxRecentsLimit);
        mMaxRecents = Elastos::Core::Math::Min(Elastos::Core::Math::Max(maxRecents, 1),
                maxRecentsLimit);
    }
    else {
        // Otherwise make all added activities match this one.
        r->mActivityType = mTaskType;
    }
    mActivities->Add(index, (IActivityRecord*)r);
    UpdateEffectiveIntent();
    if (r->IsPersistable()) {
        mService->NotifyTaskPersisterLocked(this, FALSE);
    }
}

Boolean TaskRecord::RemoveActivity(
    /* [in] */ ActivityRecord* r)
{
    Boolean result;
    if ((mActivities->Remove((IActivityRecord*)r, &result), result) && r->mFullscreen) {
        // Was previously in list.
        mNumFullscreen--;
    }
    if (r->IsPersistable()) {
        mService->NotifyTaskPersisterLocked(this, FALSE);
    }
    if (mActivities->IsEmpty(&result), result) {
        return !mReuseTask;
    }
    UpdateEffectiveIntent();
    return FALSE;
}

Boolean TaskRecord::AutoRemoveFromRecents()
{
    // We will automatically remove the task either if it has explicitly asked for
    // this, or it is empty and has never contained an activity that got shown to
    // the user->
    Boolean result;
    return mAutoRemoveRecents || ((mActivities->IsEmpty(&result), result) && !mHasBeenVisible);
}

void TaskRecord::PerformClearTaskAtIndexLocked(
    /* [in] */ Int32 activityNdx)
{
    Int32 numActivities;
    mActivities->GetSize(&numActivities);
    for (; activityNdx < numActivities; ++activityNdx) {
        AutoPtr<IInterface> obj;
        mActivities->Get(activityNdx, (IInterface**)&obj);
        ActivityRecord* r = TO_ActivityRecord(obj);
        if (r->mFinishing) {
            continue;
        }

        if (mStack == NULL) {
            // Task was restored from persistent storage.
            r->TakeFromHistory();
            mActivities->Remove(activityNdx);
            --activityNdx;
            --numActivities;
        }
        else if (mStack->FinishActivityLocked(r, IActivity::RESULT_CANCELED, NULL, String("clear"),
                FALSE)) {
            --activityNdx;
            --numActivities;
        }
    }
}

void TaskRecord::PerformClearTaskLocked()
{
    mReuseTask = TRUE;
    PerformClearTaskAtIndexLocked(0);
    mReuseTask = FALSE;
}

AutoPtr<ActivityRecord> TaskRecord::PerformClearTaskLocked(
    /* [in] */ ActivityRecord* newR,
    /* [in] */ Int32 launchFlags)
{
    Int32 numActivities;
    mActivities->GetSize(&numActivities);
    for (Int32 activityNdx = numActivities - 1; activityNdx >= 0; --activityNdx) {
        AutoPtr<IInterface> obj;
        mActivities->Get(activityNdx, (IInterface**)&obj);
        ActivityRecord* r = TO_ActivityRecord(obj);
        if (r->mFinishing) {
            continue;
        }

        Boolean equals;
        IObject::Probe(r->mRealActivity)->Equals(newR->mRealActivity, &equals);
        if (equals) {
            // Here it is!  Now finish everything in front...
            AutoPtr<ActivityRecord> ret = r;

            for (++activityNdx; activityNdx < numActivities; ++activityNdx) {
                AutoPtr<IInterface> obj;
                mActivities->Get(activityNdx, (IInterface**)&obj);
                r = TO_ActivityRecord(obj);
                if (r->mFinishing) {
                    continue;
                }
                AutoPtr<IActivityOptions> opts = r->TakeOptionsLocked();
                if (opts != NULL) {
                    ret->UpdateOptionsLocked(opts);
                }
                if (mStack->FinishActivityLocked(r, IActivity::RESULT_CANCELED,
                    NULL, String("clear"), FALSE)) {
                    --activityNdx;
                    --numActivities;
                }
            }

            // Finally, if this is a normal launch mode (that is, not
            // expecting onNewIntent()), then we will finish the current
            // instance of the activity so a new fresh one can be started.
            if (ret->mLaunchMode == IActivityInfo::LAUNCH_MULTIPLE
                    && (launchFlags & IIntent::FLAG_ACTIVITY_SINGLE_TOP) == 0) {
                if (!ret->mFinishing) {
                    mStack->FinishActivityLocked(ret, IActivity::RESULT_CANCELED, NULL,
                            String("clear"), FALSE);
                    return NULL;
                }
            }

            return ret;
        }
    }

    return NULL;
}

AutoPtr<IActivityManagerTaskThumbnail> TaskRecord::GetTaskThumbnailLocked()
{
    if (mStack != NULL) {
        AutoPtr<ActivityRecord> resumedActivity = mStack->mResumedActivity;
        if (resumedActivity != NULL && resumedActivity->mTask.Get() == this) {
            AutoPtr<IBitmap> thumbnail = mStack->ScreenshotActivities(resumedActivity);
            SetLastThumbnail(thumbnail);
        }
    }
    AutoPtr<IActivityManagerTaskThumbnail> taskThumbnail;
    CActivityManagerTaskThumbnail::New((IActivityManagerTaskThumbnail**)&taskThumbnail);
    GetLastThumbnail(taskThumbnail);
    return taskThumbnail;
}

void TaskRecord::RemoveTaskActivitiesLocked()
{
    // Just remove the entire task.
    PerformClearTaskAtIndexLocked(0);
}

Boolean TaskRecord::IsHomeTask()
{
    return mTaskType == ActivityRecord::HOME_ACTIVITY_TYPE;
}

Boolean TaskRecord::IsApplicationTask()
{
    return mTaskType == ActivityRecord::APPLICATION_ACTIVITY_TYPE;
}

Boolean TaskRecord::IsOverHomeStack()
{
    return mTaskToReturnTo == ActivityRecord::HOME_ACTIVITY_TYPE
        || mTaskToReturnTo == ActivityRecord::RECENTS_ACTIVITY_TYPE;
}

AutoPtr<ActivityRecord> TaskRecord::FindActivityInHistoryLocked(
    /* [in] */ ActivityRecord* r)
{
    AutoPtr<IComponentName> realActivity = r->mRealActivity;
    Int32 N;
    mActivities->GetSize(&N);
    for (Int32 activityNdx = N - 1; activityNdx >= 0; --activityNdx) {
        AutoPtr<IInterface> obj;
        mActivities->Get(activityNdx, (IInterface**)&obj);
        ActivityRecord* candidate = TO_ActivityRecord(obj);
        if (candidate->mFinishing) {
            continue;
        }
        Boolean equals;
        IObject::Probe(candidate->mRealActivity)->Equals(realActivity, &equals);
        if (equals) {
            return candidate;
        }
    }
    return NULL;
}

void TaskRecord::UpdateTaskDescription()
{
    // Traverse upwards looking for any break between main task activities and
    // utility activities.
    Int32 activityNdx;
    Int32 numActivities;
    mActivities->GetSize(&numActivities);
    Boolean relinquish = FALSE;
    if (numActivities != 0) {
        AutoPtr<IInterface> obj;
        mActivities->Get(0, (IInterface**)&obj);
        ActivityRecord* r = TO_ActivityRecord(obj);
        Int32 flags;
        r->mInfo->GetFlags(&flags);
        relinquish = (flags & IActivityInfo::FLAG_RELINQUISH_TASK_IDENTITY) != 0;
    }
    for (activityNdx = Elastos::Core::Math::Min(numActivities, 1); activityNdx < numActivities; ++activityNdx) {
        AutoPtr<IInterface> obj;
        mActivities->Get(activityNdx, (IInterface**)&obj);
        ActivityRecord* r = TO_ActivityRecord(obj);
        Int32 flags;
        if (relinquish && (r->mInfo->GetFlags(&flags), flags & IActivityInfo::FLAG_RELINQUISH_TASK_IDENTITY) == 0) {
            // This will be the top activity for determining taskDescription. Pre-inc to
            // overcome initial decrement below.
            ++activityNdx;
            break;
        }
        if (r->mIntent != NULL && (r->mIntent->GetFlags(&flags),
            flags & IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET) != 0) {
            break;
        }
    }
    if (activityNdx > 0) {
        // Traverse downwards starting below break looking for set label, icon.
        // Note that if there are activities in the task but none of them set the
        // recent activity values, then we do not fall back to the last set
        // values in the TaskRecord.
        String label;
        String iconFilename;
        Int32 colorPrimary = 0;
        for (--activityNdx; activityNdx >= 0; --activityNdx) {
            AutoPtr<IInterface> obj;
            mActivities->Get(activityNdx, (IInterface**)&obj);
            ActivityRecord* r = TO_ActivityRecord(obj);
            if (r->mTaskDescription != NULL) {
                if (label == NULL) {
                    r->mTaskDescription->GetLabel(&label);
                }
                if (iconFilename == NULL) {
                    r->mTaskDescription->GetIconFilename(&iconFilename);
                }
                if (colorPrimary == 0) {
                    r->mTaskDescription->GetPrimaryColor(&colorPrimary);
                }
            }
        }
        mLastTaskDescription = NULL;
        CActivityManagerTaskDescription::New(label, colorPrimary, iconFilename,
            (IActivityManagerTaskDescription**)&mLastTaskDescription);
        // Update the task affiliation color if we are the parent of the group
        if (mTaskId == mAffiliatedTaskId) {
            mLastTaskDescription->GetPrimaryColor(&mAffiliatedTaskColor);
        }
    }
}

Int32 TaskRecord::FindEffectiveRootIndex()
{
    Int32 effectiveNdx = 0;
    Int32 N;
    mActivities->GetSize(&N);
    Int32 topActivityNdx = N - 1;
    for (Int32 activityNdx = 0; activityNdx <= topActivityNdx; ++activityNdx) {
        AutoPtr<IInterface> obj;
        mActivities->Get(activityNdx, (IInterface**)&obj);
        ActivityRecord* r = TO_ActivityRecord(obj);
        if (r->mFinishing) {
            continue;
        }
        Int32 flags;
        r->mInfo->GetFlags(&flags);
        if ((flags & IActivityInfo::FLAG_RELINQUISH_TASK_IDENTITY) == 0) {
            break;
        }
    }
    return effectiveNdx;
}

void TaskRecord::UpdateEffectiveIntent()
{
    Int32 effectiveRootIndex = FindEffectiveRootIndex();
    AutoPtr<IInterface> obj;
    mActivities->Get(effectiveRootIndex, (IInterface**)&obj);
    ActivityRecord* r = TO_ActivityRecord(obj);
    SetIntent(r);
}

ECode TaskRecord::SaveToXml(
    /* [in] */ IXmlSerializer* out)
{
    if (CActivityManagerService::DEBUG_RECENTS)
        Slogger::I(TAG, "Saving task=%s", ToString().string());

    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_TASKID, StringUtils::ToString(mTaskId)));
    if (mRealActivity != NULL) {
        String str;
        mRealActivity->FlattenToShortString(&str);
        FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_REALACTIVITY, str));
    }
    if (mOrigActivity != NULL) {
        String str;
        mOrigActivity->FlattenToShortString(&str);
        FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_ORIGACTIVITY, str));
    }
    // Write affinity, and root affinity if it is different from affinity.
    // We use the special string "@" for a NULL root affinity, so we can identify
    // later whether we were given a root affinity or should just make it the
    // same as the affinity.
    if (mAffinity != NULL) {
        FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_AFFINITY, mAffinity));
        if (!mAffinity.Equals(mRootAffinity)) {
            FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_ROOT_AFFINITY,
                mRootAffinity != NULL ? mRootAffinity : String("@")));
        }
    }
    else if (mRootAffinity != NULL) {
        FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_ROOT_AFFINITY,
            mRootAffinity != NULL ? mRootAffinity : String("@")));
    }
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_ROOTHASRESET, StringUtils::ToString(mRootWasReset)));
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_AUTOREMOVERECENTS, StringUtils::ToString(mAutoRemoveRecents)));
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_ASKEDCOMPATMODE, StringUtils::ToString(mAskedCompatMode)));
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_USERID, StringUtils::ToString(mUserId)));
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_EFFECTIVE_UID, StringUtils::ToString(mEffectiveUid)));
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_TASKTYPE, StringUtils::ToString(mTaskType)));
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_FIRSTACTIVETIME, StringUtils::ToString(mFirstActiveTime)));
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_LASTACTIVETIME, StringUtils::ToString(mLastActiveTime)));
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_LASTTIMEMOVED, StringUtils::ToString(mLastTimeMoved)));
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_NEVERRELINQUISH, StringUtils::ToString(mNeverRelinquishIdentity)));
    if (mLastDescription != NULL) {
        String str;
        IObject::Probe(mLastDescription)->ToString(&str);
        FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_LASTDESCRIPTION, str));
    }
    if (mLastTaskDescription != NULL) {
        FAIL_RETURN(mLastTaskDescription->SaveToXml(out));
    }
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_TASK_AFFILIATION_COLOR, StringUtils::ToString(mAffiliatedTaskColor)));
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_TASK_AFFILIATION, StringUtils::ToString(mAffiliatedTaskId)));
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_PREV_AFFILIATION, StringUtils::ToString(mPrevAffiliateTaskId)));
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_NEXT_AFFILIATION, StringUtils::ToString(mNextAffiliateTaskId)));
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_CALLING_UID, StringUtils::ToString(mCallingUid)));
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_CALLING_PACKAGE,
        mCallingPackage == NULL ? String("") : mCallingPackage));

    if (mAffinityIntent != NULL) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), TAG_AFFINITYINTENT));
        FAIL_RETURN(mAffinityIntent->SaveToXml(out));
        FAIL_RETURN(out->WriteEndTag(String(NULL), TAG_AFFINITYINTENT));
    }

    FAIL_RETURN(out->WriteStartTag(String(NULL), TAG_INTENT));
    FAIL_RETURN(mIntent->SaveToXml(out));
    FAIL_RETURN(out->WriteEndTag(String(NULL), TAG_INTENT));

    Int32 numActivities;
    mActivities->GetSize(&numActivities);
    for (Int32 activityNdx = 0; activityNdx < numActivities; ++activityNdx) {
        AutoPtr<IInterface> obj;
        mActivities->Get(activityNdx, (IInterface**)&obj);
        ActivityRecord* r = TO_ActivityRecord(obj);
        Int32 persistableMode;
        r->mInfo->GetPersistableMode(&persistableMode);
        Int32 flags;
        if ((persistableMode == IActivityInfo::PERSIST_ROOT_ONLY || !r->IsPersistable() ||
            (r->mIntent->GetFlags(&flags), (flags & IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET) != 0)) &&
                activityNdx > 0) {
            // Stop at first non-persistable or first break in task (CLEAR_WHEN_TASK_RESET).
            break;
        }
        FAIL_RETURN(out->WriteStartTag(String(NULL), TAG_ACTIVITY));
        FAIL_RETURN(r->SaveToXml(out));
        FAIL_RETURN(out->WriteEndTag(String(NULL), TAG_ACTIVITY));
    }
    return NOERROR;
}

ECode TaskRecord::RestoreFromXml(
    /* [in] */ IXmlPullParser* in,
    /* [in] */ ActivityStackSupervisor* stackSupervisor,
    /* [out] */ TaskRecord** taskRecord)
{
    VALIDATE_NOT_NULL(taskRecord)
    AutoPtr<IIntent> intent;
    AutoPtr<IIntent> affinityIntent;
    AutoPtr<IArrayList> activities;
    CArrayList::New((IArrayList**)&activities);
    AutoPtr<IComponentName> realActivity;
    AutoPtr<IComponentName> origActivity;
    String affinity;
    String rootAffinity;
    Boolean hasRootAffinity = FALSE;
    Boolean rootHasReset = FALSE;
    Boolean autoRemoveRecents = FALSE;
    Boolean askedCompatMode = FALSE;
    Int32 taskType = ActivityRecord::APPLICATION_ACTIVITY_TYPE;
    Int32 userId = 0;
    Int32 effectiveUid = -1;
    String lastDescription;
    Int64 firstActiveTime = -1;
    Int64 lastActiveTime = -1;
    Int64 lastTimeOnTop = 0;
    Boolean neverRelinquishIdentity = TRUE;
    Int32 taskId = -1;
    Int32 outerDepth;
    FAIL_RETURN(in->GetDepth(&outerDepth));
    AutoPtr<IActivityManagerTaskDescription> taskDescription;
    CActivityManagerTaskDescription::New((IActivityManagerTaskDescription**)&taskDescription);
    Int32 taskAffiliation = -1;
    Int32 taskAffiliationColor = 0;
    Int32 prevTaskId = -1;
    Int32 nextTaskId = -1;
    Int32 callingUid = -1;
    String callingPackage("");

    Int32 attrNdx;
    FAIL_RETURN(in->GetAttributeCount(&attrNdx));
    for (--attrNdx; attrNdx >= 0; --attrNdx) {
        String attrName;
        FAIL_RETURN(in->GetAttributeName(attrNdx, &attrName));
        String attrValue;
        FAIL_RETURN(in->GetAttributeValue(attrNdx, &attrValue));
        if (TaskPersister::DEBUG)
            Slogger::D(TaskPersister::TAG, "TaskRecord: attribute name=%s value=%s",
                attrName.string(), attrValue.string());
        if (ATTR_TASKID.Equals(attrName)) {
            taskId = StringUtils::ParseInt32(attrValue);
        }
        else if (ATTR_REALACTIVITY.Equals(attrName)) {
            AutoPtr<IComponentNameHelper> cnHelper;
            CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&cnHelper);
            cnHelper->UnflattenFromString(attrValue, (IComponentName**)&realActivity);
        }
        else if (ATTR_ORIGACTIVITY.Equals(attrName)) {
            AutoPtr<IComponentNameHelper> cnHelper;
            CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&cnHelper);
            cnHelper->UnflattenFromString(attrValue, (IComponentName**)&origActivity);
        }
        else if (ATTR_AFFINITY.Equals(attrName)) {
            affinity = attrValue;
        }
        else if (ATTR_ROOT_AFFINITY.Equals(attrName)) {
            rootAffinity = attrValue;
            hasRootAffinity = TRUE;
        }
        else if (ATTR_ROOTHASRESET.Equals(attrName)) {
            rootHasReset = StringUtils::ParseBoolean(attrValue);
        }
        else if (ATTR_AUTOREMOVERECENTS.Equals(attrName)) {
            autoRemoveRecents = StringUtils::ParseBoolean(attrValue);
        }
        else if (ATTR_ASKEDCOMPATMODE.Equals(attrName)) {
            askedCompatMode = StringUtils::ParseBoolean(attrValue);
        }
        else if (ATTR_USERID.Equals(attrName)) {
            userId = StringUtils::ParseInt32(attrValue);
        }
        else if (ATTR_EFFECTIVE_UID.Equals(attrName)) {
            effectiveUid = StringUtils::ParseInt32(attrValue);
        }
        else if (ATTR_TASKTYPE.Equals(attrName)) {
            taskType = StringUtils::ParseInt32(attrValue);
        }
        else if (ATTR_FIRSTACTIVETIME.Equals(attrName)) {
            firstActiveTime = StringUtils::ParseInt64(attrValue);
        }
        else if (ATTR_LASTACTIVETIME.Equals(attrName)) {
            lastActiveTime = StringUtils::ParseInt64(attrValue);
        }
        else if (ATTR_LASTDESCRIPTION.Equals(attrName)) {
            lastDescription = attrValue;
        }
        else if (ATTR_LASTTIMEMOVED.Equals(attrName)) {
            lastTimeOnTop = StringUtils::ParseInt64(attrValue);
        }
        else if (ATTR_NEVERRELINQUISH.Equals(attrName)) {
            neverRelinquishIdentity = StringUtils::ParseBoolean(attrValue);
        }
        else if (attrName.StartWith(IActivityManagerTaskDescription::ATTR_TASKDESCRIPTION_PREFIX)) {
            taskDescription->RestoreFromXml(attrName, attrValue);
        }
        else if (ATTR_TASK_AFFILIATION.Equals(attrName)) {
            taskAffiliation = StringUtils::ParseInt32(attrValue);
        }
        else if (ATTR_PREV_AFFILIATION.Equals(attrName)) {
            prevTaskId = StringUtils::ParseInt32(attrValue);
        }
        else if (ATTR_NEXT_AFFILIATION.Equals(attrName)) {
            nextTaskId = StringUtils::ParseInt32(attrValue);
        }
        else if (ATTR_TASK_AFFILIATION_COLOR.Equals(attrName)) {
            taskAffiliationColor = StringUtils::ParseInt32(attrValue);
        }
        else if (ATTR_CALLING_UID.Equals(attrName)) {
            callingUid = StringUtils::ParseInt32(attrValue);
        }
        else if (ATTR_CALLING_PACKAGE.Equals(attrName)) {
            callingPackage = attrValue;
        }
        else {
            Slogger::W(TAG, "TaskRecord: Unknown attribute=%s", attrName.string());
        }
    }

    Int32 event, depth;
    while (((in->Next(&event), event) != IXmlPullParser::END_DOCUMENT) &&
            (event != IXmlPullParser::END_TAG || (in->GetDepth(&depth), depth < outerDepth))) {
        if (event == IXmlPullParser::START_TAG) {
            String name;
            FAIL_RETURN(in->GetName(&name));
            if (TaskPersister::DEBUG)
                Slogger::D(TaskPersister::TAG, "TaskRecord: START_TAG name=%s", name.string());
            if (TAG_AFFINITYINTENT.Equals(name)) {
                AutoPtr<IIntentHelper> iHelper;
                CIntentHelper::AcquireSingleton((IIntentHelper**)&iHelper);
                affinityIntent = NULL;
                FAIL_RETURN(iHelper->RestoreFromXml(in, (IIntent**)&affinityIntent));
            }
            else if (TAG_INTENT.Equals(name)) {
                AutoPtr<IIntentHelper> iHelper;
                CIntentHelper::AcquireSingleton((IIntentHelper**)&iHelper);
                intent = NULL;
                FAIL_RETURN(iHelper->RestoreFromXml(in, (IIntent**)&intent));
            }
            else if (TAG_ACTIVITY.Equals(name)) {
                AutoPtr<ActivityRecord> activity;
                FAIL_RETURN(ActivityRecord::RestoreFromXml(in, taskId, stackSupervisor,
                    (ActivityRecord**)&activity));
                if (TaskPersister::DEBUG)
                    Slogger::D(TaskPersister::TAG, "TaskRecord: activity=%s",
                        activity->ToString().string());
                if (activity != NULL) {
                    activities->Add((IActivityRecord*)activity);
                }
            }
            else {
                Slogger::E(TAG, "restoreTask: Unexpected name=%s", name.string());
                XmlUtils::SkipCurrentTag(in);
            }
        }
    }

    if (!hasRootAffinity) {
        rootAffinity = affinity;
    }
    else if (rootAffinity.Equals("@")) {
        rootAffinity = NULL;
    }

    if (effectiveUid <= 0) {
        AutoPtr<IIntent> checkIntent = intent != NULL ? intent : affinityIntent;
        effectiveUid = 0;
        if (checkIntent != NULL) {
            AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
            AutoPtr<IComponentName> component;
            checkIntent->GetComponent((IComponentName**)&component);
            String packageName;
            component->GetPackageName(&packageName);
            AutoPtr<IApplicationInfo> ai;
            pm->GetApplicationInfo(packageName, IPackageManager::GET_UNINSTALLED_PACKAGES
                | IPackageManager::GET_DISABLED_COMPONENTS, userId, (IApplicationInfo**)&ai);
            if (ai != NULL) {
                ai->GetUid(&effectiveUid);
            }
        }

        Slogger::W(TAG, "Updating task #%d for %s: effectiveUid=%d",
            taskId, TO_CSTR(checkIntent), effectiveUid);
    }

    AutoPtr<TaskRecord> task = new TaskRecord();
    task->constructor(stackSupervisor->mService, taskId, intent,
            affinityIntent, affinity, rootAffinity, realActivity, origActivity, rootHasReset,
            autoRemoveRecents, askedCompatMode, taskType, userId, effectiveUid, lastDescription,
            activities, firstActiveTime, lastActiveTime, lastTimeOnTop, neverRelinquishIdentity,
            taskDescription, taskAffiliation, prevTaskId, nextTaskId, taskAffiliationColor,
            callingUid, callingPackage);

    Int32 N;
    activities->GetSize(&N);
    for (Int32 activityNdx = N - 1; activityNdx >= 0; --activityNdx) {
        AutoPtr<IInterface> obj;
        activities->Get(activityNdx, (IInterface**)&obj);
        TO_ActivityRecord(obj)->mTask = task;
    }

    if (CActivityManagerService::DEBUG_RECENTS)
        Slogger::D(TAG, "Restored task=%s", task->ToString().string());

    *taskRecord = task;
    REFCOUNT_ADD(*taskRecord)
    return NOERROR;
}

void TaskRecord::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    pw->Print(prefix);
    pw->Print(String("mUserId="));
    pw->Print(mUserId);
    pw->Print(String(" mEffectiveUid="));
    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    uhHelper->FormatUid(pw, mEffectiveUid);
    pw->Print(String(" mCallingUid="));
    uhHelper->FormatUid(pw, mCallingUid);
    pw->Print(String(" mCallingPackage="));
    pw->Println(mCallingPackage);
    if (mAffinity != NULL || mRootAffinity != NULL) {
        pw->Print(prefix);
        pw->Print(String("mAffinity="));
        pw->Print(mAffinity);
        if (mAffinity == NULL || !mAffinity.Equals(mRootAffinity)) {
            pw->Print(String(" root="));
            pw->Println(mRootAffinity);
        }
        else {
            pw->Println();
        }
    }
    if (mVoiceSession != NULL || mVoiceInteractor != NULL) {
        pw->Print(prefix);
        pw->Print(String("VOICE: session=0x"));
        pw->Print(StringUtils::ToString((Int32)mVoiceSession.Get(), 16));
        pw->Print(String(" interactor=0x"));
        pw->Println(StringUtils::ToString((Int32)mVoiceInteractor.Get(), 16));
    }
    if (mIntent != NULL) {
        StringBuilder sb(128);
        sb.Append(prefix);
        sb.Append("intent={");
        mIntent->ToShortString(&sb, FALSE, TRUE, FALSE, TRUE);
        sb.Append("}");
        pw->Println(sb.ToString());
    }
    if (mAffinityIntent != NULL) {
        StringBuilder sb(128);
        sb.Append(prefix);
        sb.Append("mAffinityIntent={");
        mAffinityIntent->ToShortString(&sb, FALSE, TRUE, FALSE, TRUE);
        sb.Append("}");
        pw->Println(sb.ToString());
    }
    if (mOrigActivity != NULL) {
        pw->Print(prefix);
        pw->Print(String("mOrigActivity="));
        String str;
        mOrigActivity->FlattenToShortString(&str);
        pw->Println(str);
    }
    if (mRealActivity != NULL) {
        pw->Print(prefix);
        pw->Print(String("mRealActivity="));
        String str;
        mRealActivity->FlattenToShortString(&str);
        pw->Println(str);
    }
    if (mAutoRemoveRecents || mIsPersistable || mTaskType != 0 || mTaskToReturnTo != 0
            || mNumFullscreen != 0) {
        pw->Print(prefix);
        pw->Print(String("mAutoRemoveRecents="));
        pw->Print(mAutoRemoveRecents);
        pw->Print(String(" mIsPersistable="));
        pw->Print(mIsPersistable);
        pw->Print(String(" mNumFullscreen="));
        pw->Print(mNumFullscreen);
        pw->Print(String(" mTaskType="));
        pw->Print(mTaskType);
        pw->Print(String(" mTaskToReturnTo="));
        pw->Println(mTaskToReturnTo);
    }
    if (mRootWasReset || mNeverRelinquishIdentity || mReuseTask) {
        pw->Print(prefix);
        pw->Print(String("mRootWasReset="));
        pw->Print(mRootWasReset);
        pw->Print(String(" mNeverRelinquishIdentity="));
        pw->Print(mNeverRelinquishIdentity);
        pw->Print(String(" mReuseTask="));
        pw->Println(mReuseTask);
    }
    if (mAffiliatedTaskId != mTaskId || mPrevAffiliateTaskId != -1 || mPrevAffiliate != NULL
            || mNextAffiliateTaskId != -1 || mNextAffiliate != NULL) {
        pw->Print(prefix);
        pw->Print(String("affiliation="));
        pw->Print(mAffiliatedTaskId);
        pw->Print(String(" prevAffiliation="));
        pw->Print(mPrevAffiliateTaskId);
        pw->Print(String(" ("));
        if (mPrevAffiliate == NULL) {
            pw->Print(String("NULL"));
        }
        else {
            pw->Print(StringUtils::ToString((Int32)mPrevAffiliate.Get(), 16));
        }
        pw->Print(String(") nextAffiliation="));
        pw->Print(mNextAffiliateTaskId);
        pw->Print(String(" ("));
        if (mNextAffiliate == NULL) {
            pw->Print(String("NULL"));
        }
        else {
            pw->Print(StringUtils::ToString((Int32)mNextAffiliate.Get(), 16));
        }
        pw->Println(String(")"));
    }
    pw->Print(prefix);
    pw->Print(String("Activities="));
    pw->Println(/*mActivities*/);
    if (!mAskedCompatMode || !mInRecents || !mIsAvailable) {
        pw->Print(prefix);
        pw->Print(String("mAskedCompatMode="));
        pw->Print(mAskedCompatMode);
        pw->Print(String(" mInRecents="));
        pw->Print(mInRecents);
        pw->Print(String(" mIsAvailable="));
        pw->Println(mIsAvailable);
    }
    pw->Print(prefix);
    pw->Print(String("lastThumbnail="));
    pw->Print(mLastThumbnail);
    pw->Print(String(" lastThumbnailFile="));
    pw->Println(mLastThumbnailFile);
    if (mLastDescription != NULL) {
        pw->Print(prefix);
        pw->Print(String("mLastDescription="));
        pw->Println(mLastDescription);
    }
    pw->Print(prefix);
    pw->Print(String("mHasBeenVisible="));
    pw->Print(mHasBeenVisible);
    pw->Print(String(" mFirstActiveTime="));
    pw->Print(mLastActiveTime);
    pw->Print(String(" mLastActiveTime="));
    pw->Print(mLastActiveTime);
    pw->Print(String(" (inactive for "));
    pw->Print(GetInactiveDuration() / 1000);
    pw->Println(String("s)"));
}

// @Override
ECode TaskRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = ToString();
    return NOERROR;
}

String TaskRecord::ToString()
{
    StringBuilder sb(128);
    if (mStringName != NULL) {
        sb.Append(mStringName);
        sb.Append(" U=");
        sb.Append(mUserId);
        sb.Append(" sz=");
        Int32 N;
        mActivities->GetSize(&N);
        sb.Append(N);
        sb.AppendChar('}');
        return sb.ToString();
    }
    sb.Append("TaskRecord{");
    sb.Append(StringUtils::ToString((Int32)this, 16));
    sb.Append(" #");
    sb.Append(mTaskId);
    if (mAffinity != NULL) {
        sb.Append(" A=");
        sb.Append(mAffinity);
    }
    else if (mIntent != NULL) {
        sb.Append(" I=");
        AutoPtr<IComponentName> component;
        mIntent->GetComponent((IComponentName**)&component);
        String str;
        component->FlattenToShortString(&str);
        sb.Append(str);
    }
    else if (mAffinityIntent != NULL) {
        sb.Append(" aI=");
        AutoPtr<IComponentName> component;
        mAffinityIntent->GetComponent((IComponentName**)&component);
        String str;
        component->FlattenToShortString(&str);
        sb.Append(str);
    }
    else {
        sb.Append(" ??");
    }
    mStringName = sb.ToString();
    return ToString();
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
