
#include "elastos/droid/systemui/statusbar/CStatusBarIconView.h"

using Elastos::Droid::View::EIID_View;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {


IVIEW_METHODS_IMPL(CStatusBarIconView, StatusBarIconView)
IDRAWABLECALLBACK_METHODS_IMPL(CStatusBarIconView, StatusBarIconView)
IKEYEVENTCALLBACK_METHODS_IMPL(CStatusBarIconView, StatusBarIconView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CStatusBarIconView, StatusBarIconView)
IIMAGEVIEW_METHODS_IMPL(CStatusBarIconView, StatusBarIconView)

PInterface CStatusBarIconView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    return _CStatusBarIconView::Probe(riid);
}

ECode CStatusBarIconView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& slot,
    /* [in] */ INotification* notification)
{
    return StatusBarIconView::Init(context, slot, notification);
}

ECode CStatusBarIconView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return StatusBarIconView::Init(context, attrs);
}

ECode CStatusBarIconView::Set(
    /* [in] */ IStatusBarIcon* icon,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    *succeeded = StatusBarIconView::Set(icon);
    return NOERROR;
}

ECode CStatusBarIconView::GetStatusBarIcon(
    /* [out] */ IStatusBarIcon** icon)
{
    VALIDATE_NOT_NULL(icon);
    *icon = StatusBarIconView::GetStatusBarIcon();
    REFCOUNT_ADD(*icon);
    return NOERROR;
}

ECode CStatusBarIconView::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = StatusBarIconView::ToString();
    return NOERROR;
}

ECode CStatusBarIconView::SetAlpha(
        /* [in] */ Int32 alpha)
{
    return StatusBarIconView::SetAlpha(alpha);
}

}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos



