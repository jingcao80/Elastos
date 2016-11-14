
#ifndef __ELASTOS_DROID_SERVER_MOUNT_SERVICE_IDLER_H__
#define __ELASTOS_DROID_SERVER_MOUNT_SERVICE_IDLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CMountServiceIdler.h"
#include <elastos/core/Runnable.h>
#include <elastos/droid/app/job/JobService.h>

using Elastos::Core::Runnable;
using Elastos::Droid::App::Job::IJobInfo;
using Elastos::Droid::App::Job::IJobParameters;
using Elastos::Droid::App::Job::IJobScheduler;
using Elastos::Droid::App::Job::JobService;
using Elastos::Droid::App::Job::IJobService;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;

using Elastos::Utility::ICalendar;

namespace Elastos {
namespace Droid {
namespace Server {

class CMountServiceIdler
    : public JobService
    , public IMountServiceIdler
{
private:

    class FinishCallbackRunnable
        : public Runnable
    {
    public:
        FinishCallbackRunnable(
            /* [in] */ CMountServiceIdler* host);

        //@Override
        CARAPI Run();

    private:
        CMountServiceIdler* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CMountServiceIdler();

    ~CMountServiceIdler();

    CARAPI constructor();

    //@Override
    CARAPI OnStartJob(
        /* [in] */ IJobParameters* params,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnStopJob(
        /* [in] */ IJobParameters* params,
        /* [out] */ Boolean* result);

    /**
     * Schedule the idle job that will ping the mount service
     */
    static void ScheduleIdlePass(
        /* [in] */ IContext* context);

private:
     static AutoPtr<ICalendar> TomorrowMidnight();

private:
    static const String TAG;

    static AutoPtr<IComponentName> sIdleService;

    static Int32 MOUNT_JOB_ID;

    Boolean mStarted;
    AutoPtr<IJobParameters> mJobParams;
    AutoPtr<FinishCallbackRunnable> mFinishCallback;
};


} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_MOUNT_SERVICE_IDLER_H__
