#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CANIMATEDIMAGEVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CANIMATEDIMAGEVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CAnimatedImageView.h"


#include "elastos/droid/systemui/statusbar/AnimatedImageView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CarClass(CAnimatedImageView), public AnimatedImageView
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
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);
};


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_CANIMATEDIMAGEVIEW_H__
