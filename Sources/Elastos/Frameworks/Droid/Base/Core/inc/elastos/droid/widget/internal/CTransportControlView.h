#include "_Elastos_Droid_Widget_Internal_CTransportControlView.h"
#include "TransportControlView.h"


namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

CarClass(CTransportControlView) , public TransportControlView
{
public:
    IVIEW_METHODS_DECL()

    IFRAMELAYOUT_METHODS_DECL()

    IVIEWGROUP_METHODS_DECL()

    IVIEWPARENT_METHODS_DECL()

    IVIEWMANAGER_METHODS_DECL()

    IDRAWABLECALLBACK_METHODS_DECL()

    IKEYEVENTCALLBACK_METHODS_DECL()

    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI OnClick(
        /* [in] */ IView* v);

    CARAPI SetCallback(
        /* [in] */ ILockScreenWidgetCallback* cb);

    CARAPI ProvidesClock(
        /* [out] */ Boolean* clock);
};

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos
