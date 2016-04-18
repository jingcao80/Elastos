#include <elastos/droid/R.h>
#include "elastos/droid/server/PermissionDialog.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::StringUtils;

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

    mHost->mService->NotifyOperation(mHost->mCode, mHost->mUid, mHost->mPackageName, mode, remember);
    IDialogInterface::Probe(this)->Dismiss();
    return NOERROR;
}

//------------------------------------------------------------------
//               PermissionDialog
//------------------------------------------------------------------
PermissionDialog::PermissionDialog(
    /* [in] */ IContext* context,
    /* [in] */ CAppOpsService* service,
    /* [in] */ Int32 code,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
    : mContext(context)
    , mService(service)
    , mCode(code)
    , mUid(uid)
    , mPackageName(packageName)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    res->GetTextArray(R::array::app_ops_labels, (ArrayOf<ICharSequence*>**)&mOpLabels);

    IDialog::Probe(this)->SetCancelable(FALSE);

    mHandler = new MyHandler(this);

    String str;
    res->GetString(R::string::allow, &str);
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(ACTION_ALLOWED, (IMessage**)&message);
    IAlertDialog::Probe(this)->SetButton(IDialogInterface::BUTTON_POSITIVE,
              StringUtils::ParseCharSequence(str), message);

    res->GetString(R::string::deny, &str);
    message = NULL;
    mHandler->ObtainMessage(ACTION_IGNORED, (IMessage**)&message);
    IAlertDialog::Probe(this)->SetButton(IDialogInterface::BUTTON_NEGATIVE,
                StringUtils::ParseCharSequence(str), message);
    res->GetString(R::string::privacy_guard_dialog_title, &str);
    IDialog::Probe(this)->SetTitle(StringUtils::ParseCharSequence(str));
    AutoPtr<IWindow> window;
    IDialog::Probe(this)->GetWindow((IWindow**)&window);
    AutoPtr<IWindowManagerLayoutParams> attrs;
    window->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    attrs->SetTitle(StringUtils::ParseCharSequence(String("Permission info: ") + GetAppName(mPackageName)));
    Int32 pFlags;
    attrs->GetPrivateFlags(&pFlags);
    pFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_SYSTEM_ERROR
            | IWindowManagerLayoutParams::PRIVATE_FLAG_SHOW_FOR_ALL_USERS;
    attrs->SetPrivateFlags(pFlags);
    window->SetAttributes(attrs);
    AutoPtr<ILayoutInflater> layoutInflater;
    IDialog::Probe(this)->GetLayoutInflater((ILayoutInflater**)&layoutInflater);
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
    array->Set(0, TO_IINTERFACE(StringUtils::ParseCharSequence(name)));
    array->Set(1, TO_IINTERFACE((*mOpLabels)[mCode]));

    mContext->GetString(R::string::privacy_guard_dialog_summary,
            array, &str);
    tv->SetText(StringUtils::ParseCharSequence(str));
    IAlertDialog::Probe(this)->SetView(mView);

    // After the timeout, pretend the user clicked the quit button
    message = NULL;
    mHandler->ObtainMessage(ACTION_IGNORED_TIMEOUT, (IMessage**)&message);
    Boolean flag = FALSE;
    mHandler->SendMessageDelayed(message, DISMISS_TIMEOUT, &flag);
}

String PermissionDialog::GetAppName(
    /* [in] */ const String& packageName)
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    // try {
    AutoPtr<IApplicationInfo> appInfo;
    ECode ec = pm->GetApplicationInfo(packageName,
                  IPackageManager::GET_DISABLED_COMPONENTS
                  | IPackageManager::GET_UNINSTALLED_PACKAGES, (IApplicationInfo**)&appInfo);
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