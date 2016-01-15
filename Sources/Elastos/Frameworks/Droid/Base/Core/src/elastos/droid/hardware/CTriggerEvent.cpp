
#include "elastos/droid/hardware/CTriggerEvent.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Hardware {

CAR_INTERFACE_IMPL(CTriggerEvent, Object, ITriggerEvent)

CAR_OBJECT_IMPL(CTriggerEvent)

ECode CTriggerEvent::constructor(
    /* [in] */ Int32 size)
{
    mValues = ArrayOf<Float>::Alloc(size);
    return NOERROR;
}

ECode CTriggerEvent::SetValues(
    /* [in] */ ArrayOf<Float>* values)
{
    mValues = values;
    return NOERROR;
}

ECode CTriggerEvent::SetSensor(
    /* [in] */ ISensor* sensor)
{
    mSensor = sensor;
    return NOERROR;
}

ECode CTriggerEvent::SetTimestamp(
    /* [in] */ Int64 timestamp)
{
    mTimestamp = timestamp;
    return NOERROR;
}

ECode CTriggerEvent::GetValues(
    /* [out, callee] */ ArrayOf<Float>** values)
{
    VALIDATE_NOT_NULL(values);

    *values = mValues;
    REFCOUNT_ADD(*values);
    return NOERROR;
}

ECode CTriggerEvent::GetSensor(
    /* [out] */ ISensor** sensor)
{
    VALIDATE_NOT_NULL(sensor);

    *sensor = mSensor;
    REFCOUNT_ADD(*sensor);
    return NOERROR;
}

ECode CTriggerEvent::GetTimestamp(
    /* [out] */ Int64* timestamp)
{
    VALIDATE_NOT_NULL(timestamp);

    *timestamp = mTimestamp;
    return NOERROR;
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos
