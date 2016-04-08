#include "elastos/droid/systemui/SearchPanelCircleView.h"
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBar.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::SystemUI::StatusBar::Phone::CPhoneStatusBar;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::R;
using Elastos::Core::IFloat;
using Elastos::Core::Math;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {

//========================================================
// SearchPanelCircleView::Ripple::MyAnimatorUpdateListener
//========================================================

CAR_INTERFACE_IMPL(SearchPanelCircleView::Ripple::MyAnimatorUpdateListener, Object, IAnimatorUpdateListener)

SearchPanelCircleView::Ripple::MyAnimatorUpdateListener::MyAnimatorUpdateListener(
    /* [in] */ Ripple* host)
    : mHost(host)
{}

ECode SearchPanelCircleView::Ripple::MyAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    Float f1;
    animation->GetAnimatedFraction(&f1);
    mHost->mAlpha = 1.0f - f1;
    ITimeInterpolator::Probe(mHost->mHost->mDisappearInterpolator)->GetInterpolation(mHost->mAlpha, &(mHost->mAlpha));
    ITimeInterpolator::Probe(mHost->mHost->mAppearInterpolator)->GetInterpolation(f1, &(mHost->mRadius));
    mHost->mRadius *= mHost->mEndRadius;
    mHost->mHost->Invalidate();
    return NOERROR;
}

//========================================================
// SearchPanelCircleView::Ripple::MyAnimatorListenerAdapter
//========================================================

SearchPanelCircleView::Ripple::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ Ripple* host)
    : mHost(host)
{}

ECode SearchPanelCircleView::Ripple::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mHost->mRipples->Remove((IAnimatorListener*)this);
    mHost->mHost->UpdateClipping();
    return NOERROR;
}

ECode SearchPanelCircleView::Ripple::MyAnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mHost->mHost->mRipples->Add((IAnimatorListener*)this);
    mHost->mHost->UpdateClipping();
    return NOERROR;
}

//========================================================
// SearchPanelCircleView::Ripple
//========================================================

SearchPanelCircleView::Ripple::Ripple(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float endRadius,
    /* [in] */ SearchPanelCircleView* host)
    : mX(x)
    , mY(y)
    , mEndRadius(endRadius)
    , mHost(host)
{
}

void SearchPanelCircleView::Ripple::Start()
{
    AutoPtr<IValueAnimatorHelper> vah;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&vah);
    AutoPtr<ArrayOf<Float> > fs = ArrayOf<Float>::Alloc(2);
    (*fs)[0] = 0.0f;
    (*fs)[1] = 1.0f;
    AutoPtr<IValueAnimator> animator;
    vah->OfFloat(fs, (IValueAnimator**)&animator);

    // Linear since we are animating multiple values
    AutoPtr<ITimeInterpolator> ti;
    CLinearInterpolator::New((ITimeInterpolator**)&ti);
    IAnimator::Probe(animator)->SetInterpolator(ti);

    AutoPtr<MyAnimatorUpdateListener> aul = new MyAnimatorUpdateListener(this);
    animator->AddUpdateListener((IAnimatorUpdateListener*)aul);
    AutoPtr<MyAnimatorListenerAdapter> ala = new MyAnimatorListenerAdapter(this);
    IAnimator::Probe(animator)->AddListener((IAnimatorListener*)ala);
    animator->SetDuration(400);
    IAnimator::Probe(animator)->Start();
}

void SearchPanelCircleView::Ripple::Draw(
    /* [in] */ ICanvas* canvas)
{
    mHost->mRipplePaint->SetAlpha((Int32) (mAlpha * 255));
    canvas->DrawCircle(mX, mY, mRadius, mHost->mRipplePaint);
}

//========================================================
// SearchPanelCircleView::CircleUpdateListener
//========================================================

CAR_INTERFACE_IMPL(SearchPanelCircleView::CircleUpdateListener, Object, IAnimatorUpdateListener)

SearchPanelCircleView::CircleUpdateListener::CircleUpdateListener(
    /* [in] */ SearchPanelCircleView* host)
    : mHost(host)
{
}

ECode SearchPanelCircleView::CircleUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> av;
    animation->GetAnimatedValue((IInterface**)&av);
    AutoPtr<IFloat> f = IFloat::Probe(av);
    Float v;
    f->GetValue(&v);
    mHost->ApplyCircleSize(v);
    mHost->UpdateElevation();
    return NOERROR;
}

//========================================================
// SearchPanelCircleView::ClearAnimatorListener
//========================================================

SearchPanelCircleView::ClearAnimatorListener::ClearAnimatorListener(
    /* [in] */ SearchPanelCircleView* host)
    : mHost(host)
{
}

ECode SearchPanelCircleView::ClearAnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mCircleAnimator = NULL;
    return NOERROR;
}

//========================================================
// SearchPanelCircleView::OffsetUpdateListener
//========================================================

CAR_INTERFACE_IMPL(SearchPanelCircleView::OffsetUpdateListener, Object, IAnimatorUpdateListener)

SearchPanelCircleView::OffsetUpdateListener::OffsetUpdateListener(
    /* [in] */ SearchPanelCircleView* host)
    : mHost(host)
{
}

ECode SearchPanelCircleView::OffsetUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> av;
    animation->GetAnimatedValue((IInterface**)&av);
    AutoPtr<IFloat> f = IFloat::Probe(av);
    Float v;
    f->GetValue(&v);
    mHost->SetOffset(v);
    return NOERROR;
}

//========================================================
// SearchPanelCircleView::MyViewOutlineProvider
//========================================================

SearchPanelCircleView::MyViewOutlineProvider::MyViewOutlineProvider(
    /* [in] */ SearchPanelCircleView* host)
    : mHost(host)
{}

ECode SearchPanelCircleView::MyViewOutlineProvider::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    if (mHost->mCircleSize > 0.0f) {
        outline->SetOval(mHost->mCircleRect);
    }
    else {
        outline->SetEmpty();
    }
    outline->SetAlpha(mHost->mOutlineAlpha);
    return NOERROR;
}

//========================================================
// SearchPanelCircleView::MyAnimatorListenerAdapter2
//========================================================

SearchPanelCircleView::MyAnimatorListenerAdapter2::MyAnimatorListenerAdapter2(
    /* [in] */ SearchPanelCircleView* host,
    /* [in] */ IRunnable* endRunnable)
    : mHost(host)
    , mEndRunnable(endRunnable)
{}

ECode SearchPanelCircleView::MyAnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mEndRunnable != NULL) {
        mEndRunnable->Run();
    }
    return NOERROR;
}

//========================================================
// SearchPanelCircleView::MyAnimatorListenerAdapter3
//========================================================

SearchPanelCircleView::MyAnimatorListenerAdapter3::MyAnimatorListenerAdapter3(
    /* [in] */ SearchPanelCircleView* host,
    /* [in] */ IRunnable* endRunnable)
    : mHost(host)
    , mEndRunnable(endRunnable)
{}

ECode SearchPanelCircleView::MyAnimatorListenerAdapter3::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mOffsetAnimator = NULL;
    if (mEndRunnable != NULL) {
        mEndRunnable->Run();
    }
    return NOERROR;
}


//========================================================
// SearchPanelCircleView::MyAnimatorListenerAdapter4
//========================================================

SearchPanelCircleView::MyAnimatorListenerAdapter4::MyAnimatorListenerAdapter4(
    /* [in] */ SearchPanelCircleView* host,
    /* [in] */ IRunnable* runnable)
    : mHost(host)
    , mRunnable(runnable)
{}

ECode SearchPanelCircleView::MyAnimatorListenerAdapter4::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mRunnable != NULL) {
        mRunnable->Run();
    }
    return NOERROR;
}

//========================================================
// SearchPanelCircleView::MyRunnable
//========================================================

SearchPanelCircleView::MyRunnable::MyRunnable(
    /* [in] */ SearchPanelCircleView* host)
    : mHost(host)
{}

ECode SearchPanelCircleView::MyRunnable::Run()
{
    mHost->AddRipple();
    return NOERROR;
}

//========================================================
// SearchPanelCircleView::MyAnimatorUpdateListener2
//========================================================

CAR_INTERFACE_IMPL(SearchPanelCircleView::MyAnimatorUpdateListener2, Object, IAnimatorUpdateListener)

SearchPanelCircleView::MyAnimatorUpdateListener2::MyAnimatorUpdateListener2(
    /* [in] */ SearchPanelCircleView* host)
    : mHost(host)
{}

ECode SearchPanelCircleView::MyAnimatorUpdateListener2::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    Float animatedFraction;
    animation->GetAnimatedFraction(&animatedFraction);
    Float logoValue = animatedFraction > 0.5f ? 1.0f : animatedFraction / 0.5f;
    ITimeInterpolator::Probe(CPhoneStatusBar::ALPHA_OUT)->GetInterpolation(1.0f - logoValue, &logoValue);
    Float backgroundValue;
    ITimeInterpolator::Probe(CPhoneStatusBar::ALPHA_OUT)->GetInterpolation(
        (animatedFraction - 0.2f) / 0.8f, &backgroundValue);
    backgroundValue = animatedFraction < 0.2f ? 0.0f : backgroundValue;
    backgroundValue = 1.0f - backgroundValue;
    mHost->mBackgroundPaint->SetAlpha((Int32) (backgroundValue * 255));
    mHost->mOutlineAlpha = backgroundValue;
    IView::Probe(mHost->mLogo)->SetAlpha(logoValue);
    mHost->InvalidateOutline();
    mHost->Invalidate();
    return NOERROR;
}

//========================================================
// SearchPanelCircleView::MyAnimatorListenerAdapter5
//========================================================

SearchPanelCircleView::MyAnimatorListenerAdapter5::MyAnimatorListenerAdapter5(
    /* [in] */ SearchPanelCircleView* host,
    /* [in] */ IRunnable* endRunnable)
    : mHost(host)
    , mEndRunnable(endRunnable)
{}

ECode SearchPanelCircleView::MyAnimatorListenerAdapter5::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mEndRunnable != NULL) {
        mEndRunnable->Run();
    }
    IView::Probe(mHost->mLogo)->SetAlpha(1.0f);
    mHost->mBackgroundPaint->SetAlpha(255);
    mHost->mOutlineAlpha = 1.0f;
    mHost->mFadeOutAnimator = NULL;
    return NOERROR;
}

//========================================================
// SearchPanelCircleView
//========================================================

CAR_INTERFACE_IMPL(SearchPanelCircleView, FrameLayout, ISearchPanelCircleView)

SearchPanelCircleView::SearchPanelCircleView(
    /* [in] */ IContext* context)
{
    SearchPanelCircleView(context, NULL);
}

SearchPanelCircleView::SearchPanelCircleView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    SearchPanelCircleView(context, attrs, 0);
}

SearchPanelCircleView::SearchPanelCircleView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    SearchPanelCircleView(context, attrs, defStyleAttr, 0);
}

SearchPanelCircleView::SearchPanelCircleView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
    : mClipToOutline(FALSE)
    , mAnimatingOut(FALSE)
    , mOutlineAlpha(0.0f)
    , mOffset(0.0f)
    , mCircleSize(0.0f)
    , mHorizontal(FALSE)
    , mCircleHidden(FALSE)
    , mDraggedFarEnough(FALSE)
    , mOffsetAnimatingIn(FALSE)
    , mCircleAnimationEndValue(0.0f)
{
    CPaint::New((IPaint**)&mBackgroundPaint);
    CPaint::New((IPaint**)&mRipplePaint);
    CRect::New((IRect**)&mCircleRect);
    CRect::New((IRect**)&mStaticRect);
    CArrayList::New((IArrayList**)&mRipples);
    mCircleUpdateListener = new CircleUpdateListener(this);
    mClearAnimatorListener = new ClearAnimatorListener(this);
    mOffsetUpdateListener = new OffsetUpdateListener(this);

    FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes);
    AutoPtr<MyViewOutlineProvider> vop = new MyViewOutlineProvider(this);
    SetOutlineProvider(vop);
    SetWillNotDraw(FALSE);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::search_panel_circle_size, &mCircleMinSize);
    res->GetDimensionPixelSize(R::dimen::search_panel_circle_base_margin, &mBaseMargin);
    res->GetDimensionPixelSize(R::dimen::search_panel_circle_travel_distance, &mStaticOffset);
    res->GetDimensionPixelSize(R::dimen::search_panel_circle_elevation, &mMaxElevation);
    AutoPtr<IAnimationUtils> au;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&au);
    au->LoadInterpolator(mContext, Elastos::Droid::R::interpolator::linear_out_slow_in, (IInterpolator**)&mAppearInterpolator);
    au->LoadInterpolator(mContext, Elastos::Droid::R::interpolator::fast_out_slow_in, (IInterpolator**)&mFastOutSlowInInterpolator);
    au->LoadInterpolator(mContext, Elastos::Droid::R::interpolator::fast_out_linear_in, (IInterpolator**)&mDisappearInterpolator);
    mBackgroundPaint->SetAntiAlias(TRUE);
    AutoPtr<IResources> res2;
    GetResources((IResources**)&res2);
    Int32 color;
    res2->GetColor(R::color::search_panel_circle_color, &color);
    mBackgroundPaint->SetColor(color);
    res2->GetColor(R::color::search_panel_ripple_color, &color);
    mRipplePaint->SetColor(color);
    mRipplePaint->SetAntiAlias(TRUE);
}

void SearchPanelCircleView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    FrameLayout::OnDraw(canvas);
    DrawBackground(canvas);
    DrawRipples(canvas);
}

void SearchPanelCircleView::DrawRipples(
    /* [in] */ ICanvas* canvas)
{
    Int32 size;
    mRipples->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> _ripple;
        mRipples->Get(i, (IInterface**)&_ripple);
        AutoPtr<Ripple> ripple = (Ripple*)(IObject::Probe(_ripple));
        ripple->Draw(canvas);
    }
}

void SearchPanelCircleView::DrawBackground(
    /* [in] */ ICanvas* canvas)
{
    Int32 centerX, centerY;
    mCircleRect->GetCenterX(&centerX);
    mCircleRect->GetCenterY(&centerY);
    canvas->DrawCircle(centerX, centerY, mCircleSize / 2, mBackgroundPaint);
}

ECode SearchPanelCircleView::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();
    AutoPtr<IView> view;
    FindViewById(R::id::search_logo, (IView**)&view);
    mLogo = IImageView::Probe(view);
    return NOERROR;
}

ECode SearchPanelCircleView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Int32 w, h;
    IView::Probe(mLogo)->GetMeasuredWidth(&w);
    IView::Probe(mLogo)->GetMeasuredHeight(&h);
    IView::Probe(mLogo)->Layout(0, 0, w, h);
    if (changed) {
        UpdateCircleRect(mStaticRect, mStaticOffset, TRUE);
    }
    return NOERROR;
}

ECode SearchPanelCircleView::SetCircleSize(
    /* [in] */ Float circleSize)
{
    return SetCircleSize(circleSize, FALSE, NULL, 0, NULL);
}

ECode SearchPanelCircleView::SetCircleSize(
    /* [in] */ Float circleSize,
    /* [in] */ Boolean animated,
    /* [in] */ IRunnable* endRunnable,
    /* [in] */ Int32 startDelay,
    /* [in] */ IInterpolator* interpolator)
{
    Boolean isAnimating = mCircleAnimator != NULL;
    Boolean isRunning;
    IAnimator::Probe(mCircleAnimator)->IsRunning(&isRunning);
    Boolean animationPending = isAnimating && !isRunning;
    Boolean animatingOut = isAnimating && mCircleAnimationEndValue == 0;
    if (animated || animationPending || animatingOut) {
        if (isAnimating) {
            if (circleSize == mCircleAnimationEndValue) {
                return E_NULL_POINTER_EXCEPTION;
            }
            IAnimator::Probe(mCircleAnimator)->Cancel();
        }
        AutoPtr<IValueAnimatorHelper> vah;
        CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&vah);
        AutoPtr<ArrayOf<Float> > fs = ArrayOf<Float>::Alloc(2);
        (*fs)[0] = mCircleSize;
        (*fs)[1] = circleSize;
        vah->OfFloat(fs, (IValueAnimator**)&mCircleAnimator);

        mCircleAnimator->AddUpdateListener(mCircleUpdateListener);
        IAnimator::Probe(mCircleAnimator)->AddListener(mClearAnimatorListener);
        AutoPtr<MyAnimatorListenerAdapter2> ala = new MyAnimatorListenerAdapter2(this, endRunnable);
        IAnimator::Probe(mCircleAnimator)->AddListener(ala);
        AutoPtr<IInterpolator> desiredInterpolator = interpolator != NULL ? interpolator
                : circleSize == 0 ? mDisappearInterpolator.Get() : mAppearInterpolator.Get();
        IAnimator::Probe(mCircleAnimator)->SetInterpolator(ITimeInterpolator::Probe(desiredInterpolator));
        mCircleAnimator->SetDuration(300);
        IAnimator::Probe(mCircleAnimator)->SetStartDelay(startDelay);
        IAnimator::Probe(mCircleAnimator)->Start();
        mCircleAnimationEndValue = circleSize;
    }
    else {
        if (isAnimating) {
            Float diff = circleSize - mCircleAnimationEndValue;
            AutoPtr<ArrayOf<IPropertyValuesHolder*> > values;
            mCircleAnimator->GetValues((ArrayOf<IPropertyValuesHolder*>**)&values);
            AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
            (*fvs)[0] = diff;
            (*fvs)[1] = circleSize;
            (*values)[0]->SetFloatValues(fvs);
            Int64 currentPlayTime;
            mCircleAnimator->GetCurrentPlayTime(&currentPlayTime);
            mCircleAnimator->SetCurrentPlayTime(currentPlayTime);
            mCircleAnimationEndValue = circleSize;
        }
        else {
            ApplyCircleSize(circleSize);
            UpdateElevation();
        }
    }
    return NOERROR;
}

void SearchPanelCircleView::ApplyCircleSize(
    /* [in] */ Float circleSize)
{
    mCircleSize = circleSize;
    UpdateLayout();
}

void SearchPanelCircleView::UpdateElevation()
{
    Float t = (mStaticOffset - mOffset) / (Float) mStaticOffset;
    t = 1.0f - Elastos::Core::Math::Max(t, 0.0f);
    Float offset = t * mMaxElevation;
    SetElevation(offset);
}

ECode SearchPanelCircleView::SetOffset(
    /* [in] */ Float offset)
{
    SetOffset(offset, FALSE, 0, NULL, NULL);
    return NOERROR;
}

void SearchPanelCircleView::SetOffset(
    /* [in] */ Float offset,
    /* [in] */ Boolean animate,
    /* [in] */ Int32 startDelay,
    /* [in] */ IInterpolator* interpolator,
    /* [in] */ IRunnable* endRunnable)
{
    if (!animate) {
        mOffset = offset;
        UpdateLayout();
        if (endRunnable != NULL) {
            endRunnable->Run();
        }
    }
    else {
        if (mOffsetAnimator != NULL) {
            IAnimator::Probe(mOffsetAnimator)->RemoveAllListeners();
            IAnimator::Probe(mOffsetAnimator)->Cancel();
        }

        AutoPtr<IValueAnimatorHelper> vah;
        CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&vah);
        AutoPtr<ArrayOf<Float> > fs = ArrayOf<Float>::Alloc(2);
        (*fs)[0] = mOffset;
        (*fs)[1] = offset;
        vah->OfFloat(fs, (IValueAnimator**)&mOffsetAnimator);

        mOffsetAnimator->AddUpdateListener(mOffsetUpdateListener);
        AutoPtr<MyAnimatorListenerAdapter3> ala = new MyAnimatorListenerAdapter3(this, endRunnable);
        IAnimator::Probe(mOffsetAnimator)->AddListener(ala);

        AutoPtr<IInterpolator> desiredInterpolator = interpolator != NULL ? interpolator
                : offset == 0 ? mDisappearInterpolator.Get() : mAppearInterpolator.Get();
        IAnimator::Probe(mOffsetAnimator)->SetInterpolator(ITimeInterpolator::Probe(desiredInterpolator));
        IAnimator::Probe(mOffsetAnimator)->SetStartDelay(startDelay);
        mOffsetAnimator->SetDuration(300);
        IAnimator::Probe(mOffsetAnimator)->Start();
        mOffsetAnimatingIn = offset != 0;
    }
}

void SearchPanelCircleView::UpdateLayout()
{
    UpdateCircleRect();
    UpdateLogo();
    InvalidateOutline();
    Invalidate();
    UpdateClipping();
}

void SearchPanelCircleView::UpdateClipping()
{
    Boolean isEmpty;
    mRipples->IsEmpty(&isEmpty);
    Boolean clip = mCircleSize < mCircleMinSize || !isEmpty;
    if (clip != mClipToOutline) {
        SetClipToOutline(clip);
        mClipToOutline = clip;
    }
}

void SearchPanelCircleView::UpdateLogo()
{
    Boolean exitAnimationRunning = mFadeOutAnimator != NULL;
    AutoPtr<IRect> rect = exitAnimationRunning ? mCircleRect : mStaticRect;
    Int32 left, right, top, bottom, width, height;
    rect->GetLeft(&left);
    rect->GetRight(&right);
    rect->GetTop(&top);
    rect->GetBottom(&bottom);
    IView::Probe(mLogo)->GetWidth(&width);
    IView::Probe(mLogo)->GetHeight(&height);
    Float translationX = (left + right) / 2.0f - width / 2.0f;
    Float translationY = (top + bottom) / 2.0f - height / 2.0f;
    Float t = (mStaticOffset - mOffset) / (Float) mStaticOffset;
    if (!exitAnimationRunning) {
        if (mHorizontal) {
            translationX += t * mStaticOffset * 0.3f;
        } else {
            translationY += t * mStaticOffset * 0.3f;
        }
        Float alpha = 1.0f-t;
        alpha = Elastos::Core::Math::Max((alpha - 0.5f) * 2.0f, 0.0f);
        mLogo->SetAlpha(alpha);
    }
    else {
        translationY += (mOffset - mStaticOffset) / 2;
    }
    IView::Probe(mLogo)->SetTranslationX(translationX);
    IView::Probe(mLogo)->SetTranslationY(translationY);
}

void SearchPanelCircleView::UpdateCircleRect()
{
    UpdateCircleRect(mCircleRect, mOffset, FALSE);
}

void SearchPanelCircleView::UpdateCircleRect(
    /* [in] */ IRect* rect,
    /* [in] */ Float offset,
    /* [in] */ Boolean useStaticSize)
{
    Int32 left, top;
    Float circleSize = useStaticSize ? mCircleMinSize : mCircleSize;
    Int32 width, height;
    GetWidth(&width);
    GetHeight(&height);
    if (mHorizontal) {
        left = (Int32) (width - circleSize / 2 - mBaseMargin - offset);
        top = (Int32) ((height - circleSize) / 2);
    }
    else {
        left = (Int32) (width - circleSize) / 2;
        top = (Int32) (height - circleSize / 2 - mBaseMargin - offset);
    }
    rect->Set(left, top, (Int32) (left + circleSize), (Int32) (top + circleSize));
}

ECode SearchPanelCircleView::SetHorizontal(
    /* [in] */ Boolean horizontal)
{
    mHorizontal = horizontal;
    UpdateCircleRect(mStaticRect, mStaticOffset, TRUE);
    UpdateLayout();
    return NOERROR;
}

ECode SearchPanelCircleView::SetDragDistance(
    /* [in] */ Float distance)
{
    if (!mAnimatingOut && (!mCircleHidden || mDraggedFarEnough)) {
        Float circleSize = mCircleMinSize + Rubberband(distance);
        SetCircleSize(circleSize);
    }
    return NOERROR;
}

Float SearchPanelCircleView::Rubberband(
    /* [in] */ Float diff)
{
    return (Float) Elastos::Core::Math::Pow(Elastos::Core::Math::Abs(diff), 0.6f);
}

ECode SearchPanelCircleView::StartAbortAnimation(
    /* [in] */ IRunnable* endRunnable)
{
    if (mAnimatingOut) {
        if (endRunnable != NULL) {
            endRunnable->Run();
        }
        return E_NULL_POINTER_EXCEPTION;
    }
    SetCircleSize(0, TRUE, NULL, 0, NULL);
    SetOffset(0, TRUE, 0, NULL, endRunnable);
    mCircleHidden = TRUE;
    return NOERROR;
}

ECode SearchPanelCircleView::StartEnterAnimation()
{
    if (mAnimatingOut) {
        return E_NULL_POINTER_EXCEPTION;
    }
    ApplyCircleSize(0);
    SetOffset(0);
    SetCircleSize(mCircleMinSize, TRUE, NULL, 50, NULL);
    SetOffset(mStaticOffset, TRUE, 50, NULL, NULL);
    mCircleHidden = FALSE;
    return NOERROR;
}

ECode SearchPanelCircleView::StartExitAnimation(
    /* [in] */ IRunnable* endRunnable)
{
    if (!mHorizontal) {
        Int32 width, height;
        GetWidth(&width);
        GetHeight(&height);
        Float offset = height / 2.0f;
        SetOffset(offset - mBaseMargin, TRUE, 50, mFastOutSlowInInterpolator, NULL);
        Float xMax = width / 2;
        Float yMax = height / 2;
        Float maxRadius = (Float) Elastos::Core::Math::Ceil(Elastos::Core::Math::Hypot(xMax, yMax) * 2);
        SetCircleSize(maxRadius, TRUE, NULL, 50, mFastOutSlowInInterpolator);
        PerformExitFadeOutAnimation(50, 300, endRunnable);
    }
    else {

        // when in landscape, we don't wan't the animation as it interferes with the general
        // rotation animation to the homescreen.
        endRunnable->Run();
    }
    return NOERROR;
}

void SearchPanelCircleView::PerformExitFadeOutAnimation(
    /* [in] */ Int32 startDelay,
    /* [in] */ Int32 duration,
   /* [in] */ IRunnable* endRunnable)
{
    AutoPtr<IValueAnimatorHelper> vah;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&vah);
    AutoPtr<ArrayOf<Float> > fs = ArrayOf<Float>::Alloc(2);
    Int32 alpha;
    mBackgroundPaint->GetAlpha(&alpha);
    (*fs)[0] = alpha / 255.0f;
    (*fs)[1] = 0.0f;
    AutoPtr<IValueAnimator> animator;
    vah->OfFloat(fs, (IValueAnimator**)&mFadeOutAnimator);

    // Linear since we are animating multiple values
    AutoPtr<ITimeInterpolator> ti;
    CLinearInterpolator::New((ITimeInterpolator**)&ti);
    IAnimator::Probe(mFadeOutAnimator)->SetInterpolator(ti);
    AutoPtr<MyAnimatorUpdateListener2> aul = new MyAnimatorUpdateListener2(this);
    mFadeOutAnimator->AddUpdateListener(aul);
    AutoPtr<MyAnimatorListenerAdapter5> ala = new MyAnimatorListenerAdapter5(this, endRunnable);
    IAnimator::Probe(mFadeOutAnimator)->AddListener(ala);
    IAnimator::Probe(mFadeOutAnimator)->SetStartDelay(startDelay);
    mFadeOutAnimator->SetDuration(duration);
    IAnimator::Probe(mFadeOutAnimator)->Start();
}

ECode SearchPanelCircleView::SetDraggedFarEnough(
    /* [in] */ Boolean farEnough)
{
    if (farEnough != mDraggedFarEnough) {
        if (farEnough) {
            if (mCircleHidden) {
                StartEnterAnimation();
            }
            if (mOffsetAnimator == NULL) {
                AddRipple();
            }
            else {
                AutoPtr<MyRunnable> mr = new MyRunnable(this);
                Boolean isPost;
                PostDelayed(mr, 100, &isPost);
            }
        }
        else {
            StartAbortAnimation(NULL);
        }
        mDraggedFarEnough = farEnough;
    }
    return NOERROR;
}

void SearchPanelCircleView::AddRipple()
{
    Int32 size;
    mRipples->GetSize(&size);
    if (size > 1) {
        // we only want 2 ripples at the time
        return;
    }
    Float xInterpolation, yInterpolation;
    if (mHorizontal) {
        xInterpolation = 0.75f;
        yInterpolation = 0.5f;
    }
    else {
        xInterpolation = 0.5f;
        yInterpolation = 0.75f;
    }
    Int32 left, right, top, bottom;
    mStaticRect->GetLeft(&left);
    mStaticRect->GetRight(&right);
    mStaticRect->GetTop(&top);
    mStaticRect->GetBottom(&bottom);
    Float circleCenterX = left * (1.0f - xInterpolation) + right * xInterpolation;
    Float circleCenterY = top * (1.0f - yInterpolation) + bottom * yInterpolation;
    Float radius = Elastos::Core::Math::Max(mCircleSize, mCircleMinSize * 1.25f) * 0.75f;
    AutoPtr<Ripple> ripple = new Ripple(circleCenterX, circleCenterY, radius, this);
    ripple->Start();
}

ECode SearchPanelCircleView::Reset()
{
    mDraggedFarEnough = FALSE;
    mAnimatingOut = FALSE;
    mCircleHidden = TRUE;
    mClipToOutline = FALSE;
    if (mFadeOutAnimator != NULL) {
        IAnimator::Probe(mFadeOutAnimator)->Cancel();
    }
    mBackgroundPaint->SetAlpha(255);
    mOutlineAlpha = 1.0f;
    return NOERROR;
}

ECode SearchPanelCircleView::IsAnimationRunning(
    /* [in] */ Boolean enterAnimation,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mOffsetAnimator != NULL && (enterAnimation == mOffsetAnimatingIn);
    return NOERROR;
}

ECode SearchPanelCircleView::PerformOnAnimationFinished(
    /* [in] */ IRunnable* runnable)
{
    if (mOffsetAnimator != NULL) {
        AutoPtr<MyAnimatorListenerAdapter4> ala = new MyAnimatorListenerAdapter4(this, runnable);
        IAnimator::Probe(mOffsetAnimator)->AddListener(ala);
    }
    else {
        if (runnable != NULL) {
            runnable->Run();
        }
    }
    return NOERROR;
}

ECode SearchPanelCircleView::SetAnimatingOut(
    /* [in] */ Boolean animatingOut)
{
    mAnimatingOut = animatingOut;
    return NOERROR;
}

ECode SearchPanelCircleView::IsAnimatingOut(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAnimatingOut;
    return NOERROR;
}

ECode SearchPanelCircleView::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // not really true but it's ok during an animation, as it's never permanent
    *result = FALSE;
    return NOERROR;
}

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos