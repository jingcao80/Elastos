
#include "elastos/droid/hardware/camera2/legacy/RequestHandlerThread.h"
#include "elastos/droid/os/ConditionVariable.h"
#include "elastos/droid/os/CHandler.h"
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::EIID_IIdleHandler;
using Elastos::Droid::Os::ConditionVariable;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IMessageQueue;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(RequestHandlerThread::MyHandler, Object, IIdleHandler)

RequestHandlerThread::MyHandler::MyHandler(
    /* [in] */ RequestHandlerThread* host)
    : mHost(host)
{
}

ECode RequestHandlerThread::MyHandler::QueueIdle(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    mHost->mIdle->Open();
    *result = FALSE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(RequestHandlerThread, HandlerThread, IRequestHandlerThread)

RequestHandlerThread::RequestHandlerThread()
{
    mStarted = new ConditionVariable(FALSE);
    mIdle = new ConditionVariable(TRUE);

    mIdleHandler = new MyHandler(this);
}

ECode RequestHandlerThread::constructor()
{
    return NOERROR;
}

ECode RequestHandlerThread::constructor(
    /* [in] */ const String& name,
    /* [in] */ IHandlerCallback* callback)
{
    HandlerThread::constructor(name, Thread::MAX_PRIORITY);
    mCallback = callback;
    return NOERROR;
}

void RequestHandlerThread::OnLooperPrepared()
{
    AutoPtr<ILooper> lopper;
    GetLooper((ILooper**)&lopper);
    CHandler::New(lopper, mCallback, TRUE, (IHandler**)&mHandler);
    mStarted->Open();
}

ECode RequestHandlerThread::WaitUntilStarted()
{
    return mStarted->Block();
}

ECode RequestHandlerThread::GetHandler(
    /* [out] */ IHandler** outhand)
{
    VALIDATE_NOT_NULL(outhand);

    *outhand = mHandler;
    REFCOUNT_ADD(*outhand);
    return NOERROR;
}

ECode RequestHandlerThread::WaitAndGetHandler(
    /* [out] */ IHandler** outhand)
{
    VALIDATE_NOT_NULL(outhand);

    WaitUntilStarted();
    return GetHandler(outhand);
}

ECode RequestHandlerThread::HasAnyMessages(
    /* [in] */ ArrayOf<Int32>* what,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    AutoPtr<ILooper> lopper;
    mHandler->GetLooper((ILooper**)&lopper);
    AutoPtr<IMessageQueue> queue;
    lopper->GetQueue((IMessageQueue**)&queue);
    {    AutoLock syncLock(queue);
        for (Int32 i = 0; i < what->GetLength(); i++) {
            Int32 j = (*what)[i];
            Boolean result;
            mHandler->HasMessages(j, &result);
            if (result) {
                *value = TRUE;
                return NOERROR;
            }

        }
    }
    *value = FALSE;
    return NOERROR;
}

ECode RequestHandlerThread::RemoveMessages(
    /* [in] */ ArrayOf<Int32>* what)
{
    AutoPtr<ILooper> lopper;
    mHandler->GetLooper((ILooper**)&lopper);
    AutoPtr<IMessageQueue> queue;
    lopper->GetQueue((IMessageQueue**)&queue);
    {    AutoLock syncLock(queue);
        for (Int32 i = 0; i < what->GetLength(); i++) {
            Int32 j = (*what)[i];
            mHandler->RemoveMessages(j);
        }
    }
    return NOERROR;
}

ECode RequestHandlerThread::WaitUntilIdle()
{
    AutoPtr<IHandler> handler;
    WaitAndGetHandler((IHandler**)&handler);
    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);

    Boolean result;
    looper->IsIdling(&result);
    if (result) {
        return NOERROR;
    }
    mIdle->Close();
    AutoPtr<IMessageQueue> queue;
    looper->GetQueue((IMessageQueue**)&queue);
    queue->AddIdleHandler(mIdleHandler);
    // Ensure that the idle handler gets run even if the looper already went idle
    handler->SendEmptyMessage(MSG_POKE_IDLE_HANDLER, &result);
    looper->IsIdling(&result);
    if (result) {
        return NOERROR;
    }
    mIdle->Block();
    return NOERROR;
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
