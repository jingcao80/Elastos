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

#include "elastos/droid/app/JobSchedulerImpl.h"

using Elastos::Droid::App::Job::EIID_IJobScheduler;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(JobSchedulerImpl, Object, IJobScheduler)

JobSchedulerImpl::JobSchedulerImpl()
{}

JobSchedulerImpl::~JobSchedulerImpl()
{}

ECode JobSchedulerImpl::constructor(
    /* [in] */ IIJobScheduler* binder)
{
    mBinder = binder;
    return NOERROR;
}

ECode JobSchedulerImpl::Schedule(
    /* [in] */ IJobInfo* job,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    // TODO:cannot get JOB_SCHEDULER_SERVICE service
    // *result = IJobScheduler::RESULT_FAILURE;

    // ECode ec = mBinder->Schedule(job, result);
    // if (ec == (ECode)E_REMOTE_EXCEPTION) {
    //     *result = IJobScheduler::RESULT_FAILURE;
    //     ec = NOERROR;
    // }
    // return ec;
    return NOERROR;
}

ECode JobSchedulerImpl::Cancel(
    /* [in] */ Int32 jobId)
{
    return mBinder->Cancel(jobId);;
}

ECode JobSchedulerImpl::CancelAll()
{
    return mBinder->CancelAll();
}

ECode JobSchedulerImpl::GetAllPendingJobs(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    return mBinder->GetAllPendingJobs(result);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
