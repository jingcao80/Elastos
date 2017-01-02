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

#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_RECTSHAPE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_RECTSHAPE_H__

#include "elastos/droid/graphics/drawable/shapes/Shape.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

/**
 * Defines a rectangle shape.
 * The rectangle can be drawn to a Canvas with its own draw() method,
 * but more graphical control is available if you instead pass
 * the RectShape to a {@link android.graphics.drawable.ShapeDrawable}.
 */
class RectShape
    : public Shape
    , public IRectShape
{
public:
    CAR_INTERFACE_DECL()

    /**
     * RectShape constructor.
     */
    RectShape();

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IPaint* paint);

    CARAPI GetOutline(
        /* [in] */ IOutline* outline);

protected:
    //@Override
    CARAPI_(void) OnResize(
        /* [in] */ Float width,
        /* [in] */ Float height);

    /**
     * Returns the RectF that defines this rectangle's bounds.
     */
    CARAPI_(AutoPtr<IRectF>) Rect();

    CARAPI CloneImpl(
        /* [in] */ IRectShape* other);

private:
    AutoPtr<IRectF> mRect;
};

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_RECTSHAPE_H__
