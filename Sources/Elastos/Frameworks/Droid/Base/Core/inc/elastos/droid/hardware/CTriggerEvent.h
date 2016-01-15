
#ifndef __ELASTOS_DROID_HARDWARE_CTRIGGEREVENT_H__
#define __ELASTOS_DROID_HARDWARE_CTRIGGEREVENT_H__

#include "_Elastos_Droid_Hardware_CTriggerEvent.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {

CarClass(CTriggerEvent)
    , public Object
    , public ITriggerEvent
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int32 size);

    CARAPI SetValues(
        /* [in] */ ArrayOf<Float>* values);

    CARAPI SetSensor(
        /* [in] */ ISensor* sensor);

    CARAPI SetTimestamp(
        /* [in] */ Int64 timestamp);

    CARAPI GetValues(
        /* [out, callee] */ ArrayOf<Float>** values);

    CARAPI GetSensor(
        /* [out] */ ISensor** sensor);

    CARAPI GetTimestamp(
        /* [out] */ Int64* timestamp);

public:
    /**
     * <p>
     * The length and contents of the {@link #values values} array depends on
     * which {@link android.hardware.Sensor sensor} type is being monitored (see
     * also {@link SensorEvent} for a definition of the coordinate system used).
     * </p>
     * <h4> {@link Sensor#TYPE_SIGNIFICANT_MOTION} </h4>
     * The value field is of length 1. value[0] = 1.0 when the sensor triggers.
     * 1.0 is the only allowed value.
     */
    AutoPtr<ArrayOf<Float> > mValues;

    /**
     * The sensor that generated this event. See
     * {@link android.hardware.SensorManager SensorManager} for details.
     */
    AutoPtr<ISensor> mSensor;

    /**
     * The time in nanosecond at which the event happened
     */
    Int64 mTimestamp;
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CTRIGGEREVENT_H__
