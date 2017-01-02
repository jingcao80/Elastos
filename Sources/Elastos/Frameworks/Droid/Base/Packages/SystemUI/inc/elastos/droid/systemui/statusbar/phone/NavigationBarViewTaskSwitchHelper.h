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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BOUNCELNTERPOLATOR_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BOUNCELNTERPOLATOR_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/view/GestureDetector.h>

using Elastos::Droid::View::GestureDetector;
using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class NavigationBarViewTaskSwitchHelper
    : public GestureDetector::SimpleOnGestureListener
    , public INavigationBarViewTaskSwitchHelper
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("NavigationBarViewTaskSwitchHelper")

    NavigationBarViewTaskSwitchHelper(
        /* [in] */ IContext* context);

    CARAPI SetBar(
        /* [in] */ IBaseStatusBar* phoneStatusBar);

    CARAPI SetBarState(
        /* [in] */ Boolean isVertical,
        /* [in] */ Boolean isRTL);

    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnFling(
        /* [in] */ IMotionEvent* e1,
        /* [in] */ IMotionEvent* e2,
        /* [in] */ Float velocityX,
        /* [in] */ Float velocityY,
        /* [out] */ Boolean* result);

private:
    IBaseStatusBar* mBar;
    Boolean mIsVertical;
    Boolean mIsRTL;

    AutoPtr<IGestureDetector> mTaskSwitcherDetector;
    Int32 mScrollTouchSlop;
    Int32 mMinFlingVelocity;
    Int32 mTouchDownX;
    Int32 mTouchDownY;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BOUNCELNTERPOLATOR_H__
