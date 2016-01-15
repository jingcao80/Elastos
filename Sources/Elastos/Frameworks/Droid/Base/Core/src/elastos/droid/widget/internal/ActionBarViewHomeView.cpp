#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/widget/internal/ActionBarViewHomeView.h"
#include "elastos/droid/widget/CFrameLayoutLayoutParams.h"
#include <elastos/core/Math.h>

using Elastos::Core::Math;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

//fe4e85ad-2d3a-4404-ad58-039cdcf0943c
extern "C" const InterfaceID EIID_ActionBarViewHomeView =
        { 0xfe4e85ad, 0x2d3a, 0x4404, { 0xad, 0x58, 0x03, 0x9c, 0xdc, 0xf0, 0x94, 0x3c } };

const Int64 ActionBarViewHomeView::DEFAULT_TRANSITION_DURATION;

ActionBarViewHomeView::ActionBarViewHomeView()
    :mUpWidth(0)
{
}

ActionBarViewHomeView::ActionBarViewHomeView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    :mUpWidth(0)
{
    Init(context, attrs);
}

ECode ActionBarViewHomeView::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::Init(context, attrs);
    AutoPtr<ILayoutTransition> t = GetLayoutTransition();
    if (t) {
        // Set a lower duration than the default
        t->SetDuration(DEFAULT_TRANSITION_DURATION);
    }
    return NOERROR;
}

ECode ActionBarViewHomeView::SetUp(
    /* [in] */ Boolean isUp)
{
    mUpView->SetVisibility(isUp ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode ActionBarViewHomeView::SetIcon(
    /* [in] */ IDrawable* icon)
{
    mIconView->SetImageDrawable(icon);
    return NOERROR;
}

Boolean ActionBarViewHomeView::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    OnPopulateAccessibilityEvent(event);
    return true;
}

CARAPI ActionBarViewHomeView::OnPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    FrameLayout::OnPopulateAccessibilityEvent(event);
    AutoPtr<ICharSequence> cdesc = GetContentDescription();
    if (!TextUtils::IsEmpty(cdesc)) {
        AutoPtr<IObjectContainer> container;
        event->GetText((IObjectContainer**)&container);
        container->Add(cdesc);
    }
    return NOERROR;
}

Boolean ActionBarViewHomeView::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    return OnHoverEvent(event);
}

CARAPI ActionBarViewHomeView::OnFinishInflate()
{
    mUpView = FindViewById(R::id::up);
    AutoPtr<IView> v = FindViewById(R::id::home);
    mIconView = IImageView::Probe(v);
    return NOERROR;
}

Int32 ActionBarViewHomeView::GetStartOffset()
{
    Int32 visibility = 0;
    mUpView->GetVisibility(&visibility);
    if (visibility == IView::GONE) {
        return mUpWidth;
    } else {
        return 0;
    }
}

void ActionBarViewHomeView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    MeasureChildWithMargins(mUpView, widthMeasureSpec, 0, heightMeasureSpec, 0);
    AutoPtr<IViewGroupLayoutParams> lp;
    mUpView->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    AutoPtr<CFrameLayoutLayoutParams> upLp = (CFrameLayoutLayoutParams*)lp.Get();
    Int32 uw = 0;
    mUpView->GetMeasuredWidth(&uw);
    mUpWidth = upLp->mLeftMargin + uw + upLp->mRightMargin;
    Int32 uvb = 0;
    mUpView->GetVisibility(&uvb);
    Int32 width = uvb == IView::GONE ? 0 : mUpWidth;
    Int32 uh = 0;
    mUpView->GetMeasuredHeight(&uh);
    Int32 height = upLp->mTopMargin + uh + upLp->mBottomMargin;
    MeasureChildWithMargins(mIconView, widthMeasureSpec, width, heightMeasureSpec, 0);

    AutoPtr<IViewGroupLayoutParams> ilp;
    mIconView->GetLayoutParams((IViewGroupLayoutParams**)&ilp);
    AutoPtr<CFrameLayoutLayoutParams> iconLp = (CFrameLayoutLayoutParams*)ilp.Get();
    Int32 iw = 0;
    mIconView->GetMeasuredWidth(&iw);
    width += iconLp->mLeftMargin + iw + iconLp->mRightMargin;
    Int32 ih = 0;
    mIconView->GetMeasuredHeight(&ih);
    height = Elastos::Core::Math::Max(height, iconLp->mTopMargin + ih + iconLp->mBottomMargin);

    Int32 widthMode = MeasureSpec::GetMode(widthMeasureSpec);
    Int32 heightMode = MeasureSpec::GetMode(heightMeasureSpec);
    Int32 widthSize = MeasureSpec::GetSize(widthMeasureSpec);
    Int32 heightSize = MeasureSpec::GetSize(heightMeasureSpec);

    switch (widthMode) {
        case MeasureSpec::AT_MOST:
            width = Elastos::Core::Math::Min(width, widthSize);
            break;
        case MeasureSpec::EXACTLY:
            width = widthSize;
            break;
        case MeasureSpec::UNSPECIFIED:
        default:
            break;
    }
    switch (heightMode) {
        case MeasureSpec::AT_MOST:
            height = Elastos::Core::Math::Min(height, heightSize);
            break;
        case MeasureSpec::EXACTLY:
            height = heightSize;
            break;
        case MeasureSpec::UNSPECIFIED:
        default:
            break;
    }
    SetMeasuredDimension(width, height);
}

void ActionBarViewHomeView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Int32 vCenter = (b - t) / 2;
    Boolean isLayoutRtl = IsLayoutRtl();
    Int32 width = GetWidth();
    Int32 upOffset = 0;
    Int32 visibility = 0;

    if ((mUpView->GetVisibility(&visibility), visibility) != IView::GONE) {
        AutoPtr<IViewGroupLayoutParams> lp;
        mUpView->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<CFrameLayoutLayoutParams> upLp = (CFrameLayoutLayoutParams*)lp.Get();
        Int32 upHeight = 0;
        mUpView->GetMeasuredHeight(&upHeight);
        Int32 upWidth = 0;
        mUpView->GetMeasuredWidth(&upWidth);
        upOffset = upLp->mLeftMargin + upWidth + upLp->mRightMargin;
        Int32 upTop = vCenter - upHeight / 2;
        Int32 upBottom = upTop + upHeight;
        Int32 upRight;
        Int32 upLeft;
        if (isLayoutRtl) {
            upRight = width;
            upLeft = upRight - upWidth;
            r -= upOffset;
        } else {
            upRight = upWidth;
            upLeft = 0;
            l += upOffset;
        }
        mUpView->Layout(upLeft, upTop, upRight, upBottom);
    }

    AutoPtr<IViewGroupLayoutParams> ilp;
    mIconView->GetLayoutParams((IViewGroupLayoutParams**)&ilp);
    AutoPtr<CFrameLayoutLayoutParams> iconLp = (CFrameLayoutLayoutParams*)ilp.Get();
    Int32 iconHeight = 0;
    mIconView->GetMeasuredHeight(&iconHeight);
    Int32 iconWidth = 0;
    mIconView->GetMeasuredWidth(&iconWidth);
    Int32 hCenter = (r - l) / 2;
    Int32 iconTop = Elastos::Core::Math::Max(iconLp->mTopMargin, vCenter - iconHeight / 2);
    Int32 iconBottom = iconTop + iconHeight;
    Int32 iconLeft;
    Int32 iconRight;
    Int32 marginStart = 0;
    iconLp->GetMarginStart(&marginStart);
    Int32 delta = Elastos::Core::Math::Max(marginStart, hCenter - iconWidth / 2);
    if (isLayoutRtl) {
        iconRight = width - upOffset - delta;
        iconLeft = iconRight - iconWidth;
    } else {
        iconLeft = upOffset + delta;
        iconRight = iconLeft + iconWidth;
    }
    mIconView->Layout(iconLeft, iconTop, iconRight, iconBottom);
}

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos