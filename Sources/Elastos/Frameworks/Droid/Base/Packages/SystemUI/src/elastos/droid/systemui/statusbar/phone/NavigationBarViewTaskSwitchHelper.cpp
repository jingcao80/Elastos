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

#include "elastos/droid/systemui/statusbar/phone/NavigationBarViewTaskSwitchHelper.h"
#include "../R.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::CGestureDetector;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

static const String TAG("NavigationBarViewTaskSwitchHelper");

CAR_INTERFACE_IMPL(NavigationBarViewTaskSwitchHelper, \
    GestureDetector::SimpleOnGestureListener, INavigationBarViewTaskSwitchHelper)

NavigationBarViewTaskSwitchHelper::NavigationBarViewTaskSwitchHelper(
    /* [in] */ IContext* context)
    : mIsVertical(FALSE)
    , mIsRTL(FALSE)
    , mScrollTouchSlop(0)
    , mMinFlingVelocity(0)
    , mTouchDownX(0)
    , mTouchDownY(0)
{
    AutoPtr<IViewConfiguration> configuration;
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    helper->Get(context, (IViewConfiguration**)&configuration);
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    r->GetDimensionPixelSize(R::dimen::navigation_bar_min_swipe_distance, &mScrollTouchSlop);
    configuration->GetScaledMinimumFlingVelocity(&mMinFlingVelocity);
    CGestureDetector::New(context, this, (IGestureDetector**)&mTaskSwitcherDetector);
}

ECode NavigationBarViewTaskSwitchHelper::SetBar(
    /* [in] */ IBaseStatusBar* phoneStatusBar)
{
    mBar = phoneStatusBar;
    return NOERROR;
}

ECode NavigationBarViewTaskSwitchHelper::SetBarState(
    /* [in] */ Boolean isVertical,
    /* [in] */ Boolean isRTL)
{
    mIsVertical = isVertical;
    mIsRTL = isRTL;
    return NOERROR;
}

ECode NavigationBarViewTaskSwitchHelper::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // If we move more than a fixed amount, then start capturing for the
    // task switcher detector
    Boolean tmp = FALSE;
    mTaskSwitcherDetector->OnTouchEvent(event, &tmp);
    Int32 action = 0;
    event->GetAction(&action);
    Boolean intercepted = FALSE;
    switch (action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_DOWN: {
            Float fv = 0;
            mTouchDownX = (Int32) (event->GetX(&fv), fv);
            mTouchDownY = (Int32) (event->GetY(&fv), fv);
            break;
        }
        case IMotionEvent::ACTION_MOVE: {
            Float fv = 0;
            Int32 x = (Int32) (event->GetX(&fv), fv);
            Int32 y = (Int32) (event->GetY(&fv), fv);
            Int32 xDiff = Elastos::Core::Math::Abs(x - mTouchDownX);
            Int32 yDiff = Elastos::Core::Math::Abs(y - mTouchDownY);
            Boolean exceededTouchSlop = !mIsVertical
                    ? xDiff > mScrollTouchSlop && xDiff > yDiff
                    : yDiff > mScrollTouchSlop && yDiff > xDiff;
            if (exceededTouchSlop) {
                *result = TRUE;
                return NOERROR;
            }
            break;
        }
        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP:
            break;
    }
    *result = intercepted;
    return NOERROR;
}

ECode NavigationBarViewTaskSwitchHelper::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mTaskSwitcherDetector->OnTouchEvent(event, result);
}

ECode NavigationBarViewTaskSwitchHelper::OnFling(
    /* [in] */ IMotionEvent* e1,
    /* [in] */ IMotionEvent* e2,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Float absVelX = Elastos::Core::Math::Abs(velocityX);
    Float absVelY = Elastos::Core::Math::Abs(velocityY);
    Boolean isValidFling = absVelX > mMinFlingVelocity &&
            mIsVertical ? (absVelY > absVelX) : (absVelX > absVelY);
    if (isValidFling) {
        Boolean showNext;
        if (!mIsRTL) {
            showNext = mIsVertical ? (velocityY < 0) : (velocityX < 0);
        }
        else {
            // In RTL, vertical is still the same, but horizontal is flipped
            showNext = mIsVertical ? (velocityY < 0) : (velocityX > 0);
        }
        if (showNext) {
            mBar->ShowNextAffiliatedTask();
        }
        else {
            mBar->ShowPreviousAffiliatedTask();
        }
    }
    *result = TRUE;
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
