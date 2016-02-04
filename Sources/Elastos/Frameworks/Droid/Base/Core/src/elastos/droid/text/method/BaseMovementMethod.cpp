
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/text/method/BaseMovementMethod.h"
#include "elastos/droid/text/method/MetaKeyKeyListener.h"
#include "elastos/droid/text/method/Touch.h"
#include "elastos/droid/view/KeyEvent.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::KeyEvent;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

BaseMovementMethod::BaseMovementMethod()
{}

BaseMovementMethod::~BaseMovementMethod()
{}

ECode BaseMovementMethod::constructor()
{
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(BaseMovementMethod, Object, IBaseMovementMethod, IMovementMethod)

ECode BaseMovementMethod::CanSelectArbitrarily(
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = FALSE;
    return NOERROR;
}

ECode BaseMovementMethod::Initialize(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text)
{
    return NOERROR;
}

ECode BaseMovementMethod::OnKeyDown(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    const Int32 movementMetaState = GetMovementMetaState(text, event);
    Boolean handled = HandleMovementKey(widget, text, keyCode, movementMetaState, event);
    if (handled) {
        MetaKeyKeyListener::AdjustMetaAfterKeypress(text);
        MetaKeyKeyListener::ResetLockedMeta(text);
    }

    *ret = handled;
    return NOERROR;
}

ECode BaseMovementMethod::OnKeyOther(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    const Int32 movementMetaState = GetMovementMetaState(text, event);
    /*const*/ Int32 keyCode;
    event->GetKeyCode(&keyCode);
    Int32 action;
    if (keyCode != IKeyEvent::KEYCODE_UNKNOWN
            && (event->GetAction(&action), action) == IKeyEvent::ACTION_MULTIPLE) {
        /*const*/ Int32 repeat;
        event->GetRepeatCount(&repeat);
        Boolean handled = FALSE;
        for (Int32 i = 0; i < repeat; i++) {
            if (!HandleMovementKey(widget, text, keyCode, movementMetaState, event)) {
                break;
            }
            handled = TRUE;
        }
        if (handled) {
            MetaKeyKeyListener::AdjustMetaAfterKeypress(text);
            MetaKeyKeyListener::ResetLockedMeta(text);
        }
        *ret = handled;
        return NOERROR;
    }

    *ret = FALSE;
    return NOERROR;
}

ECode BaseMovementMethod::OnKeyUp(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = FALSE;
    return NOERROR;
}

ECode BaseMovementMethod::OnTakeFocus(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text,
    /* [in] */ Int32 direction)
{
    return NOERROR;
}

ECode BaseMovementMethod::OnTouchEvent(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = FALSE;
    return NOERROR;
}

ECode BaseMovementMethod::OnTrackballEvent(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = FALSE;
    return NOERROR;
}

ECode BaseMovementMethod::OnGenericMotionEvent(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    Int32 source, action;
    if (((IInputEvent::Probe(event)->GetSource(&source), source) & IInputDevice::SOURCE_CLASS_POINTER) != 0) {
        switch ((event->GetAction(&action), action)) {
            case IMotionEvent::ACTION_SCROLL: {
                /*const*/ Float vscroll;
                /*const*/ Float hscroll;
                Int32 metaState;
                if (((event->GetMetaState(&metaState), metaState) & IKeyEvent::META_SHIFT_ON) != 0) {
                    vscroll = 0;
                    event->GetAxisValue(IMotionEvent::AXIS_VSCROLL, &hscroll);
                }
                else {
                    Float negative;
                    event->GetAxisValue(IMotionEvent::AXIS_VSCROLL, &negative);
                    vscroll=-negative;
                    event->GetAxisValue(IMotionEvent::AXIS_HSCROLL, &hscroll);
                }

                Boolean handled = FALSE;
                if (hscroll < 0) {
                    handled |= ScrollLeft(widget, text, (Int32)Elastos::Core::Math::Ceil(-hscroll));
                }
                else if (hscroll > 0) {
                    handled |= ScrollRight(widget, text, (Int32)Elastos::Core::Math::Ceil(hscroll));
                }
                if (vscroll < 0) {
                    handled |= ScrollUp(widget, text, (Int32)Elastos::Core::Math::Ceil(-vscroll));
                }
                else if (vscroll > 0) {
                    handled |= ScrollDown(widget, text, (Int32)Elastos::Core::Math::Ceil(vscroll));
                }

                *ret = handled;
                return NOERROR;
            }
        }
    }

    *ret = FALSE;
    return NOERROR;
}

Int32 BaseMovementMethod::GetMovementMetaState(
    /* [in] */ ISpannable* buffer,
    /* [in] */ IKeyEvent* event)
{
    // We ignore locked modifiers and SHIFT.
    Int32 mkMetaState;
    MetaKeyKeyListener::GetMetaState(ICharSequence::Probe(buffer), event, &mkMetaState);

    Int32 metaState = mkMetaState
                & ~(IMetaKeyKeyListener::META_ALT_LOCKED | IMetaKeyKeyListener::META_SYM_LOCKED);
    return KeyEvent::NormalizeMetaState(metaState) & ~IKeyEvent::META_SHIFT_MASK;
}

Boolean BaseMovementMethod::HandleMovementKey(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 movementMetaState,
    /* [in] */ IKeyEvent* event)
{
    switch (keyCode) {
        Boolean bHasModifiersCtrlOn, bHasModifiersAltOn;
        case IKeyEvent::KEYCODE_DPAD_LEFT:
            if (KeyEvent::MetaStateHasNoModifiers(movementMetaState)) {
                return Left(widget, buffer);
            }
            else if ((KeyEvent::MetaStateHasModifiers(movementMetaState, IKeyEvent::META_CTRL_ON, &bHasModifiersCtrlOn), bHasModifiersCtrlOn)) {
                return LeftWord(widget, buffer);
            }
            else if ((KeyEvent::MetaStateHasModifiers(movementMetaState, IKeyEvent::META_ALT_ON, &bHasModifiersAltOn), bHasModifiersAltOn)) {
                return LineStart(widget, buffer);
            }
            break;

        case IKeyEvent::KEYCODE_DPAD_RIGHT:
            if (KeyEvent::MetaStateHasNoModifiers(movementMetaState)) {
                return Right(widget, buffer);
            }
            else if ((KeyEvent::MetaStateHasModifiers(movementMetaState, IKeyEvent::META_CTRL_ON, &bHasModifiersCtrlOn), bHasModifiersCtrlOn)) {
                return RightWord(widget, buffer);
            }
            else if ((KeyEvent::MetaStateHasModifiers(movementMetaState, IKeyEvent::META_ALT_ON, &bHasModifiersAltOn), bHasModifiersAltOn)) {
                return LineEnd(widget, buffer);
            }
            break;

        case IKeyEvent::KEYCODE_DPAD_UP:
            if (KeyEvent::MetaStateHasNoModifiers(movementMetaState)) {
                return Up(widget, buffer);
            }
            else if ((KeyEvent::MetaStateHasModifiers(movementMetaState, IKeyEvent::META_ALT_ON, &bHasModifiersAltOn), bHasModifiersAltOn)) {
                return Top(widget, buffer);
            }
            break;

        case IKeyEvent::KEYCODE_DPAD_DOWN:
            if (KeyEvent::MetaStateHasNoModifiers(movementMetaState)) {
                return Down(widget, buffer);
            }
            else if ((KeyEvent::MetaStateHasModifiers(movementMetaState, IKeyEvent::META_ALT_ON, &bHasModifiersAltOn), bHasModifiersAltOn)) {
                return Bottom(widget, buffer);
            }
            break;

        case IKeyEvent::KEYCODE_PAGE_UP:
            if (KeyEvent::MetaStateHasNoModifiers(movementMetaState)) {
                return PageUp(widget, buffer);
            }
            else if ((KeyEvent::MetaStateHasModifiers(movementMetaState, IKeyEvent::META_ALT_ON, &bHasModifiersAltOn), bHasModifiersAltOn)) {
                return Top(widget, buffer);
            }
            break;

        case IKeyEvent::KEYCODE_PAGE_DOWN:
            if (KeyEvent::MetaStateHasNoModifiers(movementMetaState)) {
                return PageDown(widget, buffer);
            }
            else if ((KeyEvent::MetaStateHasModifiers(movementMetaState, IKeyEvent::META_ALT_ON, &bHasModifiersAltOn), bHasModifiersAltOn)) {
                return Bottom(widget, buffer);
            }
            break;

        case IKeyEvent::KEYCODE_MOVE_HOME:
            if (KeyEvent::MetaStateHasNoModifiers(movementMetaState)) {
                return Home(widget, buffer);
            }
            else if ((KeyEvent::MetaStateHasModifiers(movementMetaState, IKeyEvent::META_CTRL_ON, &bHasModifiersCtrlOn), bHasModifiersCtrlOn)) {
                return Top(widget, buffer);
            }
            break;

        case IKeyEvent::KEYCODE_MOVE_END:
            if (KeyEvent::MetaStateHasNoModifiers(movementMetaState)) {
                return End(widget, buffer);
            }
            else if ((KeyEvent::MetaStateHasModifiers(movementMetaState, IKeyEvent::META_CTRL_ON, &bHasModifiersCtrlOn), bHasModifiersCtrlOn)) {
                return Bottom(widget, buffer);
            }
            break;
        }
    return FALSE;
}

Boolean BaseMovementMethod::Left(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return FALSE;
}

Boolean BaseMovementMethod::Right(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return FALSE;
}

Boolean BaseMovementMethod::Up(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return FALSE;
}

Boolean BaseMovementMethod::Down(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return FALSE;
}

Boolean BaseMovementMethod::PageUp(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return FALSE;
}

Boolean BaseMovementMethod::PageDown(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return FALSE;
}

Boolean BaseMovementMethod::Top(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return FALSE;
}

Boolean BaseMovementMethod::Bottom(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return FALSE;
}

Boolean BaseMovementMethod::LineStart(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return FALSE;
}

Boolean BaseMovementMethod::LineEnd(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return FALSE;
}

Boolean BaseMovementMethod::LeftWord(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return FALSE;
}

Boolean BaseMovementMethod::RightWord(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return FALSE;
}

Boolean BaseMovementMethod::Home(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return FALSE;
}

Boolean BaseMovementMethod::End(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return FALSE;
}

Int32 BaseMovementMethod::GetTopLine(
    /* [in] */ ITextView* widget)
{
    //Java:    return widget.getLayout().getLineForVertical(widget.getScrollY());
    Int32 scrollY;
    IView::Probe(widget)->GetScrollY(&scrollY);

    AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    Int32 lineForVertical;
    layout->GetLineForVertical(scrollY, &lineForVertical);
    return lineForVertical;
}

Int32 BaseMovementMethod::GetBottomLine(
    /* [in] */ ITextView* widget)
{
    //Java:    return widget.getLayout().getLineForVertical(widget.getScrollY() + getInnerHeight(widget));
    Int32 scrollY;
    IView::Probe(widget)->GetScrollY(&scrollY);

    AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    Int32 lineForVertical;
    layout->GetLineForVertical(scrollY+GetInnerHeight(widget), &lineForVertical);
    return lineForVertical;
}

Int32 BaseMovementMethod::GetInnerWidth(
    /* [in] */ ITextView* widget)
{
    //Java:    return widget.getWidth() - widget.getTotalPaddingLeft() - widget.getTotalPaddingRight();
    Int32 width, totalPaddingLeft, totalPaddingRight;
    IView::Probe(widget)->GetWidth(&width);
    widget->GetTotalPaddingLeft(&totalPaddingLeft);
    widget->GetTotalPaddingRight(&totalPaddingRight);
    return width-totalPaddingLeft-totalPaddingRight;
}

Int32 BaseMovementMethod::GetInnerHeight(
    /* [in] */ ITextView* widget)
{
    //Java:    return widget.getHeight() - widget.getTotalPaddingTop() - widget.getTotalPaddingBottom();
    Int32 height, totalPaddingTop, totalPaddingBottom;
    IView::Probe(widget)->GetHeight(&height);
    widget->GetTotalPaddingTop(&totalPaddingTop);
    widget->GetTotalPaddingBottom(&totalPaddingBottom);
    return height-totalPaddingTop-totalPaddingBottom;
}

Int32 BaseMovementMethod::GetCharacterWidth(
    /* [in] */ ITextView* widget)
{
    //Java:    return (int) Math.ceil(widget.getPaint().getFontSpacing());
    AutoPtr<ITextPaint> textPaint;
    widget->GetPaint((ITextPaint**)&textPaint);
    Float fFontSpacing;
    IPaint::Probe(textPaint)->GetFontSpacing(&fFontSpacing);
    return (Int32)Elastos::Core::Math::Ceil(fFontSpacing);
}

Int32 BaseMovementMethod::GetScrollBoundsLeft(
    /* [in] */ ITextView* widget)
{
    AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    const Int32 topLine = GetTopLine(widget);
    const Int32 bottomLine = GetBottomLine(widget);
    if (topLine > bottomLine) {
        return 0;
    }
    Int32 left = Elastos::Core::Math::INT32_MAX_VALUE;
    Float flineLeft;
    for (Int32 line = topLine; line <= bottomLine; line++) {
        layout->GetLineLeft(line, &flineLeft);
        const Int32 lineLeft = (Int32) Elastos::Core::Math::Floor(flineLeft);
        if (lineLeft < left) {
            left = lineLeft;
        }
    }
    return left;
}

Int32 BaseMovementMethod::GetScrollBoundsRight(
    /* [in] */ ITextView* widget)
{
    AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    const Int32 topLine = GetTopLine(widget);
    const Int32 bottomLine = GetBottomLine(widget);
    if (topLine > bottomLine) {
        return 0;
    }
    Int32 right = Elastos::Core::Math::INT32_MAX_VALUE;//0x80000000/*Integer.MIN_VALUE*/;
    Float flineRight;
    for (Int32 line = topLine; line <= bottomLine; line++) {
        layout->GetLineRight(line, &flineRight);
        const Int32 lineRight = (Int32) Elastos::Core::Math::Ceil(flineRight);
        if (lineRight > right) {
            right = lineRight;
        }
    }
    return right;
}

Boolean BaseMovementMethod::ScrollLeft(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [in] */ Int32 amount)
{
    const Int32 minScrollX = GetScrollBoundsLeft(widget);
    Int32 scrollX;
    IView::Probe(widget)->GetScrollX(&scrollX);
    if (scrollX > minScrollX) {
        scrollX = Elastos::Core::Math::Max(scrollX - GetCharacterWidth(widget) * amount, minScrollX);
        Int32 scrollY;
        IView::Probe(widget)->GetScrollY(&scrollY);
        IView::Probe(widget)->ScrollTo(scrollX, scrollY);
        return TRUE;
    }
    return FALSE;
}

Boolean BaseMovementMethod::ScrollRight(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [in] */ Int32 amount)
{
    const Int32 maxScrollX = GetScrollBoundsRight(widget) - GetInnerWidth(widget);
    Int32 scrollX;
    IView::Probe(widget)->GetScrollX(&scrollX);
    if (scrollX < maxScrollX) {
        scrollX = Elastos::Core::Math::Min(scrollX + GetCharacterWidth(widget) * amount, maxScrollX);
        Int32 scrollY;
        IView::Probe(widget)->GetScrollY(&scrollY);
        IView::Probe(widget)->ScrollTo(scrollX, scrollY);
        return TRUE;
    }
    return FALSE;
}

Boolean BaseMovementMethod::ScrollUp(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [in] */ Int32 amount)
{
    AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    /*const*/ Int32 top;
    IView::Probe(widget)->GetScrollY(&top);
    Int32 topLine;
    layout->GetLineForVertical(top, &topLine);
    Int32 layoutlineTop;
    if ((layout->GetLineTop(topLine, &layoutlineTop), layoutlineTop) == top) {
        // If the top line is partially visible, bring it all the way
        // into view; otherwise, bring the previous line into view.
        topLine -= 1;
    }
    if (topLine >= 0) {
        topLine = Elastos::Core::Math::Max(topLine - amount + 1, 0);
        Int32 scrollX;
        Touch::ScrollTo(widget, layout, (IView::Probe(widget)->GetScrollX(&scrollX), scrollX), (layout->GetLineTop(topLine, &layoutlineTop), layoutlineTop));
        return TRUE;
    }
    return FALSE;
}

Boolean BaseMovementMethod::ScrollDown(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [in] */ Int32 amount)
{
    AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    const Int32 innerHeight = GetInnerHeight(widget);
    Int32 scrollY;
    IView::Probe(widget)->GetScrollY(&scrollY);
    const Int32 bottom = scrollY + innerHeight;
    Int32 bottomLine;
    layout->GetLineForVertical(bottom, &bottomLine);
    Int32 lineTop;
    if ((layout->GetLineTop(bottomLine + 1, &lineTop), lineTop) < bottom + 1) {
        // Less than a pixel of this line is out of view,
        // so we must have tried to make it entirely in view
        // and now want the next line to be in view instead.
        bottomLine += 1;
    }
    Int32 lineCount;
    layout->GetLineCount(&lineCount);
    const Int32 limit = lineCount - 1;
    if (bottomLine <= limit) {
        bottomLine = Elastos::Core::Math::Min(bottomLine + amount - 1, limit);
        Int32 scrollX;
        IView::Probe(widget)->GetScrollX(&scrollX);
        Touch::ScrollTo(widget, layout, scrollX, (layout->GetLineTop(bottomLine + 1, &lineTop), lineTop) - innerHeight);
        return TRUE;
    }
    return FALSE;
}

Boolean BaseMovementMethod::ScrollPageUp(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    Int32 scrollY;
    IView::Probe(widget)->GetScrollY(&scrollY);
    const Int32 top = scrollY - GetInnerHeight(widget);
    Int32 topLine;
    layout->GetLineForVertical(top, &topLine);
    if (topLine >= 0) {
        Int32 scrollX;
        IView::Probe(widget)->GetScrollX(&scrollX);
        Int32 layoutlineTop;
        layout->GetLineTop(topLine, &layoutlineTop);
        Touch::ScrollTo(widget, layout, scrollX, layoutlineTop);
        return TRUE;
    }
    return FALSE;
}

Boolean BaseMovementMethod::ScrollPageDown(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    const Int32 innerHeight = GetInnerHeight(widget);
    Int32 scrollY;
    IView::Probe(widget)->GetScrollY(&scrollY);
    const Int32 bottom = scrollY + innerHeight + innerHeight;
    Int32 bottomLine;
    layout->GetLineForVertical(bottom, &bottomLine);
    Int32 lineCount;
    if (bottomLine <= (layout->GetLineCount(&lineCount), lineCount) - 1) {
        Int32 scrollX;
        IView::Probe(widget)->GetScrollX(&scrollX);
        Int32 layoutLineTop;
        layout->GetLineTop(bottomLine + 1, &layoutLineTop);
        Touch::ScrollTo(widget, layout, scrollX, layoutLineTop - innerHeight);
        return TRUE;
    }
    return FALSE;
}

Boolean BaseMovementMethod::ScrollTop(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    if (GetTopLine(widget) >= 0) {
        Int32 scrollX;
        IView::Probe(widget)->GetScrollX(&scrollX);
        Int32 layoutLineTop;
        layout->GetLineTop(0, &layoutLineTop);
        Touch::ScrollTo(widget, layout, scrollX, layoutLineTop);
        return TRUE;
    }
    return FALSE;
}

Boolean BaseMovementMethod::ScrollBottom(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    /*const*/ Int32 lineCount;
    layout->GetLineCount(&lineCount);
    if (GetBottomLine(widget) <= lineCount - 1) {
        Int32 scrollX;
        IView::Probe(widget)->GetScrollX(&scrollX);
        Int32 layoutLineTop;
        layout->GetLineTop(lineCount, &layoutLineTop);
        Touch::ScrollTo(widget, layout, scrollX, layoutLineTop - GetInnerHeight(widget));
        return TRUE;
    }
    return FALSE;
}

Boolean BaseMovementMethod::ScrollLineStart(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    const Int32 minScrollX = GetScrollBoundsLeft(widget);
    Int32 scrollX;
    IView::Probe(widget)->GetScrollX(&scrollX);
    if (scrollX > minScrollX) {
        Int32 scrollY;
        IView::Probe(widget)->GetScrollY(&scrollY);
        IView::Probe(widget)->ScrollTo(minScrollX, scrollY);
        return TRUE;
    }
    return FALSE;
}

Boolean BaseMovementMethod::ScrollLineEnd(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    const Int32 maxScrollX = GetScrollBoundsRight(widget) - GetInnerWidth(widget);
    Int32 scrollX;
    IView::Probe(widget)->GetScrollX(&scrollX);
    if (scrollX < maxScrollX) {
        Int32 scrollY;
        IView::Probe(widget)->GetScrollY(&scrollY);
        IView::Probe(widget)->ScrollTo(maxScrollX, scrollY);
        return TRUE;
    }
    return FALSE;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
