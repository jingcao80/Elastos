#include "elastos/droid/systemui/statusbar/CLatestItemView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {


IVIEW_METHODS_IMPL(CLatestItemView, LatestItemView)
IVIEWGROUP_METHODS_IMPL(CLatestItemView, LatestItemView)
IVIEWPARENT_METHODS_IMPL(CLatestItemView, LatestItemView)
IVIEWMANAGER_METHODS_IMPL(CLatestItemView, LatestItemView)
IDRAWABLECALLBACK_METHODS_IMPL(CLatestItemView, LatestItemView)
IKEYEVENTCALLBACK_METHODS_IMPL(CLatestItemView, LatestItemView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CLatestItemView, LatestItemView)
IFRAMELAYOUT_METHODS_IMPL(CLatestItemView, LatestItemView)

PInterface CLatestItemView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CLatestItemView::Probe(riid);
}

ECode CLatestItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return LatestItemView::Init(context, attrs);
}


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
