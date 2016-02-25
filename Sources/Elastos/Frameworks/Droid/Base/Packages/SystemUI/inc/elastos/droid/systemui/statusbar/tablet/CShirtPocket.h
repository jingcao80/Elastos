
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CSHIRTPOCKET_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CSHIRTPOCKET_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Tablet_CShirtPocket.h"


#include "elastos/droid/systemui/statusbar/tablet/ShirtPocket.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

CarClass(CShirtPocket), public ShirtPocket
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

    CARAPI constructor(
        /* [in] */ IContext* pContext,
        /* [in] */ IAttributeSet* pAttrs,
        /* [in] */ Int32 defStyle);

    CARAPI Holding(
        /* [out] */ Boolean* result);

    CARAPI Stash(
        /* [in] */ IClipData* clipping);

    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CSHIRTPOCKET_H__
