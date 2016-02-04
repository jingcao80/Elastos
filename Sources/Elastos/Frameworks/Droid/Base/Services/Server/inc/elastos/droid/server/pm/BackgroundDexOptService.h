
#ifndef __ELASTOS_DROID_SERVER_WM_BACKGROUNDDEXOPTSERVICE_H__
#define __ELASTOS_DROID_SERVER_WM_BACKGROUNDDEXOPTSERVICE_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/job/JobService.h"
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Core::Thread;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Droid::App::Job::JobService;
using Elastos::Droid::App::Job::IJobParameters;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class CPackageManagerService;

class BackgroundDexOptService : public JobService
{
private:
    class OnStartJobThread : public Thread
    {
    public:
        OnStartJobThread(
            /* [in] */ BackgroundDexOptService* host,
            /* [in] */ CPackageManagerService* pm,
            /* [in] */ HashSet<String>* pkgs,
            /* [in] */ IJobParameters* jobParams);

        CARAPI Run();

    private:
        BackgroundDexOptService* mHost;
        AutoPtr<CPackageManagerService> mPm;
        AutoPtr<HashSet<String> > mPkgs;
        AutoPtr<IJobParameters> mJobParams;
    };

public:
    BackgroundDexOptService();

    static CARAPI_(void) Schedule(
        /* [in] */ IContext* context);

    CARAPI OnStartJob(
        /* [in] */ IJobParameters* params,
        /* [out] */ Boolean* result);

    CARAPI OnStopJob(
        /* [in] */ IJobParameters* params,
        /* [out] */ Boolean* result);

public:
    static const String TAG;

    static const Int32 BACKGROUND_DEXOPT_JOB = 800;

    AutoPtr<IAtomicBoolean> mIdleTime;

private:
    static AutoPtr<IComponentName> sDexoptServiceName;
};

} // Pm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_BACKGROUNDDEXOPTSERVICE_H__
