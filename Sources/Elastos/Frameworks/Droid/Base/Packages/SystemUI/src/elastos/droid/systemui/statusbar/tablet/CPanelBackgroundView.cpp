#include "elastos/droid/systemui/statusbar/tablet/CPanelBackgroundView.h"

using Elastos::Droid::View::EIID_View;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


IVIEW_METHODS_IMPL(CPanelBackgroundView, PanelBackgroundView)
IDRAWABLECALLBACK_METHODS_IMPL(CPanelBackgroundView, PanelBackgroundView)
IKEYEVENTCALLBACK_METHODS_IMPL(CPanelBackgroundView, PanelBackgroundView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CPanelBackgroundView, PanelBackgroundView)


PInterface CPanelBackgroundView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    return _CPanelBackgroundView::Probe(riid);
}

ECode CPanelBackgroundView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return PanelBackgroundView::Init(context, attrs);
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
