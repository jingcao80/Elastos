/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Utilities for making Minikin work, especially from existing objects like
 * Paint and so on.
 **/

 // TODO: does this really need to be separate from MinikinSkia?

#ifndef __ELASTOS_DROID_GRAPHICS_MINIKINUTILS_H__
#define __ELASTOS_DROID_GRAPHICS_MINIKINUTILS_H__

#include <minikin/Layout.h>
#include "elastos/droid/graphics/NativePaint.h"
#include "elastos/droid/graphics/MinikinSkia.h"
#include "elastos/droid/graphics/TypefaceImpl.h"

using android::Layout;

namespace Elastos {
namespace Droid {
namespace Graphics {

// TODO: these should be defined in Minikin's Layout.h
enum {
    kBidi_LTR = 0,
    kBidi_RTL = 1,
    kBidi_Default_LTR = 2,
    kBidi_Default_RTL = 3,
    kBidi_Force_LTR = 4,
    kBidi_Force_RTL = 5,

    kBidi_Mask = 0x7
};

class MinikinUtils {
public:
    static void doLayout(Layout* layout, const NativePaint* paint, int bidiFlags, TypefaceImpl* typeface,
            const uint16_t* buf, size_t start, size_t count, size_t bufSize);

    static float xOffsetForTextAlign(NativePaint* paint, const Layout& layout);

    static float hOffsetForTextAlign(NativePaint* paint, const Layout& layout, const SkPath& path);
    // f is a functor of type void f(size_t start, size_t end);
    template <typename F>
    static void forFontRun(const Layout& layout, NativePaint* paint, F& f) {
        float saveSkewX = paint->getTextSkewX();
        bool savefakeBold = paint->isFakeBoldText();
        MinikinFont* curFont = NULL;
        size_t start = 0;
        size_t nGlyphs = layout.nGlyphs();
        for (size_t i = 0; i < nGlyphs; i++) {
            MinikinFont* nextFont = layout.getFont(i);
            if (i > 0 && nextFont != curFont) {
                MinikinFontSkia::populateSkPaint(paint, curFont, layout.getFakery(start));
                f(start, i);
                paint->setTextSkewX(saveSkewX);
                paint->setFakeBoldText(savefakeBold);
                start = i;
            }
            curFont = nextFont;
        }
        if (nGlyphs > start) {
            MinikinFontSkia::populateSkPaint(paint, curFont, layout.getFakery(start));
            f(start, nGlyphs);
            paint->setTextSkewX(saveSkewX);
            paint->setFakeBoldText(savefakeBold);
        }
    }
};

} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_GRAPHICS_MINIKINUTILS_H__
