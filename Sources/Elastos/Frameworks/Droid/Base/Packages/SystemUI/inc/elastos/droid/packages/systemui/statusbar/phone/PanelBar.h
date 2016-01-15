#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELBAR_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELBAR_H__

#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {


class PanelBar : public FrameLayout
{
public:
    PanelBar();

    PanelBar(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Go(
        /* [in] */ Int32 state);

    CARAPI AddPanel(
        /* [in] */ IPanelView* panelView);

    CARAPI SetPanelHolder(
        /* [in] */ IPanelHolder* ph);

    CARAPI_(Float) GetBarHeight();

    CARAPI_(AutoPtr<IPanelView>) SelectPanelForTouch(
        /* [in] */ IMotionEvent* touch);

    CARAPI_(Boolean) PanelsEnabled();

    // called from PanelView when self-expanding, too
    CARAPI StartOpeningPanel(
        /* [in] */ IPanelView* panelView);

    CARAPI PanelExpansionChanged(
        /* [in] */ IPanelView* panelView,
        /* [in] */ Float frac);

    CARAPI CollapseAllPanels(
        /* [in] */ Boolean animate);

    CARAPI OnPanelPeeked();

    CARAPI OnAllPanelsCollapsed();

    CARAPI OnPanelFullyOpened(
        /* [in] */ IPanelView* panelView);

    CARAPI OnTrackingStarted(
        /* [in] */ IPanelView* panelView);

    CARAPI OnTrackingStopped(
        /* [in] */ IPanelView* panelView);

    //@Override
    virtual Boolean OnTouchEvent(
        /* [in] */ IMotionEvent* event);

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
