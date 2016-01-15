#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CNOTIFICATIONLINEARLAYOUT_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CNOTIFICATIONLINEARLAYOUT_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Tablet_CNotificationLinearLayout.h"



#include "elastos/droid/systemui/statusbar/tablet/NotificationLinearLayout.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

CarClass(CNotificationLinearLayout), public NotificationLinearLayout
{
public:
    IVIEW_METHODS_DECL()
    IVIEWGROUP_METHODS_DECL()
    IVIEWPARENT_METHODS_DECL()
    IVIEWMANAGER_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
    ILINEARLAYOUT_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CNOTIFICATIONLINEARLAYOUT_H__
