
#include "elastos/droid/systemui/statusbar/ActivatableNotificationView.h"
#include "elastos/droid/systemui/statusbar/CNotificationBackgroundView.h"
#include "R.h"
#include <elastos/droid/R.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::CBitmapShader;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::CColorMatrix;
using Elastos::Droid::Graphics::CColorMatrixColorFilter;
using Elastos::Droid::Graphics::CPorterDuffColorFilter;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::IBitmapShader;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IColorMatrix;
using Elastos::Droid::Graphics::IColorMatrixColorFilter;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_ATOP;
using Elastos::Droid::Graphics::ShaderTileMode_CLAMP;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Droid::View::Animation::CPathInterpolator;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::CViewAnimationUtilsHelper;
using Elastos::Droid::View::IViewAnimationUtilsHelper;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Core::IFloat;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

static const String TAG("ActivatableNotificationView");

//==========================================================================================
// ActivatableNotificationView::HostRunnable
//==========================================================================================
ActivatableNotificationView::HostRunnable::HostRunnable(
    /* [in] */ ActivatableNotificationView* host)
    : mHost(host)
{}

ECode ActivatableNotificationView::HostRunnable::Run()
{
    mHost->MakeInactive(TRUE /* animate */);
    return NOERROR;
}

//==========================================================================================
// ActivatableNotificationView::InvisibleAnimatorListenerAdapter
//==========================================================================================
ActivatableNotificationView::InvisibleAnimatorListenerAdapter::InvisibleAnimatorListenerAdapter(
    /* [in] */ ActivatableNotificationView* host)
    : mHost(host)
{}

ECode ActivatableNotificationView::InvisibleAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mHost->mDimmed) {
        IView::Probe(mHost->mBackgroundNormal)->SetVisibility(IView::INVISIBLE);
    }
    return NOERROR;
}

//==========================================================================================
// ActivatableNotificationView::VisibleAnimatorListenerAdapter
//==========================================================================================
ActivatableNotificationView::VisibleAnimatorListenerAdapter::VisibleAnimatorListenerAdapter(
    /* [in] */ ActivatableNotificationView* host)
    : mHost(host)
{}

ECode ActivatableNotificationView::VisibleAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mHost->mDimmed) {
        IView::Probe(mHost->mBackgroundNormal)->SetVisibility(IView::INVISIBLE);
    } else {
        IView::Probe(mHost->mBackgroundDimmed)->SetVisibility(IView::INVISIBLE);
    }
    mHost->mBackgroundAnimator = NULL;
    return NOERROR;
}

//==========================================================================================
// ActivatableNotificationView::AnimatorUpdateListener
//==========================================================================================
CAR_INTERFACE_IMPL(ActivatableNotificationView::AnimatorUpdateListener, Object, IAnimatorUpdateListener)

ActivatableNotificationView::AnimatorUpdateListener::AnimatorUpdateListener(
    /* [in] */ ActivatableNotificationView* host)
    : mHost(host)
{}

ECode ActivatableNotificationView::AnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    IFloat::Probe(obj)->GetValue(&mHost->mAppearAnimationFraction);
    mHost->UpdateAppearAnimationAlpha();
    mHost->UpdateAppearRect();
    mHost->Invalidate();
    return NOERROR;
}

//==========================================================================================
// ActivatableNotificationView::AnimatorListenerAdapter3
//==========================================================================================
ActivatableNotificationView::AnimatorListenerAdapter3::AnimatorListenerAdapter3(
    /* [in] */ ActivatableNotificationView* host,
    /* [in] */ IRunnable* runnable)
    : mHost(host)
    , mRunnable(runnable)
    , mWasCancelled(FALSE)
{}

ECode ActivatableNotificationView::AnimatorListenerAdapter3::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mRunnable != NULL) {
        mRunnable->Run();
    }
    if (!mWasCancelled) {
        mHost->mAppearAnimationFraction = -1;
        mHost->SetOutlineRect(NULL);
        mHost->EnableAppearDrawing(FALSE);
    }
    return NOERROR;
}

ECode ActivatableNotificationView::AnimatorListenerAdapter3::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mWasCancelled = FALSE;
    return NOERROR;
}

ECode ActivatableNotificationView::AnimatorListenerAdapter3::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mWasCancelled = TRUE;
    return NOERROR;
}

//==========================================================================================
// ActivatableNotificationView
//==========================================================================================
const Int64 ActivatableNotificationView::DOUBLETAP_TIMEOUT_MS = 1200;
const Int32 ActivatableNotificationView::BACKGROUND_ANIMATION_LENGTH_MS = 220;
const Int32 ActivatableNotificationView::ACTIVATE_ANIMATION_LENGTH = 220;
const Float ActivatableNotificationView::HORIZONTAL_COLLAPSED_REST_PARTIAL = 0.05f;
const Float ActivatableNotificationView::HORIZONTAL_ANIMATION_END = 0.2f;
const Float ActivatableNotificationView::ALPHA_ANIMATION_END = 0.0f;
const Float ActivatableNotificationView::HORIZONTAL_ANIMATION_START = 1.0f;
const Float ActivatableNotificationView::VERTICAL_ANIMATION_START = 1.0f;

static AutoPtr<IInterpolator> InitPathInterpolator(
    /* [in] */ Float f1,
    /* [in] */ Float f2,
    /* [in] */ Float f3,
    /* [in] */ Float f4)
{
    AutoPtr<IInterpolator> obj;
    CPathInterpolator::New(f1, f2, f3, f4, (IInterpolator**)&obj);
    return obj;
}
AutoPtr<IInterpolator> ActivatableNotificationView::ACTIVATE_INVERSE_INTERPOLATOR = InitPathInterpolator(0.6f, 0, 0.5f, 1);
AutoPtr<IInterpolator> ActivatableNotificationView::ACTIVATE_INVERSE_ALPHA_INTERPOLATOR = InitPathInterpolator(0, 0, 0.5f, 1);

CAR_INTERFACE_IMPL(ActivatableNotificationView, ExpandableOutlineView, IActivatableNotificationView)

ActivatableNotificationView::ActivatableNotificationView()
    : mTintedRippleColor(0)
    , mLowPriorityRippleColor(0)
    , mNormalRippleColor(0)
    , mDimmed(FALSE)
    , mDark(FALSE)
    , mBgTint(0)
    , mRoundedRectCornerRadius(0)
    , mActivated(FALSE)
    , mDownX(0)
    , mDownY(0)
    , mTouchSlop(0)
    , mAnimationTranslationY(0)
    , mDrawingAppearAnimation(FALSE)
    , mAppearAnimationFraction(-1.0f)
    , mAppearAnimationTranslation(0)
    , mShowingLegacyBackground(FALSE)
    , mLegacyColor(0)
    , mNormalColor(0)
    , mLowPriorityColor(0)
    , mIsBelowSpeedBump(FALSE)
{
}

ECode ActivatableNotificationView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ExpandableOutlineView::constructor(context, attrs);

    mTapTimeoutRunnable = new HostRunnable(this);
    mDarkPaint = CreateDarkPaint();
    CRectF::New((IRectF**)&mAppearAnimationRect);
    CPaint::New((IPaint**)&mAppearPaint);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> vc;
    helper->Get(context, (IViewConfiguration**)&vc);
    Int32 tmp = 0;
    vc->GetScaledTouchSlop(&tmp);
    mTouchSlop = tmp;

    AutoPtr<IAnimationUtils> au;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&au);
    au->LoadInterpolator(context, Elastos::Droid::R::interpolator::fast_out_slow_in, (IInterpolator**)&mFastOutSlowInInterpolator);

    CPathInterpolator::New(0.8f, 0.0f, 0.6f, 1.0f, (IInterpolator**)&mSlowOutFastInInterpolator);
    au->LoadInterpolator(context, Elastos::Droid::R::interpolator::linear_out_slow_in, (IInterpolator**)&mLinearOutSlowInInterpolator);
    CPathInterpolator::New(0.8f, 0.0f, 1.0f, 1.0f, (IInterpolator**)&mSlowOutLinearInInterpolator);
    CLinearInterpolator::New((IInterpolator**)&mLinearInterpolator);
    SetClipChildren(FALSE);
    SetClipToPadding(FALSE);
    CPorterDuffColorFilter::New(0, PorterDuffMode_SRC_ATOP, (IPorterDuffColorFilter**)&mAppearAnimationFilter);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::notification_material_rounded_rect_radius, &mRoundedRectCornerRadius);
    res->GetColor(R::color::notification_legacy_background_color, &mLegacyColor);
    res->GetColor(R::color::notification_material_background_color, &mNormalColor);
    res->GetColor(R::color::notification_material_background_low_priority_color, &mLowPriorityColor);

    res = NULL;
    context->GetResources((IResources**)&res);
    res->GetColor(R::color::notification_ripple_tinted_color, &mTintedRippleColor);
    res->GetColor(R::color::notification_ripple_color_low_priority, &mLowPriorityRippleColor);
    res->GetColor(R::color::notification_ripple_untinted_color, &mNormalRippleColor);
    return NOERROR;
}

ECode ActivatableNotificationView::OnFinishInflate()
{
    ExpandableOutlineView::OnFinishInflate();
    AutoPtr<IView> view;
    FindViewById(R::id::backgroundNormal, (IView**)&view);
    mBackgroundNormal = INotificationBackgroundView::Probe(view);
    view = NULL;
    FindViewById(R::id::backgroundDimmed, (IView**)&view);
    mBackgroundDimmed = INotificationBackgroundView::Probe(view);
    mBackgroundNormal->SetCustomBackground(R::drawable::notification_material_bg);
    mBackgroundDimmed->SetCustomBackground(R::drawable::notification_material_bg_dim);
    UpdateBackground();
    UpdateBackgroundTint();
    return NOERROR;
}

ECode ActivatableNotificationView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mDimmed) {
        *result = HandleTouchEventDimmed(event);
        return NOERROR;
    }

    return ExpandableOutlineView::OnTouchEvent(event, result);
}

ECode ActivatableNotificationView::DrawableHotspotChanged(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    if (!mDimmed){
        ((CNotificationBackgroundView*)mBackgroundNormal.Get())->DrawableHotspotChanged(x, y);
    }
    return NOERROR;
}

ECode ActivatableNotificationView::DrawableStateChanged()
{
    ExpandableOutlineView::DrawableStateChanged();
    AutoPtr<ArrayOf<Int32> > array;
    GetDrawableState((ArrayOf<Int32>**)&array);
    if (mDimmed) {
        mBackgroundDimmed->SetState(array);
    }
    else {
        mBackgroundNormal->SetState(array);
    }
    return NOERROR;
}

Boolean ActivatableNotificationView::HandleTouchEventDimmed(
    /* [in] */ IMotionEvent* event)
{
    Int32 action = 0;
    event->GetActionMasked(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            event->GetX(&mDownX);
            event->GetY(&mDownY);
            Int32 value = 0;
            if (mDownY > (GetActualHeight(&value), value)) {
                return FALSE;
            }
            break;
        }
        case IMotionEvent::ACTION_MOVE:
            if (!IsWithinTouchSlop(event)) {
                MakeInactive(TRUE /* animate */);
                return FALSE;
            }
            break;
        case IMotionEvent::ACTION_UP: {
            if (IsWithinTouchSlop(event)) {
                if (!mActivated) {
                    MakeActive();
                    Boolean tmp = FALSE;
                    PostDelayed(mTapTimeoutRunnable, DOUBLETAP_TIMEOUT_MS, &tmp);
                }
                else {
                    Boolean performed = FALSE;
                    PerformClick(&performed);
                    if (performed) {
                        Boolean tmp = FALSE;
                        RemoveCallbacks(mTapTimeoutRunnable, &tmp);
                    }
                }
            }
            else {
                MakeInactive(TRUE /* animate */);
            }
            break;
        }
        case IMotionEvent::ACTION_CANCEL:
            MakeInactive(TRUE /* animate */);
            break;
        default:
            break;
    }
    return TRUE;
}

void ActivatableNotificationView::MakeActive()
{
    StartActivateAnimation(FALSE /* reverse */);
    mActivated = TRUE;
    if (mOnActivatedListener != NULL) {
        mOnActivatedListener->OnActivated(this);
    }
}

void ActivatableNotificationView::StartActivateAnimation(
    /* [in] */ Boolean reverse)
{
    Boolean is = FALSE;
    if (IsAttachedToWindow(&is), !is) {
        return;
    }
    Int32 tmp = 0;
    Int32 widthHalf = (IView::Probe(mBackgroundNormal)->GetWidth(&tmp), tmp) / 2;
    Int32 heightHalf = (mBackgroundNormal->GetActualHeight(&tmp), tmp) / 2;
    Float radius = (Float) Elastos::Core::Math::Sqrt(widthHalf * widthHalf + heightHalf * heightHalf);
    AutoPtr<IAnimator> animator;
    AutoPtr<IViewAnimationUtilsHelper> helper;
    CViewAnimationUtilsHelper::AcquireSingleton((IViewAnimationUtilsHelper**)&helper);
    if (reverse) {
        helper->CreateCircularReveal(IView::Probe(mBackgroundNormal),
                widthHalf, heightHalf, radius, 0, (IAnimator**)&animator);
    }
    else {
        helper->CreateCircularReveal(IView::Probe(mBackgroundNormal),
                widthHalf, heightHalf, 0, radius, (IAnimator**)&animator);
    }
    IView::Probe(mBackgroundNormal)->SetVisibility(IView::VISIBLE);
    AutoPtr<IInterpolator> interpolator;
    AutoPtr<IInterpolator> alphaInterpolator;
    if (!reverse) {
        interpolator = mLinearOutSlowInInterpolator;
        alphaInterpolator = mLinearOutSlowInInterpolator;
    }
    else {
        interpolator = ACTIVATE_INVERSE_INTERPOLATOR;
        alphaInterpolator = ACTIVATE_INVERSE_ALPHA_INTERPOLATOR;
    }
    animator->SetInterpolator(ITimeInterpolator::Probe(interpolator));
    animator->SetDuration(ACTIVATE_ANIMATION_LENGTH);
    if (reverse) {
        IView::Probe(mBackgroundNormal)->SetAlpha(1.f);
        AutoPtr<InvisibleAnimatorListenerAdapter> adapter = new InvisibleAnimatorListenerAdapter(this);
        animator->AddListener(adapter);
        animator->Start();
    }
    else {
        IView::Probe(mBackgroundNormal)->SetAlpha(0.4f);
        animator->Start();
    }

    AutoPtr<IViewPropertyAnimator> anim;
    IView::Probe(mBackgroundNormal)->Animate((IViewPropertyAnimator**)&anim);

    anim->Alpha(reverse ? 0.f : 1.f);
    anim->SetInterpolator(ITimeInterpolator::Probe(alphaInterpolator));
    anim->SetDuration(ACTIVATE_ANIMATION_LENGTH);
}

ECode ActivatableNotificationView::MakeInactive(
    /* [in] */ Boolean animate)
{
    if (mActivated) {
        if (mDimmed) {
            if (animate) {
                StartActivateAnimation(TRUE /* reverse */);
            }
            else {
                IView::Probe(mBackgroundNormal)->SetVisibility(IView::INVISIBLE);
            }
        }
        mActivated = FALSE;
    }
    if (mOnActivatedListener != NULL) {
        mOnActivatedListener->OnActivationReset(this);
    }
    Boolean tmp = FALSE;
    RemoveCallbacks(mTapTimeoutRunnable, &tmp);
    return NOERROR;
}

Boolean ActivatableNotificationView::IsWithinTouchSlop(
    /* [in] */ IMotionEvent* event)
{
    Float x = 0, y = 0;
    return Elastos::Core::Math::Abs((event->GetX(&x), x) - mDownX) < mTouchSlop
            && Elastos::Core::Math::Abs((event->GetY(&y), y) - mDownY) < mTouchSlop;
}

ECode ActivatableNotificationView::SetDimmed(
    /* [in] */ Boolean dimmed,
    /* [in] */ Boolean fade)
{
    if (mDimmed != dimmed) {
        mDimmed = dimmed;
        if (fade) {
            FadeBackground();
        }
        else {
            UpdateBackground();
        }
    }
    return NOERROR;
}

ECode ActivatableNotificationView::SetDark(
    /* [in] */ Boolean dark,
    /* [in] */ Boolean fade)
{
    // TODO implement fade
    if (mDark != dark) {
        mDark = dark;
        if (mDark) {
            SetLayerType(IView::LAYER_TYPE_HARDWARE, mDarkPaint);
        }
        else {
            SetLayerType(IView::LAYER_TYPE_NONE, NULL);
        }
    }
    return NOERROR;
}

AutoPtr<IPaint> ActivatableNotificationView::CreateDarkPaint()
{
    AutoPtr<IPaint> p;
    CPaint::New((IPaint**)&p);
    Float invert[] = {
        -1.f,  0.f,  0.f, 1.f, 1.f,
         0.f, -1.f,  0.f, 1.f, 1.f,
         0.f,  0.f, -1.f, 1.f, 1.f,
         0.f,  0.f,  0.f, 1.f, 0.f
    };

    const Int32 N = sizeof(invert) / sizeof(invert[0]);
    AutoPtr<ArrayOf<Float> > a = ArrayOf<Float>::Alloc(N);
    a->Copy(invert, N);

    AutoPtr<IColorMatrix> m;
    CColorMatrix::New(*a, (IColorMatrix**)&m);
    AutoPtr<IColorMatrix> grayscale;
    CColorMatrix::New((IColorMatrix**)&grayscale);
    grayscale->SetSaturation(0);
    m->PreConcat(grayscale);
    AutoPtr<IColorMatrixColorFilter> filter;
    CColorMatrixColorFilter::New(m, (IColorMatrixColorFilter**)&filter);
    p->SetColorFilter(IColorFilter::Probe(filter));
    return p;
}

ECode ActivatableNotificationView::SetShowingLegacyBackground(
    /* [in] */ Boolean showing)
{
    mShowingLegacyBackground = showing;
    UpdateBackgroundTint();
    return NOERROR;
}

ECode ActivatableNotificationView::SetBelowSpeedBump(
    /* [in] */ Boolean below)
{
    ExpandableOutlineView::SetBelowSpeedBump(below);
    if (below != mIsBelowSpeedBump) {
        mIsBelowSpeedBump = below;
        UpdateBackgroundTint();
    }
    return NOERROR;
}

ECode ActivatableNotificationView::SetTintColor(
    /* [in] */ Int32 color)
{
    mBgTint = color;
    UpdateBackgroundTint();
    return NOERROR;
}

void ActivatableNotificationView::UpdateBackgroundTint()
{
    Int32 color = GetBackgroundColor();
    Int32 rippleColor = GetRippleColor();
    if (color == mNormalColor) {
        // We don't need to tint a normal notification
        color = 0;
    }
    mBackgroundDimmed->SetTint(color);
    mBackgroundNormal->SetTint(color);
    mBackgroundDimmed->SetRippleColor(rippleColor);
    mBackgroundNormal->SetRippleColor(rippleColor);
}

void ActivatableNotificationView::FadeBackground()
{
    AutoPtr<IViewPropertyAnimator> anim;
    IView::Probe(mBackgroundNormal)->Animate((IViewPropertyAnimator**)&anim);
    anim->Cancel();
    if (mDimmed) {
        IView::Probe(mBackgroundDimmed)->SetVisibility(IView::VISIBLE);
    }
    else {
        IView::Probe(mBackgroundNormal)->SetVisibility(IView::VISIBLE);
    }
    Float startAlpha = mDimmed ? 1.f : 0;
    Float endAlpha = mDimmed ? 0 : 1.f;
    Int32 duration = BACKGROUND_ANIMATION_LENGTH_MS;
    // Check whether there is already a background animation running.
    if (mBackgroundAnimator != NULL) {
        AutoPtr<IInterface> obj;
        IValueAnimator::Probe(mBackgroundAnimator)->GetAnimatedValue((IInterface**)&obj);
        IFloat::Probe(obj)->GetValue(&startAlpha);
        Int64 lv = 0;
        duration = (Int32) (IValueAnimator::Probe(mBackgroundAnimator)->GetCurrentPlayTime(&lv), lv);
        IAnimator::Probe(mBackgroundAnimator)->RemoveAllListeners();
        IAnimator::Probe(mBackgroundAnimator)->Cancel();
        if (duration <= 0) {
            UpdateBackground();
            return;
        }
    }
    IView::Probe(mBackgroundNormal)->SetAlpha(startAlpha);
    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr< ArrayOf<Float> > floats = ArrayOf<Float>::Alloc(1);
    floats->Set(0, startAlpha);
    floats->Set(1, endAlpha);
    helper->OfFloat(mBackgroundNormal, View::ALPHA, floats, (IObjectAnimator**)&mBackgroundAnimator);
    IAnimator::Probe(mBackgroundAnimator)->SetInterpolator(ITimeInterpolator::Probe(mFastOutSlowInInterpolator));
    IAnimator::Probe(mBackgroundAnimator)->SetDuration(duration);
    AutoPtr<VisibleAnimatorListenerAdapter> adapter = new VisibleAnimatorListenerAdapter(this);
    IAnimator::Probe(mBackgroundAnimator)->AddListener(adapter);
    IAnimator::Probe(mBackgroundAnimator)->Start();
}

void ActivatableNotificationView::UpdateBackground()
{
    if (mDimmed) {
        IView::Probe(mBackgroundDimmed)->SetVisibility(IView::VISIBLE);
        IView::Probe(mBackgroundNormal)->SetVisibility(IView::INVISIBLE);
    }
    else {
        CancelFadeAnimations();
        IView::Probe(mBackgroundDimmed)->SetVisibility(IView::INVISIBLE);
        IView::Probe(mBackgroundNormal)->SetVisibility(IView::VISIBLE);
        IView::Probe(mBackgroundNormal)->SetAlpha(1.f);
        Boolean tmp = FALSE;
        RemoveCallbacks(mTapTimeoutRunnable, &tmp);
    }
}

void ActivatableNotificationView::CancelFadeAnimations()
{
    if (mBackgroundAnimator != NULL) {
        IAnimator::Probe(mBackgroundAnimator)->Cancel();
    }
    AutoPtr<IViewPropertyAnimator> anim;
    IView::Probe(mBackgroundNormal)->Animate((IViewPropertyAnimator**)&anim);
    anim->Cancel();
}

ECode ActivatableNotificationView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    ExpandableOutlineView::OnLayout(changed, left, top, right, bottom);
    Int32 width = 0;
    SetPivotX((GetWidth(&width), width) / 2);
    return NOERROR;
}

ECode ActivatableNotificationView::SetActualHeight(
    /* [in] */ Int32 actualHeight,
    /* [in] */ Boolean notifyListeners)
{
    ExpandableOutlineView::SetActualHeight(actualHeight, notifyListeners);
    SetPivotY(actualHeight / 2);
    mBackgroundNormal->SetActualHeight(actualHeight);
    mBackgroundDimmed->SetActualHeight(actualHeight);
    return NOERROR;
}

ECode ActivatableNotificationView::SetClipTopAmount(
    /* [in] */ Int32 clipTopAmount)
{
    ExpandableOutlineView::SetClipTopAmount(clipTopAmount);
    mBackgroundNormal->SetClipTopAmount(clipTopAmount);
    mBackgroundDimmed->SetClipTopAmount(clipTopAmount);
    return NOERROR;
}

ECode ActivatableNotificationView::PerformRemoveAnimation(
    /* [in] */ Int64 duration,
    /* [in] */ Float translationDirection,
    /* [in] */ IRunnable* onFinishedRunnable)
{
    EnableAppearDrawing(TRUE);
    if (mDrawingAppearAnimation) {
        StartAppearAnimation(FALSE /* isAppearing */, translationDirection,
                0, duration, onFinishedRunnable);
    }
    else if (onFinishedRunnable != NULL) {
        onFinishedRunnable->Run();
    }
    return NOERROR;
}

ECode ActivatableNotificationView::PerformAddAnimation(
    /* [in] */ Int64 delay,
    /* [in] */ Int64 duration)
{
    EnableAppearDrawing(TRUE);
    if (mDrawingAppearAnimation) {
        StartAppearAnimation(TRUE /* isAppearing */, -1.0f, delay, duration, NULL);
    }
    return NOERROR;
}

void ActivatableNotificationView::StartAppearAnimation(
    /* [in] */ Boolean isAppearing,
    /* [in] */ Float translationDirection,
    /* [in] */ Int64 delay,
    /* [in] */ Int64 duration,
    /* [in] */ IRunnable* onFinishedRunnable)
{
    if (mAppearAnimator != NULL) {
        IAnimator::Probe(mAppearAnimator)->Cancel();
    }
    mAnimationTranslationY = translationDirection * mActualHeight;
    if (mAppearAnimationFraction == -1.0f) {
        // not initialized yet, we start anew
        if (isAppearing) {
            mAppearAnimationFraction = 0.0f;
            mAppearAnimationTranslation = mAnimationTranslationY;
        }
        else {
            mAppearAnimationFraction = 1.0f;
            mAppearAnimationTranslation = 0;
        }
    }

    Float targetValue;
    if (isAppearing) {
        mCurrentAppearInterpolator = mSlowOutFastInInterpolator;
        mCurrentAlphaInterpolator = mLinearOutSlowInInterpolator;
        targetValue = 1.0f;
    }
    else {
        mCurrentAppearInterpolator = mFastOutSlowInInterpolator;
        mCurrentAlphaInterpolator = mSlowOutLinearInInterpolator;
        targetValue = 0.0f;
    }

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > floats = ArrayOf<Float>::Alloc(2);
    (*floats)[0] = mAppearAnimationFraction;
    (*floats)[1] = targetValue;
    helper->OfFloat(floats, (IValueAnimator**)&mAppearAnimator);
    IAnimator::Probe(mAppearAnimator)->SetInterpolator(ITimeInterpolator::Probe(mLinearInterpolator));
    IAnimator::Probe(mAppearAnimator)->SetDuration(
            (Int64) (duration * Elastos::Core::Math::Abs(mAppearAnimationFraction - targetValue)));

    AutoPtr<AnimatorUpdateListener> listener = new AnimatorUpdateListener(this);
    mAppearAnimator->AddUpdateListener(listener);
    if (delay > 0) {
        // we need to apply the initial state already to avoid drawn frames in the wrong state
        UpdateAppearAnimationAlpha();
        UpdateAppearRect();
        IAnimator::Probe(mAppearAnimator)->SetStartDelay(delay);
    }

    AutoPtr<AnimatorListenerAdapter3> adapter = new AnimatorListenerAdapter3(this, onFinishedRunnable);
    IAnimator::Probe(mAppearAnimator)->AddListener(adapter);
    IAnimator::Probe(mAppearAnimator)->Start();
}

void ActivatableNotificationView::UpdateAppearRect()
{
    Float inverseFraction = (1.0f - mAppearAnimationFraction);
    Float translationFraction = 0;
    ITimeInterpolator::Probe(mCurrentAppearInterpolator)->GetInterpolation(inverseFraction, &translationFraction);
    Float translateYTotalAmount = translationFraction * mAnimationTranslationY;
    mAppearAnimationTranslation = translateYTotalAmount;

    // handle width animation
    Float widthFraction = (inverseFraction - (1.0f - HORIZONTAL_ANIMATION_START))
            / (HORIZONTAL_ANIMATION_START - HORIZONTAL_ANIMATION_END);
    widthFraction = Elastos::Core::Math::Min(1.0f, Elastos::Core::Math::Max(0.0f, widthFraction));
    ITimeInterpolator::Probe(mCurrentAppearInterpolator)->GetInterpolation(widthFraction, &widthFraction);
    Int32 width = 0;
    GetWidth(&width);
    Float left = (width * (0.5f - HORIZONTAL_COLLAPSED_REST_PARTIAL / 2.0f) *
            widthFraction);
    Float right = width - left;

    // handle top animation
    Float heightFraction = (inverseFraction - (1.0f - VERTICAL_ANIMATION_START)) /
            VERTICAL_ANIMATION_START;
    heightFraction = Elastos::Core::Math::Max(0.0f, heightFraction);
    ITimeInterpolator::Probe(mCurrentAppearInterpolator)->GetInterpolation(heightFraction, &heightFraction);

    Float top = 0;
    Float bottom = 0;
    if (mAnimationTranslationY > 0.0f) {
        bottom = mActualHeight - heightFraction * mAnimationTranslationY * 0.1f
                - translateYTotalAmount;
        top = bottom * heightFraction;
    }
    else {
        top = heightFraction * (mActualHeight + mAnimationTranslationY) * 0.1f -
                translateYTotalAmount;
        bottom = mActualHeight * (1 - heightFraction) + top * heightFraction;
    }
    mAppearAnimationRect->Set(left, top, right, bottom);
    SetOutlineRect(left, top + mAppearAnimationTranslation, right,
            bottom + mAppearAnimationTranslation);
}

void ActivatableNotificationView::UpdateAppearAnimationAlpha()
{
    Int32 backgroundColor = GetBackgroundColor();
    if (backgroundColor != -1) {
        Float contentAlphaProgress = mAppearAnimationFraction;
        contentAlphaProgress = contentAlphaProgress / (1.0f - ALPHA_ANIMATION_END);
        contentAlphaProgress = Elastos::Core::Math::Min(1.0f, contentAlphaProgress);
        ITimeInterpolator::Probe(mCurrentAlphaInterpolator)->GetInterpolation(contentAlphaProgress, &contentAlphaProgress);

        AutoPtr<IColor> color;
        CColor::AcquireSingleton((IColor**)&color);
        Int32 red = 0, green = 0, blue = 0;
        color->Red(backgroundColor, &red);
        color->Blue(backgroundColor, &blue);
        color->Green(backgroundColor, &green);
        Int32 sourceColor = 0;
        color->Argb((Int32) (255 * (1.0f - contentAlphaProgress)), red, green, blue, &sourceColor);
        mAppearAnimationFilter->SetColor(sourceColor);
        mAppearPaint->SetColorFilter(IColorFilter::Probe(mAppearAnimationFilter));
    }
}

Int32 ActivatableNotificationView::GetBackgroundColor()
{
    if (mBgTint != 0) {
        return mBgTint;
    }
    else if (mShowingLegacyBackground) {
        return mLegacyColor;
    }
    else if (mIsBelowSpeedBump) {
        return mLowPriorityColor;
    }

    return mNormalColor;
}

Int32 ActivatableNotificationView::GetRippleColor()
{
    if (mBgTint != 0) {
        return mTintedRippleColor;
    }
    else if (mShowingLegacyBackground) {
        return mTintedRippleColor;
    }
    else if (mIsBelowSpeedBump) {
        return mLowPriorityRippleColor;
    }
    return mNormalRippleColor;
}

void ActivatableNotificationView::EnableAppearDrawing(
    /* [in] */ Boolean enable)
{
    if (enable != mDrawingAppearAnimation) {
        if (enable) {
            Int32 width = 0, height = 0;
            if ((GetWidth(&width), width) == 0 || (GetActualHeight(&height), height) == 0) {
                // TODO: This should not happen, but it can during expansion. Needs
                // investigation
                return;
            }
            GetActualHeight(&height);
            AutoPtr<IBitmapHelper> helper;
            CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
            AutoPtr<IBitmap> bitmap;
            helper->CreateBitmap(width, height, BitmapConfig_ARGB_8888, (IBitmap**)&bitmap);
            AutoPtr<ICanvas> canvas;
            CCanvas::New(bitmap, (ICanvas**)&canvas);
            Draw(canvas);
            AutoPtr<IShader> shader;
            CBitmapShader::New(bitmap, ShaderTileMode_CLAMP,
                    ShaderTileMode_CLAMP, (IShader**)&shader);
            mAppearPaint->SetShader(shader);
        }
        else {
            mAppearPaint->SetShader(NULL);
        }
        mDrawingAppearAnimation = enable;
        Invalidate();
    }
}

ECode ActivatableNotificationView::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    if (!mDrawingAppearAnimation) {
        ExpandableOutlineView::DispatchDraw(canvas);
    }
    else {
        DrawAppearRect(canvas);
    }
    return NOERROR;
}

void ActivatableNotificationView::DrawAppearRect(
    /* [in] */ ICanvas* canvas)
{
    Int32 count = 0;
    canvas->Save(&count);
    canvas->Translate(0, mAppearAnimationTranslation);
    canvas->DrawRoundRect(mAppearAnimationRect, mRoundedRectCornerRadius,
            mRoundedRectCornerRadius, mAppearPaint);
    canvas->Restore();
}

ECode ActivatableNotificationView::SetOnActivatedListener(
    /* [in] */ IActivatableNotificationViewOnActivatedListener* onActivatedListener)
{
    mOnActivatedListener = onActivatedListener;
    return NOERROR;
}

ECode ActivatableNotificationView::Reset()
{
    SetTintColor(0);
    SetShowingLegacyBackground(FALSE);
    SetBelowSpeedBump(FALSE);
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
