#include "elastos/droid/systemui/statusbar/tablet/CDropZone.h"

using Elastos::Droid::View::EIID_View;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


IVIEW_METHODS_IMPL(CDropZone, DropZone)
IDRAWABLECALLBACK_METHODS_IMPL(CDropZone, DropZone)
IKEYEVENTCALLBACK_METHODS_IMPL(CDropZone, DropZone)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CDropZone, DropZone)


PInterface CDropZone::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    return _CDropZone::Probe(riid);
}

ECode CDropZone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return DropZone::Init(context, attrs);
}

ECode CDropZone::SetPocket(
    /* [in] */ IShirtPocket* p)
{
    return DropZone::SetPocket(p);
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
