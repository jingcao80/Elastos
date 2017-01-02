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

#include "elastos/droid/hardware/CSensorManagerHelper.h"
#include "elastos/droid/hardware/SensorManager.h"

namespace Elastos {
namespace Droid {
namespace Hardware {

CAR_INTERFACE_IMPL(CSensorManagerHelper, Singleton, ISensorManagerHelper)

CAR_SINGLETON_IMPL(CSensorManagerHelper)

ECode CSensorManagerHelper::GetRotationMatrix(
    /* [in] */ ArrayOf<Float>* R,
    /* [in] */ ArrayOf<Float>* I,
    /* [in] */ ArrayOf<Float>* gravity,
    /* [in] */ ArrayOf<Float>* geomagnetic,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);

    return SensorManager::GetRotationMatrix(R, I, gravity, geomagnetic, state);
}

ECode CSensorManagerHelper::GetInclination(
    /* [in] */ ArrayOf<Float>* I,
    /* [out] */ Float* inclination)
{
    VALIDATE_NOT_NULL(inclination);

    return SensorManager::GetInclination(I, inclination);
}

ECode CSensorManagerHelper::RemapCoordinateSystem(
    /* [in] */ ArrayOf<Float>* inR,
    /* [in] */ Int32 X,
    /* [in] */ Int32 Y,
    /* [in] */ ArrayOf<Float>* outR,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);

    return SensorManager::RemapCoordinateSystem(inR, X, Y, outR, state);
}

ECode CSensorManagerHelper::GetOrientation(
    /* [in] */ ArrayOf<Float>* R,
    /* [in] */ ArrayOf<Float>* values,
    /* [out, callee] */ ArrayOf<Float>** orientation)
{
    VALIDATE_NOT_NULL(orientation);

    return SensorManager::GetOrientation(R, values, orientation);
}

ECode CSensorManagerHelper::GetAltitude(
    /* [in] */ Float p0,
    /* [in] */ Float p,
    /* [out] */ Float* altitude)
{
    VALIDATE_NOT_NULL(altitude);

    return SensorManager::GetAltitude(p0, p, altitude);
}

ECode CSensorManagerHelper::GetAngleChange(
    /* [in] */ ArrayOf<Float>* angleChange,
    /* [in] */ ArrayOf<Float>* R,
    /* [in] */ ArrayOf<Float>* prevR)
{
    return SensorManager::GetAngleChange(angleChange, R, prevR);
}

ECode CSensorManagerHelper::GetRotationMatrixFromVector(
    /* [in] */ ArrayOf<Float>* R,
    /* [in] */ ArrayOf<Float>* rotationVector)
{
    return SensorManager::GetRotationMatrixFromVector(R, rotationVector);
}

ECode CSensorManagerHelper::GetQuaternionFromVector(
    /* [in] */ ArrayOf<Float>* Q,
    /* [in] */ ArrayOf<Float>* rv)
{
    return SensorManager::GetQuaternionFromVector(Q, rv);
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos
