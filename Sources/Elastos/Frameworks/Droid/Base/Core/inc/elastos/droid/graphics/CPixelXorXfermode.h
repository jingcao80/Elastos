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

#ifndef __ELASTOS_DROID_GRAPHICS_CPIXELXORXFERMODE_H__
#define __ELASTOS_DROID_GRAPHICS_CPIXELXORXFERMODE_H__

#include "_Elastos_Droid_Graphics_CPixelXorXfermode.h"
#include "elastos/droid/graphics/Xfermode.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * PixelXorXfermode implements a simple pixel xor (op ^ src ^ dst).
 * This transformation does not follow premultiplied conventions, therefore
 * this mode *always* returns an opaque color (alpha == 255). Thus it is
 * not really usefull for operating on blended colors.
 */
CarClass(CPixelXorXfermode)
    , public Xfermode
    , public IPixelXorXfermode
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int32 opColor);

private:
    static CARAPI_(Int64) NativeCreate(
        /* [in] */ Int32 opColor);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CPIXELXORXFERMODE_H__
