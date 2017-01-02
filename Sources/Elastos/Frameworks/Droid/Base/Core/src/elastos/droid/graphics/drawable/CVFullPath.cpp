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

#include "elastos/droid/graphics/drawable/CVFullPath.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CVFullPath)

ECode CVFullPath::GetStrokeColor(
    /* [out] */ Int32* strokeColor)
{
    VALIDATE_NOT_NULL(strokeColor);
    *strokeColor = VectorDrawable::VFullPath::GetStrokeColor();
    return NOERROR;
}

ECode CVFullPath::GetStrokeWidth(
    /* [out] */ Float* strokeWidth)
{
    VALIDATE_NOT_NULL(strokeWidth);
    *strokeWidth = VectorDrawable::VFullPath::GetStrokeWidth();
    return NOERROR;
}

ECode CVFullPath::GetStrokeAlpha(
     /* [out] */ Float* strokeAlpha)
{
    VALIDATE_NOT_NULL(strokeAlpha);
    *strokeAlpha = VectorDrawable::VFullPath::GetStrokeAlpha();
    return NOERROR;
}

ECode CVFullPath::GetFillColor(
    /* [out] */ Int32* fillColor)
{
    VALIDATE_NOT_NULL(fillColor);
    *fillColor = VectorDrawable::VFullPath::GetFillColor();
    return NOERROR;
}

ECode CVFullPath::GetFillAlpha(
    /* [out] */ Float* fillAlpha)
{
    VALIDATE_NOT_NULL(fillAlpha);
    *fillAlpha = VectorDrawable::VFullPath::GetFillAlpha();
    return NOERROR;
}

ECode CVFullPath::GetTrimPathStart(
    /* [out] */ Float* trimPathStart)
{
    VALIDATE_NOT_NULL(trimPathStart);
    *trimPathStart = VectorDrawable::VFullPath::GetTrimPathStart();
    return NOERROR;
}

ECode CVFullPath::GetTrimPathEnd(
    /* [out] */ Float* trimPathEnd)
{
    VALIDATE_NOT_NULL(trimPathEnd);
    *trimPathEnd = VectorDrawable::VFullPath::GetTrimPathEnd();
    return NOERROR;
}

ECode CVFullPath::GetTrimPathOffset(
    /* [out] */ Float* trimPathOffset)
{
    VALIDATE_NOT_NULL(trimPathOffset);
    *trimPathOffset = VectorDrawable::VFullPath::GetTrimPathOffset();
    return NOERROR;
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
