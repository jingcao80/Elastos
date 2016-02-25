#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/systemui/statusbar/phone/CPanelView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

IVIEW_METHODS_IMPL(CPanelView, PanelView)
IVIEWGROUP_METHODS_IMPL(CPanelView, PanelView)
IVIEWPARENT_METHODS_IMPL(CPanelView, PanelView)
IVIEWMANAGER_METHODS_IMPL(CPanelView, PanelView)
IDRAWABLECALLBACK_METHODS_IMPL(CPanelView, PanelView)
IKEYEVENTCALLBACK_METHODS_IMPL(CPanelView, PanelView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CPanelView, PanelView)
IFRAMELAYOUT_METHODS_IMPL(CPanelView, PanelView)


PInterface CPanelView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CPanelView::Probe(riid);
}

ECode CPanelView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return PanelView::Init(context, attrs);
}

ECode CPanelView::SetRubberbandingEnabled(
    /* [in] */ Boolean enabled)
{
    return PanelView::SetRubberbandingEnabled(enabled);
}

ECode CPanelView::Fling(
    /* [in] */ Float vel,
    /* [in] */ Boolean always)
{
    return PanelView::Fling(vel, always);
}

ECode CPanelView::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = PanelView::GetName();
    return NOERROR;
}

ECode CPanelView::SetExpandedHeight(
    /* [in] */ Float height)
{
    return PanelView::SetExpandedHeight(height);
}

ECode CPanelView::SetExpandedHeightInternal(
    /* [in] */ Float h)
{
    return PanelView::SetExpandedHeightInternal(h);
}

ECode CPanelView::GetHandle(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> v = PanelView::GetHandle();
    *view = v;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CPanelView::SetExpandedFraction(
    /* [in] */ Float frac)
{
    return PanelView::SetExpandedFraction(frac);
}

ECode CPanelView::GetExpandedHeight(
    /* [out] */ Float* height)
{
    VALIDATE_NOT_NULL(height);
    *height = PanelView::GetExpandedHeight();
    return NOERROR;
}

ECode CPanelView::GetExpandedFraction(
    /* [out] */ Float* fraction)
{
    VALIDATE_NOT_NULL(fraction);
    *fraction = PanelView::GetExpandedFraction();
    return NOERROR;
}

ECode CPanelView::IsFullyExpanded(
    /* [out] */ Boolean* isFullyExpanded)
{
    VALIDATE_NOT_NULL(isFullyExpanded);
    *isFullyExpanded = PanelView::IsFullyExpanded();
    return NOERROR;
}

ECode CPanelView::IsFullyCollapsed(
    /* [out] */ Boolean* isFullyCollapsed)
{
    VALIDATE_NOT_NULL(isFullyCollapsed);
    *isFullyCollapsed = PanelView::IsFullyCollapsed();
    return NOERROR;
}

ECode CPanelView::IsCollapsing(
    /* [out] */ Boolean* isCollapsing)
{
    VALIDATE_NOT_NULL(isCollapsing);
    *isCollapsing = PanelView::IsCollapsing();
    return NOERROR;
}

ECode CPanelView::SetBar(
    /* [in] */ IPanelBar* panelBar)
{
    return PanelView::SetBar(panelBar);
}

ECode CPanelView::Collapse()
{
    return PanelView::Collapse();
}

ECode CPanelView::Expand()
{
    return PanelView::Expand();
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
