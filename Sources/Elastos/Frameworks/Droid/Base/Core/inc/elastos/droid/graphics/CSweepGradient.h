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

#ifndef __ELASTOS_DROID_GRAPHICS_CSWEEPGRADIENT_H__
#define __ELASTOS_DROID_GRAPHICS_CSWEEPGRADIENT_H__

#include "_Elastos_Droid_Graphics_CSweepGradient.h"
#include "elastos/droid/graphics/Shader.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CSweepGradient)
    , public Shader
    , public ISweepGradient
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CSweepGradient();

    /**
     * A subclass of Shader that draws a sweep gradient around a center point.
     *
     * @param cx       The x-coordinate of the center
     * @param cy       The y-coordinate of the center
     * @param colors   The colors to be distributed between around the center.
     *                 There must be at least 2 colors in the array.
     * @param positions May be NULL. The relative position of
     *                 each corresponding color in the colors array, beginning
     *                 with 0 and ending with 1.0. If the values are not
     *                 monotonic, the drawing may produce unexpected results.
     *                 If positions is NULL, then the colors are automatically
     *                 spaced evenly.
     */
    CARAPI constructor(
        /* [in] */ Float cx,
        /* [in] */ Float cy,
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ ArrayOf<Float>* positions);

    /**
     * A subclass of Shader that draws a sweep gradient around a center point.
     *
     * @param cx       The x-coordinate of the center
     * @param cy       The y-coordinate of the center
     * @param color0   The color to use at the start of the sweep
     * @param color1   The color to use at the end of the sweep
     */
    CARAPI constructor(
        /* [in] */ Float cx,
        /* [in] */ Float cy,
        /* [in] */ Int32 color0,
        /* [in] */ Int32 color1);

protected:
    /**
     * @hide
     */
    // @Override
    CARAPI Copy(
        /* [out] */ IShader** shader);

private:
    static CARAPI_(Int64) NativeCreate1(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ ArrayOf<Float>* positions);

    static CARAPI_(Int64) NativeCreate2(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int32 color0,
        /* [in] */ Int32 color1);

private:
    static const Int32 TYPE_COLORS_AND_POSITIONS;
    static const Int32 TYPE_COLOR_START_AND_COLOR_END;

    /**
     * Type of the LinearGradient: can be either TYPE_COLORS_AND_POSITIONS or
     * TYPE_COLOR_START_AND_COLOR_END.
     */
    Int32 mType;

    Float mCx;
    Float mCy;
    AutoPtr<ArrayOf<Int32> > mColors;
    AutoPtr<ArrayOf<Float> > mPositions;
    Int32 mColor0;
    Int32 mColor1;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CSWEEPGRADIENT_H__
