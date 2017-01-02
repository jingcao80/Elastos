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

#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CVFULLPATH_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CVFULLPATH_H__

#include "elastos/droid/graphics/drawable/VectorDrawable.h"
#include "_Elastos_Droid_Graphics_Drawable_CVFullPath.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CVFullPath)
    , public VectorDrawable::VFullPath
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetStrokeColor(
        /* [out] */ Int32* strokeColor);

    CARAPI GetStrokeWidth(
        /* [out] */ Float* strokeWidth);

    CARAPI GetStrokeAlpha(
         /* [out] */ Float* strokeAlpha);

    CARAPI GetFillColor(
        /* [out] */ Int32* fillColor);

    CARAPI GetFillAlpha(
        /* [out] */ Float* fillAlpha);

    CARAPI GetTrimPathStart(
        /* [out] */ Float* trimPathStart);

    CARAPI GetTrimPathEnd(
        /* [out] */ Float* trimPathEnd);

    CARAPI GetTrimPathOffset(
        /* [out] */ Float* trimPathOffset);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_DRAWABLE_CVFULLPATH_H__
