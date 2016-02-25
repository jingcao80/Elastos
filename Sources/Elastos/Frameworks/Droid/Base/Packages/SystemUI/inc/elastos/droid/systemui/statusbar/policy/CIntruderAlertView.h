#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CINTRUDERALERTVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CINTRUDERALERTVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CIntruderAlertView.h"



#include "elastos/droid/systemui/statusbar/policy/IntruderAlertView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CarClass(CIntruderAlertView), public IntruderAlertView
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
        /*  [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetBar(
        /* [in] */ IBaseStatusBar* bar);

    CARAPI ApplyIntruderContent(
        /* [in] */ IRemoteViews* intruderView,
        /* [in] */ IViewOnClickListener* listener);

    CARAPI GetChildAtPosition(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ IView** result);

    CARAPI GetChildContentView(
        /* [in] */ IView* v,
        /* [out] */ IView** result);

    CARAPI CanChildBeDismissed(
        /* [in] */ IView* v,
        /* [out] */ Boolean* result);

    CARAPI OnBeginDrag(
        /* [in] */ IView* v);

    CARAPI OnChildDismissed(
        /* [in] */ IView* v);

    CARAPI OnDragCancelled(
        /* [in] */ IView* v);
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CINTRUDERALERTVIEW_H__
