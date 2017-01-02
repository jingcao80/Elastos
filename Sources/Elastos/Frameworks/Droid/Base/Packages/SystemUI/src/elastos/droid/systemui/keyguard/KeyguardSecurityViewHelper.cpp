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

#include "elastos/droid/systemui/keyguard/KeyguardSecurityViewHelper.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode KeyguardSecurityViewHelper::MyAnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    // Fail safe and show the emergency button in onAnimationEnd()
    mCanceled = TRUE;
    return mEcaView->SetAlpha(1.0f);
}

ECode KeyguardSecurityViewHelper::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return mEcaView->SetVisibility(mCanceled ? IView::VISIBLE : IView::INVISIBLE);
}

ECode KeyguardSecurityViewHelper::ShowBouncer(
    /* [in] */ ISecurityMessageDisplay* securityMessageDisplay,
    /* [in] */ IView* ecaView,
    /* [in] */ IDrawable* bouncerFrame,
    /* [in] */ Int32 duration)
{
    if (securityMessageDisplay != NULL) {
        securityMessageDisplay->ShowBouncer(duration);
    }
    if (ecaView != NULL) {
        if (duration > 0) {
            AutoPtr<IObjectAnimatorHelper> helper;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);

            AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
            (*array)[0] = 0.0f;
            AutoPtr<IObjectAnimator> oa;
            helper->OfFloat(ecaView, String("alpha"), array,
                    (IObjectAnimator**)&oa);
            AutoPtr<IAnimator> anim = IAnimator::Probe(oa);
            anim->SetDuration(duration);

            AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter(ecaView);
            anim->AddListener(lis);
            anim->Start();
        }
        else {
            ecaView->SetAlpha(0.0f);
            ecaView->SetVisibility(IView::INVISIBLE);
        }
    }
    if (bouncerFrame != NULL) {
        if (duration > 0) {
            AutoPtr<IObjectAnimatorHelper> helper;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
            AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
            (*array)[0] = 0;
            (*array)[1] = 255;
            AutoPtr<IObjectAnimator> oa;
            helper->OfInt32(bouncerFrame, String("alpha"), array,
                    (IObjectAnimator**)&oa);
            AutoPtr<IAnimator> anim = IAnimator::Probe(oa);
            anim->SetDuration(duration);
            anim->Start();
        }
        else {
            bouncerFrame->SetAlpha(255);
        }
    }
    return NOERROR;
}

ECode KeyguardSecurityViewHelper::HideBouncer(
    /* [in] */ ISecurityMessageDisplay* securityMessageDisplay,
    /* [in] */ IView* ecaView,
    /* [in] */ IDrawable* bouncerFrame,
    /* [in] */ Int32 duration)
{
    if (securityMessageDisplay != NULL) {
        securityMessageDisplay->HideBouncer(duration);
    }
    if (ecaView != NULL) {
        ecaView->SetVisibility(IView::VISIBLE);
        if (duration > 0) {
            AutoPtr<IObjectAnimatorHelper> helper;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);

            AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
            (*array)[0] = 1.0f;
            AutoPtr<IObjectAnimator> oa;
            helper->OfFloat(ecaView, String("alpha"), array,
                    (IObjectAnimator**)&oa);
            AutoPtr<IAnimator> anim = IAnimator::Probe(oa);
            anim->SetDuration(duration);
            anim->Start();
        }
        else {
            ecaView->SetAlpha(1.0f);
        }
    }
    if (bouncerFrame != NULL) {
        if (duration > 0) {
            AutoPtr<IObjectAnimatorHelper> helper;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
            AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
            (*array)[0] = 255;
            (*array)[1] = 0;
            AutoPtr<IObjectAnimator> oa;
            helper->OfInt32(bouncerFrame, String("alpha"), array,
                    (IObjectAnimator**)&oa);
            AutoPtr<IAnimator> anim = IAnimator::Probe(oa);
            anim->SetDuration(duration);
            anim->Start();
        }
        else {
            bouncerFrame->SetAlpha(0);
        }
    }
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
