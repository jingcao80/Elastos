
#include "elastos/droid/hardware/SensorEvent.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Hardware {

CAR_INTERFACE_IMPL(SensorEvent, Object, ISensorEvent)

SensorEvent::SensorEvent()
    : mAccuracy(0)
    , mTimestamp(0)
{
}

ECode SensorEvent::SetValues(
    /* [in] */ ArrayOf<Float>* values)
{
    mValues = values;
    return NOERROR;
}

ECode SensorEvent::GetValues(
    /* [out, callee] */ ArrayOf<Float>** values)
{
    VALIDATE_NOT_NULL(values);

    *values = mValues;
    REFCOUNT_ADD(*values);
    return NOERROR;
}

ECode SensorEvent::GetSensor(
    /* [out] */ ISensor** sensor)
{
    VALIDATE_NOT_NULL(sensor);

    *sensor = mSensor;
    REFCOUNT_ADD(*sensor);
    return NOERROR;
}

ECode SensorEvent::SetSensor(
    /* [in] */ ISensor* sensor)
{
    mSensor = sensor;
    return NOERROR;
}

ECode SensorEvent::GetAccuracy(
    /* [out] */ Int32* accuracy)
{
    VALIDATE_NOT_NULL(accuracy);

    *accuracy = mAccuracy;
    return NOERROR;
}

ECode SensorEvent::SetAccuracy(
        /* [in] */ Int32 accuracy)
{
    mAccuracy = accuracy;
    return NOERROR;
}

ECode SensorEvent::GetTimestamp(
    /* [out] */ Int64* timestamp)
{
    VALIDATE_NOT_NULL(timestamp);

    *timestamp = mTimestamp;
    return NOERROR;
}

ECode SensorEvent::SetTimestamp(
    /* [in] */ Int64 timestamp)
{
    mTimestamp = timestamp;
    return NOERROR;
}

ECode SensorEvent::constructor(
    /* [in] */ Int32 valueSize)
{
    mValues = ArrayOf<Float>::Alloc(valueSize);
    return NOERROR;
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos
