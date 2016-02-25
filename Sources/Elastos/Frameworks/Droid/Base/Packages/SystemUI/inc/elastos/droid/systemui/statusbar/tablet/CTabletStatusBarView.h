#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CTABLETSTATUSBARVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CTABLETSTATUSBARVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Tablet_CTabletStatusBarView.h"



#include "elastos/droid/systemui/statusbar/tablet/TabletStatusBarView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

CarClass(CTabletStatusBarView), public TabletStatusBarView
{
public:
    IVIEW_METHODS_DECL()
    IVIEWGROUP_METHODS_DECL()
    IVIEWPARENT_METHODS_DECL()
    IVIEWMANAGER_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
    IFRAMELAYOUT_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetDelegateView(
        /* [in] */ IView* view);

    CARAPI SetBar(
        /* [in] */ IBaseStatusBar* bar);

    CARAPI SetHandler(
        /* [in] */ IHandler* h);

    /**
     * Let the status bar know that if you tap on ignore while panel is showing, don't do anything.
     *
     * Debounces taps on, say, a popup's trigger when the popup is already showing.
     */
    CARAPI SetIgnoreChildren(
        /* [in] */ Int32 index,
        /* [in] */ IView* ignore,
        /* [in] */ IView* panel);

    CARAPI SetShowVolume(
        /* [in] */ Boolean show,
        /* [in] */ IContext* context);
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CTABLETSTATUSBARVIEW_H__
