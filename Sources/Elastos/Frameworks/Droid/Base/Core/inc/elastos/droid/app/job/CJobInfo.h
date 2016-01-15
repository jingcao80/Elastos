
#ifndef __ELASTOS_DROID_APP_JOB_CJOBINFO_H__
#define __ELASTOS_DROID_APP_JOB_CJOBINFO_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_App_Job_CJobInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IPersistableBundle;

namespace Elastos {
namespace Droid {
namespace App {
namespace Job {

/**
 * Container of data passed to the {@link android.app.job.JobScheduler} fully encapsulating the
 * parameters required to schedule work against the calling application. These are constructed
 * using the {@link JobInfo.Builder}.
 * You must specify at least one sort of constraInt32 on the JobInfo object that you are creating.
 * The goal here is to provide the scheduler with high-level semantics about the work you want to
 * accomplish. Doing otherwise with throw an exception in your app.
 */
CarClass(CJobInfo)
    , public Object
    , public IJobInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CJobInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IJobInfoBuilder* b);

    /**
     * Unique job id associated with this class. This is assigned to your job by the scheduler.
     */
    CARAPI GetId(
        /* [out] */ Int32* id);

    /**
     * Bundle of extras which are returned to your application at execution time.
     */
    CARAPI GetExtras(
        /* [out] */ IPersistableBundle** extras);

    /**
     * Name of the service endpoInt32 that will be called back Int32o by the JobScheduler.
     */
    CARAPI GetService(
        /* [out] */ IComponentName** service);

    /**
     * Whether this job needs the device to be plugged in.
     */
    CARAPI IsRequireCharging(
        /* [out] */ Boolean* result);

    /**
     * Whether this job needs the device to be in an Idle maInt32enance window.
     */
    CARAPI IsRequireDeviceIdle(
        /* [out] */ Boolean* result);

    /**
     * One of {@link android.app.job.JobInfo#NETWORK_TYPE_ANY},
     * {@link android.app.job.JobInfo#NETWORK_TYPE_NONE}, or
     * {@link android.app.job.JobInfo#NETWORK_TYPE_UNMETERED}.
     */
    CARAPI GetNetworkType(
        /* [out] */ Int32* result);

    /**
     * Set for a job that does not recur periodically, to specify a delay after which the job
     * will be eligible for execution. This value is not set if the job recurs periodically.
     */
    CARAPI GetMinLatencyMillis(
        /* [out] */ Int64* result);

    /**
     * See {@link Builder#setOverrideDeadline(Int64)}. This value is not set if the job recurs
     * periodically.
     */
    CARAPI GetMaxExecutionDelayMillis(
        /* [out] */ Int64* result);

    /**
     * Track whether this job will repeat with a given period.
     */
    CARAPI IsPeriodic(
        /* [out] */ Boolean* result);

    /**
     * @return Whether or not this job should be persisted across device reboots.
     */
    CARAPI IsPersisted(
        /* [out] */ Boolean* result);

    /**
     * Set to the Int32erval between occurrences of this job. This value is <b>not</b> set if the
     * job does not recur periodically.
     */
    CARAPI GetIntervalMillis(
        /* [out] */ Int64* result);

    /**
     * The amount of time the JobScheduler will wait before rescheduling a failed job. This value
     * will be increased depending on the backoff policy specified at job creation time. Defaults
     * to 5 seconds.
     */
    CARAPI GetInitialBackoffMillis(
        /* [out] */ Int64* result);

    /**
     * One of either {@link android.app.job.JobInfo#BACKOFF_POLICY_EXPONENTIAL}, or
     * {@link android.app.job.JobInfo#BACKOFF_POLICY_LINEAR}, depending on which criteria you set
     * when creating this job.
     */
    CARAPI GetBackoffPolicy(
        /* [out] */ Int32* result);

    /**
     * User can specify an early constraInt32 of 0L, which is valid, so we keep track of whether the
     * function was called at all.
     * @hide
     */
    CARAPI HasEarlyConstraint(
        /* [out] */ Boolean* result);

    /**
     * User can specify a late constraInt32 of 0L, which is valid, so we keep track of whether the
     * function was called at all.
     * @hide
     */
    CARAPI HasLateConstraint(
        /* [out] */ Boolean* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    Int32 mJobId;
    AutoPtr<IPersistableBundle> mExtras;
    AutoPtr<IComponentName> mService;
    Boolean mRequireCharging;
    Boolean mRequireDeviceIdle;
    Boolean mHasEarlyConstraint;
    Boolean mHasLateConstraint;
    Int32 mNetworkType;
    Int64 mMinLatencyMillis;
    Int64 mMaxExecutionDelayMillis;
    Boolean mIsPeriodic;
    Boolean mIsPersisted;
    Int64 mIntervalMillis;
    Int64 mInitialBackoffMillis;
    Int32 mBackoffPolicy;
};

/** Builder class for constructing {@link JobInfo} objects. */
class JobInfoBuilder
    : public Object
    , public IJobInfoBuilder
{
public:
    CAR_INTERFACE_DECL()

    JobInfoBuilder();

    virtual ~JobInfoBuilder();

    /**
     * @param jobId Application-provided id for this job. Subsequent calls to cancel, or
     *               jobs created with the same jobId, will update the pre-existing job with
     *               the same id.
     * @param jobService The endpoInt32 that you implement that will receive the callback from the
     *            JobScheduler.
     */
    CARAPI constructor(
        /* [in] */ Int32 jobId,
        /* [in] */ IComponentName* jobService);

    /**
     * Set optional extras. This is persisted, so we only allow primitive types.
     * @param extras Bundle containing extras you want the scheduler to hold on to for you.
     */
    CARAPI SetExtras(
        /* [in] */ IPersistableBundle* extras);

    /**
     * Set some description of the kind of network type your job needs to have.
     * Not calling this function means the network is not necessary, as the default is
     * {@link #NETWORK_TYPE_NONE}.
     * Bear in mind that calling this function defines network as a strict requirement for your
     * job. If the network requested is not available your job will never run. See
     * {@link #setOverrideDeadline(Int64)} to change this behaviour.
     */
    CARAPI SetRequiredNetworkType(
        /* [in] */ Int32 networkType);

    /**
     * Specify that to run this job, the device needs to be plugged in. This defaults to
     * FALSE.
     * @param requiresCharging Whether or not the device is plugged in.
     */
    CARAPI SetRequiresCharging(
        /* [in] */ Boolean requiresCharging);

    /**
     * Specify that to run, the job needs the device to be in idle mode. This defaults to
     * FALSE.
     * <p>Idle mode is a loose definition provided by the system, which means that the device
     * is not in use, and has not been in use for some time. As such, it is a good time to
     * perform resource heavy jobs. Bear in mind that battery usage will still be attributed
     * to your application, and surfaced to the user in battery stats.</p>
     * @param requiresDeviceIdle Whether or not the device need be within an idle maInt32enance
     *                           window.
     */
    CARAPI SetRequiresDeviceIdle(
        /* [in] */ Boolean requiresDeviceIdle);

    /**
     * Specify that this job should recur with the provided Int32erval, not more than once per
     * period. You have no control over when within this Int32erval this job will be executed,
     * only the guarantee that it will be executed at most once within this Int32erval.
     * Setting this function on the builder with {@link #setMinimumLatency(Int64)} or
     * {@link #setOverrideDeadline(Int64)} will result in an error.
     * @param Int32ervalMillis Millisecond Int32erval for which this job will repeat.
     */
    CARAPI SetPeriodic(
        /* [in] */ Int64 Int32ervalMillis);

    /**
     * Specify that this job should be delayed by the provided amount of time.
     * Because it doesn't make sense setting this property on a periodic job, doing so will
     * throw an {@link java.lang.IllegalArgumentException} when
     * {@link android.app.job.JobInfo.Builder#build()} is called.
     * @param minLatencyMillis Milliseconds before which this job will not be considered for
     *                         execution.
     */
    CARAPI SetMinimumLatency(
        /* [in] */ Int64 minLatencyMillis);

    /**
     * Set deadline which is the maximum scheduling latency. The job will be run by this
     * deadline even if other requirements are not met. Because it doesn't make sense setting
     * this property on a periodic job, doing so will throw an
     * {@link java.lang.IllegalArgumentException} when
     * {@link android.app.job.JobInfo.Builder#build()} is called.
     */
    CARAPI SetOverrideDeadline(
        /* [in] */ Int64 maxExecutionDelayMillis);

    /**
     * Set up the back-off/retry policy.
     * This defaults to some respectable values: {30 seconds, Exponential}. We cap back-off at
     * 5hrs.
     * Note that trying to set a backoff criteria for a job with
     * {@link #setRequiresDeviceIdle(Boolean)} will throw an exception when you call build().
     * This is because back-off typically does not make sense for these types of jobs. See
     * {@link android.app.job.JobService#jobFinished(android.app.job.JobParameters, Boolean)}
     * for more description of the return value for the case of a job executing while in idle
     * mode.
     * @param initialBackoffMillis Millisecond time Int32erval to wait initially when job has
     *                             failed.
     * @param backoffPolicy is one of {@link #BACKOFF_POLICY_LINEAR} or
     * {@link #BACKOFF_POLICY_EXPONENTIAL}
     */
    CARAPI SetBackoffCriteria(
        /* [in] */ Int64 initialBackoffMillis,
        /* [in] */ Int32 backoffPolicy);

    /**
     * Set whether or not to persist this job across device reboots. This will only have an
     * effect if your application holds the permission
     * {@link android.Manifest.permission#RECEIVE_BOOT_COMPLETED}. Otherwise an exception will
     * be thrown.
     * @param isPersisted True to indicate that the job will be written to disk and loaded at
     *                    boot.
     */
    CARAPI SetPersisted(
        /* [in] */ Boolean isPersisted);

    /**
     * @return The job object to hand to the JobScheduler. This object is immutable.
     */
    CARAPI Build(
        /* [out] */ IJobInfo** info);

private:
    friend class CJobInfo;

    Int32 mJobId;
    AutoPtr<IPersistableBundle> mExtras;// = IPersistableBundle*.EMPTY;
    AutoPtr<IComponentName> mJobService;
    // Requirements.
    Boolean mRequiresCharging;
    Boolean mRequiresDeviceIdle;
    Int32 mNetworkType;
    Boolean mIsPersisted;
    // One-off parameters.
    Int64 mMinLatencyMillis;
    Int64 mMaxExecutionDelayMillis;
    // Periodic parameters.
    Boolean mIsPeriodic;
    Boolean mHasEarlyConstraint;
    Boolean mHasLateConstraint;
    Int64 mIntervalMillis;
    // Back-off parameters.
    Int64 mInitialBackoffMillis;// = DEFAULT_INITIAL_BACKOFF_MILLIS;
    Int32 mBackoffPolicy;// = DEFAULT_BACKOFF_POLICY;
    /** Easy way to track whether the client has tried to set a back-off policy. */
    Boolean mBackoffPolicySet;// = FALSE;
};

} // namespace Job
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_JOB_CJOBINFO_H__
