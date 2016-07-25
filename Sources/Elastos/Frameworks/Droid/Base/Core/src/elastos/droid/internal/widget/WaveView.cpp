
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/graphics/drawable/CBitmapDrawable.h"
#include "elastos/droid/internal/widget/CDrawableHolder.h"
#include "elastos/droid/internal/widget/WaveView.h"
#include "elastos/droid/media/CAudioAttributesBuilder.h"
#include "elastos/droid/provider/CSettingsSecure.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/core/IntegralToString.h"
#include <elastos/core/Math.h>
#include "elastos/core/StringUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Internal::Widget::CDrawableHolder;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::View::Accessibility::CAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::EIID_IView;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IntegralToString;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

const String WaveView::TAG("WaveView");
const Boolean WaveView::DBG;
const Int32 WaveView::WAVE_COUNT; // default wave count
const Int64 WaveView::VIBRATE_SHORT;  // msec
const Int64 WaveView::VIBRATE_LONG;  // msec
const Int32 WaveView::STATE_RESET_LOCK;
const Int32 WaveView::STATE_READY;
const Int32 WaveView::STATE_START_ATTEMPT;
const Int32 WaveView::STATE_ATTEMPTING;
const Int32 WaveView::STATE_UNLOCK_ATTEMPT;
const Int32 WaveView::STATE_UNLOCK_SUCCESS;
const Int64 WaveView::DURATION; // duration of transitional animations
const Int64 WaveView::FINAL_DURATION; // duration of final animations when unlocking
const Int64 WaveView::RING_DELAY; // when to start fading animated rings
const Int64 WaveView::FINAL_DELAY; // delay for unlock success animation
const Int64 WaveView::SHORT_DELAY; // for starting one animation after another.
const Int64 WaveView::WAVE_DURATION; // amount of time for way to expand/decay
const Int64 WaveView::RESET_TIMEOUT;// elapsed time of inactivity before we reset
const Int64 WaveView::DELAY_INCREMENT; // increment per wave while tracking motion
const Int64 WaveView::DELAY_INCREMENT2; // increment per wave while not tracking
const Int64 WaveView::WAVE_DELAY; // initial propagation delay
const Float WaveView::GRAB_HANDLE_RADIUS_SCALE_ACCESSIBILITY_DISABLED;
const Float WaveView::GRAB_HANDLE_RADIUS_SCALE_ACCESSIBILITY_ENABLED;
const AutoPtr<IAudioAttributes> WaveView::VIBRATION_ATTRIBUTES = WaveView::MiddleInitVibrationAttributes();

CAR_INTERFACE_IMPL(WaveView::InnerLockTimerRunnable, Object, IRunnable)
CAR_INTERFACE_IMPL(WaveView::InnerAddWaveRunnable, Object, IRunnable)
CAR_INTERFACE_IMPL(WaveView, View, IAnimatorUpdateListener)

WaveView::InnerLockTimerRunnable::InnerLockTimerRunnable(
    /* [in] */ WaveView* host)
    : mHost(host)
{
    assert(mHost);
}

ECode WaveView::InnerLockTimerRunnable::Run()
{
    if (DBG)
        Logger::V(TAG, "LockTimerActions");

    if (mHost->mLockState == STATE_ATTEMPTING) {
        if (DBG)
            Logger::V(TAG, "Timer resets to STATE_RESET_LOCK");
        mHost->mLockState = STATE_RESET_LOCK;
    }

    if (mHost->mLockState == STATE_UNLOCK_SUCCESS) {
        if (DBG)
            Logger::V(TAG, "Timer resets to STATE_RESET_LOCK after success");
        mHost->mLockState = STATE_RESET_LOCK;
    }
    mHost->Invalidate();
    return NOERROR;
}

WaveView::InnerAddWaveRunnable::InnerAddWaveRunnable(
    /* [in] */ WaveView* host)
    : mHost(host)
{
    assert(mHost);
}

ECode WaveView::InnerAddWaveRunnable::Run()
{
    String strX("x");
    String strY("y");
    String alpha("alpha");
    String scaleX("scaleX");
    String scaleY("scaleY");
    Double distX = mHost->mMouseX - mHost->mLockCenterX;
    Double distY = mHost->mMouseY - mHost->mLockCenterY;
    Int32 dragDistance = (Int32) Elastos::Core::Math::Ceil(Elastos::Core::Math::Hypot(distX, distY));
    if (mHost->mLockState == STATE_ATTEMPTING && dragDistance < mHost->mSnapRadius
            && mHost->mWaveTimerDelay >= WAVE_DELAY) {
        mHost->mWaveTimerDelay = Elastos::Core::Math::Min(WAVE_DURATION, mHost->mWaveTimerDelay + DELAY_INCREMENT);

        AutoPtr<IDrawableHolder> wave = (mHost->mLightWaves)[mHost->mCurrentWave];
        wave->SetAlpha(0.0f);
        wave->SetScaleX(0.2f);
        wave->SetScaleY(0.2f);
        wave->SetX(mHost->mMouseX);
        wave->SetY(mHost->mMouseY);

        AutoPtr<IObjectAnimator> res;
        wave->AddAnimTo(WAVE_DURATION, 0L, strX, mHost->mLockCenterX, TRUE, (IObjectAnimator**)&res);
        res = NULL;
        wave->AddAnimTo(WAVE_DURATION, 0L, strY, mHost->mLockCenterY, TRUE, (IObjectAnimator**)&res);
        res = NULL;
        wave->AddAnimTo(WAVE_DURATION*2/3, 0L, alpha, 1.0f, TRUE, (IObjectAnimator**)&res);
        res = NULL;
        wave->AddAnimTo(WAVE_DURATION, 0L, scaleX, 1.0f, TRUE, (IObjectAnimator**)&res);
        res = NULL;
        wave->AddAnimTo(WAVE_DURATION, 0L, scaleY, 1.0f, TRUE, (IObjectAnimator**)&res);

        wave->AddAnimTo(1000L, RING_DELAY, alpha, 0.0f, FALSE, (IObjectAnimator**)&res);
        res = NULL;
        wave->StartAnimations(IAnimatorUpdateListener::Probe(mHost));

        mHost->mCurrentWave = (mHost->mCurrentWave + 1) % mHost->mWaveCount;
        String sWaveTimerDelay = StringUtils::ToString(mHost->mWaveTimerDelay);
        if (DBG) Logger::V(TAG, String("WaveTimerDelay: start new wave in ") + sWaveTimerDelay);
    }
    else {
        mHost->mWaveTimerDelay += DELAY_INCREMENT2;
    }

    if (mHost->mFinishWaves) {
        // sentinel used to restart the waves after they've stopped
        mHost->mWavesRunning = FALSE;
    }
    else {
        Boolean resTmp;
        mHost->PostDelayed(mHost->mAddWaveAction, mHost->mWaveTimerDelay, &resTmp);
    }
    return NOERROR;
}

WaveView::WaveView()
    : mFingerDown(FALSE)
    , mRingRadius(182.0f)
    , mSnapRadius(136)
    , mWaveCount(WAVE_COUNT)
    , mWaveTimerDelay(WAVE_DELAY)
    , mCurrentWave(0)
    , mLockCenterX(0.0f)
    , mLockCenterY(0.0f)
    , mMouseX(0.0f)
    , mMouseY(0.0f)
    , mLockState(STATE_RESET_LOCK)
    , mGrabbedState(IWaveViewOnTriggerListener::NO_HANDLE)
    , mWavesRunning(FALSE)
    , mFinishWaves(FALSE)
{
}

ECode WaveView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode WaveView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    View::constructor(context, attrs);
    InitDrawables();
    return NOERROR;
}

ECode WaveView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    mLockCenterX = 0.5f * w;
    mLockCenterY = 0.5f * h;
    View::OnSizeChanged(w, h, oldw, oldh);
    return NOERROR;
}

Int32 WaveView::GetSuggestedMinimumWidth()
{
    // View should be large enough to contain the unlock ring + halo
    Int32 ringWidth, haloWidth;
    mUnlockRing->GetWidth(&ringWidth);
    mUnlockHalo->GetWidth(&haloWidth);
    return ringWidth + haloWidth ;
}

Int32 WaveView::GetSuggestedMinimumHeight()
{
    // View should be large enough to contain the unlock ring + halo
    Int32 h1, h2;
    mUnlockRing->GetHeight(&h1);
    mUnlockHalo->GetHeight(&h2);
    return h1 + h2;
}

ECode WaveView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 widthSpecMode = View::MeasureSpec::GetMode(widthMeasureSpec);
    Int32 heightSpecMode = View::MeasureSpec::GetMode(heightMeasureSpec);
    Int32 widthSpecSize =  View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 heightSpecSize =  View::MeasureSpec::GetSize(heightMeasureSpec);
    Int32 width;
    Int32 height;

    if (widthSpecMode == MeasureSpec::AT_MOST) {
        width = Elastos::Core::Math::Min(widthSpecSize, GetSuggestedMinimumWidth());
    }
    else if (widthSpecMode == MeasureSpec::EXACTLY) {
        width = widthSpecSize;
    }
    else {
        width = GetSuggestedMinimumWidth();
    }

    if (heightSpecMode == MeasureSpec::AT_MOST) {
        height = Elastos::Core::Math::Min(heightSpecSize, GetSuggestedMinimumWidth());
    }
    else if (heightSpecMode == MeasureSpec::EXACTLY) {
        height = heightSpecSize;
    }
    else {
        height = GetSuggestedMinimumHeight();
    }

    SetMeasuredDimension(width, height);
    return NOERROR;
}

void WaveView::InitDrawables()
{
    CDrawableHolder::New(CreateDrawable(R::drawable::unlock_ring), (IDrawableHolder**)&mUnlockRing);
    mUnlockRing->SetX(mLockCenterX);
    mUnlockRing->SetY(mLockCenterY);
    mUnlockRing->SetScaleX(0.1f);
    mUnlockRing->SetScaleY(0.1f);
    mUnlockRing->SetAlpha(0.0f);
    mDrawables.PushBack(mUnlockRing);

    CDrawableHolder::New(CreateDrawable(R::drawable::unlock_default), (IDrawableHolder**)&mUnlockDefault);
    mUnlockDefault->SetX(mLockCenterX);
    mUnlockDefault->SetY(mLockCenterY);
    mUnlockDefault->SetScaleX(0.1f);
    mUnlockDefault->SetScaleY(0.1f);
    mUnlockDefault->SetAlpha(0.0f);
    mDrawables.PushBack(mUnlockDefault);

    CDrawableHolder::New(CreateDrawable(R::drawable::unlock_halo), (IDrawableHolder**)&mUnlockHalo);
    mUnlockHalo->SetX(mLockCenterX);
    mUnlockHalo->SetY(mLockCenterY);
    mUnlockHalo->SetScaleX(0.1f);
    mUnlockHalo->SetScaleY(0.1f);
    mUnlockHalo->SetAlpha(0.0f);
    mDrawables.PushBack(mUnlockHalo);

    AutoPtr<IBitmapDrawable> wave = CreateDrawable(R::drawable::unlock_wave);
    for (Int32 i = 0; i < mWaveCount; i++) {
        AutoPtr<IDrawableHolder> holder;
        CDrawableHolder::New(wave, (IDrawableHolder**)&holder);
        mLightWaves.PushBack(holder);
        holder->SetAlpha(0.0f);
    }
}

void WaveView::WaveUpdateFrame(
    /* [in] */ Float mouseX,
    /* [in] */ Float mouseY,
    /* [in] */ Boolean fingerDown)
{
    Double distX = mouseX - mLockCenterX;
    Double distY = mouseY - mLockCenterY;
    Int32 dragDistance = (Int32) Elastos::Core::Math::Ceil(Elastos::Core::Math::Hypot(distX, distY));
    Double touchA = Elastos::Core::Math::Atan2(distX, distY);
    Float ringX = (Float) (mLockCenterX + mRingRadius * Elastos::Core::Math::Sin(touchA));
    Float ringY = (Float) (mLockCenterY + mRingRadius * Elastos::Core::Math::Cos(touchA));
    String strX("x");
    String strY("y");
    String alpha("alpha");
    String scaleX("scaleX");
    String scaleY("scaleY");
    AutoPtr <IObjectAnimator> res;
    switch (mLockState) {
        case STATE_RESET_LOCK:
            {
                if (DBG)
                    Logger::V(TAG, "State RESET_LOCK");
                mWaveTimerDelay = WAVE_DELAY;
                for (UInt32 i = 0; i < mLightWaves.GetSize(); i++) {
                    AutoPtr<IDrawableHolder> holder = mLightWaves[i];
                    holder->AddAnimTo(300, 0, alpha, 0.0f, FALSE, (IObjectAnimator**)&res);
                    res = NULL;
                }
                for (UInt32 i = 0; i < mLightWaves.GetSize(); i++) {
                    mLightWaves[i]->StartAnimations(IAnimatorUpdateListener::Probe(this));
                }

                mUnlockRing->AddAnimTo(DURATION, 0, strX, mLockCenterX, TRUE, (IObjectAnimator**)&res);
                res = NULL;
                mUnlockRing->AddAnimTo(DURATION, 0, strY, mLockCenterY, TRUE, (IObjectAnimator**)&res);
                res = NULL;
                mUnlockRing->AddAnimTo(DURATION, 0, scaleX, 0.1f, TRUE, (IObjectAnimator**)&res);
                res = NULL;
                mUnlockRing->AddAnimTo(DURATION, 0, scaleY, 0.1f, TRUE, (IObjectAnimator**)&res);
                res = NULL;
                mUnlockRing->AddAnimTo(DURATION, 0, alpha, 0.0f, TRUE, (IObjectAnimator**)&res);
                res = NULL;

                mUnlockDefault->RemoveAnimationFor(strX);
                mUnlockDefault->RemoveAnimationFor(strY);
                mUnlockDefault->RemoveAnimationFor(scaleX);
                mUnlockDefault->RemoveAnimationFor(scaleY);
                mUnlockDefault->RemoveAnimationFor(alpha);
                mUnlockDefault->SetX(mLockCenterX);
                mUnlockDefault->SetY(mLockCenterY);
                mUnlockDefault->SetScaleX(0.1f);
                mUnlockDefault->SetScaleY(0.1f);
                mUnlockDefault->SetAlpha(0.0f);
                mUnlockDefault->AddAnimTo(DURATION, SHORT_DELAY, scaleX, 1.0f, TRUE, (IObjectAnimator**)&res);
                res = NULL;
                mUnlockDefault->AddAnimTo(DURATION, SHORT_DELAY, scaleY, 1.0f, TRUE, (IObjectAnimator**)&res);
                res = NULL;
                mUnlockDefault->AddAnimTo(DURATION, SHORT_DELAY, alpha, 1.0f, TRUE, (IObjectAnimator**)&res);
                res = NULL;

                mUnlockHalo->RemoveAnimationFor(strX);
                mUnlockHalo->RemoveAnimationFor(strY);
                mUnlockHalo->RemoveAnimationFor(scaleX);
                mUnlockHalo->RemoveAnimationFor(scaleY);
                mUnlockHalo->RemoveAnimationFor(alpha);
                mUnlockHalo->SetX(mLockCenterX);
                mUnlockHalo->SetY(mLockCenterY);
                mUnlockHalo->SetScaleX(0.1f);
                mUnlockHalo->SetScaleY(0.1f);
                mUnlockHalo->SetAlpha(0.0f);
                mUnlockHalo->AddAnimTo(DURATION, SHORT_DELAY, strX, mLockCenterX, TRUE, (IObjectAnimator**)&res);
                res = NULL;
                mUnlockHalo->AddAnimTo(DURATION, SHORT_DELAY, strY, mLockCenterY, TRUE, (IObjectAnimator**)&res);
                res = NULL;
                mUnlockHalo->AddAnimTo(DURATION, SHORT_DELAY, scaleX, 1.0f, TRUE, (IObjectAnimator**)&res);
                res = NULL;
                mUnlockHalo->AddAnimTo(DURATION, SHORT_DELAY, scaleY, 1.0f, TRUE, (IObjectAnimator**)&res);
                res = NULL;
                mUnlockHalo->AddAnimTo(DURATION, SHORT_DELAY, alpha, 1.0f, TRUE, (IObjectAnimator**)&res);
                res = NULL;

                Boolean resTmp;
                RemoveCallbacks(mLockTimerActions, &resTmp);
                mLockState = STATE_READY;
            }
            break;
        case STATE_READY:
            {
                if (DBG)
                    Logger::V(TAG, "State READY");
                mWaveTimerDelay = WAVE_DELAY;
            }
            break;
        case STATE_START_ATTEMPT:
            {
                if (DBG)
                    Logger::V(TAG, "State START_ATTEMPT");
                mUnlockDefault->RemoveAnimationFor(strX);
                mUnlockDefault->RemoveAnimationFor(strY);
                mUnlockDefault->RemoveAnimationFor(scaleX);
                mUnlockDefault->RemoveAnimationFor(scaleY);
                mUnlockDefault->RemoveAnimationFor(alpha);
                mUnlockDefault->SetX(mLockCenterX + 182);
                mUnlockDefault->SetY(mLockCenterY);
                mUnlockDefault->SetScaleX(0.1f);
                mUnlockDefault->SetScaleY(0.1f);
                mUnlockDefault->SetAlpha(0.0f);

                mUnlockDefault->AddAnimTo(DURATION, SHORT_DELAY, scaleX, 1.0f, FALSE, (IObjectAnimator**)&res);
                res = NULL;
                mUnlockDefault->AddAnimTo(DURATION, SHORT_DELAY, scaleY, 1.0f, FALSE, (IObjectAnimator**)&res);
                res = NULL;
                mUnlockDefault->AddAnimTo(DURATION, SHORT_DELAY, alpha, 1.0f, FALSE, (IObjectAnimator**)&res);
                res = NULL;

                mUnlockRing->AddAnimTo(DURATION, 0, scaleX, 1.0f, TRUE, (IObjectAnimator**)&res);
                res = NULL;
                mUnlockRing->AddAnimTo(DURATION, 0, scaleY, 1.0f, TRUE, (IObjectAnimator**)&res);
                res = NULL;
                mUnlockRing->AddAnimTo(DURATION, 0, alpha, 1.0f, TRUE, (IObjectAnimator**)&res);
                res = NULL;

                mLockState = STATE_ATTEMPTING;
            }
            break;
        case STATE_ATTEMPTING:
            {
                String sfingerDown = StringUtils::BooleanToString(fingerDown);
                if (DBG)
                    Logger::V(TAG, String("State ATTEMPTING (fingerDown = ") + sfingerDown + ")");
                if (dragDistance > mSnapRadius) {
                    mFinishWaves = TRUE; // don't start any more waves.
                    if (fingerDown) {
                        res = NULL;
                        mUnlockHalo->AddAnimTo(0, 0, strX, ringX, TRUE, (IObjectAnimator**)&res);
                        res = NULL;
                        mUnlockHalo->AddAnimTo(0, 0, strY, ringY, TRUE, (IObjectAnimator**)&res);
                        res = NULL;
                        mUnlockHalo->AddAnimTo(0, 0, scaleX, 1.0f, TRUE, (IObjectAnimator**)&res);
                        res = NULL;
                        mUnlockHalo->AddAnimTo(0, 0, scaleY, 1.0f, TRUE, (IObjectAnimator**)&res);
                        res = NULL;
                        mUnlockHalo->AddAnimTo(0, 0, alpha, 1.0f, TRUE, (IObjectAnimator**)&res);
                        res = NULL;
                    }
                    else {
                        if (DBG)
                            Logger::V(TAG, "up detected, moving to STATE_UNLOCK_ATTEMPT");
                        mLockState = STATE_UNLOCK_ATTEMPT;
                    }
                }
                else {
                    // If waves have stopped, we need to kick them off again...
                    if (!mWavesRunning) {
                        mWavesRunning = TRUE;
                        mFinishWaves = FALSE;
                        // mWaveTimerDelay = WAVE_DELAY;
                        Boolean resTmp;
                        PostDelayed(mAddWaveAction, mWaveTimerDelay, &resTmp);
                    }
                    res = NULL;
                    mUnlockHalo->AddAnimTo(0, 0, strX, mouseX, TRUE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockHalo->AddAnimTo(0, 0, strY, mouseY, TRUE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockHalo->AddAnimTo(0, 0, scaleX, 1.0f, TRUE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockHalo->AddAnimTo(0, 0, scaleY, 1.0f, TRUE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockHalo->AddAnimTo(0, 0, alpha, 1.0f, TRUE, (IObjectAnimator**)&res);
                    res = NULL;
                }
            }
            break;
        case STATE_UNLOCK_ATTEMPT:
            {
                if (DBG)
                    Logger::V(TAG, "State UNLOCK_ATTEMPT");
                if (dragDistance > mSnapRadius) {
                    for (UInt32 n = 0; n < mLightWaves.GetSize(); n++) {
                        AutoPtr<IDrawableHolder> wave;
                        wave = mLightWaves[n];
                        Int64 delay = 1000LL*(6 + n - mCurrentWave)/10LL;
                        res = NULL;
                        wave->AddAnimTo(FINAL_DURATION, delay, strX, ringX, TRUE, (IObjectAnimator**)&res);
                        res = NULL;
                        wave->AddAnimTo(FINAL_DURATION, delay, strY, ringY, TRUE, (IObjectAnimator**)&res);
                        res = NULL;
                        wave->AddAnimTo(FINAL_DURATION, delay, scaleX, 0.1f, TRUE, (IObjectAnimator**)&res);
                        res = NULL;
                        wave->AddAnimTo(FINAL_DURATION, delay, scaleY, 0.1f, TRUE, (IObjectAnimator**)&res);
                        res = NULL;
                        wave->AddAnimTo(FINAL_DURATION, delay, alpha, 0.0f, TRUE, (IObjectAnimator**)&res);
                        res = NULL;
                    }
                    for (UInt32 i = 0; i < mLightWaves.GetSize(); i++) {
                        mLightWaves[i]->StartAnimations(IAnimatorUpdateListener::Probe(this));
                    }
                    res = NULL;
                    mUnlockRing->AddAnimTo(FINAL_DURATION, 0, strX, ringX, FALSE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockRing->AddAnimTo(FINAL_DURATION, 0, strY, ringY, FALSE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockRing->AddAnimTo(FINAL_DURATION, 0, scaleX, 0.1f, FALSE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockRing->AddAnimTo(FINAL_DURATION, 0, scaleY, 0.1f, FALSE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockRing->AddAnimTo(FINAL_DURATION, 0, alpha, 0.0f, FALSE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockRing->AddAnimTo(FINAL_DURATION, FINAL_DELAY, alpha, 0.0f, FALSE, (IObjectAnimator**)&res);
                    res = NULL;

                    mUnlockDefault->RemoveAnimationFor(strX);
                    mUnlockDefault->RemoveAnimationFor(strY);
                    mUnlockDefault->RemoveAnimationFor(scaleX);
                    mUnlockDefault->RemoveAnimationFor(scaleY);
                    mUnlockDefault->RemoveAnimationFor(alpha);
                    mUnlockDefault->SetX(ringX);
                    mUnlockDefault->SetY(ringY);
                    mUnlockDefault->SetScaleX(0.1f);
                    mUnlockDefault->SetScaleY(0.1f);
                    mUnlockDefault->SetAlpha(0.0f);

                    mUnlockDefault->AddAnimTo(FINAL_DURATION, 0, strX, ringX, TRUE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockDefault->AddAnimTo(FINAL_DURATION, 0, strY, ringY, TRUE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockDefault->AddAnimTo(FINAL_DURATION, 0, scaleX, 1.0f, TRUE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockDefault->AddAnimTo(FINAL_DURATION, 0, scaleY, 1.0f, TRUE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockDefault->AddAnimTo(FINAL_DURATION, 0, alpha, 1.0f, TRUE, (IObjectAnimator**)&res);
                    res = NULL;

                    mUnlockDefault->AddAnimTo(FINAL_DURATION, FINAL_DELAY, scaleX, 3.0f, FALSE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockDefault->AddAnimTo(FINAL_DURATION, FINAL_DELAY, scaleY, 3.0f, FALSE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockDefault->AddAnimTo(FINAL_DURATION, FINAL_DELAY, alpha, 0.0f, FALSE, (IObjectAnimator**)&res);
                    res = NULL;

                    mUnlockHalo->AddAnimTo(FINAL_DURATION, 0, strX, ringX, FALSE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockHalo->AddAnimTo(FINAL_DURATION, 0, strY, ringY, FALSE, (IObjectAnimator**)&res);
                    res = NULL;

                    mUnlockHalo->AddAnimTo(FINAL_DURATION, FINAL_DELAY, scaleX, 3.0f, FALSE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockHalo->AddAnimTo(FINAL_DURATION, FINAL_DELAY, scaleY, 3.0f, FALSE, (IObjectAnimator**)&res);
                    res = NULL;
                    mUnlockHalo->AddAnimTo(FINAL_DURATION, FINAL_DELAY, alpha, 0.0f, FALSE, (IObjectAnimator**)&res);
                    res = NULL;

                    Boolean resTmp;
                    RemoveCallbacks(mLockTimerActions, &resTmp);
                    PostDelayed(mLockTimerActions, RESET_TIMEOUT, &resTmp);
                    DispatchTriggerEvent(IWaveViewOnTriggerListener::CENTER_HANDLE);
                    mLockState = STATE_UNLOCK_SUCCESS;
                }
                else {
                    mLockState = STATE_RESET_LOCK;
                }
            }
            break;
        case STATE_UNLOCK_SUCCESS:
            {
                if (DBG)
                    Logger::V(TAG, "State UNLOCK_SUCCESS");
                Boolean resTmp;
                RemoveCallbacks(mAddWaveAction, &resTmp);
            }
            break;
        default:
            {
                if (DBG)
                    Logger::V(TAG, String("Unknown state ") + IntegralToString::ToString(mLockState));
            }
            break;
    }
    mUnlockDefault->StartAnimations(IAnimatorUpdateListener::Probe(this));
    mUnlockHalo->StartAnimations(IAnimatorUpdateListener::Probe(this));
    mUnlockRing->StartAnimations(IAnimatorUpdateListener::Probe(this));
}

AutoPtr<IBitmapDrawable> WaveView::CreateDrawable(
    /* [in] */ Int32 resId)
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IBitmapFactory> factory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
    AutoPtr<IBitmap> bitmap;
    factory->DecodeResource(res, resId, (IBitmap**)&bitmap);

    AutoPtr<IBitmapDrawable> bd;
    CBitmapDrawable::New(res, bitmap, (IBitmapDrawable**)&bd);
    return bd;
}

void WaveView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    WaveUpdateFrame(mMouseX, mMouseY, mFingerDown);
    for (UInt32 i = 0; i < mDrawables.GetSize(); ++i) {
        mDrawables[i]->Draw(canvas);
    }
    for (UInt32 i = 0; i < mLightWaves.GetSize(); ++i) {
        mLightWaves[i]->Draw(canvas);
    }
}


ECode WaveView::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);

    AutoPtr<IAccessibilityManager> manager;
    CAccessibilityManager::GetInstance(mContext, (IAccessibilityManager**)&manager);
    Boolean resTmp;
    if (manager->IsTouchExplorationEnabled(&resTmp), resTmp) {
        Int32 action;
        event->GetAction(&action);

        switch (action) {
            case IMotionEvent::ACTION_HOVER_ENTER:
                event->SetAction(IMotionEvent::ACTION_DOWN);
                break;
            case IMotionEvent::ACTION_HOVER_MOVE:
                event->SetAction(IMotionEvent::ACTION_MOVE);
                break;
            case IMotionEvent::ACTION_HOVER_EXIT:
                event->SetAction(IMotionEvent::ACTION_UP);
                break;
        }

        OnTouchEvent(event, &resTmp);
        event->SetAction(action);
    }
    return View::OnHoverEvent(event, result);
}

ECode WaveView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);

    Int32 action;
    event->GetAction(&action);

    event->GetX(&mMouseX);
    event->GetY(&mMouseY);
    Boolean handled = FALSE;
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            {
                Boolean resTmp;
                RemoveCallbacks(mLockTimerActions, &resTmp);
                mFingerDown = TRUE;
                TryTransitionToStartAttemptState(event);
                handled = TRUE;
            }
            break;
        case IMotionEvent::ACTION_MOVE:
            {
                TryTransitionToStartAttemptState(event);
                handled = TRUE;
            }
            break;
        case IMotionEvent::ACTION_UP:
            {
                if (DBG)
                    Logger::V(TAG, "ACTION_UP");
                mFingerDown = FALSE;
                Boolean resTmp;
                PostDelayed(mLockTimerActions, RESET_TIMEOUT, &resTmp);
                SetGrabbedState(IWaveViewOnTriggerListener::NO_HANDLE);
                // Normally the state machine is driven by user interaction causing redraws.
                // However, when there's no more user interaction and no running animations,
                // the state machine stops advancing because onDraw() never gets called.
                // The following ensures we advance to the next state in this case,
                // either STATE_UNLOCK_ATTEMPT or STATE_RESET_LOCK.
                WaveUpdateFrame(mMouseX, mMouseY, mFingerDown);
                handled = TRUE;
            }
            break;
        case IMotionEvent::ACTION_CANCEL:
            {
                mFingerDown = FALSE;
                handled = TRUE;
            }
            break;
    }

    Invalidate();
    Boolean resTmp;
    *result = handled ? TRUE : View::OnTouchEvent(event, &resTmp);
    return NOERROR;
}

/**
 * Tries to transition to start attempt state.
 *
 * @param event A motion event.
 */
void WaveView::TryTransitionToStartAttemptState(
    /* [in] */ IMotionEvent* event)
{
    Float x1, x2, y1, y2;
    event->GetX(&x1);
    mUnlockHalo->GetX(&x2);

    Float dx = x1 - x2;
    event->GetY(&y1);
    mUnlockHalo->GetY(&y2);
    Float dy = y1 - y2;

    Float dist = (Float) Elastos::Core::Math::Hypot(dx, dy);
    if (dist <= GetScaledGrabHandleRadius()) {
        SetGrabbedState(IWaveViewOnTriggerListener::CENTER_HANDLE);
        if (mLockState == STATE_READY) {
            mLockState = STATE_START_ATTEMPT;

            Boolean enable;
            AutoPtr<IAccessibilityManager> manager;
            CAccessibilityManager::GetInstance(mContext, (IAccessibilityManager**)&manager);
            manager->IsEnabled(&enable);
            if (enable) {
                AnnounceUnlockHandle();
            }
        }
    }
}

/**
 * @return The radius in which the handle is grabbed scaled based on
 *     whether accessibility is enabled.
 */
Float WaveView::GetScaledGrabHandleRadius()
{
    Boolean enable;
    AutoPtr<IAccessibilityManager> manager;
    CAccessibilityManager::GetInstance(mContext, (IAccessibilityManager**)&manager);
    manager->IsEnabled(&enable);

    Int32 width;
    mUnlockHalo->GetWidth(&width);

    if (enable) {
        return GRAB_HANDLE_RADIUS_SCALE_ACCESSIBILITY_ENABLED * width;
    }
    else {
        return GRAB_HANDLE_RADIUS_SCALE_ACCESSIBILITY_DISABLED * width;
    }
}

/**
 * Announces the unlock handle if accessibility is enabled.
 */
void WaveView::AnnounceUnlockHandle()
{
    String description;
    mContext->GetString(R::string::description_target_unlock_tablet, &description);
    AutoPtr<ICharSequence> cDescription;
    CString::New(description, (ICharSequence**)&cDescription);
    SetContentDescription(cDescription);
    SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_FOCUSED);
    SetContentDescription(NULL);
}

/**
 * Triggers haptic feedback.
 */
void WaveView::Vibrate(
    /* [in] */ Int64 duration)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSecure> settings;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settings);
    Int32 resTmp = 0;
    settings->GetInt32ForUser(resolver, ISettingsSystem::HAPTIC_FEEDBACK_ENABLED, 1, IUserHandle::USER_CURRENT, &resTmp);
    Boolean hapticEnabled = resTmp != 0;
    if (hapticEnabled) {
        if (mVibrator == NULL) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IInterface> interfaceTmp;
            context->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&interfaceTmp);
            mVibrator = IVibrator::Probe(interfaceTmp);
        }
        mVibrator->Vibrate(duration, VIBRATION_ATTRIBUTES);
    }
}

/**
 * Registers a callback to be invoked when the user triggers an event.
 *
 * @param listener the OnDialTriggerListener to attach to this view
 */
ECode WaveView::SetOnTriggerListener(
    /* [in] */ IWaveViewOnTriggerListener* listener)
{
    mOnTriggerListener = listener;
    return NOERROR;
}

/**
 * Dispatches a trigger event to listener. Ignored if a listener is not set.
 * @param whichHandle the handle that triggered the event.
 */
void WaveView::DispatchTriggerEvent(
    /* [in] */ Int32 whichHandle)
{
    Vibrate(VIBRATE_LONG);
    if (mOnTriggerListener != NULL) {
        mOnTriggerListener->OnTrigger(IView::Probe(this), whichHandle);
    }
}

/**
 * Sets the current grabbed state, and dispatches a grabbed state change
 * event to our listener.
 */
void WaveView::SetGrabbedState(
    /* [in] */ Int32 newState)
{
    if (newState != mGrabbedState) {
        mGrabbedState = newState;
        if (mOnTriggerListener != NULL) {
            mOnTriggerListener->OnGrabbedStateChange(IView::Probe(this), mGrabbedState);
        }
    }
}

ECode WaveView::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    Invalidate();
    return NOERROR;
}

ECode WaveView::Reset()
{
    if (DBG)
        Logger::V(TAG, "reset() : resets state to STATE_RESET_LOCK");
    mLockState = STATE_RESET_LOCK;
    Invalidate();
    return NOERROR;
}

AutoPtr<IAudioAttributes> WaveView::MiddleInitVibrationAttributes()
{
    AutoPtr<IAudioAttributesBuilder> builder;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&builder);
    builder->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
    builder->SetUsage(IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION);

    AutoPtr<IAudioAttributes> result;
    builder->Build((IAudioAttributes**)&result);
    return result;
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

