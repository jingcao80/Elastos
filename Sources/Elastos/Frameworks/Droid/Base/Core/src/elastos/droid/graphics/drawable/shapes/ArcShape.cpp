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

#include "elastos/droid/graphics/drawable/shapes/ArcShape.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

CAR_INTERFACE_IMPL(ArcShape, RectShape, IArcShape);
ArcShape::ArcShape()
    : mStart(0)
    , mSweep(0)
{}

ArcShape::ArcShape(
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle)
    : mStart(startAngle)
    , mSweep(sweepAngle)
{}

ECode ArcShape::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPaint* paint)
{
    return canvas->DrawArc(Rect(), mStart, mSweep, TRUE, paint);
}

ECode ArcShape::constructor(
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle)
{
    mStart = startAngle;
    mSweep = sweepAngle;
    return NOERROR;
}

ECode ArcShape::CloneImpl(
    /* [in] */ IArcShape* other)
{
    RectShape::CloneImpl(IRectShape::Probe(other));
    ((ArcShape*)other)->mStart = mStart;
    ((ArcShape*)other)->mSweep = mSweep;
    return NOERROR;
}

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
