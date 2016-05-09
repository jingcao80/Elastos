
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/text/method/ArrowKeyMovementMethod.h"
#include "elastos/droid/text/method/CArrowKeyMovementMethod.h"
#include "elastos/droid/text/method/MetaKeyKeyListener.h"
#include "elastos/droid/text/method/CWordIterator.h"
#include "elastos/droid/text/method/Touch.h"
#include "elastos/droid/text/Selection.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/view/KeyEvent.h"

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Text::EIID_ISelectionPositionIterator;
using Elastos::Droid::Text::ISelectionPositionIterator;
using Elastos::Droid::View::KeyEvent;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IViewParent;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

const AutoPtr<IInterface> ArrowKeyMovementMethod::LAST_TAP_DOWN = (IObject*)new Object();

AutoPtr<IArrowKeyMovementMethod> ArrowKeyMovementMethod::sInstance;

CAR_INTERFACE_IMPL(ArrowKeyMovementMethod, BaseMovementMethod, IArrowKeyMovementMethod)

ArrowKeyMovementMethod::~ArrowKeyMovementMethod()
{}

ECode ArrowKeyMovementMethod::constructor()
{
    return NOERROR;
}

Boolean ArrowKeyMovementMethod::IsSelecting(
    /* [in] */ ISpannable* buffer)
{
    Int32 n1, n2;
    return (MetaKeyKeyListener::GetMetaState(ICharSequence::Probe(buffer), IMetaKeyKeyListener::META_SHIFT_ON == 1, &n1), n1) ||
            (MetaKeyKeyListener::GetMetaState(ICharSequence::Probe(buffer), IMetaKeyKeyListener::META_SELECTING != 0, &n2), n2);
}

Int32 ArrowKeyMovementMethod::GetCurrentLineTop(
    /* [in] */ ISpannable* buffer,
    /* [in] */ ILayout* layout)
{
    //Java:    return layout.getLineTop(layout.getLineForOffset(Selection.getSelectionEnd(buffer)));
    Int32 selectingEnd;
    selectingEnd = Selection::GetSelectionEnd(ICharSequence::Probe(buffer));
    Int32 lineForOffset;
    layout->GetLineForOffset(selectingEnd, &lineForOffset);
    Int32 lineTop;
    layout->GetLineTop(lineForOffset, &lineTop);
    return lineTop;
}

Int32 ArrowKeyMovementMethod::GetPageHeight(
    /* [in] */ ITextView* widget)
{
    // This calculation does not take into account the view transformations that
    // may have been applied to the child or its containers.  In case of scaling or
    // rotation, the calculated page height may be incorrect.
    AutoPtr<IRect> rect;
    CRect::New((IRect**)&rect);
    Boolean bGlobalVisibleRect;
    IView::Probe(widget)->GetGlobalVisibleRect(rect, &bGlobalVisibleRect);
    Int32 height;
    return bGlobalVisibleRect ? (rect->GetHeight(&height), height) : 0;
}

Boolean ArrowKeyMovementMethod::HandleMovementKey(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 movementMetaState,
    /* [in] */ IKeyEvent* event)
{
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_CENTER:
            if (KeyEvent::MetaStateHasNoModifiers(movementMetaState)) {
                Int32 action, repeatCount, ret;
                if ((event->GetAction(&action), action == IKeyEvent::ACTION_DOWN)
                    && (event->GetRepeatCount(&repeatCount), repeatCount == 0)
                    && (MetaKeyKeyListener::GetMetaState(ICharSequence::Probe(buffer), IMetaKeyKeyListener::META_SELECTING, &ret), ret != 0)) {
                    Boolean bShowContextMenu;
                    return (IView::Probe(widget)->ShowContextMenu(&bShowContextMenu), bShowContextMenu);
                }
            }
        break;
    }
    return BaseMovementMethod::HandleMovementKey(widget, buffer, keyCode, movementMetaState, event);
}

Boolean ArrowKeyMovementMethod::Left(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    /*const*/ AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    if (IsSelecting(buffer)) {
        return Selection::ExtendLeft(buffer, layout);
    }
    else {
        return Selection::MoveLeft(buffer, layout);
    }
}

Boolean ArrowKeyMovementMethod::Right(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    /*const*/ AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    if (IsSelecting(buffer)) {
        return Selection::ExtendRight(buffer, layout);
    }
    else {
        return Selection::MoveRight(buffer, layout);
    }
}

Boolean ArrowKeyMovementMethod::Up(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    /*const*/ AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    if (IsSelecting(buffer)) {
        return Selection::ExtendUp(buffer, layout);
    }
    else {
        return Selection::MoveUp(buffer, layout);
    }
}

Boolean ArrowKeyMovementMethod::Down(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    /*const*/ AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    if (IsSelecting(buffer)) {
        return Selection::ExtendDown(buffer, layout);
    }
    else {
        return Selection::MoveDown(buffer, layout);
    }
}

Boolean ArrowKeyMovementMethod::PageUp(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    /*const*/ AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    const Boolean selecting = IsSelecting(buffer);
    const Int32 targetY = GetCurrentLineTop(buffer, layout) - GetPageHeight(widget);
    Boolean handled = FALSE;
    while(TRUE) {
        const Int32 previousSelectionEnd = Selection::GetSelectionEnd(ICharSequence::Probe(buffer));
        if (selecting) {
            Selection::ExtendUp(buffer, layout);
        }
        else {
            Selection::MoveUp(buffer, layout);
        }
        if (Selection::GetSelectionEnd(ICharSequence::Probe(buffer)) == previousSelectionEnd) {
            break;
        }
        handled = TRUE;
        if (GetCurrentLineTop(buffer, layout) <= targetY) {
            break;
        }
    }
    return handled;
}

Boolean ArrowKeyMovementMethod::PageDown(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    /*const*/ AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    const Boolean selecting = IsSelecting(buffer);
    const Int32 targetY = GetCurrentLineTop(buffer, layout) + GetPageHeight(widget);
    Boolean handled = FALSE;
    while(TRUE) {
        const Int32 previousSelectionEnd = Selection::GetSelectionEnd(ICharSequence::Probe(buffer));
        if (selecting) {
            Selection::ExtendDown(buffer, layout);
        }
        else {
            Selection::MoveDown(buffer, layout);
        }
        if (Selection::GetSelectionEnd(ICharSequence::Probe(buffer)) == previousSelectionEnd) {
            break;
        }
        handled = TRUE;
        if (GetCurrentLineTop(buffer, layout) >= targetY) {
            break;
        }
    }
    return handled;
}

Boolean ArrowKeyMovementMethod::Top(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    if (IsSelecting(buffer)) {
        Selection::ExtendSelection(buffer, 0);
    }
    else {
        Selection::SetSelection(buffer, 0);
    }
    return TRUE;
}

Boolean ArrowKeyMovementMethod::Bottom(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    if (IsSelecting(buffer)) {
        Int32 length;
        Selection::ExtendSelection(buffer, (ICharSequence::Probe(buffer)->GetLength(&length), length));
    }
    else {
        Int32 length;
        Selection::SetSelection(buffer, (ICharSequence::Probe(buffer)->GetLength(&length), length));
    }
    return TRUE;
}

Boolean ArrowKeyMovementMethod::LineStart(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    /*const*/ AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    if (IsSelecting(buffer)) {
        return Selection::ExtendToLeftEdge(buffer, layout);
    }
    else {
        return Selection::MoveToLeftEdge(buffer, layout);
    }
}

Boolean ArrowKeyMovementMethod::LineEnd(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    /*const*/ AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    if (IsSelecting(buffer)) {
        return Selection::ExtendToRightEdge(buffer, layout);
    }
    else {
        return Selection::MoveToRightEdge(buffer, layout);
    }
}

Boolean ArrowKeyMovementMethod::LeftWord(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    /*const*/ Int32 selectionEnd;
    widget->GetSelectionEnd(&selectionEnd);
    AutoPtr<IWordIterator> wordIterator;
    widget->GetWordIterator((IWordIterator**)&wordIterator);
    wordIterator->SetCharSequence(ICharSequence::Probe(buffer), selectionEnd, selectionEnd);
    return Selection::MoveToPreceding(buffer, ISelectionPositionIterator::Probe(wordIterator), IsSelecting(buffer));
}

Boolean ArrowKeyMovementMethod::RightWord(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    /*const*/ Int32 selectionEnd;
    widget->GetSelectionEnd(&selectionEnd);
    /*const*/ AutoPtr<IWordIterator> wordIterator;
    widget->GetWordIterator((IWordIterator**)&wordIterator);
    wordIterator->SetCharSequence(ICharSequence::Probe(buffer), selectionEnd, selectionEnd);
    return Selection::MoveToFollowing(buffer, ISelectionPositionIterator::Probe(wordIterator), IsSelecting(buffer));
}

Boolean ArrowKeyMovementMethod::Home(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return LineStart(widget, buffer);
}

Boolean ArrowKeyMovementMethod::End(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    return LineEnd(widget, buffer);
}


Boolean ArrowKeyMovementMethod::IsTouchSelecting(
    /* [in] */ Boolean isMouse,
    /* [in] */ ISpannable* buffer)
{
    Boolean flag = FALSE;
    return isMouse ? (Touch::IsActivelySelecting(buffer, &flag), flag) : IsSelecting(buffer);
}

ECode ArrowKeyMovementMethod::OnTouchEvent(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    Int32 initialScrollX = -1;
    Int32 initialScrollY = -1;
    Int32 action;
    event->GetAction(&action);
    Boolean isMouse;
    IInputEvent::Probe(event)->IsFromSource(IInputDevice::SOURCE_MOUSE, &isMouse);

    if (action == IMotionEvent::ACTION_UP) {
        Touch::GetInitialScrollX(widget, buffer, &initialScrollX);
        Touch::GetInitialScrollY(widget, buffer, &initialScrollY);
    }

    Boolean handled;
    Touch::OnTouchEvent(widget, buffer, event, &handled);

    Boolean bIsFocused, bDidTouchFocusSelect;

    if ((IView::Probe(widget)->IsFocused(&bIsFocused), bIsFocused) && !(widget->DidTouchFocusSelect(&bDidTouchFocusSelect), bDidTouchFocusSelect)) {
        Float x = 0.f, y = 0.f;
        event->GetX(&x);
        event->GetY(&y);

        if (action == IMotionEvent::ACTION_DOWN) {
            // Capture the mouse pointer down location to ensure selection starts
            // right under the mouse (and is not influenced by cursor location).
            // The code below needs to run for mouse events.
            // For touch events, the code should run only when selection is active.
            if (isMouse || IsTouchSelecting(isMouse, buffer)) {
                Int32 offset;
                widget->GetOffsetForPosition(x, y, &offset);

                buffer->SetSpan(LAST_TAP_DOWN, offset, offset, ISpanned::SPAN_POINT_POINT/*Spannable.SPAN_POINT_POINT*/);
                // Disallow intercepting of the touch events, so that
                // users can scroll and select at the same time.
                // without this, users would get booted out of select
                // mode once the view detected it needed to scroll.
                AutoPtr<IViewParent> parent;
                IView::Probe(widget)->GetParent((IViewParent**)&parent);
                assert(parent != NULL);
                parent->RequestDisallowInterceptTouchEvent(TRUE);
            }
        } else if (action == IMotionEvent::ACTION_MOVE) {

            // Cursor can be active at any location in the text while mouse pointer can start
            // selection from a totally different location. Use LAST_TAP_DOWN span to ensure
            // text selection will start from mouse pointer location.
            Boolean flag(FALSE);
            if (isMouse && (Touch::IsSelectionStarted(buffer, &flag), flag)) {
                Int32 offset;
                ISpanned::Probe(buffer)->GetSpanStart(LAST_TAP_DOWN, &offset);
                Selection::SetSelection(buffer, offset);
            }

            if (IsTouchSelecting(isMouse, buffer) && handled) {
                // Before selecting, make sure we've moved out of the "slop".
                // handled will be true, if we're in select mode AND we're
                // OUT of the slop

                // Turn long press off while we're selecting. User needs to
                // re-tap on the selection to enable long press
                IView::Probe(widget)->CancelLongPress();

                // Update selection as we're moving the selection area.

                // Get the current touch position
                Int32 offset = 0;
                widget->GetOffsetForPosition(x, y, &offset);

                Selection::ExtendSelection(buffer, offset);
                *ret = TRUE;
                return NOERROR;
            }
        }
        else if (action == IMotionEvent::ACTION_UP) {
            // If we have scrolled, then the up shouldn't move the cursor,
            // but we do need to make sure the cursor is still visible at
            // the current scroll offset to avoid the scroll jumping later
            // to show it.
            Int32 scrollY, scrollX;
            if ((initialScrollY >= 0 && initialScrollY != (IView::Probe(widget)->GetScrollY(&scrollY), scrollY)) ||
                (initialScrollX >= 0 && initialScrollX != (IView::Probe(widget)->GetScrollX(&scrollX), scrollX))) {
                Boolean bMoveCursorToVisibleOffset;
                widget->MoveCursorToVisibleOffset(&bMoveCursorToVisibleOffset);
                *ret = TRUE;
                return NOERROR;
            }

            Int32 offset = 0;
            widget->GetOffsetForPosition(x, y, &offset);
            if (IsTouchSelecting(isMouse, buffer)) {
                buffer->RemoveSpan(LAST_TAP_DOWN);
                Selection::ExtendSelection(buffer, offset);
            }

            MetaKeyKeyListener::AdjustMetaAfterKeypress(buffer);
            MetaKeyKeyListener::ResetLockedMeta(buffer);

            *ret = TRUE;
            return NOERROR;
        }
    }
    *ret = handled;
    return NOERROR;
}

ECode ArrowKeyMovementMethod::CanSelectArbitrarily(
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = TRUE;
    return NOERROR;
}

ECode ArrowKeyMovementMethod::Initialize(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text)
{
    Selection::SetSelection(text, 0);
    return NOERROR;
}

ECode ArrowKeyMovementMethod::OnTakeFocus(
    /* [in] */ ITextView* view,
    /* [in] */ ISpannable* text,
    /* [in] */ Int32 dir)
{
    if ((dir & (IView::FOCUS_FORWARD | IView::FOCUS_DOWN)) != 0) {
        AutoPtr<ILayout> layout;
        view->GetLayout((ILayout**)&layout);
        if (layout == NULL) {
            // This shouldn't be null, but do something sensible if it is.
            Int32 length;
            Selection::SetSelection(text, (ICharSequence::Probe(text)->GetLength(&length), length));
        }
    }
    else {
        Int32 length;
        Selection::SetSelection(text, (ICharSequence::Probe(text)->GetLength(&length), length));
    }
    return NOERROR;
}

ECode ArrowKeyMovementMethod::GetInstance(
    /* [out] */ IMovementMethod** ret)
{
    VALIDATE_NOT_NULL(ret)
    if (sInstance == NULL) {
        CArrowKeyMovementMethod::New((IArrowKeyMovementMethod**)&sInstance);
    }

    *ret = IMovementMethod::Probe(sInstance);
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
