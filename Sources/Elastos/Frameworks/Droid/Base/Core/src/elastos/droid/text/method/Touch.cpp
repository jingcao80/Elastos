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

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/text/method/Touch.h"
#include "elastos/droid/text/method/MetaKeyKeyListener.h"
#include "elastos/droid/view/ViewConfiguration.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Text::Method::EIID_ITouchDragState;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::ViewConfiguration;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/*****************************Touch::DragState*****************************/

CAR_INTERFACE_IMPL_2(Touch::DragState, Object, ITouchDragState, INoCopySpan)

Touch::DragState::DragState()
    : mFarEnough(FALSE)
    , mUsed(FALSE)
{}

ECode Touch::DragState::constructor(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY)
{
    mX = x;
    mY = y;
    mScrollX = scrollX;
    mScrollY = scrollY;
    return NOERROR;
}

/*****************************Touch*****************************/
Touch::Touch(){}

ECode Touch::ScrollTo(
    /* [in] */ ITextView* widget,
    /* [in] */ ILayout* layout,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 wTotalPaddingLeft, wTotalPaddingRight, wWidth;

    const Int32 horizontalPadding = (widget->GetTotalPaddingLeft(&wTotalPaddingLeft), wTotalPaddingLeft) + (widget->GetTotalPaddingRight(&wTotalPaddingRight), wTotalPaddingRight);
    const Int32 availableWidth = (IView::Probe(widget)->GetWidth(&wWidth), wWidth) - horizontalPadding;

    Int32 top;
    layout->GetLineForVertical(y, &top);
    LayoutAlignment a;
    layout->GetParagraphAlignment(top, &a);
    Int32 lParagraphDirection;
    layout->GetParagraphDirection(top, &lParagraphDirection);
    Boolean ltr = lParagraphDirection > 0;

    Int32 left, right;
    Boolean bHorizontallyScrolling = FALSE;
    if ((widget->GetHorizontallyScrolling(&bHorizontallyScrolling), bHorizontallyScrolling)) {
        Int32 wTotalPaddingTop, wTotalPaddingBottom, wHeight;
        const Int32 verticalPadding = (widget->GetTotalPaddingTop(&wTotalPaddingTop), wTotalPaddingTop) + (widget->GetTotalPaddingBottom(&wTotalPaddingBottom), wTotalPaddingBottom);
        Int32 bottom;
        layout->GetLineForVertical(y + (IView::Probe(widget)->GetHeight(&wHeight), wHeight) - verticalPadding, &bottom);

        left = Elastos::Core::Math::INT32_MAX_VALUE;
        right = 0;

        for (Int32 i = top; i <= bottom; i++) {
            Float lLineLeft, lLineRight;
            left = (Int32) Elastos::Core::Math::Min((Float)left, (layout->GetLineLeft(i, &lLineLeft), lLineLeft));
            right = (Int32) Elastos::Core::Math::Max((Float)right, (layout->GetLineRight(i, &lLineRight), lLineRight));
        }
    } else {
        left = 0;
        right = availableWidth;
    }

    const Int32 actualWidth = right - left;

    if (actualWidth < availableWidth) {
        if (a == ALIGN_CENTER) {
            x = left - ((availableWidth - actualWidth) / 2);
        } else if ((ltr && (a == ALIGN_OPPOSITE))
                    || (!ltr && (a == ALIGN_NORMAL))
                    || (a == ALIGN_RIGHT)) {
            // align_opposite does NOT mean align_right, we need the paragraph
            // direction to resolve it to left or right
            x = left - (availableWidth - actualWidth);
        } else {
            x = left;
        }
    } else {
        x = Elastos::Core::Math::Min(x, right - availableWidth);
        x = Elastos::Core::Math::Max(x, left);
    }

    return IView::Probe(widget)->ScrollTo(x, y);
}

ECode Touch::OnTouchEvent(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr< ArrayOf< IInterface* > > ds;
    Int32 actionMasked;
    switch ((event->GetActionMasked(&actionMasked), actionMasked)) {
        case IMotionEvent::ACTION_DOWN: {
            Int32 bufLen = 0;
            ICharSequence::Probe(buffer)->GetLength(&bufLen);
            ISpanned::Probe(buffer)->GetSpans(0, bufLen, EIID_ITouchDragState, (ArrayOf< IInterface* >**)&ds);

            for (Int32 i = 0; i < ds->GetLength(); i++) {
                buffer->RemoveSpan((*ds)[i]);
            }

            Float eX,eY;
            Int32 wScrollX,wScrollY;
            AutoPtr<DragState> state = new DragState();
            state->constructor((event->GetX(&eX), eX), (event->GetY(&eY), eY),
                    (IView::Probe(widget)->GetScrollX(&wScrollX), wScrollX), (IView::Probe(widget)->GetScrollY(&wScrollY), wScrollY));
            AutoPtr<ITouchDragState> tmp = ITouchDragState::Probe(state);
            buffer->SetSpan(tmp, 0, 0, ISpanned::SPAN_MARK_MARK);
            *ret = TRUE;
            return NOERROR;
        }

        case IMotionEvent::ACTION_UP: {
            Int32 bufLen = 0;
            ICharSequence::Probe(buffer)->GetLength(&bufLen);
            ISpanned::Probe(buffer)->GetSpans(0, bufLen, EIID_ITouchDragState, (ArrayOf< IInterface* >**)&ds);

            for (Int32 i = 0; i < ds->GetLength(); i++) {
                buffer->RemoveSpan((*ds)[i]);
            }

            if (ds->GetLength() > 0 && ((DragState*)(IObject::Probe((*ds)[0])))->mUsed) {
                *ret = TRUE;
                return NOERROR;
            } else {
                *ret = FALSE;
                return NOERROR;
            }
        }

        case IMotionEvent::ACTION_MOVE: {
            Int32 bufLen = 0;
            ICharSequence::Probe(buffer)->GetLength(&bufLen);
            ISpanned::Probe(buffer)->GetSpans(0, bufLen, EIID_ITouchDragState, (ArrayOf< IInterface* >**)&ds);

            if (ds->GetLength() > 0) {
                ((DragState*)(ITouchDragState::Probe((*ds)[0])))->mIsSelectionStarted = FALSE;
                if ( ((DragState*)(ITouchDragState::Probe((*ds)[0])))->mFarEnough == FALSE ) {
                    AutoPtr<IContext> context;
                    IView::Probe(widget)->GetContext((IContext**)&context);
                    Int32 slop;
                    AutoPtr<IViewConfiguration> ivdfg = ViewConfiguration::Get(context);
                    ivdfg->GetScaledTouchSlop(&slop);

                    Float eX,eY;

                    if (Elastos::Core::Math::Abs((event->GetX(&eX), eX) - ((DragState*)(ITouchDragState::Probe((*ds)[0])))->mX) >= slop ||
                        Elastos::Core::Math::Abs((event->GetY(&eY), eY) - ((DragState*)(ITouchDragState::Probe((*ds)[0])))->mY) >= slop) {
                        ((DragState*)(ITouchDragState::Probe((*ds)[0])))->mFarEnough = TRUE;
                    }
                }

                if ( ((DragState*)(ITouchDragState::Probe((*ds)[0])))->mFarEnough ) {
                    ((DragState*)(ITouchDragState::Probe((*ds)[0])))->mUsed = TRUE;
                    Int32 metaState;
                    Int32 gms;
                    Int32 gms_;
                    Boolean cap = ((event->GetMetaState(&metaState), metaState) & IKeyEvent::META_SHIFT_ON) != 0
                            || (MetaKeyKeyListener::GetMetaState(ICharSequence::Probe(buffer),
                                    IMetaKeyKeyListener::META_SHIFT_ON, &gms), gms) == 1
                            || (MetaKeyKeyListener::GetMetaState(ICharSequence::Probe(buffer),
                                    IMetaKeyKeyListener::META_SELECTING, &gms_), gms_) != 0;
                    Float dx;
                    Float dy;
                    Float ex,ey;
                    if (cap) {
                        // if we're selecting, we want the scroll to go in
                        // the direction of the drag
                        dx = (event->GetX(&ex), ex) - ((DragState*)(ITouchDragState::Probe((*ds)[0])))->mX;
                        dy = (event->GetY(&ey), ey) - ((DragState*)(ITouchDragState::Probe((*ds)[0])))->mY;
                    } else {
                        dx = ((DragState*)(ITouchDragState::Probe((*ds)[0])))->mX - (event->GetX(&ex), ex);
                        dy = ((DragState*)(ITouchDragState::Probe((*ds)[0])))->mY - (event->GetY(&ey), ey);
                    }
                    ((DragState*)(ITouchDragState::Probe((*ds)[0])))->mX = (event->GetX(&ex), ex);
                    ((DragState*)(ITouchDragState::Probe((*ds)[0])))->mY = (event->GetY(&ey), ey);

                    Int32 wScrollX, wScrollY;
                    Int32 nx = (IView::Probe(widget)->GetScrollX(&wScrollX), wScrollX) + (Int32) dx;
                    Int32 ny = (IView::Probe(widget)->GetScrollY(&wScrollY), wScrollY) + (Int32) dy;

                    Int32 wTotalPaddingTop, wTotalPaddingBottom;
                    Int32 padding = (widget->GetTotalPaddingTop(&wTotalPaddingTop), wTotalPaddingTop) + (widget->GetTotalPaddingBottom(&wTotalPaddingBottom), wTotalPaddingBottom);
                    AutoPtr<ILayout> layout;
                    widget->GetLayout((ILayout**)&layout);

                    Int32 lHeight, wHeight;
                    ny = Elastos::Core::Math::Min(ny, (layout->GetHeight(&lHeight), lHeight) - ((IView::Probe(widget)->GetHeight(&wHeight), wHeight) - padding));
                    ny = Elastos::Core::Math::Max(ny, 0);

                    Int32 oldX;
                    IView::Probe(widget)->GetScrollX(&oldX);
                    Int32 oldY;
                    IView::Probe(widget)->GetScrollY(&oldY);

                    ScrollTo(widget, layout, nx, ny);

                    // If we actually scrolled, then cancel the up action.
                    Int32 nowX, nowY;
                    if (oldX != (IView::Probe(widget)->GetScrollX(&nowX), nowX) || oldY != (IView::Probe(widget)->GetScrollY(&nowY), nowY)) {
                        IView::Probe(widget)->CancelLongPress();
                    }
                    *ret = TRUE;
                    return NOERROR;

                }
            }
        }
    }
    *ret = FALSE;
    return NOERROR;
}

ECode Touch::GetInitialScrollX(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr< ArrayOf< IInterface* > > ds;
    Int32 bufLen = 0;
    ICharSequence::Probe(buffer)->GetLength(&bufLen);
    ISpanned::Probe(buffer)->GetSpans(0, bufLen, EIID_ITouchDragState, (ArrayOf< IInterface* >**)&ds);
    assert(ds != NULL);

    *ret = ds->GetLength() > 0 ? ((DragState*)(ITouchDragState::Probe((*ds)[0])))->mScrollX : -1;
    return NOERROR;
}

ECode Touch::GetInitialScrollY(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr< ArrayOf< IInterface* > > ds;
    Int32 bufLen = 0;
    ICharSequence::Probe(buffer)->GetLength(&bufLen);
    ISpanned::Probe(buffer)->GetSpans(0, bufLen, EIID_ITouchDragState, (ArrayOf< IInterface* >**)&ds);
    assert(ds != NULL);

    *ret = ((ds != NULL) && (ds->GetLength() > 0)) ? ((DragState*)(ITouchDragState::Probe((*ds)[0])))->mScrollY : -1;
    return NOERROR;
}

ECode Touch::IsActivelySelecting(
    /* [in] */ ISpannable* buffer,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr<ArrayOf<ITouchDragState*> > ds;
    Int32 len;
    ICharSequence::Probe(buffer)->GetLength(&len);
    ISpanned::Probe(buffer)->GetSpans(0, len, EIID_ITouchDragState, (ArrayOf<IInterface*>**)&ds);

    *ret = ds->GetLength() > 0 && ((DragState*)((*ds)[0]))->mIsActivelySelecting;
    return NOERROR;
}

ECode Touch::IsSelectionStarted(
    /* [in] */ ISpannable* buffer,
    /* [out] */ Boolean* ret)
{
    AutoPtr<ArrayOf<ITouchDragState*> > ds;
    Int32 len;
    ICharSequence::Probe(buffer)->GetLength(&len);
    ISpanned::Probe(buffer)->GetSpans(0, len, EIID_ITouchDragState, (ArrayOf<IInterface*>**)&ds);

    *ret = ds->GetLength() > 0 && ((DragState*)((*ds)[0]))->mIsSelectionStarted;
    return NOERROR;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
