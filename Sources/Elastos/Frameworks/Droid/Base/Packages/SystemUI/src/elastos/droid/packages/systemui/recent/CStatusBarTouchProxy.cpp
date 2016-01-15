#include "elastos/droid/systemui/recent/CStatusBarTouchProxy.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

IVIEW_METHODS_IMPL(CStatusBarTouchProxy, StatusBarTouchProxy)
IVIEWGROUP_METHODS_IMPL(CStatusBarTouchProxy, StatusBarTouchProxy)
IVIEWPARENT_METHODS_IMPL(CStatusBarTouchProxy, StatusBarTouchProxy)
IVIEWMANAGER_METHODS_IMPL(CStatusBarTouchProxy, StatusBarTouchProxy)
IDRAWABLECALLBACK_METHODS_IMPL(CStatusBarTouchProxy, StatusBarTouchProxy)
IKEYEVENTCALLBACK_METHODS_IMPL(CStatusBarTouchProxy, StatusBarTouchProxy)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CStatusBarTouchProxy, StatusBarTouchProxy)
IFRAMELAYOUT_METHODS_IMPL(CStatusBarTouchProxy, StatusBarTouchProxy)

ECode CStatusBarTouchProxy::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return StatusBarTouchProxy::Init(context, attrs);
}

PInterface CStatusBarTouchProxy::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CStatusBarTouchProxy::Probe(riid);
}

ECode CStatusBarTouchProxy::SetStatusBar(
    /* [in] */ IView* statusBar)
{
    return StatusBarTouchProxy::SetStatusBar(statusBar);
}

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
