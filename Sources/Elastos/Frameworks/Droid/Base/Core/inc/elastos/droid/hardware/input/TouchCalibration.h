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

#ifndef __ELASTOS_DROID_HARDWARE_INPUT_TOUCHCALIBRATION_H__
#define __ELASTOS_DROID_HARDWARE_INPUT_TOUCHCALIBRATION_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Input::IKeyboardLayout;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

/**
 * Describes a keyboard layout.
 *
 * @hide
 */
class TouchCalibration
    : public Object
    , public ITouchCalibration
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Create a new TouchCalibration initialized to the identity transformation.
     */
    TouchCalibration();

    CARAPI constructor();

    /**
     * Create a new TouchCalibration from affine transformation paramters.
     * @param xScale   Influence of input x-axis value on output x-axis value.
     * @param xyMix    Influence of input y-axis value on output x-axis value.
     * @param xOffset  Constant offset to be applied to output x-axis value.
     * @param yXMix    Influence of input x-axis value on output y-axis value.
     * @param yScale   Influence of input y-axis value on output y-axis value.
     * @param yOffset  Constant offset to be applied to output y-axis value.
     */
    CARAPI constructor(
        /* [in] */ Float xScale,
        /* [in] */ Float xyMix,
        /* [in] */ Float xOffset,
        /* [in] */ Float yxMix,
        /* [in] */ Float yScale,
        /* [in] */ Float yOffset);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetAffineTransform(
        /* [out, callee] */ ArrayOf<Float>** transform);

    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* another,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetHashCode(
        /* [out] */ Int32* code);

private:
    static CARAPI_(AutoPtr<ITouchCalibration>) INIT_IDENTITY();

public:
    static const AutoPtr<ITouchCalibration> IDENTITY;

private:
    Float mXScale;
    Float mXYMix;
    Float mXOffset;
    Float mYXMix;
    Float mYScale;
    Float mYOffset;
};

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_INPUT_TOUCHCALIBRATION_H__
