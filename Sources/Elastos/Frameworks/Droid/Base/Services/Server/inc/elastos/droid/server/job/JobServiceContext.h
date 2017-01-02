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

package com.android.server.job;

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::Job::IJobParameters;
using Elastos::Droid::App::Job::IIJobCallback;
using Elastos::Droid::App::Job::IIJobService;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::internal.annotations.GuardedBy;
using Elastos::Droid::internal.annotations.VisibleForTesting;
using Elastos::Droid::internal.app.IBatteryStats;
using Elastos::Droid::Server::Ijob.controllers.JobStatus;

using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

/**
 * Handles client binding and lifecycle of a job. Jobs execute one at a time on an instance of this
 * class.
 *
 * There are two important interactions into this class from the
 * {@link com.android.server.job.JobSchedulerService}. To execute a job and to cancel a job.
 * - Execution of a new job is handled by the {@link #mAvailable}. This bit is flipped once when a
 * job lands, and again when it is complete.
 * - Cancelling is trickier, because there are also interactions from the client. It's possible
 * the {@link com.android.server.job.JobServiceContext.JobServiceHandler} tries to process a
 * {@link #MSG_CANCEL} after the client has already finished. This is handled by having
 * {@link com.android.server.job.JobServiceContext.JobServiceHandler#handleCancelH} check whether
 * the context is still valid.
 * To mitigate this, tearing down the context removes all messages from the handler, including any
 * tardy {@link #MSG_CANCEL}s. Additionally, we avoid sending duplicate OnStopJob()
 * calls to the client after they've specified JobFinished().
 *
 */
public class JobServiceContext extends IJobCallback.Stub implements ServiceConnection {
    private static const Boolean DEBUG = FALSE;
    private static const String TAG = "JobServiceContext";
    /** Define the maximum # of jobs allowed to run on a service at once. */
    private static const Int32 defaultMaxActiveJobsPerService =
            ActivityManager->IsLowRamDeviceStatic() ? 1 : 3;
    /** Amount of time a job is allowed to execute for before being considered timed-out. */
    private static const Int64 EXECUTING_TIMESLICE_MILLIS = 60 * 1000;
    /** Amount of time the JobScheduler will wait for a response from an app for a message. */
    private static const Int64 OP_TIMEOUT_MILLIS = 8 * 1000;

    private static const String[] VERB_STRINGS = {
            "VERB_BINDING", "VERB_STARTING", "VERB_EXECUTING", "VERB_STOPPING"
    };

    // States that a job occupies while interacting with the client.
    static const Int32 VERB_BINDING = 0;
    static const Int32 VERB_STARTING = 1;
    static const Int32 VERB_EXECUTING = 2;
    static const Int32 VERB_STOPPING = 3;

    // Messages that result from interactions with the client service.
    /** System timed out waiting for a response. */
    private static const Int32 MSG_TIMEOUT = 0;
    /** Received a callback from client. */
    private static const Int32 MSG_CALLBACK = 1;
    /** Run through list and start any ready jobs.*/
    private static const Int32 MSG_SERVICE_BOUND = 2;
    /** Cancel a job. */
    private static const Int32 MSG_CANCEL = 3;
    /** Shutdown the job. Used when the client crashes and we can't die gracefully.*/
    private static const Int32 MSG_SHUTDOWN_EXECUTION = 4;

    private final Handler mCallbackHandler;
    /** Make callbacks to {@link JobSchedulerService} to inform on job completion status. */
    private final JobCompletedListener mCompletedListener;
    /** Used for service binding, etc. */
    private final Context mContext;
    private final IBatteryStats mBatteryStats;
    private PowerManager.WakeLock mWakeLock;

    // Execution state.
    private JobParameters mParams;
    @VisibleForTesting
    Int32 mVerb;
    private AtomicBoolean mCancelled = new AtomicBoolean();

    /** All the information maintained about the job currently being executed. */
    private JobStatus mRunningJob;
    /** Binder to the client service. */
    IJobService service;

    private final Object mLock = new Object();
    /**
     * Whether this context is free. This is set to FALSE at the start of execution, and reset to
     * TRUE when execution is complete.
     */
    @GuardedBy("mLock")
    private Boolean mAvailable;
    /** Track start time. */
    private Int64 mExecutionStartTimeElapsed;
    /** Track when job will timeout. */
    private Int64 mTimeoutElapsed;

    JobServiceContext(JobSchedulerService service, IBatteryStats batteryStats, Looper looper) {
        This(service->GetContext(), batteryStats, service, looper);
    }

    @VisibleForTesting
    JobServiceContext(Context context, IBatteryStats batteryStats,
            JobCompletedListener completedListener, Looper looper) {
        mContext = context;
        mBatteryStats = batteryStats;
        mCallbackHandler = new JobServiceHandler(looper);
        mCompletedListener = completedListener;
        mAvailable = TRUE;
    }

    /**
     * Give a job to this context for execution. Callers must first check {@link #IsAvailable()}
     * to make sure this is a valid context.
     * @param job The status of the job that we are going to run.
     * @return True if the job is valid and is running. False if the job cannot be executed.
     */
    Boolean ExecuteRunnableJob(JobStatus job) {
        {    AutoLock syncLock(mLock);
            if (!mAvailable) {
                Slogger::E(TAG, "Starting new runnable but context is unavailable > Error.");
                return FALSE;
            }

            mRunningJob = job;
            mParams = new JobParameters(this, job->GetJobId(), job->GetExtras(),
                    !job->IsConstraintsSatisfied());
            mExecutionStartTimeElapsed = SystemClock->ElapsedRealtime();

            mVerb = VERB_BINDING;
            ScheduleOpTimeOut();
            final Intent intent = new Intent()->SetComponent(job->GetServiceComponent());
            Boolean binding = mContext->BindServiceAsUser(intent, this,
                    Context.BIND_AUTO_CREATE | Context.BIND_NOT_FOREGROUND,
                    new UserHandle(job->GetUserId()));
            if (!binding) {
                if (DEBUG) {
                    Slogger::D(TAG, job->GetServiceComponent()->GetShortClassName() + " unavailable.");
                }
                mRunningJob = NULL;
                mParams = NULL;
                mExecutionStartTimeElapsed = 0L;
                RemoveOpTimeOut();
                return FALSE;
            }
            try {
                mBatteryStats->NoteJobStart(job->GetName(), job->GetUid());
            } catch (RemoteException e) {
                // Whatever.
            }
            mAvailable = FALSE;
            return TRUE;
        }
    }

    /**
     * Used externally to query the running job. Will return NULL if there is no job running.
     * Be careful when using this function, at any moment it's possible that the job returned may
     * stop executing.
     */
    JobStatus GetRunningJob() {
        {    AutoLock syncLock(mLock);
            return mRunningJob;
        }
    }

    /** Called externally when a job that was scheduled for execution should be cancelled. */
    void CancelExecutingJob() {
        mCallbackHandler->ObtainMessage(MSG_CANCEL).SendToTarget();
    }

    /**
     * @return Whether this context is available to handle incoming work.
     */
    Boolean IsAvailable() {
        {    AutoLock syncLock(mLock);
            return mAvailable;
        }
    }

    Int64 GetExecutionStartTimeElapsed() {
        return mExecutionStartTimeElapsed;
    }

    Int64 GetTimeoutElapsed() {
        return mTimeoutElapsed;
    }

    //@Override
    CARAPI JobFinished(Int32 jobId, Boolean reschedule) {
        if (!VerifyCallingUid()) {
            return;
        }
        mCallbackHandler->ObtainMessage(MSG_CALLBACK, jobId, reschedule ? 1 : 0)
                .SendToTarget();
    }

    //@Override
    CARAPI AcknowledgeStopMessage(Int32 jobId, Boolean reschedule) {
        if (!VerifyCallingUid()) {
            return;
        }
        mCallbackHandler->ObtainMessage(MSG_CALLBACK, jobId, reschedule ? 1 : 0)
                .SendToTarget();
    }

    //@Override
    CARAPI AcknowledgeStartMessage(Int32 jobId, Boolean ongoing) {
        if (!VerifyCallingUid()) {
            return;
        }
        mCallbackHandler->ObtainMessage(MSG_CALLBACK, jobId, ongoing ? 1 : 0).SendToTarget();
    }

    /**
     * We acquire/release a wakelock on onServiceConnected/unbindService. This mirrors the work
     * we intend to send to the client - we stop sending work when the service is unbound so until
     * then we keep the wakelock.
     * @param name The concrete component name of the service that has been connected.
     * @param service The IBinder of the Service's communication channel,
     */
    //@Override
    CARAPI OnServiceConnected(ComponentName name, IBinder service) {
        if (!name->Equals(mRunningJob->GetServiceComponent())) {
            mCallbackHandler->ObtainMessage(MSG_SHUTDOWN_EXECUTION).SendToTarget();
            return;
        }
        this.service = IJobService.Stub->AsInterface(service);
        final PowerManager pm =
                (PowerManager) mContext->GetSystemService(Context.POWER_SERVICE);
        mWakeLock = pm->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, mRunningJob->GetTag());
        mWakeLock->SetWorkSource(new WorkSource(mRunningJob->GetUid()));
        mWakeLock->SetReferenceCounted(FALSE);
        mWakeLock->Acquire();
        mCallbackHandler->ObtainMessage(MSG_SERVICE_BOUND).SendToTarget();
    }

    /** If the client service crashes we reschedule this job and clean up. */
    //@Override
    CARAPI OnServiceDisconnected(ComponentName name) {
        mCallbackHandler->ObtainMessage(MSG_SHUTDOWN_EXECUTION).SendToTarget();
    }

    /**
     * This class is reused across different clients, and passes itself in as a callback. Check
     * whether the client exercising the callback is the client we expect.
     * @return True if the binder calling is coming from the client we expect.
     */
    private Boolean VerifyCallingUid() {
        if (mRunningJob == NULL || Binder->GetCallingUid() != mRunningJob->GetUid()) {
            if (DEBUG) {
                Slogger::D(TAG, "Stale callback received, ignoring.");
            }
            return FALSE;
        }
        return TRUE;
    }

    /**
     * Handles the lifecycle of the JobService binding/callbacks, etc. The convention within this
     * class is to append 'H' to each function name that can only be called on this handler. This
     * isn't strictly necessary because all of these functions are private, but helps clarity.
     */
    private class JobServiceHandler extends Handler {
        JobServiceHandler(Looper looper) {
            Super(looper);
        }

        //@Override
        CARAPI HandleMessage(Message message) {
            switch (message.what) {
                case MSG_SERVICE_BOUND:
                    RemoveOpTimeOut();
                    HandleServiceBoundH();
                    break;
                case MSG_CALLBACK:
                    if (DEBUG) {
                        Slogger::D(TAG, "MSG_CALLBACK of : " + mRunningJob + " v:" +
                                (mVerb >= 0 ? VERB_STRINGS[mVerb] : "[invalid]"));
                    }
                    RemoveOpTimeOut();

                    if (mVerb == VERB_STARTING) {
                        final Boolean workOngoing = message.arg2 == 1;
                        HandleStartedH(workOngoing);
                    } else if (mVerb == VERB_EXECUTING ||
                            mVerb == VERB_STOPPING) {
                        final Boolean reschedule = message.arg2 == 1;
                        HandleFinishedH(reschedule);
                    } else {
                        if (DEBUG) {
                            Slogger::D(TAG, "Unrecognised callback: " + mRunningJob);
                        }
                    }
                    break;
                case MSG_CANCEL:
                    HandleCancelH();
                    break;
                case MSG_TIMEOUT:
                    HandleOpTimeoutH();
                    break;
                case MSG_SHUTDOWN_EXECUTION:
                    CloseAndCleanupJobH(TRUE /* needsReschedule */);
                    break;
                default:
                    Slogger::E(TAG, "Unrecognised message: " + message);
            }
        }

        /** Start the job on the service. */
        private void HandleServiceBoundH() {
            if (DEBUG) {
                Slogger::D(TAG, "MSG_SERVICE_BOUND for " + mRunningJob->ToShortString());
            }
            if (mVerb != VERB_BINDING) {
                Slogger::E(TAG, "Sending onStartJob for a job that isn't pending. "
                        + VERB_STRINGS[mVerb]);
                CloseAndCleanupJobH(FALSE /* reschedule */);
                return;
            }
            if (mCancelled->Get()) {
                if (DEBUG) {
                    Slogger::D(TAG, "Job cancelled while waiting for bind to complete. "
                            + mRunningJob);
                }
                CloseAndCleanupJobH(TRUE /* reschedule */);
                return;
            }
            try {
                mVerb = VERB_STARTING;
                ScheduleOpTimeOut();
                service->StartJob(mParams);
            } catch (RemoteException e) {
                Slogger::E(TAG, "Error sending onStart message to '" +
                        mRunningJob->GetServiceComponent()->GetShortClassName() + "' ", e);
            }
        }

        /**
         * State behaviours.
         * VERB_STARTING   -> Successful start, change job to VERB_EXECUTING and post timeout.
         *     _PENDING    -> Error
         *     _EXECUTING  -> Error
         *     _STOPPING   -> Error
         */
        private void HandleStartedH(Boolean workOngoing) {
            switch (mVerb) {
                case VERB_STARTING:
                    mVerb = VERB_EXECUTING;
                    if (!workOngoing) {
                        // Job is finished already so fast-forward to handleFinished.
                        HandleFinishedH(FALSE);
                        return;
                    }
                    if (mCancelled->Get()) {
                        if (DEBUG) {
                            Slogger::D(TAG, "Job cancelled while waiting for onStartJob to complete.");
                        }
                        // Cancelled *while* waiting for acknowledgeStartMessage from client.
                        HandleCancelH();
                        return;
                    }
                    ScheduleOpTimeOut();
                    break;
                default:
                    Slogger::E(TAG, "Handling started job but job wasn't starting! Was "
                            + VERB_STRINGS[mVerb] + ".");
                    return;
            }
        }

        /**
         * VERB_EXECUTING  -> Client called JobFinished(), clean up and notify done.
         *     _STOPPING   -> Successful finish, clean up and notify done.
         *     _STARTING   -> Error
         *     _PENDING    -> Error
         */
        private void HandleFinishedH(Boolean reschedule) {
            switch (mVerb) {
                case VERB_EXECUTING:
                case VERB_STOPPING:
                    CloseAndCleanupJobH(reschedule);
                    break;
                default:
                    Slogger::E(TAG, "Got an execution complete message for a job that wasn't being" +
                            "executed. Was " + VERB_STRINGS[mVerb] + ".");
            }
        }

        /**
         * A job can be in various states when a cancel request comes in:
         * VERB_BINDING    -> Cancelled before bind completed. Mark as cancelled and wait for
         *                    {@link #OnServiceConnected(android.content.ComponentName, android.os.IBinder)}
         *     _STARTING   -> Mark as cancelled and wait for
         *                    {@link JobServiceContext#AcknowledgeStartMessage(Int32, Boolean)}
         *     _EXECUTING  -> call {@link #sendStopMessageH}}, but only if there are no callbacks
         *                      in the message queue.
         *     _ENDING     -> No point in doing anything here, so we ignore.
         */
        private void HandleCancelH() {
            if (mRunningJob == NULL) {
                if (DEBUG) {
                    Slogger::D(TAG, "Trying to process cancel for torn-down context, ignoring.");
                }
                return;
            }
            if (JobSchedulerService.DEBUG) {
                Slogger::D(TAG, "Handling cancel for: " + mRunningJob->GetJobId() + " "
                        + VERB_STRINGS[mVerb]);
            }
            switch (mVerb) {
                case VERB_BINDING:
                case VERB_STARTING:
                    mCancelled->Set(TRUE);
                    break;
                case VERB_EXECUTING:
                    if (HasMessages(MSG_CALLBACK)) {
                        // If the client has called jobFinished, ignore this cancel.
                        return;
                    }
                    SendStopMessageH();
                    break;
                case VERB_STOPPING:
                    // Nada.
                    break;
                default:
                    Slogger::E(TAG, "Cancelling a job without a valid verb: " + mVerb);
                    break;
            }
        }

        /** Process MSG_TIMEOUT here. */
        private void HandleOpTimeoutH() {
            switch (mVerb) {
                case VERB_BINDING:
                    Slogger::E(TAG, "Time-out while trying to bind " + mRunningJob->ToShortString() +
                            ", dropping.");
                    CloseAndCleanupJobH(FALSE /* needsReschedule */);
                    break;
                case VERB_STARTING:
                    // Client unresponsive - wedged or failed to respond in time. We don't really
                    // know what happened so let's log it and notify the JobScheduler
                    // FINISHED/NO-RETRY.
                    Slogger::E(TAG, "No response from client for onStartJob '" +
                            mRunningJob->ToShortString());
                    CloseAndCleanupJobH(FALSE /* needsReschedule */);
                    break;
                case VERB_STOPPING:
                    // At least we got somewhere, so fail but ask the JobScheduler to reschedule.
                    Slogger::E(TAG, "No response from client for onStopJob, '" +
                            mRunningJob->ToShortString());
                    CloseAndCleanupJobH(TRUE /* needsReschedule */);
                    break;
                case VERB_EXECUTING:
                    // Not an error - client ran out of time.
                    Slogger::I(TAG, "Client timed out while executing (no jobFinished received)." +
                            " sending onStop. "  + mRunningJob->ToShortString());
                    SendStopMessageH();
                    break;
                default:
                    Slogger::E(TAG, "Handling timeout for an invalid job state: " +
                            mRunningJob->ToShortString() + ", dropping.");
                    CloseAndCleanupJobH(FALSE /* needsReschedule */);
            }
        }

        /**
         * Already running, need to stop. Will switch {@link #mVerb} from VERB_EXECUTING ->
         * VERB_STOPPING.
         */
        private void SendStopMessageH() {
            RemoveOpTimeOut();
            if (mVerb != VERB_EXECUTING) {
                Slogger::E(TAG, "Sending onStopJob for a job that isn't started. " + mRunningJob);
                CloseAndCleanupJobH(FALSE /* reschedule */);
                return;
            }
            try {
                mVerb = VERB_STOPPING;
                ScheduleOpTimeOut();
                service->StopJob(mParams);
            } catch (RemoteException e) {
                Slogger::E(TAG, "Error sending onStopJob to client.", e);
                CloseAndCleanupJobH(FALSE /* reschedule */);
            }
        }

        /**
         * The provided job has finished, either by calling
         * {@link android.app.job.JobService#JobFinished(android.app.job.JobParameters, Boolean)}
         * or from acknowledging the stop message we sent. Either way, we're done tracking it and
         * we want to clean up internally.
         */
        private void CloseAndCleanupJobH(Boolean reschedule) {
            final JobStatus completedJob = mRunningJob;
            {    AutoLock syncLock(mLock);
                try {
                    mBatteryStats->NoteJobFinish(mRunningJob->GetName(), mRunningJob->GetUid());
                } catch (RemoteException e) {
                    // Whatever.
                }
                if (mWakeLock != NULL) {
                    mWakeLock->Release();
                }
                mContext->UnbindService(JobServiceContext.this);
                mWakeLock = NULL;
                mRunningJob = NULL;
                mParams = NULL;
                mVerb = -1;
                mCancelled->Set(FALSE);
                service = NULL;
                mAvailable = TRUE;
            }
            RemoveOpTimeOut();
            RemoveMessages(MSG_CALLBACK);
            RemoveMessages(MSG_SERVICE_BOUND);
            RemoveMessages(MSG_CANCEL);
            RemoveMessages(MSG_SHUTDOWN_EXECUTION);
            mCompletedListener->OnJobCompleted(completedJob, reschedule);
        }
    }

    /**
     * Called when sending a message to the client, over whose execution we have no control. If
     * we haven't received a response in a certain amount of time, we want to give up and carry
     * on with life.
     */
    private void ScheduleOpTimeOut() {
        RemoveOpTimeOut();

        final Int64 timeoutMillis = (mVerb == VERB_EXECUTING) ?
                EXECUTING_TIMESLICE_MILLIS : OP_TIMEOUT_MILLIS;
        if (DEBUG) {
            Slogger::D(TAG, "Scheduling time out for '" +
                    mRunningJob->GetServiceComponent()->GetShortClassName() + "' jId: " +
                    mParams->GetJobId() + ", in " + (timeoutMillis / 1000) + " s");
        }
        Message m = mCallbackHandler->ObtainMessage(MSG_TIMEOUT);
        mCallbackHandler->SendMessageDelayed(m, timeoutMillis);
        mTimeoutElapsed = SystemClock->ElapsedRealtime() + timeoutMillis;
    }


    private void RemoveOpTimeOut() {
        mCallbackHandler->RemoveMessages(MSG_TIMEOUT);
    }
}
