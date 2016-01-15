
#ifndef __ELASTOS_DROID_APP_CALARMCLOCKINFO_H__
#define __ELASTOS_DROID_APP_CALARMCLOCKINFO_H__

#include "_Elastos_Droid_App_CAlarmClockInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace App {

/**
 * An immutable description of an alarm clock.
 *
 * @see AlarmManager#setAlarmClock
 * @see AlarmManager#getNextAlarmClock
 */
CarClass(CAlarmClockInfo)
    , public Object
    , public IAlarmClockInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAlarmClockInfo();

    CARAPI constructor();

    /**
     * Creates a new alarm clock description.
     *
     * @param triggerTime time at which the underlying alarm is triggered in wall time
     *                    milliseconds since the epoch
     * @param showIntent an intent that can be used to show or edit details of
     *                        the alarm clock.
     */
    CARAPI constructor(
        /* [in] */ Int64 triggerTime,
        /* [in] */ IPendingIntent* showIntent);

    /**
     * Returns the time at which the alarm is going to trigger.
     *
     * This value is UTC wall clock time in milliseconds, as returned by
     * {@link System#currentTimeMillis()} for example.
     */
    CARAPI GetTriggerTime(
        /* [out] */ Int64* result);

    CARAPI SetTriggerTime(
        /* [in] */ Int64 value);

    /**
     * Returns an intent intent that can be used to show or edit details of the alarm clock in
     * the application that scheduled it.
     *
     * <p class="note">Beware that any application can retrieve and send this intent,
     * potentially with additional fields filled in. See
     * {@link PendingIntent#send(android.content.Context, int, android.content.Intent)
     * PendingIntent.send()} and {@link android.content.Intent#fillIn Intent.fillIn()}
     * for details.
     */
    CARAPI GetShowIntent(
        /* [out] */ IPendingIntent** intent);

    CARAPI SetShowIntent(
        /* [in] */ IPendingIntent* intent);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

private:
    Int64 mTriggerTime;
    AutoPtr<IPendingIntent> mShowIntent;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CALARMCLOCKINFO_H__