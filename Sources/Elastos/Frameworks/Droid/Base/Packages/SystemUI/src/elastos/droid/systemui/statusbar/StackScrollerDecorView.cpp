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

#include "elastos/droid/systemui/statusbar/StackScrollerDecorView.h"
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBar.h"

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::SystemUI::StatusBar::Phone::CPhoneStatusBar;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::IViewPropertyAnimator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

StackScrollerDecorView::ViewRunnable::ViewRunnable(
    /* [in] */ StackScrollerDecorView* host,
    /* [in] */ IRunnable* onFinishedRunnable)
    : mHost(host)
    , mOnFinishedRunnable(onFinishedRunnable)
{}

ECode StackScrollerDecorView::ViewRunnable::Run()
{
    mHost->mAnimating = FALSE;
    if (mOnFinishedRunnable != NULL) {
        mOnFinishedRunnable->Run();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(StackScrollerDecorView, ExpandableView, IStackScrollerDecorView)
StackScrollerDecorView::StackScrollerDecorView()
    : mIsVisible(FALSE)
    , mAnimating(FALSE)
    , mWillBeGone(FALSE)
{}

ECode StackScrollerDecorView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ExpandableView::constructor(context, attrs);
}

ECode StackScrollerDecorView::OnFinishInflate()
{
    ExpandableView::OnFinishInflate();
    mContent = FindContentView();
    SetInvisible();
    return NOERROR;
}

ECode StackScrollerDecorView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    ExpandableView::OnLayout(changed, left, top, right, bottom);
    SetOutlineProvider(NULL);
    return NOERROR;
}

ECode StackScrollerDecorView::IsTransparent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode StackScrollerDecorView::PerformVisibilityAnimation(
    /* [in] */ Boolean nowVisible)
{
    AnimateText(nowVisible, NULL /* onFinishedRunnable */);
    return NOERROR;
}

ECode StackScrollerDecorView::PerformVisibilityAnimation(
    /* [in] */ Boolean nowVisible,
    /* [in] */ IRunnable* onFinishedRunnable)
{
    AnimateText(nowVisible, onFinishedRunnable);
    return NOERROR;
}

ECode StackScrollerDecorView::IsVisible(
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible);
    *visible = mIsVisible || mAnimating;
    return NOERROR;
}

void StackScrollerDecorView::AnimateText(
    /* [in] */ Boolean nowVisible,
    /* [in] */ IRunnable* onFinishedRunnable)
{
    if (nowVisible != mIsVisible) {
        // Animate text
        Float endValue = nowVisible ? 1.0f : 0.0f;
        AutoPtr<IInterpolator> interpolator;
        if (nowVisible) {
            interpolator = CPhoneStatusBar::ALPHA_IN;
        }
        else {
            interpolator = CPhoneStatusBar::ALPHA_OUT;
        }
        mAnimating = TRUE;
        AutoPtr<IViewPropertyAnimator> anim;
        mContent->Animate((IViewPropertyAnimator**)&anim);
        anim->Alpha(endValue);
        anim->SetInterpolator(ITimeInterpolator::Probe(interpolator));
        anim->SetDuration(260);
        AutoPtr<ViewRunnable> run = new ViewRunnable(this, onFinishedRunnable);
        anim->WithEndAction(run);
        mIsVisible = nowVisible;
    }
    else {
        if (onFinishedRunnable != NULL) {
            onFinishedRunnable->Run();
        }
    }
}

ECode StackScrollerDecorView::SetInvisible()
{
    mContent->SetAlpha(0.0f);
    mIsVisible = FALSE;
    return NOERROR;
}

ECode StackScrollerDecorView::PerformRemoveAnimation(
    /* [in] */ Int64 duration,
    /* [in] */ Float translationDirection,
    /* [in] */ IRunnable* onFinishedRunnable)
{
    // TODO: Use duration
    PerformVisibilityAnimation(FALSE);
    return NOERROR;
}

ECode StackScrollerDecorView::PerformAddAnimation(
    /* [in] */ Int64 delay,
    /* [in] */ Int64 duration)
{
    // TODO: use delay and duration
    PerformVisibilityAnimation(TRUE);
    return NOERROR;
}

ECode StackScrollerDecorView::HasOverlappingRendering(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = FALSE;
    return NOERROR;
}

ECode StackScrollerDecorView::CancelAnimation()
{
    AutoPtr<IViewPropertyAnimator> anim;
    mContent->Animate((IViewPropertyAnimator**)&anim);
    anim->Cancel();
    return NOERROR;
}

ECode StackScrollerDecorView::WillBeGone(
    /* [out] */ Boolean* beGone)
{
    VALIDATE_NOT_NULL(beGone);
    *beGone = mWillBeGone;
    return NOERROR;
}

ECode StackScrollerDecorView::SetWillBeGone(
    /* [in] */ Boolean willBeGone)
{
    mWillBeGone = willBeGone;
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
