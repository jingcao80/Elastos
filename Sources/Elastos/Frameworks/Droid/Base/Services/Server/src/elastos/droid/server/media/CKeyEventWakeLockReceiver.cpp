
#include "elastos/droid/server/media/CKeyEventWakeLockReceiver.h"
#include "elastos/droid/server/media/MediaSessionService.h"

using Elastos::Droid::App::EIID_IPendingIntentOnFinished;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

CKeyEventWakeLockReceiver::MyRunnable::MyRunnable(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{}

CKeyEventWakeLockReceiver::MyRunnable::Run()
{
    AutoPtr<IPendingIntentOnFinished> rrObj;
    mWeakHost->Resolve(EIID_IPendingIntentOnFinished, (IInterface**)&rrObj);
    if (rrObj == NULL) {
        return NOERROR;
    }

    CKeyEventWakeLockReceiver* host = (CKeyEventWakeLockReceiver*)rrObj.Get();
    return host->Run();
}

CAR_INTERFACE_IMPL(CKeyEventWakeLockReceiver, ResultReceiver, IPendingIntentOnFinished)

CAR_OBJECT_IMPL(CKeyEventWakeLockReceiver)

ECode CKeyEventWakeLockReceiver::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ Handle64 host)
{
    ResultReceiver::constructor(handler);
    mHandler = handler;
    mHost = (MediaSessionService*)host;

    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    mRunnable = new MyRunnable(wr);
    return NOERROR;
}

void CKeyEventWakeLockReceiver::OnTimeout()
{
    AutoLock lock(mHost->mLock);
    if (mRefCount == 0) {
        // We've already released it, so just return
        return;
    }
    mLastTimeoutId++;
    mRefCount = 0;
    ReleaseWakeLockLocked();
}

void CKeyEventWakeLockReceiver::AquireWakeLockLocked()
{
    if (mRefCount == 0) {
        mHost->mMediaEventWakeLock->AcquireLock();
    }
    mRefCount++;
    mHandler->RemoveCallbacks(mRunnable);
    Boolean result;
    mHandler->PostDelayed(mRunnable, MediaSessionService::WAKELOCK_TIMEOUT, &result);
}

ECode CKeyEventWakeLockReceiver::Run()
{
    OnTimeout();
    return NOERROR;
}

void CKeyEventWakeLockReceiver::OnReceiveResult(
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* resultData)
{
    if (resultCode < mLastTimeoutId) {
        // Ignore results from calls that were before the last
        // timeout, just in case.
        return;
    }
    else {
        AutoLock lock(mHost->mLock);
        if (mRefCount > 0) {
            mRefCount--;
            if (mRefCount == 0) {
                ReleaseWakeLockLocked();
            }
        }
    }
}

void CKeyEventWakeLockReceiver::ReleaseWakeLockLocked()
{
    mHost->mMediaEventWakeLock->ReleaseLock();
    mHandler->RemoveCallbacks(mRunnable);
}

ECode CKeyEventWakeLockReceiver::OnSendFinished(
    /* [in] */ IPendingIntent* pendingIntent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* resultExtras)
{
    OnReceiveResult(resultCode, NULL);
    return NOERROR;
}

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos
