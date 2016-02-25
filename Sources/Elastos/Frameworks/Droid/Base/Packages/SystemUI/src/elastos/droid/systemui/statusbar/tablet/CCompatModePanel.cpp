#include "elastos/droid/systemui/statusbar/tablet/CCompatModePanel.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


IVIEW_METHODS_IMPL(CCompatModePanel, CompatModePanel)
IVIEWGROUP_METHODS_IMPL(CCompatModePanel, CompatModePanel)
IVIEWPARENT_METHODS_IMPL(CCompatModePanel, CompatModePanel)
IVIEWMANAGER_METHODS_IMPL(CCompatModePanel, CompatModePanel)
IDRAWABLECALLBACK_METHODS_IMPL(CCompatModePanel, CompatModePanel)
IKEYEVENTCALLBACK_METHODS_IMPL(CCompatModePanel, CompatModePanel)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CCompatModePanel, CompatModePanel)
IFRAMELAYOUT_METHODS_IMPL(CCompatModePanel, CompatModePanel)


PInterface CCompatModePanel::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CCompatModePanel::Probe(riid);
}

ECode CCompatModePanel::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return CompatModePanel::Init(context, attrs);
}

ECode CCompatModePanel::SetTrigger(
    /* [in] */  IView* view)
{
    return CompatModePanel::SetTrigger(view);
}

ECode CCompatModePanel::OpenPanel()
{
    return CompatModePanel::OpenPanel();
}

ECode CCompatModePanel::ClosePanel()
{
    return CompatModePanel::ClosePanel();
}

ECode CCompatModePanel::OnClick(
    /* [in] */ IView* v)
{
    return CompatModePanel::OnClick(v);
}

ECode CCompatModePanel::IsInContentArea(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CompatModePanel::IsInContentArea(x, y);
    return NOERROR;
}

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
