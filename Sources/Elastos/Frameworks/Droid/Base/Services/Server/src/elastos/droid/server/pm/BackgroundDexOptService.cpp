
#include "pm/BackgroundDexOptService.h"
#include "pm/CPackageManagerService.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::App::Job::IJobScheduler;
using Elastos::Droid::App::Job::IJobInfoBuilder;
using Elastos::Droid::App::Job::CJobInfoBuilder;
using Elastos::Droid::App::Job::IJobInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

//==============================================================================
//                  BackgroundDexOptService::OnStartJobThread
//==============================================================================

BackgroundDexOptService::OnStartJobThread::OnStartJobThread(
    /* [in] */ BackgroundDexOptService* host,
    /* [in] */ CPackageManagerService* pm,
    /* [in] */ HashSet<String>* pkgs)
    : mHost(host)
    , mPm(pm)
    , mPkgs(pkgs)
{}

ECode BackgroundDexOptService::OnStartJobThread::Run()
{
    HashSet<String>::Iterator it = mPkgs->Begin();
    for (; it != mPkgs->End(); ++it) {
        String pkg = *it;
        Boolean value;
        if (mHost->mIdleTime->Get(&value), !value) {
            // stopped while still working, so we need to reschedule
            Schedule(IContext::Probe(mHost));
            return NOERROR;
        }
        mPm->PerformDexOpt(pkg, String(NULL) /* instruction set */, TRUE);
    }
    // ran to completion, so we abandon our timeslice and do not reschedule
    return mHost->JobFinished(jobParams, FALSE);
}


//==============================================================================
//                  BackgroundDexOptService
//==============================================================================

const String BackgroundDexOptService::TAG("BackgroundDexOptService");
const Int32 BackgroundDexOptService::BACKGROUND_DEXOPT_JOB;

static AutoPtr<IComponentName> InitDexoptServiceName()
{
    AutoPtr<CComponentName> cn;
    CComponentName::NewByFriend(String("android"), String("BackgroundDexOptService")/*BackgroundDexOptService.class.getName()*/);
    return (IComponentName*)cn.Get();
}
AutoPtr<IComponentName> BackgroundDexOptService::sDexoptServiceName = InitDexoptServiceName();

BackgroundDexOptService::BackgroundDexOptService()
{
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mIdleTime);
}

void BackgroundDexOptService::Schedule(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::JOB_SCHEDULER_SERVICE, (IInterface**)&service);
    AutoPtr<IJobScheduler> js = IJobScheduler::Probe(service);
    AutoPtr<IJobInfoBuilder> builder;
    CJobInfoBuilder::New(BACKGROUND_DEXOPT_JOB, sDexoptServiceName, (IJobInfoBuilder**)&builder);
    builder->SetrequiresDeviceIdle(TRUE);
    builder->SetrequiresCharging(TRUE);
    AutoPtr<IJobInfo> job;
    builder->Build((IJobInfo**)&job);
    js->Schedule(job);
}

ECode BackgroundDexOptService::OnStartJob(
    /* [in] */ IJobParameters* params,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Logger::I(TAG, "onIdleStart");
    AutoPtr<IInterface> p = ServiceManager::GetService(String("package"));
    AutoPtr<CPackageManagerService> pm = p->Probe(EIID_CPackageManagerService);
    Boolean isStorageLow;
    if (pm->IsStorageLow(&isStorageLow), isStorageLow) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr< HashSet<String> > pkgs = pm->GetPackagesThatNeedDexOpt();
    if (pkgs == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IJobParameters> jobParams = params;
    mIdleTime->Set(TRUE);
    AutoPtr<OnStartJobThread> thread = new OnStartJobThread(this, pm, pkgs);
    thread->Start();
    *result = TRUE;
    return NOERROR;
}

ECode BackgroundDexOptService::OnStopJob(
    /* [in] */ IJobParameters* params,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Logger::I(TAG, "onIdleStop");
    mIdleTime->Set(FALSE);
    *result = FALSE;
    return NOERROR;
}

} // Pm
} // Server
} // Droid
} // Elastos
