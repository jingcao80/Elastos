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

package com.android.server.job;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

using Elastos::Droid::App::IAppGlobals;
using Elastos::Droid::App::Job::IJobInfo;
using Elastos::Droid::App::Job::IJobScheduler;
using Elastos::Droid::App::Job::IJobService;
using Elastos::Droid::App::Job::IIJobScheduler;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::ISparseArray;

using Elastos::Droid::internal.app.IBatteryStats;
using Elastos::Droid::Server::Ijob.controllers.BatteryController;
using Elastos::Droid::Server::Ijob.controllers.ConnectivityController;
using Elastos::Droid::Server::Ijob.controllers.IdleController;
using Elastos::Droid::Server::Ijob.controllers.JobStatus;
using Elastos::Droid::Server::Ijob.controllers.StateController;
using Elastos::Droid::Server::Ijob.controllers.TimeController;

/**
 * Responsible for taking jobs representing work to be performed by a client app, and determining
 * based on the criteria specified when that job should be run against the client application's
 * endpoint.
 * Implements logic for scheduling, and rescheduling jobs. The JobSchedulerService knows nothing
 * about constraints, or the state of active jobs. It receives callbacks from the various
 * controllers and completed jobs and operates accordingly.
 *
 * Note on locking: Any operations that manipulate {@link #mJobs} need to lock on that object.
 * Any function with the suffix 'Locked' also needs to lock on {@link #mJobs}.
 * @hide
 */
public class JobSchedulerService extends com.android.server.SystemService
        implements StateChangedListener, JobCompletedListener {
    static const Boolean DEBUG = FALSE;
    /** The number of concurrent jobs we run at one time. */
    private static const Int32 MAX_JOB_CONTEXTS_COUNT = 3;
    static const String TAG = "JobSchedulerService";
    /** Master list of jobs. */
    final JobStore mJobs;

    static const Int32 MSG_JOB_EXPIRED = 0;
    static const Int32 MSG_CHECK_JOB = 1;

    // Policy constants
    /**
     * Minimum # of idle jobs that must be ready in order to force the JMS to schedule things
     * early.
     */
    static const Int32 MIN_IDLE_COUNT = 1;
    /**
     * Minimum # of charging jobs that must be ready in order to force the JMS to schedule things
     * early.
     */
    static const Int32 MIN_CHARGING_COUNT = 1;
    /**
     * Minimum # of connectivity jobs that must be ready in order to force the JMS to schedule
     * things early.
     */
    static const Int32 MIN_CONNECTIVITY_COUNT = 2;
    /**
     * Minimum # of jobs (with no particular constraints) for which the JMS will be happy running
     * some work early.
     * This is correlated with the amount of batching we'll be able to do.
     */
    static const Int32 MIN_READY_JOBS_COUNT = 2;

    /**
     * Track Services that have currently active or pending jobs. The index is provided by
     * {@link JobStatus#GetServiceToken()}
     */
    final List<JobServiceContext> mActiveServices = new ArrayList<JobServiceContext>();
    /** List of controllers that will notify this service of updates to jobs. */
    List<StateController> mControllers;
    /**
     * Queue of pending jobs. The JobServiceContext class will receive jobs from this list
     * when ready to execute them.
     */
    final ArrayList<JobStatus> mPendingJobs = new ArrayList<JobStatus>();

    final ArrayList<Integer> mStartedUsers = new ArrayList();

    final JobHandler mHandler;
    final JobSchedulerStub mJobSchedulerStub;

    IBatteryStats mBatteryStats;

    /**
     * Set to TRUE once we are allowed to run third party apps.
     */
    Boolean mReadyToRock;

    /**
     * Cleans up outstanding jobs when a package is removed. Even if it's being replaced later we
     * still clean up. On reinstall the package will have a new uid.
     */
    private final BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            Slogger::D(TAG, "Receieved: " + intent->GetAction());
            if (IIntent::ACTION_PACKAGE_REMOVED->Equals(intent->GetAction())) {
                Int32 uidRemoved = intent->GetIntExtra(Intent.EXTRA_UID, -1);
                if (DEBUG) {
                    Slogger::D(TAG, "Removing jobs for uid: " + uidRemoved);
                }
                CancelJobsForUid(uidRemoved);
            } else if (IIntent::ACTION_USER_REMOVED->Equals(intent->GetAction())) {
                final Int32 userId = intent->GetIntExtra(Intent.EXTRA_USER_HANDLE, 0);
                if (DEBUG) {
                    Slogger::D(TAG, "Removing jobs for user: " + userId);
                }
                CancelJobsForUser(userId);
            }
        }
    };

    //@Override
    CARAPI OnStartUser(Int32 userHandle) {
        mStartedUsers->Add(userHandle);
        // Let's kick any outstanding jobs for this user.
        mHandler->ObtainMessage(MSG_CHECK_JOB).SendToTarget();
    }

    //@Override
    CARAPI OnStopUser(Int32 userHandle) {
        mStartedUsers->Remove(Integer->ValueOf(userHandle));
    }

    /**
     * Entry point from client to schedule the provided job.
     * This cancels the job if it's already been scheduled, and replaces it with the one provided.
     * @param job JobInfo object containing execution parameters
     * @param uId The package identifier of the application this job is for.
     * @return Result of this operation. See <code>JobScheduler#RESULT_*</code> return codes.
     */
    public Int32 Schedule(JobInfo job, Int32 uId) {
        JobStatus jobStatus = new JobStatus(job, uId);
        CancelJob(uId, job->GetId());
        StartTrackingJob(jobStatus);
        mHandler->ObtainMessage(MSG_CHECK_JOB).SendToTarget();
        return JobScheduler.RESULT_SUCCESS;
    }

    public List<JobInfo> GetPendingJobs(Int32 uid) {
        ArrayList<JobInfo> outList = new ArrayList<JobInfo>();
        {    AutoLock syncLock(mJobs);
            ArraySet<JobStatus> jobs = mJobs->GetJobs();
            for (Int32 i=0; i<jobs->Size(); i++) {
                JobStatus job = jobs->ValueAt(i);
                if (job->GetUid() == uid) {
                    outList->Add(job->GetJob());
                }
            }
        }
        return outList;
    }

    private void CancelJobsForUser(Int32 userHandle) {
        List<JobStatus> jobsForUser;
        {    AutoLock syncLock(mJobs);
            jobsForUser = mJobs->GetJobsByUser(userHandle);
        }
        for (Int32 i=0; i<jobsForUser->Size(); i++) {
            JobStatus toRemove = jobsForUser->Get(i);
            CancelJobImpl(toRemove);
        }
    }

    /**
     * Entry point from client to cancel all jobs originating from their uid.
     * This will remove the job from the master list, and cancel the job if it was staged for
     * execution or being executed.
     * @param uid Uid to check against for removal of a job.
     */
    CARAPI CancelJobsForUid(Int32 uid) {
        List<JobStatus> jobsForUid;
        {    AutoLock syncLock(mJobs);
            jobsForUid = mJobs->GetJobsByUid(uid);
        }
        for (Int32 i=0; i<jobsForUid->Size(); i++) {
            JobStatus toRemove = jobsForUid->Get(i);
            CancelJobImpl(toRemove);
        }
    }

    /**
     * Entry point from client to cancel the job corresponding to the jobId provided.
     * This will remove the job from the master list, and cancel the job if it was staged for
     * execution or being executed.
     * @param uid Uid of the calling client.
     * @param jobId Id of the job, provided at schedule-time.
     */
    CARAPI CancelJob(Int32 uid, Int32 jobId) {
        JobStatus toCancel;
        {    AutoLock syncLock(mJobs);
            toCancel = mJobs->GetJobByUidAndJobId(uid, jobId);
        }
        if (toCancel != NULL) {
            CancelJobImpl(toCancel);
        }
    }

    private void CancelJobImpl(JobStatus cancelled) {
        if (DEBUG) {
            Slogger::D(TAG, "Cancelling: " + cancelled);
        }
        StopTrackingJob(cancelled);
        {    AutoLock syncLock(mJobs);
            // Remove from pending queue.
            mPendingJobs->Remove(cancelled);
            // Cancel if running.
            StopJobOnServiceContextLocked(cancelled);
        }
    }

    /**
     * Initializes the system service.
     * <p>
     * Subclasses must define a single argument constructor that accepts the context
     * and passes it to super.
     * </p>
     *
     * @param context The system server context.
     */
    public JobSchedulerService(Context context) {
        Super(context);
        // Create the controllers.
        mControllers = new ArrayList<StateController>();
        mControllers->Add(ConnectivityController->Get(this));
        mControllers->Add(TimeController->Get(this));
        mControllers->Add(IdleController->Get(this));
        mControllers->Add(BatteryController->Get(this));

        mHandler = new JobHandler(context->GetMainLooper());
        mJobSchedulerStub = new JobSchedulerStub();
        mJobs = JobStore->InitAndGet(this);
    }

    //@Override
    CARAPI OnStart() {
        PublishBinderService(Context.JOB_SCHEDULER_SERVICE, mJobSchedulerStub);
    }

    //@Override
    CARAPI OnBootPhase(Int32 phase) {
        if (PHASE_SYSTEM_SERVICES_READY == phase) {
            // Register br for package removals and user removals.
            final IntentFilter filter = new IntentFilter(IIntent::ACTION_PACKAGE_REMOVED);
            filter->AddDataScheme("package");
            GetContext()->RegisterReceiverAsUser(
                    mBroadcastReceiver, UserHandle.ALL, filter, NULL, NULL);
            final IntentFilter userFilter = new IntentFilter(IIntent::ACTION_USER_REMOVED);
            GetContext()->RegisterReceiverAsUser(
                    mBroadcastReceiver, UserHandle.ALL, userFilter, NULL, NULL);
        } else if (phase == PHASE_THIRD_PARTY_APPS_CAN_START) {
            {    AutoLock syncLock(mJobs);
                // Let's go!
                mReadyToRock = TRUE;
                mBatteryStats = IBatteryStats.Stub->AsInterface(ServiceManager->GetService(
                        BatteryStats.SERVICE_NAME));
                // Create the "runners".
                for (Int32 i = 0; i < MAX_JOB_CONTEXTS_COUNT; i++) {
                    mActiveServices->Add(
                            new JobServiceContext(this, mBatteryStats,
                                    GetContext()->GetMainLooper()));
                }
                // Attach jobs to their controllers.
                ArraySet<JobStatus> jobs = mJobs->GetJobs();
                for (Int32 i=0; i<jobs->Size(); i++) {
                    JobStatus job = jobs->ValueAt(i);
                    for (Int32 controller=0; controller<mControllers->Size(); controller++) {
                        mControllers->Get(controller).MaybeStartTrackingJob(job);
                    }
                }
                // GO GO GO!
                mHandler->ObtainMessage(MSG_CHECK_JOB).SendToTarget();
            }
        }
    }

    /**
     * Called when we have a job status object that we need to insert in our
     * {@link com.android.server.job.JobStore}, and make sure all the relevant controllers know
     * about.
     */
    private void StartTrackingJob(JobStatus jobStatus) {
        Boolean update;
        Boolean rocking;
        {    AutoLock syncLock(mJobs);
            update = mJobs->Add(jobStatus);
            rocking = mReadyToRock;
        }
        if (rocking) {
            for (Int32 i=0; i<mControllers->Size(); i++) {
                StateController controller = mControllers->Get(i);
                if (update) {
                    controller->MaybeStopTrackingJob(jobStatus);
                }
                controller->MaybeStartTrackingJob(jobStatus);
            }
        }
    }

    /**
     * Called when we want to remove a JobStatus object that we've finished executing. Returns the
     * object removed.
     */
    private Boolean StopTrackingJob(JobStatus jobStatus) {
        Boolean removed;
        Boolean rocking;
        {    AutoLock syncLock(mJobs);
            // Remove from store as well as controllers.
            removed = mJobs->Remove(jobStatus);
            rocking = mReadyToRock;
        }
        if (removed && rocking) {
            for (Int32 i=0; i<mControllers->Size(); i++) {
                StateController controller = mControllers->Get(i);
                controller->MaybeStopTrackingJob(jobStatus);
            }
        }
        return removed;
    }

    private Boolean StopJobOnServiceContextLocked(JobStatus job) {
        for (Int32 i=0; i<mActiveServices->Size(); i++) {
            JobServiceContext jsc = mActiveServices->Get(i);
            final JobStatus executing = jsc->GetRunningJob();
            if (executing != NULL && executing->Matches(job->GetUid(), job->GetJobId())) {
                jsc->CancelExecutingJob();
                return TRUE;
            }
        }
        return FALSE;
    }

    /**
     * @param job JobStatus we are querying against.
     * @return Whether or not the job represented by the status object is currently being run or
     * is pending.
     */
    private Boolean IsCurrentlyActiveLocked(JobStatus job) {
        for (Int32 i=0; i<mActiveServices->Size(); i++) {
            JobServiceContext serviceContext = mActiveServices->Get(i);
            final JobStatus running = serviceContext->GetRunningJob();
            if (running != NULL && running->Matches(job->GetUid(), job->GetJobId())) {
                return TRUE;
            }
        }
        return FALSE;
    }

    /**
     * A job is rescheduled with exponential back-off if the client requests this from their
     * execution logic.
     * A caveat is for idle-mode jobs, for which the idle-mode constraint will usurp the
     * timeliness of the reschedule. For an idle-mode job, no deadline is given.
     * @param failureToReschedule Provided job status that we will reschedule.
     * @return A newly instantiated JobStatus with the same constraints as the last job except
     * with adjusted timing constraints.
     */
    private JobStatus GetRescheduleJobForFailure(JobStatus failureToReschedule) {
        final Int64 elapsedNowMillis = SystemClock->ElapsedRealtime();
        final JobInfo job = failureToReschedule->GetJob();

        final Int64 initialBackoffMillis = job->GetInitialBackoffMillis();
        final Int32 backoffAttempts = failureToReschedule->GetNumFailures() + 1;
        Int64 delayMillis;

        switch (job->GetBackoffPolicy()) {
            case JobInfo.BACKOFF_POLICY_LINEAR:
                delayMillis = initialBackoffMillis * backoffAttempts;
                break;
            default:
                if (DEBUG) {
                    Slogger::V(TAG, "Unrecognised back-off policy, defaulting to exponential.");
                }
            case JobInfo.BACKOFF_POLICY_EXPONENTIAL:
                delayMillis =
                        (Int64) Math->Scalb(initialBackoffMillis, backoffAttempts - 1);
                break;
        }
        delayMillis =
                Math->Min(delayMillis, JobInfo.MAX_BACKOFF_DELAY_MILLIS);
        return new JobStatus(failureToReschedule, elapsedNowMillis + delayMillis,
                JobStatus.NO_LATEST_RUNTIME, backoffAttempts);
    }

    /**
     * Called after a periodic has executed so we can to re-add it. We take the last execution time
     * of the job to be the time of completion (i.e. the time at which this function is called).
     * This could be inaccurate b/c the job can run for as Int64 as
     * {@link com.android.server.job.JobServiceContext#EXECUTING_TIMESLICE_MILLIS}, but will lead
     * to underscheduling at least, rather than if we had taken the last execution time to be the
     * start of the execution.
     * @return A new job representing the execution criteria for this instantiation of the
     * recurring job.
     */
    private JobStatus GetRescheduleJobForPeriodic(JobStatus periodicToReschedule) {
        final Int64 elapsedNow = SystemClock->ElapsedRealtime();
        // Compute how much of the period is remaining.
        Int64 runEarly = Math->Max(periodicToReschedule->GetLatestRunTimeElapsed() - elapsedNow, 0);
        Int64 newEarliestRunTimeElapsed = elapsedNow + runEarly;
        Int64 period = periodicToReschedule->GetJob()->GetIntervalMillis();
        Int64 newLatestRuntimeElapsed = newEarliestRunTimeElapsed + period;

        if (DEBUG) {
            Slogger::V(TAG, "Rescheduling executed periodic. New execution window [" +
                    newEarliestRunTimeElapsed/1000 + ", " + newLatestRuntimeElapsed/1000 + "]s");
        }
        return new JobStatus(periodicToReschedule, newEarliestRunTimeElapsed,
                newLatestRuntimeElapsed, 0 /* backoffAttempt */);
    }

    // JobCompletedListener implementations.

    /**
     * A job just finished executing. We fetch the
     * {@link com.android.server.job.controllers.JobStatus} from the store and depending on
     * whether we want to reschedule we readd it to the controllers.
     * @param jobStatus Completed job.
     * @param needsReschedule Whether the implementing class should reschedule this job.
     */
    //@Override
    CARAPI OnJobCompleted(JobStatus jobStatus, Boolean needsReschedule) {
        if (DEBUG) {
            Slogger::D(TAG, "Completed " + jobStatus + ", reschedule=" + needsReschedule);
        }
        if (!StopTrackingJob(jobStatus)) {
            if (DEBUG) {
                Slogger::D(TAG, "Could not find job to remove. Was job removed while executing?");
            }
            return;
        }
        if (needsReschedule) {
            JobStatus rescheduled = GetRescheduleJobForFailure(jobStatus);
            StartTrackingJob(rescheduled);
        } else if (jobStatus->GetJob()->IsPeriodic()) {
            JobStatus rescheduledPeriodic = GetRescheduleJobForPeriodic(jobStatus);
            StartTrackingJob(rescheduledPeriodic);
        }
        mHandler->ObtainMessage(MSG_CHECK_JOB).SendToTarget();
    }

    // StateChangedListener implementations.

    /**
     * Posts a message to the {@link com.android.server.job.JobSchedulerService.JobHandler} that
     * some controller's state has changed, so as to run through the list of jobs and start/stop
     * any that are eligible.
     */
    //@Override
    CARAPI OnControllerStateChanged() {
        mHandler->ObtainMessage(MSG_CHECK_JOB).SendToTarget();
    }

    //@Override
    CARAPI OnRunJobNow(JobStatus jobStatus) {
        mHandler->ObtainMessage(MSG_JOB_EXPIRED, jobStatus).SendToTarget();
    }

    private class JobHandler extends Handler {

        public JobHandler(Looper looper) {
            Super(looper);
        }

        //@Override
        CARAPI HandleMessage(Message message) {
            {    AutoLock syncLock(mJobs);
                if (!mReadyToRock) {
                    return;
                }
            }
            switch (message.what) {
                case MSG_JOB_EXPIRED:
                    {    AutoLock syncLock(mJobs);
                        JobStatus runNow = (JobStatus) message.obj;
                        // runNow can be NULL, which is a controller's way of indicating that its
                        // state is such that all ready jobs should be run immediately.
                        if (runNow != NULL && !mPendingJobs->Contains(runNow)
                                && mJobs->ContainsJob(runNow)) {
                            mPendingJobs->Add(runNow);
                        }
                        QueueReadyJobsForExecutionLockedH();
                    }
                    break;
                case MSG_CHECK_JOB:
                    {    AutoLock syncLock(mJobs);
                        // Check the list of jobs and run some of them if we feel inclined.
                        MaybeQueueReadyJobsForExecutionLockedH();
                    }
                    break;
            }
            MaybeRunPendingJobsH();
            // Don't remove JOB_EXPIRED in case one came along while processing the queue.
            RemoveMessages(MSG_CHECK_JOB);
        }

        /**
         * Run through list of jobs and execute all possible - at least one is expired so we do
         * as many as we can.
         */
        private void QueueReadyJobsForExecutionLockedH() {
            ArraySet<JobStatus> jobs = mJobs->GetJobs();
            if (DEBUG) {
                Slogger::D(TAG, "queuing all ready jobs for execution:");
            }
            for (Int32 i=0; i<jobs->Size(); i++) {
                JobStatus job = jobs->ValueAt(i);
                if (IsReadyToBeExecutedLocked(job)) {
                    if (DEBUG) {
                        Slogger::D(TAG, "    queued " + job->ToShortString());
                    }
                    mPendingJobs->Add(job);
                } else if (IsReadyToBeCancelledLocked(job)) {
                    StopJobOnServiceContextLocked(job);
                }
            }
            if (DEBUG) {
                final Int32 queuedJobs = mPendingJobs->Size();
                if (queuedJobs == 0) {
                    Slogger::D(TAG, "No jobs pending.");
                } else {
                    Slogger::D(TAG, queuedJobs + " jobs queued.");
                }
            }
        }

        /**
         * The state of at least one job has changed. Here is where we could enforce various
         * policies on when we want to execute jobs.
         * Right now the policy is such:
         * If >1 of the ready jobs is idle mode we send all of them off
         * if more than 2 network connectivity jobs are ready we send them all off.
         * If more than 4 jobs total are ready we send them all off.
         * TODO: It would be nice to consolidate these sort of high-level policies somewhere.
         */
        private void MaybeQueueReadyJobsForExecutionLockedH() {
            Int32 chargingCount = 0;
            Int32 idleCount =  0;
            Int32 backoffCount = 0;
            Int32 connectivityCount = 0;
            List<JobStatus> runnableJobs = new ArrayList<JobStatus>();
            ArraySet<JobStatus> jobs = mJobs->GetJobs();
            for (Int32 i=0; i<jobs->Size(); i++) {
                JobStatus job = jobs->ValueAt(i);
                if (IsReadyToBeExecutedLocked(job)) {
                    if (job->GetNumFailures() > 0) {
                        backoffCount++;
                    }
                    if (job->HasIdleConstraint()) {
                        idleCount++;
                    }
                    if (job->HasConnectivityConstraint() || job->HasUnmeteredConstraint()) {
                        connectivityCount++;
                    }
                    if (job->HasChargingConstraint()) {
                        chargingCount++;
                    }
                    runnableJobs->Add(job);
                } else if (IsReadyToBeCancelledLocked(job)) {
                    StopJobOnServiceContextLocked(job);
                }
            }
            if (backoffCount > 0 ||
                    idleCount >= MIN_IDLE_COUNT ||
                    connectivityCount >= MIN_CONNECTIVITY_COUNT ||
                    chargingCount >= MIN_CHARGING_COUNT ||
                    runnableJobs->Size() >= MIN_READY_JOBS_COUNT) {
                if (DEBUG) {
                    Slogger::D(TAG, "maybeQueueReadyJobsForExecutionLockedH: Running jobs.");
                }
                for (Int32 i=0; i<runnableJobs->Size(); i++) {
                    mPendingJobs->Add(runnableJobs->Get(i));
                }
            } else {
                if (DEBUG) {
                    Slogger::D(TAG, "maybeQueueReadyJobsForExecutionLockedH: Not running anything.");
                }
            }
            if (DEBUG) {
                Slogger::D(TAG, "idle=" + idleCount + " connectivity=" +
                connectivityCount + " charging=" + chargingCount + " tot=" +
                        runnableJobs->Size());
            }
        }

        /**
         * Criteria for moving a job into the pending queue:
         *      - It's ready.
         *      - It's not pending.
         *      - It's not already running on a JSC.
         *      - The user that requested the job is running.
         */
        private Boolean IsReadyToBeExecutedLocked(JobStatus job) {
            final Boolean jobReady = job->IsReady();
            final Boolean jobPending = mPendingJobs->Contains(job);
            final Boolean jobActive = IsCurrentlyActiveLocked(job);
            final Boolean userRunning = mStartedUsers->Contains(job->GetUserId());

            if (DEBUG) {
                Slogger::V(TAG, "isReadyToBeExecutedLocked: " + job->ToShortString()
                        + " ready=" + jobReady + " pending=" + jobPending
                        + " active=" + jobActive + " userRunning=" + userRunning);
            }
            return userRunning && jobReady && !jobPending && !jobActive;
        }

        /**
         * Criteria for cancelling an active job:
         *      - It's not ready
         *      - It's running on a JSC.
         */
        private Boolean IsReadyToBeCancelledLocked(JobStatus job) {
            return !job->IsReady() && IsCurrentlyActiveLocked(job);
        }

        /**
         * Reconcile jobs in the pending queue against available execution contexts.
         * A controller can force a job into the pending queue even if it's already running, but
         * here is where we decide whether to actually execute it.
         */
        private void MaybeRunPendingJobsH() {
            {    AutoLock syncLock(mJobs);
                Iterator<JobStatus> it = mPendingJobs->Iterator();
                if (DEBUG) {
                    Slogger::D(TAG, "pending queue: " + mPendingJobs->Size() + " jobs.");
                }
                while (it->HasNext()) {
                    JobStatus nextPending = it->Next();
                    JobServiceContext availableContext = NULL;
                    for (Int32 i=0; i<mActiveServices->Size(); i++) {
                        JobServiceContext jsc = mActiveServices->Get(i);
                        final JobStatus running = jsc->GetRunningJob();
                        if (running != NULL && running->Matches(nextPending->GetUid(),
                                nextPending->GetJobId())) {
                            // Already running this job for this uId, skip.
                            availableContext = NULL;
                            break;
                        }
                        if (jsc->IsAvailable()) {
                            availableContext = jsc;
                        }
                    }
                    if (availableContext != NULL) {
                        if (!availableContext->ExecuteRunnableJob(nextPending)) {
                            if (DEBUG) {
                                Slogger::D(TAG, "Error executing " + nextPending);
                            }
                            mJobs->Remove(nextPending);
                        }
                        it->Remove();
                    }
                }
            }
        }
    }

    /**
     * Binder stub trampoline implementation
     */
    final class JobSchedulerStub extends IJobScheduler.Stub {
        /** Cache determination of whether a given app can persist jobs
         * key is uid of the calling app; value is undetermined/TRUE/FALSE
         */
        private final SparseArray<Boolean> mPersistCache = new SparseArray<Boolean>();

        // Enforce that only the app itself (or shared uid participant) can schedule a
        // job that runs one of the app's services, as well as verifying that the
        // named service properly requires the BIND_JOB_SERVICE permission
        private void EnforceValidJobRequest(Int32 uid, JobInfo job) {
            final IPackageManager pm = AppGlobals->GetPackageManager();
            final ComponentName service = job->GetService();
            try {
                ServiceInfo si = pm->GetServiceInfo(service, 0, UserHandle->GetUserId(uid));
                if (si == NULL) {
                    throw new IllegalArgumentException("No such service " + service);
                }
                if (si.applicationInfo.uid != uid) {
                    throw new IllegalArgumentException("uid " + uid +
                            " cannot schedule job in " + service->GetPackageName());
                }
                if (!JobService.PERMISSION_BIND->Equals(si.permission)) {
                    throw new IllegalArgumentException("Scheduled service " + service
                            + " does not require android.permission.BIND_JOB_SERVICE permission");
                }
            } catch (RemoteException e) {
                // Can't happen; the Package Manager is in this same process
            }
        }

        private Boolean CanPersistJobs(Int32 pid, Int32 uid) {
            // If we get this far we're good to go; all we need to do now is check
            // whether the app is allowed to persist its scheduled work.
            final Boolean canPersist;
            {    AutoLock syncLock(mPersistCache);
                Boolean cached = mPersistCache->Get(uid);
                if (cached != NULL) {
                    canPersist = cached->BooleanValue();
                } else {
                    // Persisting jobs is tantamount to running at boot, so we permit
                    // it when the app has declared that it uses the RECEIVE_BOOT_COMPLETED
                    // permission
                    Int32 result = GetContext()->CheckPermission(
                            Manifest::permission::RECEIVE_BOOT_COMPLETED, pid, uid);
                    canPersist = (result == PackageManager.PERMISSION_GRANTED);
                    mPersistCache->Put(uid, canPersist);
                }
            }
            return canPersist;
        }

        // IJobScheduler implementation
        //@Override
        public Int32 Schedule(JobInfo job) throws RemoteException {
            if (DEBUG) {
                Slogger::D(TAG, "Scheduling job: " + job->ToString());
            }
            final Int32 pid = Binder->GetCallingPid();
            final Int32 uid = Binder->GetCallingUid();

            EnforceValidJobRequest(uid, job);
            if (job->IsPersisted()) {
                if (!CanPersistJobs(pid, uid)) {
                    throw new IllegalArgumentException("Error: requested job be persisted without"
                            + " holding RECEIVE_BOOT_COMPLETED permission.");
                }
            }

            Int64 ident = Binder->ClearCallingIdentity();
            try {
                return JobSchedulerService.this->Schedule(job, uid);
            } finally {
                Binder->RestoreCallingIdentity(ident);
            }
        }

        //@Override
        public List<JobInfo> GetAllPendingJobs() throws RemoteException {
            final Int32 uid = Binder->GetCallingUid();

            Int64 ident = Binder->ClearCallingIdentity();
            try {
                return JobSchedulerService.this->GetPendingJobs(uid);
            } finally {
                Binder->RestoreCallingIdentity(ident);
            }
        }

        //@Override
        CARAPI CancelAll() throws RemoteException {
            final Int32 uid = Binder->GetCallingUid();

            Int64 ident = Binder->ClearCallingIdentity();
            try {
                JobSchedulerService.this->CancelJobsForUid(uid);
            } finally {
                Binder->RestoreCallingIdentity(ident);
            }
        }

        //@Override
        CARAPI Cancel(Int32 jobId) throws RemoteException {
            final Int32 uid = Binder->GetCallingUid();

            Int64 ident = Binder->ClearCallingIdentity();
            try {
                JobSchedulerService.this->CancelJob(uid, jobId);
            } finally {
                Binder->RestoreCallingIdentity(ident);
            }
        }

        /**
         * "dumpsys" infrastructure
         */
        //@Override
        CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
            GetContext()->EnforceCallingOrSelfPermission(Manifest::permission::DUMP, TAG);

            Int64 identityToken = Binder->ClearCallingIdentity();
            try {
                JobSchedulerService.this->DumpInternal(pw);
            } finally {
                Binder->RestoreCallingIdentity(identityToken);
            }
        }
    };

    void DumpInternal(PrintWriter pw) {
        final Int64 now = SystemClock->ElapsedRealtime();
        {    AutoLock syncLock(mJobs);
            pw->Print("Started users: ");
            for (Int32 i=0; i<mStartedUsers->Size(); i++) {
                pw->Print("u" + mStartedUsers->Get(i) + " ");
            }
            pw->Println();
            pw->Println("Registered jobs:");
            if (mJobs->Size() > 0) {
                ArraySet<JobStatus> jobs = mJobs->GetJobs();
                for (Int32 i=0; i<jobs->Size(); i++) {
                    JobStatus job = jobs->ValueAt(i);
                    job->Dump(pw, "  ");
                }
            } else {
                pw->Println("  None.");
            }
            for (Int32 i=0; i<mControllers->Size(); i++) {
                pw->Println();
                mControllers->Get(i).DumpControllerState(pw);
            }
            pw->Println();
            pw->Println("Pending:");
            for (Int32 i=0; i<mPendingJobs->Size(); i++) {
                pw->Println(mPendingJobs->Get(i).HashCode());
            }
            pw->Println();
            pw->Println("Active jobs:");
            for (Int32 i=0; i<mActiveServices->Size(); i++) {
                JobServiceContext jsc = mActiveServices->Get(i);
                if (jsc->IsAvailable()) {
                    continue;
                } else {
                    final Int64 timeout = jsc->GetTimeoutElapsed();
                    pw->Print("Running for: ");
                    pw->Print((now - jsc->GetExecutionStartTimeElapsed())/1000);
                    pw->Print("s timeout=");
                    pw->Print(timeout);
                    pw->Print(" fromnow=");
                    pw->Println(timeout-now);
                    jsc->GetRunningJob()->Dump(pw, "  ");
                }
            }
            pw->Println();
            pw->Print("mReadyToRock="); pw->Println(mReadyToRock);
        }
        pw->Println();
    }
}
