#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_DELEGATEVIEWHELPER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_DELEGATEVIEWHELPER_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::SystemUI::StatusBar::IBaseStatusBar;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

class DelegateViewHelper : public ElRefBase
{
public:
    DelegateViewHelper(
        /* [in] */ IView* sourceView);

    void SetDelegateView(
        /* [in] */ IView* view);

    void SetBar(
        /* [in] */ IBaseStatusBar* phoneStatusBar);

    Boolean OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* event);

    void SetSourceView(
        /* [in] */ IView* view);

    /**
     * Selects the initial touch region based on a list of views.  This is meant to be called by
     * a container widget on children over which the initial touch should be detected.  Note this
     * will compute a minimum bound that contains all specified views.
     *
     * @param views
     */
    void SetInitialTouchRegion(
        /* [in] */ ArrayOf<IView*>* views);

    /**
     * When rotation is set to NO_SENSOR, then this allows swapping x/y for gesture detection
     * @param swap
     */
    void SetSwapXY(
        /* [in] */ Boolean swap);

public:
    AutoPtr<IRectF> mInitialTouch;
    AutoPtr<IBaseStatusBar> mBar;
private:
    AutoPtr<IView> mDelegateView;
    AutoPtr<IView> mSourceView;
    Int32 mTempPoint[2];
    Float mDownPoint[2];
    Float mTriggerThreshhold;
    Boolean mPanelShowing;
    Boolean mStarted;
    Boolean mSwapXY;
};


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_DELEGATEVIEWHELPER_H__
