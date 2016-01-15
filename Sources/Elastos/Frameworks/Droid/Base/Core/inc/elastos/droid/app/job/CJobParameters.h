
#ifndef __ELASTOS_DROID_APP_JOB_CJOBPARAMETERS_H__
#define __ELASTOS_DROID_APP_JOB_CJOBPARAMETERS_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_App_Job_CJobParameters.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Job::IIJobCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IPersistableBundle;


namespace Elastos {
namespace Droid {
namespace App {
namespace Job {

/**
 * Contains the parameters used to configure/identify your job. You do not create this object
 * yourself, instead it is handed in to your application by the System.
 */
CarClass(CJobParameters)
    , public Object
    , public IJobParameters
    , public IParcelable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CJobParameters();

    CARAPI constructor();

    /** @hide */
    CARAPI constructor(
        /* [in] */ IBinder* callback,
        /* [in] */ Int32 jobId,
        /* [in] */ IPersistableBundle* extras,
        /* [in] */ Boolean overrideDeadlineExpired);

    /**
     * @return The unique id of this job, specified at creation time.
     */
    CARAPI GetJobId(
        /* [out] */ Int32* id);

    /**
     * @return The extras you passed in when constructing this job with
     * {@link android.app.job.JobInfo.Builder#setExtras(android.os.PersistableBundle)}. This will
     * never be null. If you did not set any extras this will be an empty bundle.
     */
    CARAPI GetExtras(
        /* [out] */ IPersistableBundle** extras);

    /**
     * For jobs with {@link android.app.job.JobInfo.Builder#setOverrideDeadline(long)} set, this
     * provides an easy way to tell whether the job is being executed due to the deadline
     * expiring. Note: If the job is running because its deadline expired, it implies that its
     * constraints will not be met.
     */
    CARAPI IsOverrideDeadlineExpired(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI GetCallback(
        /* [out] */ IIJobCallback** cb);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    Int32 mJobId;
    AutoPtr<IPersistableBundle> mExtras;
    AutoPtr<IBinder> mCallback;
    Boolean mOverrideDeadlineExpired;
};

} // namespace Job
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_JOB_CJOBPARAMETERS_H__
