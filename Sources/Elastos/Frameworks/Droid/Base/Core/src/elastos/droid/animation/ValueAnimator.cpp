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

#include "elastos/droid/animation/ValueAnimator.h"
#include "elastos/droid/animation/AnimatorSet.h"
#include "elastos/droid/animation/CValueAnimator.h"
#include "elastos/droid/animation/PropertyValuesHolder.h"
#include "elastos/droid/animation/CArgbEvaluator.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/view/animation/CAccelerateDecelerateInterpolator.h"
#include "elastos/droid/view/Choreographer.h"
#include "elastos/droid/view/animation/CLinearInterpolator.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/etl/Algorithm.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::View::Choreographer;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::Animation::CAccelerateDecelerateInterpolator;
using Elastos::Droid::View::Animation::ILinearInterpolator;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Animation {

static const String TAG("ValueAnimator");

//==============================================================================
//              ValueAnimator::AnimationHandler
//==============================================================================
CAR_INTERFACE_IMPL(ValueAnimator::AnimationHandler, Object, IRunnable);

ValueAnimator::AnimationHandler::AnimationHandler()
    : mAnimationScheduled(FALSE)
{
    mChoreographer = (IChoreographer*)Choreographer::GetInstance();
}

ValueAnimator::AnimationHandler::~AnimationHandler()
{
    // clear refcount of AnimationSet
    //
    List<AutoPtr<IValueAnimator> >::Iterator it;
    for (it = mAnimations.Begin(); it != mAnimations.End(); ++it) {
        ValueAnimator* anim = (ValueAnimator*)(*it).Get();
        anim->ReleaseParent();
    }

    for (it = mPendingAnimations.Begin(); it != mPendingAnimations.End(); ++it) {
        ValueAnimator* anim = (ValueAnimator*)(*it).Get();
        anim->ReleaseParent();
    }

    for (it = mDelayedAnims.Begin(); it != mDelayedAnims.End(); ++it) {
        ValueAnimator* anim = (ValueAnimator*)(*it).Get();
        anim->ReleaseParent();
    }
}

void ValueAnimator::AnimationHandler::Start()
{
    ScheduleAnimation();
}

void ValueAnimator::AnimationHandler::DoAnimationFrame(
    /* [in] */ Int64 frameTime)
{
    // mPendingAnimations holds any animations that have requested to be started
    // We're going to clear mPendingAnimations, but starting animation may
    // cause more to be added to the pending list (for example, if one animation
    // starting triggers another starting). So we loop until mPendingAnimations
    // is empty.
    List<AutoPtr<IValueAnimator> >::Iterator it;
    while (!mPendingAnimations.IsEmpty()) {
        List<AutoPtr<IValueAnimator> > pendingCopy(mPendingAnimations);
        mPendingAnimations.Clear();
        for (it = pendingCopy.Begin(); it != pendingCopy.End(); ++it) {
            AutoPtr<IValueAnimator> anim = *it;
            ValueAnimator* va = (ValueAnimator*)anim.Get();
            // If the animation has a startDelay, place it on the delayed list
            if (va->mStartDelay == 0) {
                va->StartAnimation(this);
            }
            else {
                mDelayedAnims.PushBack(anim);
            }
        }
    }

    // Next, process animations currently sitting on the delayed queue, adding
    // them to the active animations if they are ready
    for (it = mDelayedAnims.Begin(); it != mDelayedAnims.End(); ++it) {
        AutoPtr<IValueAnimator> anim = *it;
        ValueAnimator* va = (ValueAnimator*)anim.Get();
        if (va->DelayedAnimationFrame(frameTime)) {
            mReadyAnims.PushBack(anim);
        }
    }
    if (!mReadyAnims.IsEmpty()) {
        for (it = mReadyAnims.Begin(); it != mReadyAnims.End(); ++it) {
            AutoPtr<IValueAnimator> anim = *it;
            ValueAnimator* va = (ValueAnimator*)anim.Get();
            va->StartAnimation(this);
            va->mRunning = TRUE;
            mDelayedAnims.Remove(anim);
        }
        mReadyAnims.Clear();
    }

    // Now process all active animations. The return value from animationFrame()
    // tells the handler whether it should now be ended
    for (it = mAnimations.Begin(); it != mAnimations.End(); ++it) {
        mTmpAnimations.PushBack(*it);
    }
    for (it = mTmpAnimations.Begin(); it != mTmpAnimations.End(); ++it) {
        AutoPtr<IValueAnimator> anim = *it;
        ValueAnimator* va = (ValueAnimator*)anim.Get();
        if (Find(mAnimations.Begin(), mAnimations.End(), anim) != mAnimations.End()
                && va->DoAnimationFrame(frameTime)) {
            mEndingAnims.PushBack(anim);
        }
    }
    mTmpAnimations.Clear();
    if (!mEndingAnims.IsEmpty()) {
        for (it = mEndingAnims.Begin(); it != mEndingAnims.End(); ++it) {
            AutoPtr<IValueAnimator> anim = *it;
            ValueAnimator* va = (ValueAnimator*)anim.Get();
            va->EndAnimation(this);
        }
        mEndingAnims.Clear();
    }

    // If there are still active or delayed animations, schedule a future call to
    // onAnimate to process the next frame of the animations.
    if (!mAnimations.IsEmpty() || !mDelayedAnims.IsEmpty()) {
        ScheduleAnimation();
    }
}

ECode ValueAnimator::AnimationHandler::Run()
{
    mAnimationScheduled = FALSE;
    Int64 frameTime;
    mChoreographer->GetFrameTime(&frameTime);
    DoAnimationFrame(frameTime);
    return NOERROR;
}

void ValueAnimator::AnimationHandler::ScheduleAnimation()
{
    if (!mAnimationScheduled) {
        mChoreographer->PostCallback(IChoreographer::CALLBACK_ANIMATION, this, NULL);
        mAnimationScheduled = TRUE;
    }
}


//==============================================================================
//              ValueAnimator
//==============================================================================

AutoPtr<ITimeInterpolator> CreateInterPolator()
{
    AutoPtr<ITimeInterpolator> obj;
    CAccelerateDecelerateInterpolator::New((ITimeInterpolator**)&obj);
    return obj;
}

static void ThreadDestructor(void* st)
{
    ValueAnimator::AnimationHandler* handler = static_cast<ValueAnimator::AnimationHandler*>(st);
    if (handler) {
        handler->Release();
    }
}

static Boolean InitTLS()
{
    Int32 UNUSED(result) = pthread_key_create(&ValueAnimator::sAnimationHandler, ThreadDestructor);
    assert(result == 0);
    return TRUE;
}

Float ValueAnimator::sDurationScale = 1.0f;

const Int32 ValueAnimator::STOPPED  = 0;
const Int32 ValueAnimator::RUNNING  = 1;
const Int32 ValueAnimator::SEEKED   = 2;

pthread_key_t ValueAnimator::sAnimationHandler;
Boolean ValueAnimator::sHaveKey = InitTLS();
const AutoPtr<ITimeInterpolator> ValueAnimator::sDefaultInterpolator = CreateInterPolator();

CAR_INTERFACE_IMPL(ValueAnimator, Animator, IValueAnimator);

ValueAnimator::ValueAnimator()
    : mStartTime(0)
    , mSeekTime(-1)
    , mPlayingState(STOPPED)
    , mInitialized(FALSE)
    , mPauseTime(0)
    , mResumed(FALSE)
    , mPlayingBackwards(FALSE)
    , mCurrentIteration(0)
    , mCurrentFraction(0.0f)
    , mStartedDelay(FALSE)
    , mDelayStartTime(0)
    , mRunning(FALSE)
    , mStarted(FALSE)
    , mStartListenersCalled(FALSE)
    , mDuration((Int64)(300 * sDurationScale))
    , mUnscaledDuration(300)
    , mStartDelay(0)
    , mUnscaledStartDelay(0)
    , mRepeatCount(0)
    , mRepeatMode(IValueAnimator::ANIMATION_RESTART)
    , mInterpolator(sDefaultInterpolator)
{}

ECode ValueAnimator::constructor()
{
    return NOERROR;
}

ValueAnimator::~ValueAnimator()
{
    mValuesMap.Clear();
}

void ValueAnimator::SetDurationScale(
    /* [in] */ Float durationScale)
{
    sDurationScale = durationScale;
}

Float ValueAnimator::GetDurationScale()
{
    return sDurationScale;
}

AutoPtr<IValueAnimator> ValueAnimator::OfInt32(
    /* [in] */ ArrayOf<Int32>* values)
{
    AutoPtr<IValueAnimator> anim;
    CValueAnimator::New((IValueAnimator**)&anim);
    anim->SetInt32Values(values);
    return anim;
}

AutoPtr<IValueAnimator> ValueAnimator::OfArgb(
    /* [in] */ ArrayOf<Int32>* values)
{
    AutoPtr<IValueAnimator> anim;
    CValueAnimator::New((IValueAnimator**)&anim);
    anim->SetInt32Values(values);
    anim->SetEvaluator(ITypeEvaluator::Probe(CArgbEvaluator::GetInstance()));
    return anim;
}

AutoPtr<IValueAnimator> ValueAnimator::OfFloat(
    /* [in] */ ArrayOf<Float>* values)
{
    AutoPtr<IValueAnimator> anim;
    CValueAnimator::New((IValueAnimator**)&anim);
    anim->SetFloatValues(values);
    return anim;
}

AutoPtr<IValueAnimator> ValueAnimator::OfPropertyValuesHolder(
    /* [in] */ ArrayOf<IPropertyValuesHolder*>* values)
{
    AutoPtr<IValueAnimator> anim;
    CValueAnimator::New((IValueAnimator**)&anim);
    anim->SetValues(values);
    return anim;
}

AutoPtr<IValueAnimator> ValueAnimator::OfObject(
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<IValueAnimator> anim;
    CValueAnimator::New((IValueAnimator**)&anim);
    anim->SetObjectValues(values);
    anim->SetEvaluator(evaluator);
    return anim;
}

ECode ValueAnimator::SetInt32Values(
    /* [in] */ ArrayOf<Int32>* values)
{
    if (values == NULL || values->GetLength() == 0) {
        return NOERROR;
    }

    if (mValues == NULL || mValues->GetLength() == 0) {
        AutoPtr<IPropertyValuesHolder> pvh = PropertyValuesHolder::OfInt32(String(""), values);
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
        a->Set(0, pvh);
        SetValues(a);
    }
    else {
        AutoPtr<IPropertyValuesHolder> valuesHolder = (*mValues)[0];
        valuesHolder->SetInt32Values(values);
    }
    // New property/values/target should cause re-initialization prior to starting
    mInitialized = FALSE;
    return NOERROR;
}

ECode ValueAnimator::SetFloatValues(
    /* [in] */ ArrayOf<Float>* values)
{
    if (values == NULL || values->GetLength() == 0) {
        return NOERROR;
    }
    if (mValues == NULL || mValues->GetLength() == 0) {
        AutoPtr<IPropertyValuesHolder> pvh = PropertyValuesHolder::OfFloat(String(""), values);
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
        a->Set(0, pvh);
        SetValues(a);
    }
    else {
        AutoPtr<IPropertyValuesHolder> valuesHolder = (*mValues)[0];
        valuesHolder->SetFloatValues(values);
    }
    // New property/values/target should cause re-initialization prior to starting
    mInitialized = FALSE;
    return NOERROR;
}

ECode ValueAnimator::SetObjectValues(
    /* [in] */ ArrayOf<IInterface*>* values)
{
    if (values == NULL || values->GetLength() == 0) {
        return NOERROR;
    }
    if (mValues == NULL || mValues->GetLength() == 0) {
        AutoPtr<IPropertyValuesHolder> pvh = PropertyValuesHolder::OfObject(String(""), NULL, values);
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
        a->Set(0, pvh);
        SetValues(a);
    }
    else {
        AutoPtr<IPropertyValuesHolder> valuesHolder = (*mValues)[0];
        valuesHolder->SetObjectValues(values);
    }
    // New property/values/target should cause re-initialization prior to starting
    mInitialized = FALSE;
    return NOERROR;
}

ECode ValueAnimator::SetValues(
    /* [in] */ ArrayOf<IPropertyValuesHolder*>* values)
{
    Int32 numValues = values->GetLength();
    mValues = values;
    mValuesMap.Clear();
    for (Int32 i = 0; i < numValues; ++i) {
        AutoPtr<IPropertyValuesHolder> valuesHolder = (*values)[i];
        String propertyName;
        valuesHolder->GetPropertyName(&propertyName);
        mValuesMap[propertyName] = valuesHolder;
    }
    // New property/values/target should cause re-initialization prior to starting
    mInitialized = FALSE;
    return NOERROR;
}

ECode ValueAnimator::GetValues(
    /* [out, callee] */ ArrayOf<IPropertyValuesHolder*>** values)
{
    VALIDATE_NOT_NULL(values);
    *values = mValues;
    REFCOUNT_ADD(*values);
    return NOERROR;
}

ECode ValueAnimator::InitAnimation()
{
    if (!mInitialized) {
        Int32 numValues = mValues->GetLength();
        for (Int32 i = 0; i < numValues; ++i) {
            AutoPtr<IPropertyValuesHolder> pvh = (*mValues)[i];
            PropertyValuesHolder* holder = (PropertyValuesHolder*)pvh.Get();
            holder->Init();
        }
        mInitialized = TRUE;
    }
    return NOERROR;
}

ECode ValueAnimator::SetDuration(
    /* [in] */ Int64 duration)
{
    if (duration < 0) {
        Logger::E(TAG, "IllegalArgumentException: Animators cannot have negative duration: %d", duration);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mUnscaledDuration = duration;
    UpdateScaledDuration();
    return NOERROR;
}

void ValueAnimator::UpdateScaledDuration()
{
    mDuration = (Int64)(mUnscaledDuration * sDurationScale);
}

ECode ValueAnimator::GetDuration(
    /* [out] */ Int64* duration)
{
    VALIDATE_NOT_NULL(duration);
    *duration = mUnscaledDuration;
    return NOERROR;
}

ECode ValueAnimator::SetCurrentPlayTime(
    /* [in] */ Int64 playTime)
{
    InitAnimation();
    Int64 currentTime = 0;
    AnimationUtils::CurrentAnimationTimeMillis(&currentTime);
    if (mPlayingState != RUNNING) {
        mSeekTime = playTime;
        mPlayingState = SEEKED;
    }
    mStartTime = currentTime - playTime;
    DoAnimationFrame(currentTime);
    return NOERROR;
}

ECode ValueAnimator::GetCurrentPlayTime(
    /* [out] */ Int64* playTime)
{
    VALIDATE_NOT_NULL(playTime);
    if (!mInitialized || mPlayingState == STOPPED) {
        *playTime = 0;
        return NOERROR;
    }
    Int64 tmp = 0;
    AnimationUtils::CurrentAnimationTimeMillis(&tmp);
    *playTime = tmp - mStartTime;
    return NOERROR;
}

ECode ValueAnimator::GetStartDelay(
    /* [out] */ Int64* delay)
{
    VALIDATE_NOT_NULL(delay);
    *delay = mUnscaledStartDelay;
    return NOERROR;
}

ECode ValueAnimator::SetStartDelay(
    /* [in] */ Int64 startDelay)
{
    mStartDelay = (Int64)(startDelay * sDurationScale);
    mUnscaledStartDelay = startDelay;
    return NOERROR;
}

Int64 ValueAnimator::GetFrameDelay()
{
    return Choreographer::GetFrameDelay();
}

ECode ValueAnimator::SetFrameDelay(
    /* [in]*/ Int64 delay)
{
    Choreographer::SetFrameDelay(delay);
    return NOERROR;
}

ECode ValueAnimator::GetAnimatedValue(
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    *value = NULL;
    if (mValues != NULL && mValues->GetLength() > 0) {
        AutoPtr<IPropertyValuesHolder> holder = (*mValues)[0];
        PropertyValuesHolder* pvh = (PropertyValuesHolder*)holder.Get();
        return pvh->GetAnimatedValue(value);
    }
    // Shouldn't get here; should always have values unless ValueAnimator was set up wrong
    return NOERROR;
}

ECode ValueAnimator::GetAnimatedValue(
    /* [in] */ const String& propertyName,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    *value = NULL;
    StrMapIterator it = mValuesMap.Find(propertyName);
    if (it != mValuesMap.End()) {
       AutoPtr<IPropertyValuesHolder> valuesHolder = it->mSecond;
       PropertyValuesHolder* pvh = (PropertyValuesHolder*)valuesHolder.Get();
       return pvh->GetAnimatedValue(value);
    }
    // At least avoid crashing if called with bogus propertyName
    return NOERROR;
}

ECode ValueAnimator::SetRepeatCount(
    /* [in] */ Int32 value)
{
    mRepeatCount = value;
    return NOERROR;
}

ECode ValueAnimator::GetRepeatCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mRepeatCount;
    return NOERROR;
}

ECode ValueAnimator::SetRepeatMode(
    /* [in] */ Int32 value)
{
    mRepeatMode = value;
    return NOERROR;
}

ECode ValueAnimator::GetRepeatMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mRepeatMode;
    return NOERROR;
}

ECode ValueAnimator::AddUpdateListener(
    /* [in] */ IAnimatorUpdateListener* listener)
{
    if (mUpdateListeners == NULL) {
        CArrayList::New((IArrayList**)&mUpdateListeners);
    }
    mUpdateListeners->Add(listener);
    return NOERROR;
}

ECode ValueAnimator::RemoveAllUpdateListeners()
{
    if (mUpdateListeners == NULL) {
        return NOERROR;
    }
    mUpdateListeners->Clear();
    mUpdateListeners = NULL;
    return NOERROR;
}

ECode ValueAnimator::RemoveUpdateListener(
    /* [in] */ IAnimatorUpdateListener* listener)
{
    if (mUpdateListeners == NULL) {
        return NOERROR;
    }
    mUpdateListeners->Remove(listener);
    Int32 size;
    mUpdateListeners->GetSize(&size);
    if (size == 0) {
        mUpdateListeners = NULL;
    }
    return NOERROR;
}

ECode ValueAnimator::SetInterpolator(
    /* [in] */ ITimeInterpolator* value)
{
    if (value != NULL) {
        mInterpolator = value;
    }
    else {
        CLinearInterpolator::New((ITimeInterpolator**)&mInterpolator);
    }
    return NOERROR;
}

ECode ValueAnimator::GetInterpolator(
    /* [out] */ ITimeInterpolator** interpolator)
{
    VALIDATE_NOT_NULL(interpolator);
    *interpolator = mInterpolator;
    REFCOUNT_ADD(*interpolator);
    return NOERROR;
}

ECode ValueAnimator::SetEvaluator(
    /* [in] */ ITypeEvaluator* value)
{
    if (value != NULL && mValues != NULL && mValues->GetLength() > 0) {
        (*mValues)[0]->SetEvaluator(value);
    }
    return NOERROR;
}

void ValueAnimator::NotifyStartListeners()
{
    if (mListeners != NULL && !mStartListenersCalled) {
        AutoPtr<IInterface> cloneObj;
        ICloneable::Probe(mListeners)->Clone((IInterface**)&cloneObj);
        IArrayList* tmpListeners = IArrayList::Probe(cloneObj);
        Int32 numListeners;
        tmpListeners->GetSize(&numListeners);
        for (Int32 i = 0; i < numListeners; ++i) {
            AutoPtr<IInterface> obj;
            tmpListeners->Get(i, (IInterface**)&obj);
            IAnimatorListener::Probe(obj)->OnAnimationStart(this);
        }
    }
    mStartListenersCalled = TRUE;
}

ECode ValueAnimator::Start(
    /* [in] */ Boolean playBackwards)
{
    if (Looper::GetMyLooper() == NULL) {
        Logger::E(TAG, "Animators may only be run on Looper threads");
        return E_ANDROID_RUNTIME_EXCEPTION;
    }
    mPlayingBackwards = playBackwards;
    mCurrentIteration = 0;
    mPlayingState = STOPPED;
    mStarted = TRUE;
    mStartedDelay = FALSE;
    mPaused = FALSE;
    UpdateScaledDuration(); // in case the scale factor has changed since creation time
    AutoPtr<AnimationHandler> animationHandler = GetOrCreateAnimationHandler();
    // TODO animation change refcount
    //
    AutoPtr<IValueAnimator> anim = this;
    animationHandler->mPendingAnimations.PushBack(anim);
    if (mStartDelay == 0) {
        // This sets the initial value of the animation, prior to actually starting it running
        SetCurrentPlayTime(0);
        mPlayingState = STOPPED;
        mRunning = TRUE;
        NotifyStartListeners();
    }
    animationHandler->Start();
    return NOERROR;
}

ECode ValueAnimator::Start()
{
    return Start(FALSE);
}

ECode ValueAnimator::Cancel()
{
    // Only cancel if the animation is actually running or has been started and is about
    // to run
    AutoPtr<AnimationHandler> handler = GetOrCreateAnimationHandler();
    AutoPtr<IValueAnimator> thisAnim = this;
    if (mPlayingState != STOPPED
        || Find(handler->mPendingAnimations.Begin(), handler->mPendingAnimations.End(), thisAnim)
                != handler->mPendingAnimations.End()
        || Find(handler->mDelayedAnims.Begin(), handler->mDelayedAnims.End(), thisAnim)
                != handler->mDelayedAnims.End()) {
        // Only notify listeners if the animator has actually started
        if ((mStarted || mRunning) && mListeners != NULL) {
            if (!mRunning) {
                // If it's not yet running, then start listeners weren't called. Call them now.
                NotifyStartListeners();
            }
            AutoPtr<IInterface> cloneObj;
            ICloneable::Probe(mListeners)->Clone((IInterface**)&cloneObj);
            IArrayList* tmpListeners = IArrayList::Probe(cloneObj);
            Int32 numListeners;
            tmpListeners->GetSize(&numListeners);
            for (Int32 i = 0; i < numListeners; ++i) {
                AutoPtr<IInterface> obj;
                tmpListeners->Get(i, (IInterface**)&obj);
                IAnimatorListener::Probe(obj)->OnAnimationCancel(this);
            }
        }
        EndAnimation(handler);
    }
    return NOERROR;
}

ECode ValueAnimator::End()
{
    AutoPtr<AnimationHandler> handler = GetOrCreateAnimationHandler();
    AutoPtr<IValueAnimator> thisAnim = this;
    if (Find(handler->mAnimations.Begin(), handler->mAnimations.End(), thisAnim) == handler->mAnimations.End()
        && Find(handler->mPendingAnimations.Begin(), handler->mPendingAnimations.End(), thisAnim) == handler->mPendingAnimations.End()) {
        // Special case if the animation has not yet started; get it ready for ending
        mStartedDelay = FALSE;
        StartAnimation(handler);
        mStarted = TRUE;
    }
    else if (!mInitialized) {
        InitAnimation();
    }
    AnimateValue(mPlayingBackwards ? 0.0f : 1.0f);
    EndAnimation(handler);
    return NOERROR;
}

ECode ValueAnimator::Resume()
{
    if (mPaused) {
        mResumed = TRUE;
    }
    return Animator::Resume();
}

ECode ValueAnimator::Pause()
{
    Boolean previouslyPaused = mPaused;
    Animator::Pause();
    if (!previouslyPaused && mPaused) {
        mPauseTime = -1;
        mResumed = FALSE;
    }
    return NOERROR;
}

ECode ValueAnimator::IsRunning(
    /* [out] */ Boolean* running)
{
    VALIDATE_NOT_NULL(running);
    *running = (mPlayingState == RUNNING || mRunning);
    return NOERROR;
}

ECode ValueAnimator::IsStarted(
    /* [out] */ Boolean* started)
{
    VALIDATE_NOT_NULL(started);
    *started = mStarted;
    return NOERROR;
}

ECode ValueAnimator::Reverse()
{
    mPlayingBackwards = !mPlayingBackwards;
    if (mPlayingState == RUNNING) {
        Int64 currentTime = 0;
        AnimationUtils::CurrentAnimationTimeMillis(&currentTime);
        Int64 currentPlayTime = currentTime - mStartTime;
        Int64 timeLeft = mDuration - currentPlayTime;
        mStartTime = currentTime - timeLeft;
        return NOERROR;
    }
    else if (mStarted) {
        return End();
    }
    else {
        return Start(TRUE);
    }
}

ECode ValueAnimator::CanReverse(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = TRUE;
    return NOERROR;
}

void ValueAnimator::EndAnimation(
    /* [in] */ AnimationHandler* handler)
{
    AutoPtr<IValueAnimator> anim = this; // holder ref here
    handler->mAnimations.Remove(anim);
    handler->mPendingAnimations.Remove(anim);
    handler->mDelayedAnims.Remove(anim);
    mPlayingState = STOPPED;
    mPaused = FALSE;
    if ((mStarted || mRunning) && mListeners != NULL) {
        if (!mRunning) {
            // If it's not yet running, then start listeners weren't called. Call them now.
            NotifyStartListeners();
        }
        AutoPtr<IInterface> cloneObj;
        ICloneable::Probe(mListeners)->Clone((IInterface**)&cloneObj);
        IArrayList* tmpListeners = IArrayList::Probe(cloneObj);
        Int32 numListeners;
        tmpListeners->GetSize(&numListeners);
        for (Int32 i = 0; i < numListeners; ++i) {
            AutoPtr<IInterface> obj;
            tmpListeners->Get(i, (IInterface**)&obj);
            IAnimatorListener::Probe(obj)->OnAnimationEnd(this);
        }
    }
    mRunning = FALSE;
    mStarted = FALSE;
    mStartListenersCalled = FALSE;
    mPlayingBackwards = FALSE;

    // clear refcount of AnimationSet
    //
    ReleaseParent();

    // if (Trace.isTagEnabled(Trace.TRACE_TAG_VIEW)) {
    //     Trace.asyncTraceEnd(Trace.TRACE_TAG_VIEW, getNameForTrace(),
    //             System.identityHashCode(this));
    // }
}

void ValueAnimator::StartAnimation(
    /* [in] */ AnimationHandler* handler)
{
    // if (Trace.isTagEnabled(Trace.TRACE_TAG_VIEW)) {
    //     Trace.asyncTraceBegin(Trace.TRACE_TAG_VIEW, getNameForTrace(),
    //             System.identityHashCode(this));
    // }
    InitAnimation();
    AutoPtr<IValueAnimator> anim = this;
    handler->mAnimations.PushBack(anim);
    if (mStartDelay > 0 && mListeners != NULL) {
        // Listeners were already notified in start() if startDelay is 0; this is
        // just for delayed animations
        NotifyStartListeners();
    }
}

String ValueAnimator::GetNameForTrace()
{
    return String("ValueAnimator");
}

Boolean ValueAnimator::DelayedAnimationFrame(
    /* [in] */ Int64 currentTime)
{
    if (!mStartedDelay) {
        mStartedDelay = TRUE;
        mDelayStartTime = currentTime;
    }
    if (mPaused) {
        if (mPauseTime < 0) {
            mPauseTime = currentTime;
        }
        return FALSE;
    }
    else if (mResumed) {
        mResumed = FALSE;
        if (mPauseTime > 0) {
            // Offset by the duration that the animation was paused
            mDelayStartTime += (currentTime - mPauseTime);
        }
    }
    Int64 deltaTime = currentTime - mDelayStartTime;
    if (deltaTime > mStartDelay) {
        // startDelay ended - start the anim and record the
        // mStartTime appropriately
        mStartTime = currentTime - (deltaTime - mStartDelay);
        mPlayingState = RUNNING;
        return TRUE;
    }
    return FALSE;
}

Boolean ValueAnimator::AnimationFrame(
    /* [in] */ Int64 currentTime)
{
    Boolean done = FALSE;
    switch (mPlayingState) {
        case RUNNING:
        case SEEKED:
            Float fraction = mDuration > 0 ? (Float)(currentTime - mStartTime) / mDuration : 1.0f;
            if (fraction >= 1.0f) {
                if (mCurrentIteration < mRepeatCount || mRepeatCount == INFINITE) {
                    // Time to repeat
                    if (mListeners != NULL) {
                        Int32 numListeners;
                        mListeners->GetSize(&numListeners);
                        for (Int32 i = 0; i < numListeners; ++i) {
                            AutoPtr<IInterface> obj;
                            mListeners->Get(i, (IInterface**)&obj);
                            IAnimatorListener::Probe(obj)->OnAnimationRepeat(this);
                        }
                    }
                    if (mRepeatMode == IValueAnimator::ANIMATION_REVERSE) {
                        mPlayingBackwards = !mPlayingBackwards;
                    }
                    mCurrentIteration += (Int32)fraction;
                    Int32 value = fraction;
                    fraction = fraction - value; //fraction % 1.0f;
                    mStartTime += mDuration;
                }
                else {
                    done = TRUE;
                    fraction = Elastos::Core::Math::Min(fraction, 1.0f);
                }
            }
            if (mPlayingBackwards) {
                fraction = 1.0f - fraction;
            }
            AnimateValue(fraction);
            break;
    }

    return done;
}

Boolean ValueAnimator::DoAnimationFrame(
    /* [in] */ Int64 frameTime)
{
    if (mPlayingState == STOPPED) {
        mPlayingState = RUNNING;
        if (mSeekTime < 0) {
            mStartTime = frameTime;
        }
        else {
            mStartTime = frameTime - mSeekTime;
            // Now that we're playing, reset the seek time
            mSeekTime = -1;
        }
    }
    if (mPaused) {
        if (mPauseTime < 0) {
            mPauseTime = frameTime;
        }
        return FALSE;
    }
    else if (mResumed) {
        mResumed = FALSE;
        if (mPauseTime > 0) {
            // Offset by the duration that the animation was paused
            mStartTime += (frameTime - mPauseTime);
        }
    }
    // The frame time might be before the start time during the first frame of
    // an animation.  The "current time" must always be on or after the start
    // time to avoid animating frames at negative time intervals.  In practice, this
    // is very rare and only happens when seeking backwards.
    const Int64 currentTime = Elastos::Core::Math::Max(frameTime, mStartTime);
    return AnimationFrame(currentTime);
}

ECode ValueAnimator::GetAnimatedFraction(
    /* [out] */ Float* fraction)
{
    VALIDATE_NOT_NULL(fraction);
    *fraction = mCurrentFraction;
    return NOERROR;
}

ECode ValueAnimator::AnimateValue(
    /* [in] */ Float fraction)
{
    mInterpolator->GetInterpolation(fraction, &fraction);
    mCurrentFraction = fraction;
    Int32 numValues = mValues->GetLength();
    for (Int32 i = 0; i < numValues; ++i) {
        AutoPtr<IPropertyValuesHolder> pvh = (*mValues)[i];
        PropertyValuesHolder* holder = (PropertyValuesHolder*)pvh.Get();
        holder->CalculateValue(fraction);
    }
    if (mUpdateListeners != NULL) {
        Int32 numListeners;
        mUpdateListeners->GetSize(&numListeners);
        for (Int32 i = 0; i < numListeners; ++i) {
            AutoPtr<IInterface> obj;
            mUpdateListeners->Get(i, (IInterface**)&obj);
            IAnimatorUpdateListener::Probe(obj)->OnAnimationUpdate(this);
        }
    }
    return NOERROR;
}

ECode ValueAnimator::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    AutoPtr<IValueAnimator> newObject = new CValueAnimator();

    CloneImpl(newObject);
    *object = newObject;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode ValueAnimator::CloneImpl(
    /* [in] */ IValueAnimator* object)
{
    Animator::CloneImpl(IAnimator::Probe(object));

    ValueAnimator* anim = (ValueAnimator*)object;

    if (mUpdateListeners != NULL) {
        AutoPtr<IArrayList> oldListeners = mUpdateListeners;
        CArrayList::New((IArrayList**)&anim->mUpdateListeners);
        Int32 numListeners;
        oldListeners->GetSize(&numListeners);
        for (Int32 i = 0; i < numListeners; ++i) {
            AutoPtr<IInterface> obj;
            oldListeners->Get(i, (IInterface**)&obj);
            anim->mUpdateListeners->Add(obj);
        }
    }
    anim->mSeekTime = -1;
    anim->mPlayingBackwards = FALSE;
    anim->mCurrentIteration = 0;
    anim->mInitialized = FALSE;
    anim->mPlayingState = STOPPED;
    anim->mStartedDelay = FALSE;
    if (mValues != NULL) {
        Int32 numValues = mValues->GetLength();
        anim->mValues = ArrayOf<IPropertyValuesHolder*>::Alloc(numValues);
        anim->mValuesMap.Clear();
        for (Int32 i = 0; i < numValues; ++i) {
            AutoPtr<IInterface> obj;
            ICloneable::Probe((*mValues)[i])->Clone((IInterface**)&obj);
            IPropertyValuesHolder* newValuesHolder = IPropertyValuesHolder::Probe(obj);
            anim->mValues->Set(i, newValuesHolder);
            String propertyName;
            newValuesHolder->GetPropertyName(&propertyName);
            anim->mValuesMap[propertyName] = newValuesHolder;
        }
    }
    return NOERROR;
}

Int32 ValueAnimator::GetCurrentAnimationsCount()
{
    AutoPtr<AnimationHandler> handler = (AnimationHandler*)pthread_getspecific(sAnimationHandler);
    return handler != NULL ? handler->mAnimations.GetSize() : 0;
}

void ValueAnimator::ClearAllAnimations()
{
    AutoPtr<AnimationHandler> handler = (AnimationHandler*)pthread_getspecific(sAnimationHandler);
    if (handler != NULL) {
        handler->mAnimations.Clear();
        handler->mPendingAnimations.Clear();
        handler->mDelayedAnims.Clear();
    }
}

AutoPtr<ValueAnimator::AnimationHandler> ValueAnimator::GetOrCreateAnimationHandler()
{
    AutoPtr<AnimationHandler> handler = (AnimationHandler*)pthread_getspecific(sAnimationHandler);
    if (handler == NULL) {
        handler = new AnimationHandler();
        pthread_setspecific(sAnimationHandler, handler.Get());
        handler->AddRef();
    }
    return handler;
}

ECode ValueAnimator::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    StringBuilder sb;
    sb += "ValueAnimator{0x";
    Int32 hashCode;
    GetHashCode(&hashCode);
    sb += StringUtils::ToHexString(hashCode);
    if (mValues != NULL) {
        for (Int32 i = 0; i < mValues->GetLength(); ++i) {
            sb += "\n    ";
            String info;
            IObject::Probe((*mValues)[i])->ToString(&info);
            sb += info;
        }
    }
    sb += "}";
    *info = sb.ToString();
    return NOERROR;
}

ECode ValueAnimator::SetAllowRunningAsynchronously(
    /* [in] */ Boolean mayRunAsync)
{
    // It is up to subclasses to support this, if they can.
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
