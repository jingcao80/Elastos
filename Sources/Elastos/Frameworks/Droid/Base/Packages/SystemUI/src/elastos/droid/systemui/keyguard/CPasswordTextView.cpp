//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/systemui/keyguard/CPasswordTextView.h"
#include "elastos/droid/os/SystemClock.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::Graphics::ITypefaceHelper;
using Elastos::Droid::Graphics::CTypefaceHelper;
using Elastos::Droid::Graphics::PaintAlign_CENTER;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Core::IFloat;
using Elastos::Core::Math;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CStack;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CPasswordTextView::CharState::MyAnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mCancelled = TRUE;
    return NOERROR;
}

ECode CPasswordTextView::CharState::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (!mCancelled) {
        AutoPtr<CharState> inHost = mInHost; // hold mInHost's reference
        mOutHost->mTextChars->Remove(TO_IINTERFACE(mInHost));
        mOutHost->mCharPool->Push(TO_IINTERFACE(mInHost));
        mInHost->Reset();
        mInHost->CancelAnimator(IAnimator::Probe(mInHost->mTextTranslateAnimator));
        mInHost->mTextTranslateAnimator = NULL;
    }
    return NOERROR;
}

ECode CPasswordTextView::CharState::MyAnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mCancelled = FALSE;
    return NOERROR;
}

ECode CPasswordTextView::CharState::MyAnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mDotAnimator = NULL;
    return NOERROR;
}

ECode CPasswordTextView::CharState::MyAnimatorListenerAdapter3::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mTextAnimator = NULL;
    return NOERROR;
}

ECode CPasswordTextView::CharState::MyAnimatorListenerAdapter4::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mTextTranslateAnimator = NULL;
    return NOERROR;
}

ECode CPasswordTextView::CharState::MyAnimatorListenerAdapter5::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mWidthAnimator = NULL;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CPasswordTextView::CharState::MyAnimatorUpdateListener, Object,
        IAnimatorUpdateListener)

ECode CPasswordTextView::CharState::MyAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    AutoPtr<IFloat> f = IFloat::Probe(value);
    f->GetValue(&(mInHost->mCurrentDotSizeFactor));
    return mOutHost->Invalidate();
}

CAR_INTERFACE_IMPL(CPasswordTextView::CharState::MyAnimatorUpdateListener2, Object,
        IAnimatorUpdateListener)

ECode CPasswordTextView::CharState::MyAnimatorUpdateListener2::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    AutoPtr<IFloat> f = IFloat::Probe(value);
    f->GetValue(&(mInHost->mCurrentTextSizeFactor));
    return mOutHost->Invalidate();
}

CAR_INTERFACE_IMPL(CPasswordTextView::CharState::MyAnimatorUpdateListener3, Object,
        IAnimatorUpdateListener)

ECode CPasswordTextView::CharState::MyAnimatorUpdateListener3::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    AutoPtr<IFloat> f = IFloat::Probe(value);
    f->GetValue(&(mInHost->mCurrentTextTranslationY));
    return mOutHost->Invalidate();
}

CAR_INTERFACE_IMPL(CPasswordTextView::CharState::MyAnimatorUpdateListener4, Object,
        IAnimatorUpdateListener)

ECode CPasswordTextView::CharState::MyAnimatorUpdateListener4::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    AutoPtr<IFloat> f = IFloat::Probe(value);
    f->GetValue(&(mInHost->mCurrentWidthFactor));
    return mOutHost->Invalidate();
}

ECode CPasswordTextView::CharState::MyRunnable::Run()
{
    mHost->PerformSwap();
    mHost->mIsDotSwapPending = FALSE;
    return NOERROR;
}

CPasswordTextView::CharState::CharState(
    /* [in] */ CPasswordTextView* host)
    : mWhichChar(0)
    , mTextAnimationIsGrowing(FALSE)
    , mDotAnimationIsGrowing(FALSE)
    , mWidthAnimationIsGrowing(FALSE)
    , mCurrentTextSizeFactor(0.0f)
    , mCurrentDotSizeFactor(0.0f)
    , mCurrentWidthFactor(0.0f)
    , mIsDotSwapPending(FALSE)
    , mCurrentTextTranslationY(1.0f)
    , mHost(host)
{
    mRemoveEndListener = new MyAnimatorListenerAdapter(this, mHost);
    mDotFinishListener = new MyAnimatorListenerAdapter2(this);
    mTextFinishListener = new MyAnimatorListenerAdapter3(this);
    mTextTranslateFinishListener = new MyAnimatorListenerAdapter4(this);
    mWidthFinishListener = new MyAnimatorListenerAdapter5(this);

    mDotSizeUpdater = new MyAnimatorUpdateListener(this, mHost);
    mTextSizeUpdater = new MyAnimatorUpdateListener2(this, mHost);
    mTextTranslationUpdater = new MyAnimatorUpdateListener3(this, mHost);
    mWidthUpdater = new MyAnimatorUpdateListener4(this, mHost);

    mDotSwapperRunnable = new MyRunnable(this);
}

ECode CPasswordTextView::CharState::Reset()
{
    mWhichChar = 0;
    mCurrentTextSizeFactor = 0.0f;
    mCurrentDotSizeFactor = 0.0f;
    mCurrentWidthFactor = 0.0f;
    CancelAnimator(IAnimator::Probe(mTextAnimator));
    mTextAnimator = NULL;
    CancelAnimator(mDotAnimator);
    mDotAnimator = NULL;
    CancelAnimator(IAnimator::Probe(mWidthAnimator));
    mWidthAnimator = NULL;
    mCurrentTextTranslationY = 1.0f;
    RemoveDotSwapCallbacks();
    return NOERROR;
}

ECode CPasswordTextView::CharState::StartRemoveAnimation(
    /* [in] */ Int64 startDelay,
    /* [in] */ Int64 widthDelay)
{
    Boolean dotNeedsAnimation = (mCurrentDotSizeFactor > 0.0f && mDotAnimator == NULL)
            || (mDotAnimator != NULL && mDotAnimationIsGrowing);
    Boolean textNeedsAnimation = (mCurrentTextSizeFactor > 0.0f && mTextAnimator == NULL)
            || (mTextAnimator != NULL && mTextAnimationIsGrowing);
    Boolean widthNeedsAnimation = (mCurrentWidthFactor > 0.0f && mWidthAnimator == NULL)
            || (mWidthAnimator != NULL && mWidthAnimationIsGrowing);
    if (dotNeedsAnimation) {
        StartDotDisappearAnimation(startDelay);
    }
    if (textNeedsAnimation) {
        StartTextDisappearAnimation(startDelay);
    }
    if (widthNeedsAnimation) {
        StartWidthDisappearAnimation(widthDelay);
    }
    return NOERROR;
}

ECode CPasswordTextView::CharState::StartAppearAnimation()
{
    Boolean dotNeedsAnimation = !mHost->mShowPassword
            && (mDotAnimator == NULL || !mDotAnimationIsGrowing);
    Boolean textNeedsAnimation = mHost->mShowPassword
            && (mTextAnimator == NULL || !mTextAnimationIsGrowing);
    Boolean widthNeedsAnimation = (mWidthAnimator == NULL || !mWidthAnimationIsGrowing);
    if (dotNeedsAnimation) {
        StartDotAppearAnimation(0);
    }
    if (textNeedsAnimation) {
        StartTextAppearAnimation();
    }
    if (widthNeedsAnimation) {
        StartWidthAppearAnimation();
    }
    if (mHost->mShowPassword) {
        PostDotSwap(TEXT_VISIBILITY_DURATION);
    }
    return NOERROR;
}

void CPasswordTextView::CharState::PostDotSwap(
    /* [in] */ Int64 delay)
{
    RemoveDotSwapCallbacks();
    Boolean res;
    mHost->PostDelayed(mDotSwapperRunnable, delay, &res);
    mIsDotSwapPending = TRUE;
}

void CPasswordTextView::CharState::RemoveDotSwapCallbacks()
{
    Boolean res;
    mHost->RemoveCallbacks(mDotSwapperRunnable, &res);
    mIsDotSwapPending = FALSE;
}

ECode CPasswordTextView::CharState::SwapToDotWhenAppearFinished()
{
    RemoveDotSwapCallbacks();
    if (mTextAnimator != NULL) {
        Int64 duration;
        mTextAnimator->GetDuration(&duration);
        Int64 playeTime;
        mTextAnimator->GetCurrentPlayTime(&playeTime);
        Int64 remainingDuration = duration - playeTime;
        PostDotSwap(remainingDuration + TEXT_REST_DURATION_AFTER_APPEAR);
    }
    else {
        PerformSwap();
    }
    return NOERROR;
}

void CPasswordTextView::CharState::PerformSwap()
{
    StartTextDisappearAnimation(0);
    StartDotAppearAnimation(DISAPPEAR_DURATION
            - DOT_APPEAR_TEXT_DISAPPEAR_OVERLAP_DURATION);
}

void CPasswordTextView::CharState::StartWidthDisappearAnimation(
    /* [in] */ Int64 widthDelay)
{
    CancelAnimator(IAnimator::Probe(mWidthAnimator));

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = mCurrentWidthFactor;
    (*array)[1] = 0.0f;
    helper->OfFloat(array, (IValueAnimator**)&mWidthAnimator);
    mWidthAnimator->AddUpdateListener(mWidthUpdater);
    IAnimator::Probe(mWidthAnimator)->AddListener(mWidthFinishListener);
    IAnimator::Probe(mWidthAnimator)->AddListener(mRemoveEndListener);
    mWidthAnimator->SetDuration((Int64) (DISAPPEAR_DURATION * mCurrentWidthFactor));
    IAnimator::Probe(mWidthAnimator)->SetStartDelay(widthDelay);
    IAnimator::Probe(mWidthAnimator)->Start();
    mWidthAnimationIsGrowing = FALSE;
}

void CPasswordTextView::CharState::StartTextDisappearAnimation(
    /* [in] */ Int64 startDelay)
{
    CancelAnimator(IAnimator::Probe(mTextAnimator));
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = mCurrentTextSizeFactor;
    (*array)[1] = 0.0f;
    helper->OfFloat(array, (IValueAnimator**)&mTextAnimator);
    mTextAnimator->AddUpdateListener(mTextSizeUpdater);
    IAnimator::Probe(mTextAnimator)->AddListener(mTextFinishListener);
    IAnimator::Probe(mTextAnimator)->SetInterpolator(
            ITimeInterpolator::Probe(mHost->mDisappearInterpolator));
    mTextAnimator->SetDuration((Int64) (DISAPPEAR_DURATION * mCurrentTextSizeFactor));
    IAnimator::Probe(mTextAnimator)->SetStartDelay(startDelay);
    IAnimator::Probe(mTextAnimator)->Start();
    mTextAnimationIsGrowing = FALSE;
}

void CPasswordTextView::CharState::StartDotDisappearAnimation(
    /* [in] */ Int64 startDelay)
{
    CancelAnimator(mDotAnimator);
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = mCurrentDotSizeFactor;
    (*array)[1] = 0.0f;
    AutoPtr<IValueAnimator> animator;
    helper->OfFloat(array, (IValueAnimator**)&animator);
    animator->AddUpdateListener(mDotSizeUpdater);
    IAnimator::Probe(animator)->AddListener(mDotFinishListener);
    IAnimator::Probe(animator)->SetInterpolator(
            ITimeInterpolator::Probe(mHost->mDisappearInterpolator));
    Int64 duration = (Int64) (DISAPPEAR_DURATION *
            Elastos::Core::Math::Min(mCurrentDotSizeFactor, 1.0f));
    animator->SetDuration(duration);
    IAnimator::Probe(animator)->SetStartDelay(startDelay);
    IAnimator::Probe(animator)->Start();
    mDotAnimator = IAnimator::Probe(animator);
    mDotAnimationIsGrowing = FALSE;
}

void CPasswordTextView::CharState::StartWidthAppearAnimation()
{
    CancelAnimator(IAnimator::Probe(mWidthAnimator));
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = mCurrentWidthFactor;
    (*array)[1] = 1.0f;
    helper->OfFloat(array, (IValueAnimator**)&mWidthAnimator);
    mWidthAnimator->AddUpdateListener(mWidthUpdater);
    IAnimator::Probe(mWidthAnimator)->AddListener(mWidthFinishListener);
    mWidthAnimator->SetDuration((Int64) (APPEAR_DURATION * (1.0f - mCurrentWidthFactor)));
    IAnimator::Probe(mWidthAnimator)->Start();
    mWidthAnimationIsGrowing = TRUE;
}

void CPasswordTextView::CharState::StartTextAppearAnimation()
{
    CancelAnimator(IAnimator::Probe(mTextAnimator));
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = mCurrentTextSizeFactor;
    (*array)[1] = 1.0f;
    helper->OfFloat(array, (IValueAnimator**)&mTextAnimator);
    mTextAnimator->AddUpdateListener(mTextSizeUpdater);
    IAnimator::Probe(mTextAnimator)->AddListener(mTextFinishListener);
    IAnimator::Probe(mTextAnimator)->SetInterpolator(
            ITimeInterpolator::Probe(mHost->mAppearInterpolator));
    mTextAnimator->SetDuration((Int64) (APPEAR_DURATION * (1.0f - mCurrentTextSizeFactor)));
    IAnimator::Probe(mTextAnimator)->Start();
    mTextAnimationIsGrowing = TRUE;

    // handle translation
    if (mTextTranslateAnimator == NULL) {
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
        (*array)[0] = 1.0f;
        (*array)[1] = 0.0f;
        helper->OfFloat(array, (IValueAnimator**)&mTextTranslateAnimator);
        mTextTranslateAnimator->AddUpdateListener(mTextTranslationUpdater);
        IAnimator::Probe(mTextTranslateAnimator)->AddListener(mTextTranslateFinishListener);
        IAnimator::Probe(mTextTranslateAnimator)->SetInterpolator(
                ITimeInterpolator::Probe(mHost->mAppearInterpolator));
        mTextTranslateAnimator->SetDuration(APPEAR_DURATION);
        IAnimator::Probe(mTextTranslateAnimator)->Start();
    }
}

void CPasswordTextView::CharState::StartDotAppearAnimation(
    /* [in] */ Int64 delay)
{
    CancelAnimator(IAnimator::Probe(mDotAnimator));

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    if (!mHost->mShowPassword) {
        // We perform an overshoot animation
        AutoPtr<IValueAnimator> overShootAnimator;
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
        (*array)[0] = mCurrentDotSizeFactor;
        (*array)[1] = DOT_OVERSHOOT_FACTOR;
        helper->OfFloat(array, (IValueAnimator**)&overShootAnimator);
        overShootAnimator->AddUpdateListener(mDotSizeUpdater);
        IAnimator::Probe(overShootAnimator)->SetInterpolator(
                ITimeInterpolator::Probe(mHost->mAppearInterpolator));
        Int64 overShootDuration = (Int64) (DOT_APPEAR_DURATION_OVERSHOOT
                * OVERSHOOT_TIME_POSITION);
        overShootAnimator->SetDuration(overShootDuration);

        AutoPtr<IValueAnimator> settleBackAnimator;
        AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(2);
        (*array2)[0] = DOT_OVERSHOOT_FACTOR;
        (*array2)[1] = 1.0f;
        helper->OfFloat(array2, (IValueAnimator**)&settleBackAnimator);
        settleBackAnimator->AddUpdateListener(mDotSizeUpdater);
        settleBackAnimator->SetDuration(DOT_APPEAR_DURATION_OVERSHOOT - overShootDuration);
        IAnimator::Probe(settleBackAnimator)->AddListener(mDotFinishListener);

        AutoPtr<IAnimatorSet> animatorSet;
        CAnimatorSet::New((IAnimatorSet**)&animatorSet);
        AutoPtr<ArrayOf<IAnimator*> > array3 = ArrayOf<IAnimator*>::Alloc(2);
        array3->Set(0, IAnimator::Probe(overShootAnimator));
        array3->Set(1, IAnimator::Probe(settleBackAnimator));
        animatorSet->PlaySequentially(array3);
        IAnimator::Probe(animatorSet)->SetStartDelay(delay);
        IAnimator::Probe(animatorSet)->Start();
        mDotAnimator = IAnimator::Probe(animatorSet);
    }
    else {
        AutoPtr<IValueAnimator> growAnimator;
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
        (*array)[0] = mCurrentDotSizeFactor;
        (*array)[1] = 1.0f;
        helper->OfFloat(array, (IValueAnimator**)&growAnimator);
        growAnimator->AddUpdateListener(mDotSizeUpdater);
        growAnimator->SetDuration((Int64) (APPEAR_DURATION * (1.0f - mCurrentDotSizeFactor)));
        IAnimator::Probe(growAnimator)->AddListener(mDotFinishListener);
        IAnimator::Probe(growAnimator)->SetStartDelay(delay);
        IAnimator::Probe(growAnimator)->Start();
        mDotAnimator = IAnimator::Probe(growAnimator);
    }
    mDotAnimationIsGrowing = TRUE;
}

void CPasswordTextView::CharState::CancelAnimator(
    /* [in] */ IAnimator* animator)
{
    if (animator != NULL) {
        animator->Cancel();
    }
}

ECode CPasswordTextView::CharState::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Float currentDrawPosition,
    /* [in] */ Int32 charHeight,
    /* [in] */ Float yPosition,
    /* [in] */ Float charLength,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean textVisible = mCurrentTextSizeFactor > 0;
    Boolean dotVisible = mCurrentDotSizeFactor > 0;
    Float charWidth = charLength * mCurrentWidthFactor;
    if (textVisible) {
        Float currYPosition = yPosition + charHeight / 2.0f * mCurrentTextSizeFactor
                + charHeight * mCurrentTextTranslationY * 0.8f;
        Int32 tmp;
        canvas->Save(&tmp);
        Float centerX = currentDrawPosition + charWidth / 2;
        canvas->Translate(centerX, currYPosition);
        canvas->Scale(mCurrentTextSizeFactor, mCurrentTextSizeFactor);
        String text;
        text.Append(mWhichChar);
        canvas->DrawText(text, 0, 0, mHost->mDrawPaint);
        canvas->Restore();
    }
    if (dotVisible) {
        Int32 tmp;
        canvas->Save(&tmp);
        Float centerX = currentDrawPosition + charWidth / 2;
        canvas->Translate(centerX, yPosition);
        canvas->DrawCircle(0, 0, mHost->mDotSize / 2 * mCurrentDotSizeFactor,
                mHost->mDrawPaint);
        canvas->Restore();
    }
    *result = charWidth + mHost->mCharPadding * mCurrentWidthFactor;
    return NOERROR;
}

const Float CPasswordTextView::DOT_OVERSHOOT_FACTOR = 1.5f;
const Int64 CPasswordTextView::DOT_APPEAR_DURATION_OVERSHOOT = 320;
const Int64 CPasswordTextView::APPEAR_DURATION = 160;
const Int64 CPasswordTextView::DISAPPEAR_DURATION = 160;
const Int64 CPasswordTextView::RESET_DELAY_PER_ELEMENT = 40;
const Int64 CPasswordTextView::RESET_MAX_DELAY = 200;

const Int64 CPasswordTextView::DOT_APPEAR_TEXT_DISAPPEAR_OVERLAP_DURATION = 130;

const Int64 CPasswordTextView::TEXT_REST_DURATION_AFTER_APPEAR = 100;

const Int64 CPasswordTextView::TEXT_VISIBILITY_DURATION = 1300;

const Float CPasswordTextView::OVERSHOOT_TIME_POSITION = 0.5f;

CAR_INTERFACE_IMPL(CPasswordTextView, View, IPasswordTextView)
CAR_OBJECT_IMPL(CPasswordTextView)

CPasswordTextView::CPasswordTextView()
    : mTextHeightRaw(0)
    , mText(String(""))
    , mDotSize(0)
    , mCharPadding(0)
    , mShowPassword(FALSE)
{
    CArrayList::New((IArrayList**)&mTextChars);

    CStack::New((IStack**)&mCharPool);

    CPaint::New((IPaint**)&mDrawPaint);
}

ECode CPasswordTextView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CPasswordTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CPasswordTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CPasswordTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    View::constructor(context, attrs, defStyleAttr, defStyleRes);

    SetFocusableInTouchMode(TRUE);
    SetFocusable(TRUE);
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::PasswordTextView);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    //try {
    a->GetInt32(R::styleable::PasswordTextView_scaledTextSize, 0, &mTextHeightRaw);
    //} finally {
    a->Recycle();
    //}
    mDrawPaint->SetFlags(IPaint::SUBPIXEL_TEXT_FLAG | IPaint::ANTI_ALIAS_FLAG);
    mDrawPaint->SetTextAlign(PaintAlign_CENTER);
    mDrawPaint->SetColor(0xffffffff);

    AutoPtr<ITypefaceHelper> helper;
    CTypefaceHelper::AcquireSingleton((ITypefaceHelper**)&helper);
    AutoPtr<ITypeface> typeface;
    helper->Create(String("sans-serif-light"), 0, (ITypeface**)&typeface);
    mDrawPaint->SetTypeface(typeface);

    AutoPtr<IContext> _context;
    GetContext((IContext**)&_context);
    AutoPtr<IResources> resources;
    _context->GetResources((IResources**)&resources);
    resources->GetDimensionPixelSize(R::dimen::password_dot_size, &mDotSize);
    resources->GetDimensionPixelSize(R::dimen::password_char_padding, &mCharPadding);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSystem> helper2;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper2);
    Int32 pw;
    helper2->GetInt32(cr, ISettingsSystem::TEXT_SHOW_PASSWORD, 1, &pw);
    mShowPassword = pw == 1;

    AutoPtr<IAnimationUtils> helper3;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&helper3);
    helper3->LoadInterpolator(mContext,
            Elastos::Droid::R::interpolator::linear_out_slow_in,
            (IInterpolator**)&mAppearInterpolator);
    helper3->LoadInterpolator(mContext,
            Elastos::Droid::R::interpolator::fast_out_linear_in,
            (IInterpolator**)&mDisappearInterpolator);
    helper3->LoadInterpolator(mContext,
            Elastos::Droid::R::interpolator::fast_out_slow_in,
            (IInterpolator**)&mFastOutSlowInInterpolator);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    mPM = IPowerManager::Probe(obj);
    return NOERROR;
}

void CPasswordTextView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    Float totalDrawingWidth = GetDrawingWidth();

    Int32 width;
    GetWidth(&width);
    Float currentDrawPosition = width / 2 - totalDrawingWidth / 2;
    Int32 length;
    mTextChars->GetSize(&length);
    AutoPtr<IRect> bounds = GetCharBounds();

    Int32 bottom;
    bounds->GetBottom(&bottom);
    Int32 top;
    bounds->GetTop(&top);
    Int32 charHeight = (bottom - top);

    Int32 height;
    GetHeight(&height);
    Float yPosition = height / 2;

    Int32 right;
    bounds->GetRight(&right);
    Int32 left;
    bounds->GetLeft(&left);
    Float charLength = right - left;
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> obj;
        mTextChars->Get(i, (IInterface**)&obj);
        AutoPtr<CharState> charState = (CharState*)IObject::Probe(obj);
        Float charWidth;
        charState->Draw(canvas, currentDrawPosition, charHeight, yPosition,
                charLength, &charWidth);
        currentDrawPosition += charWidth;
    }
}

ECode CPasswordTextView::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

AutoPtr<IRect> CPasswordTextView::GetCharBounds()
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetScaledDensity(&density);
    Float textHeight = mTextHeightRaw * density;
    mDrawPaint->SetTextSize(textHeight);
    AutoPtr<IRect> bounds;
    CRect::New((IRect**)&bounds);
    mDrawPaint->GetTextBounds(String("0"), 0, 1, bounds);
    return bounds;
}

Float CPasswordTextView::GetDrawingWidth()
{
    Int32 width = 0;
    Int32 length;
    mTextChars->GetSize(&length);
    AutoPtr<IRect> bounds = GetCharBounds();

    Int32 right;
    bounds->GetRight(&right);
    Int32 left;
    bounds->GetLeft(&left);
    Int32 charLength = right - left;
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> obj;
        mTextChars->Get(i, (IInterface**)&obj);
        AutoPtr<CharState> charState = (CharState*)IObject::Probe(obj);
        if (i != 0) {
            width += mCharPadding * charState->mCurrentWidthFactor;
        }
        width += charLength * charState->mCurrentWidthFactor;
    }
    return width;
}

ECode CPasswordTextView::Append(
    /* [in] */ Char32 c)
{
    Int32 visibleChars;
    mTextChars->GetSize(&visibleChars);
    mText.Append(c);
    Int32 newLength = mText.GetLength();
    AutoPtr<CharState> charState;
    if (newLength > visibleChars) {
        charState = ObtainCharState(c);
        mTextChars->Add(TO_IINTERFACE(charState));
    }
    else {
        AutoPtr<IInterface> obj;
        mTextChars->Get(newLength - 1, (IInterface**)&obj);
        charState = (CharState*)IObject::Probe(obj);
        charState->mWhichChar = c;
    }
    charState->StartAppearAnimation();

    // ensure that the previous element is being swapped
    if (newLength > 1) {
        AutoPtr<IInterface> obj;
        mTextChars->Get(newLength - 2, (IInterface**)&obj);
        AutoPtr<CharState> previousState = (CharState*)IObject::Probe(obj);
        if (previousState->mIsDotSwapPending) {
            previousState->SwapToDotWhenAppearFinished();
        }
    }
    UserActivity();
    return NOERROR;
}

void CPasswordTextView::UserActivity()
{
    mPM->UserActivity(SystemClock::GetUptimeMillis(), FALSE);
}

ECode CPasswordTextView::DeleteLastChar()
{
    Int32 length = mText.GetLength();
    if (length > 0) {
        mText = mText.Substring(0, length - 1);
        AutoPtr<IInterface> obj;
        mTextChars->Get(length - 1, (IInterface**)&obj);
        AutoPtr<CharState> charState = (CharState*)IObject::Probe(obj);
        charState->StartRemoveAnimation(0, 0);
    }
    UserActivity();
    return NOERROR;
}

ECode CPasswordTextView::GetText(
    /* [out] */ String* test)
{
    VALIDATE_NOT_NULL(test)

    *test = mText;
    return NOERROR;
}

AutoPtr<CPasswordTextView::CharState> CPasswordTextView::ObtainCharState(
    /* [in] */ Char32 c)
{
    AutoPtr<CharState> charState;

    Boolean res;
    if(mCharPool->IsEmpty(&res), res) {
        charState = new CharState(this);
    }
    else {
        AutoPtr<IInterface> outface;
        mCharPool->Pop((IInterface**)&outface);
        charState = (CharState*)IObject::Probe(outface);
        charState->Reset();
    }
    charState->mWhichChar = c;
    return charState;
}

ECode CPasswordTextView::Reset(
    /* [in] */ Boolean animated)
{
    mText = String("");
    Int32 length;
    mTextChars->GetSize(&length);
    Int32 middleIndex = (length - 1) / 2;
    Int64 delayPerElement = RESET_DELAY_PER_ELEMENT;
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> obj;
        mTextChars->Get(i, (IInterface**)&obj);
        AutoPtr<CharState> charState = (CharState*)IObject::Probe(obj);
        if (animated) {
            Int32 delayIndex;
            if (i <= middleIndex) {
                delayIndex = i * 2;
            }
            else {
                Int32 distToMiddle = i - middleIndex;
                delayIndex = (length - 1) - (distToMiddle - 1) * 2;
            }
            Int64 startDelay = delayIndex * delayPerElement;
            startDelay = Elastos::Core::Math::Min(startDelay, RESET_MAX_DELAY);
            Int64 maxDelay = delayPerElement * (length - 1);
            maxDelay = Elastos::Core::Math::Min(maxDelay, RESET_MAX_DELAY) + DISAPPEAR_DURATION;
            charState->StartRemoveAnimation(startDelay, maxDelay);
            charState->RemoveDotSwapCallbacks();
        }
        else {
            mCharPool->Push(TO_IINTERFACE(charState));
        }
    }
    if (!animated) {
        mTextChars->Clear();
    }
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
