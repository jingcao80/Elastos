
#include "elastos/droid/systemui/statusbar/stack/StackStateAnimator.h"
#include "elastos/droid/systemui/statusbar/stack/CNotificationStackScrollLayout.h"
#include "../../R.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/R.h>
#include <elastos/droid/view/animation/AnimationUtils.h>
#include <elastos/droid/view/View.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::CPropertyValuesHolderHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Animation::IPropertyValuesHolderHelper;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IViewOverlay;
using Elastos::Droid::View::IViewGroupOverlay;
using Elastos::Droid::View::View;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Core::CFloat;
using Elastos::Core::CInteger32;
using Elastos::Core::IFloat;
using Elastos::Core::IInteger32;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CStack;
using Elastos::Utility::IIterator;

#define AnimationEvent CNotificationStackScrollLayout::AnimationEvent
#define View Elastos::Droid::View::View

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

CAR_INTERFACE_IMPL(StackStateAnimator::AnimatorUpdateListener1, Object, IAnimatorUpdateListener)
StackStateAnimator::AnimatorUpdateListener1::AnimatorUpdateListener1(
    /* [in] */ IExpandableView* child)
    : mChild(child)
{}

ECode StackStateAnimator::AnimatorUpdateListener1::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    Int32 v = 0;
    IInteger32::Probe(obj)->GetValue(&v);
    mChild->SetActualHeight(v, FALSE /* notifyListeners */);
    return NOERROR;
}

StackStateAnimator::AnimatorListenerAdapter1::AnimatorListenerAdapter1(
    /* [in] */ IExpandableView* child)
    : mChild(child)
{}

ECode StackStateAnimator::AnimatorListenerAdapter1::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    IView::Probe(mChild)->SetTag(TAG_ANIMATOR_HEIGHT, NULL);
    IView::Probe(mChild)->SetTag(TAG_START_HEIGHT, NULL);
    IView::Probe(mChild)->SetTag(TAG_END_HEIGHT, NULL);
    return NOERROR;
}

CAR_INTERFACE_IMPL(StackStateAnimator::AnimatorUpdateListener2, Object, IAnimatorUpdateListener)
StackStateAnimator::AnimatorUpdateListener2::AnimatorUpdateListener2(
    /* [in] */ IExpandableView* child)
    : mChild(child)
{}

ECode StackStateAnimator::AnimatorUpdateListener2::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    Int32 v = 0;
    IInteger32::Probe(obj)->GetValue(&v);

    mChild->SetClipTopAmount(v);
    return NOERROR;
}

StackStateAnimator::AnimatorListenerAdapter2::AnimatorListenerAdapter2(
    /* [in] */ IExpandableView* child)
    : mChild(child)
{}

ECode StackStateAnimator::AnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    IView::Probe(mChild)->SetTag(TAG_ANIMATOR_TOP_INSET, NULL);
    IView::Probe(mChild)->SetTag(TAG_START_TOP_INSET, NULL);
    IView::Probe(mChild)->SetTag(TAG_END_TOP_INSET, NULL);
    return NOERROR;
}

StackStateAnimator::AnimatorListenerAdapter3::AnimatorListenerAdapter3(
    /* [in] */ IExpandableView* child,
    /* [in] */ Float newEndValue)
    : mWasCancelled(FALSE)
    , mChild(child)
    , mNewEndValue(newEndValue)
{}

ECode StackStateAnimator::AnimatorListenerAdapter3::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    IView::Probe(mChild)->SetLayerType(IView::LAYER_TYPE_NONE, NULL);
    if (mNewEndValue == 0 && !mWasCancelled) {
        IView::Probe(mChild)->SetVisibility(IView::INVISIBLE);
    }
    IView::Probe(mChild)->SetTag(TAG_ANIMATOR_ALPHA, NULL);
    IView::Probe(mChild)->SetTag(TAG_START_ALPHA, NULL);
    IView::Probe(mChild)->SetTag(TAG_END_ALPHA, NULL);
    return NOERROR;
}

ECode StackStateAnimator::AnimatorListenerAdapter3::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mWasCancelled = TRUE;
    return NOERROR;
}

ECode StackStateAnimator::AnimatorListenerAdapter3::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mWasCancelled = FALSE;
    return NOERROR;
}

StackStateAnimator::AnimatorListenerAdapter4::AnimatorListenerAdapter4()
{}

ECode StackStateAnimator::AnimatorListenerAdapter4::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

StackStateAnimator::AnimatorListenerAdapter5::AnimatorListenerAdapter5(
    /* [in] */ IExpandableView* child)
    : mChild(child)
{}

ECode StackStateAnimator::AnimatorListenerAdapter5::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    IView::Probe(mChild)->SetTag(TAG_ANIMATOR_TRANSLATION_Z, NULL);
    IView::Probe(mChild)->SetTag(TAG_START_TRANSLATION_Z, NULL);
    IView::Probe(mChild)->SetTag(TAG_END_TRANSLATION_Z, NULL);
    return NOERROR;
}

StackStateAnimator::AnimatorListenerAdapter6::AnimatorListenerAdapter6(
    /* [in] */ IExpandableView* child)
    : mChild(child)
{}

ECode StackStateAnimator::AnimatorListenerAdapter6::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    IView::Probe(mChild)->SetTag(TAG_ANIMATOR_TRANSLATION_Y, NULL);
    IView::Probe(mChild)->SetTag(TAG_START_TRANSLATION_Y, NULL);
    IView::Probe(mChild)->SetTag(TAG_END_TRANSLATION_Y, NULL);
    return NOERROR;
}

StackStateAnimator::AnimatorListenerAdapter7::AnimatorListenerAdapter7(
    /* [in] */ IExpandableView* child)
    : mChild(child)
{}

ECode StackStateAnimator::AnimatorListenerAdapter7::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    IView::Probe(mChild)->SetTag(TAG_ANIMATOR_SCALE, NULL);
    IView::Probe(mChild)->SetTag(TAG_START_SCALE, NULL);
    IView::Probe(mChild)->SetTag(TAG_END_SCALE, NULL);
    return NOERROR;
}

StackStateAnimator::AnimatorListenerAdapter8::AnimatorListenerAdapter8(
    /* [in] */ StackStateAnimator* host)
    : mHost(host)
    , mWasCancelled(FALSE)
{}

ECode StackStateAnimator::AnimatorListenerAdapter8::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mAnimatorSet->Remove(animation);
    Boolean tmp = FALSE;
    if ((mHost->mAnimatorSet->IsEmpty(&tmp), tmp) && !mWasCancelled) {
        mHost->OnAnimationFinished();
    }
    mHost->mAnimationListenerPool->Push(TO_IINTERFACE(this));
    return NOERROR;
}

ECode StackStateAnimator::AnimatorListenerAdapter8::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mWasCancelled = TRUE;
    return NOERROR;
}

ECode StackStateAnimator::AnimatorListenerAdapter8::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mWasCancelled = FALSE;
    return NOERROR;
}

StackStateAnimator::Runnable1::Runnable1(
    /* [in] */ StackStateAnimator* host,
    /* [in] */ IExpandableView* changingView)
    : mHost(host)
    , mChangingView(changingView)
{}

ECode StackStateAnimator::Runnable1::Run()
{
    // remove the temporary overlay
    AutoPtr<IViewOverlay> vo;
    IView::Probe(mHost->mHostLayout)->GetOverlay((IViewOverlay**)&vo);
    IViewGroupOverlay::Probe(vo)->Remove(IView::Probe(mChangingView));
    return NOERROR;
}

CAR_INTERFACE_IMPL(StackStateAnimator::AnimatorUpdateListener3, Object, IAnimatorUpdateListener)
StackStateAnimator::AnimatorUpdateListener3::AnimatorUpdateListener3(
    /* [in] */ StackStateAnimator* host,
    /* [in] */ Boolean onTop,
    /* [in] */ Boolean isRubberbanded)
    : mHost(host)
    , mOnTop(onTop)
    , mIsRubberbanded(isRubberbanded)
{}

ECode StackStateAnimator::AnimatorUpdateListener3::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    Float currentOverScroll = 0;
    IFloat::Probe(obj)->GetValue(&currentOverScroll);
    mHost->mHostLayout->SetOverScrollAmount(
            currentOverScroll, mOnTop, FALSE /* animate */, FALSE /* cancelAnimators */,
            mIsRubberbanded);
    return NOERROR;
}

StackStateAnimator::AnimatorListenerAdapter9::AnimatorListenerAdapter9(
    /* [in] */ StackStateAnimator* host,
    /* [in] */ Boolean onTop)
    : mHost(host)
    , mOnTop(onTop)
{}

ECode StackStateAnimator::AnimatorListenerAdapter9::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mOnTop) {
        mHost->mTopOverScrollAnimator = NULL;
    }
    else {
        mHost->mBottomOverScrollAnimator = NULL;
    }
    return NOERROR;
}


const Int32 StackStateAnimator::ANIMATION_DURATION_STANDARD = 360;
const Int32 StackStateAnimator::ANIMATION_DURATION_GO_TO_FULL_SHADE = 448;
const Int32 StackStateAnimator::ANIMATION_DURATION_APPEAR_DISAPPEAR = 464;
const Int32 StackStateAnimator::ANIMATION_DURATION_DIMMED_ACTIVATED = 220;
const Int32 StackStateAnimator::ANIMATION_DELAY_PER_ELEMENT_INTERRUPTING = 80;
const Int32 StackStateAnimator::ANIMATION_DELAY_PER_ELEMENT_MANUAL = 32;
const Int32 StackStateAnimator::ANIMATION_DELAY_PER_ELEMENT_GO_TO_FULL_SHADE = 48;
const Int32 StackStateAnimator::DELAY_EFFECT_MAX_INDEX_DIFFERENCE = 2;
const Int32 StackStateAnimator::TAG_ANIMATOR_TRANSLATION_Y = R::id::translation_y_animator_tag;
const Int32 StackStateAnimator::TAG_ANIMATOR_TRANSLATION_Z = R::id::translation_z_animator_tag;
const Int32 StackStateAnimator::TAG_ANIMATOR_SCALE = R::id::scale_animator_tag;
const Int32 StackStateAnimator::TAG_ANIMATOR_ALPHA = R::id::alpha_animator_tag;
const Int32 StackStateAnimator::TAG_ANIMATOR_HEIGHT = R::id::height_animator_tag;
const Int32 StackStateAnimator::TAG_ANIMATOR_TOP_INSET = R::id::top_inset_animator_tag;
const Int32 StackStateAnimator::TAG_END_TRANSLATION_Y = R::id::translation_y_animator_end_value_tag;
const Int32 StackStateAnimator::TAG_END_TRANSLATION_Z = R::id::translation_z_animator_end_value_tag;
const Int32 StackStateAnimator::TAG_END_SCALE = R::id::scale_animator_end_value_tag;
const Int32 StackStateAnimator::TAG_END_ALPHA = R::id::alpha_animator_end_value_tag;
const Int32 StackStateAnimator::TAG_END_HEIGHT = R::id::height_animator_end_value_tag;
const Int32 StackStateAnimator::TAG_END_TOP_INSET = R::id::top_inset_animator_end_value_tag;
const Int32 StackStateAnimator::TAG_START_TRANSLATION_Y = R::id::translation_y_animator_start_value_tag;
const Int32 StackStateAnimator::TAG_START_TRANSLATION_Z = R::id::translation_z_animator_start_value_tag;
const Int32 StackStateAnimator::TAG_START_SCALE = R::id::scale_animator_start_value_tag;
const Int32 StackStateAnimator::TAG_START_ALPHA = R::id::alpha_animator_start_value_tag;
const Int32 StackStateAnimator::TAG_START_HEIGHT = R::id::height_animator_start_value_tag;
const Int32 StackStateAnimator::TAG_START_TOP_INSET = R::id::top_inset_animator_start_value_tag;
CAR_INTERFACE_IMPL(StackStateAnimator, Object, IStackStateAnimator)
StackStateAnimator::StackStateAnimator(
    /* [in] */ INotificationStackScrollLayout* hostLayout)
    : mGoToFullShadeAppearingTranslation(0)
    , mCurrentLength(0)
    , mCurrentAdditionalDelay(0)
    , mCurrentLastNotAddedIndex(0)
{
    CArrayList::New((IArrayList**)&mNewEvents);
    CArrayList::New((IArrayList**)&mNewAddChildren);
    CHashSet::New((ISet**)&mAnimatorSet);
    CStack::New((IStack**)&mAnimationListenerPool);
    mAnimationFilter = new AnimationFilter();
    mHostLayout = hostLayout;

    AutoPtr<IContext> context;
    IView::Probe(hostLayout)->GetContext((IContext**)&context);
    AnimationUtils::LoadInterpolator(context, Elastos::Droid::R::interpolator::fast_out_slow_in,
            (IInterpolator**)&mFastOutSlowInInterpolator);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::go_to_full_shade_appearing_translation,
                &mGoToFullShadeAppearingTranslation);
}

ECode StackStateAnimator::IsRunning(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    mAnimatorSet->IsEmpty(&tmp);
    *result = !tmp;
    return NOERROR;
}

ECode StackStateAnimator::StartAnimationForEvents(
    /* [in] */ IArrayList/*<NotificationStackScrollLayout.AnimationEvent>*/* mAnimationEvents,
    /* [in] */ IStackScrollState* finalState,
    /* [in] */ Int64 additionalDelay)
{
    ProcessAnimationEvents(mAnimationEvents, (StackScrollState*)finalState);

    Int32 childCount = 0;
    IViewGroup::Probe(mHostLayout)->GetChildCount(&childCount);
    mAnimationFilter->ApplyCombination(mNewEvents);
    mCurrentAdditionalDelay = additionalDelay;
    mCurrentLength = AnimationEvent::CombineLength(mNewEvents);
    mCurrentLastNotAddedIndex = FindLastNotAddedIndex((StackScrollState*)finalState);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> view;
        IViewGroup::Probe(mHostLayout)->GetChildAt(i, (IView**)&view);
        AutoPtr<IExpandableView> child = IExpandableView::Probe(view);

        AutoPtr<IInterface> obj;
        finalState->GetViewStateForView(IView::Probe(child), (IInterface**)&obj);
        AutoPtr<ViewState> viewState = (ViewState*)IObject::Probe(obj);
        Int32 v = 0;
        if (viewState == NULL || (IView::Probe(child)->GetVisibility(&v), v) == IView::GONE) {
            continue;
        }

        IView::Probe(child)->SetClipBounds(NULL);
        StartAnimations(child, viewState, (StackScrollState*)finalState, i);
    }
    Boolean tmp = FALSE;
    if (IsRunning(&tmp), !tmp) {
        // no child has preformed any animation, lets finish
        OnAnimationFinished();
    }
    mNewEvents->Clear();
    mNewAddChildren->Clear();
    return NOERROR;
}

Int32 StackStateAnimator::FindLastNotAddedIndex(
    /* [in] */ StackScrollState* finalState)
{
    Int32 childCount = 0;
    IViewGroup::Probe(mHostLayout)->GetChildCount(&childCount);
    for (Int32 i = childCount - 1; i >= 0; i--) {
        AutoPtr<IView> view;
        IViewGroup::Probe(mHostLayout)->GetChildAt(i, (IView**)&view);
        AutoPtr<IExpandableView> child = IExpandableView::Probe(view);

        AutoPtr<IInterface> obj;
        finalState->GetViewStateForView(IView::Probe(child), (IInterface**)&obj);
        AutoPtr<ViewState> viewState = (ViewState*)IObject::Probe(obj);

        Int32 v = 0;
        if (viewState == NULL || (IView::Probe(child)->GetVisibility(&v), v) == IView::GONE) {
            continue;
        }

        Boolean contains = FALSE;
        if (mNewAddChildren->Contains(child, &contains), !contains) {
            return viewState->mNotGoneIndex;
        }
    }
    return -1;
}

void StackStateAnimator::StartAnimations(
    /* [in] */ IExpandableView* child,
    /* [in] */ ViewState* viewState,
    /* [in] */ StackScrollState* finalState,
    /* [in] */ Int32 i)
{
    Int32 childVisibility = 0;
    IView::Probe(child)->GetVisibility(&childVisibility);
    Boolean wasVisible = childVisibility == IView::VISIBLE;
    const Float alpha = viewState->mAlpha;
    if (!wasVisible && alpha != 0 && !viewState->mGone) {
        IView::Probe(child)->SetVisibility(IView::VISIBLE);
    }

    Float fy = 0, fz = 0;
    IView::Probe(child)->GetTranslationY(&fy);
    IView::Probe(child)->GetTranslationZ(&fz);
    Boolean yTranslationChanging = fy != viewState->mYTranslation;
    Boolean zTranslationChanging = fz != viewState->mZTranslation;

    Float f1 = 0, f2 = 0;
    IView::Probe(child)->GetScaleX(&f1);
    Boolean scaleChanging = f1 != viewState->mScale;
    Boolean alphaChanging = alpha != (IView::Probe(child)->GetAlpha(&f2), f2);

    Int32 iv = 0;
    Boolean heightChanging = viewState->mHeight != (child->GetActualHeight(&iv), iv);
    Boolean topInsetChanging = viewState->mClipTopAmount != (child->GetClipTopAmount(&iv), iv);
    Boolean wasAdded = FALSE;
    mNewAddChildren->Contains(child, &wasAdded);
    Boolean hasDelays = mAnimationFilter->mHasDelays;
    Boolean isDelayRelevant = yTranslationChanging || zTranslationChanging || scaleChanging ||
            alphaChanging || heightChanging || topInsetChanging;
    Boolean noAnimation = wasAdded;
    Int64 delay = 0;
    Int64 duration = mCurrentLength;
    if ((hasDelays && isDelayRelevant) || wasAdded) {
        delay = mCurrentAdditionalDelay + CalculateChildAnimationDelay(viewState, finalState);
    }

    if (wasAdded && mAnimationFilter->mHasGoToFullShadeEvent) {
        IView::Probe(child)->SetTranslationY(fy + mGoToFullShadeAppearingTranslation);
        yTranslationChanging = TRUE;
        Float longerDurationFactor = viewState->mNotGoneIndex - mCurrentLastNotAddedIndex;
        longerDurationFactor = (Float) Elastos::Core::Math::Pow(longerDurationFactor, 0.7f);
        duration = ANIMATION_DURATION_APPEAR_DISAPPEAR + 50 +
                (Int64) (100 * longerDurationFactor);
    }

    // start translationY animation
    if (yTranslationChanging) {
        if (noAnimation && !mAnimationFilter->mHasGoToFullShadeEvent) {
            IView::Probe(child)->SetTranslationY(viewState->mYTranslation);
        }
        else {
            StartYTranslationAnimation(child, viewState, duration, delay);
        }
    }

    // start translationZ animation
    if (zTranslationChanging) {
        if (noAnimation) {
            IView::Probe(child)->SetTranslationZ(viewState->mZTranslation);
        }
        else {
            StartZTranslationAnimation(child, viewState, duration, delay);
        }
    }

    // start scale animation
    if (scaleChanging) {
        if (noAnimation) {
            IView::Probe(child)->SetScaleX(viewState->mScale);
            IView::Probe(child)->SetScaleY(viewState->mScale);
        }
        else {
            StartScaleAnimation(child, viewState, duration);
        }
    }

    // start alpha animation
    Float fx = 0;
    if (alphaChanging && (IView::Probe(child)->GetTranslationX(&fx), fx) == 0) {
        if (noAnimation) {
            IView::Probe(child)->SetAlpha(viewState->mAlpha);
        }
        else {
            StartAlphaAnimation(child, viewState, duration, delay);
        }
    }

    // start height animation
    if (heightChanging && (child->GetActualHeight(&iv), iv) != 0) {
        if (noAnimation) {
            child->SetActualHeight(viewState->mHeight, FALSE);
        }
        else {
            StartHeightAnimation(child, viewState, duration, delay);
        }
    }

    // start top inset animation
    if (topInsetChanging) {
        if (noAnimation) {
            child->SetClipTopAmount(viewState->mClipTopAmount);
        }
        else {
            StartInsetAnimation(child, viewState, duration, delay);
        }
    }

    // start dimmed animation
    child->SetDimmed(viewState->mDimmed, mAnimationFilter->mAnimateDimmed && !wasAdded
            && !noAnimation);

    // start dark animation
    child->SetDark(viewState->mDark, mAnimationFilter->mAnimateDark && !noAnimation);

    // apply speed bump state
    child->SetBelowSpeedBump(viewState->mBelowSpeedBump);

    // start hiding sensitive animation
    child->SetHideSensitive(viewState->mHideSensitive, mAnimationFilter->mAnimateHideSensitive &&
            !wasAdded && !noAnimation, delay, duration);

    if (wasAdded) {
        child->PerformAddAnimation(delay, mCurrentLength);
    }
    if (ISpeedBumpView::Probe(child)) {
        finalState->PerformSpeedBumpAnimation(i, ISpeedBumpView::Probe(child),
                viewState->Probe(EIID_IInterface),
                delay + duration);
    }
}

Int64 StackStateAnimator::CalculateChildAnimationDelay(
    /* [in] */ ViewState* viewState,
    /* [in] */ StackScrollState* finalState)
{
    if (mAnimationFilter->mHasGoToFullShadeEvent) {
        return CalculateDelayGoToFullShade(viewState);
    }
    Int64 minDelay = 0;

    AutoPtr<IIterator> it;
    mNewEvents->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<AnimationEvent> event = (AnimationEvent*)IObject::Probe(obj);
        Int64 delayPerElement = ANIMATION_DELAY_PER_ELEMENT_INTERRUPTING;
        switch (event->mAnimationType) {
            case AnimationEvent::ANIMATION_TYPE_ADD: {
                Int32 ownIndex = viewState->mNotGoneIndex;
                obj = NULL;
                finalState->GetViewStateForView(event->mChangingView, (IInterface**)&obj);
                AutoPtr<ViewState> tmp = (ViewState*)IObject::Probe(obj);
                Int32 changingIndex = tmp->mNotGoneIndex;
                Int32 difference = Elastos::Core::Math::Abs(ownIndex - changingIndex);
                difference = Elastos::Core::Math::Max(0,
                    Elastos::Core::Math::Min(DELAY_EFFECT_MAX_INDEX_DIFFERENCE, difference - 1));
                Int64 delay = (DELAY_EFFECT_MAX_INDEX_DIFFERENCE - difference) * delayPerElement;
                minDelay = Elastos::Core::Math::Max(delay, minDelay);
                break;
            }
            case AnimationEvent::ANIMATION_TYPE_REMOVE_SWIPED_OUT:
                delayPerElement = ANIMATION_DELAY_PER_ELEMENT_MANUAL;
            case AnimationEvent::ANIMATION_TYPE_REMOVE: {
                Int32 ownIndex = viewState->mNotGoneIndex;
                Boolean noNextView = event->mViewAfterChangingView == NULL;
                AutoPtr<IView> viewAfterChangingView;
                if (noNextView) {
                    mHostLayout->GetLastChildNotGone((IView**)&viewAfterChangingView);
                }
                else {
                    viewAfterChangingView = event->mViewAfterChangingView;
                }

                obj = NULL;
                finalState->GetViewStateForView(viewAfterChangingView, (IInterface**)&obj);
                AutoPtr<ViewState> tmp = (ViewState*)IObject::Probe(obj);
                Int32 nextIndex = tmp->mNotGoneIndex;
                if (ownIndex >= nextIndex) {
                    // we only have the view afterwards
                    ownIndex++;
                }
                Int32 difference = Elastos::Core::Math::Abs(ownIndex - nextIndex);
                difference = Elastos::Core::Math::Max(0,
                    Elastos::Core::Math::Min(DELAY_EFFECT_MAX_INDEX_DIFFERENCE, difference - 1));
                Int64 delay = difference * delayPerElement;
                minDelay = Elastos::Core::Math::Max(delay, minDelay);
                break;
            }
            default:
                break;
        }
    }

    return minDelay;
}

Int64 StackStateAnimator::CalculateDelayGoToFullShade(
    /* [in] */ ViewState* viewState)
{
    Float index = viewState->mNotGoneIndex;
    index = (Float) Elastos::Core::Math::Pow(index, 0.7f);
    return (Int64) (index * ANIMATION_DELAY_PER_ELEMENT_GO_TO_FULL_SHADE);
}

void StackStateAnimator::StartHeightAnimation(
    /* [in] */ IExpandableView* child,
    /* [in] */ ViewState* viewState,
    /* [in] */ Int64 duration,
    /* [in] */ Int64 delay)
{
    AutoPtr<IInteger32> _previousStartValue = IInteger32::Probe(GetChildTag(IView::Probe(child), TAG_START_HEIGHT));
    AutoPtr<IInteger32> _previousEndValue = IInteger32::Probe(GetChildTag(IView::Probe(child), TAG_END_HEIGHT));
    Int32 newEndValue = viewState->mHeight;
    Int32 previousStartValue = 0, previousEndValue = 0;

    if (_previousStartValue != NULL) {
        _previousStartValue->GetValue(&previousStartValue);
    }
    if (_previousEndValue != NULL
        && (_previousEndValue->GetValue(&previousEndValue), previousEndValue) == newEndValue) {
        return;
    }
    AutoPtr<IValueAnimator> previousAnimator = IValueAnimator::Probe(GetChildTag(IView::Probe(child), TAG_ANIMATOR_HEIGHT));
    if (!mAnimationFilter->mAnimateHeight) {
        // just a local update was performed
        if (previousAnimator != NULL) {
            // we need to increase all animation keyframes of the previous animator by the
            // relative change to the end value
            AutoPtr<ArrayOf<IPropertyValuesHolder*> > values;
            previousAnimator->GetValues((ArrayOf<IPropertyValuesHolder*>**)&values);
            _previousEndValue->GetValue(&previousEndValue);
            Int32 relativeDiff = newEndValue - previousEndValue;
            Int32 newStartValue = previousStartValue + relativeDiff;
            AutoPtr<ArrayOf<Int32> > ivs = ArrayOf<Int32>::Alloc(2);
            (*ivs)[0] = newStartValue;
            (*ivs)[1] = newEndValue;
            (*values)[0]->SetInt32Values(ivs);

            AutoPtr<IInteger32> iobj;
            CInteger32::New(newStartValue, (IInteger32**)&iobj);
            IView::Probe(child)->SetTag(TAG_START_HEIGHT, iobj);

            iobj = NULL;
            CInteger32::New(newEndValue, (IInteger32**)&iobj);
            IView::Probe(child)->SetTag(TAG_END_HEIGHT, iobj);
            Int64 pt = 0;
            previousAnimator->GetCurrentPlayTime(&pt);
            previousAnimator->SetCurrentPlayTime(pt);
            return;
        }
        else {
            // no new animation needed, let's just apply the value
            child->SetActualHeight(newEndValue, FALSE);
            return;
        }
    }

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    Int32 ah = 0;
    child->GetActualHeight(&ah);
    AutoPtr<ArrayOf<Int32> > ivs = ArrayOf<Int32>::Alloc(2);
    (*ivs)[0] = ah;
    (*ivs)[1] = newEndValue;
    AutoPtr<IValueAnimator> animator;
    helper->OfInt32(ivs, (IValueAnimator**)&animator);
    AutoPtr<AnimatorUpdateListener1> listener = new AnimatorUpdateListener1(child);
    animator->AddUpdateListener(listener);
    IAnimator::Probe(animator)->SetInterpolator(ITimeInterpolator::Probe(mFastOutSlowInInterpolator));
    Int64 newDuration = CancelAnimatorAndGetNewDuration(duration, previousAnimator);
    IAnimator::Probe(animator)->SetDuration(newDuration);
    Boolean tmp = FALSE;
    if (delay > 0 && (previousAnimator == NULL || (IAnimator::Probe(previousAnimator)->IsRunning(&tmp), !tmp))) {
        IAnimator::Probe(animator)->SetStartDelay(delay);
    }
    IAnimator::Probe(animator)->AddListener(GetGlobalAnimationFinishedListener());
    // remove the tag when the animation is finished
    AutoPtr<AnimatorListenerAdapter1> adapter = new AnimatorListenerAdapter1(child);
    IAnimator::Probe(animator)->AddListener(adapter);
    StartAnimator(animator);
    IView::Probe(child)->SetTag(TAG_ANIMATOR_HEIGHT, animator);

    AutoPtr<IInteger32> iobj;
    CInteger32::New(ah, (IInteger32**)&iobj);
    IView::Probe(child)->SetTag(TAG_START_HEIGHT, iobj);

    iobj = NULL;
    CInteger32::New(newEndValue, (IInteger32**)&iobj);
    IView::Probe(child)->SetTag(TAG_END_HEIGHT, iobj);
}

void StackStateAnimator::StartInsetAnimation(
    /* [in] */ IExpandableView* child,
    /* [in] */ ViewState* viewState,
    /* [in] */ Int64 duration,
    /* [in] */ Int64 delay)
{
    AutoPtr<IInteger32> _previousStartValue = IInteger32::Probe(GetChildTag(IView::Probe(child), TAG_START_TOP_INSET));
    AutoPtr<IInteger32> _previousEndValue = IInteger32::Probe(GetChildTag(IView::Probe(child), TAG_END_TOP_INSET));
    Int32 previousStartValue = 0, previousEndValue = 0;

    if (_previousStartValue != NULL) {
        _previousStartValue->GetValue(&previousStartValue);
    }

    Int32 newEndValue = viewState->mClipTopAmount;
    if (_previousEndValue != NULL
        && (_previousEndValue->GetValue(&previousEndValue), previousEndValue) == newEndValue) {
        return;
    }
    AutoPtr<IValueAnimator> previousAnimator = IValueAnimator::Probe(GetChildTag(IView::Probe(child), TAG_ANIMATOR_TOP_INSET));
    if (!mAnimationFilter->mAnimateTopInset) {
        // just a local update was performed
        if (previousAnimator != NULL) {
            // we need to increase all animation keyframes of the previous animator by the
            // relative change to the end value
            AutoPtr<ArrayOf<IPropertyValuesHolder*> > values;
            previousAnimator->GetValues((ArrayOf<IPropertyValuesHolder*>**)&values);
            _previousEndValue->GetValue(&previousEndValue);
            Int32 relativeDiff = newEndValue - previousEndValue;
            Int32 newStartValue = previousStartValue + relativeDiff;

            AutoPtr<ArrayOf<Int32> > ivs = ArrayOf<Int32>::Alloc(2);
            (*ivs)[0] = newStartValue;
            (*ivs)[1] = newEndValue;
            (*values)[0]->SetInt32Values(ivs);

            AutoPtr<IInteger32> iobj;
            CInteger32::New(newStartValue, (IInteger32**)&iobj);
            IView::Probe(child)->SetTag(TAG_START_TOP_INSET, iobj);

            iobj = NULL;
            CInteger32::New(newEndValue, (IInteger32**)&iobj);
            IView::Probe(child)->SetTag(TAG_END_TOP_INSET, iobj);

            Int64 pt = 0;
            previousAnimator->GetCurrentPlayTime(&pt);
            previousAnimator->SetCurrentPlayTime(pt);
            return;
        }
        else {
            // no new animation needed, let's just apply the value
            child->SetClipTopAmount(newEndValue);
            return;
        }
    }

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<IValueAnimator> animator;
    Int32 it = 0;
    child->GetClipTopAmount(&it);
    AutoPtr<ArrayOf<Int32> > ivs = ArrayOf<Int32>::Alloc(2);
    (*ivs)[0] = it;
    (*ivs)[1] = newEndValue;
    helper->OfInt32(ivs, (IValueAnimator**)&animator);
    AutoPtr<AnimatorUpdateListener2> listener = new AnimatorUpdateListener2(child);
    animator->AddUpdateListener(listener);
    IAnimator::Probe(animator)->SetInterpolator(ITimeInterpolator::Probe(mFastOutSlowInInterpolator));
    Int64 newDuration = CancelAnimatorAndGetNewDuration(duration, previousAnimator);
    IAnimator::Probe(animator)->SetDuration(newDuration);
    Boolean tmp = FALSE;
    if (delay > 0 && (previousAnimator == NULL || (IAnimator::Probe(previousAnimator)->IsRunning(&tmp), !tmp))) {
        IAnimator::Probe(animator)->SetStartDelay(delay);
    }
    IAnimator::Probe(animator)->AddListener(GetGlobalAnimationFinishedListener());
    // remove the tag when the animation is finished
    AutoPtr<AnimatorListenerAdapter2> adapter = new AnimatorListenerAdapter2(child);
    IAnimator::Probe(animator)->AddListener(adapter);
    StartAnimator(animator);
    IView::Probe(child)->SetTag(TAG_ANIMATOR_TOP_INSET, animator);

    AutoPtr<IInteger32> iobj;
    CInteger32::New(it, (IInteger32**)&iobj);
    IView::Probe(child)->SetTag(TAG_START_TOP_INSET, iobj);

    iobj = NULL;
    CInteger32::New(newEndValue, (IInteger32**)&iobj);
    IView::Probe(child)->SetTag(TAG_END_TOP_INSET, iobj);
}

void StackStateAnimator::StartAlphaAnimation(
    /* [in] */ IExpandableView* child,
    /* [in] */ ViewState* viewState,
    /* [in] */ Int64 duration,
    /* [in] */ Int64 delay)
{
    AutoPtr<IFloat> _previousStartValue = IFloat::Probe(GetChildTag(IView::Probe(child), TAG_START_ALPHA));
    AutoPtr<IFloat> _previousEndValue = IFloat::Probe(GetChildTag(IView::Probe(child), TAG_END_ALPHA));
    Float previousStartValue = 0, previousEndValue = 0;

    if (_previousStartValue != NULL) {
        _previousStartValue->GetValue(&previousStartValue);
    }
    const Float newEndValue = viewState->mAlpha;
    if (_previousEndValue != NULL
        && (_previousEndValue->GetValue(&previousEndValue), previousEndValue) == newEndValue) {
        return;
    }
    AutoPtr<IObjectAnimator> previousAnimator
            = IObjectAnimator::Probe(GetChildTag(IView::Probe(child), TAG_ANIMATOR_ALPHA));
    if (!mAnimationFilter->mAnimateAlpha) {
        // just a local update was performed
        if (previousAnimator != NULL) {
            // we need to increase all animation keyframes of the previous animator by the
            // relative change to the end value
            AutoPtr<ArrayOf<IPropertyValuesHolder*> > values;
            IValueAnimator::Probe(previousAnimator)->GetValues((ArrayOf<IPropertyValuesHolder*>**)&values);
            _previousEndValue->GetValue(&previousEndValue);
            Float relativeDiff = newEndValue - previousEndValue;
            Float newStartValue = previousStartValue + relativeDiff;

            AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
            (*fvs)[0] = newStartValue;
            (*fvs)[1] = newEndValue;
            (*values)[0]->SetFloatValues(fvs);

            AutoPtr<IFloat> fObj;
            CFloat::New(newStartValue, (IFloat**)&fObj);
            IView::Probe(child)->SetTag(TAG_START_ALPHA, fObj);

            fObj = NULL;
            CFloat::New(newEndValue, (IFloat**)&fObj);
            IView::Probe(child)->SetTag(TAG_END_ALPHA, fObj);

            Int64 pt = 0;
            IValueAnimator::Probe(previousAnimator)->GetCurrentPlayTime(&pt);
            IValueAnimator::Probe(previousAnimator)->SetCurrentPlayTime(pt);
            return;
        }
        else {
            // no new animation needed, let's just apply the value
            IView::Probe(child)->SetAlpha(newEndValue);
            if (newEndValue == 0) {
                IView::Probe(child)->SetVisibility(IView::INVISIBLE);
            }
        }
    }

    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    Float alpha = 0;
    IView::Probe(child)->GetAlpha(&alpha);
    AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
    (*fvs)[0] = alpha;
    (*fvs)[1] = newEndValue;
    AutoPtr<IObjectAnimator> animator;
    helper->OfFloat(child, View::ALPHA, fvs, (IObjectAnimator**)&animator);
    IAnimator::Probe(animator)->SetInterpolator(ITimeInterpolator::Probe(mFastOutSlowInInterpolator));
    // Handle layer type
    IView::Probe(child)->SetLayerType(IView::LAYER_TYPE_HARDWARE, NULL);
    AutoPtr<AnimatorListenerAdapter3> listener = new AnimatorListenerAdapter3(child, newEndValue);
    IAnimator::Probe(animator)->AddListener(listener);
    Int64 newDuration = CancelAnimatorAndGetNewDuration(duration, IValueAnimator::Probe(previousAnimator));
    IAnimator::Probe(animator)->SetDuration(newDuration);
    Boolean tmp = FALSE;
    if (delay > 0 && (previousAnimator == NULL || (IAnimator::Probe(previousAnimator)->IsRunning(&tmp), !tmp))) {
        IAnimator::Probe(animator)->SetStartDelay(delay);
    }
    IAnimator::Probe(animator)->AddListener(GetGlobalAnimationFinishedListener());
    // remove the tag when the animation is finished
    AutoPtr<AnimatorListenerAdapter4> adapter = new AnimatorListenerAdapter4();
    IAnimator::Probe(animator)->AddListener(adapter);
    StartAnimator(IValueAnimator::Probe(animator));
    IView::Probe(child)->SetTag(TAG_ANIMATOR_ALPHA, animator);

    AutoPtr<IFloat> fObj;
    CFloat::New(alpha, (IFloat**)&fObj);
    IView::Probe(child)->SetTag(TAG_START_ALPHA, fObj);

    fObj = NULL;
    CFloat::New(newEndValue, (IFloat**)&fObj);
    IView::Probe(child)->SetTag(TAG_END_ALPHA, fObj);
}

void StackStateAnimator::StartZTranslationAnimation(
    /* [in] */ IExpandableView* child,
    /* [in] */ ViewState* viewState,
    /* [in] */ Int64 duration,
    /* [in] */ Int64 delay)
{
    AutoPtr<IFloat> _previousStartValue = IFloat::Probe(GetChildTag(IView::Probe(child), TAG_START_TRANSLATION_Z));
    AutoPtr<IFloat> _previousEndValue = IFloat::Probe(GetChildTag(IView::Probe(child), TAG_END_TRANSLATION_Z));
    Float previousStartValue = 0, previousEndValue = 0;

    if (_previousStartValue != NULL) {
        _previousStartValue->GetValue(&previousStartValue);
    }
    Float newEndValue = viewState->mZTranslation;
    if (_previousEndValue != NULL
        && (_previousEndValue->GetValue(&previousEndValue), previousEndValue) == newEndValue) {
        return;
    }
    AutoPtr<IObjectAnimator> previousAnimator
            = IObjectAnimator::Probe(GetChildTag(IView::Probe(child), TAG_ANIMATOR_TRANSLATION_Z));
    if (!mAnimationFilter->mAnimateZ) {
        // just a local update was performed
        if (previousAnimator != NULL) {
            // we need to increase all animation keyframes of the previous animator by the
            // relative change to the end value
            AutoPtr<ArrayOf<IPropertyValuesHolder*> > values;
            IValueAnimator::Probe(previousAnimator)->GetValues((ArrayOf<IPropertyValuesHolder*>**)&values);
            _previousEndValue->GetValue(&previousEndValue);
            Float relativeDiff = newEndValue - previousEndValue;
            Float newStartValue = previousStartValue + relativeDiff;

            AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
            (*fvs)[0] = newStartValue;
            (*fvs)[1] = newEndValue;
            (*values)[0]->SetFloatValues(fvs);

            AutoPtr<IFloat> fObj;
            CFloat::New(newStartValue, (IFloat**)&fObj);
            IView::Probe(child)->SetTag(TAG_START_TRANSLATION_Z, fObj);

            fObj = NULL;
            CFloat::New(newEndValue, (IFloat**)&fObj);
            IView::Probe(child)->SetTag(TAG_END_TRANSLATION_Z, fObj);

            Int64 pt = 0;
            IValueAnimator::Probe(previousAnimator)->GetCurrentPlayTime(&pt);
            IValueAnimator::Probe(previousAnimator)->SetCurrentPlayTime(pt);
            return;
        }
        else {
            // no new animation needed, let's just apply the value
            IView::Probe(child)->SetTranslationZ(newEndValue);
        }
    }

    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    Float fz = 0;
    IView::Probe(child)->GetTranslationZ(&fz);
    AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
    (*fvs)[0] = fz;
    (*fvs)[1] = newEndValue;
    AutoPtr<IObjectAnimator> animator;
    helper->OfFloat(child, View::TRANSLATION_Z, fvs, (IObjectAnimator**)&animator);
    IAnimator::Probe(animator)->SetInterpolator(ITimeInterpolator::Probe(mFastOutSlowInInterpolator));
    Int64 newDuration = CancelAnimatorAndGetNewDuration(duration, IValueAnimator::Probe(previousAnimator));
    IAnimator::Probe(animator)->SetDuration(newDuration);
    Boolean tmp = FALSE;
    if (delay > 0 && (previousAnimator == NULL || (IAnimator::Probe(previousAnimator)->IsRunning(&tmp), !tmp))) {
        IAnimator::Probe(animator)->SetStartDelay(delay);
    }
    IAnimator::Probe(animator)->AddListener(GetGlobalAnimationFinishedListener());
    // remove the tag when the animation is finished
    AutoPtr<AnimatorListenerAdapter5> listener = new AnimatorListenerAdapter5(child);
    IAnimator::Probe(animator)->AddListener(listener);
    StartAnimator(IValueAnimator::Probe(animator));
    IView::Probe(child)->SetTag(TAG_ANIMATOR_TRANSLATION_Z, animator);

    AutoPtr<IFloat> fObj;
    CFloat::New(fz, (IFloat**)&fObj);
    IView::Probe(child)->SetTag(TAG_START_TRANSLATION_Z, fObj);

    fObj = NULL;
    CFloat::New(newEndValue, (IFloat**)&fObj);
    IView::Probe(child)->SetTag(TAG_END_TRANSLATION_Z, fObj);
}

void StackStateAnimator::StartYTranslationAnimation(
    /* [in] */ IExpandableView* child,
    /* [in] */ ViewState* viewState,
    /* [in] */ Int64 duration,
    /* [in] */ Int64 delay)
{
    AutoPtr<IFloat> _previousStartValue = IFloat::Probe(GetChildTag(IView::Probe(child), TAG_START_TRANSLATION_Y));
    AutoPtr<IFloat> _previousEndValue = IFloat::Probe(GetChildTag(IView::Probe(child), TAG_END_TRANSLATION_Y));
    Float previousStartValue = 0, previousEndValue = 0;
    if (_previousStartValue != NULL) {
        _previousStartValue->GetValue(&previousStartValue);
    }
    Float newEndValue = viewState->mYTranslation;
    if (_previousEndValue != NULL
        && (_previousEndValue->GetValue(&previousEndValue), previousEndValue) == newEndValue) {
        return;
    }
    AutoPtr<IObjectAnimator> previousAnimator
            = IObjectAnimator::Probe(GetChildTag(IView::Probe(child), TAG_ANIMATOR_TRANSLATION_Y));
    if (!mAnimationFilter->mAnimateY) {
        // just a local update was performed
        if (previousAnimator != NULL) {
            // we need to increase all animation keyframes of the previous animator by the
            // relative change to the end value
            AutoPtr<ArrayOf<IPropertyValuesHolder*> > values;
            IValueAnimator::Probe(previousAnimator)->GetValues((ArrayOf<IPropertyValuesHolder*>**)&values);
            _previousEndValue->GetValue(&previousEndValue);
            Float relativeDiff = newEndValue - previousEndValue;
            Float newStartValue = previousStartValue + relativeDiff;

            AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
            (*fvs)[0] = newStartValue;
            (*fvs)[1] = newEndValue;
            (*values)[0]->SetFloatValues(fvs);

            AutoPtr<IFloat> fObj;
            CFloat::New(newStartValue, (IFloat**)&fObj);
            IView::Probe(child)->SetTag(TAG_START_TRANSLATION_Y, fObj);

            fObj = NULL;
            CFloat::New(newEndValue, (IFloat**)&fObj);
            IView::Probe(child)->SetTag(TAG_END_TRANSLATION_Y, fObj);
            Int64 pt = 0;
            IValueAnimator::Probe(previousAnimator)->GetCurrentPlayTime(&pt);
            IValueAnimator::Probe(previousAnimator)->SetCurrentPlayTime(pt);
            return;
        }
        else {
            // no new animation needed, let's just apply the value
            IView::Probe(child)->SetTranslationY(newEndValue);
            return;
        }
    }

    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    Float fy = 0;
    IView::Probe(child)->GetTranslationY(&fy);
    AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
    (*fvs)[0] = fy;
    (*fvs)[1] = newEndValue;
    AutoPtr<IObjectAnimator> animator;
    helper->OfFloat(child, View::TRANSLATION_Y, fvs, (IObjectAnimator**)&animator);
    IAnimator::Probe(animator)->SetInterpolator(ITimeInterpolator::Probe(mFastOutSlowInInterpolator));
    Int64 newDuration = CancelAnimatorAndGetNewDuration(duration, IValueAnimator::Probe(previousAnimator));
    IAnimator::Probe(animator)->SetDuration(newDuration);
    Boolean tmp = FALSE;
    if (delay > 0 && (previousAnimator == NULL || (IAnimator::Probe(previousAnimator)->IsRunning(&tmp), !tmp))) {
        IAnimator::Probe(animator)->SetStartDelay(delay);
    }
    IAnimator::Probe(animator)->AddListener(GetGlobalAnimationFinishedListener());
    // remove the tag when the animation is finished
    AutoPtr<AnimatorListenerAdapter6> listener = new AnimatorListenerAdapter6(child);
    IAnimator::Probe(animator)->AddListener(listener);
    StartAnimator(IValueAnimator::Probe(animator));
    IView::Probe(child)->SetTag(TAG_ANIMATOR_TRANSLATION_Y, animator);

    AutoPtr<IFloat> fObj;
    CFloat::New(fy, (IFloat**)&fObj);
    IView::Probe(child)->SetTag(TAG_START_TRANSLATION_Y, fObj);

    fObj = NULL;
    CFloat::New(newEndValue, (IFloat**)&fObj);
    IView::Probe(child)->SetTag(TAG_END_TRANSLATION_Y, fObj);
}

void StackStateAnimator::StartScaleAnimation(
    /* [in] */ IExpandableView* child,
    /* [in] */ ViewState* viewState,
    /* [in] */ Int64 duration)
{
    AutoPtr<IFloat> _previousStartValue = IFloat::Probe(GetChildTag(IView::Probe(child), TAG_START_SCALE));
    AutoPtr<IFloat> _previousEndValue = IFloat::Probe(GetChildTag(IView::Probe(child), TAG_END_SCALE));
    Float previousStartValue = 0, previousEndValue = 0;

    if (_previousStartValue != NULL) {
        _previousStartValue->GetValue(&previousStartValue);
    }
    Float newEndValue = viewState->mScale;
    if (_previousEndValue != NULL
        && (_previousEndValue->GetValue(&previousEndValue), previousEndValue) == newEndValue) {
        return;
    }
    AutoPtr<IObjectAnimator> previousAnimator
        = IObjectAnimator::Probe(GetChildTag(IView::Probe(child), TAG_ANIMATOR_SCALE));
    if (!mAnimationFilter->mAnimateScale) {
        // just a local update was performed
        if (previousAnimator != NULL) {
            // we need to increase all animation keyframes of the previous animator by the
            // relative change to the end value
            AutoPtr<ArrayOf<IPropertyValuesHolder*> > values;
            IValueAnimator::Probe(previousAnimator)->GetValues((ArrayOf<IPropertyValuesHolder*>**)&values);
            _previousEndValue->GetValue(&previousEndValue);
            Float relativeDiff = newEndValue - previousEndValue;
            Float newStartValue = previousStartValue + relativeDiff;

            AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
            (*fvs)[0] = newStartValue;
            (*fvs)[1] = newEndValue;
            (*values)[0]->SetFloatValues(fvs);
            (*values)[1]->SetFloatValues(fvs);

            AutoPtr<IFloat> fObj;
            CFloat::New(newStartValue, (IFloat**)&fObj);
            IView::Probe(child)->SetTag(TAG_START_SCALE, fObj);

            fObj = NULL;
            CFloat::New(newEndValue, (IFloat**)&fObj);
            IView::Probe(child)->SetTag(TAG_END_SCALE, fObj);
            Int64 pt = 0;
            IValueAnimator::Probe(previousAnimator)->GetCurrentPlayTime(&pt);
            IValueAnimator::Probe(previousAnimator)->SetCurrentPlayTime(pt);
            return;
        }
        else {
            // no new animation needed, let's just apply the value
            IView::Probe(child)->SetScaleX(newEndValue);
            IView::Probe(child)->SetScaleY(newEndValue);
        }
    }

    AutoPtr<IPropertyValuesHolderHelper> helper;
    CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
    Float fx = 0, fy = 0;
    IView::Probe(child)->GetScaleX(&fx);
    AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
    (*fvs)[0] = fx;
    (*fvs)[1] = newEndValue;
    AutoPtr<IPropertyValuesHolder> holderX;
    helper->OfFloat(View::SCALE_X, fvs, (IPropertyValuesHolder**)&holderX);

    IView::Probe(child)->GetScaleY(&fy);
    (*fvs)[0] = fy;
    (*fvs)[1] = newEndValue;
    AutoPtr<IPropertyValuesHolder> holderY;
    helper->OfFloat(View::SCALE_Y, fvs, (IPropertyValuesHolder**)&holderY);

    AutoPtr<IObjectAnimatorHelper> oAHelper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oAHelper);
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > pvs = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
    pvs->Set(0, holderX);
    pvs->Set(1, holderY);
    AutoPtr<IObjectAnimator> animator;
    oAHelper->OfPropertyValuesHolder(child, pvs, (IObjectAnimator**)&animator);
    IAnimator::Probe(animator)->SetInterpolator(ITimeInterpolator::Probe(mFastOutSlowInInterpolator));
    Int64 newDuration = CancelAnimatorAndGetNewDuration(duration, IValueAnimator::Probe(previousAnimator));
    IAnimator::Probe(animator)->SetDuration(newDuration);
    IAnimator::Probe(animator)->AddListener(GetGlobalAnimationFinishedListener());
    // remove the tag when the animation is finished
    AutoPtr<AnimatorListenerAdapter7> listener = new AnimatorListenerAdapter7(child);
    IAnimator::Probe(animator)->AddListener(listener);
    StartAnimator(IValueAnimator::Probe(animator));
    IView::Probe(child)->SetTag(TAG_ANIMATOR_SCALE, animator);

    AutoPtr<IFloat> fObj;
    CFloat::New(fx, (IFloat**)&fObj);
    IView::Probe(child)->SetTag(TAG_START_SCALE, fObj);

    fObj = NULL;
    CFloat::New(newEndValue, (IFloat**)&fObj);
    IView::Probe(child)->SetTag(TAG_END_SCALE, fObj);
}

void StackStateAnimator::StartAnimator(
    /* [in] */ IValueAnimator* animator)
{
    mAnimatorSet->Add(animator);
    IAnimator::Probe(animator)->Start();
}

AutoPtr<AnimatorListenerAdapter> StackStateAnimator::GetGlobalAnimationFinishedListener()
{
    Boolean empty = FALSE;
    if (mAnimationListenerPool->IsEmpty(&empty), !empty) {
        AutoPtr<IInterface> obj;
        mAnimationListenerPool->Pop((IInterface**)&obj);
        AutoPtr<AnimatorListenerAdapter> adapter = (AnimatorListenerAdapter*)IAnimatorListener::Probe(obj);
        return adapter;
    }

    // We need to create a new one, no reusable ones found
    AutoPtr<AnimatorListenerAdapter8> listener = new AnimatorListenerAdapter8(this);
    return listener;
}

AutoPtr<IInterface> StackStateAnimator::GetChildTag(
    /* [in] */ IView* child,
    /* [in] */ Int32 tag)
{
    AutoPtr<IInterface> obj;
    child->GetTag(tag, (IInterface**)&obj);
    return obj;
}

Int64 StackStateAnimator::CancelAnimatorAndGetNewDuration(
    /* [in] */ Int64 duration,
    /* [in] */ IValueAnimator* previousAnimator)
{
    Int64 newDuration = duration;
    if (previousAnimator != NULL) {
        // We take either the desired length of the new animation or the remaining time of
        // the previous animator, whichever is longer.
        Int64 d = 0, pt = 0;
        previousAnimator->GetDuration(&d);
        previousAnimator->GetCurrentPlayTime(&pt);
        newDuration = Elastos::Core::Math::Max(d - pt, newDuration);
        IAnimator::Probe(previousAnimator)->Cancel();
    }
    return newDuration;
}

void StackStateAnimator::OnAnimationFinished()
{
    mHostLayout->OnChildAnimationFinished();
}

void StackStateAnimator::ProcessAnimationEvents(
    /* [in] */ IArrayList/*<NotificationStackScrollLayout.AnimationEvent>*/* animationEvents,
    /* [in] */ StackScrollState* finalState)
{
    AutoPtr<IIterator> it;
    animationEvents->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    Int32 visibility = 0;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<AnimationEvent> event = (AnimationEvent*)IObject::Probe(obj);

        AutoPtr<IExpandableView> changingView = IExpandableView::Probe(event->mChangingView);
        if (event->mAnimationType == AnimationEvent::ANIMATION_TYPE_ADD) {
            // This item is added, initialize it's properties.
            AutoPtr<IInterface> obj;
            finalState->GetViewStateForView(IView::Probe(changingView), (IInterface**)&obj);
            AutoPtr<ViewState> viewState = (ViewState*)IObject::Probe(obj);
            if (viewState == NULL) {
                // The position for this child was never generated, let's continue.
                continue;
            }

            if ((IView::Probe(changingView)->GetVisibility(&visibility), visibility) == IView::GONE) {
                // The view was set to gone but the state never removed
                finalState->RemoveViewStateForView(IView::Probe(changingView));
                continue;
            }
            IView::Probe(changingView)->SetAlpha(viewState->mAlpha);
            IView::Probe(changingView)->SetTranslationY(viewState->mYTranslation);
            IView::Probe(changingView)->SetTranslationZ(viewState->mZTranslation);
            changingView->SetActualHeight(viewState->mHeight, FALSE);
            mNewAddChildren->Add(changingView);
        }
        else if (event->mAnimationType == AnimationEvent::ANIMATION_TYPE_REMOVE) {
            if ((IView::Probe(changingView)->GetVisibility(&visibility), visibility) == IView::GONE) {
                AutoPtr<IViewOverlay> vo;
                IView::Probe(mHostLayout)->GetOverlay((IViewOverlay**)&vo);
                IViewGroupOverlay::Probe(vo)->Remove(IView::Probe(changingView));
                continue;
            }

            // Find the amount to translate up. This is needed in order to understand the
            // direction of the remove animation (either downwards or upwards)
            AutoPtr<IInterface> obj;
            finalState->GetViewStateForView(event->mViewAfterChangingView, (IInterface**)&obj);
            AutoPtr<ViewState> viewState = (ViewState*)IObject::Probe(obj);
            Int32 actualHeight = 0;
            changingView->GetActualHeight(&actualHeight);
            // upwards by default
            Float translationDirection = -1.0f;
            if (viewState != NULL) {
                // there was a view after this one, Approximate the distance the next child
                // travelled
                Float fty = 0;
                IView::Probe(changingView)->GetTranslationY(&fty);
                translationDirection = ((viewState->mYTranslation
                        - (fty + actualHeight / 2.0f)) * 2 /
                        actualHeight);
                translationDirection = Elastos::Core::Math::Max(
                    Elastos::Core::Math::Min(translationDirection, 1.0f), -1.0f);

            }

            AutoPtr<Runnable1> run = new Runnable1(this, changingView);
            changingView->PerformRemoveAnimation(ANIMATION_DURATION_APPEAR_DISAPPEAR,
                    translationDirection, run);
        }
        mNewEvents->Add(event->Probe(EIID_IInterface));
    }
}

ECode StackStateAnimator::AnimateOverScrollToAmount(
    /* [in] */ Float targetAmount,
    /* [in] */ Boolean onTop,
    /* [in] */ Boolean isRubberbanded)
{
    Float startOverScrollAmount = 0;
    mHostLayout->GetCurrentOverScrollAmount(onTop, &startOverScrollAmount);
    if (targetAmount == startOverScrollAmount) {
        return NOERROR;
    }
    CancelOverScrollAnimators(onTop);

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<IValueAnimator> overScrollAnimator;
    AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
    (*fvs)[0] = startOverScrollAmount;
    (*fvs)[1] = targetAmount;
    helper->OfFloat(fvs, (IValueAnimator**)&overScrollAnimator);
    IAnimator::Probe(overScrollAnimator)->SetDuration(ANIMATION_DURATION_STANDARD);
    AutoPtr<AnimatorUpdateListener3> listener = new AnimatorUpdateListener3(this, onTop, isRubberbanded);
    overScrollAnimator->AddUpdateListener(listener);
    IAnimator::Probe(overScrollAnimator)->SetInterpolator(ITimeInterpolator::Probe(mFastOutSlowInInterpolator));
    AutoPtr<AnimatorListenerAdapter9> listener2 = new AnimatorListenerAdapter9(this, onTop);
    IAnimator::Probe(overScrollAnimator)->AddListener(listener2);
    IAnimator::Probe(overScrollAnimator)->Start();
    if (onTop) {
        mTopOverScrollAnimator = overScrollAnimator;
    }
    else {
        mBottomOverScrollAnimator = overScrollAnimator;
    }
    return NOERROR;
}

ECode StackStateAnimator::CancelOverScrollAnimators(
    /* [in] */ Boolean onTop)
{
    AutoPtr<IValueAnimator> currentAnimator = onTop ? mTopOverScrollAnimator : mBottomOverScrollAnimator;
    if (currentAnimator != NULL) {
        IAnimator::Probe(currentAnimator)->Cancel();
    }
    return NOERROR;
}

Int32 StackStateAnimator::GetFinalActualHeight(
    /* [in] */ IExpandableView* view)
{
    if (view == NULL) {
        return 0;
    }
    AutoPtr<IValueAnimator> heightAnimator
            = IValueAnimator::Probe(GetChildTag(IView::Probe(view), TAG_ANIMATOR_HEIGHT));
    Int32 v = 0;
    if (heightAnimator == NULL) {
        view->GetActualHeight(&v);
        return v;
    }

    AutoPtr<IInteger32> iobj = IInteger32::Probe(GetChildTag(IView::Probe(view), TAG_END_HEIGHT));
    iobj->GetValue(&v);
    return v;
}

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
