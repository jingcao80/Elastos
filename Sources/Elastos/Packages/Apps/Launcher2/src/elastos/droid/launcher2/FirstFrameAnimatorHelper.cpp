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

#include "elastos/droid/launcher2/FirstFrameAnimatorHelper.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::View::EIID_IOnDrawListener;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(FirstFrameAnimatorHelper::MyOnDrawListener, Object, IOnDrawListener);

FirstFrameAnimatorHelper::MyOnDrawListener::MyOnDrawListener()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mTime);
}

ECode FirstFrameAnimatorHelper::MyOnDrawListener::OnDraw()
{
    FirstFrameAnimatorHelper::sGlobalFrameCounter++;
    if (FirstFrameAnimatorHelper::DEBUG) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 newTime;
        system->GetCurrentTimeMillis(&newTime);
        Slogger::D("FirstFrameAnimatorHelper", "TICK %lld", (newTime - mTime));
        mTime = newTime;
    }
    return NOERROR;
}

FirstFrameAnimatorHelper::MyRunnable::MyRunnable(
    /* [in] */ IValueAnimator* animation,
    /* [in] */ FirstFrameAnimatorHelper* host)
    : mAnimation(animation)
    , mHost(host)
{
}

ECode FirstFrameAnimatorHelper::MyRunnable::Run()
{
    // hold and release host
    AutoPtr<FirstFrameAnimatorHelper> host = mHost;
    mHost = NULL;
    return mAnimation->RemoveUpdateListener(host);
}

const Boolean FirstFrameAnimatorHelper::DEBUG = FALSE ;
const Int32 FirstFrameAnimatorHelper::MAX_DELAY = 1000;
const Int32 FirstFrameAnimatorHelper::IDEAL_FRAME_DURATION = 16;

AutoPtr<IOnDrawListener> FirstFrameAnimatorHelper::sGlobalDrawListener;
Int64 FirstFrameAnimatorHelper::sGlobalFrameCounter;
Boolean FirstFrameAnimatorHelper::sVisible;

CAR_INTERFACE_IMPL(FirstFrameAnimatorHelper, AnimatorListenerAdapter, IAnimatorUpdateListener);

FirstFrameAnimatorHelper::FirstFrameAnimatorHelper(
    /* [in] */ IValueAnimator* animator,
    /* [in] */ IView* target)
    : mTarget(target)
    , mStartFrame(0)
    , mStartTime(-1)
    , mHandlingOnAnimationUpdate(FALSE)
    , mAdjustedSecondFrameTime(FALSE)
{
    animator->AddUpdateListener(this);
}

FirstFrameAnimatorHelper::FirstFrameAnimatorHelper(
    /* [in] */ IViewPropertyAnimator* vpa,
    /* [in] */ IView* target)
    : mTarget(target)
    , mStartFrame(0)
    , mStartTime(-1)
    , mHandlingOnAnimationUpdate(FALSE)
    , mAdjustedSecondFrameTime(FALSE)
{
    vpa->SetListener(this);
}

FirstFrameAnimatorHelper::~FirstFrameAnimatorHelper()
{
}

ECode FirstFrameAnimatorHelper::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<IValueAnimator> va = IValueAnimator::Probe(animation);
    va->AddUpdateListener(this);
    return OnAnimationUpdate(va);
}

ECode FirstFrameAnimatorHelper::SetIsVisible(
    /* [in] */ Boolean visible)
{
    sVisible = visible;
    return NOERROR;
}

ECode FirstFrameAnimatorHelper::InitializeDrawListener(
    /* [in] */ IView* view)
{
    if (sGlobalDrawListener != NULL) {
        AutoPtr<IViewTreeObserver> observer;
        view->GetViewTreeObserver((IViewTreeObserver**)&observer);
        observer->RemoveOnDrawListener(sGlobalDrawListener);
    }
    sGlobalDrawListener = new MyOnDrawListener();
    AutoPtr<IViewTreeObserver> observer;
    view->GetViewTreeObserver((IViewTreeObserver**)&observer);
    observer->AddOnDrawListener(sGlobalDrawListener);
    sVisible = TRUE;
    return NOERROR;
}

ECode FirstFrameAnimatorHelper::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    Int64 currentTime;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&currentTime);
    if (mStartTime == -1) {
        mStartFrame = sGlobalFrameCounter;
        mStartTime = currentTime;
    }

    if (!mHandlingOnAnimationUpdate && sVisible) {
        // If the current play time exceeds the duration, the animation
        // will get finished, even if we call setCurrentPlayTime -- therefore
        // don't adjust the animation in that case
        Int64 playeTime;
        animation->GetCurrentPlayTime(&playeTime);
        Int64 duration;
        animation->GetDuration(&duration);
        if (playeTime < duration) {
            mHandlingOnAnimationUpdate = TRUE;
            Int64 frameNum = sGlobalFrameCounter - mStartFrame;
            // If we haven't drawn our first frame, reset the time to t = 0
            // (give up after MAX_DELAY ms of waiting though - might happen, for example, if we
            // are no longer in the foreground and no frames are being rendered ever)
            if (frameNum == 0 && currentTime < mStartTime + MAX_DELAY) {
                // The first frame on animations doesn't always trigger an invalidate...
                // force an invalidate here to make sure the animation continues to advance
                AutoPtr<IView> view;
                mTarget->GetRootView((IView**)&view);
                view->Invalidate();
                animation->SetCurrentPlayTime(0);

            // For the second frame, if the first frame took more than 16ms,
            // adjust the start time and pretend it took only 16ms anyway. This
            // prevents a large jump in the animation due to an expensive first frame
            }
            else if (frameNum == 1 && currentTime < mStartTime + MAX_DELAY &&
                       !mAdjustedSecondFrameTime &&
                       currentTime > mStartTime + IDEAL_FRAME_DURATION) {
                animation->SetCurrentPlayTime(IDEAL_FRAME_DURATION);
                mAdjustedSecondFrameTime = TRUE;
            }
            else {
                if (frameNum > 1) {
                    AutoPtr<IRunnable> run = new MyRunnable(animation, this);
                    Boolean res;
                    mTarget->Post(run, &res);
                }
                if (DEBUG) {
                    Print(animation);
                }
            }
            mHandlingOnAnimationUpdate = FALSE;
        }
    }
    else {
        if (DEBUG) {
            Print(animation);
        }
    }
    return NOERROR;
}

ECode FirstFrameAnimatorHelper::Print(
    /* [in] */ IValueAnimator* animation)
{
    Int64 playeTime;
    animation->GetCurrentPlayTime(&playeTime);
    Int64 duration;
    animation->GetDuration(&duration);
    Float flatFraction = playeTime / (Float)duration;

    StringBuilder sb;
    sb += sGlobalFrameCounter;
    sb += "(";
    Int64 tmp = sGlobalFrameCounter - mStartFrame;
    sb += tmp;
    sb += ") ";
    sb += mTarget;
    sb += " dirty? ";
    Boolean res;
    mTarget->IsDirty(&res);
    sb += res;
    sb += " ";
    sb += flatFraction;
    sb += " ";
    sb += TO_STR(this);
    sb += " ";
    sb += animation;
    Slogger::D("FirstFrameAnimatorHelper", sb.ToString());
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos