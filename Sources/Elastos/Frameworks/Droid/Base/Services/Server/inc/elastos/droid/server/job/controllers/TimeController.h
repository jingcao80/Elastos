//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::Server::Ijob.JobSchedulerService;
using Elastos::Droid::Server::Ijob.StateChangedListener;

using Elastos::IO::IPrintWriter;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::IList;
using Elastos::Utility::IListIterator;

/**
 * This class sets an alarm for the next expiring job, and determines whether a job's minimum
 * delay has been satisfied.
 */
public class TimeController extends StateController {
    private static const String TAG = "JobScheduler.Time";
    private static const String ACTION_JOB_EXPIRED =
            "android.content.jobscheduler.JOB_DEADLINE_EXPIRED";
    private static const String ACTION_JOB_DELAY_EXPIRED =
            "android.content.jobscheduler.JOB_DELAY_EXPIRED";

    /** Set an alarm for the next job expiry. */
    private final PendingIntent mDeadlineExpiredAlarmIntent;
    /** Set an alarm for the next job delay expiry. This*/
    private final PendingIntent mNextDelayExpiredAlarmIntent;

    private Int64 mNextJobExpiredElapsedMillis;
    private Int64 mNextDelayExpiredElapsedMillis;

    private AlarmManager mAlarmService = NULL;
    /** List of tracked jobs, sorted asc. by deadline */
    private final List<JobStatus> mTrackedJobs = new LinkedList<JobStatus>();
    /** Singleton. */
    private static TimeController mSingleton;

    public static synchronized TimeController Get(JobSchedulerService jms) {
        if (mSingleton == NULL) {
            mSingleton = new TimeController(jms, jms->GetContext());
        }
        return mSingleton;
    }

    private TimeController(StateChangedListener stateChangedListener, Context context) {
        Super(stateChangedListener, context);
        mDeadlineExpiredAlarmIntent =
                PendingIntent->GetBroadcast(mContext, 0 /* ignored */,
                        new IIntent::ACTION_JOB_EXPIRED), 0);
        mNextDelayExpiredAlarmIntent =
                PendingIntent->GetBroadcast(mContext, 0 /* ignored */,
                        new IIntent::ACTION_JOB_DELAY_EXPIRED), 0);
        mNextJobExpiredElapsedMillis = Long.MAX_VALUE;
        mNextDelayExpiredElapsedMillis = Long.MAX_VALUE;

        // Register BR for these intents.
        IntentFilter intentFilter = new IntentFilter(ACTION_JOB_EXPIRED);
        intentFilter->AddAction(ACTION_JOB_DELAY_EXPIRED);
        mContext->RegisterReceiver(mAlarmExpiredReceiver, intentFilter);
    }

    /**
     * Check if the job has a timing constraint, and if so determine where to insert it in our
     * list.
     */
    //@Override
    public synchronized void MaybeStartTrackingJob(JobStatus job) {
        if (job->HasTimingDelayConstraint() || job->HasDeadlineConstraint()) {
            MaybeStopTrackingJob(job);
            ListIterator<JobStatus> it = mTrackedJobs->ListIterator(mTrackedJobs->Size());
            while (it->HasPrevious()) {
                JobStatus ts = it->Previous();
                if (ts->GetLatestRunTimeElapsed() < job->GetLatestRunTimeElapsed()) {
                    // Insert
                    break;
                }
            }
            it->Add(job);
            MaybeUpdateAlarms(
                    job->HasTimingDelayConstraint() ? job->GetEarliestRunTime() : Long.MAX_VALUE,
                    job->HasDeadlineConstraint() ? job->GetLatestRunTimeElapsed() : Long.MAX_VALUE);
        }
    }

    /**
     * When we stop tracking a job, we only need to update our alarms if the job we're no longer
     * tracking was the one our alarms were based off of.
     * Really an == comparison should be enough, but why play with fate? We'll do <=.
     */
    //@Override
    public synchronized void MaybeStopTrackingJob(JobStatus job) {
        if (mTrackedJobs->Remove(job)) {
            CheckExpiredDelaysAndResetAlarm();
            CheckExpiredDeadlinesAndResetAlarm();
        }
    }

    /**
     * Determines whether this controller can stop tracking the given job.
     * The controller is no longer interested in a job once its time constraint is satisfied, and
     * the job's deadline is fulfilled - unlike other controllers a time constraint can't toggle
     * back and forth.
     */
    private Boolean CanStopTrackingJob(JobStatus job) {
        return (!job->HasTimingDelayConstraint() ||
                job.timeDelayConstraintSatisfied->Get()) &&
                (!job->HasDeadlineConstraint() ||
                        job.deadlineConstraintSatisfied->Get());
    }

    private void EnsureAlarmService() {
        if (mAlarmService == NULL) {
            mAlarmService = (AlarmManager) mContext->GetSystemService(Context.ALARM_SERVICE);
        }
    }

    /**
     * Checks list of jobs for ones that have an expired deadline, sending them to the JobScheduler
     * if so, removing them from this list, and updating the alarm for the next expiry time.
     */
    private synchronized void CheckExpiredDeadlinesAndResetAlarm() {
        Int64 nextExpiryTime = Long.MAX_VALUE;
        final Int64 nowElapsedMillis = SystemClock->ElapsedRealtime();

        Iterator<JobStatus> it = mTrackedJobs->Iterator();
        while (it->HasNext()) {
            JobStatus job = it->Next();
            if (!job->HasDeadlineConstraint()) {
                continue;
            }
            final Int64 jobDeadline = job->GetLatestRunTimeElapsed();

            if (jobDeadline <= nowElapsedMillis) {
                job.deadlineConstraintSatisfied->Set(TRUE);
                mStateChangedListener->OnRunJobNow(job);
                it->Remove();
            } else {  // Sorted by expiry time, so take the next one and stop.
                nextExpiryTime = jobDeadline;
                break;
            }
        }
        SetDeadlineExpiredAlarm(nextExpiryTime);
    }

    /**
     * Handles alarm that notifies us that a job's delay has expired. Iterates through the list of
     * tracked jobs and marks them as ready as appropriate.
     */
    private synchronized void CheckExpiredDelaysAndResetAlarm() {
        final Int64 nowElapsedMillis = SystemClock->ElapsedRealtime();
        Int64 nextDelayTime = Long.MAX_VALUE;
        Boolean ready = FALSE;
        Iterator<JobStatus> it = mTrackedJobs->Iterator();
        while (it->HasNext()) {
            final JobStatus job = it->Next();
            if (!job->HasTimingDelayConstraint()) {
                continue;
            }
            final Int64 jobDelayTime = job->GetEarliestRunTime();
            if (jobDelayTime <= nowElapsedMillis) {
                job.timeDelayConstraintSatisfied->Set(TRUE);
                if (CanStopTrackingJob(job)) {
                    it->Remove();
                }
                if (job->IsReady()) {
                    ready = TRUE;
                }
            } else {  // Keep going through list to get next delay time.
                if (nextDelayTime > jobDelayTime) {
                    nextDelayTime = jobDelayTime;
                }
            }
        }
        if (ready) {
            mStateChangedListener->OnControllerStateChanged();
        }
        SetDelayExpiredAlarm(nextDelayTime);
    }

    private void MaybeUpdateAlarms(Int64 delayExpiredElapsed, Int64 deadlineExpiredElapsed) {
        if (delayExpiredElapsed < mNextDelayExpiredElapsedMillis) {
            SetDelayExpiredAlarm(delayExpiredElapsed);
        }
        if (deadlineExpiredElapsed < mNextJobExpiredElapsedMillis) {
            SetDeadlineExpiredAlarm(deadlineExpiredElapsed);
        }
    }

    /**
     * Set an alarm with the {@link android.app.AlarmManager} for the next time at which a job's
     * delay will expire.
     * This alarm <b>will not</b> wake up the phone.
     */
    private void SetDelayExpiredAlarm(Int64 alarmTimeElapsedMillis) {
        alarmTimeElapsedMillis = MaybeAdjustAlarmTime(alarmTimeElapsedMillis);
        mNextDelayExpiredElapsedMillis = alarmTimeElapsedMillis;
        UpdateAlarmWithPendingIntent(mNextDelayExpiredAlarmIntent, mNextDelayExpiredElapsedMillis);
    }

    /**
     * Set an alarm with the {@link android.app.AlarmManager} for the next time at which a job's
     * deadline will expire.
     * This alarm <b>will</b> wake up the phone.
     */
    private void SetDeadlineExpiredAlarm(Int64 alarmTimeElapsedMillis) {
        alarmTimeElapsedMillis = MaybeAdjustAlarmTime(alarmTimeElapsedMillis);
        mNextJobExpiredElapsedMillis = alarmTimeElapsedMillis;
        UpdateAlarmWithPendingIntent(mDeadlineExpiredAlarmIntent, mNextJobExpiredElapsedMillis);
    }

    private Int64 MaybeAdjustAlarmTime(Int64 proposedAlarmTimeElapsedMillis) {
        final Int64 earliestWakeupTimeElapsed = SystemClock->ElapsedRealtime();
        if (proposedAlarmTimeElapsedMillis < earliestWakeupTimeElapsed) {
            return earliestWakeupTimeElapsed;
        }
        return proposedAlarmTimeElapsedMillis;
    }

    private void UpdateAlarmWithPendingIntent(PendingIntent pi, Int64 alarmTimeElapsed) {
        EnsureAlarmService();
        if (alarmTimeElapsed == Long.MAX_VALUE) {
            mAlarmService->Cancel(pi);
        } else {
            if (DEBUG) {
                Slogger::D(TAG, "Setting " + pi->GetIntent()->GetAction() + " for: " + alarmTimeElapsed);
            }
            mAlarmService->Set(AlarmManager.ELAPSED_REALTIME, alarmTimeElapsed, pi);
        }
    }

    private final BroadcastReceiver mAlarmExpiredReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            if (DEBUG) {
                Slogger::D(TAG, "Just received alarm: " + intent->GetAction());
            }
            // A job has just expired, so we run through the list of jobs that we have and
            // notify our StateChangedListener.
            if (ACTION_JOB_EXPIRED->Equals(intent->GetAction())) {
                CheckExpiredDeadlinesAndResetAlarm();
            } else if (ACTION_JOB_DELAY_EXPIRED->Equals(intent->GetAction())) {
                CheckExpiredDelaysAndResetAlarm();
            }
        }
    };

    //@Override
    CARAPI DumpControllerState(PrintWriter pw) {
        final Int64 nowElapsed = SystemClock->ElapsedRealtime();
        pw->Println("Alarms (" + SystemClock->ElapsedRealtime() + ")");
        pw->Println(
                "Next delay alarm in " + (mNextDelayExpiredElapsedMillis - nowElapsed)/1000 + "s");
        pw->Println("Next deadline alarm in " + (mNextJobExpiredElapsedMillis - nowElapsed)/1000
                + "s");
        pw->Println("Tracking:");
        for (JobStatus ts : mTrackedJobs) {
            pw->Println(String->ValueOf(ts->HashCode()).Substring(0, 3) + ".."
                    + ": (" + (ts->HasTimingDelayConstraint() ? ts->GetEarliestRunTime() : "N/A")
                    + ", " + (ts->HasDeadlineConstraint() ?ts->GetLatestRunTimeElapsed() : "N/A")
                    + ")");
        }
    }
}