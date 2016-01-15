
#ifndef __ELASTOS_DROID_APP_JOB_SCHEDULERIMPL_H__
#define __ELASTOS_DROID_APP_JOB_SCHEDULERIMPL_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Job::IJobInfo;
using Elastos::Droid::App::Job::IJobScheduler;
using Elastos::Droid::App::Job::IIJobScheduler;

using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Concrete implementation of the JobScheduler Int32erface
 * @hide
 */
class JobSchedulerImpl
    : public Object
    , public IJobScheduler
{
public:
    CAR_INTERFACE_DECL()

    JobSchedulerImpl();

    virtual ~JobSchedulerImpl();

    CARAPI constructor(
        /* [in] */ IIJobScheduler* binder);

    CARAPI Schedule(
        /* [in] */ IJobInfo* job,
        /* [out] */ Int32* result);

    CARAPI Cancel(
        /* [in] */ Int32 jobId);

    CARAPI CancelAll();

    CARAPI GetAllPendingJobs(
        /* [out] */ IList** result);

private:
    AutoPtr<IIJobScheduler> mBinder;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_JOB_SCHEDULERIMPL_H__

