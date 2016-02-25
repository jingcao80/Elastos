#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CNOTIFICATIONROWLAYOUT_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CNOTIFICATIONROWLAYOUT_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CNotificationRowLayout.h"



#include "elastos/droid/systemui/statusbar/policy/NotificationRowLayout.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CarClass(CNotificationRowLayout), public NotificationRowLayout
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
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetLongPressListener(
        /* [in] */ IViewOnLongClickListener* listener);

    CARAPI SetOnSizeChangedListener(
        /* [in] */ IOnSizeChangedListener* l);

    CARAPI SetAnimateBounds(
        /* [in] */ Boolean anim);

    CARAPI CanChildBeDismissed(
        /* [in] */ IView* v,
        /* [out] */ Boolean* result);

    CARAPI CanChildBeExpanded(
        /* [in] */ IView* v,
        /* [out] */ Boolean* result);

    CARAPI SetUserExpandedChild(
        /* [in] */ IView* v,
        /* [in] */ Boolean userExpanded,
        /* [out] */ Boolean* result);

    CARAPI SetUserLockedChild(
        /* [in] */ IView* v,
        /* [in] */ Boolean userLocked,
        /* [out] */ Boolean* result);

    CARAPI OnChildDismissed(
        /* [in] */ IView* v);

    CARAPI OnBeginDrag(
        /* [in] */ IView* v);

    CARAPI OnDragCancelled(
        /* [in] */ IView* v);

    CARAPI GetChildAtPosition(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ IView** view);

    CARAPI GetChildAtRawPosition(
        /* [in] */ Float touchX,
        /* [in] */ Float touchY,
        /* [out] */ IView** view);

    CARAPI GetChildAtPosition(
        /* [in] */ Float touchX,
        /* [in] */ Float touchY,
        /* [out] */ IView** view);

    CARAPI GetChildContentView(
        /* [in] */ IView* v,
        /* [out] */ IView** view);

    CARAPI SetViewRemoval(
        /* [in] */ Boolean removeViews);

    CARAPI SetLayoutTransitionsEnabled(
        /* [in] */ Boolean b);

    CARAPI DismissRowAnimated(
        /* [in] */ IView* child);

    CARAPI DismissRowAnimated(
        /* [in] */ IView* child,
        /* [in] */ Int32 vel);
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CNOTIFICATIONROWLAYOUT_H__
