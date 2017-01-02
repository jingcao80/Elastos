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

#include "elastos/droid/launcher2/DeferredHandler.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
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
    {    AutoLock syncLock(mHost->mQueueLock);
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
    {    AutoLock syncLock(mHost->mQueueLock);
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
    looperHelper->GetMyQueue((IMessageQueue**)&mMessageQueue);
    mHandler = new Impl(this);
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
    {    AutoLock syncLock(mQueueLock);
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
    {    AutoLock syncLock(mQueueLock);
        while (mQueue->Remove(TO_IINTERFACE(runnable))) { }
    }
    return NOERROR;
}

ECode DeferredHandler::CancelAllRunnablesOfType(
    /* [in] */ Int32 type)
{
    {    AutoLock syncLock(mQueueLock);
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
    {    AutoLock syncLock(mQueueLock);
        mQueue->Clear();
    }
    return NOERROR;
}

ECode DeferredHandler::Flush()
{
    AutoPtr<ILinkedList> queue;
    CLinkedList::New((ILinkedList**)&queue);
    {    AutoLock syncLock(mQueueLock);
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