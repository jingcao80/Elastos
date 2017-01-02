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

#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_PAINTDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_PAINTDRAWABLE_H__

#include "elastos/droid/graphics/drawable/ShapeDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

/**
 * Drawable that draws its bounds in the given paint, with optional
 * rounded corners.
*/
class PaintDrawable
    : public ShapeDrawable
    , public IPaintDrawable
{
public:
    CAR_INTERFACE_DECL()

    PaintDrawable();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 color);

    /**
     * Specify radius for the corners of the rectangle. If this is > 0, then the
     * drawable is drawn in a round-rectangle, rather than a rectangle.
     * @param radius the radius for the corners of the rectangle
     */
    virtual CARAPI SetCornerRadius(
        /* [in] */ Float radius);

    /**
     * Specify radii for each of the 4 corners. For each corner, the array
     * contains 2 values, [X_radius, Y_radius]. The corners are ordered
     * top-left, top-right, bottom-right, bottom-left
     * @param radii the x and y radii of the corners
     */
    virtual CARAPI SetCornerRadii(
        /* [in] */ ArrayOf<Float>* radii);

protected:
    //@Override
    CARAPI_(Boolean) InflateTag(
        /* [in] */ const String& name,
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_PAINTDRAWABLE_H__
