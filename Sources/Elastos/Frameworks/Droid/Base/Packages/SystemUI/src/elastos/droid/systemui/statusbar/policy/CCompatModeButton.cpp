#include "elastos/droid/systemui/statusbar/policy/CCompatModeButton.h"

using Elastos::Droid::View::EIID_View;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


IVIEW_METHODS_IMPL(CCompatModeButton, CompatModeButton)
IDRAWABLECALLBACK_METHODS_IMPL(CCompatModeButton, CompatModeButton)
IKEYEVENTCALLBACK_METHODS_IMPL(CCompatModeButton, CompatModeButton)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CCompatModeButton, CompatModeButton)
IIMAGEVIEW_METHODS_IMPL(CCompatModeButton, CompatModeButton)

PInterface CCompatModeButton::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    return _CCompatModeButton::Probe(riid);
}

ECode CCompatModeButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return CompatModeButton::Init(context, attrs);
}

ECode CCompatModeButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return CompatModeButton::Init(context, attrs, defStyle);
}

ECode CCompatModeButton::Refresh()
{
    return CompatModeButton::Refresh();
}

ECode CCompatModeButton::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return CompatModeButton::SetAlpha(alpha);
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
