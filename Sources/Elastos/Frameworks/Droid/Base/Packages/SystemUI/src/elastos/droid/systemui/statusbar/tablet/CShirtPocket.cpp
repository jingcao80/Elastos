#include "elastos/droid/systemui/statusbar/tablet/CShirtPocket.h"

using Elastos::Droid::View::EIID_View;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


IVIEW_METHODS_IMPL(CShirtPocket, ShirtPocket)
IDRAWABLECALLBACK_METHODS_IMPL(CShirtPocket, ShirtPocket)
IKEYEVENTCALLBACK_METHODS_IMPL(CShirtPocket, ShirtPocket)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CShirtPocket, ShirtPocket)
IIMAGEVIEW_METHODS_IMPL(CShirtPocket, ShirtPocket)

PInterface CShirtPocket::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    return _CShirtPocket::Probe(riid);
}

ECode CShirtPocket::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ShirtPocket::Init(context, attrs);
}

ECode CShirtPocket::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return ShirtPocket::Init(context, attrs, defStyle);
}

ECode CShirtPocket::Holding(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ShirtPocket::Holding();
    return NOERROR;
}

ECode CShirtPocket::Stash(
    /* [in] */ IClipData* clipping)
{
    return ShirtPocket::Stash(clipping);
}

ECode CShirtPocket::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return CShirtPocket::SetAlpha(alpha);
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
