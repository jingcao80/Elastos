
#include "elastos/droid/launcher2/InstallWidgetReceiver.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

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
    System::GetCurrentTimeMillis(&currentTime);
    mAlarmPending = TRUE;
    mAlarmTriggerTime = currentTime + millisecondsInFuture;
    if (!mWaitingForCallback) {
        mHandler->PostDelayed(this, mAlarmTriggerTime - currentTime);
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
        System::GetCurrentTimeMillis(&currentTime);
        if (mAlarmTriggerTime > currentTime) {
            // We still need to wait some time to trigger spring loaded mode--
            // post a new callback
            mHandler->PostDelayed(this, Math::Max(0, mAlarmTriggerTime - currentTime));
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