
#include "elastos/droid/os/Process.h"
#include <Elastos.Droid.Net.h>
#include "elastos/apps/packageinstaller/CUninstallerActivity.h"
#include "elastos/apps/packageinstaller/PackageUtil.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

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
using Elastos::Apps::PackageInstaller::EIID_IUninstallerActivity;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

//=====================================================================
//          CUninstallerActivity::UninstallAlertDialogFragment
//=====================================================================
CAR_INTERFACE_IMPL(CUninstallerActivity::UninstallAlertDialogFragment, DialogFragment, IDialogInterfaceOnClickListener)

ECode CUninstallerActivity::UninstallAlertDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** result)
{
    VALIDATE_NOT_NULL(savedInstanceState);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // final PackageManager pm = getActivity().getPackageManager();
    // final DialogInfo dialogInfo = ((CUninstallerActivity) getActivity()).mDialogInfo;
    // final CharSequence appLabel = dialogInfo.appInfo.loadLabel(pm);
    //
    // AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(getActivity());
    // StringBuilder messageBuilder = new StringBuilder();
    //
    // // If the Activity label differs from the App label, then make sure the user
    // // knows the Activity belongs to the App being uninstalled.
    // if (dialogInfo.activityInfo != null) {
    //     final CharSequence activityLabel = dialogInfo.activityInfo.loadLabel(pm);
    //     if (!activityLabel.equals(appLabel)) {
    //         messageBuilder.append(
    //                 getString(R.string.uninstall_activity_text, activityLabel));
    //         messageBuilder.append(" ").append(appLabel).append(".\n\n");
    //     }
    // }
    //
    // final boolean isUpdate =
    //         ((dialogInfo.appInfo.flags & ApplicationInfo.FLAG_UPDATED_SYSTEM_APP) != 0);
    // if (isUpdate) {
    //     messageBuilder.append(getString(R.string.uninstall_update_text));
    // } else {
    //     UserManager userManager = UserManager.get(getActivity());
    //     if (dialogInfo.allUsers && userManager.getUserCount() >= 2) {
    //         messageBuilder.append(getString(R.string.uninstall_application_text_all_users));
    //     } else if (!dialogInfo.user.equals(android.os.Process.myUserHandle())) {
    //         UserInfo userInfo = userManager.getUserInfo(dialogInfo.user.getIdentifier());
    //         messageBuilder.append(
    //                 getString(R.string.uninstall_application_text_user, userInfo.name));
    //     } else {
    //         messageBuilder.append(getString(R.string.uninstall_application_text));
    //     }
    // }
    //
    // dialogBuilder.setTitle(appLabel);
    // dialogBuilder.setIcon(dialogInfo.appInfo.loadIcon(pm));
    // dialogBuilder.setPositiveButton(android.R.string.ok, this);
    // dialogBuilder.setNegativeButton(android.R.string.cancel, this);
    // dialogBuilder.setMessage(messageBuilder.toString());
    // return dialogBuilder.create();

    AutoPtr<IActivity> activity;
    //GetActivity((IActivity**)&activity);
    AutoPtr<IPackageManager> pm;
    IContext::Probe(activity)->GetPackageManager((IPackageManager**)&pm);

    CUninstallerActivity* uninstallerActivity = (CUninstallerActivity*)activity.Get();
    AutoPtr<CUninstallerActivity::DialogInfo> dialogInfo = uninstallerActivity->mDialogInfo;
    IPackageItemInfo* itemInfo = IPackageItemInfo::Probe(dialogInfo->appInfo.Get());
    AutoPtr<ICharSequence> appLabel;
    itemInfo->LoadLabel(pm, (ICharSequence**)&appLabel);

    AutoPtr<IAlertDialogBuilder> dialogBuilder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&dialogBuilder);
    AutoPtr<StringBuilder> messageBuilder = new StringBuilder();

    // If the Activity label differs from the App label, then make sure the user
    // knows the Activity belongs to the App being uninstalled.
    if (dialogInfo->activityInfo != NULL) {
        AutoPtr<ICharSequence> activityLabel;
        IPackageItemInfo::Probe(dialogInfo->activityInfo)->LoadLabel(pm, (ICharSequence**)&activityLabel);
        String str1, str2;
        activityLabel->ToString(&str1);
        appLabel->ToString(&str2);
        if (!str1.Equals(str2)) {
            String strTmp;
            //GetString(String("")/*R::string::uninstall_activity_text*/, activityLabel, &strTmp);
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
        //GetString(String("")/*R::string::uninstall_update_text*/, &strTmp);
        messageBuilder->Append(strTmp);
    }
    else {
        AutoPtr<IUserManagerHelper> helper;
        CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
        AutoPtr<IUserManager> userManager;
        helper->Get(IContext::Probe(activity), (IUserManager**)&userManager);

        Int32 count = 0;
        userManager->GetUserCount(&count);

        AutoPtr<IUserHandle> myUserHandle;
        Process::MyUserHandle((IUserHandle**)&myUserHandle);

        if (dialogInfo->allUsers && count >= 2) {
            String strTmp;
            //GetString(String("")/*R::string::uninstall_application_text_all_users*/, &strTmp);
            messageBuilder->Append(strTmp);
        }
        else if (!(TO_IINTERFACE(dialogInfo) == TO_IINTERFACE(myUserHandle))) {
            Int32 identify = 0;
            dialogInfo->user->GetIdentifier(&identify);
            AutoPtr<IUserInfo> userInfo;
            userManager->GetUserInfo(identify, (IUserInfo**)&userInfo);
            String name;
            userInfo->GetName(&name);
            String strTmp;
            //GetString(String("")/*R::string::uninstall_application_text_user*/, name, &strTmp);
            messageBuilder->Append(strTmp);
        }
        else {
            String strTmp;
            //GetString(String("")/*R::string::uninstall_application_text*/, &strTmp);
            messageBuilder->Append(strTmp);
        }
    }

    dialogBuilder->SetTitle(appLabel);
    AutoPtr<IDrawable> drawableTmp;
    IPackageItemInfo::Probe(dialogInfo->appInfo)->LoadIcon(pm, (IDrawable**)&drawableTmp);

    dialogBuilder->SetIcon(drawableTmp);
    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(String("")/*R::string::ok*/, (ICharSequence**)&charSequenceTmp);
    dialogBuilder->SetPositiveButton(charSequenceTmp, this);

    charSequenceTmp = NULL;
    CString::New(String("")/*R::string::cancel*/, (ICharSequence**)&charSequenceTmp);
    dialogBuilder->SetNegativeButton(charSequenceTmp, this);

    charSequenceTmp = NULL;
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
    VALIDATE_NOT_NULL(dialog);
    // ==================before translated======================
    // if (which == Dialog.BUTTON_POSITIVE) {
    //     ((CUninstallerActivity) getActivity()).startUninstallProgress();
    // } else {
    //     ((CUninstallerActivity) getActivity()).dispatchAborted();
    // }

    AutoPtr<IActivity> activity;
    //GetActivity((IActivity**)&activity);
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
    VALIDATE_NOT_NULL(dialog);
    // ==================before translated======================
    // super.onDismiss(dialog);
    // getActivity().finish();

    //DialogFragment::OnDismiss(dialog);
    AutoPtr<IActivity> activity;
    //GetActivity((IActivity**)&activity);
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
    VALIDATE_NOT_NULL(savedInstanceState);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new AlertDialog.Builder(getActivity())
    //         .setTitle(R.string.app_not_found_dlg_title)
    //         .setMessage(R.string.app_not_found_dlg_text)
    //         .setNeutralButton(android.R.string.ok, null)
    //         .create();

    AutoPtr<IActivity> activity;
    //GetActivity((IActivity**)&activity);
    AutoPtr<IAlertDialogBuilder> dialogBuilder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&dialogBuilder);

    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(String("")/*R::string::app_not_found_dlg_title*/, (ICharSequence**)&charSequenceTmp);
    dialogBuilder->SetTitle(charSequenceTmp);

    charSequenceTmp = NULL;
    CString::New(String("")/*R::string::app_not_found_dlg_text*/, (ICharSequence**)&charSequenceTmp);
    dialogBuilder->SetMessage(charSequenceTmp);

    charSequenceTmp = NULL;
    CString::New(String("")/*R::string::ok*/, (ICharSequence**)&charSequenceTmp);
    dialogBuilder->SetNeutralButton(charSequenceTmp, NULL);

    AutoPtr<IAlertDialog> alertDialog;
    dialogBuilder->Create((IAlertDialog**)&alertDialog);
    *result = IDialog::Probe(alertDialog);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CUninstallerActivity::AppNotFoundDialogFragment::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    VALIDATE_NOT_NULL(dialog);
    // ==================before translated======================
    // super.onDismiss(dialog);
    // ((CUninstallerActivity) getActivity()).dispatchAborted();
    // getActivity().setResult(Activity.RESULT_FIRST_USER);
    // getActivity().finish();

    //DialogFragment::OnDismiss(dialog);
    AutoPtr<IActivity> activity;
    //GetActivity((IActivity**)&activity);
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
    VALIDATE_NOT_NULL(icicle);
    // ==================before translated======================
    // super.onCreate(icicle);
    // // Get intent information.
    // // We expect an intent with URI of the form package://<packageName>#<className>
    // // className is optional; if specified, it is the activity the user chose to uninstall
    // final Intent intent = getIntent();
    // final Uri packageUri = intent.getData();
    // if (packageUri == null) {
    //     Log.e(TAG, "No package URI in intent");
    //     showAppNotFound();
    //     return;
    // }
    // final String packageName = packageUri.getEncodedSchemeSpecificPart();
    // if (packageName == null) {
    //     Log.e(TAG, "Invalid package name in URI: " + packageUri);
    //     showAppNotFound();
    //     return;
    // }
    //
    // final IPackageManager pm = IPackageManager.Stub.asInterface(
    //         ServiceManager.getService("package"));
    //
    // mDialogInfo = new DialogInfo();
    //
    // mDialogInfo.user = intent.getParcelableExtra(Intent.EXTRA_USER);
    // if (mDialogInfo.user == null) {
    //     mDialogInfo.user = android.os.Process.myUserHandle();
    // }
    //
    // mDialogInfo.allUsers = intent.getBooleanExtra(Intent.EXTRA_UNINSTALL_ALL_USERS, false);
    // mDialogInfo.callback = intent.getIBinderExtra(PackageInstaller.EXTRA_CALLBACK);
    //
    // try {
    //     mDialogInfo.appInfo = pm.getApplicationInfo(packageName,
    //             PackageManager.GET_UNINSTALLED_PACKAGES, mDialogInfo.user.getIdentifier());
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Unable to get packageName. Package manager is dead?");
    // }
    //
    // if (mDialogInfo.appInfo == null) {
    //     Log.e(TAG, "Invalid packageName: " + packageName);
    //     showAppNotFound();
    //     return;
    // }
    //
    // // The class name may have been specified (e.g. when deleting an app from all apps)
    // final String className = packageUri.getFragment();
    // if (className != null) {
    //     try {
    //         mDialogInfo.activityInfo = pm.getActivityInfo(
    //                 new ComponentName(packageName, className), 0,
    //                 mDialogInfo.user.getIdentifier());
    //     } catch (RemoteException e) {
    //         Log.e(TAG, "Unable to get className. Package manager is dead?");
    //         // Continue as the ActivityInfo isn't critical.
    //     }
    // }
    //
    // showConfirmationDialog();

    //Activity::OnCreate(icicle);
    // Get intent information.
    // We expect an intent with URI of the form package://<packageName>#<className>
    // className is optional; if specified, it is the activity the user chose to uninstall
    AutoPtr<IIntent> intent;
    //GetIntent((IIntent**)&intent);
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
    //--: no this func in car;
    //mDialogInfo.callback = intent->GetIBinderExtra(PackageInstaller.EXTRA_CALLBACK);

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
    // ==================before translated======================
    // Intent newIntent = new Intent(Intent.ACTION_VIEW);
    // newIntent.putExtra(Intent.EXTRA_USER, mDialogInfo.user);
    // newIntent.putExtra(Intent.EXTRA_UNINSTALL_ALL_USERS, mDialogInfo.allUsers);
    // newIntent.putExtra(PackageInstaller.EXTRA_CALLBACK, mDialogInfo.callback);
    // newIntent.putExtra(PackageUtil.INTENT_ATTR_APPLICATION_INFO, mDialogInfo.appInfo);
    // if (getIntent().getBooleanExtra(Intent.EXTRA_RETURN_RESULT, false)) {
    //     newIntent.putExtra(Intent.EXTRA_RETURN_RESULT, true);
    //     newIntent.addFlags(Intent.FLAG_ACTIVITY_FORWARD_RESULT);
    // }
    // newIntent.setClass(this, UninstallAppProgress.class);
    // startActivity(newIntent);

    AutoPtr<IIntent> newIntent;
    CIntent::New(IIntent::ACTION_VIEW, (IIntent**)&newIntent);
    newIntent->PutExtra(IIntent::EXTRA_USER, IParcelable::Probe(mDialogInfo->user));
    newIntent->PutExtra(IIntent::EXTRA_UNINSTALL_ALL_USERS, mDialogInfo->allUsers);
    //newIntent->PutExtra(IPackageInstaller::EXTRA_CALLBACK, mDialogInfo->callback);
    newIntent->PutExtra(PackageUtil::INTENT_ATTR_APPLICATION_INFO, IParcelable::Probe(mDialogInfo->appInfo));

    AutoPtr<IIntent> intent;
    //GetIntent((IIntent**)&intent);
    Boolean resTmp = FALSE;
    intent->GetBooleanExtra(IIntent::EXTRA_RETURN_RESULT, FALSE, &resTmp);
    if (resTmp) {
        newIntent->PutExtra(IIntent::EXTRA_RETURN_RESULT, TRUE);
        newIntent->AddFlags(IIntent::FLAG_ACTIVITY_FORWARD_RESULT);
    }

    assert(0);
    //--: no ClassID param; newIntent->SetClass(this, UninstallAppProgress.class);
    //StartActivity(newIntent);
    return NOERROR;
}

ECode CUninstallerActivity::DispatchAborted()
{
    // ==================before translated======================
    // if (mDialogInfo != null && mDialogInfo.callback != null) {
    //     final IPackageDeleteObserver2 observer = IPackageDeleteObserver2.Stub.asInterface(
    //             mDialogInfo.callback);
    //     try {
    //         observer.onPackageDeleted(mDialogInfo.appInfo.packageName,
    //                 PackageManager.DELETE_FAILED_ABORTED, "Cancelled by user");
    //     } catch (RemoteException ignored) {
    //     }
    // }

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
    // ==================before translated======================
    // showDialogFragment(new UninstallAlertDialogFragment());

    AutoPtr<UninstallAlertDialogFragment> fragment = new UninstallAlertDialogFragment();
    //ShowDialogFragment(fragment);
}

void CUninstallerActivity::ShowAppNotFound()
{
    // ==================before translated======================
    // showDialogFragment(new AppNotFoundDialogFragment());

    AutoPtr<AppNotFoundDialogFragment> fragment = new AppNotFoundDialogFragment();
    //ShowDialogFragment(fragment);
}

void CUninstallerActivity::ShowDialogFragment(
    /* [in] */ IDialogFragment* fragment)
{
    // ==================before translated======================
    // FragmentTransaction ft = getFragmentManager().beginTransaction();
    // Fragment prev = getFragmentManager().findFragmentByTag("dialog");
    // if (prev != null) {
    //     ft.remove(prev);
    // }
    // fragment.show(ft, "dialog");

    AutoPtr<IFragmentManager> fragmentManager;
    //GetFragmentManager((IFragmentManager**)&fragmentManager);
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
} // namespace Apps
} // namespace Elastos


