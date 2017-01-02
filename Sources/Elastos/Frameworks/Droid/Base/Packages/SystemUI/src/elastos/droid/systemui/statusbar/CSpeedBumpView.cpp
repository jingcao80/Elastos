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

#include "elastos/droid/systemui/statusbar/CSpeedBumpView.h"
#include "R.h"
#include <elastos/droid/R.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::IViewPropertyAnimator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CSpeedBumpView)
CAR_INTERFACE_IMPL(CSpeedBumpView, ExpandableView, ISpeedBumpView)
CSpeedBumpView::CSpeedBumpView()
    : mSpeedBumpHeight(0)
    , mIsVisible(FALSE)
{}

ECode CSpeedBumpView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ExpandableView::constructor(context, attrs);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::speed_bump_height, &mSpeedBumpHeight);

    AutoPtr<IContext> c;
    GetContext((IContext**)&c);

    AutoPtr<IAnimationUtils> au;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&au);
    au->LoadInterpolator(c, Elastos::Droid::R::interpolator::fast_out_slow_in,
            (IInterpolator**)&mFastOutSlowInInterpolator);
    return NOERROR;
}

ECode CSpeedBumpView::OnFinishInflate()
{
    ExpandableView::OnFinishInflate();
    AutoPtr<IView> view;
    FindViewById(R::id::speedbump_line, (IView**)&view);
    mLine = IAlphaOptimizedView::Probe(view);
    return NOERROR;
}

Int32 CSpeedBumpView::GetInitialHeight()
{
    return mSpeedBumpHeight;
}

ECode CSpeedBumpView::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mSpeedBumpHeight;
    return NOERROR;
}

ECode CSpeedBumpView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    ExpandableView::OnLayout(changed, left, top, right, bottom);
    Int32 value = 0;
    IView::Probe(mLine)->SetPivotX((IView::Probe(mLine)->GetWidth(&value), value) / 2);
    IView::Probe(mLine)->SetPivotY((IView::Probe(mLine)->GetHeight(&value), value) / 2);
    SetOutlineProvider(NULL);
    return NOERROR;
}

ECode CSpeedBumpView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    MeasureChildren(widthMeasureSpec, heightMeasureSpec);
    Int32 height = mSpeedBumpHeight;
    SetMeasuredDimension(MeasureSpec::GetSize(widthMeasureSpec), height);
    return NOERROR;
}

ECode CSpeedBumpView::IsTransparent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode CSpeedBumpView::PerformVisibilityAnimation(
    /* [in] */ Boolean nowVisible,
    /* [in] */ Int64 delay)
{
    AnimateDivider(nowVisible, delay, NULL /* onFinishedRunnable */);
    return NOERROR;
}

ECode CSpeedBumpView::AnimateDivider(
    /* [in] */ Boolean nowVisible,
    /* [in] */ Int64 delay,
    /* [in] */ IRunnable* onFinishedRunnable)
{
    if (nowVisible != mIsVisible) {
        // Animate dividers
        Float endValue = nowVisible ? 1.0f : 0.0f;
        AutoPtr<IViewPropertyAnimator> animator;
        IView::Probe(mLine)->Animate((IViewPropertyAnimator**)&animator);
        animator->Alpha(endValue);
        animator->SetStartDelay(delay);
        animator->ScaleX(endValue);
        animator->ScaleY(endValue);
        animator->SetInterpolator(ITimeInterpolator::Probe(mFastOutSlowInInterpolator));
        animator->WithEndAction(onFinishedRunnable);
        mIsVisible = nowVisible;
    }
    else {
        if (onFinishedRunnable != NULL) {
            onFinishedRunnable->Run();
        }
    }
    return NOERROR;
}

ECode CSpeedBumpView::SetInvisible()
{
    IView::Probe(mLine)->SetAlpha(0.0f);
    IView::Probe(mLine)->SetScaleX(0.0f);
    IView::Probe(mLine)->SetScaleY(0.0f);
    mIsVisible = FALSE;
    return NOERROR;
}

ECode CSpeedBumpView::PerformRemoveAnimation(
    /* [in] */ Int64 duration,
    /* [in] */ Float translationDirection,
    /* [in] */ IRunnable* onFinishedRunnable)
{
    // TODO: Use duration
    PerformVisibilityAnimation(FALSE, 0 /* delay */);
    return NOERROR;
}

ECode CSpeedBumpView::PerformAddAnimation(
    /* [in] */ Int64 delay,
    /* [in] */ Int64 duration)
{
    // TODO: Use duration
    PerformVisibilityAnimation(TRUE, delay);
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
