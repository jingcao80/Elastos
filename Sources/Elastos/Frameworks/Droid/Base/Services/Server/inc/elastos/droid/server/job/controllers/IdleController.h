/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 */

package com.android.server.job.controllers;

using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::Server::Ijob.JobSchedulerService;
using Elastos::Droid::Server::Ijob.StateChangedListener;

public class IdleController extends StateController {
    private static const String TAG = "IdleController";

    // Policy: we decide that we're "idle" if the device has been unused /
    // screen off or dreaming for at least this Int64
    private static const Int64 INACTIVITY_IDLE_THRESHOLD = 71 * 60 * 1000; // millis; 71 min
    private static const Int64 IDLE_WINDOW_SLOP = 5 * 60 * 1000; // 5 minute window, to be nice

    private static const String ACTION_TRIGGER_IDLE =
            "com.android.server.task.controllers.IdleController.ACTION_TRIGGER_IDLE";

    final ArrayList<JobStatus> mTrackedTasks = new ArrayList<JobStatus>();
    IdlenessTracker mIdleTracker;

    // Singleton factory
    private static Object sCreationLock = new Object();
    private static volatile IdleController sController;

    public static IdleController Get(JobSchedulerService service) {
        {    AutoLock syncLock(sCreationLock);
            if (sController == NULL) {
                sController = new IdleController(service, service->GetContext());
            }
            return sController;
        }
    }

    private IdleController(StateChangedListener stateChangedListener, Context context) {
        Super(stateChangedListener, context);
        InitIdleStateTracking();
    }

    /**
     * StateController interface
     */
    //@Override
    CARAPI MaybeStartTrackingJob(JobStatus taskStatus) {
        if (taskStatus->HasIdleConstraint()) {
            {    AutoLock syncLock(mTrackedTasks);
                mTrackedTasks->Add(taskStatus);
                taskStatus.idleConstraintSatisfied->Set(mIdleTracker->IsIdle());
            }
        }
    }

    //@Override
    CARAPI MaybeStopTrackingJob(JobStatus taskStatus) {
        {    AutoLock syncLock(mTrackedTasks);
            mTrackedTasks->Remove(taskStatus);
        }
    }

    /**
     * Interaction with the task manager service
     */
    void ReportNewIdleState(Boolean isIdle) {
        {    AutoLock syncLock(mTrackedTasks);
            for (JobStatus task : mTrackedTasks) {
                task.idleConstraintSatisfied->Set(isIdle);
            }
        }
        mStateChangedListener->OnControllerStateChanged();
    }

    /**
     * Idle state tracking, and messaging with the task manager when
     * significant state changes occur
     */
    private void InitIdleStateTracking() {
        mIdleTracker = new IdlenessTracker();
        mIdleTracker->StartTracking();
    }

    class IdlenessTracker extends BroadcastReceiver {
        private AlarmManager mAlarm;
        private PendingIntent mIdleTriggerIntent;
        Boolean mIdle;

        public IdlenessTracker() {
            mAlarm = (AlarmManager) mContext->GetSystemService(Context.ALARM_SERVICE);

            Intent intent = new IIntent::ACTION_TRIGGER_IDLE)
                    .SetPackage("android")
                    .SetFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
            mIdleTriggerIntent = PendingIntent->GetBroadcast(mContext, 0, intent, 0);

            // At boot we presume that the user has just "interacted" with the
            // device in some meaningful way.
            mIdle = FALSE;
        }

        public Boolean IsIdle() {
            return mIdle;
        }

        CARAPI StartTracking() {
            IntentFilter filter = new IntentFilter();

            // Screen state
            filter->AddAction(IIntent::ACTION_SCREEN_ON);
            filter->AddAction(IIntent::ACTION_SCREEN_OFF);

            // Dreaming state
            filter->AddAction(IIntent::ACTION_DREAMING_STARTED);
            filter->AddAction(IIntent::ACTION_DREAMING_STOPPED);

            // Debugging/instrumentation
            filter->AddAction(ACTION_TRIGGER_IDLE);

            mContext->RegisterReceiver(this, filter);
        }

        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            final String action = intent->GetAction();

            if (action->Equals(IIntent::ACTION_SCREEN_ON)
                    || action->Equals(IIntent::ACTION_DREAMING_STOPPED)) {
                // possible transition to not-idle
                if (mIdle) {
                    if (DEBUG) {
                        Slogger::V(TAG, "exiting idle : " + action);
                    }
                    mAlarm->Cancel(mIdleTriggerIntent);
                    mIdle = FALSE;
                    ReportNewIdleState(mIdle);
                }
            } else if (action->Equals(IIntent::ACTION_SCREEN_OFF)
                    || action->Equals(IIntent::ACTION_DREAMING_STARTED)) {
                // when the screen goes off or dreaming starts, we schedule the
                // alarm that will tell us when we have decided the device is
                // truly idle.
                final Int64 nowElapsed = SystemClock->ElapsedRealtime();
                final Int64 when = nowElapsed + INACTIVITY_IDLE_THRESHOLD;
                if (DEBUG) {
                    Slogger::V(TAG, "Scheduling idle : " + action + " now:" + nowElapsed + " when="
                            + when);
                }
                mAlarm->SetWindow(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                        when, IDLE_WINDOW_SLOP, mIdleTriggerIntent);
            } else if (action->Equals(ACTION_TRIGGER_IDLE)) {
                // idle time starts now
                if (!mIdle) {
                    if (DEBUG) {
                        Slogger::V(TAG, "Idle trigger fired @ " + SystemClock->ElapsedRealtime());
                    }
                    mIdle = TRUE;
                    ReportNewIdleState(mIdle);
                }
            }
        }
    }

    //@Override
    CARAPI DumpControllerState(PrintWriter pw) {
        {    AutoLock syncLock(mTrackedTasks);
            pw->Print("Idle: ");
            pw->Println(mIdleTracker->IsIdle() ? "TRUE" : "FALSE");
            pw->Println(mTrackedTasks->Size());
            for (Int32 i = 0; i < mTrackedTasks->Size(); i++) {
                final JobStatus js = mTrackedTasks->Get(i);
                pw->Print("  ");
                pw->Print(String->ValueOf(js->HashCode()).Substring(0, 3));
                pw->Println("..");
            }
        }
    }
}
