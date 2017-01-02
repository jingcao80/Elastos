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

#include "elastos/droid/phone/common/animation/AnimUtils.h"

using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::Animation::CPathInterpolator;
using Elastos::Core::IFloat;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Animation {

//============================================================
// AnimUtils::FadeOutAdapter
//============================================================
ECode AnimUtils::FadeOutAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mFadeOut->SetVisibility(IView::GONE);
    if (mCallback != NULL) {
        mCallback->OnAnimationEnd();
    }
    return NOERROR;
}

ECode AnimUtils::FadeOutAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mFadeOut->SetVisibility(IView::GONE);
    mFadeOut->SetAlpha(0);
    if (mCallback != NULL) {
        mCallback->OnAnimationCancel();
    }
    return NOERROR;
}


//============================================================
// AnimUtils::FadeInAdapter
//============================================================
ECode AnimUtils::FadeInAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mFadeIn->SetVisibility(IView::VISIBLE);
    return NOERROR;
}

ECode AnimUtils::FadeInAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mFadeIn->SetAlpha(1);
    if (mCallback != NULL) {
        mCallback->OnAnimationCancel();
    }
    return NOERROR;
}

ECode AnimUtils::FadeInAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mCallback != NULL) {
        mCallback->OnAnimationEnd();
    }
    return NOERROR;
}


//============================================================
// AnimUtils::ScaleInAdapter
//============================================================
ECode AnimUtils::ScaleInAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mView->SetVisibility(IView::VISIBLE);
    return NOERROR;
}

ECode AnimUtils::ScaleInAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mView->SetScaleX(1);
    mView->SetScaleY(1);
    return NOERROR;
}


//============================================================
// AnimUtils::ScaleOutAdapter
//============================================================
ECode AnimUtils::ScaleOutAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mView->SetVisibility(IView::GONE);
    return NOERROR;
}

ECode AnimUtils::ScaleOutAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mView->SetVisibility(IView::GONE);
    mView->SetScaleX(0);
    mView->SetScaleY(0);
    return NOERROR;
}


//============================================================
// AnimUtils::UpdateListener
//============================================================
CAR_INTERFACE_IMPL(AnimUtils::UpdateListener, Object, IAnimatorUpdateListener);

ECode AnimUtils::UpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animator)
{
    AutoPtr<IInterface> valueObj;
    animator->GetAnimatedValue((IInterface**)&valueObj);
    Float value;
    IFloat::Probe(valueObj)->GetValue(&value);

    AutoPtr<IViewGroupLayoutParams> params;
    mView->GetLayoutParams((IViewGroupLayoutParams**)&params);
    params->SetWidth((Int32)(value * mDeltaWidth + mOldWidth));
    params->SetHeight((Int32)(value * mDeltaHeight + mOldHeight));
    mView->RequestLayout();
    return NOERROR;
}


//============================================================
// AnimUtils
//============================================================
static AutoPtr<IInterpolator> CreateInterpolator(Float a, Float b, Float c, Float d)
{
    AutoPtr<IInterpolator> ip;
    CPathInterpolator::New(a, b, c, d, (IInterpolator**)&ip);
    return ip;
}

const Int32 AnimUtils::DEFAULT_DURATION = -1;
const Int32 AnimUtils::NO_DELAY = 0;

const AutoPtr<IInterpolator> AnimUtils::EASE_IN = CreateInterpolator(0.0f, 0.0f, 0.2f, 1.0f);
const AutoPtr<IInterpolator> AnimUtils::EASE_OUT = CreateInterpolator(0.4f, 0.0f, 1.0f, 1.0f);
const AutoPtr<IInterpolator> AnimUtils::EASE_OUT_EASE_IN = CreateInterpolator(0.4f, 0, 0.2f, 1);

ECode AnimUtils::CrossFadeViews(
    /* [in] */ IView* fadeIn,
    /* [in] */ IView* fadeOut,
    /* [in] */ Int32 duration)
{
    FadeIn(fadeIn, duration);
    FadeOut(fadeOut, duration);
    return NOERROR;
}

ECode AnimUtils::FadeOut(
    /* [in] */ IView* fadeOut,
    /* [in] */ Int32 duration)
{
    return FadeOut(fadeOut, duration, NULL);
}

ECode AnimUtils::FadeOut(
    /* [in] */ IView* fadeOut,
    /* [in] */ Int32 durationMs,
    /* [in] */ IAnimationCallback* callback)
{
    fadeOut->SetAlpha(1);
    AutoPtr<IViewPropertyAnimator> animator;
    fadeOut->Animate((IViewPropertyAnimator**)&animator);
    animator->Cancel();
    animator->Alpha(0);
    animator->WithLayer();
    animator->SetListener(new FadeOutAdapter(fadeOut, callback));
    if (durationMs != DEFAULT_DURATION) {
        animator->SetDuration(durationMs);
    }
    animator->Start();
    return NOERROR;
}

ECode AnimUtils::FadeIn(
    /* [in] */ IView* fadeIn,
    /* [in] */ Int32 durationMs)
{
    return FadeIn(fadeIn, durationMs, NO_DELAY, NULL);
}

ECode AnimUtils::FadeIn(
    /* [in] */ IView* fadeIn,
    /* [in] */ Int32 durationMs,
    /* [in] */ Int32 delay,
    /* [in] */ IAnimationCallback* callback)
{
    fadeIn->SetAlpha(0);
    AutoPtr<IViewPropertyAnimator> animator;
    fadeIn->Animate((IViewPropertyAnimator**)&animator);
    animator->Cancel();

    animator->SetStartDelay(delay);
    animator->Alpha(1);
    animator->WithLayer();
    animator->SetListener(new FadeInAdapter(fadeIn, callback));
    if (durationMs != DEFAULT_DURATION) {
        animator->SetDuration(durationMs);
    }
    animator->Start();
    return NOERROR;
}

ECode AnimUtils::ScaleIn(
    /* [in] */ IView* view,
    /* [in] */ Int32 durationMs,
    /* [in] */ Int32 startDelayMs)
{
    AutoPtr<AnimatorListenerAdapter> listener = new ScaleInAdapter(view);
    ScaleInternal(view, 0 /* startScaleValue */, 1 /* endScaleValue */, durationMs,
            startDelayMs, listener, EASE_IN);
    return NOERROR;
}

ECode AnimUtils::ScaleOut(
    /* [in] */ IView* view,
    /* [in] */ Int32 durationMs)
{
    AutoPtr<AnimatorListenerAdapter> listener = new ScaleOutAdapter(view);

    ScaleInternal(view, 1 /* startScaleValue */, 0 /* endScaleValue */, durationMs,
            NO_DELAY, listener, EASE_OUT);
    return NOERROR;
}

void AnimUtils::ScaleInternal(
    /* [in] */ IView* view,
    /* [in] */ Int32 startScaleValue,
    /* [in] */ Int32 endScaleValue,
    /* [in] */ Int32 durationMs,
    /* [in] */ Int32 startDelay,
    /* [in] */ AnimatorListenerAdapter* listener,
    /* [in] */ IInterpolator* interpolator)
{
    view->SetScaleX(startScaleValue);
    view->SetScaleY(startScaleValue);

    AutoPtr<IViewPropertyAnimator> animator;
    view->Animate((IViewPropertyAnimator**)&animator);
    animator->Cancel();

    animator->SetInterpolator(ITimeInterpolator::Probe(interpolator));
    animator->ScaleX(endScaleValue);
    animator->ScaleY(endScaleValue);
    animator->SetListener(listener);
    animator->WithLayer();

    if (durationMs != DEFAULT_DURATION) {
        animator->SetDuration(durationMs);
    }
    animator->SetStartDelay(startDelay);

    animator->Start();
}

ECode AnimUtils::ChangeDimensions(
    /* [in] */ IView* view,
    /* [in] */ Int32 newWidth,
    /* [in] */ Int32 newHeight)
{
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr< ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
    (*values)[0] = 0;
    (*values)[1] = 1;
    AutoPtr<IValueAnimator> animator;
    helper->OfFloat(values, (IValueAnimator**)&animator);

    Int32 oldWidth, oldHeight;
    view->GetWidth(&oldWidth);
    view->GetHeight(&oldHeight);
    Int32 deltaWidth = newWidth - oldWidth;
    Int32 deltaHeight = newHeight - oldHeight;

    animator->AddUpdateListener(new UpdateListener(view, oldWidth, oldHeight, deltaWidth, deltaHeight));
    IAnimator::Probe(animator)->Start();
    return NOERROR;
}

} // namespace Animation
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos
