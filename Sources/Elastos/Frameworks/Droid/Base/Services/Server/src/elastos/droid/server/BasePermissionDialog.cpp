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
#include "elastos/droid/server/BasePermissionDialog.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Server {

//============================================================================
// BasePermissionDialog::MyHandler::
//============================================================================
BasePermissionDialog::MyHandler::MyHandler(
    /* [in] */ BasePermissionDialog* host)
    : mHost(host)
{}

ECode BasePermissionDialog::MyHandler::HandleMessage(
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
// BasePermissionDialog::
//============================================================================
CAR_INTERFACE_IMPL(BasePermissionDialog, AlertDialog, IBasePermissionDialog)

BasePermissionDialog::BasePermissionDialog()
    : mConsuming(TRUE)
{
}

ECode BasePermissionDialog::constructor(
    /* [in] */ IContext* context)
{
    AlertDialog::constructor(context, R::style::Theme_Dialog_AppError);

    AutoPtr<MyHandler> mh = new MyHandler(this);
    mh->constructor();
    mHandler = mh.Get();

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

ECode BasePermissionDialog::OnStart()
{
    AlertDialog::OnStart();
    SetEnabled(FALSE);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(0, (IMessage**)&msg);
    Boolean b = FALSE;
    mHandler->SendMessage(msg, &b);
    return NOERROR;
}

ECode BasePermissionDialog::DispatchKeyEvent(
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

ECode BasePermissionDialog::SetEnabled(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IView> v;
    FindViewById(R::id::button1, (IView**)&v);
    if (v != NULL) {
        v->SetEnabled(enabled);
    }

    v = NULL;
    FindViewById(R::id::button2, (IView**)&v);
    if (v != NULL) {
        v->SetEnabled(enabled);
    }

    v = NULL;
    FindViewById(R::id::button3, (IView**)&v);
    if (v != NULL) {
        v->SetEnabled(enabled);
    }
    return NOERROR;
}

} //namespace Server
} //namespace Droid
} //namespace Elastos
