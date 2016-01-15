

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSTATUSBARICONVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSTATUSBARICONVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CStatusBarIconView.h"


#include "elastos/droid/systemui/statusbar/StatusBarIconView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {


CarClass(CStatusBarIconView), public StatusBarIconView
{
public:
    IVIEW_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
    IIMAGEVIEW_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& slot,
        /* [in] */ INotification* notification);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Set(
        /* [in] */ IStatusBarIcon* icon,
        /* [out] */ Boolean* succeeded);

    CARAPI GetStatusBarIcon(
        /* [out] */ IStatusBarIcon** icon);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);
};

}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSTATUSBARICONVIEW_H__
