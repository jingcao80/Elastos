
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELBAR_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELBAR_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class PanelBar
    : public FrameLayout
    , public IPanelBar
{
public:
    CAR_INTERFACE_DECL();

    PanelBar();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Go(
        /* [in] */ Int32 state);

    CARAPI AddPanel(
        /* [in] */ IPanelView* panelView);

    CARAPI SetPanelHolder(
        /* [in] */ IPanelHolder* ph);

    CARAPI GetBarHeight(
        /* [out] */ Float* result);

    CARAPI_(AutoPtr<IPanelView>) SelectPanelForTouch(
        /* [in] */ IMotionEvent* touch);

    CARAPI_(Boolean) PanelsEnabled();

    // called from PanelView when self-expanding, too
    CARAPI StartOpeningPanel(
        /* [in] */ IPanelView* panelView);

    /**
     * @param panel the panel which changed its expansion state
     * @param frac the fraction from the expansion in [0, 1]
     * @param expanded whether the panel is currently expanded; this is independent from the
     *                 fraction as the panel also might be expanded if the fraction is 0
     */
    CARAPI PanelExpansionChanged(
        /* [in] */ IPanelView* panelView,
        /* [in] */ Float frac,
        /* [in] */ Boolean expanded);

    CARAPI CollapseAllPanels(
        /* [in] */ Boolean animate);

    CARAPI OnPanelPeeked();

    CARAPI OnAllPanelsCollapsed();

    CARAPI OnPanelFullyOpened(
        /* [in] */ IPanelView* panelView);

    CARAPI OnTrackingStarted(
        /* [in] */ IPanelView* panelView);

    CARAPI OnTrackingStopped(
        /* [in] */ IPanelView* panelView,
        /* [in] */ Boolean expanded);

    CARAPI OnExpandingFinished();

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

protected:
    //@Override
    virtual CARAPI OnFinishInflate();

protected:
    AutoPtr<IPanelHolder> mPanelHolder;
    List<AutoPtr<IPanelView> > mPanels;
    AutoPtr<IPanelView> mTouchingPanel;
    Float mPanelExpandedFractionSum;

private:
    static const String TAG;
    Int32 mState;//= STATE_CLOSED;
    Boolean mTracking;
};


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELBAR_H__
