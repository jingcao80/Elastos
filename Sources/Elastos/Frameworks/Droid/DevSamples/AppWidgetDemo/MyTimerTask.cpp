
#include "MyTimerTask.h"
#include "stdio.h"

using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::EIID_ITimerTask;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AppWidgetDemo {


Int32 MyTimerTask::sCount = 0;

MyTimerTask::MyTimerTask(
    /* [in]  */ CTimeService* service)
    : mService(service)
{

}

UInt32 MyTimerTask::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 MyTimerTask::Release()
{
    return ElRefBase::Release();
}

PInterface MyTimerTask::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IRunnable) {
        return (IRunnable*)this;
    }
    else if (riid == EIID_ITimerTask) {
        return (ITimerTask*)this;
    }

    return NULL;
}

ECode MyTimerTask::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IRunnable*)this) {
        *pIID = EIID_IRunnable;
    }
    else {
        return E_INVALID_ARGUMENT;
    }

    return NOERROR;
}

ECode MyTimerTask::Run()
{
    return mService->UpdateView();
}

ECode MyTimerTask::Cancel(
    /* [out] */ Boolean* value)
{
    return TimerTask::Cancel(value);
}

ECode MyTimerTask::ScheduledExecutionTime(
    /* [out] */ Int64* value)
{
    return TimerTask::ScheduledExecutionTime(value);
}

ECode MyTimerTask::GetWhen(
        /* [out] */ Int64* when)
{
    return TimerTask::GetWhen(when);
}

ECode MyTimerTask::SetWhen(
        /* [in] */ Int64 when)
{
    return TimerTask::SetWhen(when);
}

ECode MyTimerTask::IsScheduled(
        /* [out] */ Boolean* scheduled)
{
    return TimerTask::IsScheduled(scheduled);
}

ECode MyTimerTask::IsCancelled(
        /* [out] */ Boolean* cancelled)
{
    return TimerTask::IsCancelled(cancelled);
}

ECode MyTimerTask::GetPeriod(
        /* [out] */ Int64* period)
{
    return TimerTask::GetPeriod(period);
}

ECode MyTimerTask::SetPeriod(
        /* [in] */ Int64 period)
{
    return TimerTask::SetPeriod(period);
}

ECode MyTimerTask::IsFixedRate(
        /* [out] */ Boolean* fixed)
{
    return TimerTask::IsFixedRate(fixed);
}

ECode MyTimerTask::SetFixedRate(
        /* [in] */ Boolean fixed)
{
    return TimerTask::SetFixedRate(fixed);
}

ECode MyTimerTask::SetScheduledTime(
            /* [in] */ Int64 time)
{
    return TimerTask::SetScheduledTime(time);
}

ECode MyTimerTask::Lock()
{
    mLock.Lock();
    return NOERROR;
}

ECode MyTimerTask::Unlock()
{
    mLock.Unlock();
    return NOERROR;
}

Object* MyTimerTask::GetSelfLock()
{
    return &mLock;
}

} // namespace AppWidgetDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
