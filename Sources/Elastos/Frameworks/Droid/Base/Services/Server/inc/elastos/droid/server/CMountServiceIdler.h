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
