
#include "elastos/droid/systemui/keyguard/CKeyguardWidgetCarousel.h"
#include "elastos/droid/systemui/keyguard/KeyguardWidgetFrame.h"
#include <elastos/core/Math.h>
#include "R.h"

using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IPropertyValuesHolderHelper;
using Elastos::Droid::Animation::CPropertyValuesHolderHelper;
using Elastos::Droid::View::Animation::EIID_IInterpolator;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Core::Math;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL_2(CKeyguardWidgetCarousel::MyInterpolator, Object, IInterpolator,
        ITimeInterpolator)

CKeyguardWidgetCarousel::MyInterpolator::MyInterpolator()
    : mFactor(2.5f)
{
    CDecelerateInterpolator::New(1.5f, (IInterpolator**)&mInternal);
}

ECode CKeyguardWidgetCarousel::MyInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output)

    return ITimeInterpolator::Probe(mInternal)->GetInterpolation(
            Elastos::Core::Math::Min(mFactor * input, 1.0f), output);
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
    input = mFactor * Elastos::Core::Math::Max(input, 0.0f);
    return ITimeInterpolator::Probe(mInternal)->GetInterpolation(input, output);
}

ECode CKeyguardWidgetCarousel::MyInterpolator2::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

     *res = FALSE;
    return NOERROR;
}

Float CKeyguardWidgetCarousel::MAX_SCROLL_PROGRESS = 1.3f;
Float CKeyguardWidgetCarousel::CAMERA_DISTANCE = 10000;

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
    context->GetResources((IResources**)&resources);
    Int32 tmp;
    resources->GetInteger(R::integer::kg_carousel_angle, &tmp);
    mAdjacentPagesAngle = tmp;
    return NOERROR;
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
        Float alpha = 1.0f - 1.0f *
                Elastos::Core::Math::Abs(scrollProgress / MAX_SCROLL_PROGRESS);
        *page = alpha;
        return NOERROR;
    }
    else {
        *page = 0.0f;
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
        return KeyguardWidgetPager::GetOutlineAlphaForPage(screenCenter,
                index, showSidePages, page);
    }
    else {
        *page = 0.0f;
        return NOERROR;
    }
}

void CKeyguardWidgetCarousel::UpdatePageAlphaValues(
    /* [in] */ Int32 screenCenter)
{
    if (mChildrenOutlineFadeAnimation != NULL) {
        IAnimator::Probe(mChildrenOutlineFadeAnimation)->Cancel();
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
            child->SetBackgroundAlpha(KeyguardWidgetFrame::OUTLINE_ALPHA_MULTIPLIER);
            child->SetContentAlpha(1.0f);
        }
        else {
            child->SetBackgroundAlpha(0.0f);
            child->SetContentAlpha(0.0f);
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
        GetScrollProgress(screenCenter, IView::Probe(v), i, &scrollProgress);
        Float boundedProgress;
        GetBoundedScrollProgress(screenCenter, IView::Probe(v), i, &boundedProgress);
        if (TO_IINTERFACE(v) == TO_IINTERFACE(mDragView) || v == NULL) continue;
        IView::Probe(v)->SetCameraDistance(CAMERA_DISTANCE);

        Boolean res;
        if (IsOverScrollChild(i, scrollProgress, &res), res) {
            IView::Probe(v)->SetRotationY(- OVERSCROLL_MAX_ROTATION * scrollProgress);
            v->SetOverScrollAmount(
                    Elastos::Core::Math::Abs(scrollProgress), scrollProgress < 0);
        }
        else {
            Int32 width;
            IView::Probe(v)->GetMeasuredWidth(&width);
            Float pivotX = (width / 2.0f) + boundedProgress * (width / 2.0f);
            Int32 height;
            IView::Probe(v)->GetMeasuredHeight(&height);
            Float pivotY = height / 2;
            Float rotationY = - mAdjacentPagesAngle * boundedProgress;
            IView::Probe(v)->SetPivotX(pivotX);
            IView::Probe(v)->SetPivotY(pivotY);
            IView::Probe(v)->SetRotationY(rotationY);
            v->SetOverScrollAmount(0.0f, FALSE);
        }
        Float alpha;
        IView::Probe(v)->GetAlpha(&alpha);
        // If the view has 0 alpha, we set it to be invisible so as to prevent
        // it from accepting touches
        Int32 visiable;
        if (alpha == 0) {
            IView::Probe(v)->SetVisibility(INVISIBLE);
        }
        else if ((IView::Probe(v)->GetVisibility(&visiable), visiable) != VISIBLE) {
            IView::Probe(v)->SetVisibility(VISIBLE);
        }
    }
    return NOERROR;
}

ECode CKeyguardWidgetCarousel::AnimatePagesToNeutral()
{
    if (mChildrenTransformsAnimator != NULL) {
        IAnimator::Probe(mChildrenTransformsAnimator)->Cancel();
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
            IView::Probe(child)->SetRotationY(0.0f);
        }
        AutoPtr<IPropertyValuesHolderHelper> helper;
        CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = 1.0f;
        helper->OfFloat(String("contentAlpha"), array, (IPropertyValuesHolder**)&alpha);
        AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(1);
        (*array2)[0] = KeyguardWidgetFrame::OUTLINE_ALPHA_MULTIPLIER;
        helper->OfFloat(String("backgroundAlpha"), array2, (IPropertyValuesHolder**)&outlineAlpha);
        AutoPtr<ArrayOf<Float> > array3 = ArrayOf<Float>::Alloc(1);
        (*array3)[0] = 0.0f;
        helper->OfFloat(String("rotationY"), array3, (IPropertyValuesHolder**)&rotationY);

        AutoPtr<IObjectAnimatorHelper> helper2;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper2);
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > array4 =
                ArrayOf<IPropertyValuesHolder*>::Alloc(3);
        array4->Set(0, alpha);
        array4->Set(1, outlineAlpha);
        array4->Set(2, rotationY);
        AutoPtr<IObjectAnimator> a;
        helper2->OfPropertyValuesHolder(child, array4, (IObjectAnimator**)&a);
        IView::Probe(child)->SetVisibility(VISIBLE);
        if (!inVisibleRange) {
            IAnimator::Probe(a)->SetInterpolator(
                    ITimeInterpolator::Probe(mSlowFadeInterpolator));
        }
        anims->Add(a);
    }

    Int32 duration = REORDERING_ZOOM_IN_OUT_DURATION;
    CAnimatorSet::New((IAnimatorSet**)&mChildrenTransformsAnimator);
    mChildrenTransformsAnimator->PlayTogether(ICollection::Probe(anims));

    IAnimator::Probe(mChildrenTransformsAnimator)->SetDuration(duration);
    return IAnimator::Probe(mChildrenTransformsAnimator)->Start();
}

void CKeyguardWidgetCarousel::GetTransformForPage(
    /* [in] */ Int32 screenCenter,
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Float>* transform)
{
    AutoPtr<IView> child;
    GetChildAt(index, (IView**)&child);
    Float boundedProgress;
    GetBoundedScrollProgress(screenCenter, child, index, &boundedProgress);
    Float rotationY = - mAdjacentPagesAngle * boundedProgress;
    Int32 width;
    child->GetMeasuredWidth(&width);
    Float pivotX = (width / 2.0f) + boundedProgress * (width / 2.0f);
    Int32 height;
    child->GetMeasuredHeight(&height);
    Float pivotY = height / 2;

    (*transform)[0] = pivotX;
    (*transform)[1] = pivotY;
    (*transform)[2] = rotationY;
}


ECode CKeyguardWidgetCarousel::AnimatePagesToCarousel()
{
    if (mChildrenTransformsAnimator != NULL) {
        IAnimator::Probe(mChildrenTransformsAnimator)->Cancel();
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
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = finalAlpha;
        helper->OfFloat(String("contentAlpha"), array, (IPropertyValuesHolder**)&alpha);
        AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(1);
        (*array2)[0] = finalOutlineAlpha;
        helper->OfFloat(String("backgroundAlpha"), array2, (IPropertyValuesHolder**)&outlineAlpha);
        AutoPtr<ArrayOf<Float> > array3 = ArrayOf<Float>::Alloc(1);
        (*array3)[0] = (*mTmpTransform)[0];
        helper->OfFloat(String("pivotX"), array3, (IPropertyValuesHolder**)&pivotX);
        AutoPtr<ArrayOf<Float> > array4 = ArrayOf<Float>::Alloc(1);
        (*array4)[0] = (*mTmpTransform)[1];
        helper->OfFloat(String("pivotY"), array4, (IPropertyValuesHolder**)&pivotY);
        AutoPtr<ArrayOf<Float> > array5 = ArrayOf<Float>::Alloc(1);
        (*array5)[0] = (*mTmpTransform)[2];
        helper->OfFloat(String("rotationY"), array5, (IPropertyValuesHolder**)&rotationY);

        AutoPtr<IObjectAnimatorHelper> helper2;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper2);
        AutoPtr<IObjectAnimator> a;
        if (inVisibleRange) {
            // for the central pages we animate into a rotated state
            AutoPtr<ArrayOf<IPropertyValuesHolder*> > array6 =
                    ArrayOf<IPropertyValuesHolder*>::Alloc(5);
            array6->Set(0, alpha);
            array6->Set(1, outlineAlpha);
            array6->Set(2, pivotX);
            array6->Set(3, pivotY);
            array6->Set(4, rotationY);
            helper2->OfPropertyValuesHolder(child, array6, (IObjectAnimator**)&a);
        }
        else {
            AutoPtr<ArrayOf<IPropertyValuesHolder*> > array7 =
                    ArrayOf<IPropertyValuesHolder*>::Alloc(2);
            array7->Set(0, alpha);
            array7->Set(1, outlineAlpha);
            helper2->OfPropertyValuesHolder(child, array7, (IObjectAnimator**)&a);
            IAnimator::Probe(a)->SetInterpolator(
                    ITimeInterpolator::Probe(mFastFadeInterpolator));
        }
        anims->Add(a);
    }

    Int32 duration = REORDERING_ZOOM_IN_OUT_DURATION;
    CAnimatorSet::New((IAnimatorSet**)&mChildrenTransformsAnimator);
    mChildrenTransformsAnimator->PlayTogether(ICollection::Probe(anims));

    IAnimator::Probe(mChildrenTransformsAnimator)->SetDuration(duration);
    return IAnimator::Probe(mChildrenTransformsAnimator)->Start();
}

ECode CKeyguardWidgetCarousel::ReorderStarting()
{
    mViewStateManager->FadeOutSecurity(REORDERING_ZOOM_IN_OUT_DURATION);
    AnimatePagesToNeutral();
    return NOERROR;
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
