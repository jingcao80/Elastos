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

#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/contacts/common/widget/FloatingActionButtonController.h"
#include "elastos/droid/contacts/common/util/ViewUtil.h"
#include "elastos/droid/phone/common/animation/AnimUtils.h"
#include "R.h"
#include <elastos/droid/view/animation/AnimationUtils.h>
#include <elastos/droid/R.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::Contacts::Common::Util::ViewUtil;
using Elastos::Droid::Phone::Common::Animation::AnimUtils;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Widget {

const Int32 FloatingActionButtonController::ALIGN_MIDDLE = 0;
const Int32 FloatingActionButtonController::ALIGN_QUARTER_END = 1;
const Int32 FloatingActionButtonController::ALIGN_END = 2;

const Int32 FloatingActionButtonController::FAB_SCALE_IN_DURATION = 266;
const Int32 FloatingActionButtonController::FAB_SCALE_IN_FADE_IN_DELAY = 100;
const Int32 FloatingActionButtonController::FAB_ICON_FADE_OUT_DURATION = 66;

ECode FloatingActionButtonController::constructor(
    /* [in] */ IActivity* activity,
    /* [in] */ IView* container,
    /* [in] */ IView* button)
{
    AutoPtr<IResources> resources;
    IContext::Probe(activity)->GetResources((IResources**)&resources);
    AnimationUtils::LoadInterpolator(IContext::Probe(activity),
            Elastos::Droid::R::interpolator::fast_out_slow_in, (IInterpolator**)&mFabInterpolator);
    resources->GetDimensionPixelSize(
            Elastos::Droid::Dialer::R::dimen::floating_action_button_width, &mFloatingActionButtonWidth);
    resources->GetDimensionPixelOffset(
            Elastos::Droid::Dialer::R::dimen::floating_action_button_margin_right, &mFloatingActionButtonMarginRight);
    resources->GetInteger(
            Elastos::Droid::Dialer::R::integer::floating_action_button_animation_duration, &mAnimationDuration);
    mFloatingActionButtonContainer = container;
    mFloatingActionButton = button;
    ViewUtil::SetupFloatingActionButton(mFloatingActionButtonContainer, resources);
    return NOERROR;
}

ECode FloatingActionButtonController::SetScreenWidth(
    /* [in] */ Int32 screenWidth)
{
    mScreenWidth = screenWidth;
    return NOERROR;
}

ECode FloatingActionButtonController::SetVisible(
    /* [in] */ Boolean visible)
{
    mFloatingActionButtonContainer->SetVisibility(visible ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode FloatingActionButtonController::OnPageScrolled(
    /* [in] */ Float positionOffset)
{
    // As the page is scrolling, if we're on the first tab, update the FAB position so it
    // moves along with it.
    Int32 result;
    GetTranslationXForAlignment(ALIGN_END, &result);
    mFloatingActionButtonContainer->SetTranslationX(
            (Int32) (positionOffset * result));
    mFloatingActionButtonContainer->SetTranslationY(0);
    return NOERROR;
}

ECode FloatingActionButtonController::Align(
    /* [in] */ Int32 align,
    /* [in] */ Int32 offsetX,
    /* [in] */ Int32 offsetY,
    /* [in] */ Boolean animate)
{
    if (mScreenWidth == 0) {
        return NOERROR;
    }

    Int32 translationX;
    GetTranslationXForAlignment(align, &translationX);

    // Skip animation if container is not shown; animation causes container to show again.
    Boolean shown = FALSE;
    if (animate && (mFloatingActionButtonContainer->IsShown(&shown), shown)) {
        AutoPtr<IViewPropertyAnimator> animator;
        mFloatingActionButtonContainer->Animate((IViewPropertyAnimator**)&animator);
        animator->TranslationX(translationX + offsetX);
        animator->TranslationY(offsetY);
        animator->SetInterpolator(ITimeInterpolator::Probe(mFabInterpolator));
        animator->SetDuration(mAnimationDuration);
        animator->Start();
    }
    else {
        mFloatingActionButtonContainer->SetTranslationX(translationX + offsetX);
        mFloatingActionButtonContainer->SetTranslationY(offsetY);
    }
    return NOERROR;
}

ECode FloatingActionButtonController::Resize(
    /* [in] */ Int32 dimension,
    /* [in] */ Boolean animate)
{
    if (animate) {
        AnimUtils::ChangeDimensions(mFloatingActionButtonContainer, dimension, dimension);
    }
    else {
        AutoPtr<IViewGroupLayoutParams> params;
        mFloatingActionButtonContainer->GetLayoutParams(
                (IViewGroupLayoutParams**)&params);
        params->SetWidth(dimension);
        params->SetHeight(dimension);
        mFloatingActionButtonContainer->RequestLayout();
    }
    return NOERROR;
}

ECode FloatingActionButtonController::ScaleIn(
    /* [in] */ Int32 delayMs)
{
    SetVisible(TRUE);
    AnimUtils::ScaleIn(mFloatingActionButtonContainer, FAB_SCALE_IN_DURATION, delayMs);
    AnimUtils::FadeIn(mFloatingActionButton, FAB_SCALE_IN_DURATION,
            delayMs + FAB_SCALE_IN_FADE_IN_DELAY, NULL);
    return NOERROR;
}

ECode FloatingActionButtonController::ScaleOut()
{
    AnimUtils::ScaleOut(mFloatingActionButtonContainer, mAnimationDuration);
    // Fade out the icon faster than the scale out animation, so that the icon scaling is less
    // obvious. We don't want it to scale, but the resizing the container is not as performant.
    AnimUtils::FadeOut(mFloatingActionButton, FAB_ICON_FADE_OUT_DURATION, NULL);
    return NOERROR;
}

ECode FloatingActionButtonController::GetTranslationXForAlignment(
    /* [in] */ Int32 align,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 value = 0;
    switch (align) {
        case ALIGN_MIDDLE:
            // Moves the FAB to exactly center screen.
            *result = 0;
            return NOERROR;
        case ALIGN_QUARTER_END:
            // Moves the FAB a quarter of the screen width.
            value = mScreenWidth / 4;
            break;
        case ALIGN_END:
            // Moves the FAB half the screen width. Same as aligning right with a marginRight.
            value = mScreenWidth / 2
                    - mFloatingActionButtonWidth / 2
                    - mFloatingActionButtonMarginRight;
            break;
    }
    if (IsLayoutRtl()) {
        value *= -1;
    }
    *result = value;
    return NOERROR;
}

Boolean FloatingActionButtonController::IsLayoutRtl()
{
    Int32 direction;
    mFloatingActionButtonContainer->GetLayoutDirection(&direction);
    return direction == IView::LAYOUT_DIRECTION_RTL;
}

} // Widget
} // Common
} // Contacts
} // Droid
} // Elastos
