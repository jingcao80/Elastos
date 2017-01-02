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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_REQUESTHANDLERTHREAD_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_REQUESTHANDLERTHREAD_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/HandlerThread.h"

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIdleHandler;
using Elastos::Droid::Os::HandlerThread;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IConditionVariable;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class RequestHandlerThread
    : public HandlerThread
    , public IRequestHandlerThread
{
private:
    class MyHandler
        : public Object
        , public IIdleHandler
    {
    public:
        CAR_INTERFACE_DECL()

        MyHandler(
            /* [in] */ RequestHandlerThread* host);

        CARAPI QueueIdle(
            /* [out] */ Boolean* result);

    private:
        RequestHandlerThread* mHost;
    };
public:
    CAR_INTERFACE_DECL()

    RequestHandlerThread();

    virtual ~RequestHandlerThread() {}

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IHandlerCallback* callback);

    // Blocks until thread has started
    CARAPI WaitUntilStarted();

    // May return null if the handler is not set up yet.
    CARAPI GetHandler(
        /* [out] */ IHandler** outhand);

    // Blocks until thread has started
    CARAPI WaitAndGetHandler(
        /* [out] */ IHandler** outhand);

    // Atomic multi-type message existence check
    CARAPI HasAnyMessages(
        /* [in] */ ArrayOf<Int32>* what,
        /* [out] */ Boolean* value);

    // Atomic multi-type message remove
    CARAPI RemoveMessages(
        /* [in] */ ArrayOf<Int32>* what);

    // Blocks until thread is idling
    CARAPI WaitUntilIdle();

protected:
    CARAPI_(void) OnLooperPrepared();

private:
    AutoPtr<IConditionVariable> mStarted;
    AutoPtr<IConditionVariable> mIdle;
    AutoPtr<IHandlerCallback> mCallback;
    /*volatile*/ AutoPtr<IHandler> mHandler;

    AutoPtr<IIdleHandler> mIdleHandler;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_REQUESTHANDLERTHREAD_H__
