#include "elastos/droid/systemui/statusbar/phone/IconMerger.h"
#include "elastos/droid/systemui/SystemUIR.h"

using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {


IconMerger::IconMerger()
    : mIconSize(0)
{
}

IconMerger::IconMerger(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : LinearLayout(context, attrs)
    , mIconSize(0)
{
    ASSERT_SUCCEEDED(InitIconSize(context, attrs));
}

ECode IconMerger::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ASSERT_SUCCEEDED(LinearLayout::Init(context, attrs));
    ASSERT_SUCCEEDED(InitIconSize(context, attrs));
    return NOERROR;
}

ECode IconMerger::InitIconSize(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_icon_size, &mIconSize);

    // if (DEBUG) {
    //     setBackgroundColor(0x800099FF);
    // }
    return NOERROR;
}

ECode IconMerger::SetOverflowIndicator(
    /* [in] */ IView* v)
{
    mMoreView = v;
    return NOERROR;
}

void IconMerger::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    LinearLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    // we need to constrain this to an integral multiple of our children
    Int32 width = GetMeasuredWidth();
    SetMeasuredDimension(width - (width % mIconSize), GetMeasuredHeight());
}

void IconMerger::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    LinearLayout::OnLayout(changed, l, t, r, b);
    CheckOverflow(r - l);
}

void IconMerger::CheckOverflow(
    /* [in] */ Int32 width)
{
    if (mMoreView == NULL) return;

        Int32 N = GetChildCount();
        Int32 visibleChildren = 0, visibility;
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IView> childView = GetChildAt(i);
            childView->GetVisibility(&visibility);
            if (visibility != IView::GONE) visibleChildren++;
        }

        mMoreView->GetVisibility(&visibility);
        Boolean overflowShown = (visibility == IView::VISIBLE);
        // let's assume we have one more slot if the more icon is already showing
        if (overflowShown) visibleChildren--;
        Boolean moreRequired = visibleChildren * mIconSize > width;
        if (moreRequired != overflowShown) {
            AutoPtr<IRunnable> runnable = new SetVisibilityRunnable(moreRequired, this);
            Post(runnable);
        }
}

//=================================================================================
//            SetVisibilityRunnable
//=================================================================================
SetVisibilityRunnable::SetVisibilityRunnable(
    /* [in] */ Boolean moreRequired,
    /* [in] */ IconMerger* host)
    : mMoreRequired(moreRequired)
    , mHost(host)
{
}

ECode SetVisibilityRunnable::Run()
{
    mHost->mMoreView->SetVisibility(mMoreRequired ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
