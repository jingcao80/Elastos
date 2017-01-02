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
