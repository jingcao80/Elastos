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

#ifndef __ELASTOS_DROID_SERVER_MEDIA_CKEYEVENTWAKELOCKRECEIVER_H__
#define __ELASTOS_DROID_SERVER_MEDIA_CKEYEVENTWAKELOCKRECEIVER_H__

#include "_Elastos_Droid_Server_Media_CKeyEventWakeLockReceiver.h"
#include "elastos/droid/os/ResultReceiver.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentOnFinished;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::ResultReceiver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Runnable;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

class MediaSessionService;
class CSessionManagerImpl;

CarClass(CKeyEventWakeLockReceiver)
    , public ResultReceiver
    , public IPendingIntentOnFinished
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ IWeakReference* host);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    CKeyEventWakeLockReceiver()
        : mRefCount(0)
        , mLastTimeoutId(0)
    {}

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IHandler* handler,
        /* [in] */ HANDLE host);

    CARAPI_(void) OnTimeout();

    CARAPI_(void) AquireWakeLockLocked();

    CARAPI Run();

    CARAPI_(void) OnReceiveResult(
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* resultData);

    CARAPI OnSendFinished(
        /* [in] */ IPendingIntent* pendingIntent,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& resultData,
        /* [in] */ IBundle* resultExtras);

private:
    CARAPI_(void) ReleaseWakeLockLocked();

private:
    AutoPtr<IHandler> mHandler;
    Int32 mRefCount;
    Int32 mLastTimeoutId;
    AutoPtr<IRunnable> mRunnable;

    MediaSessionService* mHost;

    friend class CSessionManagerImpl;
};

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_MEDIA_CKEYEVENTWAKELOCKRECEIVER_H__
