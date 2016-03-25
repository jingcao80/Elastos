#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/systemui/statusbar/phone/CPanelBar.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

IVIEW_METHODS_IMPL(CPanelBar, PanelBar)
IVIEWGROUP_METHODS_IMPL(CPanelBar, PanelBar)
IVIEWPARENT_METHODS_IMPL(CPanelBar, PanelBar)
IVIEWMANAGER_METHODS_IMPL(CPanelBar, PanelBar)
IDRAWABLECALLBACK_METHODS_IMPL(CPanelBar, PanelBar)
IKEYEVENTCALLBACK_METHODS_IMPL(CPanelBar, PanelBar)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CPanelBar, PanelBar)
IFRAMELAYOUT_METHODS_IMPL(CPanelBar, PanelBar)


PInterface CPanelBar::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>(this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CPanelBar::Probe(riid);
}

ECode CPanelBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return PanelBar::Init(context, attrs);
}

ECode CPanelBar::Go(
    /* [in] */ Int32 state)
{
    return PanelBar::Go(state);
}

ECode CPanelBar::AddPanel(
    /* [in] */ IPanelView* pv)
{
    return PanelBar::AddPanel(pv);
}

ECode CPanelBar::SetPanelHolder(
    /* [in] */ IPanelHolder* ph)
{
    return PanelBar::SetPanelHolder(ph);
}

ECode CPanelBar::GetBarHeight(
    /* [out] */ Float* height)
{
    VALIDATE_NOT_NULL(height);
    *height = PanelBar::GetBarHeight();
    return NOERROR;
}

ECode CPanelBar::SelectPanelForTouch(
    /* [in] */ IMotionEvent* touch,
    /* [out] */ IPanelView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IPanelView> v = PanelBar::SelectPanelForTouch(touch);
    *view = v;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CPanelBar::PanelsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = PanelBar::PanelsEnabled();
    return NOERROR;
}

// called from PanelView when self-expanding, too
ECode CPanelBar::StartOpeningPanel(
    /* [in] */ IPanelView* pv)
{
    return PanelBar::StartOpeningPanel(pv);
}

ECode CPanelBar::PanelExpansionChanged(
    /* [in] */ IPanelView* pv,
    /* [in] */ Float frac)
{
    return PanelBar::PanelExpansionChanged(pv, frac);
}

ECode CPanelBar::CollapseAllPanels(
    /* [in] */ Boolean animate)
{
    return PanelBar::CollapseAllPanels(animate);
}

ECode CPanelBar::OnPanelPeeked()
{
    return PanelBar::OnPanelPeeked();
}

ECode CPanelBar::OnAllPanelsCollapsed()
{
    return PanelBar::OnAllPanelsCollapsed();
}

ECode CPanelBar::OnPanelFullyOpened(
    /* [in] */ IPanelView* openPanel)
{
    return PanelBar::OnPanelFullyOpened(openPanel);
}

ECode CPanelBar::OnTrackingStarted(
    /* [in] */ IPanelView* panel)
{
    return PanelBar::OnTrackingStarted(panel);
}

ECode CPanelBar::OnTrackingStopped(
    /* [in] */ IPanelView* panel)
{
    return PanelBar::OnTrackingStopped(panel);
}


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
