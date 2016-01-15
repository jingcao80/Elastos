
#include "elastos/droid/internal/widget/ActionBarContainer.h"
#include "elastos/droid/R.h"
#include "elastos/droid/widget/CFrameLayoutLayoutParams.h"
#include "elastos/core/Math.h"

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Internal::Widget::EIID_IActionBarContainer;
using Elastos::Droid::Internal::Widget::IResolverDrawerLayout;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

//=====================================================================
//           ActionBarContainer::ActionBarBackgroundDrawable
//=====================================================================
ActionBarContainer::ActionBarBackgroundDrawable::ActionBarBackgroundDrawable(
    /* [in] */ ActionBarContainer* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode ActionBarContainer::ActionBarBackgroundDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    VALIDATE_NOT_NULL(canvas);
    // ==================before translated======================
    // if (mIsSplit) {
    //     if (mSplitBackground != null) {
    //         mSplitBackground.draw(canvas);
    //     }
    // } else {
    //     if (mBackground != null) {
    //         mBackground.draw(canvas);
    //     }
    //     if (mStackedBackground != null && mIsStacked) {
    //         mStackedBackground.draw(canvas);
    //     }
    // }

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
    // ==================before translated======================
    // if (mIsSplit) {
    //     if (mSplitBackground != null) {
    //         mSplitBackground.getOutline(outline);
    //     }
    // } else {
    //     // ignore the stacked background for shadow casting
    //     if (mBackground != null) {
    //         mBackground.getOutline(outline);
    //     }
    // }

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
    // ==================before translated======================
    // if (mIsSplit) {
    //     if (mSplitBackground != null
    //             && mSplitBackground.getOpacity() == PixelFormat.OPAQUE) {
    //         return PixelFormat.OPAQUE;
    //     }
    // } else {
    //     if (mIsStacked && (mStackedBackground == null
    //             || mStackedBackground.getOpacity() != PixelFormat.OPAQUE)) {
    //         return PixelFormat.UNKNOWN;
    //     }
    //     if (!isCollapsed(mActionBarView) && mBackground != null
    //             && mBackground.getOpacity() == PixelFormat.OPAQUE) {
    //         return PixelFormat.OPAQUE;
    //     }
    // }
    //
    // return PixelFormat.UNKNOWN;

    if (mOwner->mIsSplit) {
        Int32 opacity = 0;
        mOwner->mSplitBackground->GetOpacity(&opacity);
        if (mOwner->mSplitBackground != NULL && opacity == IPixelFormat::OPAQUE) {
            *result = IPixelFormat::OPAQUE;
            return NOERROR;
        }
    }
    else {
        Int32 opacity = 0;
        mOwner->mStackedBackground->GetOpacity(&opacity);
        if (mOwner->mIsStacked && (mOwner->mStackedBackground == NULL || opacity != IPixelFormat::OPAQUE)) {
            *result = IPixelFormat::UNKNOWN;
            return NOERROR;
        }

        mOwner->mBackground->GetOpacity(&opacity);
        if (!mOwner->IsCollapsed(mOwner->mActionBarView) && mOwner->mBackground != NULL && opacity == IPixelFormat::OPAQUE) {
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
    // ==================before translated======================
    // this(context, null);

    return constructor(context, NULL);
}

ECode ActionBarContainer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    // ==================before translated======================
    // super(context, attrs);
    //
    // // Set a transparent background so that we project appropriately.
    // setBackground(new ActionBarBackgroundDrawable());
    //
    // TypedArray a = context.obtainStyledAttributes(attrs,
    //         com.android.internal.R.styleable.ActionBar);
    // mBackground = a.getDrawable(com.android.internal.R.styleable.ActionBar_background);
    // mStackedBackground = a.getDrawable(
    //         com.android.internal.R.styleable.ActionBar_backgroundStacked);
    // mHeight = a.getDimensionPixelSize(com.android.internal.R.styleable.ActionBar_height, -1);
    //
    // if (getId() == com.android.internal.R.id.split_action_bar) {
    //     mIsSplit = true;
    //     mSplitBackground = a.getDrawable(
    //             com.android.internal.R.styleable.ActionBar_backgroundSplit);
    // }
    // a.recycle();
    //
    // setWillNotDraw(mIsSplit ? mSplitBackground == null :
    //         mBackground == null && mStackedBackground == null);

    FrameLayout::constructor(context, attrs);
    AutoPtr<IDrawable> drawable = new ActionBarBackgroundDrawable(this);
    SetBackgroundDrawable(drawable);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32*>(R::styleable::ActionBar),
        ArraySize(R::styleable::ActionBar));
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
    SetWillNotDraw(mIsSplit ? mSplitBackground == NULL : mBackground == NULL && mStackedBackground == NULL);
    return NOERROR;
}

ECode ActionBarContainer::OnFinishInflate()
{
    // ==================before translated======================
    // super.onFinishInflate();
    // mActionBarView = findViewById(com.android.internal.R.id.action_bar);
    // mActionContextView = findViewById(com.android.internal.R.id.action_context_bar);

    FrameLayout::OnFinishInflate();
    FindViewById(R::id::action_bar, (IView**)&mActionBarView);
    FindViewById(R::id::action_context_bar, (IView**)&mActionContextView);
    return NOERROR;
}

ECode ActionBarContainer::SetPrimaryBackground(
    /* [in] */ IDrawable* bg)
{
    VALIDATE_NOT_NULL(bg);
    // ==================before translated======================
    // if (mBackground != null) {
    //     mBackground.setCallback(null);
    //     unscheduleDrawable(mBackground);
    // }
    // mBackground = bg;
    // if (bg != null) {
    //     bg.setCallback(this);
    //     if (mActionBarView != null) {
    //         mBackground.setBounds(mActionBarView.getLeft(), mActionBarView.getTop(),
    //                 mActionBarView.getRight(), mActionBarView.getBottom());
    //     }
    // }
    // setWillNotDraw(mIsSplit ? mSplitBackground == null :
    //         mBackground == null && mStackedBackground == null);
    // invalidate();

    if (mBackground != NULL) {
        mBackground->SetCallback(NULL);
        UnscheduleDrawable(mBackground);
    }
    mBackground = bg;
    if (bg != NULL) {
        bg->SetCallback(IDrawableCallback::Probe(this));
        if (mActionBarView != NULL) {
            Int32 left = 0, top = 0, right = 0, bottom = 0;
            mActionBarView->GetLeft(&left);
            mActionBarView->GetTop(&top);
            mActionBarView->GetRight(&right);
            mActionBarView->GetBottom(&bottom);
            mBackground->SetBounds(left, top, right, bottom);
        }
    }

    SetWillNotDraw(mIsSplit ? mSplitBackground == NULL : mBackground == NULL && mStackedBackground == NULL);
    Invalidate();
    return NOERROR;
}

ECode ActionBarContainer::SetStackedBackground(
    /* [in] */ IDrawable* bg)
{
    VALIDATE_NOT_NULL(bg);
    // ==================before translated======================
    // if (mStackedBackground != null) {
    //     mStackedBackground.setCallback(null);
    //     unscheduleDrawable(mStackedBackground);
    // }
    // mStackedBackground = bg;
    // if (bg != null) {
    //     bg.setCallback(this);
    //     if ((mIsStacked && mStackedBackground != null)) {
    //         mStackedBackground.setBounds(mTabContainer.getLeft(), mTabContainer.getTop(),
    //                 mTabContainer.getRight(), mTabContainer.getBottom());
    //     }
    // }
    // setWillNotDraw(mIsSplit ? mSplitBackground == null :
    //         mBackground == null && mStackedBackground == null);
    // invalidate();

    if (mStackedBackground != NULL) {
        mStackedBackground->SetCallback(NULL);
        UnscheduleDrawable(mStackedBackground);
    }
    mStackedBackground = bg;
    if (bg != NULL) {
        bg->SetCallback(IDrawableCallback::Probe(this));
        if (mIsStacked && mStackedBackground != NULL) {
            Int32 left = 0, top = 0, right = 0, bottom = 0;
            mTabContainer->GetLeft(&left);
            mTabContainer->GetTop(&top);
            mTabContainer->GetRight(&right);
            mTabContainer->GetBottom(&bottom);
            mStackedBackground->SetBounds(left, top, right, bottom);
        }
    }
    SetWillNotDraw(mIsSplit ? mSplitBackground == NULL : mBackground == NULL && mStackedBackground == NULL);
    Invalidate();
    return NOERROR;
}

ECode ActionBarContainer::SetSplitBackground(
    /* [in] */ IDrawable* bg)
{
    VALIDATE_NOT_NULL(bg);
    // ==================before translated======================
    // if (mSplitBackground != null) {
    //     mSplitBackground.setCallback(null);
    //     unscheduleDrawable(mSplitBackground);
    // }
    // mSplitBackground = bg;
    // if (bg != null) {
    //     bg.setCallback(this);
    //     if (mIsSplit && mSplitBackground != null) {
    //         mSplitBackground.setBounds(0, 0, getMeasuredWidth(), getMeasuredHeight());
    //     }
    // }
    // setWillNotDraw(mIsSplit ? mSplitBackground == null :
    //         mBackground == null && mStackedBackground == null);
    // invalidate();

    if (mSplitBackground != NULL) {
        mSplitBackground->SetCallback(NULL);
        UnscheduleDrawable(mSplitBackground);
    }
    mSplitBackground = bg;
    if (bg != NULL) {
        bg->SetCallback(IDrawableCallback::Probe(this));
        if (mIsSplit && mSplitBackground != NULL) {
            Int32 measureWidth = 0, measureHeight = 0;
            GetMeasuredWidth(&measureWidth);
            GetMeasuredHeight(&measureHeight);
            mSplitBackground->SetBounds(0, 0, measureWidth, measureHeight);
        }
    }
    SetWillNotDraw(mIsSplit ? mSplitBackground == NULL : mBackground == NULL && mStackedBackground == NULL);
    Invalidate();
    return NOERROR;
}

ECode ActionBarContainer::SetVisibility(
    /* [in] */ Int32 visibility)
{
    // ==================before translated======================
    // super.setVisibility(visibility);
    // final boolean isVisible = visibility == VISIBLE;
    // if (mBackground != null) mBackground.setVisible(isVisible, false);
    // if (mStackedBackground != null) mStackedBackground.setVisible(isVisible, false);
    // if (mSplitBackground != null) mSplitBackground.setVisible(isVisible, false);

    FrameLayout::SetVisibility(visibility);
    Boolean isVisible = visibility == IView::VISIBLE;
    Boolean res = FALSE;
    if (mBackground != NULL)
        mBackground->SetVisible(isVisible, FALSE, &res);
    if (mStackedBackground != NULL)
        mStackedBackground->SetVisible(isVisible, FALSE, &res);
    if (mSplitBackground != NULL)
        mSplitBackground->SetVisible(isVisible, FALSE, &res);
    return NOERROR;
}

ECode ActionBarContainer::JumpDrawablesToCurrentState()
{
    // ==================before translated======================
    // super.jumpDrawablesToCurrentState();
    // if (mBackground != null) {
    //     mBackground.jumpToCurrentState();
    // }
    // if (mStackedBackground != null) {
    //     mStackedBackground.jumpToCurrentState();
    // }
    // if (mSplitBackground != null) {
    //     mSplitBackground.jumpToCurrentState();
    // }

    FrameLayout::JumpDrawablesToCurrentState();
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
    // ==================before translated======================
    // super.onResolveDrawables(layoutDirection);
    // if (mBackground != null) {
    //     mBackground.setLayoutDirection(layoutDirection);
    // }
    // if (mStackedBackground != null) {
    //     mStackedBackground.setLayoutDirection(layoutDirection);
    // }
    // if (mSplitBackground != null) {
    //     mSplitBackground.setLayoutDirection(layoutDirection);
    // }

    FrameLayout::OnResolveDrawables(layoutDirection);
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
    // ==================before translated======================
    // mIsTransitioning = isTransitioning;
    // setDescendantFocusability(isTransitioning ? FOCUS_BLOCK_DESCENDANTS
    //         : FOCUS_AFTER_DESCENDANTS);

    mIsTransitioning = isTransitioning;
    SetDescendantFocusability(isTransitioning ? FOCUS_BLOCK_DESCENDANTS : FOCUS_AFTER_DESCENDANTS);
    return NOERROR;
}

ECode ActionBarContainer::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIsTransitioning || super.onInterceptTouchEvent(ev);

    Boolean resTmp;
    *result = mIsTransitioning || FrameLayout::OnInterceptTouchEvent(ev, &resTmp);
    return NOERROR;
}

ECode ActionBarContainer::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // super.onTouchEvent(ev);
    //
    // // An action bar always eats touch events.
    // return true;

    Boolean resTmp;
    FrameLayout::OnTouchEvent(ev, &resTmp);
    *result = TRUE;
    return NOERROR;
}

ECode ActionBarContainer::OnHoverEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    // ==================before translated======================
    // super.onHoverEvent(ev);
    //
    // // An action bar always eats hover events.
    // return true;

    Boolean resTmp;
    FrameLayout::OnHoverEvent(ev, &resTmp);
    *result = TRUE;
    return NOERROR;
}

ECode ActionBarContainer::SetTabContainer(
    /* [in] */ IScrollingTabContainerView* tabView)
{
    VALIDATE_NOT_NULL(tabView);
    // ==================before translated======================
    // if (mTabContainer != null) {
    //     removeView(mTabContainer);
    // }
    // mTabContainer = tabView;
    // if (tabView != null) {
    //     addView(tabView);
    //     final ViewGroup.LayoutParams lp = tabView.getLayoutParams();
    //     lp.width = LayoutParams.MATCH_PARENT;
    //     lp.height = LayoutParams.WRAP_CONTENT;
    //     tabView.setAllowCollapse(false);
    // }

    if (mTabContainer != NULL) {
        RemoveView(mTabContainer);
    }
    mTabContainer = IView::Probe(tabView);
    if (tabView != NULL) {
        AddView(IView::Probe(tabView));
        AutoPtr<IViewGroupLayoutParams> lp;
        IView::Probe(tabView)->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        lp->SetWidth(IViewGroupLayoutParams::MATCH_PARENT);
        lp->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);
        tabView->SetAllowCollapse(FALSE);
    }
    return NOERROR;
}

ECode ActionBarContainer::GetTabContainer(
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mTabContainer;

    *result = mTabContainer;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarContainer::StartActionModeForChild(
    /* [in] */ IView* child,
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** result)
{
    VALIDATE_NOT_NULL(child);
    VALIDATE_NOT_NULL(callback);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // No starting an action mode for an action bar child! (Where would it go?)
    // return null;

    *result = NULL;
    return NOERROR;
}

void ActionBarContainer::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // ==================before translated======================
    // if (mActionBarView == null &&
    //         MeasureSpec.getMode(heightMeasureSpec) == MeasureSpec.AT_MOST && mHeight >= 0) {
    //     heightMeasureSpec = MeasureSpec.makeMeasureSpec(
    //             Math.min(mHeight, MeasureSpec.getSize(heightMeasureSpec)), MeasureSpec.AT_MOST);
    // }
    // super.onMeasure(widthMeasureSpec, heightMeasureSpec);
    //
    // if (mActionBarView == null) return;
    //
    // if (mTabContainer != null && mTabContainer.getVisibility() != GONE) {
    //     int nonTabMaxHeight = 0;
    //     final int childCount = getChildCount();
    //     for (int i = 0; i < childCount; i++) {
    //         final View child = getChildAt(i);
    //         if (child == mTabContainer) {
    //             continue;
    //         }
    //         nonTabMaxHeight = Math.max(nonTabMaxHeight, isCollapsed(child) ? 0 :
    //                 getMeasuredHeightWithMargins(child));
    //     }
    //     final int mode = MeasureSpec.getMode(heightMeasureSpec);
    //     final int maxHeight = mode == MeasureSpec.AT_MOST ?
    //             MeasureSpec.getSize(heightMeasureSpec) : Integer.MAX_VALUE;
    //     setMeasuredDimension(getMeasuredWidth(),
    //             Math.min(nonTabMaxHeight + getMeasuredHeightWithMargins(mTabContainer),
    //                     maxHeight));
    // }

    if (mActionBarView == NULL && View::MeasureSpec::GetMode(heightMeasureSpec) == View::MeasureSpec::AT_MOST && mHeight >= 0) {
        heightMeasureSpec = View::MeasureSpec::MakeMeasureSpec(Elastos::Core::Math::Min(mHeight,
            View::MeasureSpec::GetSize(heightMeasureSpec)), View::MeasureSpec::AT_MOST);
    }
    FrameLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    if (mActionBarView == NULL)
        return;

    Int32 visible = 0;
    if (mTabContainer != NULL && (mTabContainer->GetVisibility(&visible), visible) != IView::GONE) {
        Int32 nonTabMaxHeight = 0;
        Int32 childCount = 0;
        GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; ++i) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            if (TO_IINTERFACE(child) == TO_IINTERFACE(mTabContainer)) {
                continue;
            }
            nonTabMaxHeight = Elastos::Core::Math::Max(nonTabMaxHeight, IsCollapsed(child) ? 0 : GetMeasuredHeightWithMargins(child));
        }
        Int32 mode = View::MeasureSpec::GetMode(heightMeasureSpec);
        Int32 maxHeight = mode == View::MeasureSpec::AT_MOST ? View::MeasureSpec::GetSize(heightMeasureSpec) : Elastos::Core::Math::INT32_MAX_VALUE;
        Int32 measureWidth = 0;
        GetMeasuredWidth(&measureWidth);
        SetMeasuredDimension(measureWidth, Elastos::Core::Math::Min(nonTabMaxHeight + GetMeasuredHeightWithMargins(mTabContainer), maxHeight));
    }
}

ECode ActionBarContainer::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    // ==================before translated======================
    // super.onLayout(changed, l, t, r, b);
    //
    // final View tabContainer = mTabContainer;
    // final boolean hasTabs = tabContainer != null && tabContainer.getVisibility() != GONE;
    //
    // if (tabContainer != null && tabContainer.getVisibility() != GONE) {
    //     final int containerHeight = getMeasuredHeight();
    //     final LayoutParams lp = (LayoutParams) tabContainer.getLayoutParams();
    //     final int tabHeight = tabContainer.getMeasuredHeight();
    //     tabContainer.layout(l, containerHeight - tabHeight - lp.bottomMargin, r,
    //             containerHeight - lp.bottomMargin);
    // }
    //
    // boolean needsInvalidate = false;
    // if (mIsSplit) {
    //     if (mSplitBackground != null) {
    //         mSplitBackground.setBounds(0, 0, getMeasuredWidth(), getMeasuredHeight());
    //         needsInvalidate = true;
    //     }
    // } else {
    //     if (mBackground != null) {
    //         if (mActionBarView.getVisibility() == View.VISIBLE) {
    //             mBackground.setBounds(mActionBarView.getLeft(), mActionBarView.getTop(),
    //                     mActionBarView.getRight(), mActionBarView.getBottom());
    //         } else if (mActionContextView != null &&
    //                 mActionContextView.getVisibility() == View.VISIBLE) {
    //             mBackground.setBounds(mActionContextView.getLeft(), mActionContextView.getTop(),
    //                     mActionContextView.getRight(), mActionContextView.getBottom());
    //         } else {
    //             mBackground.setBounds(0, 0, 0, 0);
    //         }
    //         needsInvalidate = true;
    //     }
    //     mIsStacked = hasTabs;
    //     if (hasTabs && mStackedBackground != null) {
    //         mStackedBackground.setBounds(tabContainer.getLeft(), tabContainer.getTop(),
    //                 tabContainer.getRight(), tabContainer.getBottom());
    //         needsInvalidate = true;
    //     }
    // }
    //
    // if (needsInvalidate) {
    //     invalidate();
    // }

    FrameLayout::OnLayout(changed, l, t, r, b);
    AutoPtr<IView> tabContainer = mTabContainer;
    Int32 visibility = 0;
    tabContainer->GetVisibility(&visibility);
    Boolean hasTabs = tabContainer != NULL && visibility != IView::GONE;

    if (tabContainer != NULL && visibility != IView::GONE) {
        Int32 containerHeight = 0;
        GetMeasuredHeight(&containerHeight);

        AutoPtr<IViewGroupLayoutParams> lp;
        IView::Probe(tabContainer)->GetLayoutParams((IViewGroupLayoutParams**)&lp);

        Int32 tabHeight = 0;
        tabContainer->GetMeasuredHeight(&tabHeight);

        Int32 bottomMargin = 0;
        IViewGroupMarginLayoutParams::Probe(lp)->GetBottomMargin(&bottomMargin);
        tabContainer->Layout(l, containerHeight - tabHeight - bottomMargin, r, containerHeight - bottomMargin);
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
        if (mBackground != NULL) {
            mActionBarView->GetVisibility(&visibility);
            if (visibility == IView::VISIBLE) {
                Int32 left = 0, top = 0, right = 0, bottom = 0;
                mActionBarView->GetLeft(&left);
                mActionBarView->GetTop(&top);
                mActionBarView->GetRight(&right);
                mActionBarView->GetBottom(&bottom);
                mBackground->SetBounds(left, top, right, bottom);
            }
            else if (mActionContextView != NULL && (mActionContextView->GetVisibility(&visibility), visibility) == IView::VISIBLE) {
                Int32 left = 0, top = 0, right = 0, bottom = 0;
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
            Int32 left = 0, top = 0, right = 0, bottom = 0;
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
    // ==================before translated======================
    // return (who == mBackground && !mIsSplit) || (who == mStackedBackground && mIsStacked) ||
    //         (who == mSplitBackground && mIsSplit) || super.verifyDrawable(who);

    return (who == mBackground && !mIsSplit) || (who == mStackedBackground && mIsStacked) ||
        (who == mSplitBackground && mIsSplit) || FrameLayout::VerifyDrawable(who);
}

ECode ActionBarContainer::DrawableStateChanged()
{
    // ==================before translated======================
    // super.drawableStateChanged();
    // if (mBackground != null && mBackground.isStateful()) {
    //     mBackground.setState(getDrawableState());
    // }
    // if (mStackedBackground != null && mStackedBackground.isStateful()) {
    //     mStackedBackground.setState(getDrawableState());
    // }
    // if (mSplitBackground != null && mSplitBackground.isStateful()) {
    //     mSplitBackground.setState(getDrawableState());
    // }

    FrameLayout::DrawableStateChanged();
    Boolean res = FALSE;
    AutoPtr< ArrayOf<Int32> > drawableState;
    GetDrawableState((ArrayOf<Int32>**)&drawableState);
    Boolean b = FALSE;
    if (mBackground != NULL && (mBackground->IsStateful(&res), res)) {
        mBackground->SetState(drawableState, &b);
    }
    if (mStackedBackground != NULL && (mStackedBackground->IsStateful(&res), res)) {
        mStackedBackground->SetState(drawableState, &b);
    }
    if (mSplitBackground != NULL && (mSplitBackground->IsStateful(&res), res)) {
        mSplitBackground->SetState(drawableState, &b);
    }
    return NOERROR;
}

Boolean ActionBarContainer::IsCollapsed(
    /* [in] */ IView* view)
{
    // ==================before translated======================
    // return view == null || view.getVisibility() == GONE || view.getMeasuredHeight() == 0;

    Int32 visibility = 0;
    view->GetVisibility(&visibility);
    Int32 measureHeight = 0;
    view->GetMeasuredHeight(&measureHeight);
    return view == NULL || visibility == IView::GONE || measureHeight == 0;
}

Int32 ActionBarContainer::GetMeasuredHeightWithMargins(
    /* [in] */ IView* view)
{
    // ==================before translated======================
    // final LayoutParams lp = (LayoutParams) view.getLayoutParams();
    // return view.getMeasuredHeight() + lp.topMargin + lp.bottomMargin;

    AutoPtr<IViewGroupLayoutParams> lp;
    view->GetLayoutParams((IViewGroupLayoutParams**)&lp);

    Int32 measureHeight = 0, topMargin = 0, bottomMargin = 0;
    view->GetMeasuredHeight(&measureHeight);
    IViewGroupMarginLayoutParams::Probe(lp)->GetTopMargin(&topMargin);
    IViewGroupMarginLayoutParams::Probe(lp)->GetBottomMargin(&bottomMargin);
    return measureHeight + topMargin + bottomMargin;
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos


