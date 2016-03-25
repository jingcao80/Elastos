#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/systemui/statusbar/phone/CPanelHolder.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

IVIEW_METHODS_IMPL(CPanelHolder, PanelHolder)
IVIEWGROUP_METHODS_IMPL(CPanelHolder, PanelHolder)
IVIEWPARENT_METHODS_IMPL(CPanelHolder, PanelHolder)
IVIEWMANAGER_METHODS_IMPL(CPanelHolder, PanelHolder)
IDRAWABLECALLBACK_METHODS_IMPL(CPanelHolder, PanelHolder)
IKEYEVENTCALLBACK_METHODS_IMPL(CPanelHolder, PanelHolder)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CPanelHolder, PanelHolder)
IFRAMELAYOUT_METHODS_IMPL(CPanelHolder, PanelHolder)


PInterface CPanelHolder::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>(this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CPanelHolder::Probe(riid);
}

ECode CPanelHolder::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return PanelHolder::Init(context, attrs);
}

ECode CPanelHolder::GetPanelIndex(
    /* [in] */ IPanelView* pv,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = PanelHolder::GetPanelIndex(pv);
    return NOERROR;
}

ECode CPanelHolder::SetSelectedPanel(
    /* [in] */ IPanelView* pv)
{
    return PanelHolder::SetSelectedPanel(pv);
}

ECode CPanelHolder::SetBar(
    /* [in] */ IPanelBar* panelBar)
{
    return PanelHolder::SetBar(panelBar);
}


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
