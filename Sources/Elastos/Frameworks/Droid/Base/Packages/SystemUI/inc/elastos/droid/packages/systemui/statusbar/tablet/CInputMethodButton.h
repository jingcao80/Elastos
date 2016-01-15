
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CINPUTMETHODBUTTON_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CINPUTMETHODBUTTON_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Tablet_CInputMethodButton.h"


#include "elastos/droid/systemui/statusbar/tablet/InputMethodButton.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

CarClass(CInputMethodButton), public InputMethodButton
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
        /* [in] */ IContext* pCtx,
        /* [in] */ IAttributeSet* pAttrs);

    CARAPI SetIconImage(
        /* [in] */ Int32 resId);

    CARAPI SetImeWindowStatus(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean showButton);

    CARAPI SetHardKeyboardStatus(
        /* [in] */ Boolean available);

    CARAPI SetScreenLocked(
        /* [in] */ Boolean available);

    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CINPUTMETHODBUTTON_H__
