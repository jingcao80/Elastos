#ifndef  __ELASTOS_DROID_LAUNCHER2_ALARM_H__
#define  __ELASTOS_DROID_LAUNCHER2_ALARM_H__

#include "_Launcher2.h"
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