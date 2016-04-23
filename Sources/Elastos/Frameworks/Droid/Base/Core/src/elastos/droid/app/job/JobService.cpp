
#include "elastos/droid/app/job/JobService.h"
#include "elastos/droid/os/CMessage.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CMessage;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {
namespace Job {

const String JobService::TAG("JobService");

const Int32 JobService::MSG_EXECUTE_JOB = 0;
const Int32 JobService::MSG_STOP_JOB = 1;
const Int32 JobService::MSG_JOB_FINISHED = 2;

//================================================================================
// JobService::JobHandler:
//================================================================================
JobService::JobHandler::JobHandler(
    /* [in] */ JobService* host)
    : mHost(host)
{
}

//@Override
ECode JobService::JobHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    IJobParameters* params = IJobParameters::Probe(obj);
    Int32 what;
    msg->GetWhat(&what);
    ECode ec = NOERROR;
    switch (what) {
        case JobService::MSG_EXECUTE_JOB: {
            Boolean workOngoing;
            ec = mHost->OnStartJob(params, &workOngoing);
            if (FAILED(ec)) {
                Int32 id;
                params->GetJobId(&id);
                Logger::E("JobService", "Error while executing job: %d", id);
                return E_RUNTIME_EXCEPTION;
            }
            ec = AckStartMessage(params, workOngoing);
            if (FAILED(ec)) {
                Int32 id;
                params->GetJobId(&id);
                Logger::E("JobService", "Error while executing job: %d", id);
                return E_RUNTIME_EXCEPTION;
            }
            break;
        }

        case JobService::MSG_STOP_JOB: {
            Boolean ret;
            ECode ec = mHost->OnStopJob(params, &ret);
            if (FAILED(ec)) {
                Logger::E("JobService", "Application unable to handle onStopJob.");
                return E_RUNTIME_EXCEPTION;
            }
            ec = AckStopMessage(params, ret);
            if (FAILED(ec)) {
                Logger::E("JobService", "Application unable to handle onStopJob.");
                return E_RUNTIME_EXCEPTION;
            }
            break;
        }

        case JobService::MSG_JOB_FINISHED: {
            Int32 arg2;
            msg->GetArg2(&arg2);
            Boolean needsReschedule = (arg2 == 1);
            AutoPtr<IIJobCallback> callback;
            params->GetCallback((IIJobCallback**)&callback);
            if (callback != NULL) {
                Int32 id;
                params->GetJobId(&id);
                callback->JobFinished(id, needsReschedule);
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    Logger::E("JobService", "Error reporting job finish to system: binder has gone away.");
                    return NOERROR;
                }
                return ec;
            } else {
                Logger::E("JobService", "finishJob() called for a nonexistent job id.");
            }
            break;
        }

        default:
            Logger::E("JobService", "Unrecognised message received.");
            break;
    }
    return NOERROR;
}

ECode JobService::JobHandler::AckStartMessage(
    /* [in] */ IJobParameters* params,
    /* [in] */ Boolean workOngoing)
{
    AutoPtr<IIJobCallback> callback;
    params->GetCallback((IIJobCallback**)&callback);
    Int32 jobId;
    params->GetJobId(&jobId);
    if (callback != NULL) {
        // try {
        ECode ec = callback->AcknowledgeStartMessage(jobId, workOngoing);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::E("JobService", "System unreachable for starting job.");
            ec = NOERROR;
        }
        return ec;
    }
    else {
        // if (Log.isLoggable(TAG, Log.DEBUG)) {
            Logger::D("JobService", "Attempting to ack a job that has already been processed.");
        // }
    }
    return NOERROR;
}

ECode JobService::JobHandler::AckStopMessage(
    /* [in] */ IJobParameters* params,
    /* [in] */ Boolean reschedule)
{
    AutoPtr<IIJobCallback> callback;
    params->GetCallback((IIJobCallback**)&callback);
    Int32 jobId;
    params->GetJobId(&jobId);
    if (callback != NULL) {
        // try {
        return callback->AcknowledgeStopMessage(jobId, reschedule);
        // } catch(RemoteException e) {
        //     Logger::E("JobService", "System unreachable for stopping job.");
        // }
    } else {
        // if (Log.isLoggable(TAG, Log.DEBUG)) {
        Logger::D("JobService", "Attempting to ack a job that has already been processed.");
        // }
    }
    return NOERROR;
}

//================================================================================
// JobService
//================================================================================

CAR_INTERFACE_IMPL(JobService, Service, IJobService)

/** @hide */
ECode JobService::EnsureHandler()
{
    synchronized(mHandlerLock) {
        if (mHandler == NULL) {
            AutoPtr<ILooper> looper;
            GetMainLooper((ILooper**)&looper);
            mHandler = new JobHandler(this);
            mHandler->constructor(looper);
        }
    }
    return NOERROR;
}

ECode JobService::OnBind(
    /* [in] */ IIntent* Int32ent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)

    *binder = IBinder::Probe(mBinder);
    REFCOUNT_ADD(*binder)
    return NOERROR;
}

ECode JobService::JobFinished(
    /* [in] */ IJobParameters* params,
    /* [in] */ Boolean needsReschedule)
{
    FAIL_RETURN(EnsureHandler())

    AutoPtr<IMessage> m = CMessage::Obtain(mHandler, MSG_JOB_FINISHED, params);
    m->SetArg2(needsReschedule ? 1 : 0);
    return m->SendToTarget();
}

} // namespace Job
} // namespace App
} // namespace Droid
} // namespace Elastos
