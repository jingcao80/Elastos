#include "elastos/droid/systemui/statusbar/CSignalClusterView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {


IVIEW_METHODS_IMPL(CSignalClusterView, SignalClusterView)
IVIEWGROUP_METHODS_IMPL(CSignalClusterView, SignalClusterView)
IVIEWPARENT_METHODS_IMPL(CSignalClusterView, SignalClusterView)
IVIEWMANAGER_METHODS_IMPL(CSignalClusterView, SignalClusterView)
IDRAWABLECALLBACK_METHODS_IMPL(CSignalClusterView, SignalClusterView)
IKEYEVENTCALLBACK_METHODS_IMPL(CSignalClusterView, SignalClusterView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CSignalClusterView, SignalClusterView)
ILINEARLAYOUT_METHODS_IMPL(CSignalClusterView, SignalClusterView)


PInterface CSignalClusterView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CSignalClusterView::Probe(riid);
}

ECode CSignalClusterView::constructor(
    /* [in] */ IContext* context)
{
    return SignalClusterView::Init(context);
}

ECode CSignalClusterView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return SignalClusterView::Init(context, attrs);
}

ECode CSignalClusterView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defaultStyle)
{
    return SignalClusterView::Init(context, attrs, defaultStyle);
}

ECode CSignalClusterView::SetNetworkController(
    /* [in] */ INetworkController* nc)
{
    return SignalClusterView::SetNetworkController(nc);
}

ECode CSignalClusterView::SetWifiIndicators(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 strengthIcon,
    /* [in] */ Int32 activityIcon,
    /* [in] */ const String& contentDescription)
{
    return SignalClusterView::SetWifiIndicators(
        visible, strengthIcon, activityIcon, contentDescription);
}

ECode CSignalClusterView::SetMobileDataIndicators(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 strengthIcon,
    /* [in] */ Int32 activityIcon,
    /* [in] */ Int32 typeIcon,
    /* [in] */ const String& contentDescription,
    /* [in] */ const String& typeContentDescription)
{
    return SignalClusterView::SetMobileDataIndicators(
        visible, strengthIcon, activityIcon, typeIcon,
        contentDescription, typeContentDescription);
}

ECode CSignalClusterView::SetEthernetIndicators(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 stateIcon,
    /* [in] */ Int32 activityIcon,
    /* [in] */ const String& contentDescription)
{
    return SignalClusterView::SetEthernetIndicators(
        visible, stateIcon, activityIcon, contentDescription);
}

ECode CSignalClusterView::SetIsAirplaneMode(
    /* [in] */ Boolean is,
    /* [in] */ Int32 airplaneIcon)
{
    return SignalClusterView::SetIsAirplaneMode(is, airplaneIcon);
}

}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
