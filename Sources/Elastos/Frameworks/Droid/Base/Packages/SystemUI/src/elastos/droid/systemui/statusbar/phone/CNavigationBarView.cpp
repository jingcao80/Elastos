#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/systemui/statusbar/phone/CNavigationBarView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

IVIEW_METHODS_IMPL(CNavigationBarView, NavigationBarView)
IVIEWGROUP_METHODS_IMPL(CNavigationBarView, NavigationBarView)
IVIEWPARENT_METHODS_IMPL(CNavigationBarView, NavigationBarView)
IVIEWMANAGER_METHODS_IMPL(CNavigationBarView, NavigationBarView)
IDRAWABLECALLBACK_METHODS_IMPL(CNavigationBarView, NavigationBarView)
IKEYEVENTCALLBACK_METHODS_IMPL(CNavigationBarView, NavigationBarView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CNavigationBarView, NavigationBarView)
ILINEARLAYOUT_METHODS_IMPL(CNavigationBarView, NavigationBarView)

PInterface CNavigationBarView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CNavigationBarView::Probe(riid);
}

ECode CNavigationBarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return NavigationBarView::Init(context, attrs);
}

ECode CNavigationBarView::SetDelegateView(
    /* [in] */ IView* view)
{
    return NavigationBarView::SetDelegateView(view);
}

ECode CNavigationBarView::SetBar(
   /* [in] */ IBaseStatusBar* phoneStatusBar)
{
    return NavigationBarView::SetBar(phoneStatusBar);
}

ECode CNavigationBarView::GetRecentsButton(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> temp = NavigationBarView::GetRecentsButton();
    *view = temp;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNavigationBarView::GetMenuButton(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> temp = NavigationBarView::GetMenuButton();
    *view = temp;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNavigationBarView::GetBackButton(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> temp = NavigationBarView::GetBackButton();
    *view = temp;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNavigationBarView::GetHomeButton(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> temp = NavigationBarView::GetHomeButton();
    *view = temp;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNavigationBarView::GetScreenShotButton(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> temp = NavigationBarView::GetScreenShotButton();
    *view = temp;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNavigationBarView::GetVolumeDownButton(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> temp = NavigationBarView::GetVolumeDownButton();
    *view = temp;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNavigationBarView::GetVolumeUpButton(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> temp = NavigationBarView::GetVolumeUpButton();
    *view = temp;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

// for when home is disabled, but search isn't
ECode CNavigationBarView::GetSearchLight(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> temp = NavigationBarView::GetSearchLight();
    *view = temp;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNavigationBarView::NotifyScreenOn(
    /* [in] */ Boolean screenOn)
{
    return NavigationBarView::NotifyScreenOn(screenOn);
}

ECode CNavigationBarView::SetNavigationIconHints(
    /* [in] */ Int32 hints)
{
    return NavigationBarView::SetNavigationIconHints(hints);
}

ECode CNavigationBarView::SetNavigationIconHints(
    /* [in] */ Int32 hints,
    /* [in] */ Boolean force)
{
    return NavigationBarView::SetNavigationIconHints(hints, force);
}

ECode CNavigationBarView::SetDisabledFlags(
    /* [in] */ Int32 disabledFlags)
{
    return NavigationBarView::SetDisabledFlags(disabledFlags);
}

ECode CNavigationBarView::SetDisabledFlags(
    /* [in] */ Int32 disabledFlags,
    /* [in] */ Boolean force)
{
    return NavigationBarView::SetDisabledFlags(disabledFlags, force);
}

ECode CNavigationBarView::SetSlippery(
    /* [in] */ Boolean newSlippery)
{
    return NavigationBarView::SetSlippery(newSlippery);
}

ECode CNavigationBarView::SetMenuVisibility(
    /* [in] */ Boolean show)
{
    return NavigationBarView::NotifyScreenOn(show);
}

ECode CNavigationBarView::SetMenuVisibility(
    /* [in] */ Boolean show,
    /* [in] */ Boolean force)
{
    return NavigationBarView::SetMenuVisibility(show, force);
}

ECode CNavigationBarView::SetLowProfile(
    /* [in] */ Boolean lightsOut)
{
    return NavigationBarView::SetLowProfile(lightsOut);
}

ECode CNavigationBarView::SetLowProfile(
    /* [in] */ Boolean lightsOut,
    /* [in] */ Boolean animate,
    /* [in] */ Boolean force)
{
    return NavigationBarView::SetLowProfile(lightsOut, animate, force);
}

ECode CNavigationBarView::SetHidden(
    /* [in] */ Boolean hide)
{
    return NavigationBarView::SetHidden(hide);
}

ECode CNavigationBarView::Reorient()
{
    return NavigationBarView::Reorient();
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
