#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/R.h"
#include "elastos/droid/app/AlertDialog.h"
#include "elastos/droid/internal/app/CAlertController.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::R;
using Elastos::Droid::Internal::App::CAlertController;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::EIID_IDialogInterface;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(AlertDialog, Dialog, IAlertDialog)

AlertDialog::AlertDialog()
{
    Logger::V("AlertDialog", " >> create AlertDialog() %p", this);
}

AlertDialog::~AlertDialog()
{
    Logger::V("AlertDialog", " >> Destroy ~AlertDialog() %p", this);
}

ECode AlertDialog::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, ResolveDialogTheme(context, 0), TRUE);
}

ECode AlertDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme)
{
    return constructor(context, theme, TRUE);
}

ECode AlertDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme,
    /* [in] */ Boolean createThemeContextWrapper)
{
    FAIL_RETURN(Dialog::constructor(context, ResolveDialogTheme(context, theme), createThemeContextWrapper));

    mWindow->AlwaysReadCloseOnTouchAttr();
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return CAlertController::New(ctx, this, window,
            (IAlertController**)&mAlert);
}

ECode AlertDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Boolean cancelable,
    /* [in] */ IDialogInterfaceOnCancelListener* cancelListener)
{
    FAIL_RETURN(Dialog::constructor(context, ResolveDialogTheme(context, 0)));

    mWindow->AlwaysReadCloseOnTouchAttr();
    SetCancelable(cancelable);
    SetOnCancelListener(cancelListener);
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return CAlertController::New(context, this, window,
            (IAlertController**)&mAlert);
}

Int32 AlertDialog::ResolveDialogTheme(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resid)
{
    if (resid == IAlertDialog::THEME_TRADITIONAL) {
        return R::style::Theme_Dialog_Alert;
    } else if (resid == IAlertDialog::THEME_HOLO_DARK) {
        return R::style::Theme_Holo_Dialog_Alert;
    } else if (resid == IAlertDialog::THEME_HOLO_LIGHT) {
        return R::style::Theme_Holo_Light_Dialog_Alert;
    } else if (resid == IAlertDialog::THEME_DEVICE_DEFAULT_DARK) {
        return R::style::Theme_DeviceDefault_Dialog_Alert;
    } else if (resid == IAlertDialog::THEME_DEVICE_DEFAULT_LIGHT) {
        return R::style::Theme_DeviceDefault_Light_Dialog_Alert;
    } else if (resid >= 0x01000000) {   // start of real resource IDs.
        return resid;
    } else {
        return GetResourceId(context, R::attr::alertDialogTheme);
    }
}

ECode AlertDialog::GetButton(
    /* [in] */ Int32 whichButton,
    /* [out] */ IButton** button)
{
    return mAlert->GetButton(whichButton, button);
}

ECode AlertDialog::GetListView(
    /* [out] */ IListView** listView)
{
    return mAlert->GetListView(listView);
}

ECode AlertDialog::SetTitle(
    /* [in] */ ICharSequence* title)
{
    Dialog::SetTitle(title);
    return mAlert->SetTitle(title);
}

ECode AlertDialog::SetCustomTitle(
    /* [in] */ IView* customTitleView)
{
    return mAlert->SetCustomTitle(customTitleView);
}

ECode AlertDialog::SetMessage(
    /* [in] */ ICharSequence* message)
{
    return mAlert->SetMessage(message);
}

ECode AlertDialog::SetView(
    /* [in] */ IView* view)
{
    return mAlert->SetView(view);
}

ECode AlertDialog::SetView(
    /* [in] */ IView* view,
    /* [in] */ Int32 viewSpacingLeft,
    /* [in] */ Int32 viewSpacingTop,
    /* [in] */ Int32 viewSpacingRight,
    /* [in] */ Int32 viewSpacingBottom)
{
    return mAlert->SetView(view,
            viewSpacingLeft, viewSpacingTop,
            viewSpacingRight, viewSpacingBottom);
}

ECode AlertDialog::SetButtonPanelLayoutHint(
    /* [in] */ Int32 layoutHint)
{
    return mAlert->SetButtonPanelLayoutHint(layoutHint);
}

ECode AlertDialog::SetButton(
    /* [in] */ Int32 whichButton,
    /* [in] */ ICharSequence* text,
    /* [in] */ IMessage* msg)
{
    return mAlert->SetButton(whichButton, text, NULL, msg);
}

ECode AlertDialog::SetButton(
    /* [in] */ Int32 whichButton,
    /* [in] */ ICharSequence* text,
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    return mAlert->SetButton(whichButton, text, listener, NULL);
}

ECode AlertDialog::SetIcon(
    /* [in] */ Int32 resId)
{
    return mAlert->SetIcon(resId);
}

ECode AlertDialog::SetIcon(
    /* [in] */ IDrawable* icon)
{
    return mAlert->SetIcon(icon);
}

ECode AlertDialog::SetIconAttribute(
    /* [in] */ Int32 attrId)
{
    Int32 resourceId = GetResourceId(mContext, attrId);
    return mAlert->SetIcon(resourceId);
}

ECode AlertDialog::SetInverseBackgroundForced(
    /* [in] */ Boolean forceInverseBackground)
{
    return mAlert->SetInverseBackgroundForced(forceInverseBackground);
}


ECode AlertDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Dialog::OnCreate(savedInstanceState);
    mAlert->InstallContent();
    return NOERROR;
}

ECode AlertDialog::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean bval;
    mAlert->OnKeyDown(keyCode, event, &bval);
    if (bval) {
        *result = TRUE;
        return NOERROR;
    }

    return Dialog::OnKeyDown(keyCode, event, result);
}

ECode AlertDialog::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean bval;
    mAlert->OnKeyUp(keyCode, event, &bval);
    if (bval) {
        *result = TRUE;
        return NOERROR;
    }

    return Dialog::OnKeyUp(keyCode, event, result);
}

ECode AlertDialog::GetAlertController(
    /* [out] */ IAlertController** alert)
{
    VALIDATE_NOT_NULL(alert);
    *alert = mAlert;
    REFCOUNT_ADD(*alert);
    return NOERROR;
}

ECode AlertDialog::SetAlertController(
    /* [in] */ IAlertController* alert)
{
    mAlert = alert;
    return NOERROR;
}

void AlertDialog::OnLastStrongRef(
    /* [in] */ const void* id)
{
    CAlertController* ac = (CAlertController*)mAlert.Get();
    if (!ac->IsHoldedByListener()) return;

    // if mAlert is holded by it's listener, then should
    // let mAlert holds this dialog
    ExtendObjectLifetime(OBJECT_LIFETIME_WEAK);
    GetWeakRefs()->AttemptIncStrong(NULL);
    ac->HoldDialogInterface();
    ExtendObjectLifetime(OBJECT_LIFETIME_STRONG);
    mAlert = NULL;
}

Boolean AlertDialog::OnIncStrongAttempted(
    /* [in] */ UInt32 flags,
    /* [in] */ const void* id)
{
    return TRUE;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
