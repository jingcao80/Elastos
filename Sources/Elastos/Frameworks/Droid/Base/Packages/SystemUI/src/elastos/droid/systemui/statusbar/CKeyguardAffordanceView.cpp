
#include "elastos/droid/systemui/statusbar/CKeyguardAffordanceView.h"
#include "../R.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/R.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::CArgbEvaluator;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IArgbEvaluator;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::ITypeEvaluator;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_ATOP;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::CViewAnimationUtilsHelper;
using Elastos::Droid::View::IViewAnimationUtilsHelper;
using Elastos::Core::CInteger32;
using Elastos::Core::IFloat;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CKeyguardAffordanceView::ClipEndListener::ClipEndListener(
    /* [in] */ CKeyguardAffordanceView* host)
    : mHost(host)
{}

ECode CKeyguardAffordanceView::ClipEndListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mPreviewClipper = NULL;
    return NOERROR;
}

CKeyguardAffordanceView::CircleEndListener::CircleEndListener(
    /* [in] */ CKeyguardAffordanceView* host)
    : mHost(host)
{}

ECode CKeyguardAffordanceView::CircleEndListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mCircleAnimator = NULL;
    return NOERROR;
}

CKeyguardAffordanceView::ScaleEndListener::ScaleEndListener(
    /* [in] */ CKeyguardAffordanceView* host)
    : mHost(host)
{}

ECode CKeyguardAffordanceView::ScaleEndListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mScaleAnimator = NULL;
    return NOERROR;
}

CKeyguardAffordanceView::AlphaEndListener::AlphaEndListener(
    /* [in] */ CKeyguardAffordanceView* host)
    : mHost(host)
{}

ECode CKeyguardAffordanceView::AlphaEndListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mAlphaAnimator = NULL;
    return NOERROR;
}

CKeyguardAffordanceView::ArrowEndListener::ArrowEndListener(
    /* [in] */ CKeyguardAffordanceView* host)
    : mHost(host)
{}

ECode CKeyguardAffordanceView::ArrowEndListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mArrowAnimator = NULL;
    return NOERROR;
}

CKeyguardAffordanceView::AnimatorListenerAdapter1::AnimatorListenerAdapter1(
    /* [in] */ IRunnable* runnable)
    : mAnimationEndRunnable(runnable)
{}

ECode CKeyguardAffordanceView::AnimatorListenerAdapter1::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mAnimationEndRunnable->Run();
    return NOERROR;
}

CKeyguardAffordanceView::AnimatorListenerAdapter2::AnimatorListenerAdapter2(
    /* [in] */ CKeyguardAffordanceView* host)
    : mHost(host)
{}

ECode CKeyguardAffordanceView::AnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mPreviewView->SetVisibility(IView::INVISIBLE);
    return NOERROR;
}

CKeyguardAffordanceView::AnimatorListenerAdapter3::AnimatorListenerAdapter3(
    /* [in] */ IRunnable* runnable)
    : mRunnable(runnable)
    , mCancelled(FALSE)
{}

ECode CKeyguardAffordanceView::AnimatorListenerAdapter3::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mCancelled = TRUE;
    return NOERROR;
}

ECode CKeyguardAffordanceView::AnimatorListenerAdapter3::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (!mCancelled) {
        mRunnable->Run();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardAffordanceView::AnimatorUpdateListener, Object, IAnimatorUpdateListener);
CKeyguardAffordanceView::AnimatorUpdateListener::AnimatorUpdateListener(
    /* [in] */ CKeyguardAffordanceView* host)
    : mHost(host)
{}

ECode CKeyguardAffordanceView::AnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    IFloat::Probe(obj)->GetValue(&mHost->mCircleRadius);
    mHost->UpdateIconColor();
    mHost->Invalidate();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardAffordanceView::AnimatorUpdateListener2, Object, IAnimatorUpdateListener);
CKeyguardAffordanceView::AnimatorUpdateListener2::AnimatorUpdateListener2(
    /* [in] */ CKeyguardAffordanceView* host)
    : mHost(host)
{}

ECode CKeyguardAffordanceView::AnimatorUpdateListener2::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    IFloat::Probe(obj)->GetValue(&mHost->mImageScale);
    mHost->Invalidate();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardAffordanceView::AnimatorUpdateListener3, Object, IAnimatorUpdateListener);
CKeyguardAffordanceView::AnimatorUpdateListener3::AnimatorUpdateListener3(
    /* [in] */ CKeyguardAffordanceView* host,
    /* [in] */ IDrawable* background)
    : mHost(host)
    , mBackground(background)
{}

ECode CKeyguardAffordanceView::AnimatorUpdateListener3::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    Int32 alpha = 0;
    IInteger32::Probe(obj)->GetValue(&alpha);
    if (mBackground != NULL) {
        AutoPtr<IDrawable> d;
        mBackground->Mutate((IDrawable**)&d);
        d->SetAlpha(alpha);
    }
    mHost->SetImageAlpha(alpha);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardAffordanceView::AnimatorUpdateListener4, Object, IAnimatorUpdateListener);
CKeyguardAffordanceView::AnimatorUpdateListener4::AnimatorUpdateListener4(
    /* [in] */ CKeyguardAffordanceView* host)
    : mHost(host)
{}

ECode CKeyguardAffordanceView::AnimatorUpdateListener4::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    IFloat::Probe(obj)->GetValue(&mHost->mArrowAlpha);
    mHost->Invalidate();
    return NOERROR;
}

const Float CKeyguardAffordanceView::MAX_ICON_SCALE_AMOUNT = 1.5f;
const Float CKeyguardAffordanceView::MIN_ICON_SCALE_AMOUNT = 0.8f;
const Int64 CKeyguardAffordanceView::CIRCLE_APPEAR_DURATION = 80;
const Int64 CKeyguardAffordanceView::CIRCLE_DISAPPEAR_MAX_DURATION = 200;
const Int64 CKeyguardAffordanceView::NORMAL_ANIMATION_DURATION = 200;

CAR_OBJECT_IMPL(CKeyguardAffordanceView);
CAR_INTERFACE_IMPL(CKeyguardAffordanceView, ImageView, IKeyguardAffordanceView);
CKeyguardAffordanceView::CKeyguardAffordanceView()
    : mMinBackgroundRadius(0)
    , mInverseColor(0)
    , mNormalColor(0)
    , mHintChevronPadding(0)
    , mCircleRadius(0)
    , mCenterX(0)
    , mCenterY(0)
    , mCircleStartValue(0)
    , mCircleWillBeHidden(FALSE)
    , mImageScale(0)
    , mCircleColor(0)
    , mIsLeft(0)
    , mArrowAlpha(0.0f)
    , mCircleStartRadius(0)
    , mMaxCircleSize(0)
{
    mTempPoint = ArrayOf<Int32>::Alloc(2);
    mClipEndListener = new ClipEndListener(this);
    mCircleEndListener = new CircleEndListener(this);
    mScaleEndListener = new ScaleEndListener(this);
    mAlphaEndListener = new AlphaEndListener(this);
    mArrowEndListener = new ArrowEndListener(this);
}

ECode CKeyguardAffordanceView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CKeyguardAffordanceView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CKeyguardAffordanceView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CKeyguardAffordanceView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ImageView::constructor(context, attrs, defStyleAttr, defStyleRes);
    CPaint::New((IPaint**)&mCirclePaint);
    mCirclePaint->SetAntiAlias(TRUE);
    mCircleColor = 0xffffffff;
    mCirclePaint->SetColor(mCircleColor);

    mNormalColor = 0xffffffff;
    mInverseColor = 0xff000000;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::keyguard_affordance_min_background_radius, &mMinBackgroundRadius);
    res->GetDimensionPixelSize(R::dimen::hint_chevron_circle_padding, &mHintChevronPadding);

    AutoPtr<IAnimationUtils> au;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&au);
    au->LoadInterpolator(mContext,
            Elastos::Droid::R::interpolator::linear_out_slow_in, (IInterpolator**)&mAppearInterpolator);
    au->LoadInterpolator(mContext,
            Elastos::Droid::R::interpolator::fast_out_linear_in, (IInterpolator**)&mDisappearInterpolator);

    CArgbEvaluator::New((IArgbEvaluator**)&mColorInterpolator);
    mFlingAnimationUtils = new FlingAnimationUtils(mContext, 0.3f);
    context->GetDrawable(R::drawable::ic_chevron_left, (IDrawable**)&mArrowDrawable);
    Int32 width = 0, height = 0;
    mArrowDrawable->GetIntrinsicWidth(&width);
    mArrowDrawable->GetIntrinsicHeight(&height);
    mArrowDrawable->SetBounds(0, 0, width, height);
    return NOERROR;
}

ECode CKeyguardAffordanceView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    ImageView::OnLayout(changed, left, top, right, bottom);
    Int32 value = 0;
    mCenterX = (GetWidth(&value), value) / 2;
    mCenterY = (GetHeight(&value), value) / 2;
    mMaxCircleSize = GetMaxCircleSize();
    return NOERROR;
}

void CKeyguardAffordanceView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    DrawBackgroundCircle(canvas);
    DrawArrow(canvas);
    Int32 save = 0;
    canvas->Save(&save);
    Int32 width = 0, height = 0;
    GetWidth(&width);
    GetHeight(&height);
    canvas->Scale(mImageScale, mImageScale, width / 2, height / 2);
    ImageView::OnDraw(canvas);
    canvas->Restore();
}

ECode CKeyguardAffordanceView::SetPreviewView(
    /* [in] */ IView* v)
{
    mPreviewView = v;
    if (mPreviewView != NULL) {
        mPreviewView->SetVisibility(IView::INVISIBLE);
    }
    return NOERROR;
}

void CKeyguardAffordanceView::DrawArrow(
    /* [in] */ ICanvas* canvas)
{
    if (mArrowAlpha > 0) {
        Int32 save = 0;
        canvas->Save(&save);
        canvas->Translate(mCenterX, mCenterY);
        if (mIsLeft) {
            canvas->Scale(-1.0f, 1.0f);
        }

        Int32 width = 0, height = 0;
        mArrowDrawable->GetIntrinsicWidth(&width);
        mArrowDrawable->GetIntrinsicHeight(&height);
        canvas->Translate(- mCircleRadius - mHintChevronPadding - width / 2, - height / 2);
        mArrowDrawable->SetAlpha((Int32) (mArrowAlpha * 255));
        mArrowDrawable->Draw(canvas);
        canvas->Restore();
    }
}

void CKeyguardAffordanceView::UpdateIconColor()
{
    AutoPtr<IDrawable> d;
    GetDrawable((IDrawable**)&d);
    AutoPtr<IDrawable> drawable;
    d->Mutate((IDrawable**)&drawable);
    Float alpha = mCircleRadius / mMinBackgroundRadius;
    alpha = Elastos::Core::Math::Min(1.0f, alpha);

    AutoPtr<IInteger32> integer1, integer2;
    CInteger32::New(mNormalColor, (IInteger32**)&integer1);
    CInteger32::New(mInverseColor, (IInteger32**)&integer2);
    AutoPtr<IInterface> obj;
    ITypeEvaluator::Probe(mColorInterpolator)->Evaluate(alpha, integer1, integer2, (IInterface**)&obj);
    Int32 color = 0;
    IInteger32::Probe(obj)->GetValue(&color);
    drawable->SetColorFilter(color, PorterDuffMode_SRC_ATOP);
}

void CKeyguardAffordanceView::DrawBackgroundCircle(
    /* [in] */ ICanvas* canvas)
{
    if (mCircleRadius > 0) {
        UpdateCircleColor();
        canvas->DrawCircle(mCenterX, mCenterY, mCircleRadius, mCirclePaint);
    }
}

void CKeyguardAffordanceView::UpdateCircleColor()
{
    Float fraction = 0.5f + 0.5f * Elastos::Core::Math::Max(0.0f, Elastos::Core::Math::Min(1.0f,
            (mCircleRadius - mMinBackgroundRadius) / (0.5f * mMinBackgroundRadius)));
    if (mPreviewView != NULL) {
        Float finishingFraction = 1 - Elastos::Core::Math::Max(0, (Int32)(mCircleRadius - mCircleStartRadius))
                / (mMaxCircleSize - mCircleStartRadius);
        fraction *= finishingFraction;
    }

    AutoPtr<IColor> c;
    CColor::AcquireSingleton((IColor**)&c);
    Int32 alpha = 0, red = 0, green = 0, blue = 0;
    c->Alpha(mCircleColor, &alpha);
    c->Red(mCircleColor, &red);
    c->Green(mCircleColor, &green);
    c->Blue(mCircleColor, &blue);
    Int32 color = 0;
    c->Argb((Int32) (alpha * fraction), red, green, blue, &color);
    mCirclePaint->SetColor(color);
}

ECode CKeyguardAffordanceView::FinishAnimation(
    /* [in] */ Float velocity,
    /* [in] */ IRunnable* mAnimationEndRunnable)
{
    CancelAnimator(IAnimator::Probe(mCircleAnimator));
    CancelAnimator(mPreviewClipper);
    mCircleStartRadius = mCircleRadius;
    Float maxCircleSize = GetMaxCircleSize();
    AutoPtr<IValueAnimator> animatorToRadius = GetAnimatorToRadius(maxCircleSize);
    mFlingAnimationUtils->ApplyDismissing(IAnimator::Probe(animatorToRadius), mCircleRadius, maxCircleSize,
            velocity, maxCircleSize);
    AutoPtr<AnimatorListenerAdapter1> adapter = new AnimatorListenerAdapter1(mAnimationEndRunnable);
    IAnimator::Probe(animatorToRadius)->AddListener(adapter);
    IAnimator::Probe(animatorToRadius)->Start();
    SetImageAlpha(0, TRUE);
    if (mPreviewView != NULL) {
        mPreviewView->SetVisibility(IView::VISIBLE);

        Int32 left = 0, top = 0;
        GetLeft(&left);
        GetTop(&top);
        AutoPtr<IViewAnimationUtilsHelper> helper;
        CViewAnimationUtilsHelper::AcquireSingleton((IViewAnimationUtilsHelper**)&helper);
        helper->CreateCircularReveal(
                mPreviewView, left + mCenterX, top + mCenterY, mCircleRadius,
                maxCircleSize, (IAnimator**)&mPreviewClipper);
        mFlingAnimationUtils->ApplyDismissing(mPreviewClipper, mCircleRadius, maxCircleSize,
                velocity, maxCircleSize);
        mPreviewClipper->AddListener(mClipEndListener);
        mPreviewClipper->Start();
    }
    return NOERROR;
}

Float CKeyguardAffordanceView::GetMaxCircleSize()
{
    GetLocationInWindow(mTempPoint);
    AutoPtr<IView> view;
    GetRootView((IView**)&view);
    Int32 w = 0;
    view->GetWidth(&w);
    Float rootWidth = w;
    Float width = (*mTempPoint)[0] + mCenterX;
    width = Elastos::Core::Math::Max(rootWidth - width, width);
    Float height = (*mTempPoint)[1] + mCenterY;
    return (Float) Elastos::Core::Math::Hypot(width, height);
}

ECode CKeyguardAffordanceView::SetCircleRadius(
    /* [in] */ Float circleRadius)
{
    SetCircleRadius(circleRadius, FALSE, FALSE);
    return NOERROR;
}

ECode CKeyguardAffordanceView::SetCircleRadius(
    /* [in] */ Float circleRadius,
    /* [in] */ Boolean slowAnimation)
{
    SetCircleRadius(circleRadius, slowAnimation, FALSE);
    return NOERROR;
}

ECode CKeyguardAffordanceView::SetCircleRadiusWithoutAnimation(
    /* [in] */ Float circleRadius)
{
    CancelAnimator(IAnimator::Probe(mCircleAnimator));
    SetCircleRadius(circleRadius, FALSE ,TRUE);
    return NOERROR;
}

void CKeyguardAffordanceView::SetCircleRadius(
    /* [in] */ Float circleRadius,
    /* [in] */ Boolean slowAnimation,
    /* [in] */ Boolean noAnimation)
{
    // Check if we need a new animation
    Boolean radiusHidden = (mCircleAnimator != NULL && mCircleWillBeHidden)
            || (mCircleAnimator == NULL && mCircleRadius == 0.0f);
    Boolean nowHidden = circleRadius == 0.0f;
    Boolean radiusNeedsAnimation = (radiusHidden != nowHidden) && !noAnimation;
    if (!radiusNeedsAnimation) {
        if (mCircleAnimator == NULL) {
            mCircleRadius = circleRadius;
            UpdateIconColor();
            Invalidate();
            if (nowHidden) {
                if (mPreviewView != NULL) {
                    mPreviewView->SetVisibility(IView::INVISIBLE);
                }
            }
        }
        else if (!mCircleWillBeHidden) {
            // We just update the end value
            Float diff = circleRadius - mMinBackgroundRadius;
            AutoPtr<ArrayOf<IPropertyValuesHolder*> > values;
            mCircleAnimator->GetValues((ArrayOf<IPropertyValuesHolder*>**)&values);

            AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
            (*array)[0] = mCircleStartValue + diff;
            (*array)[1] = circleRadius;
            (*values)[0]->SetFloatValues(array);
            Int64 time = 0;
            mCircleAnimator->GetCurrentPlayTime(&time);
            mCircleAnimator->SetCurrentPlayTime(time);
        }
    }
    else {
        CancelAnimator(IAnimator::Probe(mCircleAnimator));
        CancelAnimator(mPreviewClipper);
        AutoPtr<IValueAnimator> animator = GetAnimatorToRadius(circleRadius);
        AutoPtr<IInterpolator> interpolator = circleRadius == 0.0f
                ? mDisappearInterpolator
                : mAppearInterpolator;
        IAnimator::Probe(animator)->SetInterpolator(ITimeInterpolator::Probe(interpolator));
        Int64 duration = 250;
        if (!slowAnimation) {
            Float durationFactor = Elastos::Core::Math::Abs(mCircleRadius - circleRadius)
                    / (Float) mMinBackgroundRadius;
            duration = (Int64) (CIRCLE_APPEAR_DURATION * durationFactor);
            duration = Elastos::Core::Math::Min(duration, CIRCLE_DISAPPEAR_MAX_DURATION);
        }
        animator->SetDuration(duration);
        IAnimator::Probe(animator)->Start();
        Int32 visible = 0;
        if (mPreviewView != NULL && (mPreviewView->GetVisibility(&visible), visible) == IView::VISIBLE) {
            mPreviewView->SetVisibility(IView::VISIBLE);

            Int32 left = 0, top = 0;
            GetLeft(&left);
            GetTop(&top);
            AutoPtr<IViewAnimationUtilsHelper> helper;
            CViewAnimationUtilsHelper::AcquireSingleton((IViewAnimationUtilsHelper**)&helper);
            helper->CreateCircularReveal(mPreviewView, left + mCenterX, top + mCenterY, mCircleRadius,
                    circleRadius, (IAnimator**)&mPreviewClipper);
            mPreviewClipper->SetInterpolator(ITimeInterpolator::Probe(interpolator));
            mPreviewClipper->SetDuration(duration);
            mPreviewClipper->AddListener(mClipEndListener);

            AutoPtr<AnimatorListenerAdapter2> adapter = new AnimatorListenerAdapter2(this);
            mPreviewClipper->AddListener(adapter);
            mPreviewClipper->Start();
        }
    }
}

AutoPtr<IValueAnimator> CKeyguardAffordanceView::GetAnimatorToRadius(
    /* [in] */ Float circleRadius)
{
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<IValueAnimator> animator;

    AutoPtr<ArrayOf<Float> > floats = ArrayOf<Float>::Alloc(2);
    (*floats)[0] = mCircleRadius;
    (*floats)[1] = circleRadius;
    helper->OfFloat(floats, (IValueAnimator**)&animator);
    mCircleAnimator = animator;
    mCircleStartValue = mCircleRadius;
    mCircleWillBeHidden = circleRadius == 0.0f;
    AutoPtr<AnimatorUpdateListener> listener = new AnimatorUpdateListener(this);
    animator->AddUpdateListener(listener);
    IAnimator::Probe(animator)->AddListener(mCircleEndListener);
    return animator;
}

void CKeyguardAffordanceView::CancelAnimator(
    /* [in] */ IAnimator* animator)
{
    if (animator != NULL) {
        animator->Cancel();
    }
}

ECode CKeyguardAffordanceView::SetImageScale(
    /* [in] */ Float imageScale,
    /* [in] */ Boolean animate)
{
    SetImageScale(imageScale, animate, -1, NULL);
    return NOERROR;
}

ECode CKeyguardAffordanceView::SetImageScale(
    /* [in] */ Float imageScale,
    /* [in] */ Boolean animate,
    /* [in] */ Int64 duration,
    /* [in] */ IInterpolator* interpolator)
{
    CancelAnimator(IAnimator::Probe(mScaleAnimator));
    if (!animate) {
        mImageScale = imageScale;
        Invalidate();
    }
    else {
        AutoPtr<ArrayOf<Float> > floats = ArrayOf<Float>::Alloc(2);
        (*floats)[0] = mImageScale;
        (*floats)[1] = imageScale;

        AutoPtr<IValueAnimatorHelper> helper;
        CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
        AutoPtr<IValueAnimator> animator;
        helper->OfFloat(floats, (IValueAnimator**)&animator);
        mScaleAnimator = animator;
        AutoPtr<AnimatorUpdateListener2> listener = new AnimatorUpdateListener2(this);
        animator->AddUpdateListener(listener);
        IAnimator::Probe(animator)->AddListener(mScaleEndListener);
        if (interpolator == NULL) {
            interpolator = imageScale == 0.0f
                    ? mDisappearInterpolator
                    : mAppearInterpolator;
        }
        IAnimator::Probe(animator)->SetInterpolator(ITimeInterpolator::Probe(interpolator));
        if (duration == -1) {
            Float durationFactor = Elastos::Core::Math::Abs(mImageScale - imageScale)
                    / (1.0f - MIN_ICON_SCALE_AMOUNT);
            durationFactor = Elastos::Core::Math::Min(1.0f, durationFactor);
            duration = (Int64) (NORMAL_ANIMATION_DURATION * durationFactor);
        }
        animator->SetDuration(duration);
        IAnimator::Probe(animator)->Start();
    }
    return NOERROR;
}

ECode CKeyguardAffordanceView::SetImageAlpha(
    /* [in] */ Float alpha,
    /* [in] */ Boolean animate)
{
    SetImageAlpha(alpha, animate, -1, NULL, NULL);
    return NOERROR;
}

ECode CKeyguardAffordanceView::SetImageAlpha(
    /* [in] */ Float alpha,
    /* [in] */ Boolean animate,
    /* [in] */ Int64 duration,
    /* [in] */ IInterpolator* interpolator,
    /* [in] */ IRunnable* runnable)
{
    CancelAnimator(IAnimator::Probe(mAlphaAnimator));
    Int32 endAlpha = (Int32) (alpha * 255);
    AutoPtr<IDrawable> background;
    GetBackground((IDrawable**)&background);
    if (!animate) {
        if (background != NULL) {
            AutoPtr<IDrawable> d;
            background->Mutate((IDrawable**)&d);
            d->SetAlpha(endAlpha);
        }
        SetImageAlpha(endAlpha);
    }
    else {
        Int32 currentAlpha = 0;
        GetImageAlpha(&currentAlpha);

        AutoPtr<ArrayOf<Int32> > ints = ArrayOf<Int32>::Alloc(2);
        (*ints)[0] = currentAlpha;
        (*ints)[1] = endAlpha;

        AutoPtr<IValueAnimatorHelper> helper;
        CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
        AutoPtr<IValueAnimator> animator;
        helper->OfInt32(ints, (IValueAnimator**)&animator);
        mAlphaAnimator = animator;
        AutoPtr<AnimatorUpdateListener3> listener = new AnimatorUpdateListener3(this, background);
        animator->AddUpdateListener(listener);
        IAnimator::Probe(animator)->AddListener(mAlphaEndListener);
        if (interpolator == NULL) {
            interpolator = alpha == 0.0f
                    ? mDisappearInterpolator
                    : mAppearInterpolator;
        }
        IAnimator::Probe(animator)->SetInterpolator(ITimeInterpolator::Probe(interpolator));
        if (duration == -1) {
            Float durationFactor = Elastos::Core::Math::Abs(currentAlpha - endAlpha) / 255.f;
            durationFactor = Elastos::Core::Math::Min(1.0f, durationFactor);
            duration = (Int64) (NORMAL_ANIMATION_DURATION * durationFactor);
        }
        animator->SetDuration(duration);
        if (runnable != NULL) {
            AutoPtr<IAnimatorListener> listener = GetEndListener(runnable);
            IAnimator::Probe(animator)->AddListener(listener);
        }
        IAnimator::Probe(animator)->Start();
    }
    return NOERROR;
}

AutoPtr<IAnimatorListener> CKeyguardAffordanceView::GetEndListener(
    /* [in] */ IRunnable* runnable)
{
    AutoPtr<IAnimatorListener> listener = new AnimatorListenerAdapter3(runnable);
    return listener;
}

ECode CKeyguardAffordanceView::GetCircleRadius(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCircleRadius;
    return NOERROR;
}

ECode CKeyguardAffordanceView::ShowArrow(
    /* [in] */ Boolean show)
{
    CancelAnimator(IAnimator::Probe(mArrowAnimator));
    Float targetAlpha = show ? 1.0f : 0.0f;
    if (mArrowAlpha == targetAlpha) {
        return NOERROR;
    }

    AutoPtr<ArrayOf<Float> > floats = ArrayOf<Float>::Alloc(2);
    (*floats)[0] = mArrowAlpha;
    (*floats)[1] = targetAlpha;

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);

    AutoPtr<IValueAnimator> animator;
    helper->OfFloat(floats, (IValueAnimator**)&animator);
    mArrowAnimator = animator;
    AutoPtr<AnimatorUpdateListener4> listener = new AnimatorUpdateListener4(this);
    animator->AddUpdateListener(listener);
    IAnimator::Probe(animator)->AddListener(mArrowEndListener);
    AutoPtr<IInterpolator> interpolator = show
                ? mAppearInterpolator
                : mDisappearInterpolator;
    IAnimator::Probe(animator)->SetInterpolator(ITimeInterpolator::Probe(interpolator));
    Float durationFactor = Elastos::Core::Math::Abs(mArrowAlpha - targetAlpha);
    Int64 duration = (Int64) (NORMAL_ANIMATION_DURATION * durationFactor);
    animator->SetDuration(duration);
    IAnimator::Probe(animator)->Start();
    return NOERROR;
}

ECode CKeyguardAffordanceView::SetIsLeft(
    /* [in] */ Boolean left)
{
    mIsLeft = left;
    return NOERROR;
}

ECode CKeyguardAffordanceView::PerformClick(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    if (IsClickable(&tmp), tmp) {
        return ImageView::PerformClick(result);
    }
    *result = FALSE;
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
