
#ifndef __ELASTOS_PROVIDERS_DOWNLOADPROVIDER_DOWNLOADS_CDOWNLOADIDLESERVICE_H__
#define __ELASTOS_PROVIDERS_DOWNLOADPROVIDER_DOWNLOADS_CDOWNLOADIDLESERVICE_H__

#include "elastos/droid/app/job/JobService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Providers_DownloadProvider_Downloads_CDownloadIdleService.h"
#include "elastos/core/Object.h"

using Elastos::Droid::App::Job::IJobParameters;
using Elastos::Droid::App::Job::JobService;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Providers {
namespace DownloadProvider {
namespace Downloads {

/**
 * Idle-time service for {@link DownloadManager}. Reconciles database
 * metadata and files on disk, which can become inconsistent when files are
 * deleted directly on disk.
 */
CarClass(CDownloadIdleService)
    , public JobService
    , public IDownloadIdleService
{
private:
    class IdleRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        IdleRunnable(
            /* [in] */ IJobParameters* params,
            /* [in] */ CDownloadIdleService* host);

        CARAPI Run();

    private:
        AutoPtr<IJobParameters> mParams;
        CDownloadIdleService* mHost;
    };

    class StaleQuery
        : public Object
    {
    public:
        static AutoPtr<ArrayOf<String> > PROJECTION;

        static Int32 _ID;
    };

    class OrphanQuery
        : public Object
    {
    public:
        static AutoPtr<ArrayOf<String> > PROJECTION;

        static Int32 _ID;
        static Int32 _DATA;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI OnStartJob(
        /* [in] */ IJobParameters* params,
        /* [out] */ Boolean* result);

    CARAPI OnStopJob(
        /* [in] */ IJobParameters* params,
        /* [out] */ Boolean* result);

    /**
     * Remove stale downloads that third-party apps probably forgot about. We
     * only consider non-visible downloads that haven't been touched in over a
     * week.
     */
    CARAPI CleanStale();

    /**
     * Clean up orphan downloads, both in database and on disk.
     */
    CARAPI CleanOrphans();
};

} // namespace Downloads
} // namespace DownloadProvider
} // namespace Providers
} // namespace Elastos

#endif //__ELASTOS_PROVIDERS_DOWNLOADPROVIDER_DOWNLOADS_CDOWNLOADIDLESERVICE_H__
