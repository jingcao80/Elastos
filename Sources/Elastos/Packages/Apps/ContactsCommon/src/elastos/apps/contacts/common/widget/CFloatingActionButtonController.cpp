
#include "CFloatingActionButtonController.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Widget {

CAR_INTERFACE_IMPL(CFloatingActionButtonController, Object, IFloatingActionButtonController)

CAR_OBJECT_IMPL(CFloatingActionButtonController)

ECode CFloatingActionButtonController::constructor(
    /* [in] */ IActivity* activity,
    /* [in] */ IView* container,
    /* [in] */ IView* button)
{
    AutoPtr<IResources> resources;
    activity->GetResources((IResources**)&resources);
    AnimationUtils::LoadInterpolator(activity,
            Elastos::R::interpolator::fast_out_slow_in, (IInterpolator**)&mFabInterpolator);
    resources->GetDimensionPixelSize(
            R::dimen::floating_action_button_width, &mFloatingActionButtonWidth);
    resources->GetDimensionPixelOffset(
            R::dimen::floating_action_button_margin_right, &mFloatingActionButtonMarginRight);
    resources->GetInteger(
            R::integer::floating_action_button_animation_duration, &mAnimationDuration);
    mFloatingActionButtonContainer = container;
    mFloatingActionButton = button;
    ViewUtil::SetupFloatingActionButton(mFloatingActionButtonContainer, resources);

    return NOERROR;
}

ECode CFloatingActionButtonController::SetScreenWidth(
    /* [in] */ Int32 screenWidth)
{
    mScreenWidth = screenWidth;
    return NOERROR;
}

ECode CFloatingActionButtonController::SetVisible(
    /* [in] */ Boolean visible)
{
    mFloatingActionButtonContainer->SetVisibility(visible ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode CFloatingActionButtonController::OnPageScrolled(
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

ECode CFloatingActionButtonController::Align(
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
    if (animate && mFloatingActionButtonContainer->IsShown(&shown), shown) {
        AutoPtr<IViewPropertyAnimator> animator;
        mFloatingActionButtonContainer->Animate(&animator);
        animator->TranslationX(translationX + offsetX);
        animator->TranslationY(offsetY);
        animator->SetInterpolator(mFabInterpolator);
        animator->SetDuration(mAnimationDuration);
        animator->Start();
    }
    else {
        mFloatingActionButtonContainer->SetTranslationX(translationX + offsetX);
        mFloatingActionButtonContainer->SetTranslationY(offsetY);
    }

    return NOERROR;
}

ECode CFloatingActionButtonController::Resize(
    /* [in] */ Int32 dimension,
    /* [in] */ Boolean animate)
{
    if (animate) {
        assert(0 && "TODO");
        // AnimUtils::ChangeDimensions(mFloatingActionButtonContainer, dimension, dimension);
    } else {
        AutoPtr<IViewGroupLayoutParams> params;
        mFloatingActionButtonContainer->GetLayoutParams(
                (IViewGroupLayoutParams**)&params);
        params->SetWidth(dimension);
        params->SetHeight(dimension);
        mFloatingActionButtonContainer->RequestLayout();
    }

    return NOERROR;
}

ECode CFloatingActionButtonController::ScaleIn(
    /* [in] */ Int32 delayMs)
{
    SetVisible(true);
    assert(0 && "TODO");
    // AnimUtils::ScaleIn(mFloatingActionButtonContainer, FAB_SCALE_IN_DURATION, delayMs);
    // AnimUtils::FadeIn(mFloatingActionButton, FAB_SCALE_IN_DURATION,
    //         delayMs + FAB_SCALE_IN_FADE_IN_DELAY, NULL);
    return NOERROR;
}

ECode CFloatingActionButtonController::ScaleOut()
{
    assert(0 && "TODO");
    // AnimUtils::ScaleOut(mFloatingActionButtonContainer, mAnimationDuration);
    // // Fade out the icon faster than the scale out animation, so that the icon scaling is less
    // // obvious. We don't want it to scale, but the resizing the container is not as performant.
    // AnimUtils::FadeOut(mFloatingActionButton, FAB_ICON_FADE_OUT_DURATION, NULL);
    return NOERROR;
}

ECode CFloatingActionButtonController::GetTranslationXForAlignment(
    /* [in] */ Int32 align,
    /* [out] */ Int32* result)
{
    VALUE_NOT_NULL(result);

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

Boolean CFloatingActionButtonController::IsLayoutRtl()
{
    Int32 direction;
    mFloatingActionButtonContainer->GetLayoutDirection(&direction);
    return direction == IView::LAYOUT_DIRECTION_RTL;
}

} // Widget
} // Common
} // Contacts
} // Apps
} // Elastos
