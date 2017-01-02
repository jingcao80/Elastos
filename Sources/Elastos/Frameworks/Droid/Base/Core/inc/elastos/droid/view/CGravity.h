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

#ifndef __ELASTOS_DROID_VIEW_CGRAVITY_H__
#define __ELASTOS_DROID_VIEW_CGRAVITY_H__

#include "_Elastos_Droid_View_CGravity.h"
#include "elastos/droid/view/Gravity.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CGravity)
    , public Singleton
    , public IGravity
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Apply a gravity constant to an object. This suppose that the layout direction is LTR.
     *
     * @param gravity The desired placement of the object, as defined by the
     *                constants in this class.
     * @param w The horizontal size of the object.
     * @param h The vertical size of the object.
     * @param container The frame of the containing space, in which the object
     *                  will be placed.  Should be large enough to contain the
     *                  width and height of the object.
     * @param outRect Receives the computed frame of the object in its
     *                container.
     */
    CARAPI Apply(
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ IRect* container,
        /* [in] */ IRect* outRect);

    /**
     * Apply a gravity constant to an object and take care if layout direction is RTL or not.
     *
     * @param gravity The desired placement of the object, as defined by the
     *                constants in this class.
     * @param w The horizontal size of the object.
     * @param h The vertical size of the object.
     * @param container The frame of the containing space, in which the object
     *                  will be placed.  Should be large enough to contain the
     *                  width and height of the object.
     * @param outRect Receives the computed frame of the object in its
     *                container.
     * @param layoutDirection The layout direction.
     *
     * @see View#LAYOUT_DIRECTION_LTR
     * @see View#LAYOUT_DIRECTION_RTL
     */
    CARAPI Apply(
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ IRect* container,
        /* [in] */ IRect* outRect,
        /* [in] */ Int32 layoutDirection);

    /**
     * Apply a gravity constant to an object.
     *
     * @param gravity The desired placement of the object, as defined by the
     *                constants in this class.
     * @param w The horizontal size of the object.
     * @param h The vertical size of the object.
     * @param container The frame of the containing space, in which the object
     *                  will be placed.  Should be large enough to contain the
     *                  width and height of the object.
     * @param xAdj Offset to apply to the X axis.  If gravity is LEFT this
     *             pushes it to the right; if gravity is RIGHT it pushes it to
     *             the left; if gravity is CENTER_HORIZONTAL it pushes it to the
     *             right or left; otherwise it is ignored.
     * @param yAdj Offset to apply to the Y axis.  If gravity is TOP this pushes
     *             it down; if gravity is BOTTOM it pushes it up; if gravity is
     *             CENTER_VERTICAL it pushes it down or up; otherwise it is
     *             ignored.
     * @param outRect Receives the computed frame of the object in its
     *                container.
     */
    CARAPI Apply(
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ IRect* container,
        /* [in] */ Int32 xAdj,
        /* [in] */ Int32 yAdj,
        /* [in] */ IRect* outRect);

    /**
     * Apply a gravity constant to an object.
     *
     * @param gravity The desired placement of the object, as defined by the
     *                constants in this class.
     * @param w The horizontal size of the object.
     * @param h The vertical size of the object.
     * @param container The frame of the containing space, in which the object
     *                  will be placed.  Should be large enough to contain the
     *                  width and height of the object.
     * @param xAdj Offset to apply to the X axis.  If gravity is LEFT this
     *             pushes it to the right; if gravity is RIGHT it pushes it to
     *             the left; if gravity is CENTER_HORIZONTAL it pushes it to the
     *             right or left; otherwise it is ignored.
     * @param yAdj Offset to apply to the Y axis.  If gravity is TOP this pushes
     *             it down; if gravity is BOTTOM it pushes it up; if gravity is
     *             CENTER_VERTICAL it pushes it down or up; otherwise it is
     *             ignored.
     * @param outRect Receives the computed frame of the object in its
     *                container.
     * @param layoutDirection The layout direction.
     *
     * @see View#LAYOUT_DIRECTION_LTR
     * @see View#LAYOUT_DIRECTION_RTL
     */
    CARAPI Apply(
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ IRect* container,
        /* [in] */ Int32 xAdj,
        /* [in] */ Int32 yAdj,
        /* [in] */ IRect* outRect,
        /* [in] */ Int32 layoutDirection);

    /**
     * Apply additional gravity behavior based on the overall "display" that an
     * object exists in.  This can be used after
     * {@link #apply(Int32, Int32, Int32, Rect, Int32, Int32, Rect)} to place the object
     * within a visible display.  By default this moves or clips the object
     * to be visible in the display; the gravity flags
     * {@link #DISPLAY_CLIP_HORIZONTAL} and {@link #DISPLAY_CLIP_VERTICAL}
     * can be used to change this behavior.
     *
     * @param gravity Gravity constants to modify the placement within the
     * display.
     * @param display The rectangle of the display in which the object is
     * being placed.
     * @param inoutObj Supplies the current object position; returns with it
     * modified if needed to fit in the display.
     */
    CARAPI ApplyDisplay(
        /* [in] */ Int32 gravity,
        /* [in] */ IRect* display,
        /* [in] */ IRect* inoutObj);

    /**
     * Apply additional gravity behavior based on the overall "display" that an
     * object exists in.  This can be used after
     * {@link #apply(Int32, Int32, Int32, Rect, Int32, Int32, Rect)} to place the object
     * within a visible display.  By default this moves or clips the object
     * to be visible in the display; the gravity flags
     * {@link #DISPLAY_CLIP_HORIZONTAL} and {@link #DISPLAY_CLIP_VERTICAL}
     * can be used to change this behavior.
     *
     * @param gravity Gravity constants to modify the placement within the
     * display.
     * @param display The rectangle of the display in which the object is
     * being placed.
     * @param inoutObj Supplies the current object position; returns with it
     * modified if needed to fit in the display.
     * @param layoutDirection The layout direction.
     *
     * @see View#LAYOUT_DIRECTION_LTR
     * @see View#LAYOUT_DIRECTION_RTL
     */
    CARAPI ApplyDisplay(
        /* [in] */ Int32 gravity,
        /* [in] */ IRect* display,
        /* [in] */ IRect* inoutObj,
        /* [in] */ Int32 layoutDirection);

    /**
     * <p>Indicate whether the supplied gravity has a vertical pull.</p>
     *
     * @param gravity the gravity to check for vertical pull
     * @return true if the supplied gravity has a vertical pull
     */
    CARAPI IsVertical(
        /* [in] */ Int32 gravity,
        /* [out] */ Boolean* isVertical);

    /**
     * <p>Indicate whether the supplied gravity has an horizontal pull.</p>
     *
     * @param gravity the gravity to check for horizontal pull
     * @return true if the supplied gravity has an horizontal pull
     */
    CARAPI IsHorizontal(
        /* [in] */ Int32 gravity,
        /* [out] */ Boolean* isHorizontal);

    /**
     * <p>Convert script specific gravity to absolute horizontal value.</p>
     *
     * if horizontal direction is LTR, then START will set LEFT and END will set RIGHT.
     * if horizontal direction is RTL, then START will set RIGHT and END will set LEFT.
     *
     *
     * @param gravity The gravity to convert to absolute (horizontal) values.
     * @param layoutDirection The layout direction.
     * @return gravity converted to absolute (horizontal) values.
     */
    CARAPI GetAbsoluteGravity(
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 layoutDirection,
        /* [out] */ Int32* absoluteGravity);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CGRAVITY_H__
