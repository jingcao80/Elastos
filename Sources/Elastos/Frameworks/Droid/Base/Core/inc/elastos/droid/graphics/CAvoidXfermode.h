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

#ifndef __ELASTOS_DROID_GRAPHICS_CAVOIDXFERMODE_H__
#define __ELASTOS_DROID_GRAPHICS_CAVOIDXFERMODE_H__

#include "_Elastos_Droid_Graphics_CAvoidXfermode.h"
#include "elastos/droid/graphics/Xfermode.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * AvoidXfermode xfermode will draw the src everywhere except on top of the
 * opColor or, depending on the Mode, draw only on top of the opColor.
 */
CarClass(CAvoidXfermode)
    , public Xfermode
    , public IAvoidXfermode
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /** This xfermode draws, or doesn't draw, based on the destination's
     * distance from an op-color.
     *
     * There are two modes, and each mode interprets a tolerance value.
     *
     * Avoid: In this mode, drawing is allowed only on destination pixels that
     * are different from the op-color.
     * Tolerance near 0: avoid any colors even remotely similar to the op-color
     * Tolerance near 255: avoid only colors nearly identical to the op-color
     *
     * Target: In this mode, drawing only occurs on destination pixels that
     * are similar to the op-color
     * Tolerance near 0: draw only on colors that are nearly identical to the op-color
     * Tolerance near 255: draw on any colors even remotely similar to the op-color
     */
    CARAPI constructor(
        /* [in] */ Int32 opColor,
        /* [in] */ Int32 tolerance,
        /* [in] */ Int32 mode);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CAVOIDXFERMODE_H__
