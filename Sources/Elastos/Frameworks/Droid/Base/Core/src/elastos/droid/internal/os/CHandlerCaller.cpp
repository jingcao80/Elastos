
#include "Elastos.Droid.Content.h"
#include "elastos/droid/internal/os/CHandlerCaller.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/internal/os/SomeArgs.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::Looper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

//==========================================================================
// CHandlerCaller::MyHandler
//==========================================================================
CHandlerCaller::MyHandler::MyHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ Boolean async,
    /* [in] */ IWeakReference* host)
    : Handler(looper, NULL, async)
    , mWeakHost(host)
{
}

ECode CHandlerCaller::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IWeakReferenceSource> wrs;
    mWeakHost->Resolve(EIID_IWeakReferenceSource, (IInterface**)&wrs);
    if (wrs != NULL) {
        CHandlerCaller* hc = (CHandlerCaller*)wrs.Get();
        AutoPtr<IHandlerCallerCallback> cb = hc->GetCallback();
        if (cb) {
            return cb->ExecuteMessage(msg);
        }
    }

    return NOERROR;
}

//==========================================================================
// CHandlerCaller
//==========================================================================
CAR_INTERFACE_IMPL(CHandlerCaller, Object, IHandlerCaller)

CAR_OBJECT_IMPL(CHandlerCaller)

ECode CHandlerCaller::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ILooper* looper,
    /* [in] */ IHandlerCallerCallback* callback,
    /* [in] */ Boolean asyncHandler,
    /* [in] */ Boolean isStrong)
{
    if (looper != NULL)
        mMainLooper = looper;
    else
        context->GetMainLooper((ILooper**)&mMainLooper);
    AutoPtr<IWeakReference> weakReference;
    GetWeakReference((IWeakReference**)&weakReference);
    mH = new MyHandler(mMainLooper, asyncHandler, weakReference);

    if (isStrong) {
        mCallback = callback;
    }
    else if (IWeakReferenceSource::Probe(callback) != NULL) {
        IWeakReferenceSource::Probe(callback)->GetWeakReference((IWeakReference**)&mWeakCallback);
    }
    else {
        assert(0 && "IWeakReferenceSource::Probe(callback) == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CHandlerCaller::ExecuteOrSendMessage(
    /* [in] */ IMessage* msg)
{
    // If we are calling this from the main thread, then we can call
    // right through.  Otherwise, we need to send the message to the
    // main thread.
    AutoPtr<ILooper> myLooper = Looper::GetMyLooper();
    if (myLooper == mMainLooper) {
        ECode ec = NOERROR;
        AutoPtr<IHandlerCallerCallback> cb = GetCallback();
        if (cb) {
            ec = cb->ExecuteMessage(msg);
        }

        msg->Recycle();
        return ec;
    }

    Boolean result;
    return mH->SendMessage(msg, &result);
}

ECode CHandlerCaller::SendMessageDelayed(
    /* [in] */ IMessage* msg,
    /* [in] */ Int64 delayMillis)
{
    Boolean res;
    return mH->SendMessageDelayed(msg, delayMillis, &res);
}

ECode CHandlerCaller::HasMessages(
    /* [in] */ Int32 what,
    /* [out] */ Boolean* result)
{
    return mH->HasMessages(what, result);
}

ECode CHandlerCaller::RemoveMessages(
    /* [in] */ Int32 what)
{
    return mH->RemoveMessages(what);
}

ECode CHandlerCaller::RemoveMessages(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mH->RemoveMessages(what, obj);
}

ECode CHandlerCaller::SendMessage(
    /* [in] */ IMessage* msg)
{
    Boolean result;
    return mH->SendMessage(msg, &result);
}

ECode CHandlerCaller::SendMessageAndWait(
    /* [in] */ IMessage* msg,
    /* [out] */ ISomeArgs** rargs)
{
    AutoPtr<ILooper> looper;
    mH->GetLooper((ILooper**)&looper);
    if (Looper::GetMyLooper() == looper) {
        Logger::E("CHandlerCaller", "Can't wait on same thread as looper");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<SomeArgs> args = (SomeArgs*)ISomeArgs::Probe(obj);
    args->mWaitState = SomeArgs::WAIT_WAITING;
    Boolean res;
    mH->SendMessage(msg, &res);
    {
        AutoLock lock(args);
        while (args->mWaitState == SomeArgs::WAIT_WAITING) {
            if (args->Wait() == (ECode)E_INTERRUPTED_EXCEPTION) {
                *rargs = NULL;
                return NOERROR;
            }
        }
    }
    args->mWaitState = SomeArgs::WAIT_NONE;
    *rargs = args;
    (*rargs)->AddRef();
    return NOERROR;
}

ECode CHandlerCaller::ObtainMessage(
    /* [in] */ Int32 what,
    /* [out] */ IMessage** msg)
{
    return mH->ObtainMessage(what, msg);
}

ECode CHandlerCaller::ObtainMessageBO(
    /* [in] */ Int32 what,
    /* [in] */ Boolean arg1,
    /* [in] */ IInterface* arg2,
    /* [out] */ IMessage** msg)
{
    return mH->ObtainMessage(what, arg1 ? 1 : 0, 0, arg2, msg);
}

ECode CHandlerCaller::ObtainMessageBOO(
    /* [in] */ Int32 what,
    /* [in] */ Boolean arg1,
    /* [in] */ IInterface* arg2,
    /* [in] */ IInterface* arg3,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = arg2;
    args->mArg2 = arg3;

    return mH->ObtainMessage(what, arg1 ? 1 : 0, 0, TO_IINTERFACE(args), msg);
}

ECode CHandlerCaller::ObtainMessageO(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* arg1,
    /* [out] */ IMessage** msg)
{
    return mH->ObtainMessage(what, 0, 0, arg1, msg);
}

ECode CHandlerCaller::ObtainMessageI(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [out] */ IMessage** msg)
{
    return mH->ObtainMessage(what, arg1, 0, NULL, msg);
}

ECode CHandlerCaller::ObtainMessageII(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [out] */ IMessage** msg)
{
    return mH->ObtainMessage(what, arg1, arg2, NULL, msg);
}

ECode CHandlerCaller::ObtainMessageIO(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ IInterface* arg2,
    /* [out] */ IMessage** msg)
{
    return mH->ObtainMessage(what, arg1, 0, arg2, msg);
}

ECode CHandlerCaller::ObtainMessageIIO(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* arg3,
    /* [out] */ IMessage** msg)
{
    return mH->ObtainMessage(what, arg1, arg2, arg3, msg);
}

ECode CHandlerCaller::ObtainMessageIIOO(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* arg3,
    /* [in] */ IInterface* arg4,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = arg3;
    args->mArg2 = arg4;

    return mH->ObtainMessage(what, arg1, arg2, TO_IINTERFACE(args), msg);
}

ECode CHandlerCaller::ObtainMessageIOO(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ IInterface* arg2,
    /* [in] */ IInterface* arg3,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = arg2;
    args->mArg2 = arg3;

    return mH->ObtainMessage(what, arg1, 0, TO_IINTERFACE(args), msg);
}

ECode CHandlerCaller::ObtainMessageIOOO(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ IInterface* arg2,
    /* [in] */ IInterface* arg3,
    /* [in] */ IInterface* arg4,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = arg2;
    args->mArg2 = arg3;
    args->mArg3 = arg4;

    return mH->ObtainMessage(what, arg1, 0, TO_IINTERFACE(args), msg);
}

ECode CHandlerCaller::ObtainMessageOO(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* arg1,
    /* [in] */ IInterface* arg2,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = arg1;
    args->mArg2 = arg2;

    return mH->ObtainMessage(what, 0, 0, TO_IINTERFACE(args), msg);
}

ECode CHandlerCaller::ObtainMessageOOO(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* arg1,
    /* [in] */ IInterface* arg2,
    /* [in] */ IInterface* arg3,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = arg1;
    args->mArg2 = arg2;
    args->mArg3 = arg3;

    return mH->ObtainMessage(what, 0, 0, TO_IINTERFACE(args), msg);
}

ECode CHandlerCaller::ObtainMessageOOOO(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* arg1,
    /* [in] */ IInterface* arg2,
    /* [in] */ IInterface* arg3,
    /* [in] */ IInterface* arg4,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = arg1;
    args->mArg2 = arg2;
    args->mArg3 = arg3;
    args->mArg4 = arg4;

    return mH->ObtainMessage(what, 0, 0, TO_IINTERFACE(args), msg);
}

ECode CHandlerCaller::ObtainMessageOOOOO(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* arg1,
    /* [in] */ IInterface* arg2,
    /* [in] */ IInterface* arg3,
    /* [in] */ IInterface* arg4,
    /* [in] */ IInterface* arg5,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = arg1;
    args->mArg2 = arg2;
    args->mArg3 = arg3;
    args->mArg4 = arg4;
    args->mArg5 = arg5;

    return mH->ObtainMessage(what, 0, 0, TO_IINTERFACE(args), msg);
}

ECode CHandlerCaller::ObtainMessageIIII(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ Int32 arg3,
    /* [in] */ Int32 arg4,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArgi1 = arg1;
    args->mArgi2 = arg2;
    args->mArgi3 = arg3;
    args->mArgi4 = arg4;

    return mH->ObtainMessage(what, 0, 0, TO_IINTERFACE(args), msg);
}

ECode CHandlerCaller::ObtainMessageIIIIII(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ Int32 arg3,
    /* [in] */ Int32 arg4,
    /* [in] */ Int32 arg5,
    /* [in] */ Int32 arg6,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArgi1 = arg1;
    args->mArgi2 = arg2;
    args->mArgi3 = arg3;
    args->mArgi4 = arg4;
    args->mArgi5 = arg5;
    args->mArgi6 = arg6;

    return mH->ObtainMessage(what, 0, 0, TO_IINTERFACE(args), msg);
}

ECode CHandlerCaller::ObtainMessageIIIIO(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ Int32 arg3,
    /* [in] */ Int32 arg4,
    /* [in] */ IInterface* arg5,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArgi1 = arg1;
    args->mArgi2 = arg2;
    args->mArgi3 = arg3;
    args->mArgi4 = arg4;
    args->mArg1 = arg5;

    return mH->ObtainMessage(what, 0, 0, TO_IINTERFACE(args), msg);
}

AutoPtr<IHandlerCallerCallback> CHandlerCaller::GetCallback()
{
    if (mCallback != NULL)
        return mCallback;

    AutoPtr<IHandlerCallerCallback> callback;
    mWeakCallback->Resolve(EIID_IHandlerCallerCallback, (IInterface**)&callback);
    return callback;
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
