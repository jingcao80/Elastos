#include "elastos/droid/systemui/statusbar/tablet/CInputMethodsPanel.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


IVIEW_METHODS_IMPL(CInputMethodsPanel, InputMethodsPanel)
IVIEWGROUP_METHODS_IMPL(CInputMethodsPanel, InputMethodsPanel)
IVIEWPARENT_METHODS_IMPL(CInputMethodsPanel, InputMethodsPanel)
IVIEWMANAGER_METHODS_IMPL(CInputMethodsPanel, InputMethodsPanel)
IDRAWABLECALLBACK_METHODS_IMPL(CInputMethodsPanel, InputMethodsPanel)
IKEYEVENTCALLBACK_METHODS_IMPL(CInputMethodsPanel, InputMethodsPanel)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CInputMethodsPanel, InputMethodsPanel)
ILINEARLAYOUT_METHODS_IMPL(CInputMethodsPanel, InputMethodsPanel)


PInterface CInputMethodsPanel::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CInputMethodsPanel::Probe(riid);
}

ECode CInputMethodsPanel::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return InputMethodsPanel::Init(context, attrs);
}

ECode CInputMethodsPanel::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return InputMethodsPanel::Init(context, attrs, defStyle);
}

ECode CInputMethodsPanel::SetHardKeyboardEnabledChangeListener(
    /* [in] */ IOnHardKeyboardEnabledChangeListener* listener)
{
    return InputMethodsPanel::SetHardKeyboardEnabledChangeListener(listener);
}

ECode CInputMethodsPanel::OpenPanel()
{
    return InputMethodsPanel::OpenPanel();
}

ECode CInputMethodsPanel::ClosePanel(
    /* [in] */ Boolean closeKeyboard)
{
    return InputMethodsPanel::ClosePanel(closeKeyboard);
}

ECode CInputMethodsPanel::SetImeToken(
    /* [in] */ IBinder* token)
{
    return InputMethodsPanel::SetImeToken(token);
}

ECode CInputMethodsPanel::SetImeSwitchButton(
    /* [in] */ IInputMethodButton* imb)
{
    return InputMethodsPanel::SetImeSwitchButton(imb);
}

ECode CInputMethodsPanel::SetHardKeyboardStatus(
    /* [in] */ Boolean available,
    /* [in] */ Boolean enabled)
{
    return InputMethodsPanel::SetHardKeyboardStatus(available, enabled);
}

ECode CInputMethodsPanel::OnClick(
    /* [in] */ IView* v)
{
    return InputMethodsPanel::OnClick(v);
}

ECode CInputMethodsPanel::IsInContentArea(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = InputMethodsPanel::IsInContentArea(x, y);
    return NOERROR;
}

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
