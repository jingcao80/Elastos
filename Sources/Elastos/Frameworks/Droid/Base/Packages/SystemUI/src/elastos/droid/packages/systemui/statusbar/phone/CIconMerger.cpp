#include "elastos/droid/systemui/statusbar/phone/CIconMerger.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {


IVIEW_METHODS_IMPL(CIconMerger, IconMerger)
IVIEWGROUP_METHODS_IMPL(CIconMerger, IconMerger)
IVIEWPARENT_METHODS_IMPL(CIconMerger, IconMerger)
IVIEWMANAGER_METHODS_IMPL(CIconMerger, IconMerger)
IDRAWABLECALLBACK_METHODS_IMPL(CIconMerger, IconMerger)
IKEYEVENTCALLBACK_METHODS_IMPL(CIconMerger, IconMerger)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CIconMerger, IconMerger)
ILINEARLAYOUT_METHODS_IMPL(CIconMerger, IconMerger)


PInterface CIconMerger::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CIconMerger::Probe(riid);
}

ECode CIconMerger::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return IconMerger::Init(context, attrs);
}

ECode CIconMerger::SetOverflowIndicator(
    /* [in] */ IView* v)
{
    return IconMerger::SetOverflowIndicator(v);
}


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
