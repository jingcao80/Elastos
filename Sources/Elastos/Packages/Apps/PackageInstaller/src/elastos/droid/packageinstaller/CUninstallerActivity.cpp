
#include "elastos/droid/packageinstaller/CUninstallerActivity.h"
#include "elastos/droid/packageinstaller/PackageUtil.h"
#include "elastos/droid/os/Process.h"
#include <Elastos.Droid.Net.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageDeleteObserver2;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageInstaller;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Os::Process;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

//=====================================================================
//          CUninstallerActivity::UninstallAlertDialogFragment
//=====================================================================
CAR_INTERFACE_IMPL(CUninstallerActivity::UninstallAlertDialogFragment, DialogFragment, IDialogInterfaceOnClickListener)

ECode CUninstallerActivity::UninstallAlertDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CUninstallerActivity* uninstallerActivity = (CUninstallerActivity*)activity.Get();

    AutoPtr<IPackageManager> pm;
    uninstallerActivity->GetPackageManager((IPackageManager**)&pm);

    AutoPtr<CUninstallerActivity::DialogInfo> dialogInfo = uninstallerActivity->mDialogInfo;
    IPackageItemInfo* itemInfo = IPackageItemInfo::Probe(dialogInfo->appInfo.Get());
    AutoPtr<ICharSequence> appLabel;
    itemInfo->LoadLabel(pm, (ICharSequence**)&appLabel);

    AutoPtr<IAlertDialogBuilder> dialogBuilder;
    CAlertDialogBuilder::New(uninstallerActivity, (IAlertDialogBuilder**)&dialogBuilder);
    AutoPtr<StringBuilder> messageBuilder = new StringBuilder();

    // If the Activity label differs from the App label, then make sure the user
    // knows the Activity belongs to the App being uninstalled.
    if (dialogInfo->activityInfo != NULL) {
        AutoPtr<ICharSequence> activityLabel;
        IPackageItemInfo::Probe(dialogInfo->activityInfo)->LoadLabel(pm, (ICharSequence**)&activityLabel);
        if (!Object::Equals(activityLabel, appLabel)) {
            AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, activityLabel);
            String strTmp;
            uninstallerActivity->GetString(R::string::uninstall_activity_text, args, &strTmp);
            messageBuilder->Append(strTmp);
            messageBuilder->Append(" ");
            messageBuilder->Append(appLabel);
            messageBuilder->Append(".\n\n");
        }
    }

    Int32 flags = 0;
    dialogInfo->appInfo->GetFlags(&flags);
    Boolean isUpdate = ((flags & IApplicationInfo::FLAG_UPDATED_SYSTEM_APP) != 0);
    if (isUpdate) {
        String strTmp;
        uninstallerActivity->GetString(R::string::uninstall_update_text, &strTmp);
        messageBuilder->Append(strTmp);
    }
    else {
        AutoPtr<IUserManagerHelper> helper;
        CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
        AutoPtr<IUserManager> userManager;
        helper->Get(uninstallerActivity, (IUserManager**)&userManager);

        Int32 count = 0;
        userManager->GetUserCount(&count);

        AutoPtr<IUserHandle> myUserHandle;
        Process::MyUserHandle((IUserHandle**)&myUserHandle);

        if (dialogInfo->allUsers && count >= 2) {
            String strTmp;
            uninstallerActivity->GetString(R::string::uninstall_application_text_all_users, &strTmp);
            messageBuilder->Append(strTmp);
        }
        else if (!(TO_IINTERFACE(dialogInfo) == TO_IINTERFACE(myUserHandle))) {
            Int32 identify = 0;
            dialogInfo->user->GetIdentifier(&identify);
            AutoPtr<IUserInfo> userInfo;
            userManager->GetUserInfo(identify, (IUserInfo**)&userInfo);
            String name;
            userInfo->GetName(&name);
            AutoPtr<ICharSequence> cs;
            CString::New(name, (ICharSequence**)&cs);
            AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, cs);
            String strTmp;
            uninstallerActivity->GetString(R::string::uninstall_application_text_user, args, &strTmp);
            messageBuilder->Append(strTmp);
        }
        else {
            String strTmp;
            uninstallerActivity->GetString(R::string::uninstall_application_text, &strTmp);
            messageBuilder->Append(strTmp);
        }
    }

    dialogBuilder->SetTitle(appLabel);
    AutoPtr<IDrawable> drawableTmp;
    IPackageItemInfo::Probe(dialogInfo->appInfo)->LoadIcon(pm, (IDrawable**)&drawableTmp);

    dialogBuilder->SetIcon(drawableTmp);
    dialogBuilder->SetPositiveButton(R::string::ok, this);
    dialogBuilder->SetNegativeButton(R::string::cancel, this);

    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(messageBuilder->ToString(), (ICharSequence**)&charSequenceTmp);
    dialogBuilder->SetMessage(charSequenceTmp);

    AutoPtr<IAlertDialog> alertDialog;
    dialogBuilder->Create((IAlertDialog**)&alertDialog);
    *result = IDialog::Probe(alertDialog);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CUninstallerActivity::UninstallAlertDialogFragment::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CUninstallerActivity* uninstallerActivity = (CUninstallerActivity*)activity.Get();
    if (which == IDialogInterface::BUTTON_POSITIVE) {
        uninstallerActivity->StartUninstallProgress();
    }
    else {
        uninstallerActivity->DispatchAborted();
    }
    return NOERROR;
}

ECode CUninstallerActivity::UninstallAlertDialogFragment::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    activity->Finish();
    return NOERROR;
}

//=====================================================================
//            CUninstallerActivity::AppNotFoundDialogFragment
//=====================================================================
ECode CUninstallerActivity::AppNotFoundDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IAlertDialogBuilder> dialogBuilder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&dialogBuilder);
    dialogBuilder->SetTitle(R::string::app_not_found_dlg_title);
    dialogBuilder->SetMessage(R::string::app_not_found_dlg_text);
    dialogBuilder->SetNeutralButton(R::string::ok, NULL);

    AutoPtr<IAlertDialog> alertDialog;
    dialogBuilder->Create((IAlertDialog**)&alertDialog);
    *result = IDialog::Probe(alertDialog);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CUninstallerActivity::AppNotFoundDialogFragment::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    DialogFragment::OnDismiss(dialog);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    ((CUninstallerActivity*)activity.Get())->DispatchAborted();
    activity->SetResult(IActivity::RESULT_FIRST_USER);
    activity->Finish();
    return NOERROR;
}

//=====================================================================
//                         CUninstallerActivity
//=====================================================================
const String CUninstallerActivity::TAG("CUninstallerActivity");

CAR_INTERFACE_IMPL(CUninstallerActivity, Activity, IUninstallerActivity)

CAR_OBJECT_IMPL(CUninstallerActivity)

CUninstallerActivity::CUninstallerActivity()
{
}

ECode CUninstallerActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);
    // Get intent information.
    // We expect an intent with URI of the form package://<packageName>#<className>
    // className is optional; if specified, it is the activity the user chose to uninstall
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IUri> packageUri;
    intent->GetData((IUri**)&packageUri);
    if (packageUri == NULL) {
        Logger::E(TAG, "No package URI in intent");
        ShowAppNotFound();
        return NOERROR;
    }

    String packageName;
    packageUri->GetEncodedSchemeSpecificPart(&packageName);
    if (packageName.IsEmpty()) {
        String strTmp;
        packageUri->ToSafeString(&strTmp);
        Logger::E(TAG, String("Invalid package name in URI: ") + strTmp);
        ShowAppNotFound();
        return NOERROR;
    }

    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    AutoPtr<IInterface> interfaceTmp;
    serviceManager->GetService(String("package"), (IInterface**)&interfaceTmp);

    IIPackageManager* pm = IIPackageManager::Probe(interfaceTmp);

    mDialogInfo = new DialogInfo();

    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(IIntent::EXTRA_USER, (IParcelable**)&parcelable);
    mDialogInfo->user = IUserHandle::Probe(parcelable);
    if (mDialogInfo->user == NULL) {
        AutoPtr<IUserHandle> userHandle;
        Process::MyUserHandle((IUserHandle**)&userHandle);
        mDialogInfo->user = IUserHandle::Probe(userHandle);
    }

    intent->GetBooleanExtra(IIntent::EXTRA_UNINSTALL_ALL_USERS, FALSE, &(mDialogInfo->allUsers));
    intent->GetIBinderExtra(IPackageInstaller::EXTRA_CALLBACK, (IBinder**)&mDialogInfo->callback);

    //try {
        Int32 identify = 0;
        mDialogInfo->user->GetIdentifier(&identify);
        pm->GetApplicationInfo(packageName, IPackageManager::GET_UNINSTALLED_PACKAGES,
            identify, (IApplicationInfo**)&(mDialogInfo->appInfo));
    //} catch (RemoteException e) {
        //Log.e(TAG, "Unable to get packageName. Package manager is dead?");
    //}

    if (mDialogInfo->appInfo == NULL) {
        Logger::E(TAG, String("Invalid packageName: ") + packageName);
        ShowAppNotFound();
        return NOERROR;
    }

    // The class name may have been specified (e.g. when deleting an app from all apps)
    String className;
    packageUri->GetFragment(&className);
    if (!className.IsEmpty()) {
        //try {
            AutoPtr<IComponentName> componentName;
            CComponentName::New((IComponentName**)&componentName);
            Int32 identify = 0;
            mDialogInfo->user->GetIdentifier(&identify);
            pm->GetActivityInfo(componentName, 0, identify, (IActivityInfo**)&(mDialogInfo->activityInfo));
        //} catch (RemoteException e) {
            //Log.e(TAG, "Unable to get className. Package manager is dead?");
            // Continue as the ActivityInfo isn't critical.
        //}
    }

    ShowConfirmationDialog();
    return NOERROR;
}

ECode CUninstallerActivity::StartUninstallProgress()
{
    AutoPtr<IIntent> newIntent;
    CIntent::New(IIntent::ACTION_VIEW, (IIntent**)&newIntent);
    newIntent->PutExtra(IIntent::EXTRA_USER, IParcelable::Probe(mDialogInfo->user));
    newIntent->PutExtra(IIntent::EXTRA_UNINSTALL_ALL_USERS, mDialogInfo->allUsers);
    newIntent->PutExtra(IPackageInstaller::EXTRA_CALLBACK, mDialogInfo->callback);
    newIntent->PutExtra(PackageUtil::INTENT_ATTR_APPLICATION_INFO, IParcelable::Probe(mDialogInfo->appInfo));

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    Boolean resTmp = FALSE;
    intent->GetBooleanExtra(IIntent::EXTRA_RETURN_RESULT, FALSE, &resTmp);
    if (resTmp) {
        newIntent->PutBooleanExtra(IIntent::EXTRA_RETURN_RESULT, TRUE);
        newIntent->AddFlags(IIntent::FLAG_ACTIVITY_FORWARD_RESULT);
    }

    newIntent->SetClass(this, ECLSID_CUninstallAppProgress);
    StartActivity(newIntent);
    return NOERROR;
}

ECode CUninstallerActivity::DispatchAborted()
{
    if (mDialogInfo != NULL && mDialogInfo->callback != NULL) {
        IIPackageDeleteObserver2* observer = IIPackageDeleteObserver2::Probe(mDialogInfo->callback);
        //try {
            String packageName;
            IPackageItemInfo::Probe(mDialogInfo->appInfo)->GetPackageName(&packageName);
            observer->OnPackageDeleted(packageName, IPackageManager::DELETE_FAILED_ABORTED, String("Cancelled by user"));
        //} catch (RemoteException ignored) {
        //}
    }
    return NOERROR;
}

void CUninstallerActivity::ShowConfirmationDialog()
{
    AutoPtr<UninstallAlertDialogFragment> fragment = new UninstallAlertDialogFragment();
    ShowDialogFragment(fragment);
}

void CUninstallerActivity::ShowAppNotFound()
{
    AutoPtr<AppNotFoundDialogFragment> fragment = new AppNotFoundDialogFragment();
    ShowDialogFragment(fragment);
}

void CUninstallerActivity::ShowDialogFragment(
    /* [in] */ IDialogFragment* fragment)
{
    AutoPtr<IFragmentManager> fragmentManager;
    GetFragmentManager((IFragmentManager**)&fragmentManager);
    AutoPtr<IFragmentTransaction> ft;
    fragmentManager->BeginTransaction((IFragmentTransaction**)&ft);

    AutoPtr<IFragment> prev;
    fragmentManager->FindFragmentByTag(String("dialog"), (IFragment**)&prev);
    if (prev != NULL) {
        ft->Remove(prev);
    }

    Int32 resTmp;
    fragment->Show(ft, String("dialog"), &resTmp);
}

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos
