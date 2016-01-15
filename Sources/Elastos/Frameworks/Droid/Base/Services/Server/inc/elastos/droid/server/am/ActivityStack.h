#ifndef __ELASTOS_DROID_SERVER_AM_ACTIVITYSTACK_H__
#define __ELASTOS_DROID_SERVER_AM_ACTIVITYSTACK_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/server/am/ActivityStackSupervisor.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
//TODO #include "elastos/droid/server/wm/CWindowManagerService.h"

using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Internal::App::IIVoiceInteractor;

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPersistableBundle;
//TODO using Elastos::Droid::Server::Wm::CWindowManagerService;
using Elastos::Droid::Service::Voice::IIVoiceInteractionSession;
using Elastos::Droid::Utility::IArraySet;

using Elastos::Core::ICharSequence;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

/**
  * State and management of a single stack of activities.
  */
class ActivityStack
    : public Object
{
protected:
    class ScheduleDestroyArgs
        : public Object
    {
    public:
        ScheduleDestroyArgs(
            /* [in] */ ProcessRecord* owner,
            /* [in] */ const String& reason);

    public:
        AutoPtr<ProcessRecord> mOwner;
        String mReason;
    };

    class ActivityStackHandler
        : public Handler
    {
    public:
        ActivityStackHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ ActivityStack* owner);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        ActivityStack* mOwner;
    };

protected:
    /*
    enum ActivityState {
        ActivityState_INITIALIZING,
        ActivityState_RESUMED,
        ActivityState_PAUSING,
        ActivityState_PAUSED,
        ActivityState_STOPPING,
        ActivityState_STOPPED,
        ActivityState_FINISHING,
        ActivityState_DESTROYING,
        ActivityState_DESTROYED
    };
    */

public:
    ActivityStack(
        /* [in] */ ActivityStackSupervisor::ActivityContainer* activityContainer);

    CARAPI_(Int32) NumActivities();

    CARAPI_(Boolean) OkToShowLocked(
        /* [in] */ ActivityRecord* r);

    CARAPI_(AutoPtr<ActivityRecord>) TopRunningActivityLocked(
        /* [in] */ ActivityRecord* notTop);

    CARAPI_(AutoPtr<ActivityRecord>) TopRunningNonDelayedActivityLocked(
        /* [in] */ ActivityRecord* notTop);

    /**
      * This is a simplified version of topRunningActivityLocked that provides a number of
      * optional skip-over modes.  It is intended for use with the ActivityController hook only.
      *
      * @param token If non-null, any history records matching this token will be skipped.
      * @param taskId If non-zero, we'll attempt to skip over records with the same task ID.
      *
      * @return Returns the HistoryRecord of the next activity on the stack.
      */
    CARAPI_(AutoPtr<ActivityRecord>) TopRunningActivityLocked(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 taskId);

    CARAPI_(AutoPtr<ActivityRecord>) TopActivity();

    CARAPI_(AutoPtr<TaskRecord>) TopTask();

    CARAPI_(AutoPtr<TaskRecord>) TaskForIdLocked(
        /* [in] */ Int32 id);

    CARAPI_(AutoPtr<ActivityRecord>) IsInStackLocked(
        /* [in] */ IBinder* token);

    CARAPI_(Boolean) UpdateLRUListLocked(
        /* [in] */ ActivityRecord* r);

    CARAPI_(Boolean) IsHomeStack();

    CARAPI_(Boolean) IsOnHomeDisplay();

    CARAPI_(void) MoveToFront();

    CARAPI_(Boolean) IsAttached();

    /**
      * Returns the top activity in any existing task matching the given
      * Intent.  Returns null if no such task is found.
      */
    CARAPI_(AutoPtr<ActivityRecord>) FindTaskLocked(
        /* [in] */ ActivityRecord* target);

    /**
      * Returns the first activity (starting from the top of the stack) that
      * is the same as the given activity.  Returns null if no such activity
      * is found.
      */
    CARAPI_(AutoPtr<ActivityRecord>) FindActivityLocked(
        /* [in] */ IIntent* intent,
        /* [in] */ IActivityInfo* info);

    /*
      * Move the activities around in the stack to bring a user to the foreground.
      */
    CARAPI_(void) SwitchUserLocked(
        /* [in] */ Int32 userId);

    CARAPI_(void) MinimalResumeActivityLocked(
        /* [in] */ ActivityRecord* r);

    CARAPI_(void) SetLaunchTime(
        /* [in] */ ActivityRecord* r);

    CARAPI_(void) ClearLaunchTime(
        /* [in] */ ActivityRecord* r);

    CARAPI_(void) AwakeFromSleepingLocked();

    /**
      * @return true if something must be done before going to sleep.
      */
    CARAPI_(Boolean) CheckReadyForSleepLocked();

    CARAPI_(void) GoToSleep();

    CARAPI_(AutoPtr<IBitmap>) ScreenshotActivities(
        /* [in] */ ActivityRecord* who);

    /**
      * Start pausing the currently resumed activity.  It is an error to call this if there
      * is already an activity being paused or there is no resumed activity.
      *
      * @param userLeaving True if this should result in an onUserLeaving to the current activity.
      * @param uiSleeping True if this is happening with the user interface going to sleep (the
      * screen turning off).
      * @param resuming True if this is being called as part of resuming the top activity, so
      * we shouldn't try to instigate a resume here.
      * @param dontWait True if the caller does not want to wait for the pause to complete.  If
      * set to true, we will immediately complete the pause here before returning.
      * @return Returns true if an activity now is in the PAUSING state, and we are waiting for
      * it to tell us when it is done.
      */
    CARAPI_(Boolean) StartPausingLocked(
        /* [in] */ Boolean userLeaving,
        /* [in] */ Boolean uiSleeping,
        /* [in] */ Boolean resuming,
        /* [in] */ Boolean dontWait);

    CARAPI_(void) ActivityPausedLocked(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean timeout);

    CARAPI_(void) ActivityStoppedLocked(
        /* [in] */ ActivityRecord* r,
        /* [in] */ IBundle* icicle,
        /* [in] */ IPersistableBundle* persistentState,
        /* [in] */ ICharSequence* description);

    // Find the first visible activity above the passed activity and if it is translucent return it
    // otherwise return null;
    CARAPI_(AutoPtr<ActivityRecord>) FindNextTranslucentActivity(
        /* [in] */ ActivityRecord* r);

    /**
      * Make sure that all activities that need to be visible (that is, they
      * currently can be seen by the user) actually are.
      */
    CARAPI_(void) EnsureActivitiesVisibleLocked(
        /* [in] */ ActivityRecord* starting,
        /* [in] */ Int32 configChanges);

    CARAPI_(void) ConvertToTranslucent(
        /* [in] */ ActivityRecord* r);

    /**
      * Called as activities below the top translucent activity are redrawn. When the last one is
      * redrawn notify the top activity by calling
      * {@link Activity#onTranslucentConversionComplete}.
      *
      * @param r The most recent background activity to be drawn. Or, if r is null then a timeout
      * occurred and the activity will be notified immediately.
      */
    CARAPI_(void) NotifyActivityDrawnLocked(
        /* [in] */ ActivityRecord* r);

    /** If any activities below the top running one are in the INITIALIZING state and they have a
      * starting window displayed then remove that starting window. It is possible that the activity
      * in this state will never resumed in which case that starting window will be orphaned. */
    CARAPI_(void) CancelInitializingActivities();

    /**
      * Ensure that the top activity in the stack is resumed.
      *
      * @param prev The previously resumed activity, for when in the process
      * of pausing; can be null to call from elsewhere.
      *
      * @return Returns true if something is being resumed, or false if
      * nothing happened.
      */
    CARAPI_(Boolean) ResumeTopActivityLocked(
        /* [in] */ ActivityRecord* prev);

    CARAPI_(Boolean) ResumeTopActivityLocked(
        /* [in] */ ActivityRecord* prev,
        /* [in] */ IBundle* options);

    CARAPI_(Boolean) ResumeTopActivityInnerLocked(
        /* [in] */ ActivityRecord* prev,
        /* [in] */ IBundle* options);

    CARAPI_(void) StartActivityLocked(
        /* [in] */ ActivityRecord* r,
        /* [in] */ Boolean newTask,
        /* [in] */ Boolean doResume,
        /* [in] */ Boolean keepCurTransition,
        /* [in] */ IBundle* options);

    CARAPI_(void) ValidateAppTokensLocked();

    /**
      * Perform a reset of the given task, if needed as part of launching it.
      * Returns the new HistoryRecord at the top of the task.
      */
    /**
      * Helper method for #resetTaskIfNeededLocked.
      * We are inside of the task being reset...  we'll either finish this activity, push it out
      * for another task, or leave it as-is.
      * @param task The task containing the Activity (taskTop) that might be reset.
      * @param forceReset
      * @return An ActivityOptions that needs to be processed.
      */
    CARAPI_(AutoPtr<IActivityOptions>) ResetTargetTaskIfNeededLocked(
        /* [in] */ TaskRecord* task,
        /* [in] */ Boolean forceReset);

    CARAPI_(AutoPtr<ActivityRecord>) ResetTaskIfNeededLocked(
        /* [in] */ ActivityRecord* taskTop,
        /* [in] */ ActivityRecord* newActivity);

    CARAPI_(void) SendActivityResultLocked(
        /* [in] */ Int32 callingUid,
        /* [in] */ ActivityRecord* r,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    CARAPI_(void) StopActivityLocked(
        /* [in] */ ActivityRecord* r);

    /**
      * @return Returns true if the activity is being finished, false if for
      * some reason it is being left as-is.
      */
    CARAPI_(Boolean) RequestFinishActivityLocked(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* resultData,
        /* [in] */ const String& reason,
        /* [in] */ Boolean oomAdj);

    CARAPI_(void) FinishSubActivityLocked(
        /* [in] */ ActivityRecord* self,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode);

    CARAPI_(void) FinishTopRunningActivityLocked(
        /* [in] */ ProcessRecord* app);

    CARAPI_(void) FinishVoiceTask(
        /* [in] */ IIVoiceInteractionSession* session);

    CARAPI_(Boolean) FinishActivityAffinityLocked(
        /* [in] */ ActivityRecord* r);

    CARAPI_(void) FinishActivityResultsLocked(
        /* [in] */ ActivityRecord* r,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* resultData);

    /**
      * @return Returns true if this activity has been removed from the history
      * list, or false if it is still in the list and will be removed later.
      */
    CARAPI_(Boolean) FinishActivityLocked(
        /* [in] */ ActivityRecord* r,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* resultData,
        /* [in] */ const String& reason,
        /* [in] */ Boolean oomAdj);

    CARAPI_(AutoPtr<ActivityRecord>) FinishCurrentActivityLocked(
        /* [in] */ ActivityRecord* r,
        /* [in] */ Int32 mode,
        /* [in] */ Boolean oomAdj);

    CARAPI_(void) FinishAllActivitiesLocked(
        /* [in] */ Boolean immediately);

    CARAPI_(Boolean) ShouldUpRecreateTaskLocked(
        /* [in] */ ActivityRecord* srec,
        /* [in] */ const String& destAffinity);

    CARAPI_(Boolean) NavigateUpToLocked(
        /* [in] */ IBinder* token,
        /* [in] */ IIntent* destIntent,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* resultData);

    /**
      * Perform the common clean-up of an activity record.  This is called both
      * as part of destroyActivityLocked() (when destroying the client-side
      * representation) and cleaning things up as a result of its hosting
      * processing going away, in which case there is no remaining client-side
      * state to destroy so only the cleanup here is needed.
      */
    CARAPI_(void) CleanUpActivityLocked(
        /* [in] */ ActivityRecord* r,
        /* [in] */ Boolean cleanServices,
        /* [in] */ Boolean setState);

    /**
      * Perform clean-up of service connections in an activity record.
      */
    CARAPI_(void) CleanUpActivityServicesLocked(
        /* [in] */ ActivityRecord* r);

    CARAPI_(void) ScheduleDestroyActivities(
        /* [in] */ ProcessRecord* owner,
        /* [in] */ const String& reason);

    CARAPI_(void) DestroyActivitiesLocked(
        /* [in] */ ProcessRecord* owner,
        /* [in] */ const String& reason);

    CARAPI_(Boolean) SafelyDestroyActivityLocked(
        /* [in] */ ActivityRecord* r,
        /* [in] */ const String& reason);

    CARAPI_(Int32) ReleaseSomeActivitiesLocked(
        /* [in] */ ProcessRecord* app,
        ///* [in] */ IArraySet<TaskRecord*>* tasks,
        /* [in] */ IArraySet* tasks,
        /* [in] */ const String& reason);

    /**
      * Destroy the current CLIENT SIDE instance of an activity.  This may be
      * called both when actually finishing an activity, or when performing
      * a configuration switch where we destroy the current client-side object
      * but then create a new client-side object for this same HistoryRecord.
      */
    CARAPI_(Boolean) DestroyActivityLocked(
        /* [in] */ ActivityRecord* r,
        /* [in] */ Boolean removeFromApp,
        /* [in] */ const String& reason);

    CARAPI_(void) ActivityDestroyedLocked(
        /* [in] */ IBinder* token);

    CARAPI_(void) ReleaseBackgroundResources();

    CARAPI_(void) BackgroundResourcesReleased(
        /* [in] */ IBinder* token);

    CARAPI_(Boolean) HasVisibleBehindActivity();

    CARAPI_(void) SetVisibleBehindActivity(
        /* [in] */ ActivityRecord* r);

    CARAPI_(AutoPtr<ActivityRecord>) GetVisibleBehindActivity();

    CARAPI_(Boolean) RemoveHistoryRecordsForAppLocked(
        /* [in] */ ProcessRecord* app);

    CARAPI_(void) UpdateTransitLocked(
        /* [in] */ Int32 transit,
        /* [in] */ IBundle* options);

    CARAPI_(void) UpdateTaskMovement(
        /* [in] */ TaskRecord* task,
        /* [in] */ Boolean toFront);

    CARAPI_(void) MoveHomeStackTaskToTop(
        /* [in] */ Int32 homeStackTaskType);

    CARAPI_(void) MoveTaskToFrontLocked(
        /* [in] */ TaskRecord* tr,
        /* [in] */ ActivityRecord* reason,
        /* [in] */ IBundle* options);

    /**
      * Worker method for rearranging history stack. Implements the function of moving all
      * activities for a specific task (gathering them if disjoint) into a single group at the
      * bottom of the stack.
      *
      * If a watcher is installed, the action is preflighted and the watcher has an opportunity
      * to premeptively cancel the move.
      *
      * @param taskId The taskId to collect and move to the bottom.
      * @return Returns true if the move completed, false if not.
      */
    CARAPI_(Boolean) MoveTaskToBackLocked(
        /* [in] */ Int32 taskId,
        /* [in] */ ActivityRecord* reason);

    static CARAPI_(void) LogStartActivity(
        /* [in] */ Int32 tag,
        /* [in] */ ActivityRecord* r,
        /* [in] */ TaskRecord* task);

    /**
      * Make sure the given activity matches the current configuration.  Returns
      * false if the activity had to be destroyed.  Returns true if the
      * configuration is the same, or the activity will remain running as-is
      * for whatever reason.  Ensures the HistoryRecord is updated with the
      * correct configuration and all other bookkeeping is handled.
      */
    CARAPI_(Boolean) EnsureActivityConfigurationLocked(
        /* [in] */ ActivityRecord* r,
        /* [in] */ Int32 globalChanges);

    CARAPI_(Boolean) WillActivityBeVisibleLocked(
        /* [in] */ IBinder* token);

    CARAPI_(void) CloseSystemDialogsLocked();

    CARAPI_(Boolean) ForceStopPackageLocked(
        /* [in] */ const String& name,
        /* [in] */ Boolean doit,
        /* [in] */ Boolean evenPersistent,
        /* [in] */ Int32 userId);

    CARAPI_(void) GetTasksLocked(
        /* [in] */ IList* list,//IActivityManagerRunningTaskInfo
        /* [in] */ Int32 callingUid,
        /* [in] */ Boolean allowed);

    CARAPI_(void) UnhandledBackLocked();

    /**
      * Reset local parameters because an app's activity died.
      * @param app The app of the activity that died.
      * @return result from removeHistoryRecordsForAppLocked.
      */
    CARAPI_(Boolean) HandleAppDiedLocked(
        /* [in] */ ProcessRecord* app);

    CARAPI_(void) HandleAppCrashLocked(
        /* [in] */ ProcessRecord* app);

    CARAPI_(Boolean) DumpActivitiesLocked(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Boolean dumpAll,
        /* [in] */ Boolean dumpClient,
        /* [in] */ const String& dumpPackage,
        /* [in] */ Boolean needSep,
        /* [in] */ const String& header);

    CARAPI_(AutoPtr<IArrayList>) GetDumpActivitiesLocked(
        /* [in] */ const String& name);

    CARAPI_(AutoPtr<ActivityRecord>) RestartPackage(
        /* [in] */ const String& packageName);

    CARAPI_(void) RemoveTask(
        /* [in] */ TaskRecord* task);

    CARAPI_(AutoPtr<TaskRecord>) CreateTaskRecord(
        /* [in] */ Int32 taskId,
        /* [in] */ IActivityInfo* info,
        /* [in] */ IIntent* intent,
        /* [in] */ IIVoiceInteractionSession* voiceSession,
        /* [in] */ IIVoiceInteractor* voiceInteractor,
        /* [in] */ Boolean toTop);

    CARAPI_(AutoPtr<IArrayList>) GetAllTasks();

    CARAPI_(void) AddTask(
        /* [in] */ TaskRecord* task,
        /* [in] */ Boolean toTop,
        /* [in] */ Boolean moving);

    CARAPI_(Int32) GetStackId();

    CARAPI ToString(
        /* [out] */ String* result);
    // @Override
    CARAPI_(String) ToString();

private:
    /**
      * Checks whether the userid is a profile of the current user.
      */
    CARAPI_(Boolean) IsCurrentProfileLocked(
        /* [in] */ Int32 userId);

    CARAPI_(void) StartLaunchTraces();

    CARAPI_(void) StopFullyDrawnTraceIfNeeded();

    /**
      * This resets the saved state from the last screenshot, forcing a new screenshot to be taken
      * again when requested.
      */
    CARAPI_(void) InvalidateLastScreenshot();

    CARAPI_(void) CompletePauseLocked(
        /* [in] */ Boolean resumeNext);

    /**
      * Once we know that we have asked an application to put an activity in
      * the resumed state (either by launching it or explicitly telling it),
      * this function updates the rest of our state to match that fact.
      */
    CARAPI_(void) CompleteResumeLocked(
        /* [in] */ ActivityRecord* next);

    CARAPI_(void) SetVisibile(
        /* [in] */ ActivityRecord* r,
        /* [in] */ Boolean visible);

    // Checks if any of the stacks above this one has a fullscreen activity behind it.
    // If so, this stack is hidden, otherwise it is visible.
    CARAPI_(Boolean) IsStackVisible();

    CARAPI_(void) InsertTaskAtTop(
        /* [in] */ TaskRecord* task);

    /**
      * Helper method for #resetTaskIfNeededLocked. Processes all of the activities in a given
      * TaskRecord looking for an affinity with the task of resetTaskIfNeededLocked.taskTop.
      * @param affinityTask The task we are looking for an affinity to.
      * @param task Task that resetTaskIfNeededLocked.taskTop belongs to.
      * @param topTaskIsHigher True if #task has already been processed by resetTaskIfNeededLocked.
      * @param forceReset Flag passed in to resetTaskIfNeededLocked.
      */
    CARAPI_(Int32) ResetAffinityTaskIfNeededLocked(
        /* [in] */ TaskRecord* affinityTask,
        /* [in] */ TaskRecord* task,
        /* [in] */ Boolean topTaskIsHigher,
        /* [in] */ Boolean forceReset,
        /* [in] */ Int32 taskInsertionPoint);

    CARAPI_(void) AdjustFocusedActivityLocked(
        /* [in] */ ActivityRecord* r);

    CARAPI_(void) RemoveTimeoutsForActivityLocked(
        /* [in] */ ActivityRecord* r);

    CARAPI_(void) RemoveActivityFromHistoryLocked(
        /* [in] */ ActivityRecord* r);

    CARAPI_(void) RemoveHistoryRecordsForAppLocked(
        ///* [in] */ IArrayList<ActivityRecord*>* list,
        /* [in] */ IArrayList* list,
        /* [in] */ ProcessRecord* app,
        /* [in] */ const String& listName);

    CARAPI_(Boolean) RelaunchActivityLocked(
        /* [in] */ ActivityRecord* r,
        /* [in] */ Int32 changes,
        /* [in] */ Boolean andResume);

public:
    // Ticks during which we check progress while waiting for an app to launch.
    static const Int32 LAUNCH_TICK = 500;
    // How long we wait until giving up on the last activity to pause.  This
    // is short because it directly impacts the responsiveness of starting the
    // next activity.
    static const Int32 PAUSE_TIMEOUT = 500;
    // How long we wait for the activity to tell us it has stopped before
    // giving up.  This is a good amount of time because we really need this
    // from the application in order to get its saved state.
    static const Int32 STOP_TIMEOUT = 10*1000;
    // How long we wait until giving up on an activity telling us it has
    // finished destroying itself.
    static const Int32 DESTROY_TIMEOUT = 10*1000;
    // How long until we reset a task when the user returns to it.  Currently
    // disabled.
    static const Int64 ACTIVITY_INACTIVE_RESET_TIME = 0;
    // How long between activity launches that we consider safe to not warn
    // the user about an unexpected activity being launched on top.
    static const Int64 START_WARN_TIME = 5*1000;
    // Set to false to disable the preview that is shown while a new activity
    // is being started.
    static const Boolean SHOW_APP_STARTING_PREVIEW;
    // How long to wait for all background Activities to redraw following a call to
    // convertToTranslucent().
    static const Int64 TRANSLUCENT_CONVERSION_TIMEOUT = 2000;
    static const Boolean SCREENSHOT_FORCE_565;

    AutoPtr<CActivityManagerService> mService;
    //TODO AutoPtr<CWindowManagerService> mWindowManager;//TODO CWindowManagerService->IIWindowManager
    /**
      * Used for validating app tokens with window manager.
      */
    AutoPtr<IArrayList> mValidateAppTokens;//ITaskGroup
    /**
      * List of running activities, sorted by recent usage.
      * The first entry in the list is the least recently used.
      * It contains HistoryRecord objects.
      */
    AutoPtr<IArrayList> mLRUActivities;//IActivityRecord
    /**
      * Animations that for the current transition have requested not to
      * be considered for the transition animation.
      */
    AutoPtr<IArrayList> mNoAnimActivities;//IActivityRecord
    /**
      * When we are in the process of pausing an activity, before starting the
      * next one, this variable holds the activity that is currently being paused.
      */
    AutoPtr<ActivityRecord> mPausingActivity;
    /**
      * This is the last activity that we put into the paused state.  This is
      * used to determine if we need to do an activity transition while sleeping,
      * when we normally hold the top activity paused.
      */
    AutoPtr<ActivityRecord> mLastPausedActivity;
    /**
      * Activities that specify No History must be removed once the user navigates away from them.
      * If the device goes to sleep with such an activity in the paused state then we save it here
      * and finish it later if another activity replaces it on wakeup.
      */
    AutoPtr<ActivityRecord> mLastNoHistoryActivity;
    /**
      * Current activity that is resumed, or null if there is none.
      */
    AutoPtr<ActivityRecord> mResumedActivity;
    /**
      * This is the last activity that has been started.  It is only used to
      * identify when multiple activities are started at once so that the user
      * can be warned they may not be in the activity they think they are.
      */
    AutoPtr<ActivityRecord> mLastStartedActivity;
    // The topmost Activity passed to convertToTranslucent(). When non-null it means we are
    // waiting for all Activities in mUndrawnActivitiesBelowTopTranslucent to be removed as they
    // are drawn. When the last member of mUndrawnActivitiesBelowTopTranslucent is removed the
    // Activity in mTranslucentActivityWaiting is notified via
    // Activity.onTranslucentConversionComplete(false). If a timeout occurs prior to the last
    // background activity being drawn then the same call will be made with a true value.
    AutoPtr<ActivityRecord> mTranslucentActivityWaiting;
    /**
      * Set when we know we are going to be calling updateConfiguration()
      * soon, so want to skip intermediate config checks.
      */
    Boolean mConfigWillChange;
    Int64 mLaunchStartTime;
    Int64 mFullyDrawnStartTime;
    Int32 mCurrentUser;
    /*const*/ Int32 mStackId;//TODO
    AutoPtr<ActivityStackSupervisor::ActivityContainer> mActivityContainer;
    /** The other stacks, in order, on the attached display. Updated at attach/detach time. */
    AutoPtr<IArrayList> mStacks;//ActivityStack
    /** The attached Display's unique identifier, or -1 if detached */
    Int32 mDisplayId;
    /** Run all ActivityStacks through this */
    AutoPtr<ActivityStackSupervisor> mStackSupervisor;
    static const Int32 PAUSE_TIMEOUT_MSG = CActivityManagerService::FIRST_ACTIVITY_STACK_MSG + 1;
    static const Int32 DESTROY_TIMEOUT_MSG = CActivityManagerService::FIRST_ACTIVITY_STACK_MSG + 2;
    static const Int32 LAUNCH_TICK_MSG = CActivityManagerService::FIRST_ACTIVITY_STACK_MSG + 3;
    static const Int32 STOP_TIMEOUT_MSG = CActivityManagerService::FIRST_ACTIVITY_STACK_MSG + 4;
    static const Int32 DESTROY_ACTIVITIES_MSG = CActivityManagerService::FIRST_ACTIVITY_STACK_MSG + 5;
    static const Int32 TRANSLUCENT_TIMEOUT_MSG = CActivityManagerService::FIRST_ACTIVITY_STACK_MSG + 6;
    static const Int32 RELEASE_BACKGROUND_RESOURCES_TIMEOUT_MSG = CActivityManagerService::FIRST_ACTIVITY_STACK_MSG + 7;
    AutoPtr<IHandler> mHandler;
    static const Int32 FINISH_IMMEDIATELY = 0;
    static const Int32 FINISH_AFTER_PAUSE = 1;
    static const Int32 FINISH_AFTER_VISIBLE = 2;

private:
    /**
      * The back history of all previous (and possibly still
      * running) activities.  It contains #TaskRecord objects.
      */
    AutoPtr<IArrayList> mTaskHistory;//TaskRecord
    AutoPtr<IArrayList> mUndrawnActivitiesBelowTopTranslucent;//ActivityRecord
    /**
      * Save the most recent screenshot for reuse. This keeps Recents from taking two identical
      * screenshots, one for the Recents thumbnail and one for the pauseActivity thumbnail.
      */
    AutoPtr<ActivityRecord> mLastScreenshotActivity;
    AutoPtr<IBitmap> mLastScreenshotBitmap;
    /** Used to keep resumeTopActivityLocked() from being entered recursively */
    Boolean mInResumeTopActivity;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_ACTIVITYSTACK_H__
