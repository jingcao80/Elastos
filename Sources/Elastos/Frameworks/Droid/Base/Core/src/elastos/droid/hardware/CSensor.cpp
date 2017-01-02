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

#include "elastos/droid/hardware/CSensor.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Os::Build;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Hardware {

const Int32 CSensor::SENSOR_FLAG_WAKE_UP_SENSOR = 1;

const Int32 CSensor::REPORTING_MODE_MASK = 0xE;
const Int32 CSensor::REPORTING_MODE_SHIFT = 1;

static AutoPtr<ArrayOf<Int32> > InitSensorReportingModes()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(26);

    (*array)[0] = 0; // padding because sensor types start at 1
    (*array)[1] = 3; // SENSOR_TYPE_ACCELEROMETER
    (*array)[2] = 3; // SENSOR_TYPE_GEOMAGNETIC_FIELD
    (*array)[3] = 3; // SENSOR_TYPE_ORIENTATION
    (*array)[4] = 3; // SENSOR_TYPE_GYROSCOPE
    (*array)[5] = 3; // SENSOR_TYPE_LIGHT
    (*array)[6] = 3; // SENSOR_TYPE_PRESSURE
    (*array)[7] = 3; // SENSOR_TYPE_TEMPERATURE
    (*array)[8] = 3; // SENSOR_TYPE_PROXIMITY
    (*array)[9] = 3; // SENSOR_TYPE_GRAVITY
    (*array)[10] = 3; // SENSOR_TYPE_LINEAR_ACCELERATION
    (*array)[11] = 5; // SENSOR_TYPE_ROTATION_VECTOR
    (*array)[12] = 3; // SENSOR_TYPE_RELATIVE_HUMIDITY
    (*array)[13] = 3; // SENSOR_TYPE_AMBIENT_TEMPERATURE
    (*array)[14] = 6; // SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED
    (*array)[15] = 4; // SENSOR_TYPE_GAME_ROTATION_VECTOR
    (*array)[16] = 6; // SENSOR_TYPE_GYROSCOPE_UNCALIBRATED
    (*array)[17] = 1; // SENSOR_TYPE_SIGNIFICANT_MOTION
    (*array)[18] = 1; // SENSOR_TYPE_STEP_DETECTOR
    (*array)[19] = 1; // SENSOR_TYPE_STEP_COUNTER
    (*array)[20] = 5; // SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR
    (*array)[21] = 1; // SENSOR_TYPE_HEART_RATE_MONITOR
    (*array)[22] = 1; // SENSOR_TYPE_WAKE_UP_TILT_DETECTOR
    (*array)[23] = 1; // SENSOR_TYPE_WAKE_GESTURE
    (*array)[24] = 1; // SENSOR_TYPE_GLANCE_GESTURE
    (*array)[25] = 1; // SENSOR_TYPE_PICK_UP_GESTURE

    return array;
}

const AutoPtr<ArrayOf<Int32> > CSensor::sSensorReportingModes = InitSensorReportingModes();

CAR_INTERFACE_IMPL(CSensor, Object, ISensor)

CAR_OBJECT_IMPL(CSensor)

CSensor::CSensor()
    : mVersion(0)
    , mHandle(0)
    , mType(0)
    , mMaxRange(0)
    , mResolution(0)
    , mPower(0)
    , mMinDelay(0)
{
}

ECode CSensor::constructor()
{
    return NOERROR;
}

ECode CSensor::GetReportingMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);

    *mode = ((mFlags & REPORTING_MODE_MASK) >> REPORTING_MODE_SHIFT);
    return NOERROR;
}

Int32 CSensor::GetMaxLengthValuesArray(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 sdkLevel)
{
    // RotationVector length has changed to 3 to 5 for API level 18
    // Set it to 3 for backward compatibility.
    if (((CSensor*)sensor)->mType == ISensor::TYPE_ROTATION_VECTOR &&
            sdkLevel <= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        return 3;
    }
    Int32 offset = ((CSensor*)sensor)->mType;
    if (offset >= sSensorReportingModes->GetLength()) {
        // we don't know about this sensor, so this is probably a
        // vendor-defined sensor, in that case, we don't know how many value
        // it has
        // so we return the maximum and assume the app will know.
        // FIXME: sensor HAL should advertise how much data is returned per
        // sensor
        return 16;
    }
    return (*sSensorReportingModes)[offset];
}

ECode CSensor::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode CSensor::GetVendor(
    /* [out] */ String* vendor)
{
    VALIDATE_NOT_NULL(vendor);

    *vendor = mVendor;
    return NOERROR;
}

ECode CSensor::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);

    *type = mType;
    return NOERROR;
}

ECode CSensor::GetVersion(
    /* [out] */ Int32* version)
{
    VALIDATE_NOT_NULL(version);

    *version = mVersion;
    return NOERROR;
}

ECode CSensor::GetMaximumRange(
    /* [out] */ Float* range)
{
    VALIDATE_NOT_NULL(range);

    *range = mMaxRange;
    return NOERROR;
}

ECode CSensor::GetResolution(
    /* [out] */ Float* resolution)
{
    VALIDATE_NOT_NULL(resolution);

    *resolution = mResolution;
    return NOERROR;
}

ECode CSensor::GetPower(
    /* [out] */ Float* power)
{
    VALIDATE_NOT_NULL(power);

    *power = mPower;
    return NOERROR;
}

ECode CSensor::GetMinDelay(
    /* [out] */ Int32* delay)
{
    VALIDATE_NOT_NULL(delay);

    *delay = mMinDelay;
    return NOERROR;
}

ECode CSensor::GetFifoReservedEventCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    *count = mFifoReservedEventCount;
    return NOERROR;
}

ECode CSensor::GetFifoMaxEventCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    *count = mFifoMaxEventCount;
    return NOERROR;
}

ECode CSensor::GetStringType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);

    *type = mStringType;
    return NOERROR;
}

ECode CSensor::GetRequiredPermission(
    /* [out] */ String* permission)
{
    VALIDATE_NOT_NULL(permission);

    *permission = mRequiredPermission;
    return NOERROR;
}

ECode CSensor::GetHandle(
    /* [out] */ Int32* handle)
{
    VALIDATE_NOT_NULL(handle);

    *handle = mHandle;
    return NOERROR;
}

ECode CSensor::GetMaxDelay(
    /* [out] */ Int32* delay)
{
    VALIDATE_NOT_NULL(delay);

    *delay = mMaxDelay;
    return NOERROR;
}

ECode CSensor::IsWakeUpSensor(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result =  (mFlags & SENSOR_FLAG_WAKE_UP_SENSOR) != 0;
    return NOERROR;
}

ECode CSensor::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;

    sb += "{Sensor name=\"";
    sb += mName;
    sb += "\", vendor=\"";
    sb += mVendor;
    sb += "\", version=";
    sb += mVersion;
    sb += ", type=";
    sb += mType;
    sb += ", maxRange=";
    sb += mMaxRange;
    sb += ", resolution=";
    sb += mResolution;
    sb += ", power=";
    sb += mPower;
    sb += ", minDelay=";
    sb += mMinDelay;
    sb += "}";

    return sb.ToString(str);
}

void CSensor::SetRange(
    /* [in] */ Float max,
    /* [in] */ Float res)
{
    mMaxRange = max;
    mResolution = res;
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos