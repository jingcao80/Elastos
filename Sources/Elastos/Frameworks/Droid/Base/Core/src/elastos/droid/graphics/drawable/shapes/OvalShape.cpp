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

#include "elastos/droid/graphics/drawable/shapes/OvalShape.h"
#include <elastos/core/Math.h>

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

CAR_INTERFACE_IMPL(OvalShape, RectShape, IOvalShape);
OvalShape::OvalShape()
{}

ECode OvalShape::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPaint* paint)
{
    return canvas->DrawOval(Rect().Get(), paint);
}

ECode OvalShape::GetOutline(
    /* [in] */ IOutline* outline)
{
    AutoPtr<IRectF> rect = Rect();
    Float left = 0.f, top = 0.f, right = 0.f, bottom = 0.f;
    rect->Get(&left, &top, &right, &bottom);
    return outline->SetOval((Int32) Elastos::Core::Math::Ceil(left), (Int32) Elastos::Core::Math::Ceil(top),
            (Int32) Elastos::Core::Math::Floor(right), (Int32) Elastos::Core::Math::Floor(bottom));
}

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
