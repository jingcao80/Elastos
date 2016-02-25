
#include "elastos/droid/systemui/statusbar/phone/IconMerger.h"
#include "../../R.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

IconMerger::IconRunnable::IconRunnable(
    /* [in] */ IconMerger* host,
    /* [in] */ Boolean moreRequired)
    : mHost(host)
    , mMoreRequired(moreRequired)
{}

ECode IconMerger::IconRunnable::Run()
{
    mHost->mMoreView->SetVisibility(mMoreRequired ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

const String IconMerger::TAG("IconMerger");
const Boolean IconMerger::DEBUG = FALSE;
CAR_INTERFACE_IMPL(IconMerger, LinearLayout, IIconMerger);
IconMerger::IconMerger()
    : mIconSize(0)
{}

ECode IconMerger::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::constructor(context, attrs);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::status_bar_icon_size, &mIconSize);

    if (DEBUG) {
        SetBackgroundColor(0x800099FF);
    }
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
    Int32 width = 0, height = 0;
    GetMeasuredWidth(&width);
    SetMeasuredDimension(width - (width % mIconSize), (GetMeasuredHeight(&height), height));
}

ECode IconMerger::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    LinearLayout::OnLayout(changed, l, t, r, b);
    CheckOverflow(r - l);
    return NOERROR;
}

void IconMerger::CheckOverflow(
    /* [in] */ Int32 width)
{
    if (mMoreView == NULL) return;

    Int32 N = 0;
    GetChildCount(&N);
    Int32 visibleChildren = 0, visibility = 0;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        child->GetVisibility(&visibility);
        if (visibility != IView::GONE) visibleChildren++;
    }

    mMoreView->GetVisibility(&visibility);
    Boolean overflowShown = (visibility == IView::VISIBLE);
    // let's assume we have one more slot if the more icon is already showing
    if (overflowShown) visibleChildren --;
    Boolean moreRequired = visibleChildren * mIconSize > width;
    if (moreRequired != overflowShown) {
        AutoPtr<IRunnable> run = new IconRunnable(this, moreRequired);
        Boolean tmp = FALSE;
        Post(run, &tmp);
    }
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
