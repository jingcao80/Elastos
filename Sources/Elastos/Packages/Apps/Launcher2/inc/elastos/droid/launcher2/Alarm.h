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

#ifndef  __ELASTOS_DROID_LAUNCHER2_ALARM_H__
#define  __ELASTOS_DROID_LAUNCHER2_ALARM_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class Alarm
    : public Object
    , public IAlarm
    , public IRunnable
{
public:
    CAR_INTERFACE_DECL();

    Alarm();

    CARAPI SetOnAlarmListener(
        /* [in] */ IAlarmOnAlarmListener* alarmListener);

    // Sets the alarm to go off in a certain number of milliseconds. If the alarm is already set,
    // it's overwritten and only the new alarm setting is used
    CARAPI SetAlarm(
        /* [in] */ Int64 millisecondsInFuture);

    CARAPI CancelAlarm();

    // this is called when our timer runs out
    CARAPI Run();

    CARAPI AlarmPending(
        /* [out] */ Boolean* result);

private:
    // if we reach this time and the alarm hasn't been cancelled, call the listener
    Int64 mAlarmTriggerTime;

    // if we've scheduled a call to run() (ie called mHandler.postDelayed), this variable is true.
    // We use this to avoid having multiple pending callbacks
    Boolean mWaitingForCallback;

    AutoPtr<IHandler> mHandler;
    AutoPtr<IAlarmOnAlarmListener> mAlarmListener;
    Boolean mAlarmPending;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_ALARM_H__