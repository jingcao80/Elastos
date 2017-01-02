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

#include <elastos/droid/R.h>
#include "elastos/droid/server/PermissionDialog.h"
#include "elastos/droid/server/CAppOpsService.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Server {

const String PermissionDialog::TAG("PermissionDialog");
const Int32 PermissionDialog::ACTION_ALLOWED = 0x2;
const Int32 PermissionDialog::ACTION_IGNORED = 0x4;
const Int32 PermissionDialog::ACTION_IGNORED_TIMEOUT = 0x8;

const Int64 PermissionDialog::DISMISS_TIMEOUT = 1000 * 15 * 1;

//------------------------------------------------------------------
//               PermissionDialog::MyHandler
//------------------------------------------------------------------
PermissionDialog::MyHandler::MyHandler(
    /* [in] */ PermissionDialog* host)
    : mHost(host)
{}

ECode PermissionDialog::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 mode;
    Boolean remember = FALSE;
    ICheckable::Probe(mHost->mChoice)->IsChecked(&remember);
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case ACTION_ALLOWED:
            mode = IAppOpsManager::MODE_ALLOWED;
            break;
        case ACTION_IGNORED:
            mode = IAppOpsManager::MODE_IGNORED;
            break;
        default:
            mode = IAppOpsManager::MODE_IGNORED;
            remember = FALSE;
    }

    CAppOpsService* aos = (CAppOpsService*)mHost->mService.Get();
    aos->NotifyOperation(mHost->mCode, mHost->mUid, mHost->mPackageName, mode, remember);
    mHost->Dismiss();
    return NOERROR;
}

//------------------------------------------------------------------
//               PermissionDialog
//------------------------------------------------------------------

CAR_INTERFACE_IMPL(PermissionDialog, BasePermissionDialog, IPermissionDialog)

PermissionDialog::PermissionDialog()
    : mCode(0)
    , mUid(0)
{}

ECode PermissionDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIAppOpsService* service,
    /* [in] */ Int32 code,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    BasePermissionDialog::constructor(context);

    mService = service;
    mCode = code;
    mUid = uid;
    mPackageName = packageName;

    mContext = context;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    res->GetTextArray(R::array::app_ops_labels, (ArrayOf<ICharSequence*>**)&mOpLabels);

    SetCancelable(FALSE);

    AutoPtr<MyHandler> mh = new MyHandler(this);
    mh->constructor();
    mHandler = (IHandler*)mh.Get();

    String str;
    res->GetString(R::string::allow, &str);
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(ACTION_ALLOWED, (IMessage**)&message);
    SetButton(IDialogInterface::BUTTON_POSITIVE, CoreUtils::Convert(str), message);

    res->GetString(R::string::deny, &str);
    message = NULL;
    mHandler->ObtainMessage(ACTION_IGNORED, (IMessage**)&message);
    SetButton(IDialogInterface::BUTTON_NEGATIVE, CoreUtils::Convert(str), message);

    res->GetString(R::string::privacy_guard_dialog_title, &str);
    SetTitle(CoreUtils::Convert(str));

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    AutoPtr<IWindowManagerLayoutParams> attrs;
    window->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    StringBuilder sb("Permission info: ");
    sb += GetAppName(mPackageName);
    attrs->SetTitle(CoreUtils::Convert(sb.ToString()));
    Int32 pFlags;
    attrs->GetPrivateFlags(&pFlags);
    pFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_SYSTEM_ERROR
            | IWindowManagerLayoutParams::PRIVATE_FLAG_SHOW_FOR_ALL_USERS;
    attrs->SetPrivateFlags(pFlags);
    window->SetAttributes(attrs);

    AutoPtr<ILayoutInflater> layoutInflater;
    GetLayoutInflater((ILayoutInflater**)&layoutInflater);
    layoutInflater->Inflate(R::layout::permission_confirmation_dialog, NULL, (IView**)&mView);
    AutoPtr<IView> obj;
    mView->FindViewById(R::id::permission_text, (IView**)&obj);
    AutoPtr<ITextView> tv = ITextView::Probe(obj);
    obj = NULL;
    mView->FindViewById(R::id::permission_remember_choice_checkbox, (IView**)&obj);
    mChoice = ICheckBox::Probe(obj);
    String name = GetAppName(mPackageName);
    if (name.IsNull()) {
        name = mPackageName;
    }

    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(2);
    array->Set(0, CoreUtils::Convert(name).Get());
    array->Set(1, (*mOpLabels)[mCode]);

    mContext->GetString(R::string::privacy_guard_dialog_summary, array, &str);
    tv->SetText(CoreUtils::Convert(str));
    SetView(mView);

    // After the timeout, pretend the user clicked the quit button
    message = NULL;
    mHandler->ObtainMessage(ACTION_IGNORED_TIMEOUT, (IMessage**)&message);
    Boolean flag = FALSE;
    return mHandler->SendMessageDelayed(message, DISMISS_TIMEOUT, &flag);
}

String PermissionDialog::GetAppName(
    /* [in] */ const String& packageName)
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    // try {
    AutoPtr<IApplicationInfo> appInfo;
    ECode ec = pm->GetApplicationInfo(packageName,
      IPackageManager::GET_DISABLED_COMPONENTS | IPackageManager::GET_UNINSTALLED_PACKAGES,
      (IApplicationInfo**)&appInfo);
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        return String(NULL);
    }
    // } catch (final NameNotFoundException e) {
        // return null;
    // }
    if (appInfo != NULL) {
        AutoPtr<ICharSequence> cs;
        pm->GetApplicationLabel(appInfo, (ICharSequence**)&cs);
        String str;
        cs->ToString(&str);
        return str;
    }
    return String(NULL);
}

} // Server
} // Droid
} // Elastos