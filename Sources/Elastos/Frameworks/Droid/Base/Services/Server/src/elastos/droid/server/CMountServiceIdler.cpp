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

#include <elastos/droid/server/CMountServiceIdler.h>
// #include <elastos/droid/server/CMountService.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Utility.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::Job::IJobInfo;
using Elastos::Droid::App::Job::IJobInfoBuilder;
using Elastos::Droid::App::Job::CJobInfoBuilder;
using Elastos::Droid::App::Job::IJobParameters;
using Elastos::Droid::App::Job::IJobScheduler;
using Elastos::Droid::App::Job::JobService;
using Elastos::Droid::App::Job::IJobService;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

static AutoPtr<IComponentName> InitIdleService()
{
    AutoPtr<IComponentName> cn;
    CComponentName::New(
        String("Elastos.Droid.Server.eco"),
        String("Elastos.Droid.Server.CMountServiceIdler"), (IComponentName**)&cn);
    return cn;
}

const String CMountServiceIdler::TAG("CMountServiceIdler");
AutoPtr<IComponentName> CMountServiceIdler::sIdleService = InitIdleService();
Int32 CMountServiceIdler::MOUNT_JOB_ID = 808;

//===================================================================
// CMountServiceIdler::FinishCallbackRunnable
//===================================================================
CMountServiceIdler::FinishCallbackRunnable::FinishCallbackRunnable(
    /* [in] */ CMountServiceIdler* host)
    : mHost(host)
{}


ECode CMountServiceIdler::FinishCallbackRunnable::Run()
{
    Slogger::I(CMountServiceIdler::TAG, "Got mount service completion callback");
    {    AutoLock syncLock(mHost->mFinishCallback.Get());
        if (mHost->mStarted) {
            mHost->JobFinished(mHost->mJobParams, FALSE);
            mHost->mStarted = FALSE;
        }
    }
    // ... and try again tomorrow
    mHost->ScheduleIdlePass(mHost);
    return NOERROR;
}

//===================================================================
// CMountServiceIdler
//===================================================================
CAR_INTERFACE_IMPL(CMountServiceIdler, JobService, IMountServiceIdler)

CAR_OBJECT_IMPL(CMountServiceIdler)

CMountServiceIdler::CMountServiceIdler()
    : mStarted(FALSE)
{}

CMountServiceIdler::~CMountServiceIdler()
{}

ECode CMountServiceIdler::constructor()
{
    JobService::constructor();

    mFinishCallback = new FinishCallbackRunnable(this);
    return NOERROR;
}

ECode CMountServiceIdler::OnStartJob(
    /* [in] */ IJobParameters* params,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // The mount service will run an fstrim operation asynchronously
    // on a designated separate thread, so we provide it with a callback
    // that lets us cleanly end our idle timeslice.  It's safe to call
    // FinishIdle() from any thread.
    mJobParams = params;
    assert(0 && "TODO");
    // CMountService* ms = CMountService::sSelf;
    // if (ms != NULL) {
    //     Object* obj = mFinishCallback.Get();
    //     {    AutoLock syncLock(obj);
    //         mStarted = TRUE;
    //     }
    //     ms->RunIdleMaintenance(mFinishCallback);
    // }
    // *result = ms != NULL;
    return NOERROR;
}

ECode CMountServiceIdler::OnStopJob(
    /* [in] */ IJobParameters* params,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // Once we kick off the fstrim we aren't actually interruptible; just note
    // that we don't need to call JobFinished(), and let everything happen in
    // the callback from the mount service.
    Object* obj = mFinishCallback.Get();
    {    AutoLock syncLock(obj);
        mStarted = FALSE;
    }
    return FALSE;
}

void CMountServiceIdler::ScheduleIdlePass(
    /* [in] */ IContext* context)
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::JOB_SCHEDULER_SERVICE, (IInterface**)&service);
    AutoPtr<IJobScheduler> tm = IJobScheduler::Probe(service);

    AutoPtr<ICalendar> calendar = TomorrowMidnight();
    Int64 ms, currentMs;
    calendar->GetTimeInMillis(&ms);
    system->GetCurrentTimeMillis(&currentMs);
    Int64 timeToMidnight = ms - currentMs;

    AutoPtr<IJobInfoBuilder> builder;
    CJobInfoBuilder::New(MOUNT_JOB_ID, sIdleService, (IJobInfoBuilder**)&builder);
    builder->SetRequiresDeviceIdle(TRUE);
    builder->SetRequiresCharging(TRUE);
    builder->SetMinimumLatency(timeToMidnight);

    AutoPtr<IJobInfo> jobInfo;
    builder->Build((IJobInfo**)&jobInfo);
    Int32 ival;
    tm->Schedule(jobInfo, &ival);
}

AutoPtr<ICalendar> CMountServiceIdler::TomorrowMidnight()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> calendar;
    helper->GetInstance((ICalendar**)&calendar);
    Int64 ms;
    system->GetCurrentTimeMillis(&ms);
    calendar->SetTimeInMillis(ms);
    calendar->Set(ICalendar::HOUR, 0);
    calendar->Set(ICalendar::MINUTE, 0);
    calendar->Set(ICalendar::SECOND, 0);
    calendar->Set(ICalendar::MILLISECOND, 0);
    calendar->Add(ICalendar::DAY_OF_MONTH, 1);
    return calendar;
}

} // Server
} // Droid
} // Elastos
