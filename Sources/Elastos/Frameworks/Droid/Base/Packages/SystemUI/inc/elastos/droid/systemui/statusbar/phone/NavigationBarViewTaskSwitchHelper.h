
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BOUNCELNTERPOLATOR_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BOUNCELNTERPOLATOR_H__

#include "_SystemUI.h"
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
    CAR_INTERFACE_DECL();

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
    AutoPtr<IBaseStatusBar> mBar;
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
