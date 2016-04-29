#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_DELEGATEVIEWHELPER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_DELEGATEVIEWHELPER_H__

#include <elastos/droid/ext/frameworkext.h>
#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::SystemUI::StatusBar::IBaseStatusBar;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

class DelegateViewHelper : public Object
{
public:
    DelegateViewHelper(
        /* [in] */ IView* sourceView);

    CARAPI_(void) SetDelegateView(
        /* [in] */ IView* view);

    CARAPI_(void) SetBar(
        /* [in] */ IBaseStatusBar* phoneStatusBar);

    CARAPI_(Boolean) OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) SetSourceView(
        /* [in] */ IView* view);

    /**
     * Selects the initial touch region based on a list of views.  This is meant to be called by
     * a container widget on children over which the initial touch should be detected.  Note this
     * will compute a minimum bound that contains all specified views.
     *
     * @param views
     */
    CARAPI_(void) SetInitialTouchRegion(
        /* [in] */ ArrayOf<IView*>* views);

    /**
     * When rotation is set to NO_SENSOR, then this allows swapping x/y for gesture detection
     * @param swap
     */
    CARAPI_(void) SetSwapXY(
        /* [in] */ Boolean swap);

    CARAPI_(void) SetDisabled(
        /* [in] */ Boolean disabled);

public:
    AutoPtr<IRectF> mInitialTouch;
    AutoPtr<IBaseStatusBar> mBar;

private:
    AutoPtr<IView> mDelegateView;
    AutoPtr<IView> mSourceView;
    AutoPtr<ArrayOf<Int32> > mTempPoint;
    Float mDownPoint[2];
    Float mTriggerThreshhold;
    Boolean mPanelShowing;
    Boolean mStarted;
    Boolean mSwapXY;
    Boolean mDisabled;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_DELEGATEVIEWHELPER_H__
