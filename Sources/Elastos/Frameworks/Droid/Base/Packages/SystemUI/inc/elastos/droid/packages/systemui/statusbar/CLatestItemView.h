#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CLATESTITEMVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CLATESTITEMVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CLatestItemView.h"



#include "elastos/droid/systemui/statusbar/LatestItemView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CarClass(CLatestItemView), public LatestItemView
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
};


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_CLATESTITEMVIEW_H__
