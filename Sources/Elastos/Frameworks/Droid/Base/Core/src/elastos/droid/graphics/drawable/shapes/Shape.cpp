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

#include <Elastos.CoreLibrary.Core.h>
#include "elastos/droid/graphics/drawable/shapes/Shape.h"

using Elastos::Core::EIID_ICloneable;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

CAR_INTERFACE_IMPL_2(Shape, Object, IShape, ICloneable)

Shape::Shape()
    : mWidth(0)
    , mHeight(0)
{
}

ECode Shape::GetWidth(
    /* [out] */ Float* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mWidth;
    return NOERROR;
}

ECode Shape::GetHeight(
    /* [out] */ Float* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mHeight;
    return NOERROR;
}

ECode Shape::Resize(
    /* [in] */ Float width,
    /* [in] */ Float height)
{
    if (width < 0) {
        width = 0;
    }
    if (height < 0) {
        height =0;
    }
    if (mWidth != width || mHeight != height) {
        mWidth = width;
        mHeight = height;
        OnResize(width, height);
    }

    return NOERROR;
}

ECode Shape::HasAlpha(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = TRUE;
    return NOERROR;
}

ECode Shape::GetOutline(
    /* [in] */ IOutline* outline)
{
    return NOERROR;
}

void Shape::OnResize(
    /* [in] */ Float width,
    /* [in] */ Float height)
{}

// ECode Shape::Clone(
//     /* [out] */ IInterface** other)
// {
//     VALIDATE_NOT_NULL(other);
//     *other = this;
//     REFCOUNT_ADD(*other);
//     return NOERROR;
// }

ECode Shape::CloneImpl(
    /* [in] */ IShape* other)
{
    ((Shape*)other)->mWidth = mWidth;
    ((Shape*)other)->mHeight = mHeight;
    return NOERROR;
}

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
