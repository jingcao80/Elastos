
#include "elastos/droid/systemui/qs/QSTileView.h"
#include "elastos/droid/systemui/qs/QSDualTileLabel.h"
#include "elastos/droid/systemui/qs/QSTile.h"
#include "elastos/droid/systemui/FontSizeUtils.h"
#include "R.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/R.h>
#include <elastos/droid/os/Looper.h>
#include <elastos/droid/utility/MathUtils.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CTypefaceHelper;
using Elastos::Droid::Graphics::ITypefaceHelper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::View::CView;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Widget::CTextView;
using Elastos::Droid::Widget::CImageView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ImageViewScaleType_CENTER_INSIDE;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

const Int32 QSTileView::H::STATE_CHANGED = 1;
QSTileView::H::H(
    /* [in] */ QSTileView* host)
    : mHost(host)
{
    Handler::constructor(Looper::GetMainLooper());
}

ECode QSTileView::H::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    if (what == STATE_CHANGED) {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        mHost->HandleStateChanged(IQSTileState::Probe(obj));
    }
    return NOERROR;
}

AutoPtr<ITypeface> QSTileView::CONDENSED = InitStatic();
CAR_INTERFACE_IMPL(QSTileView, ViewGroup, IQSTileView)
QSTileView::QSTileView()
    : mIconSizePx(0)
    , mTileSpacingPx(0)
    , mTilePaddingTopPx(0)
    , mTilePaddingBelowIconPx(0)
    , mDualTileVerticalPaddingPx(0)
    , mDual(FALSE)
{
    mHandler = new H(this);
}

AutoPtr<ITypeface> QSTileView::InitStatic()
{
    AutoPtr<ITypefaceHelper> helper;
    CTypefaceHelper::AcquireSingleton((ITypefaceHelper**)&helper);
    helper->Create(String("sans-serif-condensed"), ITypeface::NORMAL, (ITypeface**)&CONDENSED);
    return CONDENSED;
}

ECode QSTileView::constructor(
    /* [in] */ IContext* context)
{
    ViewGroup::constructor(context);

    mContext = context;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::qs_tile_icon_size, &mIconSizePx);
    res->GetDimensionPixelSize(R::dimen::qs_tile_spacing, &mTileSpacingPx);
    res->GetDimensionPixelSize(R::dimen::qs_tile_padding_below_icon, &mTilePaddingBelowIconPx);
    res->GetDimensionPixelSize(R::dimen::qs_dual_tile_padding_vertical, &mDualTileVerticalPaddingPx);
    RecreateLabel();
    SetClipChildren(FALSE);

    CView::New(context, (IView**)&mTopBackgroundView);
    AddView(mTopBackgroundView);

    mIcon = CreateIcon();
    AddView(mIcon);

    CView::New(mContext, (IView**)&mDivider);
    Int32 v = 0;
    res->GetColor(R::color::qs_tile_divider, &v);
    mDivider->SetBackgroundColor(v);
    Int32 dh = 0;
    res->GetDimensionPixelSize(R::dimen::qs_tile_divider_height, &dh);

    AutoPtr<IViewGroupLayoutParams> vp;
    CViewGroupLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT, dh, (IViewGroupLayoutParams**)&vp);
    mDivider->SetLayoutParams(vp);
    AddView(mDivider);

    SetClickable(TRUE);

    UpdateTopPadding();
    return NOERROR;
}

void QSTileView::UpdateTopPadding()
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 padding = 0;
    res->GetDimensionPixelSize(R::dimen::qs_tile_padding_top, &padding);
    Int32 largePadding = 0;
    res->GetDimensionPixelSize(R::dimen::qs_tile_padding_top_large_text, &largePadding);

    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    Float fontScale = 0;
    config->GetFontScale(&fontScale);
    Float largeFactor = (MathUtils::Constrain(fontScale,
            1.0f, FontSizeUtils::LARGE_TEXT_SCALE) - 1.f) / (FontSizeUtils::LARGE_TEXT_SCALE - 1.f);
    mTilePaddingTopPx = Elastos::Core::Math::Round((1 - largeFactor) * padding + largeFactor * largePadding);
    RequestLayout();
}

ECode QSTileView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    ViewGroup::OnConfigurationChanged(newConfig);
    UpdateTopPadding();
    FontSizeUtils::UpdateFontSize(mLabel, R::dimen::qs_tile_text_size);
    if (mDualLabel != NULL) {
        Int32 v = 0;
        AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        res->GetDimensionPixelSize(R::dimen::qs_tile_text_size, &v);
        mDualLabel->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, v);
    }
    return NOERROR;
}

void QSTileView::RecreateLabel()
{
    AutoPtr<ICharSequence> labelText;
    AutoPtr<ICharSequence> labelDescription;
    if (mLabel != NULL) {
        mLabel->GetText((ICharSequence**)&labelText);
        RemoveView(IView::Probe(mLabel));
        mLabel = NULL;
    }
    if (mDualLabel != NULL) {
        labelText = NULL;
        String str;
        mDualLabel->GetText(&str);
        CString::New(str, (ICharSequence**)&labelText);
        IView::Probe(mLabel)->GetContentDescription((ICharSequence**)&labelDescription);
        RemoveView(IView::Probe(mDualLabel));
        mDualLabel = NULL;
    }
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    if (mDual) {
        mDualLabel = new QSDualTileLabel(mContext);
        IView::Probe(mDualLabel)->SetId(Elastos::Droid::R::id::title);
        IView::Probe(mDualLabel)->SetBackgroundResource(R::drawable::btn_borderless_rect);

        Int32 v = 0;
        res->GetColor(R::color::qs_tile_text, &v);
        mDualLabel->SetTextColor(v);
        IView::Probe(mDualLabel)->SetPadding(0, mDualTileVerticalPaddingPx, 0, mDualTileVerticalPaddingPx);
        mDualLabel->SetTypeface(CONDENSED);
        res->GetDimensionPixelSize(R::dimen::qs_tile_text_size, &v);
        mDualLabel->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, v);
        IView::Probe(mDualLabel)->SetClickable(TRUE);
        IView::Probe(mDualLabel)->SetOnClickListener(mClickSecondary);
        IView::Probe(mDualLabel)->SetFocusable(TRUE);
        if (labelText != NULL) {
            mDualLabel->SetText(labelText);
        }
        if (labelDescription != NULL) {
            IView::Probe(mDualLabel)->SetContentDescription(labelDescription);
        }
        AddView(IView::Probe(mDualLabel));
    }
    else {
        CTextView::New(mContext, (ITextView**)&mLabel);
        IView::Probe(mLabel)->SetId(Elastos::Droid::R::id::title);
        Int32 v = 0;
        res->GetColor(R::color::qs_tile_text, &v);
        mLabel->SetTextColor(v);
        mLabel->SetGravity(IGravity::CENTER_HORIZONTAL);
        mLabel->SetMinLines(2);
        IView::Probe(mLabel)->SetPadding(0, 0, 0, 0);
        mLabel->SetTypeface(CONDENSED);

        res->GetDimensionPixelSize(R::dimen::qs_tile_text_size, &v);
        mLabel->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, v);
        IView::Probe(mLabel)->SetClickable(FALSE);
        if (labelText != NULL) {
            mLabel->SetText(labelText);
        }
        AddView(IView::Probe(mLabel));
    }
}

ECode QSTileView::SetDual(
    /* [in] */ Boolean dual)
{
    const Boolean changed = dual != mDual;
    mDual = dual;
    if (changed) {
        RecreateLabel();
    }
    AutoPtr<IDrawable> tileBackground = GetTileBackground();
    IRippleDrawable* rippleDrawable = IRippleDrawable::Probe(tileBackground);
    if (rippleDrawable) {
        SetRipple(rippleDrawable);
    }
    if (dual) {
        mTopBackgroundView->SetOnClickListener(mClickPrimary);
        SetOnClickListener(NULL);
        SetClickable(FALSE);
        SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_NO);
        mTopBackgroundView->SetBackground(tileBackground);
    }
    else {
        mTopBackgroundView->SetOnClickListener(NULL);
        mTopBackgroundView->SetClickable(FALSE);
        SetOnClickListener(mClickPrimary);
        SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
        SetBackground(tileBackground);
    }
    mTopBackgroundView->SetFocusable(dual);
    SetFocusable(!dual);
    mDivider->SetVisibility(dual ? IView::VISIBLE : IView::GONE);
    PostInvalidate();
    return NOERROR;
}

void QSTileView::SetRipple(
    /* [in] */ IRippleDrawable* tileBackground)
{
    mRipple = tileBackground;
    Int32 w = 0;
    if ((GetWidth(&w), w) != 0) {
        Int32 h = 0;
        GetHeight(&h);
        UpdateRippleSize(w, h);
    }
}

ECode QSTileView::Init(
    /* [in] */ IViewOnClickListener* clickPrimary,
    /* [in] */ IViewOnClickListener* clickSecondary)
{
    mClickPrimary = clickPrimary;
    mClickSecondary = clickSecondary;
    return NOERROR;
}

AutoPtr<IView> QSTileView::CreateIcon()
{
    AutoPtr<IImageView> icon;
    CImageView::New(mContext, (IImageView**)&icon);
    IView::Probe(icon)->SetId(Elastos::Droid::R::id::icon);
    icon->SetScaleType(ImageViewScaleType_CENTER_INSIDE);
    return IView::Probe(icon);
}

AutoPtr<IDrawable> QSTileView::GetTileBackground()
{
    AutoPtr<ArrayOf<Int32> > attrs = ArrayOf<Int32>::Alloc(1);
    (*attrs)[0] = Elastos::Droid::R::attr::selectableItemBackgroundBorderless;
    AutoPtr<ITypedArray> ta;
    mContext->ObtainStyledAttributes(attrs, (ITypedArray**)&ta);
    AutoPtr<IDrawable> d;
    ta->GetDrawable(0, (IDrawable**)&d);
    ta->Recycle();
    return d;
}

AutoPtr<IView> QSTileView::LabelView()
{
    return mDual ? IView::Probe(mDualLabel) : IView::Probe(mLabel);
}

ECode QSTileView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    const Int32 w = MeasureSpec::GetSize(widthMeasureSpec);
    const Int32 h = MeasureSpec::GetSize(heightMeasureSpec);
    const Int32 iconSpec = Exactly(mIconSizePx);
    mIcon->Measure(MeasureSpec::MakeMeasureSpec(w, MeasureSpec::AT_MOST), iconSpec);
    LabelView()->Measure(widthMeasureSpec, MeasureSpec::MakeMeasureSpec(h, MeasureSpec::AT_MOST));
    if (mDual) {
        AutoPtr<IViewGroupLayoutParams> vp;
        mDivider->GetLayoutParams((IViewGroupLayoutParams**)&vp);
        Int32 height = 0;
        vp->GetHeight(&height);
        mDivider->Measure(widthMeasureSpec, Exactly(height));
    }
    Int32 heightSpec = Exactly(
            mIconSizePx + mTilePaddingBelowIconPx + mTilePaddingTopPx);
    mTopBackgroundView->Measure(widthMeasureSpec, heightSpec);
    SetMeasuredDimension(w, h);
    return NOERROR;
}

Int32 QSTileView::Exactly(
    /* [in] */ Int32 size)
{
    return View::MeasureSpec::MakeMeasureSpec(size, View::MeasureSpec::EXACTLY);
}

ECode QSTileView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Int32 w = 0;
    GetMeasuredWidth(&w);
    Int32 h = 0;
    GetMeasuredHeight(&h);

    Layout(mTopBackgroundView, 0, mTileSpacingPx);

    Int32 top = 0, tmp = 0;
    top += mTileSpacingPx;
    top += mTilePaddingTopPx;
    const Int32 iconLeft = (w - (mIcon->GetMeasuredWidth(&tmp), tmp)) / 2;
    Layout(mIcon, iconLeft, top);
    if (mRipple != NULL) {
        UpdateRippleSize(w, h);
    }
    mIcon->GetBottom(&top);
    top += mTilePaddingBelowIconPx;
    if (mDual) {
        Layout(mDivider, 0, top);
        mDivider->GetBottom(&top);
    }
    Layout(LabelView(), 0, top);
    return NOERROR;
}

void QSTileView::UpdateRippleSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    // center the touch feedback on the center of the icon, and dial it down a bit
    const Int32 cx = width / 2;
    Int32 t = 0, h = 0;
    const Int32 cy = mDual ? (mIcon->GetTop(&t), t) + (mIcon->GetHeight(&h), h) / 2 : height / 2;
    const Int32 rad = (Int32)((mIcon->GetHeight(&h), h) * 1.25f);
    IDrawable::Probe(mRipple)->SetHotspotBounds(cx - rad, cy - rad, cx + rad, cy + rad);
}

void QSTileView::Layout(
    /* [in] */ IView* child,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top)
{
    Int32 w = 0, h = 0;
    child->Layout(left, top, left + (child->GetMeasuredWidth(&w), w)
        , top + (child->GetMeasuredHeight(&h), h));
}

ECode QSTileView::HandleStateChanged(
    /* [in] */ IQSTileState* state)
{
    if (IImageView::Probe(mIcon)) {
        AutoPtr<IImageView> iv = IImageView::Probe(mIcon);
        AutoPtr<IDrawable> icon = ((QSTile::State*)state)->mIcon;
        Int32 iconId = ((QSTile::State*)state)->mIconId;
        if (icon != NULL) {
            iv->SetImageDrawable(icon);
        }
        else if (iconId > 0) {
            iv->SetImageResource(iconId);
        }
        AutoPtr<IDrawable> drawable;
        iv->GetDrawable((IDrawable**)&drawable);

        Boolean autoMirrorDrawable = ((QSTile::State*)state)->mAutoMirrorDrawable;
        if (autoMirrorDrawable && drawable != NULL) {
            drawable->SetAutoMirrored(TRUE);
        }
    }
    if (mDual) {
        AutoPtr<ICharSequence> v;
        CString::New(((QSTile::State*)state)->mLabel, (ICharSequence**)&v);
        mDualLabel->SetText(v);
        v = NULL;
        CString::New(((QSTile::State*)state)->mDualLabelContentDescription, (ICharSequence**)&v);
        IView::Probe(mDualLabel)->SetContentDescription(v);
        v = NULL;
        CString::New(((QSTile::State*)state)->mContentDescription, (ICharSequence**)&v);
        mTopBackgroundView->SetContentDescription(v);
    }
    else {
        AutoPtr<ICharSequence> v;
        CString::New(((QSTile::State*)state)->mLabel, (ICharSequence**)&v);
        mLabel->SetText(v);
        v = NULL;
        CString::New(((QSTile::State*)state)->mContentDescription, (ICharSequence**)&v);
        SetContentDescription(v);
    }

    return NOERROR;
}

ECode QSTileView::OnStateChanged(
    /* [in] */ IQSTileState* state)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(H::STATE_CHANGED, state, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
