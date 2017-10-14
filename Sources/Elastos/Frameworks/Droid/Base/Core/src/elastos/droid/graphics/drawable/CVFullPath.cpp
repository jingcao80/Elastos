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
    return VectorDrawable::VFullPath::GetStrokeColor(strokeColor);
}

ECode CVFullPath::GetStrokeWidth(
    /* [out] */ Float* strokeWidth)
{
    return VectorDrawable::VFullPath::GetStrokeWidth(strokeWidth);
}

ECode CVFullPath::GetStrokeAlpha(
     /* [out] */ Float* strokeAlpha)
{
    return VectorDrawable::VFullPath::GetStrokeAlpha(strokeAlpha);
}

ECode CVFullPath::GetFillColor(
    /* [out] */ Int32* fillColor)
{
    return VectorDrawable::VFullPath::GetFillColor(fillColor);
}

ECode CVFullPath::GetFillAlpha(
    /* [out] */ Float* fillAlpha)
{
    return VectorDrawable::VFullPath::GetFillAlpha(fillAlpha);
}

ECode CVFullPath::GetTrimPathStart(
    /* [out] */ Float* trimPathStart)
{
    return VectorDrawable::VFullPath::GetTrimPathStart(trimPathStart);
}

ECode CVFullPath::GetTrimPathEnd(
    /* [out] */ Float* trimPathEnd)
{
    return VectorDrawable::VFullPath::GetTrimPathEnd(trimPathEnd);
}

ECode CVFullPath::GetTrimPathOffset(
    /* [out] */ Float* trimPathOffset)
{
    return VectorDrawable::VFullPath::GetTrimPathOffset(trimPathOffset);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
