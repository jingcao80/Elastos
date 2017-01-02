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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CMATERINGRECTANGLE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CMATERINGRECTANGLE_H__

#include "_Elastos_Droid_Hardware_Camera2_Params_CMeteringRectangle.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Utility::ISize;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CarClass(CMeteringRectangle)
    , public Object
    , public IMeteringRectangle
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CMeteringRectangle();

    virtual ~CMeteringRectangle();

    CARAPI constructor();

    /**
     * Create a new metering rectangle.
     *
     * @param x coordinate >= 0
     * @param y coordinate >= 0
     * @param width width >= 0
     * @param height height >= 0
     * @param meteringWeight weight between {@value #METERING_WEIGHT_MIN} and
     *        {@value #METERING_WEIGHT_MAX} inclusively
     * @throws IllegalArgumentException if any of the parameters were negative
     */
    CARAPI constructor(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 meteringWeight);

    /**
     * Create a new metering rectangle.
     *
     * <p>The point {@code xy}'s data is copied; the reference is not retained.</p>
     *
     * @param xy a non-{@code null} {@link Point} with both x,y >= 0
     * @param dimensions a non-{@code null} {@link android.util.Size Size} with width, height >= 0
     * @param meteringWeight weight >= 0
     *
     * @throws IllegalArgumentException if any of the parameters were negative
     * @throws NullPointerException if any of the arguments were null
     */
    CARAPI constructor(
        /* [in] */ IPoint* xy,
        /* [in] */ ISize* dimensions,
        /* [in] */ Int32 meteringWeight);

    /**
     * Create a new metering rectangle.
     *
     * <p>The rectangle data is copied; the reference is not retained.</p>
     *
     * @param rect a non-{@code null} rectangle with all x,y,w,h dimensions >= 0
     * @param meteringWeight weight >= 0
     *
     * @throws IllegalArgumentException if any of the parameters were negative
     * @throws NullPointerException if any of the arguments were null
     */
    CARAPI constructor(
        /* [in] */ IRect* rect,
        /* [in] */ Int32 meteringWeight);

    /**
     * Return the X coordinate of the left side of the rectangle.
     *
     * @return x coordinate >= 0
     */
    CARAPI GetX(
        /* [out] */ Int32* value);

    /**
     * Return the Y coordinate of the upper side of the rectangle.
     *
     * @return y coordinate >= 0
     */
    CARAPI GetY(
        /* [out] */ Int32* value);

    /**
     * Return the width of the rectangle.
     *
     * @return width >= 0
     */
    CARAPI GetWidth(
        /* [out] */ Int32* value);

    /**
     * Return the height of the rectangle.
     *
     * @return height >= 0
     */
    CARAPI GetHeight(
        /* [out] */ Int32* value);

    /**
     * Return the metering weight of the rectangle.
     *
     * @return weight >= 0
     */
    CARAPI GetMeteringWeight(
        /* [out] */ Int32* value);

    /**
     * Convenience method to create the upper-left (X,Y) coordinate as a {@link Point}.
     *
     * @return a new {@code (x,y)} {@link Point} with both x,y >= 0
     */
    CARAPI GetUpperLeftPoint(
        /* [out] */ IPoint** outpoint);

    /**
     * Convenience method to create the size from this metering rectangle.
     *
     * <p>This strips away the X,Y,weight from the rectangle.</p>
     *
     * @return a new {@link Size} with non-negative width and height
     */
    CARAPI GetSize(
        /* [out] */ ISize** outsize);

    /**
     * Convenience method to create a {@link Rect} from this metering rectangle.
     *
     * <p>This strips away the weight from the rectangle.</p>
     *
     * @return a new {@link Rect} with non-negative x1, y1, x2, y2
     */
    CARAPI GetRect(
        /* [out] */ IRect** outrect);

    /**
     * {@inheritDoc}
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* value);

    /**
     * {@inheritDoc}
     */
    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* value);

    /**
     * Return the metering rectangle as a string representation
     * {@code "(x:%d, y:%d, w:%d, h:%d, wt:%d)"} where each {@code %d} respectively represents
     * the x, y, width, height, and weight points.
     *
     * @return string representation of the metering rectangle
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    Int32 mX;
    Int32 mY;
    Int32 mWidth;
    Int32 mHeight;
    Int32 mWeight;
};

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CMATERINGRECTANGLE_H__
