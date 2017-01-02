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

#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/incallui/widget/multiwaveview/Ease.h"
#include "elastos/droid/incallui/widget/multiwaveview/GlowPadView.h"
#include "R.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Dialer::R;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ITypedValueHelper;
using Elastos::Droid::Utility::CTypedValueHelper;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::CGravity;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace InCallUI {
namespace Widget {
namespace MultiwaveView {

//=====================================================================
//   GlowPadView::AnimationBundle
//=====================================================================
GlowPadView::AnimationBundle::AnimationBundle()
    : mSuspended(FALSE)
{
    ArrayList::constructor();
}

void GlowPadView::AnimationBundle::Start()
{
    if (mSuspended) return; // ignore attempts to start animations
    Int32 count = 0;
    GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> p;
        Get(i, (IInterface**)&p);
        Tweener* anim = (Tweener*)IObject::Probe(p);
        IAnimator::Probe(anim->mAnimator)->Start();
    }
}

void GlowPadView::AnimationBundle::Cancel()
{
    Int32 count = 0;
    GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> p;
        Get(i, (IInterface**)&p);
        Tweener* anim = (Tweener*)IObject::Probe(p);
        IAnimator::Probe(anim->mAnimator)->Cancel();
    }
    Clear();
}

void GlowPadView::AnimationBundle::Stop()
{
    Int32 count = 0;
    GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> p;
        Get(i, (IInterface**)&p);
        Tweener* anim = (Tweener*)IObject::Probe(p);
        IAnimator::Probe(anim->mAnimator)->End();
    }
    Clear();
}

void GlowPadView::AnimationBundle::SetSuspended(
    /* [in] */ Boolean suspend)
{
    mSuspended = suspend;
}

ECode GlowPadView::AnimationBundle::ToString(
    /* [ou] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("GlowPadView::AnimationBundle@");
    String info;
    ArrayList::ToString(&info);
    sb += info;
    *str = sb.ToString();
    return NOERROR;
}


//=====================================================================
//   GlowPadView::ResetListener
//=====================================================================
ECode GlowPadView::ResetListener::OnAnimationEnd(
    /* [in] */ IAnimator* animator)
{
    mHost->SwitchToState(STATE_IDLE, mHost->mWaveCenterX, mHost->mWaveCenterY);
    mHost->DispatchOnFinishFinalAnimation();
    return NOERROR;
}


//=====================================================================
//   GlowPadView::ResetListenerWithPing
//=====================================================================
ECode GlowPadView::ResetListenerWithPing::OnAnimationEnd(
    /* [in] */ IAnimator* animator)
{
    mHost->Ping();
    mHost->SwitchToState(STATE_IDLE, mHost->mWaveCenterX, mHost->mWaveCenterY);
    mHost->DispatchOnFinishFinalAnimation();
    return NOERROR;
}


//=====================================================================
//   GlowPadView::UpdateListener
//=====================================================================
CAR_INTERFACE_IMPL(GlowPadView::UpdateListener, Object, IAnimatorUpdateListener)

ECode GlowPadView::UpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    mHost->Invalidate();
    return NOERROR;
}


//=====================================================================
//   GlowPadView::TargetUpdateListener
//=====================================================================
ECode GlowPadView::TargetUpdateListener::OnAnimationEnd(
    /* [in] */ IAnimator* animator)
{
    if (mHost->mNewTargetResources != 0) {
        mHost->InternalSetTargetResources(mHost->mNewTargetResources);
        mHost->mNewTargetResources = 0;
        mHost->HideTargets(FALSE, FALSE);
    }
    mHost->mAnimatingTargets = FALSE;
    return NOERROR;
}


//=====================================================================
//   GlowPadView::WaveAnimationListener
//=====================================================================
ECode GlowPadView::WaveAnimationListener::OnAnimationEnd(
    /* [in] */ IAnimator* animator)
{
    mHost->mPointCloud->mWaveManager->SetRadius(0.0f);
    mHost->mPointCloud->mWaveManager->SetAlpha(0.0f);
    return NOERROR;
}


//=====================================================================
//   GlowPadView
//=====================================================================
const String GlowPadView::TAG("GlowPadView");
const Boolean GlowPadView::DEBUG = FALSE;

const Int32 GlowPadView::STATE_IDLE = 0;
const Int32 GlowPadView::STATE_START = 1;
const Int32 GlowPadView::STATE_FIRST_TOUCH = 2;
const Int32 GlowPadView::STATE_TRACKING = 3;
const Int32 GlowPadView::STATE_SNAP = 4;
const Int32 GlowPadView::STATE_FINISH = 5;

const Float GlowPadView::SNAP_MARGIN_DEFAULT = 20.0f; // distance to ring before we snap to it

const Int32 GlowPadView::WAVE_ANIMATION_DURATION = 1350;
const Int32 GlowPadView::RETURN_TO_HOME_DELAY = 1200;
const Int32 GlowPadView::RETURN_TO_HOME_DURATION = 200;
const Int32 GlowPadView::HIDE_ANIMATION_DELAY = 200;
const Int32 GlowPadView::HIDE_ANIMATION_DURATION = 200;
const Int32 GlowPadView::SHOW_ANIMATION_DURATION = 200;
const Int32 GlowPadView::SHOW_ANIMATION_DELAY = 50;
const Int32 GlowPadView::INITIAL_SHOW_HANDLE_DURATION = 200;
const Int32 GlowPadView::REVEAL_GLOW_DELAY = 0;
const Int32 GlowPadView::REVEAL_GLOW_DURATION = 0;

const Float GlowPadView::TAP_RADIUS_SCALE_ACCESSIBILITY_ENABLED = 1.3f;
const Float GlowPadView::TARGET_SCALE_EXPANDED = 1.0f;
const Float GlowPadView::TARGET_SCALE_COLLAPSED = 0.8f;
const Float GlowPadView::RING_SCALE_EXPANDED = 1.0f;
const Float GlowPadView::RING_SCALE_COLLAPSED = 0.5f;

CAR_INTERFACE_IMPL(GlowPadView, View, IGlowPadView)

GlowPadView::GlowPadView()
    : mFeedbackCount(3)
    , mVibrationDuration(0)
    , mGrabbedState(0)
    , mActiveTarget(-1)
    , mGlowRadius(0.0f)
    , mWaveCenterX(0.0f)
    , mWaveCenterY(0.0f)
    , mMaxTargetHeight(0)
    , mMaxTargetWidth(0)
    , mRingScaleFactor(1.0f)
    , mAllowScaling(FALSE)
    , mOuterRadius(0.0f)
    , mSnapMargin(0.0f)
    , mDragging(FALSE)
    , mNewTargetResources(0)
    , mTargetResourceId(0)
    , mTargetDescriptionsResourceId(0)
    , mDirectionDescriptionsResourceId(0)
    , mAlwaysTrackFinger(FALSE)
    , mHorizontalInset(0)
    , mVerticalInset(0)
    , mGravity(IGravity::TOP)
    , mInitialLayout(TRUE)
    , mInnerRadius(0.0f)
    , mPointerId(0)
{
    CArrayList::New((IArrayList**)&mTargetDrawables);
    mWaveAnimations = new AnimationBundle();
    mTargetAnimations = new AnimationBundle();
    mGlowAnimations = new AnimationBundle();

    mResetListener = new ResetListener(this);
    mResetListenerWithPing = new ResetListenerWithPing(this);
    mUpdateListener = new UpdateListener(this);
    mTargetUpdateListener = new TargetUpdateListener(this);
}

ECode GlowPadView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode GlowPadView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(View::constructor(context, attrs));
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::GlowPadView);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    a->GetDimension(R::styleable::GlowPadView_innerRadius, mInnerRadius, &mInnerRadius);
    a->GetDimension(R::styleable::GlowPadView_outerRadius, mOuterRadius, &mOuterRadius);
    a->GetDimension(R::styleable::GlowPadView_snapMargin, mSnapMargin, &mSnapMargin);
    a->GetInt32(R::styleable::GlowPadView_vibrationDuration,
            mVibrationDuration, &mVibrationDuration);
    a->GetInt32(R::styleable::GlowPadView_feedbackCount,
            mFeedbackCount, &mFeedbackCount);
    a->GetBoolean(R::styleable::GlowPadView_allowScaling, FALSE, &mAllowScaling);
    AutoPtr<ITypedValue> handle;
    a->PeekValue(R::styleable::GlowPadView_handleDrawable, (ITypedValue**)&handle);
    Int32 resourceId;
    if (handle != NULL) handle->GetResourceId(&resourceId);
    else resourceId = R::drawable::ic_incall_audio_handle;
    SetHandleDrawable(resourceId);
    CTargetDrawable::NewByFriend(res,
            GetResourceId(a, R::styleable::GlowPadView_outerRingDrawable), 1, (CTargetDrawable**)&mOuterRing);

    a->GetBoolean(R::styleable::GlowPadView_alwaysTrackFinger, FALSE, &mAlwaysTrackFinger);

    Int32 pointId = GetResourceId(a, R::styleable::GlowPadView_pointDrawable);
    AutoPtr<IDrawable> pointDrawable;
    if (pointId != 0) {
        context->GetDrawable(pointId, (IDrawable**)&pointDrawable);
    }
    a->GetDimension(R::styleable::GlowPadView_glowRadius, 0.0f, &mGlowRadius);

    AutoPtr<ITypedValue> outValue;
    CTypedValue::New((ITypedValue**)&outValue);

    // Read array of target drawables
    Boolean b = FALSE;
    a->GetValue(R::styleable::GlowPadView_targetDrawables, outValue, &b);
    if (b) {
        Int32 resID = 0;
        outValue->GetResourceId(&resID);
        InternalSetTargetResources(resID);
    }
    Int32 tSize = 0;
    if (mTargetDrawables == NULL || (mTargetDrawables->GetSize(&tSize), tSize) == 0) {
        // throw new IllegalStateException("Must specify at least one target drawable");
        Logger::E(TAG, "Must specify at least one target drawable");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // Read array of target descriptions
    a->GetValue(R::styleable::GlowPadView_targetDescriptions, outValue, &b);
    if (b) {
        Int32 resourceId = 0;
        outValue->GetResourceId(&resourceId);
        if (resourceId == 0) {
            // throw new IllegalStateException("Must specify target descriptions");
            Logger::E(TAG, "Must specify target descriptions");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        SetTargetDescriptionsResourceId(resourceId);
    }

    // Read array of direction descriptions
    a->GetValue(R::styleable::GlowPadView_directionDescriptions, outValue, &b);
    if (b) {
        Int32 resourceId = 0;
        outValue->GetResourceId(&resourceId);
        if (resourceId == 0) {
            // throw new IllegalStateException("Must specify direction descriptions");
            Logger::E(TAG, "Must specify direction descriptions");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        SetDirectionDescriptionsResourceId(resourceId);
    }

    a->GetInt32(R::styleable::GlowPadView_android_gravity, IGravity::TOP, &mGravity);
    a->Recycle();

    SetVibrateEnabled(mVibrationDuration > 0);

    AssignDefaultsIfNeeded();

    mPointCloud = new PointCloud(pointDrawable);
    mPointCloud->MakePointCloud(mInnerRadius, mOuterRadius);
    mPointCloud->mGlowManager->SetRadius(mGlowRadius);
    return NOERROR;
}

Int32 GlowPadView::GetResourceId(
    /* [in] */ ITypedArray* a,
    /* [in] */ Int32 id)
{
    AutoPtr<ITypedValue> tv;
    a->PeekValue(id, (ITypedValue**)&tv);
    Int32 resId = 0;
    return tv == NULL ? 0 : (tv->GetResourceId(&resId), resId);
}

void GlowPadView::Dump()
{
    Logger::V(TAG, "Outer Radius = %d", mOuterRadius);
    Logger::V(TAG, "SnapMargin = %d", mSnapMargin);
    Logger::V(TAG, "FeedbackCount = %d", mFeedbackCount);
    Logger::V(TAG, "VibrationDuration = %d", mVibrationDuration);
    Logger::V(TAG, "GlowRadius = %d", mGlowRadius);
    Logger::V(TAG, "WaveCenterX = %d", mWaveCenterX);
    Logger::V(TAG, "WaveCenterY = %d", mWaveCenterY);
}

void GlowPadView::SuspendAnimations()
{
    mWaveAnimations->SetSuspended(TRUE);
    mTargetAnimations->SetSuspended(TRUE);
    mGlowAnimations->SetSuspended(TRUE);
}

void GlowPadView::ResumeAnimations()
{
    mWaveAnimations->SetSuspended(FALSE);
    mTargetAnimations->SetSuspended(FALSE);
    mGlowAnimations->SetSuspended(FALSE);
    mWaveAnimations->Start();
    mTargetAnimations->Start();
    mGlowAnimations->Start();
}

Int32 GlowPadView::GetSuggestedMinimumWidth()
{
    // View should be large enough to contain the background + handle and
    // target drawable on either edge.
    ;
    return (Int32) (Elastos::Core::Math::Max((Float)mOuterRing->GetWidth(), 2 * mOuterRadius) + mMaxTargetWidth);
}

Int32 GlowPadView::GetSuggestedMinimumHeight()
{
    // View should be large enough to contain the unlock ring + target and
    // target drawable on either edge
    return (Int32) (Elastos::Core::Math::Max((Float)mOuterRing->GetHeight(), 2 * mOuterRadius) + mMaxTargetHeight);
}

Int32 GlowPadView::GetScaledSuggestedMinimumWidth()
{
    return (Int32) (mRingScaleFactor * Elastos::Core::Math::Max((Float)mOuterRing->GetWidth(), 2 * mOuterRadius)
            + mMaxTargetWidth);
}

Int32 GlowPadView::GetScaledSuggestedMinimumHeight()
{
    return (Int32) (mRingScaleFactor * Elastos::Core::Math::Max((Float)mOuterRing->GetHeight(), 2 * mOuterRadius)
            + mMaxTargetHeight);
}

Int32 GlowPadView::ResolveMeasured(
    /* [in] */ Int32 measureSpec,
    /* [in] */ Int32 desired)
{
    Int32 result = 0;
    Int32 specSize = View::MeasureSpec::GetSize(measureSpec);
    switch (View::MeasureSpec::GetMode(measureSpec)) {
        case View::MeasureSpec::UNSPECIFIED:
            result = desired;
            break;
        case View::MeasureSpec::AT_MOST:
            result = Elastos::Core::Math::Min(specSize, desired);
            break;
        case View::MeasureSpec::EXACTLY:
        default:
            result = specSize;
    }
    return result;
}

ECode GlowPadView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 minimumWidth = GetSuggestedMinimumWidth();
    Int32 minimumHeight = GetSuggestedMinimumHeight();
    Int32 computedWidth = ResolveMeasured(widthMeasureSpec, minimumWidth);
    Int32 computedHeight = ResolveMeasured(heightMeasureSpec, minimumHeight);

    mRingScaleFactor = ComputeScaleFactor(minimumWidth, minimumHeight,
            computedWidth, computedHeight);

    Int32 scaledWidth = GetScaledSuggestedMinimumWidth();
    Int32 scaledHeight = GetScaledSuggestedMinimumHeight();

    ComputeInsets(computedWidth - scaledWidth, computedHeight - scaledHeight);
    SetMeasuredDimension(computedWidth, computedHeight);
    return NOERROR;
}

void GlowPadView::SwitchToState(
    /* [in] */ Int32 state,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    switch (state) {
        case STATE_IDLE: {
            DeactivateTargets();
            HideGlow(0, 0, 0.0f, NULL);
            StartBackgroundAnimation(0, 0.0f);
            mHandleDrawable->SetState(TargetDrawable::STATE_INACTIVE);
            mHandleDrawable->SetAlpha(1.0f);
            break;
        }

        case STATE_START:
            StartBackgroundAnimation(0, 0.0f);
            break;

        case STATE_FIRST_TOUCH: {
            mHandleDrawable->SetAlpha(0.0f);
            DeactivateTargets();
            ShowTargets(TRUE);
            StartBackgroundAnimation(INITIAL_SHOW_HANDLE_DURATION, 1.0f);
            SetGrabbedState(IGlowPadViewOnTriggerListener::CENTER_HANDLE);

            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IInterface> obj;
            context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
            IAccessibilityManager* accessibilityManager = IAccessibilityManager::Probe(obj);
            Boolean enabled = FALSE;
            if (accessibilityManager->IsEnabled(&enabled), enabled) {
                AnnounceTargets();
            }
            break;
        }

        case STATE_TRACKING: {
            mHandleDrawable->SetAlpha(0.0f);
            ShowGlow(REVEAL_GLOW_DURATION , REVEAL_GLOW_DELAY, 1.0f, NULL);
            break;
        }

        case STATE_SNAP: {
            // TODO: Add transition states (see list_selector_background_transition.xml)
            mHandleDrawable->SetAlpha(0.0f);
            ShowGlow(REVEAL_GLOW_DURATION , REVEAL_GLOW_DELAY, 0.0f, NULL);
            break;
        }

        case STATE_FINISH:
            DoFinish();
            break;
    }
}

void GlowPadView::ShowGlow(
    /* [in] */ Int32 duration,
    /* [in] */ Int32 delay,
    /* [in] */ Float finalAlpha,
    /* [in] */ IAnimatorListener* finishListener)
{
    mGlowAnimations->Cancel();
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(10);
    arr->Set(0, CoreUtils::Convert(String("ease")));
    arr->Set(1, (ITimeInterpolator*)Ease::Cubic::sEaseIn);
    arr->Set(2, CoreUtils::Convert(String("delay")));
    arr->Set(3, CoreUtils::Convert(delay));
    arr->Set(4, CoreUtils::Convert(String("alpha")));
    arr->Set(5, CoreUtils::Convert(finalAlpha));
    arr->Set(6, CoreUtils::Convert(String("onUpdate")));
    arr->Set(7, mUpdateListener);
    arr->Set(8, CoreUtils::Convert(String("onComplete")));
    arr->Set(9, finishListener);
    AutoPtr<Tweener> res;
    Tweener::To((IObject*)mPointCloud->mGlowManager, duration, arr, (Tweener**)&res);
    mGlowAnimations->Add((IObject*)res);
    mGlowAnimations->Start();
}

void GlowPadView::HideGlow(
    /* [in] */ Int32 duration,
    /* [in] */ Int32 delay,
    /* [in] */ Float finalAlpha,
    /* [in] */ IAnimatorListener* finishListener)
{
    mGlowAnimations->Cancel();
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(14);
    arr->Set(0, CoreUtils::Convert(String("ease")));
    arr->Set(1, (ITimeInterpolator*)Ease::Quart::sEaseOut);
    arr->Set(2, CoreUtils::Convert(String("delay")));
    arr->Set(3, CoreUtils::Convert(delay));
    arr->Set(4, CoreUtils::Convert(String("alpha")));
    arr->Set(5, CoreUtils::Convert(finalAlpha));
    arr->Set(6, CoreUtils::Convert(String("x")));
    arr->Set(7, CoreUtils::Convert(0.0f));
    arr->Set(8, CoreUtils::Convert(String("y")));
    arr->Set(9, CoreUtils::Convert(0.0f));
    arr->Set(10, CoreUtils::Convert(String("onUpdate")));
    arr->Set(11, mUpdateListener);
    arr->Set(12, CoreUtils::Convert(String("onComplete")));
    arr->Set(13, finishListener);
    AutoPtr<Tweener> res;
    Tweener::To((IObject*)mPointCloud->mGlowManager, duration, arr, (Tweener**)&res);
    mGlowAnimations->Add((IObject*)res);
    mGlowAnimations->Start();
}

void GlowPadView::DeactivateTargets()
{
    Int32 count = 0;
    mTargetDrawables->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> p;
        mTargetDrawables->Get(i, (IInterface**)&p);
        TargetDrawable* target = (TargetDrawable*)IObject::Probe(p);
        target->SetState(TargetDrawable::STATE_INACTIVE);
    }
    mActiveTarget = -1;
}

void GlowPadView::DispatchTriggerEvent(
    /* [in] */ Int32 whichTarget)
{
    Vibrate();
    if (mOnTriggerListener != NULL) {
        mOnTriggerListener->OnTrigger(this, whichTarget);
    }
}

void GlowPadView::DispatchOnFinishFinalAnimation()
{
    if (mOnTriggerListener != NULL) {
        mOnTriggerListener->OnFinishFinalAnimation();
    }
}

void GlowPadView::DoFinish()
{
    Int32 activeTarget = mActiveTarget;
    Boolean targetHit =  activeTarget != -1;

    if (targetHit) {
        if (DEBUG) Logger::V(TAG, "Finish with target hit = %d", targetHit);

        HighlightSelected(activeTarget);

        // Inform listener of any active targets.  Typically only one will be active.
        HideGlow(RETURN_TO_HOME_DURATION, RETURN_TO_HOME_DELAY, 0.0f, mResetListener);
        DispatchTriggerEvent(activeTarget);
        if (!mAlwaysTrackFinger) {
            // Force ring and targets to finish animation to expanded state
            mTargetAnimations->Stop();
        }
    }
    else {
        // Animate handle back to the center based on current state.
        HideGlow(HIDE_ANIMATION_DURATION, 0, 0.0f, mResetListenerWithPing);
        HideTargets(TRUE, FALSE);
    }

    SetGrabbedState(IGlowPadViewOnTriggerListener::NO_HANDLE);
}

void GlowPadView::HighlightSelected(
    /* [in] */ Int32 activeTarget)
{
    // Highlight the given target and fade others
    AutoPtr<IInterface> p;
    mTargetDrawables->Get(activeTarget, (IInterface**)&p);
    ((TargetDrawable*)IObject::Probe(p))->SetState(TargetDrawable::STATE_ACTIVE);
    HideUnselected(activeTarget);
}

void GlowPadView::HideUnselected(
    /* [in] */ Int32 active)
{
    Int32 s = 0;
    mTargetDrawables->GetSize(&s);
    for (Int32 i = 0; i < s; i++) {
        if (i != active) {
            AutoPtr<IInterface> p;
            mTargetDrawables->Get(i, (IInterface**)&p);
            ((TargetDrawable*)IObject::Probe(p))->SetAlpha(0.0f);
        }
    }
}

void GlowPadView::HideTargets(
    /* [in] */ Boolean animate,
    /* [in] */ Boolean expanded)
{
    mTargetAnimations->Cancel();
    // Note: these animations should complete at the same time so that we can swap out
    // the target assets asynchronously from the setTargetResources() call.
    mAnimatingTargets = animate;
    Int32 duration = animate ? HIDE_ANIMATION_DURATION : 0;
    Int32 delay = animate ? HIDE_ANIMATION_DELAY : 0;

    Float targetScale = expanded ?
            TARGET_SCALE_EXPANDED : TARGET_SCALE_COLLAPSED;
    Int32 length = 0;
    mTargetDrawables->GetSize(&length);
    AutoPtr<ITimeInterpolator> interpolator = Ease::Cubic::sEaseOut;
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> p;
        mTargetDrawables->Get(i, (IInterface**)&p);
        TargetDrawable* target = (TargetDrawable*)IObject::Probe(p);
        target->SetState(TargetDrawable::STATE_INACTIVE);
        AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(12);
        arr->Set(0, CoreUtils::Convert(String("ease")));
        arr->Set(1, interpolator);
        arr->Set(2, CoreUtils::Convert(String("alpha")));
        arr->Set(3, CoreUtils::Convert(0.0f));
        arr->Set(4, CoreUtils::Convert(String("scaleX")));
        arr->Set(5, CoreUtils::Convert(targetScale));
        arr->Set(6, CoreUtils::Convert(String("scaleY")));
        arr->Set(7, CoreUtils::Convert(targetScale));
        arr->Set(8, CoreUtils::Convert(String("delay")));
        arr->Set(9, CoreUtils::Convert(delay));
        arr->Set(10, CoreUtils::Convert(String("onUpdate")));
        arr->Set(11, mUpdateListener);
        AutoPtr<Tweener> res;
        Tweener::To((IObject*)target, duration, arr, (Tweener**)&res);
        mTargetAnimations->Add((IObject*)res);
    }

    Float ringScaleTarget = expanded ?
            RING_SCALE_EXPANDED : RING_SCALE_COLLAPSED;
    ringScaleTarget *= mRingScaleFactor;

    AutoPtr<ArrayOf<IInterface*> > arr2 = ArrayOf<IInterface*>::Alloc(14);
    arr2->Set(0, CoreUtils::Convert(String("ease")));
    arr2->Set(1, interpolator);
    arr2->Set(2, CoreUtils::Convert(String("alpha")));
    arr2->Set(3, CoreUtils::Convert(0.0f));
    arr2->Set(4, CoreUtils::Convert(String("scaleX")));
    arr2->Set(5, CoreUtils::Convert(ringScaleTarget));
    arr2->Set(6, CoreUtils::Convert(String("scaleY")));
    arr2->Set(7, CoreUtils::Convert(ringScaleTarget));
    arr2->Set(8, CoreUtils::Convert(String("delay")));
    arr2->Set(9, CoreUtils::Convert(delay));
    arr2->Set(10, CoreUtils::Convert(String("onUpdate")));
    arr2->Set(11, mUpdateListener);
    arr2->Set(12, CoreUtils::Convert(String("onComplete")));
    arr2->Set(13, mTargetUpdateListener);
    AutoPtr<Tweener> res2;
    Tweener::To((IObject*)mOuterRing, duration, arr2, (Tweener**)&res2);
    mTargetAnimations->Add((IObject*)res2);

    mTargetAnimations->Start();
}

void GlowPadView::ShowTargets(
    /* [in] */ Boolean animate)
{
    mTargetAnimations->Stop();
    mAnimatingTargets = animate;
    Int32 delay = animate ? SHOW_ANIMATION_DELAY : 0;
    Int32 duration = animate ? SHOW_ANIMATION_DURATION : 0;
    Int32 length = 0;
    mTargetDrawables->GetSize(&length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> p;
        mTargetDrawables->Get(i, (IInterface**)&p);
        TargetDrawable* target = (TargetDrawable*)IObject::Probe(p);
        target->SetState(TargetDrawable::STATE_INACTIVE);

        AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(12);
        arr->Set(0, CoreUtils::Convert(String("ease")));
        arr->Set(1, (ITimeInterpolator*)Ease::Cubic::sEaseOut);
        arr->Set(2, CoreUtils::Convert(String("alpha")));
        arr->Set(3, CoreUtils::Convert(1.0f));
        arr->Set(4, CoreUtils::Convert(String("scaleX")));
        arr->Set(5, CoreUtils::Convert(1.0f));
        arr->Set(6, CoreUtils::Convert(String("scaleY")));
        arr->Set(7, CoreUtils::Convert(1.0f));
        arr->Set(8, CoreUtils::Convert(String("delay")));
        arr->Set(9, CoreUtils::Convert(delay));
        arr->Set(10, CoreUtils::Convert(String("onUpdate")));
        arr->Set(11, mUpdateListener);
        AutoPtr<Tweener> res;
        Tweener::To((IObject*)target, duration, arr, (Tweener**)&res);
        mTargetAnimations->Add((IObject*)res);
    }
    Float ringScale = mRingScaleFactor * RING_SCALE_EXPANDED;
    AutoPtr<ArrayOf<IInterface*> > arr2 = ArrayOf<IInterface*>::Alloc(14);
    arr2->Set(0, CoreUtils::Convert(String("ease")));
    arr2->Set(1, (ITimeInterpolator*)Ease::Cubic::sEaseOut);
    arr2->Set(2, CoreUtils::Convert(String("alpha")));
    arr2->Set(3, CoreUtils::Convert(1.0f));
    arr2->Set(4, CoreUtils::Convert(String("scaleX")));
    arr2->Set(5, CoreUtils::Convert(ringScale));
    arr2->Set(6, CoreUtils::Convert(String("scaleY")));
    arr2->Set(7, CoreUtils::Convert(ringScale));
    arr2->Set(8, CoreUtils::Convert(String("delay")));
    arr2->Set(9, CoreUtils::Convert(delay));
    arr2->Set(10, CoreUtils::Convert(String("onUpdate")));
    arr2->Set(11, mUpdateListener);
    arr2->Set(12, CoreUtils::Convert(String("onComplete")));
    arr2->Set(13, mTargetUpdateListener);
    AutoPtr<Tweener> res2;
    Tweener::To((IObject*)mOuterRing, duration, arr2, (Tweener**)&res2);
    mTargetAnimations->Add((IObject*)res2);

    mTargetAnimations->Start();
}

void GlowPadView::Vibrate()
{
    if (mVibrator != NULL) {
        mVibrator->Vibrate(mVibrationDuration);
    }
}

AutoPtr<IArrayList> GlowPadView::LoadDrawableArray(
    /* [in] */ Int32 resourceId)
{
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    AutoPtr<IResources> res;
    cxt->GetResources((IResources**)&res);
    AutoPtr<ITypedArray> array;
    res->ObtainTypedArray(resourceId, (ITypedArray**)&array);
    Int32 count = 0;
    array->GetLength(&count);
    AutoPtr<IArrayList> drawables;
    CArrayList::New(count, (IArrayList**)&drawables);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<ITypedValue> value;
        array->PeekValue(i, (ITypedValue**)&value);
        Int32 resId = 0;
        AutoPtr<ITargetDrawable> target;
        CTargetDrawable::New(res, value != NULL ? (value->GetResourceId(&resId), resId) : 0, 3, (ITargetDrawable**)&target);
        drawables->Add(target);
    }
    array->Recycle();
    return drawables;
}

void GlowPadView::InternalSetTargetResources(
    /* [in] */ Int32 resourceId)
{
    AutoPtr<IArrayList> targets = LoadDrawableArray(resourceId);
    mTargetDrawables = targets;
    mTargetResourceId = resourceId;

    Int32 maxWidth = mHandleDrawable->GetWidth();
    Int32 maxHeight = mHandleDrawable->GetHeight();
    Int32 count = 0;
    targets->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> p;
        targets->Get(i, (IInterface**)&p);
        TargetDrawable* target = (TargetDrawable*)IObject::Probe(p);
        maxWidth = Elastos::Core::Math::Max(maxWidth, target->GetWidth());
        maxHeight = Elastos::Core::Math::Max(maxHeight, target->GetHeight());
    }
    if (mMaxTargetWidth != maxWidth || mMaxTargetHeight != maxHeight) {
        mMaxTargetWidth = maxWidth;
        mMaxTargetHeight = maxHeight;
        RequestLayout(); // required to resize layout and call updateTargetPositions()
    }
    else {
        UpdateTargetPositions(mWaveCenterX, mWaveCenterY);
        UpdatePointCloudPosition(mWaveCenterX, mWaveCenterY);
    }
}

void GlowPadView::SetTargetResources(
    /* [in] */ Int32 resourceId)
{
    if (mAnimatingTargets) {
        // postpone this change until we return to the initial state
        mNewTargetResources = resourceId;
    }
    else {
        InternalSetTargetResources(resourceId);
    }
}

Int32 GlowPadView::GetTargetResourceId()
{
    return mTargetResourceId;
}

void GlowPadView::SetHandleDrawable(
    /* [in] */ Int32 resourceId)
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    mHandleDrawable = NULL;
    CTargetDrawable::NewByFriend(res, resourceId, 1, (CTargetDrawable**)&mHandleDrawable);
    mHandleDrawable->SetState(TargetDrawable::STATE_INACTIVE);
}

void GlowPadView::SetTargetDescriptionsResourceId(
    /* [in] */ Int32 resourceId)
{
    mTargetDescriptionsResourceId = resourceId;
    if (mTargetDescriptions != NULL) {
        mTargetDescriptions->Clear();
    }
}

Int32 GlowPadView::GetTargetDescriptionsResourceId()
{
    return mTargetDescriptionsResourceId;
}

void GlowPadView::SetDirectionDescriptionsResourceId(
    /* [in] */ Int32 resourceId)
{
    mDirectionDescriptionsResourceId = resourceId;
    if (mDirectionDescriptions != NULL) {
        mDirectionDescriptions->Clear();
    }
}

Int32 GlowPadView::GetDirectionDescriptionsResourceId()
{
    return mDirectionDescriptionsResourceId;
}

void GlowPadView::SetVibrateEnabled(
    /* [in] */ Boolean enabled)
{
    if (enabled && mVibrator == NULL) {
        AutoPtr<IContext> cxt;
        GetContext((IContext**)&cxt);
        AutoPtr<IInterface> p;
        cxt->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&p);
        mVibrator = IVibrator::Probe(p);
    }
    else {
        mVibrator = NULL;
    }
}

void GlowPadView::Ping()
{
    if (mFeedbackCount > 0) {
        Boolean doWaveAnimation = TRUE;
        AutoPtr<AnimationBundle> waveAnimations = mWaveAnimations;

        // Don't do a wave if there's already one in progress
        Int32 s = 0;
        if (waveAnimations->GetSize(&s), s > 0) {
            AutoPtr<IInterface> p;
            waveAnimations->Get(0, (IInterface**)&p);
            Tweener* realNode = (Tweener*)IObject::Probe(p);
            Boolean running;
            if (IAnimator::Probe(realNode->mAnimator)->IsRunning(&running), running) {
                Int64 t = 0;
                IValueAnimator::Probe(realNode->mAnimator)->GetCurrentPlayTime(&t);
                if (t < WAVE_ANIMATION_DURATION / 2) {
                    doWaveAnimation = FALSE;
                }
            }
        }

        if (doWaveAnimation) {
            StartWaveAnimation();
        }
    }
}

void GlowPadView::StopAndHideWaveAnimation()
{
    mWaveAnimations->Cancel();
    mPointCloud->mWaveManager->SetAlpha(0.0f);
}

void GlowPadView::StartWaveAnimation()
{
    mWaveAnimations->Cancel();
    mPointCloud->mWaveManager->SetAlpha(1.0f);
    mPointCloud->mWaveManager->SetRadius(mHandleDrawable->GetWidth() / 2.0f);
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(10);
    arr->Set(0, CoreUtils::Convert(String("ease")));
    arr->Set(1, (ITimeInterpolator*)Ease::Quad::sEaseOut);
    arr->Set(2, CoreUtils::Convert(String("delay")));
    arr->Set(3, CoreUtils::Convert(0));
    arr->Set(4, CoreUtils::Convert(String("radius")));
    arr->Set(5, CoreUtils::Convert(2.0f * mOuterRadius));
    arr->Set(6, CoreUtils::Convert(String("onUpdate")));
    arr->Set(7, mUpdateListener);
    arr->Set(8, CoreUtils::Convert(String("onComplete")));
    arr->Set(9, (IAnimatorListener*)new WaveAnimationListener(this));
    AutoPtr<Tweener> res;
    Tweener::To((IObject*)mPointCloud->mWaveManager, WAVE_ANIMATION_DURATION, arr, (Tweener**)&res);
    mWaveAnimations->Add((IObject*)res);
    mWaveAnimations->Start();
}

void GlowPadView::Reset(
    /* [in] */ Boolean animate)
{
    mGlowAnimations->Stop();
    mTargetAnimations->Stop();
    StartBackgroundAnimation(0, 0.0f);
    StopAndHideWaveAnimation();
    HideTargets(animate, FALSE);
    HideGlow(0, 0, 0.0f, NULL);
    Tweener::Reset();
}

void GlowPadView::StartBackgroundAnimation(
    /* [in] */ Int32 duration,
    /* [in] */ Float alpha)
{
    AutoPtr<IDrawable> background;
    GetBackground((IDrawable**)&background);
    if (mAlwaysTrackFinger && background != NULL) {
        if (mBackgroundAnimator != NULL) {
            IAnimator::Probe(mBackgroundAnimator->mAnimator)->Cancel();
        }
        AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(6);
        arr->Set(0, CoreUtils::Convert(String("ease")));
        arr->Set(1, (ITimeInterpolator*)Ease::Cubic::sEaseIn);
        arr->Set(2, CoreUtils::Convert(String("alpha")));
        arr->Set(3, CoreUtils::Convert((Int32)(255.0f * alpha)));
        arr->Set(4, CoreUtils::Convert(String("delay")));
        arr->Set(5, CoreUtils::Convert(SHOW_ANIMATION_DELAY));
        mBackgroundAnimator = NULL;
        Tweener::To(background, duration, arr, (Tweener**)&mBackgroundAnimator);
        IAnimator::Probe(mBackgroundAnimator->mAnimator)->Start();
    }
}

ECode GlowPadView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 action = 0;
    event->GetActionMasked(&action);
    Boolean handled = FALSE;
    switch (action) {
        case IMotionEvent::ACTION_POINTER_DOWN:
        case IMotionEvent::ACTION_DOWN:
            if (DEBUG) Logger::V(TAG, "*** DOWN ***");
            HandleDown(event);
            HandleMove(event);
            handled = TRUE;
            break;

        case IMotionEvent::ACTION_MOVE:
            if (DEBUG) Logger::V(TAG, "*** MOVE ***");
            HandleMove(event);
            handled = TRUE;
            break;

        case IMotionEvent::ACTION_POINTER_UP:
        case IMotionEvent::ACTION_UP:
            if (DEBUG) Logger::V(TAG, "*** UP ***");
            HandleMove(event);
            HandleUp(event);
            handled = TRUE;
            break;

        case IMotionEvent::ACTION_CANCEL:
            if (DEBUG) Logger::V(TAG, "*** CANCEL ***");
            HandleMove(event);
            HandleCancel(event);
            handled = TRUE;
            break;

    }
    Invalidate();
    if (handled) {
        *result = TRUE;
        return NOERROR;
    }
    else {
        return View::OnTouchEvent(event, result);
    }
}

void GlowPadView::UpdateGlowPosition(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Float dx = x - mOuterRing->GetX();
    Float dy = y - mOuterRing->GetY();
    dx *= 1.0f / mRingScaleFactor;
    dy *= 1.0f / mRingScaleFactor;
    mPointCloud->mGlowManager->SetX(mOuterRing->GetX() + dx);
    mPointCloud->mGlowManager->SetY(mOuterRing->GetY() + dy);
}

void GlowPadView::HandleDown(
    /* [in] */ IMotionEvent* event)
{
    Int32 actionIndex = 0;
    event->GetActionIndex(&actionIndex);
    Float eventX = 0.0f;
    event->GetX(actionIndex, &eventX);
    Float eventY = 0.0f;
    event->GetY(actionIndex, &eventY);
    SwitchToState(STATE_START, eventX, eventY);
    if (!TrySwitchToFirstTouchState(eventX, eventY)) {
        mDragging = FALSE;
    }
    else {
        event->GetPointerId(actionIndex, &mPointerId);
        UpdateGlowPosition(eventX, eventY);
    }
}

void GlowPadView::HandleUp(
    /* [in] */ IMotionEvent* event)
{
    if (DEBUG && mDragging) {
        Logger::V(TAG, "** Handle RELEASE");
    }
    Int32 actionIndex = 0;
    event->GetActionIndex(&actionIndex);
    Int32 pId = 0;
    event->GetPointerId(actionIndex, &pId);
    if (pId == mPointerId) {
        Float x = 0.0f, y = 0.0f;
        event->GetX(actionIndex, &x);
        event->GetY(actionIndex, &y);
        SwitchToState(STATE_FINISH, x, y);
    }
}

void GlowPadView::HandleCancel(
    /* [in] */ IMotionEvent* event)
{
    if (DEBUG && mDragging) {
        Logger::V(TAG, "** Handle CANCEL");
    }

    // We should drop the active target here but it interferes with
    // moving off the screen in the direction of the navigation bar. At some point we may
    // want to revisit how we handle this. For now we'll allow a canceled event to
    // activate the current target.

    // mActiveTarget = -1; // Drop the active target if canceled.

    Int32 actionIndex = 0;
    event->FindPointerIndex(mPointerId, &actionIndex);
    actionIndex = actionIndex == -1 ? 0 : actionIndex;
    Float aiX = 0.0f, aiY = 0.0f;
    event->GetX(actionIndex, &aiX);
    event->GetY(actionIndex, &aiY);
    SwitchToState(STATE_FINISH, aiX, aiY);
}

void GlowPadView::HandleMove(
    /* [in] */ IMotionEvent* event)
{
    Int32 activeTarget = -1;
    Int32 historySize = 0;
    event->GetHistorySize(&historySize);
    AutoPtr<IArrayList> targets = mTargetDrawables;
    Int32 ntargets = 0;
    targets->GetSize(&ntargets);
    Float x = 0.0f;
    Float y = 0.0f;
    Int32 actionIndex = 0;
    event->FindPointerIndex(mPointerId, &actionIndex);

    if (actionIndex == -1) {
        return;  // no data for this pointer
    }

    for (Int32 k = 0; k < historySize + 1; k++) {
        Float eventX, eventY;
        if (k < historySize) {
            event->GetHistoricalX(actionIndex, k, &eventX);
        }
        else event->GetX(actionIndex, &eventX);
        if (k < historySize) {
            event->GetHistoricalY(actionIndex, k, &eventY);
        }
        else event->GetY(actionIndex, &eventY);
        // tx and ty are relative to wave center
        Float tx = eventX - mWaveCenterX;
        Float ty = eventY - mWaveCenterY;
        Float touchRadius = (Float) Elastos::Core::Math::Sqrt(Dist2(tx, ty));
        Float scale = touchRadius > mOuterRadius ? mOuterRadius / touchRadius : 1.0f;
        Float limitX = tx * scale;
        Float limitY = ty * scale;
        Double angleRad = Elastos::Core::Math::Atan2(-ty, tx);

        if (!mDragging) {
            TrySwitchToFirstTouchState(eventX, eventY);
        }

        if (mDragging) {
            // For multiple targets, snap to the one that matches
            Float snapRadius = mRingScaleFactor * mOuterRadius - mSnapMargin;
            Float snapDistance2 = snapRadius * snapRadius;
            // Find first target in range
            for (Int32 i = 0; i < ntargets; i++) {
                AutoPtr<IInterface> p;
                targets->Get(i, (IInterface**)&p);
                TargetDrawable* target = (TargetDrawable*)IObject::Probe(p);

                Double targetMinRad = (i - 0.5) * 2 * Elastos::Core::Math::PI / ntargets;
                Double targetMaxRad = (i + 0.5) * 2 * Elastos::Core::Math::PI / ntargets;
                if (target->IsEnabled()) {
                    Boolean angleMatches =
                        (angleRad > targetMinRad && angleRad <= targetMaxRad) ||
                        (angleRad + 2 * Elastos::Core::Math::PI > targetMinRad &&
                         angleRad + 2 * Elastos::Core::Math::PI <= targetMaxRad);
                    if (angleMatches && (Dist2(tx, ty) > snapDistance2)) {
                        activeTarget = i;
                    }
                }
            }
        }
        x = limitX;
        y = limitY;
    }

    if (!mDragging) {
        return;
    }

    if (activeTarget != -1) {
        SwitchToState(STATE_SNAP, x,y);
        UpdateGlowPosition(x, y);
    }
    else {
        SwitchToState(STATE_TRACKING, x, y);
        UpdateGlowPosition(x, y);
    }

    if (mActiveTarget != activeTarget) {
        // Defocus the old target
        if (mActiveTarget != -1) {
            AutoPtr<IInterface> p;
            targets->Get(mActiveTarget, (IInterface**)&p);
            TargetDrawable* target = (TargetDrawable*)IObject::Probe(p);
            target->SetState(TargetDrawable::STATE_INACTIVE);
        }
        // Focus the new target
        if (activeTarget != -1) {
            AutoPtr<IInterface> p;
            targets->Get(activeTarget, (IInterface**)&p);
            TargetDrawable* target = (TargetDrawable*)IObject::Probe(p);
            target->SetState(TargetDrawable::STATE_FOCUSED);

            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IInterface> obj;
            context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
            IAccessibilityManager* accessibilityManager = IAccessibilityManager::Probe(obj);
            Boolean enabled = FALSE;
            if (accessibilityManager->IsEnabled(&enabled), enabled) {
                String targetContentDescription = GetTargetDescription(activeTarget);
                AnnounceForAccessibility(CoreUtils::Convert(targetContentDescription));
            }
        }
    }
    mActiveTarget = activeTarget;
}

ECode GlowPadView::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
    IAccessibilityManager* accessibilityManager = IAccessibilityManager::Probe(obj);
    Boolean enabled = FALSE;
    accessibilityManager->IsTouchExplorationEnabled(&enabled);
    if (enabled) {
        Int32 action = 0;
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
        OnTouchEvent(event, result);
        event->SetAction(action);
    }
    View::OnHoverEvent(event, result);
    *result = TRUE;
    return NOERROR;
}

void GlowPadView::SetGrabbedState(
    /* [in] */ Int32 newState)
{
    if (newState != mGrabbedState) {
        if (newState != IGlowPadViewOnTriggerListener::NO_HANDLE) {
            Vibrate();
        }
        mGrabbedState = newState;
        if (mOnTriggerListener != NULL) {
            if (newState == IGlowPadViewOnTriggerListener::NO_HANDLE) {
                mOnTriggerListener->OnReleased(this, IGlowPadViewOnTriggerListener::CENTER_HANDLE);
            }
            else {
                mOnTriggerListener->OnGrabbed(this, IGlowPadViewOnTriggerListener::CENTER_HANDLE);
            }
            mOnTriggerListener->OnGrabbedStateChange(this, newState);
        }
    }
}

Boolean GlowPadView::TrySwitchToFirstTouchState(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Float tx = x - mWaveCenterX;
    Float ty = y - mWaveCenterY;
    if (mAlwaysTrackFinger || Dist2(tx,ty) <= GetScaledGlowRadiusSquared()) {
        if (DEBUG) {
            Logger::V(TAG, "** Handle HIT");
        }
        SwitchToState(STATE_FIRST_TOUCH, x, y);
        UpdateGlowPosition(tx, ty);
        mDragging = TRUE;
        return TRUE;
    }
    return FALSE;
}

void GlowPadView::AssignDefaultsIfNeeded()
{
    if (mOuterRadius == 0.0f) {
        mOuterRadius = Elastos::Core::Math::Max(mOuterRing->GetWidth(), mOuterRing->GetHeight()) / 2.0f;
    }
    if (mSnapMargin == 0.0f) {
        AutoPtr<IContext> cxt;
        GetContext((IContext**)&cxt);
        AutoPtr<IResources> res;
        cxt->GetResources((IResources**)&res);
        AutoPtr<IDisplayMetrics> dm;
        res->GetDisplayMetrics((IDisplayMetrics**)&dm);
        AutoPtr<ITypedValueHelper> hlp;
        CTypedValueHelper::AcquireSingleton((ITypedValueHelper**)&hlp);
        hlp->ApplyDimension(ITypedValue::COMPLEX_UNIT_DIP,
                SNAP_MARGIN_DEFAULT, dm, &mSnapMargin);
    }
    if (mInnerRadius == 0.0f) {
        mInnerRadius = mHandleDrawable->GetWidth() / 10.0f;
    }
}

void GlowPadView::ComputeInsets(
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    Int32 layoutDirection = 0;
    GetLayoutDirection(&layoutDirection);
    AutoPtr<IGravity> gra;
    CGravity::AcquireSingleton((IGravity**)&gra);
    Int32 absoluteGravity = 0;
    gra->GetAbsoluteGravity(mGravity, layoutDirection, &absoluteGravity);

    switch (absoluteGravity & IGravity::HORIZONTAL_GRAVITY_MASK) {
        case IGravity::LEFT:
            mHorizontalInset = 0;
            break;
        case IGravity::RIGHT:
            mHorizontalInset = dx;
            break;
        case IGravity::CENTER_HORIZONTAL:
        default:
            mHorizontalInset = dx / 2;
            break;
    }
    switch (absoluteGravity & IGravity::VERTICAL_GRAVITY_MASK) {
        case IGravity::TOP:
            mVerticalInset = 0;
            break;
        case IGravity::BOTTOM:
            mVerticalInset = dy;
            break;
        case IGravity::CENTER_VERTICAL:
        default:
            mVerticalInset = dy / 2;
            break;
    }
}

Float GlowPadView::ComputeScaleFactor(
    /* [in] */ Int32 desiredWidth,
    /* [in] */ Int32 desiredHeight,
    /* [in] */ Int32 actualWidth,
    /* [in] */ Int32 actualHeight)
{
    // Return unity if scaling is not allowed.
    if (!mAllowScaling) return 1.0f;

    Int32 layoutDirection = 0;
    GetLayoutDirection(&layoutDirection);
    AutoPtr<IGravity> gra;
    CGravity::AcquireSingleton((IGravity**)&gra);
    Int32 absoluteGravity = 0;
    gra->GetAbsoluteGravity(mGravity, layoutDirection, &absoluteGravity);

    Float scaleX = 1.0f;
    Float scaleY = 1.0f;

    // We use the gravity as a cue for whether we want to scale on a particular axis.
    // We only scale to fit horizontally if we're not pinned to the left or right. Likewise,
    // we only scale to fit vertically if we're not pinned to the top or bottom. In these
    // cases, we want the ring to hang off the side or top/bottom, respectively.
    switch (absoluteGravity & IGravity::HORIZONTAL_GRAVITY_MASK) {
        case IGravity::LEFT:
        case IGravity::RIGHT:
            break;
        case IGravity::CENTER_HORIZONTAL:
        default:
            if (desiredWidth > actualWidth) {
                scaleX = (1.0f * actualWidth - mMaxTargetWidth) /
                        (desiredWidth - mMaxTargetWidth);
            }
            break;
    }
    switch (absoluteGravity & IGravity::VERTICAL_GRAVITY_MASK) {
        case IGravity::TOP:
        case IGravity::BOTTOM:
            break;
        case IGravity::CENTER_VERTICAL:
        default:
            if (desiredHeight > actualHeight) {
                scaleY = (1.0f * actualHeight - mMaxTargetHeight) /
                        (desiredHeight - mMaxTargetHeight);
            }
            break;
    }
    return Elastos::Core::Math::Min(scaleX, scaleY);
}

Float GlowPadView::GetRingWidth()
{
    return mRingScaleFactor * Elastos::Core::Math::Max((Float) mOuterRing->GetWidth(), 2 * mOuterRadius);
}

Float GlowPadView::GetRingHeight()
{
    return mRingScaleFactor * Elastos::Core::Math::Max((Float) mOuterRing->GetHeight(), 2 * mOuterRadius);
}

ECode GlowPadView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    FAIL_RETURN(View::OnLayout(changed, left, top, right, bottom));

    // Target placement width/height. This puts the targets on the greater of the ring
    // width or the specified outer radius.
    Float placementWidth = GetRingWidth();
    Float placementHeight = GetRingHeight();
    Float newWaveCenterX = mHorizontalInset
            + (mMaxTargetWidth + placementWidth) / 2;
    Float newWaveCenterY = mVerticalInset
            + (mMaxTargetHeight + placementHeight) / 2;

    if (mInitialLayout) {
        StopAndHideWaveAnimation();
        HideTargets(FALSE, FALSE);
        mInitialLayout = FALSE;
    }

    mOuterRing->SetPositionX(newWaveCenterX);
    mOuterRing->SetPositionY(newWaveCenterY);

    mPointCloud->SetScale(mRingScaleFactor);

    mHandleDrawable->SetPositionX(newWaveCenterX);
    mHandleDrawable->SetPositionY(newWaveCenterY);

    UpdateTargetPositions(newWaveCenterX, newWaveCenterY);
    UpdatePointCloudPosition(newWaveCenterX, newWaveCenterY);
    UpdateGlowPosition(newWaveCenterX, newWaveCenterY);

    mWaveCenterX = newWaveCenterX;
    mWaveCenterY = newWaveCenterY;

    if (DEBUG) {
        Dump();
    }
    return NOERROR;
}

void GlowPadView::UpdateTargetPositions(
    /* [in] */ Float centerX,
    /* [in] */ Float centerY)
{
    // Reposition the target drawables if the view changed.
    AutoPtr<IArrayList> targets = mTargetDrawables;
    Int32 size;
    targets->GetSize(&size);
    Float alpha = (Float) (-2.0f * Elastos::Core::Math::PI / size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        targets->Get(i, (IInterface**)&obj);
        TargetDrawable* targetIcon = (TargetDrawable*)IObject::Probe(obj);
        Float angle = alpha * i;
        targetIcon->SetPositionX(centerX);
        targetIcon->SetPositionY(centerY);
        targetIcon->SetX(GetRingWidth() / 2 * (Float) Elastos::Core::Math::Cos(angle));
        targetIcon->SetY(GetRingHeight() / 2 * (Float) Elastos::Core::Math::Sin(angle));
    }
}

void GlowPadView::UpdatePointCloudPosition(
    /* [in] */ Float centerX,
    /* [in] */ Float centerY)
{
    mPointCloud->SetCenter(centerX, centerY);
}

void GlowPadView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    mPointCloud->Draw(canvas);
    mOuterRing->Draw(canvas);
    Int32 ntargets = 0;
    mTargetDrawables->GetSize(&ntargets);
    for (Int32 i = 0; i < ntargets; i++) {
        AutoPtr<IInterface> p;
        mTargetDrawables->Get(i, (IInterface**)&p);
        TargetDrawable* target = (TargetDrawable*)IObject::Probe(p);
        if (target != NULL) {
            target->Draw(canvas);
        }
    }
    mHandleDrawable->Draw(canvas);
}

void GlowPadView::SetOnTriggerListener(
    /* [in] */ IGlowPadViewOnTriggerListener* listener)
{
    mOnTriggerListener = listener;
}

Float GlowPadView::Square(
    /* [in] */ Float d)
{
    return d * d;
}

Float GlowPadView::Dist2(
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    return dx*dx + dy*dy;
}

Float GlowPadView::GetScaledGlowRadiusSquared()
{
    Float scaledTapRadius = 0.0f;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
    IAccessibilityManager* accessibilityManager = IAccessibilityManager::Probe(obj);
    Boolean enabled = FALSE;
    accessibilityManager->IsEnabled(&enabled);
    if (enabled) {
        scaledTapRadius = TAP_RADIUS_SCALE_ACCESSIBILITY_ENABLED * mGlowRadius;
    }
    else {
        scaledTapRadius = mGlowRadius;
    }
    return Square(scaledTapRadius);
}

void GlowPadView::AnnounceTargets()
{
    StringBuilder utterance;
    Int32 targetCount = 0;
    mTargetDrawables->GetSize(&targetCount);
    for (Int32 i = 0; i < targetCount; i++) {
        String targetDescription = GetTargetDescription(i);
        String directionDescription = GetDirectionDescription(i);
        if (!TextUtils::IsEmpty(targetDescription)
                && !TextUtils::IsEmpty(directionDescription)) {
            String text;
            text.AppendFormat(directionDescription.string(), targetDescription.string());
            utterance.Append(text);
        }
    }
    if (utterance.GetLength() > 0) {
        AnnounceForAccessibility(CoreUtils::Convert(utterance.ToString()));
    }
}

String GlowPadView::GetTargetDescription(
    /* [in] */ Int32 index)
{
    Boolean empty = FALSE;
    if (mTargetDescriptions == NULL || (mTargetDescriptions->IsEmpty(&empty), empty)) {
        mTargetDescriptions = LoadDescriptions(mTargetDescriptionsResourceId);
        Int32 s1 = 0, s2 = 0;
        mTargetDrawables->GetSize(&s1);
        mTargetDescriptions->GetSize(&s2);
        if (s1 != s2) {
            Logger::V(TAG, "The number of target drawables must be"
                        " equal to the number of target descriptions.");
            return String(NULL);
        }
    }
    AutoPtr<IInterface> p;
    mTargetDescriptions->Get(index, (IInterface**)&p);
    String str;
    ICharSequence::Probe(p)->ToString(&str);
    return str;
}

String GlowPadView::GetDirectionDescription(
    /* [in] */ Int32 index)
{
    Boolean empty = FALSE;
    if (mDirectionDescriptions == NULL || (mDirectionDescriptions->IsEmpty(&empty), empty)) {
        mDirectionDescriptions = LoadDescriptions(mDirectionDescriptionsResourceId);
        Int32 s1 = 0, s2 = 0;
        mTargetDrawables->GetSize(&s1);
        mDirectionDescriptions->GetSize(&s2);
        if (s1 != s2) {
            Logger::V(TAG, "The number of target drawables must be"
                        " equal to the number of direction descriptions.");
            return String(NULL);
        }
    }
    AutoPtr<IInterface> p;
    mDirectionDescriptions->Get(index, (IInterface**)&p);
    String str;
    ICharSequence::Probe(p)->ToString(&str);
    return str;
}

AutoPtr<IArrayList> GlowPadView::LoadDescriptions(
    /* [in] */ Int32 resourceId)
{
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    AutoPtr<IResources> res;
    cxt->GetResources((IResources**)&res);
    AutoPtr<ITypedArray> array;
    res->ObtainTypedArray(resourceId, (ITypedArray**)&array);
    Int32 count = 0;
    array->GetLength(&count);
    AutoPtr<IArrayList> targetContentDescriptions;
    CArrayList::New(count, (IArrayList**)&targetContentDescriptions);
    for (Int32 i = 0; i < count; i++) {
        String contentDescription;
        array->GetString(i, &contentDescription);
        targetContentDescriptions->Add(CoreUtils::Convert(contentDescription));
    }
    array->Recycle();
    return targetContentDescriptions;
}

Int32 GlowPadView::GetResourceIdForTarget(
    /* [in] */ Int32 index)
{
    AutoPtr<IInterface> p;
    mTargetDrawables->Get(index, (IInterface**)&p);
    TargetDrawable* drawable = (TargetDrawable*)IObject::Probe(p);
    return drawable == NULL ? 0 : drawable->GetResourceId();
}

void GlowPadView::SetEnableTarget(
    /* [in] */ Int32 resourceId,
    /* [in] */ Boolean enabled)
{
    Int32 s = 0;
    mTargetDrawables->GetSize(&s);
    for (Int32 i = 0; i < s; i++) {
        AutoPtr<IInterface> p;
        mTargetDrawables->Get(i, (IInterface**)&p);
        TargetDrawable* target = (TargetDrawable*)IObject::Probe(p);
        if (target->GetResourceId() == resourceId) {
            target->SetEnabled(enabled);
            break; // should never be more than one match
        }
    }
}

Int32 GlowPadView::GetTargetPosition(
    /* [in] */ Int32 resourceId)
{
    Int32 s = 0;
    mTargetDrawables->GetSize(&s);
    for (Int32 i = 0; i < s; i++) {
        AutoPtr<IInterface> p;
        mTargetDrawables->Get(i, (IInterface**)&p);
        TargetDrawable* target = (TargetDrawable*)IObject::Probe(p);
        if (target->GetResourceId() == resourceId) {
            return i; // should never be more than one match
        }
    }
    return -1;
}

Boolean GlowPadView::ReplaceTargetDrawables(
    /* [in] */ IResources* res,
    /* [in] */ Int32 existingResourceId,
    /* [in] */ Int32 newResourceId)
{
    if (existingResourceId == 0 || newResourceId == 0) {
        return FALSE;
    }

    Boolean result = FALSE;
    AutoPtr<IArrayList> drawables = mTargetDrawables;
    Int32 size = 0;
    drawables->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> p;
        mTargetDrawables->Get(i, (IInterface**)&p);
        TargetDrawable* target = (TargetDrawable*)IObject::Probe(p);
        if (target != NULL && (target->GetResourceId() == existingResourceId)) {
            target->SetDrawable(res, newResourceId);
            result = TRUE;
        }
    }

    if (result) {
        RequestLayout(); // in case any given drawable's size changes
    }

    return result;
}

Boolean GlowPadView::ReplaceTargetDrawablesIfPresent(
    /* [in] */ IComponentName* component,
    /* [in] */ const String& name,
    /* [in] */ Int32 existingResId)
{
    if (existingResId == 0) {
        return FALSE;
    }

    Boolean replaced = FALSE;
    if (component != NULL) {
        // try {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IPackageManager> packageManager;
        context->GetPackageManager((IPackageManager**)&packageManager);
        // Look for the search icon specified in the activity meta-data
        AutoPtr<IActivityInfo> ainfo;
        packageManager->GetActivityInfo(
                component, IPackageManager::GET_META_DATA, (IActivityInfo**)&ainfo);
        AutoPtr<IBundle> metaData;
        IPackageItemInfo::Probe(ainfo)->GetMetaData((IBundle**)&metaData);
        if (metaData != NULL) {
            Int32 iconResId = 0;
            metaData->GetInt32(name, &iconResId);
            if (iconResId != 0) {
                AutoPtr<IResources> res;
                packageManager->GetResourcesForActivity(component, (IResources**)&res);
                replaced = ReplaceTargetDrawables(res, existingResId, iconResId);
            }
        }
        // } catch (NameNotFoundException e) {
        //     Log.w(TAG, "Failed to swap drawable; "
        //             + component.flattenToShortString() + " not found", e);
        // } catch (Resources.NotFoundException nfe) {
        //     Log.w(TAG, "Failed to swap drawable from "
        //             + component.flattenToShortString(), nfe);
        // }
    }
    if (!replaced) {
        // Restore the original drawable
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        ReplaceTargetDrawables(res, existingResId, existingResId);
    }
    return replaced;
}

} // namespace MultiwaveView
} // namespace Widget
} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
