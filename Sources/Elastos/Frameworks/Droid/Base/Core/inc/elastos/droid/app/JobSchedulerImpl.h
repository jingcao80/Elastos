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

