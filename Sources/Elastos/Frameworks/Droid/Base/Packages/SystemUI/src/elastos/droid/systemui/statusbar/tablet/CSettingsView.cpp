#include "elastos/droid/systemui/statusbar/tablet/CSettingsView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


IVIEW_METHODS_IMPL(CSettingsView, SettingsView)
IVIEWGROUP_METHODS_IMPL(CSettingsView, SettingsView)
IVIEWPARENT_METHODS_IMPL(CSettingsView, SettingsView)
IVIEWMANAGER_METHODS_IMPL(CSettingsView, SettingsView)
IDRAWABLECALLBACK_METHODS_IMPL(CSettingsView, SettingsView)
IKEYEVENTCALLBACK_METHODS_IMPL(CSettingsView, SettingsView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CSettingsView, SettingsView)
ILINEARLAYOUT_METHODS_IMPL(CSettingsView, SettingsView)


PInterface CSettingsView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CSettingsView::Probe(riid);
}

ECode CSettingsView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return SettingsView::Init(context, attrs);
}

ECode CSettingsView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return SettingsView::Init(context, attrs, defStyle);
}

ECode CSettingsView::OnClick(
    /* [in] */ IView* v)
{
    return SettingsView::OnClick(v);
}

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
