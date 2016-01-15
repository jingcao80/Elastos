
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/am/BaseErrorDialog.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/R.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Droid::R;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::App::EIID_IDialog;
using Elastos::Droid::App::EIID_IAlertDialog;
using Elastos::Droid::View::EIID_IWindowCallback;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::EIID_IViewOnCreateContextMenuListener;
using Elastos::Droid::Content::EIID_IDialogInterface;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {


ECode BaseErrorDialog::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    if (what == 0) {
        mHost->HandleOnStartMessage();
    }

    return NOERROR;
}

IALERTDIALOG_METHODS_IMPL(BaseErrorDialog, AlertDialog);

IDIALOG_METHODS_IMPL(BaseErrorDialog, AlertDialog);

IWINDOWCALLBACK_METHODS_IMPL(BaseErrorDialog, AlertDialog);

BaseErrorDialog::BaseErrorDialog()
    : mConsuming(TRUE)
{}

BaseErrorDialog::BaseErrorDialog(
    /* [in] */ IContext* context)
    : mConsuming(TRUE)
{
    BaseErrorDialog::Init(context);
}

ECode BaseErrorDialog::Init(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(AlertDialog::Init(context, R::style::Theme_Dialog_AppError));
    mHandler = new MyHandler(this);

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    window->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    window->SetFlags(IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM,
            IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM);
    AutoPtr<IWindowManagerLayoutParams> attrs;
    window->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    AutoPtr<ICharSequence> cs;
    CString::New(String("Error Dialog") ,(ICharSequence**)&cs);
    attrs->SetTitle(cs);
    window->SetAttributes(attrs);
    SetIconAttribute(R::attr::alertDialogIcon);
    return NOERROR;
}

PInterface BaseErrorDialog::Probe(
    /* [in] */ REIID riid)
{
    if (EIID_IInterface == riid) {
        return (PInterface)(IAlertDialog*)this;
    }
    else if (riid == EIID_IAlertDialog) {
        return (PInterface)(IAlertDialog*)this;
    }
    else if (riid == EIID_IDialog) {
        return (PInterface)(IDialog*)this;
    }
    else if (riid == EIID_IDialogInterface) {
        return (PInterface)(IDialogInterface*)this;
    }
    else if (riid == EIID_IWindowCallback) {
        return (PInterface)(IWindowCallback*)this;
    }
    if (riid == EIID_IKeyEventCallback) {
        return (PInterface)(IKeyEventCallback*)this;
    }
    else if (riid == EIID_IViewOnCreateContextMenuListener) {
        return (PInterface)(IViewOnCreateContextMenuListener*)this;
    }
    else if (riid == EIID_IWeakReferenceSource) {
        return (PInterface)(IWeakReferenceSource*)this;
    }

    return NULL;
}

UInt32 BaseErrorDialog::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 BaseErrorDialog::Release()
{
    return ElRefBase::Release();
}

ECode BaseErrorDialog::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

void BaseErrorDialog::OnStart()
{
    AlertDialog::OnStart();
    SetEnabled(FALSE);

    Boolean result;
    mHandler->SendEmptyMessageDelayed(0, 1000, &result);
}

void BaseErrorDialog::HandleOnStartMessage()
{
    mConsuming = FALSE;
    SetEnabled(TRUE);
}

Boolean BaseErrorDialog::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    if (mConsuming) {
        //Slog.i(TAG, "Consuming: " + event);
        return TRUE;
    }
    //Slog.i(TAG, "Dispatching: " + event);
    return AlertDialog::DispatchKeyEvent(event);
}

ECode BaseErrorDialog::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    return AlertDialog::OnCreateContextMenu(menu, v, menuInfo);
}

void BaseErrorDialog::SetEnabled(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IView> view;
    FindViewById(R::id::button1, (IView**)&view);
    AutoPtr<IButton> b = IButton::Probe(view);
    if (b != NULL) {
        b->SetEnabled(enabled);
    }

    view = NULL;
    FindViewById(R::id::button2, (IView**)&view);
    b = IButton::Probe(view);
    if (b != NULL) {
        b->SetEnabled(enabled);
    }

    view = NULL;
    FindViewById(R::id::button3, (IView**)&view);
    b = IButton::Probe(view);
    if (b != NULL) {
        b->SetEnabled(enabled);
    }
}

ECode BaseErrorDialog::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = AlertDialog::OnKeyDown(keyCode, event);

    return NOERROR;
}

ECode BaseErrorDialog::OnKeyLongPress(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = AlertDialog::OnKeyLongPress(keyCode, event);

    return NOERROR;
}

ECode BaseErrorDialog::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = AlertDialog::OnKeyUp(keyCode, event);

    return NOERROR;
}

ECode BaseErrorDialog::OnKeyMultiple(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 repeatCount,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = AlertDialog::OnKeyMultiple(keyCode, repeatCount, event);

    return NOERROR;
}

ECode BaseErrorDialog::GetWeakReference(
    /* [out] */ IWeakReference** weakReference)
{
    VALIDATE_NOT_NULL(weakReference);
    *weakReference = new WeakReferenceImpl(THIS_PROBE(IInterface), CreateWeak(this));
    (*weakReference)->AddRef();
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
