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

ECode CKeyEventWakeLockReceiver::MyRunnable::Run()
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
