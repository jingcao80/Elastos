#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPANELHOLDER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPANELHOLDER_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CPanelHolder.h"



#include "elastos/droid/systemui/statusbar/phone/PanelHolder.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CPanelHolder), public PanelHolder
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
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI GetPanelIndex(
        /* [in] */ IPanelView* pv,
        /* [out] */ Int32* index);

    CARAPI SetSelectedPanel(
        /* [in] */ IPanelView* pv);

    CARAPI SetBar(
        /* [in] */ IPanelBar* panelBar);
};


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPANELHOLDER_H__
