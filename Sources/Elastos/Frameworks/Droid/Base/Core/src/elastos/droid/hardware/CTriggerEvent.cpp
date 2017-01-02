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
