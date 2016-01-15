#include "CTransportControlView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;


namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

IVIEW_METHODS_IMPL(CTransportControlView, TransportControlView)

IFRAMELAYOUT_METHODS_IMPL(CTransportControlView, TransportControlView)

IVIEWGROUP_METHODS_IMPL(CTransportControlView, TransportControlView)

IVIEWPARENT_METHODS_IMPL(CTransportControlView, TransportControlView)

IVIEWMANAGER_METHODS_IMPL(CTransportControlView, TransportControlView)

IDRAWABLECALLBACK_METHODS_IMPL(CTransportControlView, TransportControlView)

IKEYEVENTCALLBACK_METHODS_IMPL(CTransportControlView, TransportControlView)

IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CTransportControlView, TransportControlView)

PInterface CTransportControlView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CTransportControlView::Probe(riid);
}

ECode CTransportControlView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TransportControlView::Init(context, attrs);
}

ECode CTransportControlView::OnClick(
        /* [in] */ IView* v)
{
    return TransportControlView::OnClick(v);
}

ECode CTransportControlView::SetCallback(
        /* [in] */ ILockScreenWidgetCallback* cb)
{
    return TransportControlView::SetCallback(cb);
}

ECode CTransportControlView::ProvidesClock(
        /* [out] */ Boolean* clock)
{
    *clock = TransportControlView::ProvidesClock();
    return NOERROR;
}

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos
