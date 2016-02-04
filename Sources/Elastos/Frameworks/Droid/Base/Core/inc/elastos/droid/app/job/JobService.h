
#ifndef __ELASTOS_DROID_APP_JOB_CJOBPARAMETERS_H__
#define __ELASTOS_DROID_APP_JOB_CJOBPARAMETERS_H__

#include "elastos/droid/app/Service.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;

namespace Elastos {
namespace Droid {
namespace App {
namespace Job {

/**
 * Contains the parameters used to configure/identify your job. You do not create this object
 * yourself, instead it is handed in to your application by the System.
 */
class ECO_PUBLIC JobService
    : public Service
    , public IJobService
{
private:
    /**
     * Runs on application's main thread - callbacks are meant to offboard work to some other
     * (app-specified) mechanism.
     * @hide
     */
    class JobHandler
        : public Handler
    {
    public:
        JobHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ JobService* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI AckStartMessage(
            /* [in] */ IJobParameters* params,
            /* [in] */ Boolean workOngoing);

        CARAPI AckStopMessage(
            /* [in] */ IJobParameters* params,
            /* [in] */ Boolean reschedule);
    private:
        JobService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /** @hide */
    CARAPI EnsureHandler();

    /** @hide */
    CARAPI OnBind(
        /* [in] */ IIntent* Int32ent,
        /* [out] */ IBinder** binder);

    /**
     * Override this method with the callback logic for your job. Any such logic needs to be
     * performed on a separate thread, as this function is executed on your application's main
     * thread.
     *
     * @param params Parameters specifying info about this job, including the extras bundle you
     *               optionally provided at job-creation time.
     * @return True if your service needs to process the work (on a separate thread). False if
     * there's no more work to be done for this job.
     */
    virtual CARAPI OnStartJob(
        /* [in] */ IJobParameters* params,
        /* [out] */ Boolean* result) = 0;

    /**
     * This method is called if the system has determined that you must stop execution of your job
     * even before you've had a chance to call {@link #jobFinished(JobParameters, Boolean)}.
     *
     * <p>This will happen if the requirements specified at schedule time are no longer met. For
     * example you may have requested WiFi with
     * {@link android.app.job.JobInfo.Builder#setRequiredNetworkType(Int32)}, yet while your
     * job was executing the user toggled WiFi. Another example is if you had specified
     * {@link android.app.job.JobInfo.Builder#setRequiresDeviceIdle(Boolean)}, and the phone left its
     * idle maInt32enance window. You are solely responsible for the behaviour of your application
     * upon receipt of this message; your app will likely start to misbehave if you ignore it. One
     * immediate repercussion is that the system will cease holding a wakelock for you.</p>
     *
     * @param params Parameters specifying info about this job.
     * @return True to indicate to the JobManager whether you'd like to reschedule this job based
     * on the retry criteria provided at job creation-time. False to drop the job. Regardless of
     * the value returned, your job must stop executing.
     */
    virtual CARAPI OnStopJob(
        /* [in] */ IJobParameters* params,
        /* [out] */ Boolean* result) = 0;

    /**
     * Callback to inform the JobManager you've finished executing. This can be called from any
     * thread, as it will ultimately be run on your application's main thread. When the system
     * receives this message it will release the wakelock being held.
     * <p>
     *     You can specify post-execution behaviour to the scheduler here with
     *     <code>needsReschedule </code>. This will apply a back-off timer to your job based on
     *     the default, or what was set with
     *     {@link android.app.job.JobInfo.Builder#setBackoffCriteria(long, Int32)}. The original
     *     requirements are always honoured even for a backed-off job. Note that a job running in
     *     idle mode will not be backed-off. Instead what will happen is the job will be re-added
     *     to the queue and re-executed within a future idle maInt32enance window.
     * </p>
     *
     * @param params Parameters specifying system-provided info about this job, this was given to
     *               your application in {@link #onStartJob(JobParameters)}.
     * @param needsReschedule True if this job should be rescheduled according to the back-off
     *                        criteria specified at schedule-time. False otherwise.
     */
    CARAPI JobFinished(
        /* [in] */ IJobParameters* params,
        /* [in] */ Boolean needsReschedule);

private:
    friend class JobHandler;

    ECO_LOCAL static const String TAG;
    /**
     * Identifier for a message that will result in a call to
     * {@link #onStartJob(android.app.job.JobParameters)}.
     */
    ECO_LOCAL static const Int32 MSG_EXECUTE_JOB;
    /**
     * Message that will result in a call to {@link #onStopJob(android.app.job.JobParameters)}.
     */
    ECO_LOCAL static const Int32 MSG_STOP_JOB;
    /**
     * Message that the client has completed execution of this job.
     */
    ECO_LOCAL static const Int32 MSG_JOB_FINISHED;

    /** Lock object for {@link #mHandler}. */
    Object mHandlerLock;

    /**
     * Handler we post jobs to. Responsible for calling Int32o the client logic, and handling the
     * callback to the system.
     */
    // @GuardedBy("mHandlerLock")
    AutoPtr<JobHandler> mHandler;

    /** Binder for this service. */
    AutoPtr<IIJobService> mBinder;
    //  = new IJobService.Stub() {
    //     //@Override
    //     public void startJob(JobParameters jobParams) {
    //         FAIL_RETURN(EnsureHandler())
    //         Message m = Message.obtain(mHandler, MSG_EXECUTE_JOB, jobParams);
    //         m.sendToTarget();
    //     }
    //     //@Override
    //     public void stopJob(JobParameters jobParams) {
    //         FAIL_RETURN(EnsureHandler())
    //         Message m = Message.obtain(mHandler, MSG_STOP_JOB, jobParams);
    //         m.sendToTarget();
    //     }
    // };
};

} // namespace Job
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_JOB_CJOBPARAMETERS_H__