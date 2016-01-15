#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CNOTIFICATIONPANELTITLE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CNOTIFICATIONPANELTITLE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Tablet_CNotificationPanelTitle.h"



#include "elastos/droid/systemui/statusbar/tablet/NotificationPanelTitle.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

CarClass(CNotificationPanelTitle), public NotificationPanelTitle
{
public:
    IVIEW_METHODS_DECL()
    IVIEWGROUP_METHODS_DECL()
    IVIEWPARENT_METHODS_DECL()
    IVIEWMANAGER_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
    IRELATIVELAYOUT_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetPanel(
        /* [in] */ INotificationPanel* b);

    CARAPI OnClick(
        /* [in] */ IView* v);
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CNOTIFICATIONPANELTITLE_H__
