#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CNAVIGATIONBARVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CNAVIGATIONBARVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CNavigationBarView.h"



#include "elastos/droid/systemui/statusbar/phone/NavigationBarView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CNavigationBarView), public NavigationBarView
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

    CARAPI SetDelegateView(
        /* [in] */ IView* view);

    CARAPI SetBar(
        /* [in] */ IBaseStatusBar* phoneStatusBar);

    CARAPI GetRecentsButton(
        /* [out] */ IView** view);

    CARAPI GetMenuButton(
        /* [out] */ IView** view);

    CARAPI GetBackButton(
        /* [out] */ IView** view);

    CARAPI GetHomeButton(
        /* [out] */ IView** view);

    CARAPI GetScreenShotButton(
        /* [out] */ IView** view);

    CARAPI GetVolumeDownButton(
        /* [out] */ IView** view);

    CARAPI GetVolumeUpButton(
        /* [out] */ IView** view);

    // for when home is disabled, but search isn't
    CARAPI GetSearchLight(
        /* [out] */ IView** view);

    CARAPI NotifyScreenOn(
        /* [in] */ Boolean screenOn);

    CARAPI SetNavigationIconHints(
        /* [in] */ Int32 hints);

    CARAPI SetNavigationIconHints(
        /* [in] */ Int32 hints,
        /* [in] */ Boolean force);

    CARAPI SetDisabledFlags(
        /* [in] */ Int32 disabledFlags);

    CARAPI SetDisabledFlags(
        /* [in] */ Int32 disabledFlags,
        /* [in] */ Boolean force);

    CARAPI SetSlippery(
        /* [in] */ Boolean newSlippery);

    CARAPI SetMenuVisibility(
        /* [in] */ Boolean show);

    CARAPI SetMenuVisibility(
        /* [in] */ Boolean show,
        /* [in] */ Boolean force);

    CARAPI SetLowProfile(
        /* [in] */ Boolean lightsOut);

    CARAPI SetLowProfile(
        /* [in] */ Boolean lightsOut,
        /* [in] */ Boolean animate,
        /* [in] */ Boolean force);

    CARAPI SetHidden(
        /* [in] */ Boolean hide);

    CARAPI Reorient();

};


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CNAVIGATIONBARVIEW_H__
