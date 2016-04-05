
#include "elastos/droid/launcher2/DeferredHandler.h"
#include "Elastos.Droid.Service.h"
#include "R.h"
#include <elastos/core/AutoLock.h>
#include "Elastos.Droid.Utility.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Os::EIID_IIdleHandler;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::CPair;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IList;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::IListIterator;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(DeferredHandler::Impl, Handler, IIdleHandler);

DeferredHandler::Impl::Impl(
    /* [in] */ DeferredHandler* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode DeferredHandler::Impl::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IPair> p;
    AutoPtr<IRunnable> r;
    synchronized(mHost->mQueueLock) {
        Int32 size;
        IList::Probe(mHost->mQueue)->GetSize(&size);
        if (size == 0) {
            return NOERROR;
        }
        AutoPtr<IInterface> object;
        mHost->mQueue->RemoveFirst((IInterface**)&object);
        p = IPair::Probe(object);
        AutoPtr<IInterface> tmp;
        p->GetFirst((IInterface**)&tmp);
        r = IRunnable::Probe(tmp);
    }
    r->Run();
    synchronized(mHost->mQueueLock) {
        mHost->ScheduleNextLocked();
    }
    return NOERROR;
}

ECode DeferredHandler::Impl::QueueIdle(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    HandleMessage(NULL);
    *result = FALSE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(DeferredHandler::IdleRunnable, Runnable, IIdleRunnable);

DeferredHandler::IdleRunnable::IdleRunnable(
    /* [in] */ IRunnable* r)
    : mRunnable(r)
{
}

ECode DeferredHandler::IdleRunnable::Run()
{
    return mRunnable->Run();
}

DeferredHandler::DeferredHandler()
{
    CLinkedList::New((ILinkedList**)&mQueue);
    AutoPtr<ILooperHelper> looperHelper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&looperHelper);
    AutoPtr<IMessageQueue> mMessageQueue;
    looperHelper->GetMyQueue((IMessageQueue**)&mMessageQueue);
    AutoPtr<Impl> mHandler = new Impl(this);
}

ECode DeferredHandler::Post(
    /* [in] */ IRunnable* runnable)
{
    return Post(runnable, 0);
}

ECode DeferredHandler::Post(
    /* [in] */ IRunnable* runnable,
    /* [in] */ Int32 type)
{
    synchronized(mQueueLock) {
        AutoPtr<IInteger32> obj = CoreUtils::Convert(type);
        AutoPtr<IPair> pair;
        CPair::New(TO_IINTERFACE(runnable), TO_IINTERFACE(obj), (IPair**)&pair);
        mQueue->Add(TO_IINTERFACE(pair));
        Int32 size;
        mQueue->GetSize(&size);
        if (size == 1) {
            ScheduleNextLocked();
        }
    }
    return NOERROR;
}

ECode DeferredHandler::PostIdle(
    /* [in] */ IRunnable* runnable)
{
    return PostIdle(runnable, 0);
}

ECode DeferredHandler::PostIdle(
    /* [in] */ IRunnable* runnable,
    /* [in] */ Int32 type)
{
    AutoPtr<IRunnable> run = new IdleRunnable(runnable);
    return Post(run, type);
}

ECode DeferredHandler::CancelRunnable(
    /* [in] */ IRunnable* runnable)
{
    synchronized(mQueueLock) {
        while (mQueue->Remove(TO_IINTERFACE(runnable))) { }
    }
    return NOERROR;
}

ECode DeferredHandler::CancelAllRunnablesOfType(
    /* [in] */ Int32 type)
{
    synchronized(mQueueLock) {
        AutoPtr<IListIterator> iter;
        mQueue->GetListIterator((IListIterator**)&iter);
        AutoPtr<IPair> p;

        Boolean hasNext;
        while (IIterator::Probe(iter)->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            IIterator::Probe(iter)->GetNext((IInterface**)&obj);
            p = IPair::Probe(obj);

            AutoPtr<IInterface> tmp;
            p->GetSecond((IInterface**)&tmp);
            AutoPtr<IInteger32> intObj = IInteger32::Probe(tmp);
            Int32 value;
            intObj->GetValue(&value);
            if (value == type) {
                IIterator::Probe(iter)->Remove();
            }
        }
    }
    return NOERROR;
}

ECode DeferredHandler::Cancel()
{
    synchronized(mQueueLock) {
        mQueue->Clear();
    }
    return NOERROR;
}

ECode DeferredHandler::Flush()
{
    AutoPtr<ILinkedList> queue;
    CLinkedList::New((ILinkedList**)&queue);
    synchronized(mQueueLock) {
        queue->AddAll(ICollection::Probe(mQueue));
        mQueue->Clear();
    }

    Int32 size;
    queue->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        queue->Get(i, (IInterface**)&obj);
        AutoPtr<IPair> p = IPair::Probe(obj);

        AutoPtr<IInterface> tmp;
        p->GetFirst((IInterface**)&tmp);
        AutoPtr<IRunnable> peek = IRunnable::Probe(tmp);
        return peek->Run();
    }
    return NOERROR;
}

ECode DeferredHandler::ScheduleNextLocked()
{
    Int32 size;
    mQueue->GetSize(&size);
    if (size > 0) {
        AutoPtr<IInterface> obj;
        mQueue->GetFirst((IInterface**)&obj);
        AutoPtr<IPair> p = IPair::Probe(obj);

        AutoPtr<IInterface> tmp;
        p->GetFirst((IInterface**)&tmp);
        AutoPtr<IRunnable> peek = IRunnable::Probe(tmp);
        if (IIdleRunnable::Probe(peek) != NULL) {
            return mMessageQueue->AddIdleHandler(mHandler);
        }
        else {
            Boolean res;
            return mHandler->SendEmptyMessage(1, &res);
        }
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos