#include "elastos/droid/systemui/statusbar/tablet/CInputMethodButton.h"

using Elastos::Droid::View::EIID_View;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


IVIEW_METHODS_IMPL(CInputMethodButton, InputMethodButton)
IDRAWABLECALLBACK_METHODS_IMPL(CInputMethodButton, InputMethodButton)
IKEYEVENTCALLBACK_METHODS_IMPL(CInputMethodButton, InputMethodButton)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CInputMethodButton, InputMethodButton)
IIMAGEVIEW_METHODS_IMPL(CInputMethodButton, InputMethodButton)


PInterface CInputMethodButton::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    return _CInputMethodButton::Probe(riid);
}

ECode CInputMethodButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return InputMethodButton::Init(context, attrs);
}

ECode CInputMethodButton::SetIconImage(
    /* [in] */ Int32 resId)
{
    return InputMethodButton::SetIconImage(resId);
}

ECode CInputMethodButton::SetImeWindowStatus(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean showButton)
{
    return InputMethodButton::SetImeWindowStatus(token, showButton);
}

ECode CInputMethodButton::SetHardKeyboardStatus(
    /* [in] */ Boolean available)
{
    return InputMethodButton::SetHardKeyboardStatus(available);
}

ECode CInputMethodButton::SetScreenLocked(
    /* [in] */ Boolean available)
{
    return InputMethodButton::SetScreenLocked(available);
}

ECode CInputMethodButton::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return InputMethodButton::SetAlpha(alpha);
}

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
