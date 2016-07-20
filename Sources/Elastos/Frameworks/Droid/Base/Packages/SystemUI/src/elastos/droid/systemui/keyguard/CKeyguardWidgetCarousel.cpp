
#include "elastos/droid/systemui/keyguard/CKeyguardWidgetCarousel.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL_2(CKeyguardWidgetCarousel::MyInterpolator, Object, IInterpolator,
        ITimeInterpolator)

CKeyguardWidgetCarousel::MyInterpolator::MyInterpolator()
    : mFactor(2.5f)
{
    mInternal = new CDecelerateInterpolator::New(1.5f, (IInterpolator**)&mInternal);
}

ECode CKeyguardWidgetCarousel::MyInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output)

    return mInternal->GetInterpolation(Math::Min(mFactor * input, 1.0f), output);
}

ECode CKeyguardWidgetCarousel::MyInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    *res = FALSE;
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CKeyguardWidgetCarousel::MyInterpolator2, Object, IInterpolator,
        ITimeInterpolator)

CKeyguardWidgetCarousel::MyInterpolator2::MyInterpolator2()
    : mFactor(1.3f)
{
    CAccelerateInterpolator::New(1.5f, (IInterpolator**)&mInternal);
}

ECode CKeyguardWidgetCarousel::MyInterpolator2::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output)

    input -= (1 - 1 / mFactor);
    input = mFactor * Math::Max(input, 0f);
    return mInternal->GetInterpolation(input, output);
}

ECode CKeyguardWidgetCarousel::MyInterpolator2::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

     *res = FALSE;
    return NOERROR;
}

static Float CKeyguardWidgetCarousel::MAX_SCROLL_PROGRESS = 1.3f;
static Float CKeyguardWidgetCarousel::CAMERA_DISTANCE = 10000;

CAR_OBJECT_IMPL(CKeyguardWidgetCarousel)

CKeyguardWidgetCarousel::CKeyguardWidgetCarousel()
   : mAdjacentPagesAngle(0.0f)
{
    mTmpTransform = ArrayOf<Float>::Alloc(3);

    mFastFadeInterpolator = new MyInterpolator();

    mSlowFadeInterpolator = new MyInterpolator2();
}

ECode CKeyguardWidgetCarousel::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode CKeyguardWidgetCarousel::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CKeyguardWidgetCarousel::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    KeyguardWidgetPager::constructor(context, attrs, defStyle);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources)
    return resources->GetInteger(R::integer::kg_carousel_angle, &mAdjacentPagesAngle);
}

ECode CKeyguardWidgetCarousel::GetMaxScrollProgress(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    *result = MAX_SCROLL_PROGRESS;
    return NOERROR;
}

ECode CKeyguardWidgetCarousel::GetAlphaForPage(
    /* [in] */ Int32 screenCenter,
    /* [in] */ Int32 index,
    /* [in] */ Boolean showSidePages,
    /* [out] */ Float* page)
{
    VALIDATE_NOT_NULL(page)

    AutoPtr<IView> child;
    GetChildAt(index, (IView**)&child);
    if (child == NULL) {
        *page = 0.0f;
        return NOERROR;
    }

    Int32 _page;
    GetNextPage(&_page);
    Boolean inVisibleRange = index >= _page - 1 &&
            index <= _page + 1;
    Float scrollProgress;
    GetScrollProgress(screenCenter, child, index, &scrollProgress);

    Boolean result;
    if (IsOverScrollChild(index, scrollProgress, &result), result) {
        *page = 1.0f;
        return NOERROR;
    }
    else if ((showSidePages && inVisibleRange) || index == _page) {
        GetBoundedScrollProgress(screenCenter, child, index, &scrollProgress);
        Float alpha = 1.0f - 1.0f * Math::Abs(scrollProgress / MAX_SCROLL_PROGRESS);
        *page = alpha;
        return NOERROR;
    }
    else {
        *page = 0f;
        return NOERROR;
    }
}

ECode CKeyguardWidgetCarousel::GetOutlineAlphaForPage(
    /* [in] */ Int32 screenCenter,
    /* [in] */ Int32 index,
    /* [in] */ Boolean showSidePages,
    /* [out] */ Float* page)
{
    VALIDATE_NOT_NULL(page)

    Int32 _page;
    GetNextPage(&_page);
    Boolean inVisibleRange = index >= _page - 1 && index <= _page + 1;
    if (inVisibleRange) {
        return KeyguardWidgetPager::GetOutlineAlphaForPage(screenCenter, index, showSidePages, page);
    }
    else {
        *page = 0f;
        return NOERROR;
    }
}

void CKeyguardWidgetCarousel::UpdatePageAlphaValues(
    /* [in] */ Int32 screenCenter)
{
    if (mChildrenOutlineFadeAnimation != NULL) {
        mChildrenOutlineFadeAnimation->Cancel();
        mChildrenOutlineFadeAnimation = NULL;
    }
    Boolean res;
    Boolean showSidePages = mShowingInitialHints || (IsPageMoving(&res), res);
    if (IsReordering(FALSE, &res), !res) {
        Int32 count;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IKeyguardWidgetFrame> child;
            GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&child);
            if (child != NULL) {
                Float outlineAlpha;
                GetOutlineAlphaForPage(screenCenter, i, showSidePages, &outlineAlpha);
                Float contentAlpha;
                GetAlphaForPage(screenCenter, i,showSidePages, &contentAlpha);
                child->SetBackgroundAlpha(outlineAlpha);
                child->SetContentAlpha(contentAlpha);
            }
        }
    }
}

ECode CKeyguardWidgetCarousel::ShowInitialPageHints()
{
    mShowingInitialHints = TRUE;
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        Int32 page;
        GetNextPage(&page);
        Boolean inVisibleRange = i >= page - 1 && i <= page + 1;
        AutoPtr<IKeyguardWidgetFrame> child;
        GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&child);
        if (inVisibleRange) {
            child->SetBackgroundAlpha(IKeyguardWidgetFrame::OUTLINE_ALPHA_MULTIPLIER);
            child->SetContentAlpha(1f);
        }
        else {
            child->SetBackgroundAlpha(0f);
            child->SetContentAlpha(0f);
        }
    }
    return NOERROR;
}

ECode CKeyguardWidgetCarousel::ScreenScrolled(
    /* [in] */ Int32 screenCenter)
{
    mScreenCenter = screenCenter;
    UpdatePageAlphaValues(screenCenter);
    Boolean res;
    if (IsReordering(FALSE, &res), res) return NOERROR;

    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IKeyguardWidgetFrame> v;
        GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&v);
        Float scrollProgress;
        GetScrollProgress(screenCenter, v, i, &scrollProgress);
        Float boundedProgress;
        GetBoundedScrollProgress(screenCenter, v, i, &boundedProgress);
        if (TO_IINTERFACE(v) == TO_IINTERFACE(mDragView) || v == NULL) continue;
        v->SetCameraDistance(CAMERA_DISTANCE);

        Boolean res;
        if (IsOverScrollChild(i, scrollProgress, &res), res) {
            v->SetRotationY(- OVERSCROLL_MAX_ROTATION * scrollProgress);
            v->SetOverScrollAmount(Math::Abs(scrollProgress), scrollProgress < 0);
        }
        else {
            Int32 width;
            v->GetMeasuredWidth(&width);
            Float pivotX = (width / 2f) + boundedProgress * (width / 2f);
            Int32 height;
            v->GetMeasuredHeight(&height);
            Float pivotY = height / 2;
            Float rotationY = - mAdjacentPagesAngle * boundedProgress;
            v->SetPivotX(pivotX);
            v->SetPivotY(pivotY);
            v->SetRotationY(rotationY);
            v->SetOverScrollAmount(0.0f, FALSE);
        }
        Float alpha;
        v->GetAlpha(&alpha);
        // If the view has 0 alpha, we set it to be invisible so as to prevent
        // it from accepting touches
        Int32 visiable;
        if (alpha == 0) {
            v->SetVisibility(INVISIBLE);
        }
        else if ((v->GetVisibility(&visiable), visiable) != VISIBLE) {
            v->SetVisibility(VISIBLE);
        }
    }
    return NOERROR;
}

ECode CKeyguardWidgetCarousel::AnimatePagesToNeutral()
{
    if (mChildrenTransformsAnimator != NULL) {
        mChildrenTransformsAnimator->Cancel();
        mChildrenTransformsAnimator = NULL;
    }

    Int32 count;
    GetChildCount(&count);
    AutoPtr<IPropertyValuesHolder> alpha;
    AutoPtr<IPropertyValuesHolder> outlineAlpha;
    AutoPtr<IPropertyValuesHolder> rotationY;
    AutoPtr<IArrayList> anims;
    CArrayList::New((IArrayList**)&anims);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IKeyguardWidgetFrame> child;
        GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&child);
        Boolean inVisibleRange = (i >= mCurrentPage - 1 && i <= mCurrentPage + 1);
        if (!inVisibleRange) {
            child->SetRotationY(0.0f);
        }
        AutoPtr<IPropertyValuesHolderHelper> helper;
        CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
        helper->OfFloat(String("contentAlpha"), 1.0f, &alpha);
        helper->OfFloat(String("backgroundAlpha"),IKeyguardWidgetFrame::OUTLINE_ALPHA_MULTIPLIER, &outlineAlpha);
        helper->OfFloat(String("rotationY"), 0.0f, &rotationY);

        AutoPtr<IObjectAnimatorHelper> helper2;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper2);
        AutoPtr<IObjectAnimator> a;
        helper2->OfPropertyValuesHolder(child, alpha, outlineAlpha, rotationY, (IObjectAnimator**)&a);
        child->SetVisibility(VISIBLE);
        if (!inVisibleRange) {
            a->SetInterpolator(mSlowFadeInterpolator);
        }
        anims->Add(a);
    }

    Int32 duration = REORDERING_ZOOM_IN_OUT_DURATION;
    CAnimatorSet::New((IAnimatorSet**)&mChildrenTransformsAnimator);
    mChildrenTransformsAnimator->PlayTogether(anims);

    mChildrenTransformsAnimator->SetDuration(duration);
    return mChildrenTransformsAnimator->Start();
}

ECode CKeyguardWidgetCarousel::AnimatePagesToCarousel()
{
    if (mChildrenTransformsAnimator != NULL) {
        mChildrenTransformsAnimator->Cancel();
        mChildrenTransformsAnimator = NULL;
    }

    Int32 count;
    GetChildCount(&count);
    AutoPtr<IPropertyValuesHolder> alpha;
    AutoPtr<IPropertyValuesHolder> outlineAlpha;
    AutoPtr<IPropertyValuesHolder> rotationY;
    AutoPtr<IPropertyValuesHolder> pivotX;
    AutoPtr<IPropertyValuesHolder> pivotY;
    AutoPtr<IArrayList> anims;
    CArrayList::New((IArrayList**)&anims);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IKeyguardWidgetFrame> child;
        GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&child);
        Float finalAlpha;
        GetAlphaForPage(mScreenCenter, i, TRUE, &finalAlpha);
        Float finalOutlineAlpha;
        GetOutlineAlphaForPage(mScreenCenter, i, TRUE, &finalOutlineAlpha);
        GetTransformForPage(mScreenCenter, i, mTmpTransform);

        Boolean inVisibleRange = (i >= mCurrentPage - 1 && i <= mCurrentPage + 1);

        AutoPtr<IPropertyValuesHolderHelper> helper;
        CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
        helper->OfFloat(String("contentAlpha"), finalAlpha, &alpha);
        helper->OfFloat(String("backgroundAlpha"), finalOutlineAlpha, &outlineAlpha);
        helper->OfFloat(String("pivotX"), (*mTmpTransform)[0], &pivotX);
        helper->OfFloat(String("pivotY"), (*mTmpTransform)[1], &pivotY);
        helper->OfFloat(String("rotationY"), (*mTmpTransform)[2], &rotationY);

        AutoPtr<IObjectAnimatorHelper> helper2;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper2);
        AutoPtr<IObjectAnimator> a;
        if (inVisibleRange) {
            // for the central pages we animate into a rotated state
            helper2->OfPropertyValuesHolder(child, alpha, outlineAlpha,
                    pivotX, pivotY, rotationY, (IObjectAnimator**)&a);
        }
        else {
            helper2->OfPropertyValuesHolder(child, alpha, outlineAlpha, (IObjectAnimator**)&a);
            a->SetInterpolator(mFastFadeInterpolator);
        }
        anims->Add(a);
    }

    Int32 duration = REORDERING_ZOOM_IN_OUT_DURATION;
    CAnimatorSet::New((IAnimatorSet**)&mChildrenTransformsAnimator);
    mChildrenTransformsAnimator->PlayTogether(anims);

    mChildrenTransformsAnimator->SetDuration(duration);
    return mChildrenTransformsAnimator->Start();
}

ECode CKeyguardWidgetCarousel::ReorderStarting()
{
    mViewStateManager->FadeOutSecurity(REORDERING_ZOOM_IN_OUT_DURATION);
    AnimatePagesToNeutral();
}

ECode CKeyguardWidgetCarousel::ZoomIn(
    /* [in] */ IRunnable* onCompleteRunnable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AnimatePagesToCarousel();
    return KeyguardWidgetPager::ZoomIn(onCompleteRunnable, result);
}

ECode CKeyguardWidgetCarousel::OnEndReordering()
{
    KeyguardWidgetPager::OnEndReordering();
    return mViewStateManager->FadeInSecurity(REORDERING_ZOOM_IN_OUT_DURATION);
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
