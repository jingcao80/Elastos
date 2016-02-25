#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CINPUTMETHODSPANEL_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CINPUTMETHODSPANEL_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Tablet_CInputMethodsPanel.h"



#include "elastos/droid/systemui/statusbar/tablet/InputMethodsPanel.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

CarClass(CInputMethodsPanel), public InputMethodsPanel
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

    CARAPI SetHardKeyboardEnabledChangeListener(
        /* [in] */ IOnHardKeyboardEnabledChangeListener* listener);

    CARAPI OpenPanel();

    CARAPI ClosePanel(
        /* [in] */ Boolean closeKeyboard);

    CARAPI SetImeToken(
        /* [in] */ IBinder* token);

    CARAPI SetImeSwitchButton(
        /* [in] */ IInputMethodButton* imb);

    CARAPI SetHardKeyboardStatus(
        /* [in] */ Boolean available,
        /* [in] */ Boolean enabled);

    CARAPI OnClick(
        /* [in] */ IView* v);

    CARAPI IsInContentArea(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Boolean* result);
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CINPUTMETHODSPANEL_H__
