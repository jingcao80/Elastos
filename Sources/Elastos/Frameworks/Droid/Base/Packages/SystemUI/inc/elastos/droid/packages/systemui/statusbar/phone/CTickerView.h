#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CTICKERVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CTICKERVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CTickerView.h"




#include "elastos/droid/systemui/statusbar/phone/TickerView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CTickerView), public TickerView
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

    CARAPI SetTicker(
        /* [in] */ ITicker* t);
};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CTICKERVIEW_H__
