
#include "elastos/droid/systemui/recents/views/TaskViewThumbnail.h"
#include "elastos/droid/systemui/recents/model/Task.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Graphics::CBitmapShader;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::CLightingColorFilter;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IShader;
using Elastos::Droid::Graphics::MatrixScaleToFit_FILL;
using Elastos::Droid::Graphics::ShaderTileMode_CLAMP;
using Elastos::Core::IFloat;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

static const String TAG("TaskViewThumbnail");

//===========================================================
// TaskViewThumbnail::AnimatorUpdateListener
//===========================================================
CAR_INTERFACE_IMPL(TaskViewThumbnail::AnimatorUpdateListener, Object, IAnimatorUpdateListener)

TaskViewThumbnail::AnimatorUpdateListener::AnimatorUpdateListener(
    /* [in] */ TaskViewThumbnail* host)
    : mHost(host)
{}

// @Override
ECode TaskViewThumbnail::AnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    IFloat::Probe(value)->GetValue(&mHost->mBitmapAlpha);
    mHost->UpdateFilter();
    return NOERROR;
}

//===========================================================
// TaskViewThumbnail::MyAnimatorListenerAdapter
//===========================================================
TaskViewThumbnail::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ IRunnable* postAnimRunnable)
    : mPostAnimRunnable(postAnimRunnable)
{}

// @Override
ECode TaskViewThumbnail::MyAnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mCancelled = TRUE;
    return NOERROR;
}

// @Override
ECode TaskViewThumbnail::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (!mCancelled) {
        mPostAnimRunnable->Run();
    }
    return NOERROR;
}

//===========================================================
// TaskViewThumbnail
//===========================================================
CAR_INTERFACE_IMPL(TaskViewThumbnail, Elastos::Droid::View::View, ITaskViewThumbnail)

TaskViewThumbnail::TaskViewThumbnail()
    : mCornerRadius(0)
    , mBitmapAlpha(0.0f)
    , mDimAlpha(0.0f)
    , mInvisible(FALSE)
{
}

ECode TaskViewThumbnail::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode TaskViewThumbnail::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode TaskViewThumbnail::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode TaskViewThumbnail::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(View::constructor(context, attrs, defStyleAttr, defStyleRes));

    CMatrix::New((IMatrix**)&mScaleMatrix);
    CRect::New((IRect**)&mClipRect);
    CPaint::New((IPaint**)&mDrawPaint);
    CLightingColorFilter::New(0xffffffff, 0, (ILightingColorFilter**)&mLightingColorFilter);
    CRectF::New((IRectF**)&mBitmapRect);
    CRectF::New((IRectF**)&mLayoutRect);
    mAlphaUpdateListener = new AnimatorUpdateListener(this);

    mConfig = RecentsConfiguration::GetInstance();
    mCornerRadius = mConfig->mTaskViewRoundedCornerRadiusPx;
    mDrawPaint->SetColorFilter(IColorFilter::Probe(mLightingColorFilter));
    mDrawPaint->SetFilterBitmap(TRUE);
    mDrawPaint->SetAntiAlias(TRUE);
    return NOERROR;
}

// @Override
void TaskViewThumbnail::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (mInvisible) {
        return;
    }

    Int32 w, h;
    GetWidth(&w);
    GetHeight(&h);
    canvas->DrawRoundRect(0, 0, w, h, mCornerRadius, mCornerRadius, mDrawPaint);
}

// @Override
ECode TaskViewThumbnail::OnFinishInflate()
{
    mBitmapAlpha = 0.9f;
    UpdateFilter();
    return NOERROR;
}

void TaskViewThumbnail::UpdateFilter()
{
    if (mInvisible) {
        return;
    }
    Int32 mul = (Int32) ((1.0f - mDimAlpha) * mBitmapAlpha * 255);
    Int32 add = (Int32) ((1.0f - mDimAlpha) * (1 - mBitmapAlpha) * 255);
    AutoPtr<IColor> color;
    CColor::AcquireSingleton((IColor**)&color);
    if (mBitmapShader != NULL) {
        Int32 argb;
        color->Argb(255, mul, mul, mul, &argb);
        mLightingColorFilter->SetColorMultiply(argb);
        color->Argb(0, add, add, add, &argb);
        mLightingColorFilter->SetColorAdd(argb);
        mDrawPaint->SetColorFilter(IColorFilter::Probe(mLightingColorFilter));
        mDrawPaint->SetColor(0xffffffff);
    }
    else {
        mDrawPaint->SetColorFilter(NULL);
        Int32 grey = mul + add;
        Int32 argb;
        color->Argb(255, grey, grey, grey, &argb);
        mDrawPaint->SetColor(argb);
    }
    Invalidate();
}

/** Updates the clip rect based on the given task bar. */
void TaskViewThumbnail::EnableTaskBarClip(
    /* [in] */ IView* taskBar)
{
    mTaskBar = taskBar;
    Float y;
    taskBar->GetTranslationY(&y);
    Int32 h;
    taskBar->GetMeasuredHeight(&h);
    Int32 top = (Int32) Elastos::Core::Math::Max(0.0f, y + h - 1);
    Int32 width, height;
    GetMeasuredHeight(&width);
    GetMeasuredHeight(&height);
    mClipRect->Set(0, top, width, height);
    SetClipBounds(mClipRect);
}

void TaskViewThumbnail::UpdateVisibility(
    /* [in] */ Int32 clipBottom)
{
    Int32 h, th;
    Boolean invisible = FALSE;
    if (mTaskBar != NULL) {
        GetHeight(&h);
        mTaskBar->GetHeight(&th);
        invisible = h - clipBottom < th;
    }

    if (invisible != mInvisible) {
        mInvisible = invisible;
        if (!mInvisible) {
            UpdateFilter();
        }
        Invalidate();
    }
}

/** Binds the thumbnail view to the screenshot. */
Boolean TaskViewThumbnail::BindToScreenshot(
    /* [in] */ IBitmap* ss)
{
    SetImageBitmap(ss);
    return ss != NULL;
}

/** Unbinds the thumbnail view from the screenshot. */
void TaskViewThumbnail::UnbindFromScreenshot()
{
    SetImageBitmap(NULL);
}

/** Binds the thumbnail view to the task */
ECode TaskViewThumbnail::RebindToTask(
    /* [in] */ Task* t)
{
    if (t->mThumbnail != NULL) {
        SetImageBitmap(t->mThumbnail);
    }
    else {
        SetImageBitmap(NULL);
    }
    return NOERROR;
}

ECode TaskViewThumbnail::SetImageBitmap(
    /* [in] */ IBitmap* bm)
{
    if (bm != NULL) {
        mBitmapShader = NULL;
        CBitmapShader::New(bm, ShaderTileMode_CLAMP,
            ShaderTileMode_CLAMP, (IBitmapShader**)&mBitmapShader);
        mDrawPaint->SetShader(IShader::Probe(mBitmapShader));
        Int32 width, height;
        bm->GetWidth(&width);
        bm->GetHeight(&height);
        mBitmapRect->Set(0, 0, width, height);
        UpdateBitmapScale();
    }
    else {
        mBitmapShader = NULL;
        mDrawPaint->SetShader(NULL);
    }
    UpdateFilter();
    return NOERROR;
}

// @Override
ECode TaskViewThumbnail::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    Elastos::Droid::View::View::OnLayout(changed, left, top, right, bottom);
    if (changed) {
        Int32 width, height;
        GetWidth(&width);
        GetHeight(&height);
        mLayoutRect->Set(0, 0, width, height);
        UpdateBitmapScale();
    }
    return NOERROR;
}

void TaskViewThumbnail::UpdateBitmapScale()
{
    if (mBitmapShader != NULL) {
        Boolean res;
        mScaleMatrix->SetRectToRect(mBitmapRect, mLayoutRect, MatrixScaleToFit_FILL, &res);
        IShader::Probe(mBitmapShader)->SetLocalMatrix(mScaleMatrix);
    }
}

ECode TaskViewThumbnail::SetDimAlpha(
    /* [in] */ Float dimAlpha)
{
    mDimAlpha = dimAlpha;
    UpdateFilter();
    return NOERROR;
}

/** Unbinds the thumbnail view from the task */
void TaskViewThumbnail::UnbindFromTask()
{
    SetImageBitmap(NULL);
}

/** Handles focus changes. */
void TaskViewThumbnail::OnFocusChanged(
    /* [in] */ Boolean focused)
{
    Float alpha;
    GetAlpha(&alpha);
    if (focused) {
        if (Elastos::Core::Math::Compare(alpha, 1.0f) != 0) {
            StartFadeAnimation(1.0f, 0, 150, NULL);
        }
    }
    else {
        if (Elastos::Core::Math::Compare(alpha, mConfig->mTaskViewThumbnailAlpha) != 0) {
            StartFadeAnimation(mConfig->mTaskViewThumbnailAlpha, 0, 150, NULL);
        }
    }
}

/** Prepares for the enter recents animation. */
void TaskViewThumbnail::PrepareEnterRecentsAnimation(
    /* [in] */ Boolean isTaskViewLaunchTargetTask)
{
    if (isTaskViewLaunchTargetTask) {
        mBitmapAlpha = 1.0f;
    }
    else {
        mBitmapAlpha = mConfig->mTaskViewThumbnailAlpha;
    }
    UpdateFilter();
}

/** Animates this task thumbnail as it enters recents */
void TaskViewThumbnail::StartEnterRecentsAnimation(
    /* [in] */ Int32 delay,
    /* [in] */ IRunnable* postAnimRunnable)
{
    StartFadeAnimation(mConfig->mTaskViewThumbnailAlpha, delay,
        mConfig->mTaskBarEnterAnimDuration, postAnimRunnable);
}

/** Animates this task thumbnail as it exits recents */
void TaskViewThumbnail::StartLaunchTaskAnimation(
    /* [in] */ IRunnable* postAnimRunnable)
{
    StartFadeAnimation(1.0f, 0, mConfig->mTaskBarExitAnimDuration, postAnimRunnable);
}

/** Animates the thumbnail alpha. */
void TaskViewThumbnail::StartFadeAnimation(
    /* [in] */ Float finalAlpha,
    /* [in] */ Int32 delay,
    /* [in] */ Int32 duration,
    /* [in] */ IRunnable* postAnimRunnable)
{
    if (mAlphaAnimator != NULL) {
        IAnimator::Probe(mAlphaAnimator)->Cancel();
        mAlphaAnimator = NULL;
    }
    AutoPtr<IValueAnimatorHelper> vaHelper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&vaHelper);
    AutoPtr<ArrayOf<Float> > params = ArrayOf<Float>::Alloc(2);
    (*params)[0] = mBitmapAlpha;
    (*params)[1] = finalAlpha;
    vaHelper->OfFloat(params, (IValueAnimator**)&mAlphaAnimator);
    IAnimator* animator = IAnimator::Probe(mAlphaAnimator);
    mAlphaAnimator->AddUpdateListener(mAlphaUpdateListener);
    animator->SetStartDelay(delay);
    animator->SetInterpolator(mConfig->mFastOutSlowInInterpolator);
    animator->SetDuration(duration);
    animator->Start();
    if (postAnimRunnable != NULL) {
        AutoPtr<AnimatorListenerAdapter> adapter = new MyAnimatorListenerAdapter(postAnimRunnable);
        animator->AddListener(adapter);
    }
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
