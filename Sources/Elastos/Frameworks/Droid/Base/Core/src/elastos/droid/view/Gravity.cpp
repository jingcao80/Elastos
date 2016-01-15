
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/Gravity.h"
#include "elastos/droid/graphics/CRect.h"

using Elastos::Droid::Graphics::CRect;

namespace Elastos {
namespace Droid {
namespace View {

ECode Gravity::Apply(
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IRect* container,
    /* [in] */ IRect* outRect)
{
    return Apply(gravity, w, h, container, 0, 0, outRect);
}

ECode Gravity::Apply(
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IRect* container,
    /* [in] */ IRect* outRect,
    /* [in] */ Int32 layoutDirection)
{
    Int32 absGravity = GetAbsoluteGravity(gravity, layoutDirection);
    return Apply(absGravity, w, h, container, 0, 0, outRect);
}

ECode Gravity::Apply(
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IRect* _container,
    /* [in] */ Int32 xAdj,
    /* [in] */ Int32 yAdj,
    /* [in] */ IRect* _outRect)
{
    CRect* container = (CRect*)_container;
    CRect* outRect = (CRect*)_outRect;

    switch (gravity & ((IGravity::AXIS_PULL_BEFORE | IGravity::AXIS_PULL_AFTER)
            << IGravity::AXIS_X_SHIFT)) {
        case 0:
            outRect->mLeft = container->mLeft
                    + ((container->mRight - container->mLeft - w) / 2) + xAdj;
            outRect->mRight = outRect->mLeft + w;
            if ((gravity & (IGravity::AXIS_CLIP << IGravity::AXIS_X_SHIFT))
                    == (IGravity::AXIS_CLIP << IGravity::AXIS_X_SHIFT)) {
                if (outRect->mLeft < container->mLeft) {
                    outRect->mLeft = container->mLeft;
                }
                if (outRect->mRight > container->mRight) {
                    outRect->mRight = container->mRight;
                }
            }
            break;
        case IGravity::AXIS_PULL_BEFORE << IGravity::AXIS_X_SHIFT:
            outRect->mLeft = container->mLeft + xAdj;
            outRect->mRight = outRect->mLeft + w;
            if ((gravity & (IGravity::AXIS_CLIP << IGravity::AXIS_X_SHIFT))
                    == (IGravity::AXIS_CLIP << IGravity::AXIS_X_SHIFT)) {
                if (outRect->mRight > container->mRight) {
                    outRect->mRight = container->mRight;
                }
            }
            break;
        case IGravity::AXIS_PULL_AFTER << IGravity::AXIS_X_SHIFT:
            outRect->mRight = container->mRight - xAdj;
            outRect->mLeft = outRect->mRight - w;
            if ((gravity & (IGravity::AXIS_CLIP << IGravity::AXIS_X_SHIFT))
                    == (IGravity::AXIS_CLIP << IGravity::AXIS_X_SHIFT)) {
                if (outRect->mLeft < container->mLeft) {
                    outRect->mLeft = container->mLeft;
                }
            }
            break;
        default:
            outRect->mLeft = container->mLeft + xAdj;
            outRect->mRight = container->mRight + xAdj;
            break;
    }

    switch (gravity & ((IGravity::AXIS_PULL_BEFORE | IGravity::AXIS_PULL_AFTER)
            << IGravity::AXIS_Y_SHIFT)) {
        case 0:
            outRect->mTop = container->mTop
                    + ((container->mBottom - container->mTop - h)/2) + yAdj;
            outRect->mBottom = outRect->mTop + h;
            if ((gravity & (IGravity::AXIS_CLIP << IGravity::AXIS_Y_SHIFT))
                    == (IGravity::AXIS_CLIP << IGravity::AXIS_Y_SHIFT)) {
                if (outRect->mTop < container->mTop) {
                    outRect->mTop = container->mTop;
                }
                if (outRect->mBottom > container->mBottom) {
                    outRect->mBottom = container->mBottom;
                }
            }
            break;
        case IGravity::AXIS_PULL_BEFORE << IGravity::AXIS_Y_SHIFT:
            outRect->mTop = container->mTop + yAdj;
            outRect->mBottom = outRect->mTop + h;
            if ((gravity & (IGravity::AXIS_CLIP << IGravity::AXIS_Y_SHIFT))
                    == (IGravity::AXIS_CLIP << IGravity::AXIS_Y_SHIFT)) {
                if (outRect->mBottom > container->mBottom) {
                    outRect->mBottom = container->mBottom;
                }
            }
            break;
        case IGravity::AXIS_PULL_AFTER << IGravity::AXIS_Y_SHIFT:
            outRect->mBottom = container->mBottom - yAdj;
            outRect->mTop = outRect->mBottom - h;
            if ((gravity & (IGravity::AXIS_CLIP << IGravity::AXIS_Y_SHIFT))
                    == (IGravity::AXIS_CLIP << IGravity::AXIS_Y_SHIFT)) {
                if (outRect->mTop < container->mTop) {
                    outRect->mTop = container->mTop;
                }
            }
            break;
        default:
            outRect->mTop = container->mTop + yAdj;
            outRect->mBottom = container->mBottom + yAdj;
            break;
    }
    return NOERROR;
}

ECode Gravity::Apply(
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IRect* container,
    /* [in] */ Int32 xAdj,
    /* [in] */ Int32 yAdj,
    /* [in] */ IRect* outRect,
    /* [in] */ Int32 layoutDirection)
{
    Int32 absGravity = GetAbsoluteGravity(gravity, layoutDirection);
    return Apply(absGravity, w, h, container, xAdj, yAdj, outRect);
}

ECode Gravity::ApplyDisplay(
    /* [in] */ Int32 gravity,
    /* [in] */ IRect* _display,
    /* [in] */ IRect* _inoutObj)
{
    CRect* display = (CRect*)_display;
    CRect* inoutObj = (CRect*)_inoutObj;

    if ((gravity & IGravity::DISPLAY_CLIP_VERTICAL) != 0) {
        if (inoutObj->mTop < display->mTop) inoutObj->mTop = display->mTop;
        if (inoutObj->mBottom > display->mBottom) inoutObj->mBottom = display->mBottom;
    }
    else {
        Int32 off = 0;
        if (inoutObj->mTop < display->mTop) off = display->mTop-inoutObj->mTop;
        else if (inoutObj->mBottom > display->mBottom) off = display->mBottom-inoutObj->mBottom;
        if (off != 0) {
            Int32 h;
            inoutObj->GetHeight(&h);
            if (h > (display->mBottom-display->mTop)) {
                inoutObj->mTop = display->mTop;
                inoutObj->mBottom = display->mBottom;
            }
            else {
                inoutObj->mTop += off;
                inoutObj->mBottom += off;
            }
        }
    }

    if ((gravity & IGravity::DISPLAY_CLIP_HORIZONTAL) != 0) {
        if (inoutObj->mLeft < display->mLeft) inoutObj->mLeft = display->mLeft;
        if (inoutObj->mRight > display->mRight) inoutObj->mRight = display->mRight;
    }
    else {
        Int32 off = 0;
        if (inoutObj->mLeft < display->mLeft) off = display->mLeft-inoutObj->mLeft;
        else if (inoutObj->mRight > display->mRight) off = display->mRight-inoutObj->mRight;
        if (off != 0) {
            Int32 w;
            inoutObj->GetWidth(&w);
            if (w > (display->mRight-display->mLeft)) {
                inoutObj->mLeft = display->mLeft;
                inoutObj->mRight = display->mRight;
            }
            else {
                inoutObj->mLeft += off;
                inoutObj->mRight += off;
            }
        }
    }
    return NOERROR;
}

ECode Gravity::ApplyDisplay(
    /* [in] */ Int32 gravity,
    /* [in] */ IRect* display,
    /* [in] */ IRect* inoutObj,
    /* [in] */ Int32 layoutDirection)
{
    Int32 absGravity = GetAbsoluteGravity(gravity, layoutDirection);
    return ApplyDisplay(absGravity, display, inoutObj);
}

Boolean Gravity::IsVertical(
    /* [in] */ Int32 gravity)
{
    return gravity > 0 && (gravity & IGravity::VERTICAL_GRAVITY_MASK) != 0;
}

ECode Gravity::IsVertical(
    /* [in] */ Int32 gravity,
    /* [out] */ Boolean* rst)
{
    *rst = IsVertical(gravity);
    return NOERROR;
}

Boolean Gravity::IsHorizontal(
    /* [in] */ Int32 gravity)
{
    return gravity > 0 && (gravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) != 0;
}

ECode Gravity::IsHorizontal(
    /* [in] */ Int32 gravity,
    /* [out] */ Boolean* rst)
{
    *rst = IsHorizontal(gravity);
    return NOERROR;
}

ECode Gravity::GetAbsoluteGravity(
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 layoutDirection,
    /* [out] */ Int32* rst)
{
    *rst = GetAbsoluteGravity(gravity, layoutDirection);
    return NOERROR;
}

Int32 Gravity::GetAbsoluteGravity(
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 layoutDirection)
{
    Int32 result = gravity;
    // If layout is script specific and gravity is horizontal relative (START or END)
    if ((result & IGravity::RELATIVE_LAYOUT_DIRECTION) > 0) {
        if ((result & IGravity::START) == IGravity::START) {
            // Remove the START bit
            result &= ~IGravity::START;
            if (layoutDirection == IView::LAYOUT_DIRECTION_RTL) {
                // Set the RIGHT bit
                result |= IGravity::RIGHT;
            }
            else {
                // Set the LEFT bit
                result |= IGravity::LEFT;
            }
        }
        else if ((result & IGravity::END) == IGravity::END) {
            // Remove the END bit
            result &= ~IGravity::END;
            if (layoutDirection == IView::LAYOUT_DIRECTION_RTL) {
                // Set the LEFT bit
                result |= IGravity::LEFT;
            }
            else {
                // Set the RIGHT bit
                result |= IGravity::RIGHT;
            }
        }
        // Don't need the script specific bit any more, so remove it as we are converting to
        // absolute values (LEFT or RIGHT)
        result &= ~IGravity::RELATIVE_LAYOUT_DIRECTION;
    }
    return result;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
