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

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Os::IBatteryManagerInternal;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::internal.annotations.VisibleForTesting;
using Elastos::Droid::Server::ILocalServices;
using Elastos::Droid::Server::Ijob.JobSchedulerService;
using Elastos::Droid::Server::Ijob.StateChangedListener;

using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

/**
 * Simple controller that tracks whether the phone is charging or not. The phone is considered to
 * be charging when it's been plugged in for more than two minutes, and the system has broadcast
 * ACTION_BATTERY_OK.
 */
public class BatteryController extends StateController {
    private static const String TAG = "JobScheduler.Batt";

    private static const Object sCreationLock = new Object();
    private static volatile BatteryController sController;
    private static const String ACTION_CHARGING_STABLE =
            "com.android.server.task.controllers.BatteryController.ACTION_CHARGING_STABLE";
    /** Wait this Int64 after phone is plugged in before doing any work. */
    private static const Int64 STABLE_CHARGING_THRESHOLD_MILLIS = 2 * 60 * 1000; // 2 minutes.

    private List<JobStatus> mTrackedTasks = new ArrayList<JobStatus>();
    private ChargingTracker mChargeTracker;

    public static BatteryController Get(JobSchedulerService taskManagerService) {
        {    AutoLock syncLock(sCreationLock);
            if (sController == NULL) {
                sController = new BatteryController(taskManagerService,
                        taskManagerService->GetContext());
            }
        }
        return sController;
    }

    @VisibleForTesting
    public ChargingTracker GetTracker() {
        return mChargeTracker;
    }

    @VisibleForTesting
    public static BatteryController GetForTesting(StateChangedListener stateChangedListener,
                                           Context context) {
        return new BatteryController(stateChangedListener, context);
    }

    private BatteryController(StateChangedListener stateChangedListener, Context context) {
        Super(stateChangedListener, context);
        mChargeTracker = new ChargingTracker();
        mChargeTracker->StartTracking();
    }

    //@Override
    CARAPI MaybeStartTrackingJob(JobStatus taskStatus) {
        final Boolean isOnStablePower = mChargeTracker->IsOnStablePower();
        if (taskStatus->HasChargingConstraint()) {
            {    AutoLock syncLock(mTrackedTasks);
                mTrackedTasks->Add(taskStatus);
                taskStatus.chargingConstraintSatisfied->Set(isOnStablePower);
            }
        }
        if (isOnStablePower) {
            mChargeTracker->SetStableChargingAlarm();
        }
    }

    //@Override
    CARAPI MaybeStopTrackingJob(JobStatus taskStatus) {
        if (taskStatus->HasChargingConstraint()) {
            {    AutoLock syncLock(mTrackedTasks);
                mTrackedTasks->Remove(taskStatus);
            }
        }
    }

    private void MaybeReportNewChargingState() {
        final Boolean stablePower = mChargeTracker->IsOnStablePower();
        if (DEBUG) {
            Slogger::D(TAG, "maybeReportNewChargingState: " + stablePower);
        }
        Boolean reportChange = FALSE;
        {    AutoLock syncLock(mTrackedTasks);
            for (JobStatus ts : mTrackedTasks) {
                Boolean previous = ts.chargingConstraintSatisfied->GetAndSet(stablePower);
                if (previous != stablePower) {
                    reportChange = TRUE;
                }
            }
        }
        // Let the scheduler know that state has changed. This may or may not result in an
        // execution.
        if (reportChange) {
            mStateChangedListener->OnControllerStateChanged();
        }
        // Also tell the scheduler that any ready jobs should be flushed.
        if (stablePower) {
            mStateChangedListener->OnRunJobNow(NULL);
        }
    }

    public class ChargingTracker extends BroadcastReceiver {
        private final AlarmManager mAlarm;
        private final PendingIntent mStableChargingTriggerIntent;
        /**
         * Track whether we're "charging", where charging means that we're ready to commit to
         * doing work.
         */
        private Boolean mCharging;
        /** Keep track of whether the battery is charged enough that we want to do work. */
        private Boolean mBatteryHealthy;

        public ChargingTracker() {
            mAlarm = (AlarmManager) mContext->GetSystemService(Context.ALARM_SERVICE);
            Intent intent = new IIntent::ACTION_CHARGING_STABLE);
            mStableChargingTriggerIntent = PendingIntent->GetBroadcast(mContext, 0, intent, 0);
        }

        CARAPI StartTracking() {
            IntentFilter filter = new IntentFilter();

            // Battery health.
            filter->AddAction(IIntent::ACTION_BATTERY_LOW);
            filter->AddAction(IIntent::ACTION_BATTERY_OKAY);
            // Charging/not charging.
            filter->AddAction(IIntent::ACTION_POWER_CONNECTED);
            filter->AddAction(IIntent::ACTION_POWER_DISCONNECTED);
            // Charging stable.
            filter->AddAction(ACTION_CHARGING_STABLE);
            mContext->RegisterReceiver(this, filter);

            // Initialise tracker state.
            BatteryManagerInternal batteryManagerInternal =
                    LocalServices->GetService(BatteryManagerInternal.class);
            mBatteryHealthy = !batteryManagerInternal->GetBatteryLevelLow();
            mCharging = batteryManagerInternal->IsPowered(BatteryManager.BATTERY_PLUGGED_ANY);
        }

        Boolean IsOnStablePower() {
            return mCharging && mBatteryHealthy;
        }

        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            OnReceiveInternal(intent);
        }

        @VisibleForTesting
        CARAPI OnReceiveInternal(Intent intent) {
            final String action = intent->GetAction();
            if (IIntent::ACTION_BATTERY_LOW->Equals(action)) {
                if (DEBUG) {
                    Slogger::D(TAG, "Battery life too low to do work. @ "
                            + SystemClock->ElapsedRealtime());
                }
                // If we get this action, the battery is discharging => it isn't plugged in so
                // there's no work to cancel. We track this variable for the case where it is
                // charging, but hasn't been for Int64 enough to be healthy.
                mBatteryHealthy = FALSE;
            } else if (IIntent::ACTION_BATTERY_OKAY->Equals(action)) {
                if (DEBUG) {
                    Slogger::D(TAG, "Battery life healthy enough to do work. @ "
                            + SystemClock->ElapsedRealtime());
                }
                mBatteryHealthy = TRUE;
                MaybeReportNewChargingState();
            } else if (IIntent::ACTION_POWER_CONNECTED->Equals(action)) {
                if (DEBUG) {
                    Slogger::D(TAG, "Received charging intent, setting alarm for "
                            + STABLE_CHARGING_THRESHOLD_MILLIS);
                }
                // Set up an alarm for ACTION_CHARGING_STABLE - we don't want to kick off tasks
                // here if the user unplugs the phone immediately.
                SetStableChargingAlarm();
                mCharging = TRUE;
            } else if (IIntent::ACTION_POWER_DISCONNECTED->Equals(action)) {
                if (DEBUG) {
                    Slogger::D(TAG, "Disconnected from power, cancelling any set alarms.");
                }
                // If an alarm is set, breathe a sigh of relief and cancel it - crisis averted.
                mAlarm->Cancel(mStableChargingTriggerIntent);
                mCharging = FALSE;
                MaybeReportNewChargingState();
            }else if (ACTION_CHARGING_STABLE->Equals(action)) {
                // Here's where we actually do the notify for a task being ready.
                if (DEBUG) {
                    Slogger::D(TAG, "Stable charging fired @ " + SystemClock->ElapsedRealtime()
                            + " charging: " + mCharging);
                }
                if (mCharging) {  // Should never receive this intent if mCharging is FALSE.
                    MaybeReportNewChargingState();
                }
            }
        }

        void SetStableChargingAlarm() {
            final Int64 alarmTriggerElapsed =
                    SystemClock->ElapsedRealtime() + STABLE_CHARGING_THRESHOLD_MILLIS;
            if (DEBUG) {
                Slogger::D(TAG, "Setting stable alarm to go off in " +
                        (STABLE_CHARGING_THRESHOLD_MILLIS / 1000) + "s");
            }
            mAlarm->Set(AlarmManager.ELAPSED_REALTIME_WAKEUP, alarmTriggerElapsed,
                    mStableChargingTriggerIntent);
        }
    }

    //@Override
    CARAPI DumpControllerState(PrintWriter pw) {
        pw->Println("Batt.");
        pw->Println("Stable power: " + mChargeTracker->IsOnStablePower());
        {    AutoLock syncLock(mTrackedTasks);
            Iterator<JobStatus> it = mTrackedTasks->Iterator();
            if (it->HasNext()) {
                pw->Print(String->ValueOf(it->Next()->HashCode()));
            }
            while (it->HasNext()) {
                pw->Print("," + String->ValueOf(it->Next()->HashCode()));
            }
            pw->Println();
        }
    }
}
