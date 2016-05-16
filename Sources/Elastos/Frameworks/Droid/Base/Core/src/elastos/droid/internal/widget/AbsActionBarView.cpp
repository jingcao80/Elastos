
#include "elastos/droid/animation/CAnimatorSet.h"
#include "elastos/droid/animation/CObjectAnimator.h"
#include "elastos/droid/internal/widget/AbsActionBarView.h"
#include "elastos/droid/R.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/view/animation/CDecelerateInterpolator.h"
#include "elastos/droid/view/CContextThemeWrapper.h"
#include "elastos/droid/view/View.h"
#include "elastos/core/Math.h"

using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::CObjectAnimator;
using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IAnimatorSetBuilder;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Internal::Widget::EIID_IAbsActionBarView;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::View::CContextThemeWrapper;
using Elastos::Droid::View::View;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

//=====================================================================
//               AbsActionBarView::VisibilityAnimListener
//=====================================================================
CAR_INTERFACE_IMPL(AbsActionBarView::VisibilityAnimListener, Object, IAnimatorListener)

AbsActionBarView::VisibilityAnimListener::VisibilityAnimListener(
    /* [in] */ AbsActionBarView* owner)
    : mFinalVisibility(0)
    , mOwner(owner)
    , mCanceled(FALSE)
{
    assert(mOwner);
}

ECode AbsActionBarView::VisibilityAnimListener::WithFinalVisibility(
    /* [in] */ Int32 visibility,
    /* [out] */ VisibilityAnimListener** result)
{
    VALIDATE_NOT_NULL(result)

    mFinalVisibility = visibility;
    *result = this;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AbsActionBarView::VisibilityAnimListener::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    VALIDATE_NOT_NULL(animation);
    // ==================before translated======================
    // setVisibility(VISIBLE);
    // mVisibilityAnim = animation;
    // mCanceled = false;

    mOwner->SetVisibility(IView::VISIBLE);
    mOwner->mVisibilityAnim = animation;
    mCanceled = FALSE;
    return NOERROR;
}

ECode AbsActionBarView::VisibilityAnimListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    VALIDATE_NOT_NULL(animation)

    if (mCanceled)
        return NOERROR;

    mOwner->mVisibilityAnim = NULL;
    mOwner->SetVisibility(mFinalVisibility);
    if (mOwner->mSplitView != NULL && mOwner->mMenuView != NULL) {
        IView::Probe(mOwner->mMenuView)->SetVisibility(mFinalVisibility);
    }
    return NOERROR;
}

ECode AbsActionBarView::VisibilityAnimListener::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    VALIDATE_NOT_NULL(animation)
    mCanceled = TRUE;
    return NOERROR;
}

ECode AbsActionBarView::VisibilityAnimListener::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    VALIDATE_NOT_NULL(animation);
    return NOERROR;
}

//=====================================================================
//           AbsActionBarView::InnerShowOverflowMenuRunnable
//=====================================================================
CAR_INTERFACE_IMPL(AbsActionBarView::InnerShowOverflowMenuRunnable, Object, IRunnable)

AbsActionBarView::InnerShowOverflowMenuRunnable::InnerShowOverflowMenuRunnable(
    /* [in] */ AbsActionBarView* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode AbsActionBarView::InnerShowOverflowMenuRunnable::Run()
{
    Boolean resTmp;
    return mOwner->ShowOverflowMenu(&resTmp);
}

//=====================================================================
//                           AbsActionBarView
//=====================================================================
const AutoPtr<ITimeInterpolator> AbsActionBarView::sAlphaInterpolator = AbsActionBarView::InitAlphaInterpolator();
const Int32 AbsActionBarView::FADE_DURATION;

CAR_INTERFACE_IMPL(AbsActionBarView, ViewGroup, IAbsActionBarView)

AbsActionBarView::AbsActionBarView()
    : mSplitActionBar(FALSE)
    , mSplitWhenNarrow(FALSE)
    , mContentHeight(0)
{
    mVisAnimListener = new VisibilityAnimListener(this);
}

ECode AbsActionBarView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode AbsActionBarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode AbsActionBarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode AbsActionBarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ViewGroup::constructor(context, attrs, defStyleAttr, defStyleRes);
    AutoPtr<ITypedValue> tv;
    CTypedValue::New((ITypedValue**)&tv);

    AutoPtr<IResourcesTheme> theme;
    context->GetTheme((IResourcesTheme**)&theme);
    Boolean resTmp = FALSE;
    theme->ResolveAttribute(R::attr::actionBarPopupTheme, tv, TRUE, &resTmp);
    Int32 resourceId = 0;
    tv->GetResourceId(&resourceId);
    if (resTmp && resourceId != 0) {
        CContextThemeWrapper::New(context, resourceId, (IContext**)&mPopupContext);
    }
    else {
        mPopupContext = context;
    }
    return NOERROR;
}

ECode AbsActionBarView::SetSplitToolbar(
    /* [in] */ Boolean split)
{
    mSplitActionBar = split;
    return NOERROR;
}

ECode AbsActionBarView::SetSplitWhenNarrow(
    /* [in] */ Boolean splitWhenNarrow)
{
    // ==================before translated======================
    // mSplitWhenNarrow = splitWhenNarrow;

    mSplitWhenNarrow = splitWhenNarrow;
    return NOERROR;
}

ECode AbsActionBarView::SetContentHeight(
    /* [in] */ Int32 height)
{
    // ==================before translated======================
    // mContentHeight = height;
    // requestLayout();

    mContentHeight = height;
    RequestLayout();
    return NOERROR;
}

ECode AbsActionBarView::GetContentHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mContentHeight;

    *result = mContentHeight;
    return NOERROR;
}

ECode AbsActionBarView::SetSplitView(
    /* [in] */ IViewGroup* splitView)
{
    VALIDATE_NOT_NULL(splitView);
    // ==================before translated======================
    // mSplitView = splitView;

    mSplitView = splitView;
    return NOERROR;
}

ECode AbsActionBarView::GetAnimatedVisibility(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mVisibilityAnim != null) {
    //     return mVisAnimListener.mFinalVisibility;
    // }
    // return getVisibility();

    if (mVisibilityAnim != NULL) {
        *result = mVisAnimListener->mFinalVisibility;
        return NOERROR;
    }
    return GetVisibility(result);
}

ECode AbsActionBarView::AnimateToVisibility(
    /* [in] */ Int32 visibility)
{
    // ==================before translated======================
    // if (mVisibilityAnim != null) {
    //     mVisibilityAnim.cancel();
    // }
    // if (visibility == VISIBLE) {
    //     if (getVisibility() != VISIBLE) {
    //         setAlpha(0);
    //         if (mSplitView != null && mMenuView != null) {
    //             mMenuView.setAlpha(0);
    //         }
    //     }
    //     ObjectAnimator anim = ObjectAnimator.ofFloat(this, "alpha", 1);
    //     anim.setDuration(FADE_DURATION);
    //     anim.setInterpolator(sAlphaInterpolator);
    //     if (mSplitView != null && mMenuView != null) {
    //         AnimatorSet set = new AnimatorSet();
    //         ObjectAnimator splitAnim = ObjectAnimator.ofFloat(mMenuView, "alpha", 1);
    //         splitAnim.setDuration(FADE_DURATION);
    //         set.addListener(mVisAnimListener.withFinalVisibility(visibility));
    //         set.play(anim).with(splitAnim);
    //         set.start();
    //     } else {
    //         anim.addListener(mVisAnimListener.withFinalVisibility(visibility));
    //         anim.start();
    //     }
    // } else {
    //     ObjectAnimator anim = ObjectAnimator.ofFloat(this, "alpha", 0);
    //     anim.setDuration(FADE_DURATION);
    //     anim.setInterpolator(sAlphaInterpolator);
    //     if (mSplitView != null && mMenuView != null) {
    //         AnimatorSet set = new AnimatorSet();
    //         ObjectAnimator splitAnim = ObjectAnimator.ofFloat(mMenuView, "alpha", 0);
    //         splitAnim.setDuration(FADE_DURATION);
    //         set.addListener(mVisAnimListener.withFinalVisibility(visibility));
    //         set.play(anim).with(splitAnim);
    //         set.start();
    //     } else {
    //         anim.addListener(mVisAnimListener.withFinalVisibility(visibility));
    //         anim.start();
    //     }
    // }

    assert(0);
    if (mVisibilityAnim != NULL) {
        mVisibilityAnim->Cancel();
    }

    if (visibility == IView::VISIBLE) {
        Int32 visibility = 0;
        GetVisibility(&visibility);
        if (visibility != IView::VISIBLE) {
            SetAlpha(0);
            if (mSplitView != NULL && mMenuView != NULL) {
                IView::Probe(mMenuView)->SetAlpha(0);
            }
        }

        AutoPtr< ArrayOf<Float> > values = ArrayOf<Float>::Alloc(1);
        (*values)[0] = 1;
        AutoPtr<IObjectAnimator> anim = CObjectAnimator::OfFloat(TO_IINTERFACE(this), String("alpha"), values);
        IValueAnimator::Probe(anim)->SetDuration(FADE_DURATION);
        IAnimator::Probe(anim)->SetInterpolator(sAlphaInterpolator);
        if (mSplitView != NULL && mMenuView != NULL) {
            AutoPtr<IAnimatorSet> set;
            CAnimatorSet::New((IAnimatorSet**)&set);
            AutoPtr<IObjectAnimator> splitAnim = CObjectAnimator::OfFloat(mMenuView, String("alpha"), values);
            IValueAnimator::Probe(splitAnim)->SetDuration(FADE_DURATION);

            AutoPtr<VisibilityAnimListener> animListener;
            mVisAnimListener->WithFinalVisibility(visibility, (VisibilityAnimListener**)&animListener);
            IAnimator::Probe(set)->AddListener(IAnimatorListener::Probe(animListener));
            AutoPtr<IAnimatorSetBuilder> ab;
            set->Play(IAnimator::Probe(anim), (IAnimatorSetBuilder**)&ab);
            ab->With(IAnimator::Probe(splitAnim));
            IAnimator::Probe(set)->Start();
        }
        else {
            AutoPtr<VisibilityAnimListener> animListener;
            mVisAnimListener->WithFinalVisibility(visibility, (VisibilityAnimListener**)&animListener);
            IAnimator::Probe(anim)->AddListener(IAnimatorListener::Probe(animListener));
            IAnimator::Probe(anim)->Start();
        }
    }
    else {
       AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(1);
        (*values)[0] = 0;
        AutoPtr<IObjectAnimator> anim = CObjectAnimator::OfFloat(TO_IINTERFACE(this), String("alpha"), values);
        IValueAnimator::Probe(anim)->SetDuration(FADE_DURATION);
        IAnimator::Probe(anim)->SetInterpolator(sAlphaInterpolator);
        if (mSplitView != NULL && mMenuView != NULL) {
            AutoPtr<IAnimatorSet> set;
            CAnimatorSet::New((IAnimatorSet**)&set);
            AutoPtr<IObjectAnimator> splitAnim = CObjectAnimator::OfFloat(mMenuView, String("alpha"), values);
            IValueAnimator::Probe(splitAnim)->SetDuration(FADE_DURATION);

            AutoPtr<VisibilityAnimListener> animListener;
            mVisAnimListener->WithFinalVisibility(visibility, (VisibilityAnimListener**)&animListener);
            IAnimator::Probe(set)->AddListener(IAnimatorListener::Probe(animListener));

            AutoPtr<IAnimatorSetBuilder> ab;
            set->Play(IAnimator::Probe(anim), (IAnimatorSetBuilder**)&ab);
            ab->With(IAnimator::Probe(splitAnim));
            IAnimator::Probe(set)->Start();
        }
        else {
            AutoPtr<VisibilityAnimListener> animListener;
            mVisAnimListener->WithFinalVisibility(visibility, (VisibilityAnimListener**)&animListener);
            IAnimator::Probe(anim)->AddListener(IAnimatorListener::Probe(animListener));
            IAnimator::Probe(anim)->Start();
        }
    }
    return NOERROR;
}

ECode AbsActionBarView::SetVisibility(
    /* [in] */ Int32 visibility)
{
    // ==================before translated======================
    // if (visibility != getVisibility()) {
    //     if (mVisibilityAnim != null) {
    //         mVisibilityAnim.end();
    //     }
    //     super.setVisibility(visibility);
    // }

    Int32 visibilityTmp = 0;
    GetVisibility(&visibilityTmp);
    if (visibility != visibilityTmp) {
        if (mVisibilityAnim != NULL) {
            mVisibilityAnim->End();
        }
        ViewGroup::SetVisibility(visibility);
    }
    return NOERROR;
}

ECode AbsActionBarView::ShowOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mActionMenuPresenter != null) {
    //     return mActionMenuPresenter.showOverflowMenu();
    // }
    // return false;

    if (mActionMenuPresenter != NULL) {
        return mActionMenuPresenter->ShowOverflowMenu(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode AbsActionBarView::PostShowOverflowMenu()
{
    // ==================before translated======================
    // post(new Runnable() {
    //     public void run() {
    //         showOverflowMenu();
    //     }
    // });

    AutoPtr<IRunnable> runnable = new InnerShowOverflowMenuRunnable(this);
    Boolean resTmp;
    return Post(runnable, &resTmp);
}

ECode AbsActionBarView::HideOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mActionMenuPresenter != null) {
    //     return mActionMenuPresenter.hideOverflowMenu();
    // }
    // return false;

    if (mActionMenuPresenter != NULL) {
        return mActionMenuPresenter->HideOverflowMenu(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode AbsActionBarView::IsOverflowMenuShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mActionMenuPresenter != null) {
    //     return mActionMenuPresenter.isOverflowMenuShowing();
    // }
    // return false;

    if (mActionMenuPresenter != NULL) {
        return mActionMenuPresenter->IsOverflowMenuShowing(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode AbsActionBarView::IsOverflowMenuShowPending(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mActionMenuPresenter != null) {
    //     return mActionMenuPresenter.isOverflowMenuShowPending();
    // }
    // return false;

    if (mActionMenuPresenter != NULL) {
        return mActionMenuPresenter->IsOverflowMenuShowPending(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode AbsActionBarView::IsOverflowReserved(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mActionMenuPresenter != null && mActionMenuPresenter.isOverflowReserved();

    Boolean resTmp = FALSE;
    *result = mActionMenuPresenter != NULL && (mActionMenuPresenter->IsOverflowReserved(&resTmp), resTmp);
    return NOERROR;
}

ECode AbsActionBarView::CanShowOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return isOverflowReserved() && getVisibility() == VISIBLE;

    Boolean resTmp = FALSE;
    IsOverflowReserved(&resTmp);
    Int32 visibility = 0;
    GetVisibility(&visibility);
    *result = resTmp && visibility == VISIBLE;
    return NOERROR;
}

ECode AbsActionBarView::DismissPopupMenus()
{
    // ==================before translated======================
    // if (mActionMenuPresenter != null) {
    //     mActionMenuPresenter.dismissPopupMenus();
    // }

    if (mActionMenuPresenter != NULL) {
        Boolean resTmp = FALSE;
        mActionMenuPresenter->DismissPopupMenus(&resTmp);
    }
    return NOERROR;
}

void AbsActionBarView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    // ==================before translated======================
    // super.onConfigurationChanged(newConfig);
    //
    // // Action bar can change size on configuration changes.
    // // Reread the desired height from the theme-specified style.
    // TypedArray a = getContext().obtainStyledAttributes(null, R.styleable.ActionBar,
    //         com.android.internal.R.attr.actionBarStyle, 0);
    // setContentHeight(a.getLayoutDimension(R.styleable.ActionBar_height, 0));
    // a.recycle();
    // if (mSplitWhenNarrow) {
    //     setSplitToolbar(getContext().getResources().getBoolean(
    //             com.android.internal.R.bool.split_action_bar_is_narrow));
    // }
    // if (mActionMenuPresenter != null) {
    //     mActionMenuPresenter.onConfigurationChanged(newConfig);
    // }

    ViewGroup::OnConfigurationChanged(newConfig);

    // Action bar can change size on configuration changes.
    // Reread the desired height from the theme-specified style.
    AutoPtr<ArrayOf<Int32> > values = ArrayOf<Int32>::Alloc(
        const_cast<Int32*>(R::styleable::ActionBar),
        ArraySize(R::styleable::ActionBar));

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(NULL, values, R::attr::actionBarStyle, 0, (ITypedArray**)&a);

    Int32 h = 0;
    a->GetLayoutDimension(R::styleable::ActionBar_height, 0, &h);
    SetContentHeight(h);

    a->Recycle();
    if (mSplitWhenNarrow) {
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);

        Boolean narrow = FALSE;
        resources->GetBoolean(R::bool_::split_action_bar_is_narrow, &narrow);
        SetSplitToolbar(narrow);
    }
    if (mActionMenuPresenter != NULL) {
        mActionMenuPresenter->OnConfigurationChanged(newConfig);
    }
}

Int32 AbsActionBarView::MeasureChildView(
    /* [in] */ IView* child,
    /* [in] */ Int32 availableWidth,
    /* [in] */ Int32 childSpecHeight,
    /* [in] */ Int32 spacing)
{
    // ==================before translated======================
    // child.measure(MeasureSpec.makeMeasureSpec(availableWidth, MeasureSpec.AT_MOST),
    //         childSpecHeight);
    //
    // availableWidth -= child.getMeasuredWidth();
    // availableWidth -= spacing;
    //
    // return Math.max(0, availableWidth);

    child->Measure(View::MeasureSpec::MakeMeasureSpec(availableWidth, View::MeasureSpec::AT_MOST), childSpecHeight);
    Int32 w = 0;
    child->GetMeasuredWidth(&w);
    availableWidth -= w;
    availableWidth -= spacing;
    return Elastos::Core::Math::Max(0, availableWidth);
}

Int32 AbsActionBarView::Next(
    /* [in] */ Int32 x,
    /* [in] */ Int32 val,
    /* [in] */ Boolean isRtl)
{
    // ==================before translated======================
    // return isRtl ? x - val : x + val;

    return isRtl ? x - val : x + val;
}

Int32 AbsActionBarView::PositionChild(
    /* [in] */ IView* child,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 contentHeight,
    /* [in] */ Boolean reverse)
{
    // ==================before translated======================
    // int childWidth = child.getMeasuredWidth();
    // int childHeight = child.getMeasuredHeight();
    // int childTop = y + (contentHeight - childHeight) / 2;
    //
    // if (reverse) {
    //     child.layout(x - childWidth, childTop, x, childTop + childHeight);
    // } else {
    //     child.layout(x, childTop, x + childWidth, childTop + childHeight);
    // }
    //
    // return  (reverse ? -childWidth : childWidth);

    Int32 childWidth = 0;
    child->GetMeasuredWidth(&childWidth);
    Int32 childHeight = 0;
    child->GetMeasuredHeight(&childHeight);
    Int32 childTop = y + (contentHeight - childHeight) / 2;
    if (reverse) {
        child->Layout(x - childWidth, childTop, x, childTop + childHeight);
    }
    else {
        child->Layout(x, childTop, x + childWidth, childTop + childHeight);
    }
    return (reverse ? -childWidth : childWidth);
}

ECode AbsActionBarView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    return NOERROR;
}

AutoPtr<ITimeInterpolator> AbsActionBarView::InitAlphaInterpolator()
{
    AutoPtr<IDecelerateInterpolator> resultTmp;
    CDecelerateInterpolator::New((IDecelerateInterpolator**)&resultTmp);
    AutoPtr<ITimeInterpolator> result = ITimeInterpolator::Probe(resultTmp);
    return result;
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos


