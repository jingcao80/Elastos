
#ifndef __ELASTOS_DROID_SERVER_AM_TASKRECORD_H__
#define __ELASTOS_DROID_SERVER_AM_TASKRECORD_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::App::IActivityManagerTaskThumbnail;
using Elastos::Droid::App::IActivityManagerTaskDescription;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Service::Voice::IIVoiceInteractionSession;
using Elastos::Droid::Internal::App::IIVoiceInteractor;
using Elastos::Core::ICharSequence;
using Elastos::IO::IFile;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::List;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ActivityStack;
class ActivityStackSupervisor;
class CActivityManagerService;
class ActivityRecord;
class TaskPersister;

class TaskRecord : public Object
{
public:
    TaskRecord();

    CARAPI constructor(
        /* [in] */ CActivityManagerService* service,
        /* [in] */ Int32 taskId,
        /* [in] */ IActivityInfo* info,
        /* [in] */ IIntent* intent,
        /* [in] */ IIVoiceInteractionSession* voiceSession,
        /* [in] */ IIVoiceInteractor* voiceInteractor);

    CARAPI constructor(
        /* [in] */ CActivityManagerService* service,
        /* [in] */ Int32 taskId,
        /* [in] */ IActivityInfo* info,
        /* [in] */ IIntent* intent,
        /* [in] */ IActivityManagerTaskDescription* taskDescription);

    CARAPI constructor(
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
        /* [in] */ List<AutoPtr<ActivityRecord> >* activities,
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
        /* [in] */ const String& callingPackage);

    CARAPI_(void) TouchActiveTime();

    CARAPI_(Int64) GetInactiveDuration();

    /** Sets the original intent, and the calling uid and package. */
    CARAPI_(void) SetIntent(
        /* [in] */ ActivityRecord* r);

    CARAPI_(void) SetTaskToReturnTo(
        /* [in] */ Int32 taskToReturnTo);

    CARAPI_(Int32) GetTaskToReturnTo();

    CARAPI_(void) SetPrevAffiliate(
        /* [in] */ TaskRecord* prevAffiliate);

    CARAPI_(void) SetNextAffiliate(
        /* [in] */ TaskRecord* nextAffiliate);

    // Close up recents linked list.
    CARAPI_(void) CloseRecentsChain();

    CARAPI_(void) RemovedFromRecents(
        /* [in] */ TaskPersister* persister);

    CARAPI_(void) SetTaskToAffiliateWith(
        /* [in] */ TaskRecord* _taskToAffiliateWith);

    /**
     * Sets the last thumbnail.
     * @return whether the thumbnail was set
     */
    CARAPI_(Boolean) SetLastThumbnail(
        /* [in] */ IBitmap* thumbnail);

    CARAPI_(void) GetLastThumbnail(
        /* [in] */ IActivityManagerTaskThumbnail* thumbs);

    CARAPI_(void) FreeLastThumbnail();

    CARAPI_(void) DisposeThumbnail();

    /** Returns the intent for the root activity for this task */
    CARAPI_(AutoPtr<IIntent>) GetBaseIntent();

    /** Returns the first non-finishing activity from the root. */
    CARAPI_(AutoPtr<ActivityRecord>) GetRootActivity();

    CARAPI_(AutoPtr<ActivityRecord>) GetTopActivity();

    CARAPI_(AutoPtr<ActivityRecord>) TopRunningActivityLocked(
        /* [in] */ ActivityRecord* notTop);

    /** Call after activity movement or finish to make sure that frontOfTask is set correctly */
    CARAPI_(void) SetFrontOfTask();

    /**
     * Reorder the history stack so that the passed activity is brought to the front.
     */
    CARAPI_(void) MoveActivityToFrontLocked(
        /* [in] */ ActivityRecord* newTop);

    CARAPI_(void) AddActivityAtBottom(
        /* [in] */ ActivityRecord* r);

    CARAPI_(void) AddActivityToTop(
        /* [in] */ ActivityRecord* r);

    CARAPI_(void) AddActivityAtIndex(
        /* [in] */ Int32 index,
        /* [in] */ ActivityRecord* r);

    /** @return TRUE if this was the last activity in the task */
    CARAPI_(Boolean) RemoveActivity(
        /* [in] */ ActivityRecord* r);

    CARAPI_(Boolean) AutoRemoveFromRecents();

    /**
     * Completely remove all activities associated with an existing
     * task starting at a specified index.
     */
    CARAPI_(void) PerformClearTaskAtIndexLocked(
        /* [in] */ Int32 activityNdx);

    /**
     * Completely remove all activities associated with an existing task.
     */
    CARAPI_(void) PerformClearTaskLocked();

    /**
     * Perform clear operation as requested by
     * {@link Intent#FLAG_ACTIVITY_CLEAR_TOP}: search from the top of the
     * stack to the given task, then look for
     * an instance of that activity in the stack and, if found, finish all
     * activities on top of it and return the instance.
     *
     * @param newR Description of the new activity being started.
     * @return Returns the old activity that should be continued to be used,
     * or NULL if none was found.
     */
    CARAPI_(AutoPtr<ActivityRecord>) PerformClearTaskLocked(
        /* [in] */ ActivityRecord* newR,
        /* [in] */ Int32 launchFlags);

    CARAPI_(AutoPtr<IActivityManagerTaskThumbnail>) GetTaskThumbnailLocked();

    CARAPI_(void) RemoveTaskActivitiesLocked();

    CARAPI_(Boolean) IsHomeTask();

    CARAPI_(Boolean) IsApplicationTask();

    CARAPI_(Boolean) IsOverHomeStack();

    /**
     * Find the activity in the history stack within the given task.  Returns
     * the index within the history at which it's found, or < 0 if not found.
     */
    CARAPI_(AutoPtr<ActivityRecord>) FindActivityInHistoryLocked(
        /* [in] */ ActivityRecord* r);

    /** Updates the last task description values. */
    CARAPI_(void) UpdateTaskDescription();

    CARAPI_(Int32) FindEffectiveRootIndex();

    CARAPI_(void) UpdateEffectiveIntent();

    CARAPI SaveToXml(
        /* [in] */ IXmlSerializer* out);

    static CARAPI RestoreFromXml(
        /* [in] */ IXmlPullParser* in,
        /* [in] */ ActivityStackSupervisor* stackSupervisor,
        /* [out] */ TaskRecord** taskRecord);

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI_(String) ToString();

private:
    /** Sets the original intent, _without_ updating the calling uid or package. */
    CARAPI_(void) SetIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ IActivityInfo* info);

public:
    static const Boolean IGNORE_RETURN_TO_RECENTS = TRUE;

    Int32 mTaskId;       // Unique identifier for this task.
    String mAffinity;        // The affinity name for this task, or NULL; may change identity.
    String mRootAffinity;    // Initial base affinity, or NULL; does not change from initial root.
    AutoPtr<IIVoiceInteractionSession> mVoiceSession;    // Voice interaction session driving task
    AutoPtr<IIVoiceInteractor> mVoiceInteractor;         // Associated interactor to provide to app
    AutoPtr<IIntent> mIntent;          // The original intent that started the task.
    AutoPtr<IIntent> mAffinityIntent;  // Intent of affinity-moved activity that started this task.
    Int32 mEffectiveUid;       // The current effective uid of the identity of this task.
    AutoPtr<IComponentName> mOrigActivity; // The non-alias activity component of the intent.
    AutoPtr<IComponentName> mRealActivity; // The actual activity component that started the task.
    Int64 mFirstActiveTime;   // First time this task was active.
    Int64 mLastActiveTime;    // Last time this task was active, including sleep.
    Boolean mInRecents;      // Actually in the recents list?
    Boolean mIsAvailable;    // Is the activity available to be launched?
    Boolean mRootWasReset;   // True if the intent at the root of the task had
                            // the FLAG_ACTIVITY_RESET_TASK_IF_NEEDED flag.
    Boolean mAutoRemoveRecents;  // If TRUE, we should automatically remove the task from
                                // recents when activity finishes
    Boolean mAskedCompatMode;// Have asked the user about compat mode for this task.
    Boolean mHasBeenVisible; // Set if any activities in the task have been visible to the user.

    String mStringName;      // caching of toString() result.
    Int32 mUserId;             // user for which this task was created
    Int32 mCreatorUid;         // The app uid that originally created the task

    Int32 mNumFullscreen;      // Number of fullscreen activities.

    // This represents the last resolved activity values for this task
    // NOTE: This value needs to be persisted with each task
    AutoPtr<IActivityManagerTaskDescription> mLastTaskDescription;

    /** List of all activities in the task arranged in history order */
    AutoPtr<List<AutoPtr<ActivityRecord> > > mActivities;

    /** Current stack */
    ActivityStack* mStack;

    /** Takes on same set of values as ActivityRecord.mActivityType */
    Int32 mTaskType;

    /** Takes on same value as first root activity */
    Boolean mIsPersistable;
    Int32 mMaxRecents;

    /** Only used for persistable tasks, otherwise 0. The last time this task was moved. Used for
     * determining the order when restoring. Sign indicates whether last task movement was to front
     * (positive) or back (negative). Absolute value indicates time. */
    Int64 mLastTimeMoved;

    /** If original intent did not allow relinquishing task identity, save that information */
    Boolean mNeverRelinquishIdentity;

    // Used in the unique case where we are clearing the task in order to reuse it. In that case we
    // do not want to delete the stack when the task goes empty.
    Boolean mReuseTask;
    AutoPtr<ICharSequence> mLastDescription; // Last description captured for this item.

    Int32 mAffiliatedTaskId; // taskId of parent affiliation or self if no parent.
    Int32 mAffiliatedTaskColor; // color of the parent task affiliation.
    AutoPtr<TaskRecord> mPrevAffiliate; // previous task in affiliated chain.
    Int32 mPrevAffiliateTaskId; // previous id for persistence.
    AutoPtr<TaskRecord> mNextAffiliate; // next task in affiliated chain.
    Int32 mNextAffiliateTaskId; // next id for persistence.

    // For relaunching the task from recents as though it was launched by the original launcher.
    Int32 mCallingUid;
    String mCallingPackage;

    CActivityManagerService* mService;

private:
    static const String TAG;
    static const String ATTR_TASKID;
    static const String TAG_INTENT;
    static const String TAG_AFFINITYINTENT;
    static const String ATTR_REALACTIVITY;
    static const String ATTR_ORIGACTIVITY;
    static const String TAG_ACTIVITY;
    static const String ATTR_AFFINITY;
    static const String ATTR_ROOT_AFFINITY;
    static const String ATTR_ROOTHASRESET;
    static const String ATTR_AUTOREMOVERECENTS;
    static const String ATTR_ASKEDCOMPATMODE;
    static const String ATTR_USERID;
    static const String ATTR_EFFECTIVE_UID;
    static const String ATTR_TASKTYPE;
    static const String ATTR_FIRSTACTIVETIME;
    static const String ATTR_LASTACTIVETIME;
    static const String ATTR_LASTDESCRIPTION;
    static const String ATTR_LASTTIMEMOVED;
    static const String ATTR_NEVERRELINQUISH;
    static const String ATTR_TASK_AFFILIATION;
    static const String ATTR_PREV_AFFILIATION;
    static const String ATTR_NEXT_AFFILIATION;
    static const String ATTR_TASK_AFFILIATION_COLOR;
    static const String ATTR_CALLING_UID;
    static const String ATTR_CALLING_PACKAGE;

    static const String TASK_THUMBNAIL_SUFFIX;

    /** Indication of what to run next when task exits. Use ActivityRecord types.
     * ActivityRecord.APPLICATION_ACTIVITY_TYPE indicates to resume the task below this one in the
     * task stack. */
    Int32 mTaskToReturnTo;

    AutoPtr<IBitmap> mLastThumbnail; // Last thumbnail captured for this item.
    AutoPtr<IFile> mLastThumbnailFile; // File containing last thubmnail.
    String mFilename;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_TASKRECORD_H__
