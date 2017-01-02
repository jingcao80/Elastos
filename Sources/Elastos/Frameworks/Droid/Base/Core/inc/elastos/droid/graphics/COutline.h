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

#ifndef __ELASTOS_DROID_GRAPHICS_COUTLINE_H__
#define __ELASTOS_DROID_GRAPHICS_COUTLINE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Graphics_COutline.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * Defines a simple shape, used for bounding graphical regions.
 * <p>
 * Can be computed for a View, or computed by a Drawable, to drive the shape of
 * shadows cast by a View, or to clip the contents of the View.
 *
 * @see android.view.ViewOutlineProvider
 * @see android.view.View#setOutlineProvider(android.view.ViewOutlineProvider)
 * @see Drawable#getOutline(Outline)
 */
CarClass(COutline)
    , public Object
    , public IOutline
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    COutline();

    /**
     * Constructs an empty Outline. Call one of the setter methods to make
     * the outline valid for use with a View.
     */
    CARAPI constructor();

    /**
     * Constructs an Outline with a copy of the data in src.
     */
    CARAPI constructor(
        /* [in] */ /*@NonNull*/ IOutline* src);

    /**
     * Sets the outline to be empty.
     *
     * @see #isEmpty()
     */
    CARAPI SetEmpty();

    /**
     * Returns whether the Outline is empty.
     * <p>
     * Outlines are empty when constructed, or if {@link #setEmpty()} is called,
     * until a setter method is called
     *
     * @see #setEmpty()
     */
    CARAPI IsEmpty(
        /* [out] */ Boolean * empty);


    /**
     * Returns whether the outline can be used to clip a View.
     * <p>
     * Currently, only Outlines that can be represented as a rectangle, circle,
     * or round rect support clipping.
     *
     * @see {@link android.view.View#setClipToOutline(boolean)}
     */
    CARAPI CanClip(
        /* [out] */ Boolean* can);

    /**
     * Sets the alpha represented by the Outline - the degree to which the
     * producer is guaranteed to be opaque over the Outline's shape.
     * <p>
     * An alpha value of <code>0.0f</code> either represents completely
     * transparent content, or content that isn't guaranteed to fill the shape
     * it publishes.
     * <p>
     * Content producing a fully opaque (alpha = <code>1.0f</code>) outline is
     * assumed by the drawing system to fully cover content beneath it,
     * meaning content beneath may be optimized away.
     */
    CARAPI SetAlpha(
        /* [in] */ Float alpha);

    /**
     * Returns the alpha represented by the Outline.
     */
    CARAPI GetAlpha(
        /* [out] */ Float* alpha);

    /**
     * Replace the contents of this Outline with the contents of src.
     *
     * @param src Source outline to copy from.
     */
    CARAPI Set(
        /* [in] */ /*@NonNull*/ IOutline* src);

    /**
     * Sets the Outline to the rounded rect defined by the input rect, and
     * corner radius.
     */
    CARAPI SetRect(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /**
     * Convenience for {@link #setRect(int, int, int, int)}
     */
    CARAPI SetRect(
        /* [in] */ /*@NonNull*/ IRect* rect);

    /**
     * Sets the Outline to the rounded rect defined by the input rect, and corner radius.
     * <p>
     * Passing a zero radius is equivalent to calling {@link #setRect(int, int, int, int)}
     */
    CARAPI SetRoundRect(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Float radius);

    /**
     * Convenience for {@link #setRoundRect(int, int, int, int, float)}
     */
    CARAPI SetRoundRect(
        /* [in] */ /*@NonNull*/ IRect* rect,
        /* [in] */ Float radius);

    /**
     * Sets the outline to the oval defined by input rect.
     */
    CARAPI SetOval(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /**
     * Convenience for {@link #setOval(int, int, int, int)}
     */
    CARAPI SetOval(
        /* [in] */ /*@NonNull*/ IRect* rect);

    /**
     * Sets the Constructs an Outline from a
     * {@link android.graphics.Path#isConvex() convex path}.
     */
    CARAPI SetConvexPath(
        /* [in] */ /*@NonNull*/ IPath* convexPath);

public:
    /** @hide */
    AutoPtr<IPath> mPath;

    /** @hide */
    AutoPtr<IRect> mRect;
    /** @hide */
    Float mRadius;
    /** @hide */
    Float mAlpha;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_GRAPHICS_COUTLINE_H__
