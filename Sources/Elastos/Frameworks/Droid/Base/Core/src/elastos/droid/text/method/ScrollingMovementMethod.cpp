
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/text/method/ScrollingMovementMethod.h"
#include "elastos/droid/text/method/CScrollingMovementMethod.h"
#include "elastos/droid/text/method/Touch.h"

using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

AutoPtr<IScrollingMovementMethod> ScrollingMovementMethod::sInstance;

ScrollingMovementMethod::ScrollingMovementMethod()
{}

ScrollingMovementMethod::~ScrollingMovementMethod()
{}

CAR_INTERFACE_IMPL_3(ScrollingMovementMethod, Object, IScrollingMovementMethod, IMovementMethod, IBaseMovementMethod)

ECode ScrollingMovementMethod::constructor()
{
    return NOERROR;
}

Boolean ScrollingMovementMethod::Left(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return ScrollLeft(widget, buffer, 1);
}

Boolean ScrollingMovementMethod::Right(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return ScrollRight(widget, buffer, 1);
}

Boolean ScrollingMovementMethod::Up(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return ScrollUp(widget, buffer, 1);
}

Boolean ScrollingMovementMethod::Down(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return ScrollDown(widget, buffer, 1);
}

Boolean ScrollingMovementMethod::PageUp(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return ScrollPageUp(widget, buffer);
}

Boolean ScrollingMovementMethod::PageDown(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return ScrollPageDown(widget, buffer);
}

Boolean ScrollingMovementMethod::Top(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return ScrollTop(widget, buffer);
}

Boolean ScrollingMovementMethod::Bottom(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return ScrollBottom(widget, buffer);
}

Boolean ScrollingMovementMethod::LineStart(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return ScrollLineStart(widget, buffer);
}

Boolean ScrollingMovementMethod::LineEnd(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return ScrollLineEnd(widget, buffer);
}

Boolean ScrollingMovementMethod::Home(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return Top(widget, buffer);
}

Boolean ScrollingMovementMethod::End(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return Bottom(widget, buffer);
}

ECode ScrollingMovementMethod::OnTouchEvent(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* ret)
{
    Touch::OnTouchEvent(widget, buffer, event, ret);
    return NOERROR;
}

ECode ScrollingMovementMethod::OnTakeFocus(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text,
    /* [in] */ Int32 dir)
{
    AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);

    if (layout != NULL && (dir & IView::FOCUS_FORWARD) != 0) {
        Int32 scrollX, lineTop;
        IView::Probe(widget)->ScrollTo((IView::Probe(widget)->GetScrollX(&scrollX), scrollX), (layout->GetLineTop(0, &lineTop), lineTop));
    }
    if (layout != NULL && (dir & IView::FOCUS_BACKWARD) != 0) {
        Int32 wTotalPaddingTop, wTotalPaddingBottom, lLineCount, wScrollX, lLineTop, wHeight;
        Int32 padding = (widget->GetTotalPaddingTop(&wTotalPaddingTop), wTotalPaddingTop) +
                        (widget->GetTotalPaddingBottom(&wTotalPaddingBottom), wTotalPaddingBottom);
        Int32 line = (layout->GetLineCount(&lLineCount), lLineCount) - 1;

        IView::Probe(widget)->ScrollTo((IView::Probe(widget)->GetScrollX(&wScrollX), wScrollX),
                         (layout->GetLineTop(line + 1, &lLineTop), lLineTop) -
                         ((IView::Probe(widget)->GetHeight(&wHeight), wHeight) - padding));
    }
    return NOERROR;
}

ECode ScrollingMovementMethod::GetInstance(
    /* [out] */ IMovementMethod** ret)
{
    VALIDATE_NOT_NULL(ret)
    if (sInstance == NULL) {
        CScrollingMovementMethod::New((IScrollingMovementMethod**)&sInstance);
    }

    *ret = IMovementMethod::Probe(sInstance);
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
