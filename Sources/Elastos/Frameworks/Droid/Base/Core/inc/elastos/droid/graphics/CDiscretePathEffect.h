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

#ifndef __ELASTOS_DROID_GRAPHICS_CDISCRETEPATHEFFECT_H__
#define __ELASTOS_DROID_GRAPHICS_CDISCRETEPATHEFFECT_H__

#include "_Elastos_Droid_Graphics_CDiscretePathEffect.h"
#include "elastos/droid/graphics/PathEffect.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CDiscretePathEffect)
    , public PathEffect
    , public IDiscretePathEffect
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Chop the path into lines of segmentLength, randomly deviating from the
     * original path by deviation.
     */
    CARAPI constructor(
        /* [in] */ Float segmentLength,
        /* [in] */ Float deviation);

private:
    static CARAPI_(Int64) NativeCreate(
        /* [in] */ Float length,
        /* [in] */ Float deviation);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CDISCRETEPATHEFFECT_H__
