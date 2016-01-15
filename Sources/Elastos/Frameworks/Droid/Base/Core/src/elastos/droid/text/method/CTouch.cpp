#include "elastos/droid/text/method/CTouch.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CTouch)

CAR_INTERFACE_IMPL(CTouch, Singleton, ITouch)

ECode CTouch::ScrollTo(
    /* [in] */ ITextView* widget,
    /* [in] */ ILayout* layout,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    return Touch::ScrollTo(widget, layout, x, y);
}

ECode CTouch::OnTouchEvent(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* ret)
{
    return Touch::OnTouchEvent(widget, buffer, event, ret);
}

ECode CTouch::GetInitialScrollX(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [out] */ Int32* ret)
{
    return Touch::GetInitialScrollX(widget, buffer, ret);
}

ECode CTouch::GetInitialScrollY(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [out] */ Int32* ret)
{
    return Touch::GetInitialScrollY(widget, buffer, ret);
}

ECode CTouch::IsActivelySelecting(
    /* [in] */ ISpannable* buffer,
    /* [out] */ Boolean* ret)
{
    return Touch::IsActivelySelecting(buffer, ret);
}

ECode CTouch::IsSelectionStarted(
    /* [in] */ ISpannable* buffer,
    /* [out] */ Boolean* ret)
{
    return Touch::IsSelectionStarted(buffer, ret);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos