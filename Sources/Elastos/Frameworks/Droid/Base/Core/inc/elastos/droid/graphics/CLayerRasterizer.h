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

#ifndef __ELASTOS_DROID_GRAPHICS_CLAYERRASTERIZER_H__
#define __ELASTOS_DROID_GRAPHICS_CLAYERRASTERIZER_H__

#include "_Elastos_Droid_Graphics_CLayerRasterizer.h"
#include "elastos/droid/graphics/Rasterizer.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CLayerRasterizer)
    , public Rasterizer
    , public ILayerRasterizer
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Add a new layer (above any previous layers) to the rasterizer.
     * The layer will extract those fields that affect the mask from
     * the specified paint, but will not retain a reference to the paint
     * object itself, so it may be reused without danger of side-effects.
     */
    CARAPI AddLayer(
        /* [in] */ IPaint* paint,
        /* [in] */ Float dx,
        /* [in] */ Float dy);

    CARAPI AddLayer(
        /* [in] */ IPaint* paint);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CLAYERRASTERIZER_H__
