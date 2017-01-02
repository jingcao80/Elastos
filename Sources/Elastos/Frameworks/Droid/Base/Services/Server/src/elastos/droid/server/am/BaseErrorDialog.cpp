//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/server/am/BaseErrorDialog.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::App::EIID_IAlertDialog;
using Elastos::Droid::Os::CHandler;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

const Int32 BaseErrorDialog::ENABLE_BUTTONS = 0;
const Int32 BaseErrorDialog::DISABLE_BUTTONS= 0;

BaseErrorDialog::MyHandler::MyHandler(
    /* [in] */ BaseErrorDialog* service)
{
    Handler::constructor();
    service->GetWeakReference((IWeakReference**)&mWeakHost);
}

ECode BaseErrorDialog::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    AutoPtr<IAlertDialog> dialog;
    mWeakHost->Resolve(EIID_IAlertDialog, (IInterface**)&dialog);
    if (dialog == NULL) return NOERROR;

    BaseErrorDialog* host = (BaseErrorDialog*)dialog.Get();
    if (what == ENABLE_BUTTONS) {
        host->HandleOnStartMessage();
    }
    else if (what == DISABLE_BUTTONS) {
        host->SetEnabled(FALSE);
    }

    return NOERROR;
}

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
    FAIL_RETURN(AlertDialog::constructor(context, R::style::Theme_Dialog_AppError));
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
    return NOERROR;
}

ECode BaseErrorDialog::OnStart()
{
    AlertDialog::OnStart();
    Boolean result;
    mHandler->SendEmptyMessage(DISABLE_BUTTONS, &result);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(ENABLE_BUTTONS, (IMessage**)&msg);
    mHandler->SendMessageDelayed(msg, 1000, &result);
    return NOERROR;
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
    Boolean result;
    AlertDialog::DispatchKeyEvent(event, &result);
    return result;
}

ECode BaseErrorDialog::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = DispatchKeyEvent(event);
    return NOERROR;
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
        IView::Probe(b)->SetEnabled(enabled);
    }

    view = NULL;
    FindViewById(R::id::button2, (IView**)&view);
    b = IButton::Probe(view);
    if (b != NULL) {
        IView::Probe(b)->SetEnabled(enabled);
    }

    view = NULL;
    FindViewById(R::id::button3, (IView**)&view);
    b = IButton::Probe(view);
    if (b != NULL) {
        IView::Probe(b)->SetEnabled(enabled);
    }
}

ECode BaseErrorDialog::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AlertDialog::OnKeyDown(keyCode, event, result);

    return NOERROR;
}

ECode BaseErrorDialog::OnKeyLongPress(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AlertDialog::OnKeyLongPress(keyCode, event, result);

    return NOERROR;
}

ECode BaseErrorDialog::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AlertDialog::OnKeyUp(keyCode, event, result);

    return NOERROR;
}

ECode BaseErrorDialog::OnKeyMultiple(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 repeatCount,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AlertDialog::OnKeyMultiple(keyCode, repeatCount, event, result);

    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
