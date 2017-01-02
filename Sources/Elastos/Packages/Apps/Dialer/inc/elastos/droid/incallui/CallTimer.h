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

#ifndef __ELASTOS_DROID_INCALLUI_CALLTIMER_H__
#define __ELASTOS_DROID_INCALLUI_CALLTIMER_H__

#include <elastos/droid/os/Handler.h>
#include <elastos/core/Runnable.h>

using Elastos::Droid::Os::Handler;
using Elastos::Core::Runnable;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * Helper class used to keep track of events requiring regular intervals.
 */
class CallTimer
    : public Handler
{
private:
    class CallTimerCallback : public Runnable
    {
    public:
        CallTimerCallback(
            /* [in] */ CallTimer* host)
            : mHost(host)
        {}

        // @Override
        CARAPI Run();

    private:
        CallTimer* mHost;
    };

public:
    CallTimer(
        /* [in] */ IRunnable* callback);

    TO_STRING_IMPL("CallTimer")

    CARAPI_(Boolean) Start(
        /* [in] */ Int64 interval);

    CARAPI_(void) Cancel();

private:
    CARAPI_(void) PeriodicUpdateTimer();

private:
    AutoPtr<IRunnable> mInternalCallback;
    AutoPtr<IRunnable> mCallback;
    Int64 mLastReportedTime;
    Int64 mInterval;
    Boolean mRunning;

    friend class CallTimerCallback;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_CALLTIMER_H__
