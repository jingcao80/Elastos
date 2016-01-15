
#ifndef __ELASTOS_DROID_WIDGET_CSTACKVIEW_H__
#define __ELASTOS_DROID_WIDGET_CSTACKVIEW_H__

#include "_Elastos_Droid_Widget_CStackView.h"
#include "elastos/droid/widget/StackView.h"



namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CStackView), public StackView
{
public:

    IVIEW_METHODS_DECL()
    IVIEWGROUP_METHODS_DECL()
    IVIEWPARENT_METHODS_DECL()
    IVIEWMANAGER_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
    IADAPTERVIEW_METHODS_DECL()

    CARAPI_(PInterface) Probe(
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

    CARAPI SetDisplayedChild(
        /* [in] */ Int32 whichChild);

    CARAPI GetDisplayedChild(
        /* [out] */ Int32* child);

    CARAPI ShowNext();

    CARAPI ShowPrevious();

    CARAPI GetCurrentView(
        /* [out] */ IView** view);

    CARAPI GetInAnimation(
        /* [out] */ IObjectAnimator** animator);

    CARAPI SetInAnimation(
        /* [in] */ IObjectAnimator* inAnimation);

    CARAPI GetOutAnimation(
        /* [out] */ IObjectAnimator** animator);

    CARAPI SetOutAnimation(
        /* [in] */ IObjectAnimator* outAnimation);

    CARAPI SetInAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resourceID);

    CARAPI SetOutAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resourceID);

    CARAPI SetAnimateFirstView(
        /* [in] */ Boolean animate);

    CARAPI SetRemoteViewsOnClickHandler(
        /* [in] */ IRemoteViewsOnClickHandler* handler);

    CARAPI SetRemoteViewsAdapter(
        /* [in] */ IIntent* intent);

    CARAPI Advance();

    CARAPI FyiWillBeAdvancedByHostKThx();

    CARAPI OnRemoteAdapterConnected(
        /* [out] */ Boolean* res);

    CARAPI OnRemoteAdapterDisconnected();

    CARAPI DeferNotifyDataSetChanged();

};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CSTACKVIEW_H__
