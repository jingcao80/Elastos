
#include "elastos/droid/systemui/recents/views/DebugOverlayView.h"
#include "../R.h"

using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::IProgressBar;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

CAR_INTERFACE_IMPL(DebugOverlayView::OnSeekBarChangeListener, Object, ISeekBarOnSeekBarChangeListener)

DebugOverlayView::OnSeekBarChangeListener::OnSeekBarChangeListener(
    /* [in] */ DebugOverlayView* host)
    : mHost(host)
{}

ECode DebugOverlayView::OnSeekBarChangeListener::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    return mHost->OnStopTrackingTouch(seekBar);
}

// @Override
ECode DebugOverlayView::OnSeekBarChangeListener::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    return mHost->OnStartTrackingTouch(seekBar);
}

// @Override
ECode DebugOverlayView::OnSeekBarChangeListener::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    return mHost->OnProgressChanged(seekBar, progress, fromUser);
}

const Int32 DebugOverlayView::sCornerRectSize;

CAR_INTERFACE_IMPL(DebugOverlayView, FrameLayout, IDebugOverlayView)

DebugOverlayView::DebugOverlayView()
    : mEnabled(TRUE)
{
    CPaint::New((IPaint**)&mDebugOutline);
    CPaint::New((IPaint**)&mTmpPaint);
    CRect::New((IRect**)&mTmpRect);
    mListener = new OnSeekBarChangeListener(this);
}

ECode DebugOverlayView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode DebugOverlayView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode DebugOverlayView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode DebugOverlayView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes));
    mConfig = RecentsConfiguration::GetInstance();
    mDebugOutline->SetColor(0xFFff0000);
    mDebugOutline->SetStyle(PaintStyle_STROKE);
    mDebugOutline->SetStrokeWidth(8.0f);
    SetWillNotDraw(FALSE);
    return NOERROR;
}

ECode DebugOverlayView::SetCallbacks(
    /* [in] */ IDebugOverlayViewCallbacks* cb)
{
    mCb = cb;
    return NOERROR;
}

// @Override
ECode DebugOverlayView::OnFinishInflate()
{
    AutoPtr<IView> view;
    FindViewById(R::id::debug_seek_bar_1, (IView**)&view);
    mPrimarySeekBar = ISeekBar::Probe(view);
    mPrimarySeekBar->SetOnSeekBarChangeListener(mListener);
    view = NULL;
    FindViewById(R::id::debug_seek_bar_2, (IView**)&view);
    mSecondarySeekBar = ISeekBar::Probe(view);
    mSecondarySeekBar->SetOnSeekBarChangeListener(mListener);
    return NOERROR;
}

/** Enables the debug overlay drawing. */
ECode DebugOverlayView::Enable()
{
    mEnabled = TRUE;
    SetVisibility(IView::VISIBLE);
    return NOERROR;
}

/** Disables the debug overlay drawing. */
ECode DebugOverlayView::Disable()
{
    mEnabled = FALSE;
    SetVisibility(IView::GONE);
    return NOERROR;
}

/** Clears all debug rects. */
ECode DebugOverlayView::Clear()
{
    mRects.Clear();
    return NOERROR;
}

/** Adds a rect to be drawn. */
void DebugOverlayView::AddRect(
    /* [in] */ IRect* r,
    /* [in] */ Int32 color)
{
    mRects.PushBack(new Pair<AutoPtr<IRect>, Int32>(r, color));
    Invalidate();
}

/** Adds a view's global rect to be drawn. */
void DebugOverlayView::AddViewRect(
    /* [in] */ IView* v,
    /* [in] */ Int32 color)
{
    AutoPtr<IRect> vr;
    CRect::New((IRect**)&vr);
    Boolean res;
    v->GetGlobalVisibleRect(vr, &res);
    mRects.PushBack(new Pair<AutoPtr<IRect>, Int32>(vr, color));
    Invalidate();
}

/** Adds a rect, relative to a given view to be drawn. */
void DebugOverlayView::AddRectRelativeToView(
    /* [in] */ IView* v,
    /* [in] */ IRect* r,
    /* [in] */ Int32 color)
{
    AutoPtr<IRect> vr;
    CRect::New((IRect**)&vr);
    Boolean res;
    v->GetGlobalVisibleRect(vr, &res);
    Int32 left, top;
    vr->GetLeft(&left);
    vr->GetTop(&top);
    r->OffsetTo(left, top);
    mRects.PushBack(new Pair<AutoPtr<IRect>, Int32>(r, color));
    Invalidate();
}

/** Sets the debug text at the bottom of the screen. */
void DebugOverlayView::SetText(
    /* [in] */ const String& message)
{
    mText = message;
    Invalidate();
}

// @Override
ECode DebugOverlayView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    FrameLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    AutoPtr<IRect> r;
    CRect::New(0, 0, sCornerRectSize, sCornerRectSize, (IRect**)&r);
    AddRect(r, 0xFFff0000);
    Int32 w, h;
    GetMeasuredWidth(&w);
    GetMeasuredHeight(&h);
    r = NULL;
    CRect::New(w - sCornerRectSize, h - sCornerRectSize, w, h, (IRect**)&r);
    AddRect(r, 0xFFff0000);
    return NOERROR;
}

// @Override
void DebugOverlayView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (mEnabled) {
        Int32 w, h;
        GetMeasuredWidth(&w);
        GetMeasuredHeight(&h);
        // Draw the outline
        canvas->DrawRect(0, 0, w, h, mDebugOutline);

        // Draw the rects
        List<AutoPtr<Pair<AutoPtr<IRect>, Int32> > >::Iterator iter = mRects.Begin();
        for (; iter != mRects.End(); ++iter) {
            AutoPtr<Pair<AutoPtr<IRect>, Int32> > r = *iter;
            mTmpPaint->SetColor(r->mSecond);
            canvas->DrawRect(r->mFirst, mTmpPaint);
        }

        // Draw the text
        if (mText != NULL && mText.GetLength() > 0) {
            mTmpPaint->SetColor(0xFFff0000);
            mTmpPaint->SetTextSize(60);
            mTmpPaint->GetTextBounds(mText, 0, 1, mTmpRect);
            Int32 height, bottom;
            mTmpRect->GetHeight(&height);
            mConfig->mSystemInsets->GetBottom(&bottom);
            canvas->DrawText(mText, 10.0f, h - height - bottom, mTmpPaint);
        }
    }
}

/**** SeekBar.OnSeekBarChangeListener Implementation ****/

// @Override
ECode DebugOverlayView::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    // Do nothing
    return NOERROR;
}

// @Override
ECode DebugOverlayView::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    // Do nothing
    return NOERROR;
}

// @Override
ECode DebugOverlayView::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    Int32 max;
    if (seekBar == mPrimarySeekBar) {
        IProgressBar::Probe(mPrimarySeekBar)->GetMax(&max);
        mCb->OnPrimarySeekBarChanged((Float) progress / max);
    }
    else if (seekBar == mSecondarySeekBar) {
        IProgressBar::Probe(mSecondarySeekBar)->GetMax(&max);
        mCb->OnSecondarySeekBarChanged((Float) progress / max);
    }
    return NOERROR;
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
