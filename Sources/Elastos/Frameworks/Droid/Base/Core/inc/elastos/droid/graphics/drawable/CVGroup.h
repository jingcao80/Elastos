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

#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CVGROUP_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CVGROUP_H__

#include "elastos/droid/graphics/drawable/VectorDrawable.h"
#include "_Elastos_Droid_Graphics_Drawable_CVGroup.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CVGroup)
    , public VectorDrawable::VGroup
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetRotation(
        /* [out] */ Float* rotation);

    //CARAPI SetRotation(
    //    /* [in] */ Float rotation);

    CARAPI GetPivotX(
        /* [out] */ Float* pivotx);

    //CARAPI SetPivotX(
    //    /* [in] */ Float pivotx);

    CARAPI GetPivotY(
        /* [out] */ Float* pivoty);

    //CARAPI SetPivotY(
    //    /* [in] */ Float pivoty);

    CARAPI GetScaleX(
        /* [out] */ Float* scalex);

    //CARAPI SetScaleX(
    //    /* [in] */ Float scalex);

    CARAPI GetScaleY(
        /* [out] */ Float* scaley);

    //CARAPI SetScaleY(
    //    /* [in] */ Float scaley);

    CARAPI GetTranslateX(
        /* [out] */ Float* translatex);

    //CARAPI SetTranslateX(
    //    /* [in] */ Float translatex);

    CARAPI GetTranslateY(
        /* [out] */ Float* translatey);

    //CARAPI SetTranslateY(
    //    /* [in] */ Float translatey);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_DRAWABLE_CVGROUP_H__
