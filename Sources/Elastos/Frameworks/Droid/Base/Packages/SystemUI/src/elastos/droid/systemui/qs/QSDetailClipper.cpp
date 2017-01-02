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

#include "elastos/droid/systemui/qs/QSDetailClipper.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::CViewAnimationUtilsHelper;
using Elastos::Droid::View::IViewAnimationUtilsHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

QSDetailClipper::ReverseBackground::ReverseBackground(
    /* [in] */ QSDetailClipper* host)
    : mHost(host)
{}

ECode QSDetailClipper::ReverseBackground::Run()
{
    if (mHost->mAnimator != NULL) {
        Int64 d = 0;
        mHost->mAnimator->GetDuration(&d);
        mHost->mBackground->ReverseTransition((Int32)(d * 0.35));
    }
    return NOERROR;
}

QSDetailClipper::VisibleOnStart::VisibleOnStart(
    /* [in] */ QSDetailClipper* host)
    : mHost(host)
{}

ECode QSDetailClipper::VisibleOnStart::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mHost->mDetail->SetVisibility(IView::VISIBLE);
    return NOERROR;
}

ECode QSDetailClipper::VisibleOnStart::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mDetail->SetLayerType(IView::LAYER_TYPE_NONE, NULL);
    mHost->mAnimator = NULL;
    return NOERROR;
}

QSDetailClipper::GoneOnEnd::GoneOnEnd(
    /* [in] */ QSDetailClipper* host)
    : mHost(host)
{}

ECode QSDetailClipper::GoneOnEnd::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mDetail->SetLayerType(IView::LAYER_TYPE_NONE, NULL);
    mHost->mDetail->SetVisibility(IView::GONE);
    mHost->mBackground->ResetTransition();
    mHost->mAnimator = NULL;
    return NOERROR;
}

QSDetailClipper::QSDetailClipper(
    /* [in] */ IView* detail)
{
    mReverseBackground = new ReverseBackground(this);
    mVisibleOnStart = new VisibleOnStart(this);
    mGoneOnEnd = new GoneOnEnd(this);
    mDetail = detail;
    AutoPtr<IDrawable> d;
    detail->GetBackground((IDrawable**)&d);
    mBackground = ITransitionDrawable::Probe(d);
}

void QSDetailClipper::AnimateCircularClip(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Boolean in,
    /* [in] */ IAnimatorListener* listener)
{
    if (mAnimator != NULL) {
        mAnimator->Cancel();
        mAnimator = NULL;
    }

    Int32 tmp = 0;
    const Int32 w = (mDetail->GetWidth(&tmp), tmp) - x;
    const Int32 h = (mDetail->GetHeight(&tmp), tmp) - y;
    Int32 r = (Int32) Elastos::Core::Math::Ceil(Elastos::Core::Math::Sqrt(x * x + y * y));
    r = (Int32) Elastos::Core::Math::Max((Double)r,
        Elastos::Core::Math::Ceil(Elastos::Core::Math::Sqrt(w * w + y * y)));
    r = (Int32) Elastos::Core::Math::Max((Double)r,
        Elastos::Core::Math::Ceil(Elastos::Core::Math::Sqrt(w * w + h * h)));
    r = (Int32) Elastos::Core::Math::Max((Double)r,
        Elastos::Core::Math::Ceil(Elastos::Core::Math::Sqrt(x * x + h * h)));

    AutoPtr<IViewAnimationUtilsHelper> helper;
    CViewAnimationUtilsHelper::AcquireSingleton((IViewAnimationUtilsHelper**)&helper);
    if (in) {
        helper->CreateCircularReveal(mDetail, x, y, 0, r, (IAnimator**)&mAnimator);
    }
    else {
        helper->CreateCircularReveal(mDetail, x, y, r, 0, (IAnimator**)&mAnimator);
    }
    Int64 dr = 0;
    mAnimator->GetDuration(&dr);
    mAnimator->SetDuration((Int64)(dr * 1.5));
    if (listener != NULL) {
        mAnimator->AddListener(listener);
    }
    mDetail->SetLayerType(IView::LAYER_TYPE_HARDWARE, NULL);
    if (in) {
        mAnimator->GetDuration(&dr);
        mBackground->StartTransition((Int32)(dr * 0.6));
        mAnimator->AddListener(mVisibleOnStart);
    }
    else {
        mAnimator->GetDuration(&dr);
        Boolean tmp = FALSE;
        mDetail->PostDelayed(mReverseBackground, (Int64)(dr * 0.65), &tmp);
        mAnimator->AddListener(mGoneOnEnd);
    }
    mAnimator->Start();
}

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
