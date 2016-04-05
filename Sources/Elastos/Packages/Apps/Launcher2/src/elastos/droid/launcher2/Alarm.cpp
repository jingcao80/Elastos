
#include "elastos/droid/launcher2/Alarm.h"
#include "Elastos.Droid.Service.h"
#include "R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Os::CHandler;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL_2(Alarm, Object, IAlarm, IRunnable);

Alarm::Alarm()
    : mAlarmTriggerTime(0)
    , mWaitingForCallback(FALSE)
    , mAlarmPending(FALSE)
{
    CHandler::New((IHandler**)&mHandler);
}

ECode Alarm::SetOnAlarmListener(
    /* [in] */ IAlarmOnAlarmListener* alarmListener)
{
    mAlarmListener = alarmListener;
    return NOERROR;
}

ECode Alarm::SetAlarm(
    /* [in] */ Int64 millisecondsInFuture)
{
    Int64 currentTime;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&currentTime);
    mAlarmPending = TRUE;
    mAlarmTriggerTime = currentTime + millisecondsInFuture;
    if (!mWaitingForCallback) {
        Boolean res;
        mHandler->PostDelayed(this, mAlarmTriggerTime - currentTime, &res);
        mWaitingForCallback = TRUE;
    }
    return NOERROR;
}

ECode Alarm::CancelAlarm()
{
    mAlarmTriggerTime = 0;
    mAlarmPending = FALSE;
    return NOERROR;
}

ECode Alarm::Run()
{
    mWaitingForCallback = FALSE;
    if (mAlarmTriggerTime != 0) {
        Int64 currentTime;
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&currentTime);
        if (mAlarmTriggerTime > currentTime) {
            // We still need to wait some time to trigger spring loaded mode--
            // post a new callback
            Boolean res;
            mHandler->PostDelayed(this, Elastos::Core::Math::Max(0l, mAlarmTriggerTime - currentTime), &res);
            mWaitingForCallback = TRUE;
        }
        else {
            mAlarmPending = FALSE;
            if (mAlarmListener != NULL) {
                mAlarmListener->OnAlarm(this);
            }
        }
    }
    return NOERROR;
}

ECode Alarm::AlarmPending(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mAlarmPending;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos