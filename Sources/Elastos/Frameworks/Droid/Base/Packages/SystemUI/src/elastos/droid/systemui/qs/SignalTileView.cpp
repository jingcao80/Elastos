
#include "elastos/droid/systemui/qs/SignalTileView.h"
#include "elastos/droid/systemui/qs/QSTile.h"
#include "../R.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Animation::CValueAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::Widget::CFrameLayout;
using Elastos::Droid::Widget::CImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

const Int64 SignalTileView::DEFAULT_DURATION = InitStatic();
const Int64 SignalTileView::SHORT_DURATION = DEFAULT_DURATION / 3;
SignalTileView::SignalTileView(
    /* [in] */ IContext* context)
{
    QSTileView::constructor(context);

    mIn = AddTrafficView(R::drawable::ic_qs_signal_in);
    mOut = AddTrafficView(R::drawable::ic_qs_signal_out);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::wide_type_icon_start_padding_qs, &mWideOverlayIconStartPadding);
}

Int64 SignalTileView::InitStatic()
{
    AutoPtr<IValueAnimator> va;
    CValueAnimator::New((IValueAnimator**)&va);
    Int64 v = 0;
    va->GetDuration(&v);
    return v;
}

AutoPtr<IImageView> SignalTileView::AddTrafficView(
    /* [in] */ Int32 icon)
{
    AutoPtr<IImageView> traffic;
    CImageView::New(mContext, (IImageView**)&traffic);
    traffic->SetImageResource(icon);
    traffic->SetAlpha(0.f);
    AddView(IView::Probe(traffic));
    return traffic;
}

AutoPtr<IView> SignalTileView::CreateIcon()
{
    CFrameLayout::New(mContext, (IFrameLayout**)&mIconFrame);
    CImageView::New(mContext, (IImageView**)&mSignal);
    IViewGroup::Probe(mIconFrame)->AddView(IView::Probe(mSignal));
    CImageView::New(mContext, (IImageView**)&mOverlay);
    IViewGroup::Probe(mIconFrame)->AddView(IView::Probe(mOverlay), IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT);
    return IView::Probe(mIconFrame);
}

void SignalTileView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    QSTileView::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    Int32 v = 0;
    Int32 hs = MeasureSpec::MakeMeasureSpec((IView::Probe(mIconFrame)->GetMeasuredHeight(&v), v), MeasureSpec::EXACTLY);
    Int32 ws = MeasureSpec::MakeMeasureSpec((IView::Probe(mIconFrame)->GetMeasuredHeight(&v), v), MeasureSpec::AT_MOST);
    IView::Probe(mIn)->Measure(ws, hs);
    IView::Probe(mOut)->Measure(ws, hs);
}

ECode SignalTileView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    QSTileView::OnLayout(changed, l, t, r, b);
    LayoutIndicator(IView::Probe(mIn));
    LayoutIndicator(IView::Probe(mOut));
    return NOERROR;
}

void SignalTileView::LayoutIndicator(
    /* [in] */ IView* indicator)
{
    Int32 v = 0;
    Boolean isRtl = (GetLayoutDirection(&v), v) == LAYOUT_DIRECTION_RTL;
    Int32 left, right;
    if (isRtl) {
        IView::Probe(mIconFrame)->GetLeft(&right);
        left = right - (indicator->GetMeasuredWidth(&v), v);
    }
    else {
        IView::Probe(mIconFrame)->GetRight(&left);
        right = left + (indicator->GetMeasuredWidth(&v), v);
    }
    Int32 b = 0;
    IView::Probe(mIconFrame)->GetBottom(&b);
    indicator->Layout(
            left, b - (indicator->GetMeasuredHeight(&v), v),
            right, b);
}

ECode SignalTileView::HandleStateChanged(
    /* [in] */ IQSTileState* state)
{
    QSTileView::HandleStateChanged(state);
    AutoPtr<QSTile::SignalState> s = (QSTile::SignalState*) state;
    mSignal->SetImageResource(s->mIconId);
    if (s->mOverlayIconId > 0) {
        IView::Probe(mOverlay)->SetVisibility(VISIBLE);
        mOverlay->SetImageResource(s->mOverlayIconId);
    }
    else {
        IView::Probe(mOverlay)->SetVisibility(GONE);
    }
    if (s->mOverlayIconId > 0 && s->mIsOverlayIconWide) {
        IView::Probe(mSignal)->SetPaddingRelative(mWideOverlayIconStartPadding, 0, 0, 0);
    }
    else {
        IView::Probe(mSignal)->SetPaddingRelative(0, 0, 0, 0);
    }
    AutoPtr<IDrawable> drawable;
    mSignal->GetDrawable((IDrawable**)&drawable);
    if (((QSTile::State*)state)->mAutoMirrorDrawable && drawable != NULL) {
        drawable->SetAutoMirrored(TRUE);
    }
    Boolean shown = FALSE;
    IsShown(&shown);
    SetVisibility(IView::Probe(mIn), shown, s->mActivityIn);
    SetVisibility(IView::Probe(mOut), shown, s->mActivityOut);
    return NOERROR;
}

void SignalTileView::SetVisibility(
    /* [in] */ IView* view,
    /* [in] */ Boolean shown,
    /* [in] */ Boolean visible)
{
    const Float newAlpha = shown && visible ? 1 : 0;
    Float fv = 0;
    if ((view->GetAlpha(&fv), fv) == newAlpha) return;
    if (shown) {
        AutoPtr<IViewPropertyAnimator> vpa;
        view->Animate((IViewPropertyAnimator**)&vpa);
        vpa->SetDuration(visible ? SHORT_DURATION : DEFAULT_DURATION);
        vpa->Alpha(newAlpha);
        vpa->Start();
    }
    else {
        view->SetAlpha(newAlpha);
    }
}

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
