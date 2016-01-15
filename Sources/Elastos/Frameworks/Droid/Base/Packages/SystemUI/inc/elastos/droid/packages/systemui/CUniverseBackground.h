#ifndef __ELASTOS_DROID_SYSTEMUI_CUNHIVERSEBZACKGROUND_H__
#define __ELASTOS_DROID_SYSTEMUI_CUNHIVERSEBZACKGROUND_H__

#include "_Elastos_Droid_SystemUI_CUniverseBackground.h"



#include "elastos/droid/systemui/UniverseBackground.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {

CarClass(CUniverseBackground), public UniverseBackground
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
        /* [in] */ IContext* context);

    CARAPI GetWindowManagerLayoutParams(
        /* [out] */ IWindowManagerLayoutParams** lp);

    CARAPI AnimateCollapse();

    CARAPI AnimateCollapse(
        /* [in] */ Float velocityMultiplier);

    CARAPI ConsumeEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);
};

}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_CUNHIVERSEBZACKGROUND_H__
