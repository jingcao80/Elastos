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

#ifndef __ELASTOS_DROID_GRAPHICS_CPATHDASHPATHEFFECT_H__
#define __ELASTOS_DROID_GRAPHICS_CPATHDASHPATHEFFECT_H__

#include "_Elastos_Droid_Graphics_CPathDashPathEffect.h"
#include "elastos/droid/graphics/PathEffect.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CPathDashPathEffect)
    , public PathEffect
    , public IPathDashPathEffect
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
    * Dash the drawn path by stamping it with the specified shape. This only
    * applies to drawings when the paint's style is STROKE or STROKE_AND_FILL.
    * If the paint's style is FILL, then this effect is ignored. The paint's
    * strokeWidth does not affect the results.
    * @param shape The path to stamp along
    * @param advance spacing between each stamp of shape
    * @param phase amount to offset before the first shape is stamped
    * @param style how to transform the shape at each position as it is stamped
    */
    CARAPI constructor(
        /* [in] */ IPath* shape,
        /* [in] */ Float advance,
        /* [in] */ Float phase,
        /* [in] */ PathDashPathEffectStyle style);

private:
    static CARAPI_(Int64) NativeCreate(
        /* [in] */ Int64 nativePath,
        /* [in] */ Float advance,
        /* [in] */ Float phase,
        /* [in] */ Int32 nativeStyle);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CPATHDASHPATHEFFECT_H__
