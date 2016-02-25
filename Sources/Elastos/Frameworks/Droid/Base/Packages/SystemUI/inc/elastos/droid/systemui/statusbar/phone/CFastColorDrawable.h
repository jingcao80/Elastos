#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CFASTCOLORDRAWABLE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CFASTCOLORDRAWABLE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CFastColorDrawable.h"

#include "elastos/droid/systemui/statusbar/phone/FastColorDrawable.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CFastColorDrawable), public FastColorDrawable
{
public:
    IDRAWABLE_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ Int32 color);
};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CFASTCOLORDRAWABLE_H__
