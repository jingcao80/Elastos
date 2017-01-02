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

#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADIDLESERVICE_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADIDLESERVICE_H__

#include "elastos/droid/app/job/JobService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Providers_Downloads_CDownloadIdleService.h"
#include "elastos/core/Object.h"

using Elastos::Droid::App::Job::IJobParameters;
using Elastos::Droid::App::Job::JobService;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Providers {
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
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADIDLESERVICE_H__
