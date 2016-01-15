#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSIGNALCLUSTERVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSIGNALCLUSTERVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CSignalClusterView.h"



#include "elastos/droid/systemui/statusbar/SignalClusterView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {


CarClass(CSignalClusterView), public SignalClusterView
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
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defaultStyle);

    CARAPI SetNetworkController(
        /* [in] */ INetworkController* nc);

    CARAPI SetWifiIndicators(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 strengthIcon,
        /* [in] */ Int32 activityIcon,
        /* [in] */ const String& contentDescription);

    CARAPI SetMobileDataIndicators(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 strengthIcon,
        /* [in] */ Int32 activityIcon,
        /* [in] */ Int32 typeIcon,
        /* [in] */ const String& contentDescription,
        /* [in] */ const String& typeContentDescription);

    CARAPI SetEthernetIndicators(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 stateIcon,
        /* [in] */ Int32 activityIcon,
        /* [in] */ const String& contentDescription);

    CARAPI SetIsAirplaneMode(
        /* [in] */ Boolean is,
        /* [in] */ Int32 airplaneIcon);
};


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSIGNALCLUSTERVIEW_H__
