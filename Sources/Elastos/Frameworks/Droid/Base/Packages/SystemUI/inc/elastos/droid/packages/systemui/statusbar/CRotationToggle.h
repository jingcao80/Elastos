
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CROTATIONTOGGLE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CROTATIONTOGGLE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CRotationToggle.h"



#include "elastos/droid/systemui/statusbar/RotationToggle.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CarClass(CRotationToggle), public RotationToggle
{
public:
    IVIEW_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
    ITEXTVIEW_METHODS_DECL()
    ICHECKABLE_METHODS_DECL()
    ICOMPOUNDBUTTON_METHODS_DECL()

    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetRotationLockControlVisibility(
        /* [in] */ Boolean show);

    CARAPI OnPreDraw(
        /* [out] */ Boolean* result);

    CARAPI GetButtonDrawable(
        /* [out] */ IDrawable** d);
};


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_CROTATIONTOGGLE_H__
