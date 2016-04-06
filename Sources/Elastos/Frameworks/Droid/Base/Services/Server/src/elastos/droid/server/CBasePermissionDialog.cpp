
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/server/CBasePermissionDialog.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Server {

//============================================================================
// CBasePermissionDialog::MyHandler::
//============================================================================
CBasePermissionDialog::MyHandler::MyHandler(
    /* [in] */ CBasePermissionDialog* host)
    : mHost(host)
{}

ECode CBasePermissionDialog::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    if (what == 0) {
        mHost->mConsuming = FALSE;
        mHost->SetEnabled(TRUE);
    }
    return NOERROR;
}

//============================================================================
// CBasePermissionDialog::
//============================================================================
CAR_OBJECT_IMPL(CBasePermissionDialog)

CAR_INTERFACE_IMPL(CBasePermissionDialog, AlertDialog, IBasePermissionDialog)

CBasePermissionDialog::CBasePermissionDialog()
    : mConsuming(TRUE)
{
    mHandler = new MyHandler(this);
}

ECode CBasePermissionDialog::constructor(
    /* [in] */ IContext* context)
{
    AlertDialog::constructor(context, R::style::Theme_Dialog_AppError);

    AutoPtr<IWindow> wd;
    GetWindow((IWindow**)&wd);
    wd->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    wd->SetFlags(IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM,
            IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM);
    AutoPtr<IWindowManagerLayoutParams> attrs;
    wd->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    attrs->SetTitle(CoreUtils::Convert("Permission Dialog"));
    wd->SetAttributes(attrs);
    SetIconAttribute(R::attr::alertDialogIcon);
    return NOERROR;
}

ECode CBasePermissionDialog::OnStart()
{
    AlertDialog::OnStart();
    SetEnabled(FALSE);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(0, (IMessage**)&msg);
    Boolean b = FALSE;
    mHandler->SendMessage(msg, &b);
    return NOERROR;
}

ECode CBasePermissionDialog::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mConsuming) {
        // Slog.i(TAG, "Consuming: " + event);
        *result = TRUE;
        return NOERROR;
    }
    // Slog.i(TAG, "Dispatching: " + event);
    return AlertDialog::DispatchKeyEvent(event, result);
}

ECode CBasePermissionDialog::SetEnabled(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IView> v;
    FindViewById(R::id::button1, (IView**)&v);
    AutoPtr<IButton> b = IButton::Probe(v);
    if (b != NULL) {
        IView::Probe(b)->SetEnabled(enabled);
    }
    v = NULL;
    FindViewById(R::id::button2, (IView**)&v);
    b = IButton::Probe(v);
    if (b != NULL) {
        IView::Probe(b)->SetEnabled(enabled);
    }
    v = NULL;
    FindViewById(R::id::button3, (IView**)&v);
    b = IButton::Probe(v);
    if (b != NULL) {
        IView::Probe(b)->SetEnabled(enabled);
    }
    return NOERROR;
}

} //namespace Server
} //namespace Droid
} //namespace Elastos
