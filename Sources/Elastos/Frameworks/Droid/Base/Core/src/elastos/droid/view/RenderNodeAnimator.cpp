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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/view/RenderNodeAnimator.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/view/ViewPropertyAnimator.h"
#include "elastos/droid/view/GLES20RecordingCanvas.h"
#include "elastos/droid/internal/view/animation/FallbackLUTInterpolator.h"
#include "elastos/droid/animation/ValueAnimator.h"
#include <elastos/utility/logging/Slogger.h>
#include <hwui/Animator.h>

using Elastos::Droid::Animation::ValueAnimator;
using Elastos::Droid::Animation::EIID_IAnimator;
using Elastos::Droid::Internal::View::Animation::FallbackLUTInterpolator;
using Elastos::Droid::Internal::View::Animation::INativeInterpolatorFactory;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Slogger;
using android::uirenderer::RenderPropertyAnimator;
using android::uirenderer::BaseRenderNodeAnimator;
using android::uirenderer::CanvasPropertyPrimitive;
using android::uirenderer::CanvasPropertyPrimitiveAnimator;
using android::uirenderer::CanvasPropertyPaint;
using android::uirenderer::CanvasPropertyPaintAnimator;
using android::uirenderer::RevealAnimator;
using android::uirenderer::Interpolator;
using android::uirenderer::AnimationListener;

namespace Elastos {
namespace Droid {
namespace View {

HashMap<Int32, Int32> RenderNodeAnimator::sViewPropertyAnimatorMap(15);
pthread_key_t RenderNodeAnimator::sAnimationHelper;

void ThreadDestructor(void* st)
{
    RenderNodeAnimator::DelayedAnimationHelper* handler = static_cast<RenderNodeAnimator::DelayedAnimationHelper*>(st);
    if (handler) {
        handler->Release();
    }
}

Boolean InitTLS()
{
    Int32 UNUSED(result) = pthread_key_create(&RenderNodeAnimator::sAnimationHelper, ThreadDestructor);
    assert(result == 0);
    return TRUE;
}

Boolean RenderNodeAnimator::InitHashMap()
{
    sViewPropertyAnimatorMap[ViewPropertyAnimator::TRANSLATION_X] = IRenderNodeAnimator::TRANSLATION_X;
    sViewPropertyAnimatorMap[ViewPropertyAnimator::TRANSLATION_Y] = IRenderNodeAnimator::TRANSLATION_Y;
    sViewPropertyAnimatorMap[ViewPropertyAnimator::TRANSLATION_Z] = IRenderNodeAnimator::TRANSLATION_Z;
    sViewPropertyAnimatorMap[ViewPropertyAnimator::SCALE_X] = IRenderNodeAnimator::SCALE_X;
    sViewPropertyAnimatorMap[ViewPropertyAnimator::SCALE_Y] = IRenderNodeAnimator::SCALE_Y;
    sViewPropertyAnimatorMap[ViewPropertyAnimator::ROTATION] = IRenderNodeAnimator::ROTATION;
    sViewPropertyAnimatorMap[ViewPropertyAnimator::ROTATION_X] = IRenderNodeAnimator::ROTATION_X;
    sViewPropertyAnimatorMap[ViewPropertyAnimator::ROTATION_Y] = IRenderNodeAnimator::ROTATION_Y;
    sViewPropertyAnimatorMap[ViewPropertyAnimator::_X] = IRenderNodeAnimator::X;
    sViewPropertyAnimatorMap[ViewPropertyAnimator::_Y] = IRenderNodeAnimator::Y;
    sViewPropertyAnimatorMap[ViewPropertyAnimator::_Z] = IRenderNodeAnimator::Z;
    sViewPropertyAnimatorMap[ViewPropertyAnimator::ALPHA] = IRenderNodeAnimator::ALPHA;
    return TRUE;
}

Boolean RenderNodeAnimator::sHaveKey = InitTLS();
Boolean RenderNodeAnimator::sHasInitMap = RenderNodeAnimator::InitHashMap();

CAR_INTERFACE_IMPL(RenderNodeAnimator, Animator, IRenderNodeAnimator)
CAR_INTERFACE_IMPL(RenderNodeAnimator::DelayedAnimationHelper, Object, IRunnable)

RenderNodeAnimator::DelayedAnimationHelper::DelayedAnimationHelper()
{
    mChoreographer = Choreographer::GetInstance();
}

ECode RenderNodeAnimator::DelayedAnimationHelper::AddDelayedAnimation(
    /* [in] */ IRenderNodeAnimator* animator)
{
    mDelayedAnims.PushBack(animator);
    return ScheduleCallback();
}

ECode RenderNodeAnimator::DelayedAnimationHelper::RemoveDelayedAnimation(
    /* [in] */ IRenderNodeAnimator* animator)
{
    mDelayedAnims.Remove(animator);
    return NOERROR;
}

ECode RenderNodeAnimator::DelayedAnimationHelper::Run()
{
    Int64 frameTimeMs;
    mChoreographer->GetFrameTime(&frameTimeMs);
    mCallbackScheduled = FALSE;

    List<AutoPtr<IRenderNodeAnimator> >::Iterator end = mDelayedAnims.Begin();
    List<AutoPtr<IRenderNodeAnimator> >::Iterator it = mDelayedAnims.Begin();
    for (; it != mDelayedAnims.End(); it++) {
        AutoPtr<IRenderNodeAnimator> animator = *it;
        if (!((RenderNodeAnimator*)animator.Get())->ProcessDelayed(frameTimeMs)) {
            if (end != it) {
                *end = animator;
            }
            end++;
        }
    }
    while (mDelayedAnims.End() != end) {
        List<AutoPtr<IRenderNodeAnimator> >::Iterator tmp = --mDelayedAnims.End();
        mDelayedAnims.Remove(*tmp);
    }

    if (mDelayedAnims.GetSize() > 0) {
        ScheduleCallback();
    }
    return NOERROR;
}

ECode RenderNodeAnimator::DelayedAnimationHelper::ScheduleCallback()
{
    if (!mCallbackScheduled) {
        mCallbackScheduled = TRUE;
        mChoreographer->PostCallback(IChoreographer::CALLBACK_ANIMATION, this, NULL);
    }
    return NOERROR;
}

class AnimationListenerBridge : public AnimationListener {
public:
    // This holds a strong reference to a Java WeakReference<T> object. This avoids
    // cyclic-references-of-doom. If you think "I know, just use NewWeakGlobalRef!"
    // then you end up with basically a PhantomReference, which is totally not
    // what we want.
    AnimationListenerBridge(
        /* [in] */ IRenderNodeAnimator* finishListener)
    {
        mFinishListener = finishListener;
    }

    virtual ~AnimationListenerBridge()
    {
        if (mFinishListener) {
            onAnimationFinished(NULL);
        }
    }

    virtual void onAnimationFinished(
        /* [in] */ BaseRenderNodeAnimator*)
    {
        // LOG_ALWAYS_FATAL_IF(!mFinishListener, "Finished listener twice?");
        if (!mFinishListener) {
            Slogger::E("RenderNodeAnimator", "Finished listener twice?");
            assert(0);
        }
        RenderNodeAnimator::CallOnFinished(mFinishListener);
        release();
    }

private:
    void release() {
        mFinishListener = NULL;
    }

    AutoPtr<IRenderNodeAnimator> mFinishListener;
};

RenderNodeAnimator::RenderNodeAnimator()
    : mRenderProperty(-1)
    , mFinalValue(0.f)
    , mState(STATE_PREPARE)
    , mUnscaledDuration(300)
    , mUnscaledStartDelay(0)
    , mUiThreadHandlesDelay(FALSE)
    , mStartDelay(0)
    , mStartTime(0)
{}

RenderNodeAnimator::~RenderNodeAnimator()
{
}

ECode RenderNodeAnimator::constructor(
    /* [in] */ Int32 property,
    /* [in] */ Float finalValue)
{
    mRenderProperty = property;
    mFinalValue = finalValue;
    mUiThreadHandlesDelay = TRUE;
    Init(nCreateAnimator(property, finalValue));
    return NOERROR;
}

ECode RenderNodeAnimator::constructor(
    /* [in] */ ICanvasProperty* property,
    /* [in] */ Float finalValue)
{
    Int64 ptr;
    property->GetNativeContainer(&ptr);
    Init(nCreateCanvasPropertyFloatAnimator(ptr, finalValue));
    mUiThreadHandlesDelay = FALSE;
    return NOERROR;
}

/**
 * Creates a new render node animator for a field on a Paint property.
 *
 * @param property The paint property to target
 * @param paintField Paint field to animate, one of {@link #PAINT_ALPHA} or
 *            {@link #PAINT_STROKE_WIDTH}
 * @param finalValue The target value for the property
 */
ECode RenderNodeAnimator::constructor(
    /* [in] */ ICanvasProperty* property,
    /* [in] */ Int32 paintField,
    /* [in] */ Float finalValue)
{
    Int64 ptr;
    property->GetNativeContainer(&ptr);
    Init(nCreateCanvasPropertyPaintAnimator(
            ptr, paintField, finalValue));
    mUiThreadHandlesDelay = FALSE;
    return NOERROR;
}

ECode RenderNodeAnimator::constructor(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Float startRadius,
    /* [in] */ Float endRadius)
{
    Init(nCreateRevealAnimator(x, y, startRadius, endRadius));
    mUiThreadHandlesDelay = FALSE;
    return NOERROR;
}

ECode RenderNodeAnimator::Start()
{
    if (mTarget == NULL) {
        Slogger::E("RenderNodeAnimator", "Missing target!");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (mState != STATE_PREPARE) {
        Slogger::E("RenderNodeAnimator", "Already started!");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mState = STATE_DELAYED;
    ApplyInterpolator();

    if (mStartDelay <= 0 || !mUiThreadHandlesDelay) {
        nSetStartDelay(mNativePtr->get(), mStartDelay);
        DoStart();
    } else {
        GetHelper()->AddDelayedAnimation(this);
    }
    return NOERROR;
}

ECode RenderNodeAnimator::Cancel()
{
    if (mState != STATE_FINISHED) {
        if (mState == STATE_DELAYED) {
            GetHelper()->RemoveDelayedAnimation(this);
            NotifyStartListeners();
        }
        nEnd(mNativePtr->get());

        AutoPtr<IArrayList> listeners = CloneListeners();
        Int32 numListeners = 0;
        if (listeners != NULL) {
            listeners->GetSize(&numListeners);
        }
        for (Int32 i = 0; i < numListeners; i++) {
            AutoPtr<IInterface> obj;
            listeners->Get(i, (IInterface**)&obj);
            IAnimatorListener::Probe(obj)->OnAnimationCancel(this);
        }

        if (mViewTarget != NULL) {
            // Kick off a frame to flush the state change
            ((View*)mViewTarget.Get())->InvalidateViewProperty(TRUE, FALSE);
        }
    }
    return NOERROR;
}

ECode RenderNodeAnimator::End()
{
    if (mState != STATE_FINISHED) {
        nEnd(mNativePtr->get());
    }
    return NOERROR;
}

ECode RenderNodeAnimator::Pause()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RenderNodeAnimator::Resume()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RenderNodeAnimator::SetTarget(
    /* [in] */ IView* view)
{
    mViewTarget = view;
    return SetTarget(((View*)view)->mRenderNode);
}

ECode RenderNodeAnimator::SetTarget(
    /* [in] */ ICanvas* canvas)
{
    if (IGLES20RecordingCanvas::Probe(canvas) == NULL) {
        Slogger::E("RenderNodeAnimator", "Not a GLES20RecordingCanvas");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    GLES20RecordingCanvas* recordingCanvas = (GLES20RecordingCanvas*)canvas;
    return SetTarget(recordingCanvas->mNode);
}

ECode RenderNodeAnimator::SetTarget(
    /* [in] */ IInterface* node)
{
    if (mTarget != NULL) {
        Slogger::E("RenderNodeAnimator", "Target already set!");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (ICanvas::Probe(node) != NULL) {
        return SetTarget(ICanvas::Probe(node));
    }
    else if (IView::Probe(node) != NULL) {
        return SetTarget(IView::Probe(node));
    }

    return SetTarget(IRenderNode::Probe(node));
}

ECode RenderNodeAnimator::SetTarget(
    /* [in] */ IRenderNode* node)
{
    assert(node != NULL);
    mTarget = node;
    return mTarget->AddAnimator(this);
}

ECode RenderNodeAnimator::SetStartValue(
    /* [in] */ Float startValue)
{
    FAIL_RETURN(CheckMutable())
    nSetStartValue(mNativePtr->get(), startValue);
    return NOERROR;
}

ECode RenderNodeAnimator::SetStartDelay(
    /* [in] */ Int64 startDelay)
{
    FAIL_RETURN(CheckMutable())
    if (startDelay < 0) {
        Slogger::E("RenderNodeAnimator", "startDelay must be positive; %lld", startDelay);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mUnscaledStartDelay = startDelay;
    mStartDelay = (Int64) (ValueAnimator::GetDurationScale() * startDelay);
    return NOERROR;
}

ECode RenderNodeAnimator::GetStartDelay(
    /* [out] */ Int64* startDelay)
{
    VALIDATE_NOT_NULL(startDelay)

    *startDelay = mUnscaledStartDelay;
    return NOERROR;
}

ECode RenderNodeAnimator::SetDuration(
    /* [in] */ Int64 duration)
{
    FAIL_RETURN(CheckMutable())
    if (duration < 0) {
        Slogger::E("RenderNodeAnimator", "duration must be positive; %lld", duration);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mUnscaledDuration = duration;
    nSetDuration(mNativePtr->get(), (Int64) (duration * ValueAnimator::GetDurationScale()));
    return NOERROR;
}

ECode RenderNodeAnimator::GetDuration(
    /* [out] */ Int64* duration)
{
    VALIDATE_NOT_NULL(duration)

    *duration = mUnscaledDuration;
    return NOERROR;
}

ECode RenderNodeAnimator::IsRunning(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mState == STATE_DELAYED || mState == STATE_RUNNING;
    return NOERROR;
}

ECode RenderNodeAnimator::IsStarted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mState != STATE_PREPARE;
    return NOERROR;
}

ECode RenderNodeAnimator::SetInterpolator(
    /* [in] */ ITimeInterpolator* interpolator)
{
    FAIL_RETURN(CheckMutable())
    mInterpolator = interpolator;
    return NOERROR;
}

ECode RenderNodeAnimator::GetInterpolator(
    /* [out] */ ITimeInterpolator** interpolator)
{
    VALIDATE_NOT_NULL(interpolator)

    *interpolator = mInterpolator;
    REFCOUNT_ADD(*interpolator)
    return NOERROR;
}

ECode RenderNodeAnimator::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    Slogger::E("RenderNodeAnimator", "Cannot clone this animator");
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode RenderNodeAnimator::GetNativeAnimator(
    /* [out] */ Int64* ptr)
{
    VALIDATE_NOT_NULL(ptr)
    *ptr = mNativePtr->get();
    return NOERROR;
}

ECode RenderNodeAnimator::SetAllowRunningAsynchronously(
    /* [in] */ Boolean mayRunAsync)
{
    FAIL_RETURN(CheckMutable())
    nSetAllowRunningAsync(mNativePtr->get(), mayRunAsync);
    return NOERROR;
}

Int32 RenderNodeAnimator::MapViewPropertyToRenderProperty(
    /* [in] */ Int32 viewProperty)
{
    HashMap<Int32, Int32>::Iterator it = sViewPropertyAnimatorMap.Find(viewProperty);
    if (it != sViewPropertyAnimatorMap.End()) {
        return it->mSecond;
    }
    return -1;
}

Boolean RenderNodeAnimator::IsNativeInterpolator(
    /* [in] */ ITimeInterpolator* interpolator)
{
    Boolean res;
    interpolator->HasNativeInterpolator(&res);
    return res;
}

void RenderNodeAnimator::OnFinished()
{
    if (mState == STATE_DELAYED) {
        GetHelper()->RemoveDelayedAnimation(this);
        NotifyStartListeners();
    }
    mState = STATE_FINISHED;

    AutoPtr<IArrayList> listeners = CloneListeners();
    Int32 numListeners = 0;
    if (listeners != NULL) {
        listeners->GetSize(&numListeners);
    }
    for (Int32 i = 0; i < numListeners; i++) {
        AutoPtr<IInterface> obj;
        listeners->Get(i, (IInterface**)&obj);
        IAnimatorListener::Probe(obj)->OnAnimationEnd(this);
    }

    // Release the native object, as it has a global reference to us. This
    // breaks the cyclic reference chain, and allows this object to be
    // GC'd
    mNativePtr = NULL;
}

Int64 RenderNodeAnimator::GetNativeAnimator()
{
    return mNativePtr->get();
}

void RenderNodeAnimator::Init(
    /* [in] */ Int64 ptr)
{
    mNativePtr = new VirtualRefBasePtr(ptr);
}

ECode RenderNodeAnimator::CheckMutable()
{
    if (mState != STATE_PREPARE) {
        // throw new IllegalStateException("Animator has already started, cannot change it now!");
        Slogger::E("RenderNodeAnimator", "Animator has already started, cannot change it now!");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

void RenderNodeAnimator::ApplyInterpolator()
{
    if (mInterpolator == NULL) return;

    Int64 ni;
    if (IsNativeInterpolator(mInterpolator)) {
        (INativeInterpolatorFactory::Probe(mInterpolator))->CreateNativeInterpolator(&ni);
    } else {
        Int64 duration = nGetDuration(mNativePtr->get());
        ni = FallbackLUTInterpolator::CreateNativeInterpolator(mInterpolator, duration);
    }
    nSetInterpolator(mNativePtr->get(), ni);
}

void RenderNodeAnimator::DoStart()
{
    mState = STATE_RUNNING;
    nStart(mNativePtr->get(), this);

    // Alpha is a special snowflake that has the canonical value stored
    // in mTransformationInfo instead of in RenderNode, so we need to update
    // it with the final value here.
    if (mRenderProperty == IRenderNodeAnimator::ALPHA) {
        // Don't need null check because ViewPropertyAnimator's
        // ctor calls ensureTransformationInfo()
        ((View*)mViewTarget.Get())->mTransformationInfo->mAlpha = mFinalValue;
    }

    NotifyStartListeners();

    if (mViewTarget != NULL) {
        // Kick off a frame to start the process
        ((View*)mViewTarget.Get())->InvalidateViewProperty(TRUE, FALSE);
    }
}

void RenderNodeAnimator::NotifyStartListeners()
{
    AutoPtr<IArrayList> listeners = CloneListeners();
    Int32 numListeners = 0;
    if (listeners != NULL) {
        listeners->GetSize(&numListeners);
    }
    for (Int32 i = 0; i < numListeners; i++) {
        AutoPtr<IInterface> obj;
        listeners->Get(i, (IInterface**)&obj);
        IAnimatorListener::Probe(obj)->OnAnimationStart(this);
    }
}

AutoPtr<RenderNodeAnimator::DelayedAnimationHelper> RenderNodeAnimator::GetHelper()
{
    AutoPtr<DelayedAnimationHelper> handler = (DelayedAnimationHelper*)pthread_getspecific(sAnimationHelper);
    if (handler == NULL) {
        handler = new DelayedAnimationHelper();
        pthread_setspecific(sAnimationHelper, handler.Get());
        handler->AddRef();
    }

    return handler;
}

AutoPtr<IArrayList> RenderNodeAnimator::CloneListeners()
{
    AutoPtr<IArrayList> listeners;
    GetListeners((IArrayList**)&listeners);
    if (listeners != NULL) {
        AutoPtr<IInterface> cloneObj;
        ICloneable::Probe(listeners)->Clone((IInterface**)&cloneObj);
        listeners = IArrayList::Probe(cloneObj);
    }
    return listeners;
}

Boolean RenderNodeAnimator::ProcessDelayed(
    /* [in] */ Int64 frameTimeMs)
{
    if (mStartTime == 0) {
        mStartTime = frameTimeMs;
    } else if ((frameTimeMs - mStartTime) >= mStartDelay) {
        DoStart();
        return TRUE;
    }
    return FALSE;
}

void RenderNodeAnimator::CallOnFinished(
    /* [in] */ IRenderNodeAnimator* animator)
{
    ((RenderNodeAnimator*)animator)->OnFinished();
}

static inline RenderPropertyAnimator::RenderProperty toRenderProperty(
    /* [in] */ Int32 property)
{
    // LOG_ALWAYS_FATAL_IF(property < 0 || property > RenderPropertyAnimator::ALPHA,
    //         "Invalid property %d", property);
    if (property < 0 || property > RenderPropertyAnimator::ALPHA) {
        Slogger::E("RenderNodeAnimator", "Invalid property %d", property);
        assert(0);
    }
    return static_cast<RenderPropertyAnimator::RenderProperty>(property);
}

static inline CanvasPropertyPaintAnimator::PaintField toPaintField(
    /* [in] */ Int32 field)
{
    // LOG_ALWAYS_FATAL_IF(field < 0
    //         || field > CanvasPropertyPaintAnimator::ALPHA,
    //         "Invalid paint field %d", field);
    if (field < 0 || field > RenderPropertyAnimator::ALPHA) {
        Slogger::E("RenderNodeAnimator", "Invalid paint field %d", field);
        assert(0);
    }
    return static_cast<CanvasPropertyPaintAnimator::PaintField>(field);
}

Int64 RenderNodeAnimator::nCreateAnimator(
    /* [in] */ Int32 propertyRaw,
    /* [in] */ Float finalValue)
{
    RenderPropertyAnimator::RenderProperty property = toRenderProperty(propertyRaw);
    BaseRenderNodeAnimator* animator = new RenderPropertyAnimator(property, finalValue);
    return reinterpret_cast<Int64>( animator );
}

Int64 RenderNodeAnimator::nCreateCanvasPropertyFloatAnimator(
    /* [in] */ Int64 canvasPropertyPtr,
    /* [in] */ Float finalValue)
{
    CanvasPropertyPrimitive* canvasProperty = reinterpret_cast<CanvasPropertyPrimitive*>(canvasPropertyPtr);
    BaseRenderNodeAnimator* animator = new CanvasPropertyPrimitiveAnimator(canvasProperty, finalValue);
    return reinterpret_cast<Int64>( animator );
}

Int64 RenderNodeAnimator::nCreateCanvasPropertyPaintAnimator(
    /* [in] */ Int64 canvasPropertyPtr,
    /* [in] */ Int32 paintFieldRaw,
    /* [in] */ Float finalValue)
{
    CanvasPropertyPaint* canvasProperty = reinterpret_cast<CanvasPropertyPaint*>(canvasPropertyPtr);
    CanvasPropertyPaintAnimator::PaintField paintField = toPaintField(paintFieldRaw);
    BaseRenderNodeAnimator* animator = new CanvasPropertyPaintAnimator(
            canvasProperty, paintField, finalValue);
    return reinterpret_cast<Int64>(animator);
}

Int64 RenderNodeAnimator::nCreateRevealAnimator(
    /* [in] */ Int32 centerX,
    /* [in] */ Int32 centerY,
    /* [in] */ Float startRadius,
    /* [in] */ Float endRadius)
{
    BaseRenderNodeAnimator* animator = new RevealAnimator(centerX, centerY, startRadius, endRadius);
    return reinterpret_cast<Int64>( animator );
}

void RenderNodeAnimator::nSetStartValue(
    /* [in] */ Int64 animatorPtr,
    /* [in] */ Float startValue)
{
    BaseRenderNodeAnimator* animator = reinterpret_cast<BaseRenderNodeAnimator*>(animatorPtr);
    animator->setStartValue(startValue);
}

void RenderNodeAnimator::nSetDuration(
    /* [in] */ Int64 animatorPtr,
    /* [in] */ Int64 duration)
{
    // LOG_ALWAYS_FATAL_IF(duration < 0, "Duration cannot be negative");
    if (duration < 0) {
        Slogger::E("RenderNodeAnimator", "Duration cannot be negative");
        assert(0);
    }
    BaseRenderNodeAnimator* animator = reinterpret_cast<BaseRenderNodeAnimator*>(animatorPtr);
    animator->setDuration(duration);
}

Int64 RenderNodeAnimator::nGetDuration(
    /* [in] */ Int64 animatorPtr)
{
    BaseRenderNodeAnimator* animator = reinterpret_cast<BaseRenderNodeAnimator*>(animatorPtr);
    return static_cast<Int64>(animator->duration());
}

void RenderNodeAnimator::nSetStartDelay(
    /* [in] */ Int64 animatorPtr,
    /* [in] */ Int64 startDelay)
{
    // LOG_ALWAYS_FATAL_IF(startDelay < 0, "Start delay cannot be negative");
    if (startDelay < 0) {
        Slogger::E("RenderNodeAnimator", "Start delay cannot be negative");
        assert(0);
    }
    BaseRenderNodeAnimator* animator = reinterpret_cast<BaseRenderNodeAnimator*>(animatorPtr);
    animator->setStartDelay(startDelay);
}

void RenderNodeAnimator::nSetInterpolator(
    /* [in] */ Int64 animatorPtr,
    /* [in] */ Int64 interpolatorPtr)
{
    BaseRenderNodeAnimator* animator = reinterpret_cast<BaseRenderNodeAnimator*>(animatorPtr);
    Interpolator* interpolator = reinterpret_cast<Interpolator*>(interpolatorPtr);
    animator->setInterpolator(interpolator);
}

void RenderNodeAnimator::nSetAllowRunningAsync(
    /* [in] */ Int64 animatorPtr,
    /* [in] */ Boolean mayRunAsync)
{
    BaseRenderNodeAnimator* animator = reinterpret_cast<BaseRenderNodeAnimator*>(animatorPtr);
    animator->setAllowRunningAsync(mayRunAsync);
}

void RenderNodeAnimator::nStart(
    /* [in] */ Int64 animatorPtr,
    /* [in] */ IRenderNodeAnimator* finishListener)
{
    BaseRenderNodeAnimator* animator = reinterpret_cast<BaseRenderNodeAnimator*>(animatorPtr);
    if (finishListener) {
        animator->setListener(new AnimationListenerBridge(finishListener));
    }
    animator->start();
}

void RenderNodeAnimator::nEnd(
    /* [in] */ Int64 animatorPtr)
{
    BaseRenderNodeAnimator* animator = reinterpret_cast<BaseRenderNodeAnimator*>(animatorPtr);
    animator->end();
}

} // namespace View
} // namespace Droid
} // namespace Elastos
