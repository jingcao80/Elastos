
#ifndef __ELASTOS_DROID_HARDWARE_CSENSOR_H__
#define __ELASTOS_DROID_HARDWARE_CSENSOR_H__

#include "_Elastos_Droid_Hardware_CSensor.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {

CarClass(CSensor)
    , public Object
    , public ISensor
{
public:
    CSensor();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Each sensor has exactly one reporting mode associated with it. This method returns the
     * reporting mode constant for this sensor type.
     *
     * @return Reporting mode for the input sensor, one of REPORTING_MODE_* constants.
     * @see #REPORTING_MODE_CONTINUOUS
     * @see #REPORTING_MODE_ON_CHANGE
     * @see #REPORTING_MODE_ONE_SHOT
     * @see #REPORTING_MODE_SPECIAL_TRIGGER
     */
    CARAPI GetReportingMode(
        /* [out] */ Int32* mode);

    static CARAPI_(Int32) GetMaxLengthValuesArray(
        /* [in] */ ISensor* sensor,
        /* [in] */ Int32 sdkLevel);

    /**
     * @return name string of the sensor.
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * @return vendor string of this sensor.
     */
    CARAPI GetVendor(
        /* [out] */ String* vendor);

    /**
     * @return generic type of this sensor.
     */
    CARAPI GetType(
        /* [out] */ Int32* type);

    /**
     * @return version of the sensor's module.
     */
    CARAPI GetVersion(
        /* [out] */ Int32* version);

    /**
     * @return maximum range of the sensor in the sensor's unit.
     */
    CARAPI GetMaximumRange(
        /* [out] */ Float* range);

    /**
     * @return resolution of the sensor in the sensor's unit.
     */
    CARAPI GetResolution(
        /* [out] */ Float* resolution);

    /**
     * @return the power in mA used by this sensor while in use
     */
    CARAPI GetPower(
        /* [out] */ Float* power);

    /**
     * @return the minimum delay allowed between two events in microsecond
     * or zero if this sensor only returns a value when the data it's measuring
     * changes.
     */
    CARAPI GetMinDelay(
        /* [out] */ Int32* delay);

    /**
     * @return Number of events reserved for this sensor in the batch mode FIFO. This gives a
     * guarantee on the minimum number of events that can be batched.
     */
    CARAPI GetFifoReservedEventCount(
        /* [out] */ Int32* count);

    /**
     * @return Maximum number of events of this sensor that could be batched. If this value is zero
     * it indicates that batch mode is not supported for this sensor. If other applications
     * registered to batched sensors, the actual number of events that can be batched might be
     * smaller because the hardware FiFo will be partially used to batch the other sensors.
     */
    CARAPI GetFifoMaxEventCount(
        /* [out] */ Int32* count);

    /**
     * @return The type of this sensor as a string.
     */
    CARAPI GetStringType(
        /* [out] */ String* type);

    /**
     * @hide
     * @return The permission required to access this sensor. If empty, no permission is required.
     */
    CARAPI GetRequiredPermission(
        /* [out] */ String* permission);

    /** @hide */
    CARAPI GetHandle(
        /* [out] */ Int32* handle);

    /**
     * This value is defined only for continuous and on-change sensors. It is the delay between two
     * sensor events corresponding to the lowest frequency that this sensor supports. When lower
     * frequencies are requested through registerListener() the events will be generated at this
     * frequency instead. It can be used to estimate when the batch FIFO may be full. Older devices
     * may set this value to zero. Ignore this value in case it is negative or zero.
     *
     * @return The max delay for this sensor in microseconds.
     */
    CARAPI GetMaxDelay(
        /* [out] */ Int32* delay);

    /**
     * Returns whether this sensor is a wake-up sensor.
     * <p>
     * Wake up sensors wake the application processor up when they have events to deliver. When a
     * wake up sensor is registered to without batching enabled, each event will wake the
     * application processor up.
     * <p>
     * When a wake up sensor is registered to with batching enabled, it
     * wakes the application processor up when maxReportingLatency has elapsed or when the hardware
     * FIFO storing the events from wake up sensors is getting full.
     * <p>
     * Non-wake up sensors never wake the application processor up. Their events are only reported
     * when the application processor is awake, for example because the application holds a wake
     * lock, or another source woke the application processor up.
     * <p>
     * When a non-wake up sensor is registered to without batching enabled, the measurements made
     * while the application processor is asleep might be lost and never returned.
     * <p>
     * When a non-wake up sensor is registered to with batching enabled, the measurements made while
     * the application processor is asleep are stored in the hardware FIFO for non-wake up sensors.
     * When this FIFO gets full, new events start overwriting older events. When the application
     * then wakes up, the latest events are returned, and some old events might be lost. The number
     * of events actually returned depends on the hardware FIFO size, as well as on what other
     * sensors are activated. If losing sensor events is not acceptable during batching, you must
     * use the wake-up version of the sensor.
     * @return true if this is a wake up sensor, false otherwise.
     */
    CARAPI IsWakeUpSensor(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(void) SetRange(
        /* [in] */ Float max,
        /* [in] */ Float res);

private:
    friend class CSensorManagerHelper;
    friend class CSystemSensorManager;
    friend class SystemSensorManager;

    // If this flag is set, the sensor defined as a wake up sensor. This field and REPORTING_MODE_*
    // constants are defined as flags in sensors.h. Modify at both places if needed.
    static const Int32 SENSOR_FLAG_WAKE_UP_SENSOR;

    // Mask for the LSB 2nd, 3rd and fourth bits.
    static const Int32 REPORTING_MODE_MASK;
    static const Int32 REPORTING_MODE_SHIFT;

    // TODO(): The following arrays are fragile and error-prone. This needs to be refactored.

    // Note: This needs to be updated, whenever a new sensor is added.
    // Holds the reporting mode and maximum length of the values array
    // associated with
    // {@link SensorEvent} or {@link TriggerEvent} for the Sensor
    static const AutoPtr<ArrayOf<Int32> > sSensorReportingModes;

    /* Some of these fields are set only by the native bindings in
     * SensorManager.
     */
    String  mName;
    String  mVendor;
    Int32   mVersion;
    Int32   mHandle;
    Int32   mType;
    Float   mMaxRange;
    Float   mResolution;
    Float   mPower;
    Int32   mMinDelay;
    Int32   mFifoReservedEventCount;
    Int32   mFifoMaxEventCount;
    String  mStringType;
    String  mRequiredPermission;
    Int32   mMaxDelay;
    Int32   mFlags;
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CSENSOR_H__
