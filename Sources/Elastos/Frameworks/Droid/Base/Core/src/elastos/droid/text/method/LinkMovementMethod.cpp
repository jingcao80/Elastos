
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/text/method/LinkMovementMethod.h"
#include "elastos/droid/text/method/CLinkMovementMethod.h"
#include "elastos/droid/text/Selection.h"
#include "elastos/droid/view/KeyEvent.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Text::Selection;
using Elastos::Droid::Text::Style::EIID_IClickableSpan;
using Elastos::Droid::Text::Style::IClickableSpan;
using Elastos::Droid::View::KeyEvent;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

AutoPtr<IInterface> LinkMovementMethod::FROM_BELOW /*= (IInterface*)(new Object())*/;//new ElObject();;// = new NoCopySpan.Concrete();

AutoPtr<ILinkMovementMethod> LinkMovementMethod::sInstance;

const Int32 LinkMovementMethod::CLICK = 1;

const Int32 LinkMovementMethod::UP = 2;

const Int32 LinkMovementMethod::DOWN = 3;

LinkMovementMethod::LinkMovementMethod()
{}

LinkMovementMethod::~LinkMovementMethod()
{}

CAR_INTERFACE_IMPL_3(LinkMovementMethod, Object, ILinkMovementMethod, IMovementMethod, IBaseMovementMethod)

ECode LinkMovementMethod::constructor()
{
    return NOERROR;
}

ECode LinkMovementMethod::CanSelectArbitrarily(
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = TRUE;
    return NOERROR;
}

Boolean LinkMovementMethod::HandleMovementKey(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 movementMetaState,
    /* [in] */ IKeyEvent* event)
{
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_CENTER:
        case IKeyEvent::KEYCODE_ENTER:
            if (KeyEvent::MetaStateHasNoModifiers(movementMetaState)) {
                Int32 action, repeatCount;
                if ((event->GetAction(&action), action) == IKeyEvent::ACTION_DOWN &&
                        (event->GetRepeatCount(&repeatCount), repeatCount) == 0 && Action(CLICK, widget, buffer)) {
                    return TRUE;
                }
            }
            break;
    }
    return ScrollingMovementMethod::HandleMovementKey(widget, buffer, keyCode, movementMetaState, event);
}

Boolean LinkMovementMethod::Up(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    if (Action(UP, widget, buffer)) {
        return TRUE;
    }
    return ScrollingMovementMethod::Up(widget, buffer);
}

Boolean LinkMovementMethod::Down(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    if (Action(DOWN, widget, buffer)) {
        return TRUE;
    }

    return ScrollingMovementMethod::Down(widget, buffer);
}

Boolean LinkMovementMethod::Left(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    if (Action(UP, widget, buffer)) {
        return TRUE;
    }

    return ScrollingMovementMethod::Left(widget, buffer);
}

Boolean LinkMovementMethod::Right(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    if (Action(DOWN, widget, buffer)) {
        return TRUE;
    }

    return ScrollingMovementMethod::Right(widget, buffer);
}

Boolean LinkMovementMethod::Action(
    /* [in] */ Int32 what,
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer)
{
    AutoPtr<ILayout> layout;
    widget->GetLayout((ILayout**)&layout);
    Int32 wTotalpaddingTop, wTotalPaddingBottom, wScrollY, wHeight;
    widget->GetTotalPaddingTop(&wTotalpaddingTop);
    widget->GetTotalPaddingBottom(&wTotalPaddingBottom);
    IView::Probe(widget)->GetScrollY(&wScrollY);
    IView::Probe(widget)->GetHeight(&wHeight);

    Int32 padding = wTotalpaddingTop +
                      wTotalPaddingBottom;
    Int32 areatop = wScrollY;
    Int32 areabot = areatop + wHeight - padding;

    Int32 linetop;
    layout->GetLineForVertical(areatop, &linetop);
    Int32 linebot;
    layout->GetLineForVertical(areabot, &linebot);

    Int32 first;
    layout->GetLineStart(linetop, &first);
    Int32 last;
    layout->GetLineEnd(linebot, &last);

    AutoPtr< ArrayOf< IClickableSpan* > > candidates;
    ISpanned::Probe(buffer)->GetSpans(first, last, EIID_IClickableSpan, (ArrayOf< IInterface* >**)&candidates);

    Int32 a = Selection::GetSelectionStart(ICharSequence::Probe(buffer));
    Int32 b = Selection::GetSelectionEnd(ICharSequence::Probe(buffer));

    Int32 selStart = Elastos::Core::Math::Min(a, b);
    Int32 selEnd = Elastos::Core::Math::Max(a, b);

    if (selStart < 0) {
        Int32 spanStart;
        if ((ISpanned::Probe(buffer)->GetSpanStart(FROM_BELOW, &spanStart), spanStart) >= 0) {
            Int32 bufLen;
            ICharSequence::Probe(buffer)->GetLength(&bufLen);
            selStart = selEnd = bufLen;
        }
    }

    if (selStart > last)
        selStart = selEnd = Elastos::Core::Math::INT32_MAX_VALUE;
    if (selEnd < first)
        selStart = selEnd = -1;

    switch (what) {
        case CLICK:{
            if (selStart == selEnd) {
                return FALSE;
            }

            AutoPtr< ArrayOf< IClickableSpan* > > link;
            ISpanned::Probe(buffer)->GetSpans(selStart, selEnd, EIID_IClickableSpan, (ArrayOf< IInterface* >**)&link);

            if (link->GetLength() != 1)
                return FALSE;

            (*link)[0]->OnClick(IView::Probe(widget));
            break;
        }

        case UP:{
            Int32 beststart, bestend;

            beststart = -1;
            bestend = -1;

            for (Int32 i = 0; i < candidates->GetLength(); i++) {
                Int32 end;
                ISpanned::Probe(buffer)->GetSpanEnd((*candidates)[i], &end);

                if (end < selEnd || selStart == selEnd) {
                    if (end > bestend) {
                        ISpanned::Probe(buffer)->GetSpanStart((*candidates)[i], &beststart);
                        bestend = end;
                    }
                }
            }

            if (beststart >= 0) {
                Selection::SetSelection(buffer, bestend, beststart);
                return TRUE;
            }

            break;
        }

        case DOWN:{
            Int32 beststart, bestend;
            beststart = Elastos::Core::Math::INT32_MAX_VALUE;
            bestend = Elastos::Core::Math::INT32_MAX_VALUE;

            for (Int32 i = 0; i < candidates->GetLength(); i++) {
                Int32 start;
                ISpanned::Probe(buffer)->GetSpanStart((*candidates)[i], &start);

                if (start > selStart || selStart == selEnd) {
                    if (start < beststart) {
                        beststart = start;
                        ISpanned::Probe(buffer)->GetSpanEnd((*candidates)[i], &bestend);
                    }
                }
            }

            if (bestend < Elastos::Core::Math::INT32_MAX_VALUE) {
                Selection::SetSelection(buffer, beststart, bestend);
                return TRUE;
            }

            break;
        }
    }

    return FALSE;
}

ECode LinkMovementMethod::OnTouchEvent(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    Int32 action;
    event->GetAction(&action);

    if (action == IMotionEvent::ACTION_UP ||
        action == IMotionEvent::ACTION_DOWN) {
        Float fX,fY;
        event->GetX(&fX);
        event->GetY(&fY);
        Int32 x = (Int32) fX;
        Int32 y = (Int32) fY;

        Int32 wTotalPaddingLeft, wTotalPaddingTop, wScrollX, wScrollY;

        x -= (widget->GetTotalPaddingLeft(&wTotalPaddingLeft), wTotalPaddingLeft);
        y -= (widget->GetTotalPaddingTop(&wTotalPaddingTop), wTotalPaddingTop);

        x += (IView::Probe(widget)->GetScrollX(&wScrollX), wScrollX);
        y += (IView::Probe(widget)->GetScrollY(&wScrollY), wScrollY);

        AutoPtr<ILayout> layout;
        widget->GetLayout((ILayout**)&layout);
        Int32 line;
        layout->GetLineForVertical(y, &line);
        Int32 off;
        layout->GetOffsetForHorizontal(line, x, &off);

        AutoPtr< ArrayOf< IClickableSpan* > > link;
        ISpanned::Probe(buffer)->GetSpans(off, off,  EIID_IClickableSpan, (ArrayOf< IInterface* >**)&link);

        if (link->GetLength() != 0) {
            if (action == IMotionEvent::ACTION_UP) {
                (*link)[0]->OnClick((IView*)widget);
            } else if (action == IMotionEvent::ACTION_DOWN) {
                Int32 spanStart, spanEnd;
                ISpanned::Probe(buffer)->GetSpanStart((IInterface*)((*link)[0]), &spanStart);
                ISpanned::Probe(buffer)->GetSpanEnd((IInterface*)((*link)[0]), &spanEnd);
                Selection::SetSelection(buffer, spanStart, spanEnd);
            }

            *ret = TRUE;
            return NOERROR;
        } else {
            Selection::RemoveSelection(buffer);
        }
    }
    return ScrollingMovementMethod::OnTouchEvent(widget, buffer, event, ret);
}

ECode LinkMovementMethod::Initialize(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text)
{
    Selection::RemoveSelection(text);
    return text->RemoveSpan(FROM_BELOW);
}

ECode LinkMovementMethod::OnTakeFocus(
    /* [in] */ ITextView* view,
    /* [in] */ ISpannable* text,
    /* [in] */ Int32 dir)
{
    Selection::RemoveSelection(text);

    if ((dir & IView::FOCUS_BACKWARD) != 0) {
        text->SetSpan(FROM_BELOW, 0, 0, ISpanned::SPAN_POINT_POINT);
    } else {
        text->RemoveSpan(FROM_BELOW);
    }
    return NOERROR;
}

ECode LinkMovementMethod::GetInstance(
    /* [out] */ IMovementMethod** ret)
{
    VALIDATE_NOT_NULL(ret);
    if (sInstance == NULL) {
        CLinkMovementMethod::New((ILinkMovementMethod**)&sInstance);
    }

    *ret = IMovementMethod::Probe(sInstance);
    REFCOUNT_ADD(*ret);
    return NOERROR;
}


} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
