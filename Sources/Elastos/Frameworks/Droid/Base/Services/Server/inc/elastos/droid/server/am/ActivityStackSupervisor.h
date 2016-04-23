#ifndef __ELASTOS_DROID_SERVER_AM_ACTIVITYSTACKSUPERVISOR_H__
#define __ELASTOS_DROID_SERVER_AM_ACTIVITYSTACKSUPERVISOR_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/server/am/ActivityRecord.h"
#include "elastos/droid/server/am/ActivityStack.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/am/LockTaskNotify.h"
#include "elastos/droid/server/am/ActivityStack.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"

using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::App::Admin::IIDevicePolicyManager;
using Elastos::Droid::App::IIActivityContainer;
using Elastos::Droid::App::IActivityContainerCallback;
using Elastos::Droid::App::IActivityManagerStackInfo;
using Elastos::Droid::App::IActivityManagerWaitResult;
using Elastos::Droid::App::IApplicationThread;
using Elastos::Droid::App::IProfilerInfo;
using Elastos::Droid::Content::IIIntentSender;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Hardware::Display::IDisplayListener;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Hardware::Display::IVirtualDisplay;
using Elastos::Droid::Hardware::Input::IInputManagerInternal;
using Elastos::Droid::Internal::App::IIVoiceInteractor;
using Elastos::Droid::Internal::StatusBar::IIStatusBarService;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Server::Am::IUserStartedState;
using Elastos::Droid::Server::Wm::CWindowManagerService;
using Elastos::Droid::Service::Voice::IIVoiceInteractionSession;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IDisplayInfo;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::ISurface;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ActivityStackSupervisor
    : public Object
    , public IDisplayListener
{
public:
    /**
      * Description of a request to start a new activity, which has been held
      * due to app switches being disabled.
      */
    class PendingActivityLaunch
        : public Object
    {
    public:
        PendingActivityLaunch(
            /* [in] */ ActivityRecord* _r,
            /* [in] */ ActivityRecord* _sourceRecord,
            /* [in] */ Int32 _startFlags,
            /* [in] */ ActivityStack* _stack);

    public:
        AutoPtr<ActivityRecord> mR;
        AutoPtr<ActivityRecord> mSourceRecord;
        Int32 mStartFlags;
        AutoPtr<ActivityStack> mStack;
    };

    class ActivityStackSupervisorHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("ActivityStackSupervisor::ActivityStackSupervisorHandler")

        ActivityStackSupervisorHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ ActivityStackSupervisor* owner);

        CARAPI ActivityIdleInternal(
            /* [in] */ ActivityRecord* r);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        ActivityStackSupervisor* mOwner;
    };

public:
    CAR_INTERFACE_DECL();

    ActivityStackSupervisor(
        /* [in] */ CActivityManagerService* service);

    /**
      * At the time when the constructor runs, the power manager has not yet been
      * initialized.  So we initialize our wakelocks afterwards.
      */
    virtual CARAPI InitPowerManagement();

    virtual CARAPI SetWindowManager(
        /* [in] */ CWindowManagerService* wm);

    virtual CARAPI NotifyActivityDrawnForKeyguard();

    virtual CARAPI_(AutoPtr<ActivityStack>) GetFocusedStack();

    virtual CARAPI_(AutoPtr<ActivityStack>) GetLastStack();

    // TODO: Split into two methods isFrontStack for any visible stack and isFrontmostStack for the
    // top of all visible stacks.
    virtual CARAPI_(Boolean) IsFrontStack(
        /* [in] */ ActivityStack* stack);

    virtual CARAPI MoveHomeStack(
        /* [in] */ Boolean toFront);

    virtual CARAPI MoveHomeStackTaskToTop(
        /* [in] */ Int32 homeStackTaskType);

    virtual CARAPI_(Boolean) ResumeHomeStackTask(
        /* [in] */ Int32 homeStackTaskType,
        /* [in] */ ActivityRecord* prev);

    virtual CARAPI KeyguardWaitingForActivityDrawn();

    virtual CARAPI_(AutoPtr<TaskRecord>) AnyTaskForIdLocked(
        /* [in] */ Int32 id);

    virtual CARAPI_(AutoPtr<ActivityRecord>) IsInAnyStackLocked(
        /* [in] */ IBinder* token);

    virtual CARAPI SetNextTaskId(
        /* [in] */ Int32 taskId);

    virtual CARAPI_(Int32) GetNextTaskId();

    virtual CARAPI_(AutoPtr<ActivityRecord>) ResumedAppLocked();

    virtual CARAPI AttachApplicationLocked(
        /* [in] */ ProcessRecord* app,
        /* [out] */ Boolean* result);

    virtual CARAPI_(Boolean) AllResumedActivitiesIdle();

    virtual CARAPI_(Boolean) AllResumedActivitiesComplete();

    virtual CARAPI_(Boolean) AllResumedActivitiesVisible();

    /**
      * Pause all activities in either all of the stacks or just the back stacks.
      * @param userLeaving Passed to pauseActivity() to indicate whether to call onUserLeaving().
      * @return true if any activity was paused as a result of this call.
      */
    virtual CARAPI_(Boolean) PauseBackStacks(
        /* [in] */ Boolean userLeaving,
        /* [in] */ Boolean resuming,
        /* [in] */ Boolean dontWait);

    virtual CARAPI_(Boolean) AllPausedActivitiesComplete();

    virtual CARAPI PauseChildStacks(
        /* [in] */ ActivityRecord* parent,
        /* [in] */ Boolean userLeaving,
        /* [in] */ Boolean uiSleeping,
        /* [in] */ Boolean resuming,
        /* [in] */ Boolean dontWait);

    virtual CARAPI ReportActivityVisibleLocked(
        /* [in] */ ActivityRecord* r);

    virtual CARAPI SendWaitingVisibleReportLocked(
        /* [in] */ ActivityRecord* r);

    virtual CARAPI ReportActivityLaunchedLocked(
        /* [in] */ Boolean timeout,
        /* [in] */ ActivityRecord* r,
        /* [in] */ Int64 thisTime,
        /* [in] */ Int64 totalTime);

    virtual CARAPI_(AutoPtr<ActivityRecord>) TopRunningActivityLocked();

    virtual CARAPI GetTasksLocked(
        /* [in] */ Int32 maxNum,
        /* [in] */ IList* list,// IActivityManagerRunningTaskInfo
        /* [in] */ Int32 callingUid,
        /* [in] */ Boolean allowed);

    virtual CARAPI_(AutoPtr<IActivityInfo>) ResolveActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 startFlags,
        /* [in] */ IProfilerInfo* profilerInfo,
        /* [in] */ Int32 userId);

    virtual CARAPI StartHomeActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ IActivityInfo* aInfo);

    virtual CARAPI StartActivityMayWait(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ Int32 callingUid,
        /* [in] */ const String& callingPackage,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IIVoiceInteractionSession* voiceSession,
        /* [in] */ IIVoiceInteractor* voiceInteractor,
        /* [in] */ IBinder* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 startFlags,
        /* [in] */ IProfilerInfo* profilerInfo,
        /* [in] */ IActivityManagerWaitResult* outResult,
        /* [in] */ IConfiguration* config,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [in] */ IIActivityContainer* iContainer,
        /* [in] */ TaskRecord* inTask,
        /* [out] */ Int32* result);

    virtual CARAPI StartActivities(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ Int32 callingUid,
        /* [in] */ const String& callingPackage,
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ ArrayOf<String>* resolvedTypes,
        /* [in] */ IBinder* resultTo,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    virtual CARAPI RealStartActivityLocked(
        /* [in] */ ActivityRecord* r,
        /* [in] */ ProcessRecord* app,
        /* [in] */ Boolean andResume,
        /* [in] */ Boolean checkConfig,
        /* [out] */ Boolean* result);

    virtual CARAPI StartSpecificActivityLocked(
        /* [in] */ ActivityRecord* r,
        /* [in] */ Boolean andResume,
        /* [in] */ Boolean checkConfig);

    virtual CARAPI StartActivityLocked(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IActivityInfo* aInfo,
        /* [in] */ IIVoiceInteractionSession* voiceSession,
        /* [in] */ IIVoiceInteractor* voiceInteractor,
        /* [in] */ IBinder* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid,
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 realCallingPid,
        /* [in] */ Int32 realCallingUid,
        /* [in] */ Int32 startFlags,
        /* [in] */ IBundle* options,
        /* [in] */ Boolean componentSpecified,
        /* [in] */ ArrayOf<ActivityRecord*>* outActivity,
        /* [in] */ IIActivityContainer* container,
        /* [in] */ TaskRecord* inTask,
        /* [out] */ Int32* result);

    virtual CARAPI_(AutoPtr<ActivityStack>) AdjustStackFocus(
        /* [in] */ ActivityRecord* r,
        /* [in] */ Boolean newTask);

    virtual CARAPI SetFocusedStack(
        /* [in] */ ActivityRecord* r);

    virtual CARAPI StartActivityUncheckedLocked(
        /* [in] */ ActivityRecord* r,
        /* [in] */ ActivityRecord* sourceRecord,
        /* [in] */ IIVoiceInteractionSession* voiceSession,
        /* [in] */ IIVoiceInteractor* voiceInteractor,
        /* [in] */ Int32 startFlags,
        /* [in] */ Boolean doResume,
        /* [in] */ IBundle* options,
        /* [in] */ TaskRecord* inTask,
        /* [out] */ Int32* result);

    virtual CARAPI DoPendingActivityLaunchesLocked(
        /* [in] */ Boolean doResume);

    virtual CARAPI RemovePendingActivityLaunchesLocked(
        /* [in] */ ActivityStack* stack);

    virtual CARAPI AcquireLaunchWakelock();

    // Checked.
    virtual CARAPI ActivityIdleInternalLocked(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean fromTimeout,
        /* [in] */ IConfiguration* config,
        /* [out] */ ActivityRecord** result);

    virtual CARAPI_(Boolean) HandleAppDiedLocked(
        /* [in] */ ProcessRecord* app);

    virtual CARAPI CloseSystemDialogsLocked();

    virtual CARAPI RemoveUserLocked(
        /* [in] */ Int32 userId);

    /**
      * @return true if some activity was finished (or would have finished if doit were true).
      */
    virtual CARAPI_(Boolean) ForceStopPackageLocked(
        /* [in] */ const String& name,
        /* [in] */ Boolean doit,
        /* [in] */ Boolean evenPersistent,
        /* [in] */ Int32 userId);

    virtual CARAPI UpdatePreviousProcessLocked(
        /* [in] */ ActivityRecord* r);

    virtual CARAPI_(Boolean) ResumeTopActivitiesLocked();

    virtual CARAPI_(Boolean) ResumeTopActivitiesLocked(
        /* [in] */ ActivityStack* targetStack,
        /* [in] */ ActivityRecord* target,
        /* [in] */ IBundle* targetOptions);

    virtual CARAPI FinishTopRunningActivityLocked(
        /* [in] */ ProcessRecord* app);

    virtual CARAPI FinishVoiceTask(
        /* [in] */ IIVoiceInteractionSession* session);

    virtual CARAPI FindTaskToMoveToFrontLocked(
        /* [in] */ TaskRecord* task,
        /* [in] */ Int32 flags,
        /* [in] */ IBundle* options);

    virtual CARAPI_(AutoPtr<ActivityStack>) GetStack(
        /* [in] */ Int32 stackId);

    virtual CARAPI_(AutoPtr<IArrayList>) GetStacks();//ActivityStack

    virtual CARAPI_(AutoPtr<IBinder>) GetHomeActivityToken();

    virtual CARAPI_(AutoPtr<ActivityRecord>) GetHomeActivity();

    virtual CARAPI_(AutoPtr<IIActivityContainer>) CreateActivityContainer(
        /* [in] */ ActivityRecord* parentActivity,
        /* [in] */ IActivityContainerCallback* callback);

    virtual CARAPI RemoveChildActivityContainers(
        /* [in] */ ActivityRecord* parentActivity);

    virtual CARAPI DeleteActivityContainer(
        /* [in] */ IIActivityContainer* container);

    virtual CARAPI_(Int32) GetNextStackId();

    virtual CARAPI CreateStackForRestoredTaskHistory(
        /* [in] */ List<AutoPtr<TaskRecord> >* tasks);

    virtual CARAPI MoveTaskToStack(
        /* [in] */ Int32 taskId,
        /* [in] */ Int32 stackId,
        /* [in] */ Boolean toTop);

    virtual CARAPI_(AutoPtr<ActivityRecord>) FindTaskLocked(
        /* [in] */ ActivityRecord* r);

    virtual CARAPI_(AutoPtr<ActivityRecord>) FindActivityLocked(
        /* [in] */ IIntent* intent,
        /* [in] */ IActivityInfo* info);

    virtual CARAPI GoingToSleepLocked();

    virtual CARAPI_(Boolean) ShutdownLocked(
        /* [in] */ Int32 timeout);

    virtual CARAPI ComeOutOfSleepIfNeededLocked();

    virtual CARAPI ActivitySleptLocked(
        /* [in] */ ActivityRecord* r);

    virtual CARAPI CheckReadyForSleepLocked();

    virtual CARAPI_(Boolean) ReportResumedActivityLocked(
        /* [in] */ ActivityRecord* r);

    virtual CARAPI HandleAppCrashLocked(
        /* [in] */ ProcessRecord* app);

    virtual CARAPI_(Boolean) RequestVisibleBehindLocked(
        /* [in] */ ActivityRecord* r,
        /* [in] */ Boolean visible);

    // Called when WindowManager has finished animating the launchingBehind activity to the back.
    virtual CARAPI HandleLaunchTaskBehindCompleteLocked(
        /* [in] */ ActivityRecord* r);

    virtual CARAPI ScheduleLaunchTaskBehindComplete(
        /* [in] */ IBinder* token);

    virtual CARAPI EnsureActivitiesVisibleLocked(
        /* [in] */ ActivityRecord* starting,
        /* [in] */ Int32 configChanges);

    virtual CARAPI ScheduleDestroyAllActivities(
        /* [in] */ ProcessRecord* app,
        /* [in] */ const String& reason);

    virtual CARAPI ReleaseSomeActivitiesLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ const String& reason);

    virtual CARAPI_(Boolean) SwitchUserLocked(
        /* [in] */ Int32 userId,
        /* [in] */ IUserStartedState* uss);

    /**
      * Add background users to send boot completed events to.
      * @param userId The user being started in the background
      * @param uss The state object for the user.
      */
    virtual CARAPI StartBackgroundUserLocked(
        /* [in] */ Int32 userId,
        /* [in] */ IUserStartedState* uss);

    virtual CARAPI_(AutoPtr<IArrayList>) ProcessStoppingActivitiesLocked(
        /* [in] */ Boolean remove);//ActivityRecord

    virtual CARAPI ValidateTopActivitiesLocked();

    virtual CARAPI Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    virtual CARAPI_(AutoPtr<IArrayList>) GetDumpActivitiesLocked(//ActivityRecord
        /* [in] */ const String& name);

    static CARAPI_(Boolean) PrintThisActivity(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ActivityRecord* activity,
        /* [in] */ const String& dumpPackage,
        /* [in] */ Boolean needSep,
        /* [in] */ const String& prefix);

    virtual CARAPI_(Boolean) DumpActivitiesLocked(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Boolean dumpAll,
        /* [in] */ Boolean dumpClient,
        /* [in] */ const String& dumpPackage);

    static CARAPI_(Boolean) DumpHistoryList(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ IList* list,// ActivityRecord
        /* [in] */ const String& prefix,
        /* [in] */ const String& label,
        /* [in] */ Boolean complete,
        /* [in] */ Boolean brief,
        /* [in] */ Boolean client,
        /* [in] */ const String& dumpPackage,
        /* [in] */ Boolean needNL,
        /* [in] */ const String& header1,
        /* [in] */ const String& header2);

    virtual CARAPI ScheduleIdleTimeoutLocked(
        /* [in] */ ActivityRecord* next);

    virtual CARAPI ScheduleIdleLocked();

    virtual CARAPI RemoveTimeoutsForActivityLocked(
        /* [in] */ ActivityRecord* r);

    virtual CARAPI ScheduleResumeTopActivities();

    virtual CARAPI RemoveSleepTimeouts();

    virtual CARAPI ScheduleSleepTimeout();

    // @Override
    CARAPI OnDisplayAdded(
        /* [in] */ Int32 displayId);

    // @Override
    CARAPI OnDisplayRemoved(
        /* [in] */ Int32 displayId);

    // @Override
    CARAPI OnDisplayChanged(
        /* [in] */ Int32 displayId);

    virtual CARAPI HandleDisplayAddedLocked(
        /* [in] */ Int32 displayId);

    virtual CARAPI HandleDisplayRemovedLocked(
        /* [in] */ Int32 displayId);

    virtual CARAPI HandleDisplayChangedLocked(
        /* [in] */ Int32 displayId);

    virtual CARAPI_(AutoPtr<IActivityManagerStackInfo>) GetStackInfo(
        /* [in] */ ActivityStack* stack);

    virtual CARAPI_(AutoPtr<IActivityManagerStackInfo>) GetStackInfoLocked(
        /* [in] */ Int32 stackId);

    virtual CARAPI_(AutoPtr<IArrayList>) GetAllStackInfosLocked();//IActivityManagerStackInfo

    virtual CARAPI ShowLockTaskToast();

    virtual CARAPI SetLockTaskModeLocked(
        /* [in] */ TaskRecord* task,
        /* [in] */ Boolean isLocked);

    virtual CARAPI_(Boolean) IsLockTaskModeViolation(
        /* [in] */ TaskRecord* task);

    virtual CARAPI EndLockTaskModeIfTaskEnding(
        /* [in] */ TaskRecord* task);

    virtual CARAPI_(Boolean) IsInLockTaskMode();

private:
    // This function returns a IStatusBarService. The value is from ServiceManager.
    // getService and is cached.
    CARAPI_(AutoPtr<IIStatusBarService>) GetStatusBarService();

    CARAPI_(AutoPtr<IIDevicePolicyManager>) GetDevicePolicyManager();

    CARAPI_(Int32) CreateStackOnDisplay(
        /* [in] */ Int32 stackId,
        /* [in] */ Int32 displayId);

    CARAPI_(Boolean) IsLeanbackOnlyDevice();

public:
    static const Boolean DEBUG;
    static const Boolean DEBUG_ADD_REMOVE;
    static const Boolean DEBUG_APP;
    static const Boolean DEBUG_CONTAINERS;
    static const Boolean DEBUG_IDLE;
    static const Boolean DEBUG_RELEASE;
    static const Boolean DEBUG_SAVED_STATE;
    static const Boolean DEBUG_SCREENSHOTS;
    static const Boolean DEBUG_STATES;
    static const Boolean DEBUG_VISIBLE_BEHIND;
    static const Int32 HOME_STACK_ID = 0;
    /** How long we wait until giving up on the last activity telling us it is idle. */
    static const Int32 IDLE_TIMEOUT = 10*1000;
    /** How long we can hold the sleep wake lock before giving up. */
    static const Int32 SLEEP_TIMEOUT = 5*1000;
    // How long we can hold the launch wake lock before giving up.
    static const Int32 LAUNCH_TIMEOUT = 10*1000;
    static const Int32 IDLE_TIMEOUT_MSG = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG;
    static const Int32 IDLE_NOW_MSG = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 1;
    static const Int32 RESUME_TOP_ACTIVITY_MSG = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 2;
    static const Int32 SLEEP_TIMEOUT_MSG = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 3;
    static const Int32 LAUNCH_TIMEOUT_MSG = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 4;
    static const Int32 HANDLE_DISPLAY_ADDED = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 5;
    static const Int32 HANDLE_DISPLAY_CHANGED = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 6;
    static const Int32 HANDLE_DISPLAY_REMOVED = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 7;
    static const Int32 CONTAINER_CALLBACK_VISIBILITY = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 8;
    static const Int32 LOCK_TASK_START_MSG = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 9;
    static const Int32 LOCK_TASK_END_MSG = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 10;
    static const Int32 CONTAINER_CALLBACK_TASK_LIST_EMPTY = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 11;
    static const Int32 CONTAINER_TASK_LIST_EMPTY_TIMEOUT = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 12;
    static const Int32 LAUNCH_TASK_BEHIND_COMPLETE = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 13;
    // For debugging to make sure the caller when acquiring/releasing our
    // wake lock is the system process.
    static const Boolean VALIDATE_WAKE_LOCK_CALLER;
    AutoPtr<CActivityManagerService> mService;
    AutoPtr<ActivityStackSupervisorHandler> mHandler;
    /** Short cut */
    AutoPtr<CWindowManagerService> mWindowManager;
    AutoPtr<IDisplayManager> mDisplayManager;
    /** List of activities that are waiting for a new activity to become visible before completing
      * whatever operation they are supposed to do. */
    AutoPtr<IArrayList> mWaitingVisibleActivities;//ActivityRecord
    /** List of processes waiting to find out about the next visible activity. */
    AutoPtr<IArrayList> mWaitingActivityVisible;//IActivityManagerWaitResult
    /** List of processes waiting to find out about the next launched activity. */
    AutoPtr<IArrayList> mWaitingActivityLaunched;//IActivityManagerWaitResult
    /** List of activities that are ready to be stopped, but waiting for the next activity to
      * settle down before doing so. */
    AutoPtr<IArrayList> mStoppingActivities;// ActivityRecord
    /** List of activities that are ready to be finished, but waiting for the previous activity to
      * settle down before doing so.  It contains ActivityRecord objects. */
    AutoPtr<IArrayList> mFinishingActivities; // ActivityRecord
    /** List of activities that are in the process of going to sleep. */
    AutoPtr<IArrayList> mGoingToSleepActivities; // ActivityRecord
    /** Used on user changes */
    AutoPtr<IArrayList> mStartingUsers; // IUserStartedState
    /** Used to queue up any background users being started */
    AutoPtr<IArrayList> mStartingBackgroundUsers; // IUserStartedState
    /** Set to indicate whether to issue an onUserLeaving callback when a newly launched activity
      * is being brought in front of us. */
    Boolean mUserLeaving;
    /** Set when we have taken too long waiting to go to sleep. */
    Boolean mSleepTimeout;
    /**
     * Is the privacy guard currently enabled? Shared between ActivityStacks
     */
    String mPrivacyGuardPackageName;

    AutoPtr<IPowerManager> mPm;
    /**
      * We don't want to allow the device to go to sleep while in the process
      * of launching an activity.  This is primarily to allow alarm intent
      * receivers to launch an activity and get that to run before the device
      * goes back to sleep.
      */
    AutoPtr<IPowerManagerWakeLock> mLaunchingActivity; // IPowerManagerWakeLock
    /**
      * Set when the system is going to sleep, until we have
      * successfully paused the current activity and released our wake lock.
      * At that point the system is allowed to actually sleep.
      */
    AutoPtr<IPowerManagerWakeLock> mGoingToSleep;
    /** Stack id of the front stack when user switched, indexed by userId. */
    AutoPtr<ISparseInt32Array> mUserStackInFront;
    AutoPtr<IInputManagerInternal> mInputManagerInternal;
    /** If non-null then the task specified remains in front and no other tasks may be started
      * until the task exits or #stopLockTaskMode() is called. */
    AutoPtr<TaskRecord> mLockTaskModeTask;
    AutoPtr<IArrayList> mPendingActivityLaunches; // PendingActivityLaunch

private:
    friend class ActivityContainer;
    friend class VirtualActivityContainer;

    static const String LOCK_TASK_TAG;
    /** Status Bar Service **/
    AutoPtr<IBinder> mToken;
    AutoPtr<IIStatusBarService> mStatusBarService;
    AutoPtr<IIDevicePolicyManager> mDevicePolicyManager;
    /** Identifier counter for all ActivityStacks */
    Int32 mLastStackId;
    /** Task identifier that activities are currently being started in.  Incremented each time a
      * new task is created. */
    Int32 mCurTaskId;
    /** The current user */
    Int32 mCurrentUser;
    /** The stack containing the launcher app. Assumed to always be attached to
      * Display.DEFAULT_DISPLAY. */
    AutoPtr<ActivityStack> mHomeStack;
    /** The stack currently receiving input or launching the next activity. */
    AutoPtr<ActivityStack> mFocusedStack;
    /** If this is the same as mFocusedStack then the activity on the top of the focused stack has
      * been resumed. If stacks are changing position this will hold the old stack until the new
      * stack becomes resumed after which it will be set to mFocusedStack. */
    AutoPtr<ActivityStack> mLastFocusedStack;
    /** Indicates if we are running on a Leanback-only (TV) device. Only initialized after
      * setWindowManager is called. **/
    Boolean mLeanbackOnlyDevice;
    // TODO: Add listener for removal of references.
    /** Mapping from (ActivityStack/TaskStack).mStackId to their current state */
    AutoPtr<ISparseArray> mActivityContainers; // ActivityContainer
    /** Mapping from displayId to display current state */
    AutoPtr<ISparseArray> mActivityDisplays; // ActivityDisplay
    /** Whether lock task has been entered by an authorized app and cannot
      * be exited. */
    Boolean mLockTaskIsLocked;
    /**
      * Notifies the user when entering/exiting lock-task.
      */
    AutoPtr<LockTaskNotify> mLockTaskNotify;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_ACTIVITYSTACKSUPERVISOR_H__
