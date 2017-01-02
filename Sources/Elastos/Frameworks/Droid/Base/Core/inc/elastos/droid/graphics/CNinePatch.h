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

#ifndef __ELASTOS_DROID_GRAPHICS_CNINEPATCH_H__
#define __ELASTOS_DROID_GRAPHICS_CNINEPATCH_H__

#include "_Elastos_Droid_Graphics_CNinePatch.h"
#include "elastos/droid/graphics/NinePatch.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * The NinePatch class permits drawing a bitmap in nine sections.
 * The four corners are unscaled; the four edges are scaled in one axis,
 * and the middle is scaled in both axes. Normally, the middle is
 * transparent so that the patch can provide a selection about a rectangle.
 * Essentially, it allows the creation of custom graphics that will scale the
 * way that you define, when content added within the image exceeds the normal
 * bounds of the graphic. For a thorough explanation of a NinePatch image,
 * read the discussion in the
 * <a href="{@docRoot}guide/topics/graphics/2d-graphics.html#nine-patch">2D
 * Graphics</a> document.
 * <p>
 * The <a href="{@docRoot}guide/developing/tools/draw9patch.html">Draw 9-Patch</a>
 * tool offers an extremely handy way to create your NinePatch images,
 * using a WYSIWYG graphics editor.
 * </p>
 */
CarClass(CNinePatch), public NinePatch
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ ArrayOf<Byte>* chunk);

    /**
     * Create a drawable projection from a bitmap to nine patches.
     *
     * @param bitmap    The bitmap describing the patches.
     * @param chunk     The 9-patch data chunk describing how the underlying
     *                  bitmap is split apart and drawn.
     * @param srcName   The name of the source for the bitmap. Might be null.
     */
    CARAPI constructor(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ ArrayOf<Byte>* chunk,
        /* [in] */ const String& srcName);

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ INinePatch* patch);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CNINEPATCH_H__

