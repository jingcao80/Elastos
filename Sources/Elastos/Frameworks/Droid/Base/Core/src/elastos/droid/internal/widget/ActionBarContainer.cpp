
#include "elastos/droid/internal/widget/ActionBarContainer.h"
#include "elastos/droid/internal/widget/CActionBarBackgroundDrawable.h"
#include "elastos/droid/R.h"
#include "elastos/core/Math.h"

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Internal::Widget::EIID_IActionBarContainer;
using Elastos::Droid::Internal::Widget::IResolverDrawerLayout;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

//=====================================================================
//           ActionBarContainer::ActionBarBackgroundDrawable
//=====================================================================
ECode ActionBarContainer::ActionBarBackgroundDrawable::constructor(
    /* [in] */ IActionBarContainer* owner)
{
    mOwner = (ActionBarContainer*)owner;
    return NOERROR;
}

ECode ActionBarContainer::ActionBarBackgroundDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    if (mOwner->mIsSplit) {
        if (mOwner->mSplitBackground != NULL) {
            mOwner->mSplitBackground->Draw(canvas);
        }
    }
    else {
        if (mOwner->mBackground != NULL) {
            mOwner->mBackground->Draw(canvas);
        }
        if (mOwner->mStackedBackground != NULL && mOwner->mIsStacked) {
            mOwner->mStackedBackground->Draw(canvas);
        }
    }
    return NOERROR;
}

ECode ActionBarContainer::ActionBarBackgroundDrawable::GetOutline(
    /* [in] */ IOutline* outline)
{
    if (mOwner->mIsSplit) {
        if (mOwner->mSplitBackground != NULL) {
            mOwner->mSplitBackground->GetOutline(outline);
        }
    }
    else {
        // ignore the stacked background for shadow casting
        if (mOwner->mBackground != NULL) {
            mOwner->mBackground->GetOutline(outline);
        }
    }
    return NOERROR;
}

ECode ActionBarContainer::ActionBarBackgroundDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return NOERROR;
}

ECode ActionBarContainer::ActionBarBackgroundDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return NOERROR;
}

ECode ActionBarContainer::ActionBarBackgroundDrawable::GetOpacity(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mOwner->mIsSplit) {
        Int32 opacity = 0;
        if (mOwner->mSplitBackground != NULL &&
            (mOwner->mSplitBackground->GetOpacity(&opacity), opacity == IPixelFormat::OPAQUE)) {
            *result = IPixelFormat::OPAQUE;
            return NOERROR;
        }
    }
    else {
        Int32 opacity = 0;
        if (mOwner->mIsStacked && (mOwner->mStackedBackground == NULL ||
            (mOwner->mStackedBackground->GetOpacity(&opacity), opacity != IPixelFormat::OPAQUE))) {
            *result = IPixelFormat::UNKNOWN;
            return NOERROR;
        }

        if (!mOwner->IsCollapsed(mOwner->mActionBarView) && mOwner->mBackground != NULL &&
            (mOwner->mBackground->GetOpacity(&opacity), opacity == IPixelFormat::OPAQUE)) {
            *result = IPixelFormat::OPAQUE;
            return NOERROR;
        }
    }

    *result = IPixelFormat::UNKNOWN;
    return NOERROR;
}

//=====================================================================
//                          ActionBarContainer
//=====================================================================
CAR_INTERFACE_IMPL(ActionBarContainer, FrameLayout, IActionBarContainer)

ActionBarContainer::ActionBarContainer()
    : mIsTransitioning(FALSE)
    , mIsSplit(FALSE)
    , mIsStacked(FALSE)
    , mHeight(0)
{
}

ECode ActionBarContainer::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode ActionBarContainer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(FrameLayout::constructor(context, attrs));

    AutoPtr<IDrawable> drawable;
    CActionBarBackgroundDrawable::New(this, (IDrawable**)&drawable);
    SetBackgroundDrawable(drawable);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::ActionBar);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    a->GetDrawable(R::styleable::ActionBar_background, (IDrawable**)&mBackground);
    a->GetDrawable(R::styleable::ActionBar_backgroundStacked, (IDrawable**)&mStackedBackground);
    a->GetDimensionPixelSize(R::styleable::ActionBar_height, -1, &mHeight);

    Int32 id = 0;
    GetId(&id);
    if (id == R::id::split_action_bar) {
        mIsSplit = TRUE;
        a->GetDrawable(R::styleable::ActionBar_backgroundSplit, (IDrawable**)&mSplitBackground);
    }

    a->Recycle();
    SetWillNotDraw(mIsSplit ? mSplitBackground == NULL :
            mBackground == NULL && mStackedBackground == NULL);
    return NOERROR;
}

ECode ActionBarContainer::OnFinishInflate()
{
    FAIL_RETURN(FrameLayout::OnFinishInflate());
    mActionBarView = NULL;
    FindViewById(R::id::action_bar, (IView**)&mActionBarView);
    mActionContextView = NULL;
    FindViewById(R::id::action_context_bar, (IView**)&mActionContextView);
    return NOERROR;
}

ECode ActionBarContainer::SetPrimaryBackground(
    /* [in] */ IDrawable* bg)
{
    if (mBackground != NULL) {
        mBackground->SetCallback(NULL);
        UnscheduleDrawable(mBackground);
    }
    mBackground = bg;
    if (bg != NULL) {
        bg->SetCallback((IDrawableCallback*)this);
        if (mActionBarView != NULL) {
            Int32 left = 0, top = 0, right = 0, bottom = 0;
            mActionBarView->GetLeft(&left);
            mActionBarView->GetTop(&top);
            mActionBarView->GetRight(&right);
            mActionBarView->GetBottom(&bottom);
            mBackground->SetBounds(left, top, right, bottom);
        }
    }
    SetWillNotDraw(mIsSplit ? mSplitBackground == NULL :
            mBackground == NULL && mStackedBackground == NULL);
    Invalidate();
    return NOERROR;
}

ECode ActionBarContainer::SetStackedBackground(
    /* [in] */ IDrawable* bg)
{
    if (mStackedBackground != NULL) {
        mStackedBackground->SetCallback(NULL);
        UnscheduleDrawable(mStackedBackground);
    }
    mStackedBackground = bg;
    if (bg != NULL) {
        bg->SetCallback((IDrawableCallback*)this);
        if (mIsStacked && mStackedBackground != NULL) {
            Int32 left = 0, top = 0, right = 0, bottom = 0;
            mTabContainer->GetLeft(&left);
            mTabContainer->GetTop(&top);
            mTabContainer->GetRight(&right);
            mTabContainer->GetBottom(&bottom);
            mStackedBackground->SetBounds(left, top, right, bottom);
        }
    }
    SetWillNotDraw(mIsSplit ? mSplitBackground == NULL :
            mBackground == NULL && mStackedBackground == NULL);
    Invalidate();
    return NOERROR;
}

ECode ActionBarContainer::SetSplitBackground(
    /* [in] */ IDrawable* bg)
{
    if (mSplitBackground != NULL) {
        mSplitBackground->SetCallback(NULL);
        UnscheduleDrawable(mSplitBackground);
    }
    mSplitBackground = bg;
    if (bg != NULL) {
        bg->SetCallback((IDrawableCallback*)this);
        if (mIsSplit && mSplitBackground != NULL) {
            Int32 measureWidth = 0, measureHeight = 0;
            GetMeasuredWidth(&measureWidth);
            GetMeasuredHeight(&measureHeight);
            mSplitBackground->SetBounds(0, 0, measureWidth, measureHeight);
        }
    }
    SetWillNotDraw(mIsSplit ? mSplitBackground == NULL :
            mBackground == NULL && mStackedBackground == NULL);
    Invalidate();
    return NOERROR;
}

ECode ActionBarContainer::SetVisibility(
    /* [in] */ Int32 visibility)
{
    FAIL_RETURN(FrameLayout::SetVisibility(visibility));
    Boolean isVisible = visibility == IView::VISIBLE;
    Boolean res = FALSE;
    if (mBackground != NULL) {
        mBackground->SetVisible(isVisible, FALSE, &res);
    }
    if (mStackedBackground != NULL) {
        mStackedBackground->SetVisible(isVisible, FALSE, &res);
    }
    if (mSplitBackground != NULL) {
        mSplitBackground->SetVisible(isVisible, FALSE, &res);
    }
    return NOERROR;
}

ECode ActionBarContainer::JumpDrawablesToCurrentState()
{
    FAIL_RETURN(FrameLayout::JumpDrawablesToCurrentState());
    if (mBackground != NULL) {
        mBackground->JumpToCurrentState();
    }
    if (mStackedBackground != NULL) {
        mStackedBackground->JumpToCurrentState();
    }
    if (mSplitBackground != NULL) {
        mSplitBackground->JumpToCurrentState();
    }
    return NOERROR;
}

ECode ActionBarContainer::OnResolveDrawables(
    /* [in] */ Int32 layoutDirection)
{
    FAIL_RETURN(FrameLayout::OnResolveDrawables(layoutDirection));
    if (mBackground != NULL) {
        mBackground->SetLayoutDirection(layoutDirection);
    }
    if (mStackedBackground != NULL) {
        mStackedBackground->SetLayoutDirection(layoutDirection);
    }
    if (mSplitBackground != NULL) {
        mSplitBackground->SetLayoutDirection(layoutDirection);
    }
    return NOERROR;
}

ECode ActionBarContainer::SetTransitioning(
    /* [in] */ Boolean isTransitioning)
{
    mIsTransitioning = isTransitioning;
    SetDescendantFocusability(isTransitioning ? FOCUS_BLOCK_DESCENDANTS
            : FOCUS_AFTER_DESCENDANTS);
    return NOERROR;
}

ECode ActionBarContainer::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean res;
    *result = mIsTransitioning || (FrameLayout::OnInterceptTouchEvent(ev, &res), res);
    return NOERROR;
}

ECode ActionBarContainer::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    FrameLayout::OnTouchEvent(ev, result);

    // An action bar always eats hover events.
    *result = TRUE;
    return NOERROR;
}

ECode ActionBarContainer::OnHoverEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    FrameLayout::OnHoverEvent(ev, result);

    // An action bar always eats hover events.
    *result = TRUE;
    return NOERROR;
}

ECode ActionBarContainer::SetTabContainer(
    /* [in] */ IScrollingTabContainerView* tabView)
{
    if (mTabContainer != NULL) {
        RemoveView(mTabContainer);
    }
    mTabContainer = IView::Probe(tabView);
    if (tabView != NULL) {
        IView* view = IView::Probe(tabView);
        AddView(view);
        AutoPtr<IViewGroupLayoutParams> lp;
        view->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        lp->SetWidth(IViewGroupLayoutParams::MATCH_PARENT);
        lp->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);
        tabView->SetAllowCollapse(FALSE);
    }
    return NOERROR;
}

ECode ActionBarContainer::GetTabContainer(
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTabContainer;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ActionBarContainer::StartActionModeForChild(
    /* [in] */ IView* child,
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** result)
{
    VALIDATE_NOT_NULL(result)
    // No starting an action mode for an action bar child! (Where would it go?)
    *result = NULL;
    return NOERROR;
}

ECode ActionBarContainer::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    using Elastos::Core::Math;
    if (mActionBarView == NULL
        && View::MeasureSpec::GetMode(heightMeasureSpec) == View::MeasureSpec::AT_MOST && mHeight >= 0) {
        heightMeasureSpec = View::MeasureSpec::MakeMeasureSpec(Math::Min(mHeight,
                View::MeasureSpec::GetSize(heightMeasureSpec)), View::MeasureSpec::AT_MOST);
    }
    FrameLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    if (mActionBarView == NULL) return NOERROR;

    Int32 visible = 0;
    if (mTabContainer != NULL && (mTabContainer->GetVisibility(&visible), visible != IView::GONE)) {
        Int32 nonTabMaxHeight = 0;
        Int32 childCount = 0;
        GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; ++i) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            if (child == mTabContainer) {
                continue;
            }
            nonTabMaxHeight = Math::Max(nonTabMaxHeight, IsCollapsed(child) ? 0 :
                    GetMeasuredHeightWithMargins(child));
        }
        Int32 mode = View::MeasureSpec::GetMode(heightMeasureSpec);
        Int32 maxHeight = mode == View::MeasureSpec::AT_MOST ?
                View::MeasureSpec::GetSize(heightMeasureSpec) : Math::INT32_MAX_VALUE;
        Int32 measureWidth = 0;
        GetMeasuredWidth(&measureWidth);
        SetMeasuredDimension(measureWidth,
                Math::Min(nonTabMaxHeight + GetMeasuredHeightWithMargins(mTabContainer),
                        maxHeight));
    }
    return NOERROR;
}

ECode ActionBarContainer::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    FAIL_RETURN(FrameLayout::OnLayout(changed, l, t, r, b));

    AutoPtr<IView> tabContainer = mTabContainer;
    Int32 visibility = 0;
    Boolean hasTabs = tabContainer != NULL &&
            (tabContainer->GetVisibility(&visibility), visibility != IView::GONE);

    if (hasTabs) {
        Int32 containerHeight = 0;
        GetMeasuredHeight(&containerHeight);
        AutoPtr<IViewGroupLayoutParams> lp;
        IView::Probe(tabContainer)->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        Int32 tabHeight = 0;
        tabContainer->GetMeasuredHeight(&tabHeight);
        Int32 bottomMargin = 0;
        IViewGroupMarginLayoutParams::Probe(lp)->GetBottomMargin(&bottomMargin);
        tabContainer->Layout(l, containerHeight - tabHeight - bottomMargin, r,
                containerHeight - bottomMargin);
    }

    Boolean needsInvalidate = FALSE;
    if (mIsSplit) {
        if (mSplitBackground != NULL) {
            Int32 measureWidth = 0, measureHeight = 0;
            GetMeasuredWidth(&measureWidth);
            GetMeasuredHeight(&measureHeight);
            mSplitBackground->SetBounds(0, 0, measureWidth, measureHeight);
            needsInvalidate = TRUE;
        }
    }
    else {
        Int32 left = 0, top = 0, right = 0, bottom = 0;
        if (mBackground != NULL) {
            mActionBarView->GetVisibility(&visibility);
            if (visibility == IView::VISIBLE) {
                mActionBarView->GetLeft(&left);
                mActionBarView->GetTop(&top);
                mActionBarView->GetRight(&right);
                mActionBarView->GetBottom(&bottom);
                mBackground->SetBounds(left, top, right, bottom);
            }
            else if (mActionContextView != NULL &&
                    (mActionContextView->GetVisibility(&visibility), visibility == IView::VISIBLE)) {
                mActionContextView->GetLeft(&left);
                mActionContextView->GetTop(&top);
                mActionContextView->GetRight(&right);
                mActionContextView->GetBottom(&bottom);
                mBackground->SetBounds(left, top, right, bottom);
            }
            else {
                mBackground->SetBounds(0, 0, 0, 0);
            }
            needsInvalidate = TRUE;
        }
        mIsStacked = hasTabs;
        if (hasTabs && mStackedBackground != NULL) {
            tabContainer->GetLeft(&left);
            tabContainer->GetTop(&top);
            tabContainer->GetRight(&right);
            tabContainer->GetBottom(&bottom);
            mStackedBackground->SetBounds(left, top, right, bottom);
            needsInvalidate = TRUE;
        }
    }

    if (needsInvalidate) {
        Invalidate();
    }
    return NOERROR;
}

Boolean ActionBarContainer::VerifyDrawable(
    /* [in] */ IDrawable* who)
{
    return (who == mBackground.Get() && !mIsSplit) || (who == mStackedBackground.Get() && mIsStacked) ||
        (who == mSplitBackground.Get() && mIsSplit) || FrameLayout::VerifyDrawable(who);
}

ECode ActionBarContainer::DrawableStateChanged()
{
    FAIL_RETURN(FrameLayout::DrawableStateChanged());

    AutoPtr< ArrayOf<Int32> > drawableState;
    GetDrawableState((ArrayOf<Int32>**)&drawableState);
    Boolean res = FALSE;
    if (mBackground != NULL && (mBackground->IsStateful(&res), res)) {
        mBackground->SetState(drawableState, &res);
    }
    if (mStackedBackground != NULL && (mStackedBackground->IsStateful(&res), res)) {
        mStackedBackground->SetState(drawableState, &res);
    }
    if (mSplitBackground != NULL && (mSplitBackground->IsStateful(&res), res)) {
        mSplitBackground->SetState(drawableState, &res);
    }
    return NOERROR;
}

Boolean ActionBarContainer::IsCollapsed(
    /* [in] */ IView* view)
{
    Int32 visibility = 0;
    Int32 measureHeight = 0;
    return view == NULL
        || (view->GetVisibility(&visibility), visibility == IView::GONE)
        || (view->GetMeasuredHeight(&measureHeight), measureHeight == 0);
}

Int32 ActionBarContainer::GetMeasuredHeightWithMargins(
    /* [in] */ IView* view)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    view->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    IViewGroupMarginLayoutParams* vgmlp = IViewGroupMarginLayoutParams::Probe(lp);

    Int32 measureHeight = 0, topMargin = 0, bottomMargin = 0;
    view->GetMeasuredHeight(&measureHeight);
    vgmlp->GetTopMargin(&topMargin);
    vgmlp->GetBottomMargin(&bottomMargin);
    return measureHeight + topMargin + bottomMargin;
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos


