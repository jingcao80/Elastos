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

using Elastos::Droid::App::Job::IJobInfo;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IPersistableBundle;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Text::Iformat.DateUtils;

using Elastos::IO::IPrintWriter;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

/**
 * Uniquely identifies a job internally.
 * Created from the public {@link android.app.job.JobInfo} object when it lands on the scheduler.
 * Contains current state of the requirements of the job, as well as a function to evaluate
 * whether it's ready to run.
 * This object is shared among the various controllers - hence why the different fields are atomic.
 * This isn't strictly necessary because each controller is only interested in a specific field,
 * and the receivers that are listening for global state change will all run on the main looper,
 * but we don't enforce that so this is safer.
 * @hide
 */
public class JobStatus {
    public static const Int64 NO_LATEST_RUNTIME = Long.MAX_VALUE;
    public static const Int64 NO_EARLIEST_RUNTIME = 0L;

    final JobInfo job;
    /** Uid of the package requesting this job. */
    final Int32 uId;
    final String name;
    final String tag;

    // Constraints.
    final AtomicBoolean chargingConstraintSatisfied = new AtomicBoolean();
    final AtomicBoolean timeDelayConstraintSatisfied = new AtomicBoolean();
    final AtomicBoolean deadlineConstraintSatisfied = new AtomicBoolean();
    final AtomicBoolean idleConstraintSatisfied = new AtomicBoolean();
    final AtomicBoolean unmeteredConstraintSatisfied = new AtomicBoolean();
    final AtomicBoolean connectivityConstraintSatisfied = new AtomicBoolean();

    /**
     * Earliest point in the future at which this job will be eligible to run. A value of 0
     * indicates there is no delay constraint. See {@link #HasTimingDelayConstraint()}.
     */
    private Int64 earliestRunTimeElapsedMillis;
    /**
     * Latest point in the future at which this job must be run. A value of {@link Long#MAX_VALUE}
     * indicates there is no deadline constraint. See {@link #HasDeadlineConstraint()}.
     */
    private Int64 latestRunTimeElapsedMillis;
    /** How many times this job has failed, used to compute back-off. */
    private final Int32 numFailures;

    /** Provide a handle to the service that this job will be run on. */
    public Int32 GetServiceToken() {
        return uId;
    }

    private JobStatus(JobInfo job, Int32 uId, Int32 numFailures) {
        this.job = job;
        this.uId = uId;
        this.name = job->GetService()->FlattenToShortString();
        this.tag = "*job*/" + this.name;
        this.numFailures = numFailures;
    }

    /** Create a newly scheduled job. */
    public JobStatus(JobInfo job, Int32 uId) {
        This(job, uId, 0);

        final Int64 elapsedNow = SystemClock->ElapsedRealtime();

        if (job->IsPeriodic()) {
            earliestRunTimeElapsedMillis = elapsedNow;
            latestRunTimeElapsedMillis = elapsedNow + job->GetIntervalMillis();
        } else {
            earliestRunTimeElapsedMillis = job->HasEarlyConstraint() ?
                    elapsedNow + job->GetMinLatencyMillis() : NO_EARLIEST_RUNTIME;
            latestRunTimeElapsedMillis = job->HasLateConstraint() ?
                    elapsedNow + job->GetMaxExecutionDelayMillis() : NO_LATEST_RUNTIME;
        }
    }

    /**
     * Create a new JobStatus that was loaded from disk. We ignore the provided
     * {@link android.app.job.JobInfo} time criteria because we can load a persisted periodic job
     * from the {@link com.android.server.job.JobStore} and still want to respect its
     * wallclock runtime rather than resetting it on every boot.
     * We consider a freshly loaded job to no longer be in back-off.
     */
    public JobStatus(JobInfo job, Int32 uId, Int64 earliestRunTimeElapsedMillis,
                      Int64 latestRunTimeElapsedMillis) {
        This(job, uId, 0);

        this.earliestRunTimeElapsedMillis = earliestRunTimeElapsedMillis;
        this.latestRunTimeElapsedMillis = latestRunTimeElapsedMillis;
    }

    /** Create a new job to be rescheduled with the provided parameters. */
    public JobStatus(JobStatus rescheduling, Int64 newEarliestRuntimeElapsedMillis,
                      Int64 newLatestRuntimeElapsedMillis, Int32 backoffAttempt) {
        This(rescheduling.job, rescheduling->GetUid(), backoffAttempt);

        earliestRunTimeElapsedMillis = newEarliestRuntimeElapsedMillis;
        latestRunTimeElapsedMillis = newLatestRuntimeElapsedMillis;
    }

    public JobInfo GetJob() {
        return job;
    }

    public Int32 GetJobId() {
        return job->GetId();
    }

    public Int32 GetNumFailures() {
        return numFailures;
    }

    public ComponentName GetServiceComponent() {
        return job->GetService();
    }

    public Int32 GetUserId() {
        return UserHandle->GetUserId(uId);
    }

    public Int32 GetUid() {
        return uId;
    }

    public String GetName() {
        return name;
    }

    public String GetTag() {
        return tag;
    }

    public PersistableBundle GetExtras() {
        return job->GetExtras();
    }

    public Boolean HasConnectivityConstraint() {
        return job->GetNetworkType() == JobInfo.NETWORK_TYPE_ANY;
    }

    public Boolean HasUnmeteredConstraint() {
        return job->GetNetworkType() == JobInfo.NETWORK_TYPE_UNMETERED;
    }

    public Boolean HasChargingConstraint() {
        return job->IsRequireCharging();
    }

    public Boolean HasTimingDelayConstraint() {
        return earliestRunTimeElapsedMillis != NO_EARLIEST_RUNTIME;
    }

    public Boolean HasDeadlineConstraint() {
        return latestRunTimeElapsedMillis != NO_LATEST_RUNTIME;
    }

    public Boolean HasIdleConstraint() {
        return job->IsRequireDeviceIdle();
    }

    public Boolean IsPersisted() {
        return job->IsPersisted();
    }

    public Int64 GetEarliestRunTime() {
        return earliestRunTimeElapsedMillis;
    }

    public Int64 GetLatestRunTimeElapsed() {
        return latestRunTimeElapsedMillis;
    }

    /**
     * @return Whether or not this job is ready to run, based on its requirements. This is TRUE if
     * the constraints are satisfied <strong>or</strong> the deadline on the job has expired.
     */
    public synchronized Boolean IsReady() {
        return IsConstraintsSatisfied()
                || (HasDeadlineConstraint() && deadlineConstraintSatisfied->Get());
    }

    /**
     * @return Whether the constraints set on this job are satisfied.
     */
    public synchronized Boolean IsConstraintsSatisfied() {
        return (!HasChargingConstraint() || chargingConstraintSatisfied->Get())
                && (!HasTimingDelayConstraint() || timeDelayConstraintSatisfied->Get())
                && (!HasConnectivityConstraint() || connectivityConstraintSatisfied->Get())
                && (!HasUnmeteredConstraint() || unmeteredConstraintSatisfied->Get())
                && (!HasIdleConstraint() || idleConstraintSatisfied->Get());
    }

    public Boolean Matches(Int32 uid, Int32 jobId) {
        return this.job->GetId() == jobId && this.uId == uid;
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return String->ValueOf(HashCode()).Substring(0, 3) + ".."
                + ":[" + job->GetService()
                + ",jId=" + job->GetId()
                + ",u" + GetUserId()
                + ",R=(" + FormatRunTime(earliestRunTimeElapsedMillis, NO_EARLIEST_RUNTIME)
                + "," + FormatRunTime(latestRunTimeElapsedMillis, NO_LATEST_RUNTIME) + ")"
                + ",N=" + job->GetNetworkType() + ",C=" + job->IsRequireCharging()
                + ",I=" + job->IsRequireDeviceIdle() + ",F=" + numFailures
                + ",P=" + job->IsPersisted()
                + (IsReady() ? "(READY)" : "")
                + "]";
    }

    private String FormatRunTime(Int64 runtime, Int64  defaultValue) {
        if (runtime == defaultValue) {
            return "none";
        } else {
            Int64 elapsedNow = SystemClock->ElapsedRealtime();
            Int64 nextRuntime = runtime - elapsedNow;
            if (nextRuntime > 0) {
                return DateUtils->FormatElapsedTime(nextRuntime / 1000);
            } else {
                return "-" + DateUtils->FormatElapsedTime(nextRuntime / -1000);
            }
        }
    }

    /**
     * Convenience function to identify a job uniquely without pulling all the data that
     * {@link #ToString()} returns.
     */
    public String ToShortString() {
        return job->GetService()->FlattenToShortString() + " jId=" + job->GetId() +
                ", u" + GetUserId();
    }

    // Dumpsys infrastructure
    CARAPI Dump(PrintWriter pw, String prefix) {
        pw->Print(prefix);
        pw->Println(this->ToString());
    }
}
