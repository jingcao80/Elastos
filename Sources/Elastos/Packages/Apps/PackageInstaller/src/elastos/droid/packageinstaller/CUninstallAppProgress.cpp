
#include "elastos/droid/packageinstaller/CUninstallAppProgress.h"
#include "elastos/droid/packageinstaller/CPackageDeleteObserver.h"
#include "elastos/droid/packageinstaller/PackageUtil.h"
#include "elastos/droid/os/Process.h"
#include <Elastos.Droid.Widget.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::CPackageDeleteObserver2;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::EIID_IIPackageDeleteObserver;
using Elastos::Droid::Content::Pm::IIPackageDeleteObserver2;
using Elastos::Droid::Content::Pm::IIPackageDeleteObserver;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageInstaller;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::Process;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Core::CString;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

//=====================================================================
//             CUninstallAppProgress::PackageDeleteObserver
//=====================================================================
CAR_INTERFACE_IMPL_2(CUninstallAppProgress::PackageDeleteObserver, Object, IIPackageDeleteObserver, IBinder)

CAR_OBJECT_IMPL(CPackageDeleteObserver)

ECode CUninstallAppProgress::PackageDeleteObserver::constructor(
    /* [in] */ IUninstallAppProgress* owner)
{
    mOwner = (CUninstallAppProgress*)owner;
    return NOERROR;
}

ECode CUninstallAppProgress::PackageDeleteObserver::PackageDeleted(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 returnCode)
{
    AutoPtr<IMessage> msg;
    mOwner->mHandler->ObtainMessage(UNINSTALL_COMPLETE, (IMessage**)&msg);
    msg->SetArg1(returnCode);

    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(packageName, (ICharSequence**)&charSequenceTmp);
    msg->SetObj(charSequenceTmp);
    Boolean resTmp;
    mOwner->mHandler->SendMessage(msg, &resTmp);
    return NOERROR;
}

ECode CUninstallAppProgress::PackageDeleteObserver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

CAR_INTERFACE_IMPL(CUninstallAppProgress::ViewOnClickListener, Object, IViewOnClickListener)

ECode CUninstallAppProgress::ViewOnClickListener::OnClick(
  /* [in] */ IView* v)
{
    return mOwner->OnClick(v);
}

//=====================================================================
//                  CUninstallAppProgress::InnerHandler
//=====================================================================
CUninstallAppProgress::InnerHandler::InnerHandler(
    /* [in] */ CUninstallAppProgress* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
    Handler::constructor();
}

ECode CUninstallAppProgress::InnerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);

    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case UNINSTALL_COMPLETE:
            {
                Int32 arg1 = 0;
                msg->GetArg1(&arg1);
                mOwner->mResultCode = arg1;
                AutoPtr<IInterface> interfaceTmp;
                msg->GetObj((IInterface**)&interfaceTmp);
                ICharSequence* charSequenceTmp = ICharSequence::Probe(interfaceTmp);
                String packageName;
                charSequenceTmp->ToString(&packageName);

                if (mCallback != NULL) {
                    IIPackageDeleteObserver2* observer = IIPackageDeleteObserver2::Probe(mCallback);
                    //try {
                        String appInfoPackageName;
                        IPackageItemInfo::Probe(mOwner->mAppInfo)->GetPackageName(&appInfoPackageName);
                        observer->OnPackageDeleted(appInfoPackageName, mOwner->mResultCode, packageName);
                    //} catch (RemoteException ignored) {
                    //}
                    mOwner->Finish();
                    return NOERROR;
                }

                AutoPtr<IIntent> intent;
                mOwner->GetIntent((IIntent**)&intent);
                Boolean resTmp = FALSE;
                intent->GetBooleanExtra(IIntent::EXTRA_RETURN_RESULT, FALSE, &resTmp);
                if (resTmp) {
                    AutoPtr<IIntent> result;
                    CIntent::New((IIntent**)&result);
                    result->PutExtra(IIntent::EXTRA_INSTALL_RESULT, mOwner->mResultCode);
                    mOwner->SetResult(mOwner->mResultCode == IPackageManager::DELETE_SUCCEEDED
                           ? IActivity::RESULT_OK : IActivity::RESULT_FIRST_USER,
                                  result);
                    mOwner->Finish();
                    return NOERROR;
                }

                // Update the status text
                String statusText;
                arg1 = 0;
                msg->GetArg1(&arg1);
                switch (arg1) {
                    case IPackageManager::DELETE_SUCCEEDED:
                        {
                            mOwner->GetString(R::string::uninstall_done, &statusText);
                            // Show a Toast and finish the activity
                            AutoPtr<IContext> ctx;
                            mOwner->GetBaseContext((IContext**)&ctx);

                            AutoPtr<IToastHelper> helper;
                            CToastHelper::AcquireSingleton((IToastHelper**)&helper);
                            AutoPtr<ICharSequence> charSequenceTmp1;
                            CString::New(statusText, (ICharSequence**)&charSequenceTmp1);
                            AutoPtr<IToast> toast;
                            helper->MakeText(ctx, charSequenceTmp1, IToast::LENGTH_LONG, (IToast**)&toast);
                            toast->Show();
                            mOwner->SetResultAndFinish(mOwner->mResultCode);
                        }
                        return NOERROR;
                    case IPackageManager::DELETE_FAILED_DEVICE_POLICY_MANAGER:
                        {
                            Logger::D(mOwner->TAG, "Uninstall failed because %s is a device admin",
                                packageName.string());
                            IView::Probe(mOwner->mDeviceManagerButton)->SetVisibility(IView::VISIBLE);
                            mOwner->GetString(R::string::uninstall_failed_device_policy_manager, &statusText);
                        }
                        break;
                    case IPackageManager::DELETE_FAILED_OWNER_BLOCKED:
                        {
                            AutoPtr<IInterface> interfaceTmp;
                            mOwner->GetSystemService(IContext::USER_SERVICE, (IInterface**)&interfaceTmp);
                            IUserManager* userManager = IUserManager::Probe(interfaceTmp);

                            AutoPtr<IServiceManager> serviceManager;
                            CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
                            interfaceTmp = NULL;
                            serviceManager->GetService(String("package"), (IInterface**)&interfaceTmp);
                            IIPackageManager* packageManager = IIPackageManager::Probe(interfaceTmp);

                            AutoPtr<IList> users;
                            userManager->GetUsers((IList**)&users);
                            Int32 blockingUserId = IUserHandle::USER_NULL;
                            Int32 usersSize = 0;
                            users->GetSize(&usersSize);
                            for (Int32 i = 0; i < usersSize; ++i) {
                                interfaceTmp = NULL;
                                users->Get(i, (IInterface**)&interfaceTmp);
                                IUserInfo* user = IUserInfo::Probe(interfaceTmp);
                                //try {
                                    Int32 userIdTmp = 0;
                                    user->GetId(&userIdTmp);
                                    Boolean resTmp = FALSE;
                                    packageManager->GetBlockUninstallForUser(packageName, userIdTmp, &resTmp);
                                    if (resTmp) {
                                        blockingUserId = userIdTmp;
                                        break;
                                    }
                                //} catch (RemoteException e) {
                                    // Shouldn't happen.
                                    //Log.e(TAG, "Failed to talk to package manager", e);
                                //}
                            }

                            IView::Probe(mOwner->mDeviceManagerButton)->SetVisibility(IView::VISIBLE);
                            if (blockingUserId == IUserHandle::USER_OWNER) {
                                mOwner->GetString(R::string::uninstall_blocked_device_owner, &statusText);
                            }
                            else if (blockingUserId == IUserHandle::USER_NULL) {
                                msg->GetArg1(&arg1);
                                Logger::D(mOwner->TAG, "Uninstall failed for %s with code %d no blocking user",
                                        packageName.string(), arg1);
                                mOwner->GetString(R::string::uninstall_failed, &statusText);
                            }
                            else {
                                AutoPtr<IUserInfo> userInfo;
                                userManager->GetUserInfo(blockingUserId, (IUserInfo**)&userInfo);
                                String userName;
                                userInfo->GetName(&userName);

                                String strTmp;
                                mOwner->GetString(R::string::uninstall_blocked_profile_owner, &strTmp);
                                statusText.AppendFormat(strTmp, userName.string());
                            }
                        }
                        break;
                    default:
                        {
                            msg->GetArg1(&arg1);
                            Logger::D(mOwner->TAG, "Uninstall failed for %s with code %d",
                                    packageName.string(), arg1);
                            mOwner->GetString(R::string::uninstall_failed, &statusText);
                        }
                        break;
                }

                AutoPtr<ICharSequence> statusTextCharSequence;
                CString::New(statusText, (ICharSequence**)&statusTextCharSequence);
                mOwner->mStatusTextView->SetText(statusTextCharSequence);

                // Hide the progress bar; Show the ok button
                IView::Probe(mOwner->mProgressBar)->SetVisibility(IView::INVISIBLE);
                mOwner->mOkPanel->SetVisibility(IView::VISIBLE);
            }
            break;
        default:
            break;
    }
    return NOERROR;
}

//=====================================================================
//             CUninstallAppProgress::InnerOnClickListener
//=====================================================================
CAR_INTERFACE_IMPL(CUninstallAppProgress::InnerOnClickListener, Object, IViewOnClickListener)

CUninstallAppProgress::InnerOnClickListener::InnerOnClickListener(
    /* [in] */ CUninstallAppProgress* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

ECode CUninstallAppProgress::InnerOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetClassName(String("Settings"), String("Settings.Settings$DeviceAdminSettingsActivity"));
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NO_HISTORY | IIntent::FLAG_ACTIVITY_NEW_TASK);
    mOwner->StartActivity(intent);
    mOwner->Finish();
    return NOERROR;
}

//=====================================================================
//                         CUninstallAppProgress
//=====================================================================
const Int32 CUninstallAppProgress::UNINSTALL_COMPLETE;

CAR_INTERFACE_IMPL_2(CUninstallAppProgress, Activity, IViewOnClickListener, IUninstallAppProgress)

CAR_OBJECT_IMPL(CUninstallAppProgress)

CUninstallAppProgress::CUninstallAppProgress()
{
    mHandler = new InnerHandler(this);
}

ECode CUninstallAppProgress::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);

    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(PackageUtil::INTENT_ATTR_APPLICATION_INFO, (IParcelable**)&parcelable);
    mAppInfo = IApplicationInfo::Probe(parcelable);
    intent->GetBooleanExtra(IIntent::EXTRA_UNINSTALL_ALL_USERS, FALSE, &mAllUsers);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 myUserId = 0;
    helper->GetMyUserId(&myUserId);
    if (mAllUsers && myUserId != IUserHandle::USER_OWNER) {
        //throw new SecurityException("Only owner user can request uninstall for all users");
        return E_SECURITY_EXCEPTION;
    }

    parcelable = NULL;
    intent->GetParcelableExtra(IIntent::EXTRA_USER, (IParcelable**)&parcelable);
    mUser = IUserHandle::Probe(parcelable);
    if (mUser == NULL) {
        Process::MyUserHandle((IUserHandle**)&mUser);
    }
    else {
        AutoPtr<IInterface> interfaceTmp;
        GetSystemService(IContext::USER_SERVICE, (IInterface**)&interfaceTmp);
        IUserManager* userManager = IUserManager::Probe(interfaceTmp);
        AutoPtr<IList> profiles;
        userManager->GetUserProfiles((IList**)&profiles);
        Boolean resTmp = FALSE;
        profiles->Contains(TO_IINTERFACE(mUser), &resTmp);
        if (!resTmp) {
            //throw new SecurityException("User " + android.os.Process.myUserHandle() + " can't "
            //        + "request uninstall for user " + mUser);
            return E_SECURITY_EXCEPTION;
        }
    }

    intent->GetIBinderExtra(IPackageInstaller::EXTRA_CALLBACK, (IBinder**)&mCallback);
    InitView();
    return NOERROR;
}

ECode CUninstallAppProgress::SetResultAndFinish(
    /* [in] */ Int32 retCode)
{
    SetResult(retCode);
    Finish();
    return NOERROR;
}

ECode CUninstallAppProgress::InitView()
{
    Int32 flags = 0;
    mAppInfo->GetFlags(&flags);
    Boolean isUpdate = ((flags & IApplicationInfo::FLAG_UPDATED_SYSTEM_APP) != 0);
    SetTitle(isUpdate ? R::string::uninstall_update_title : R::string::uninstall_application_title);

    SetContentView(R::layout::uninstall_progress);
    // Initialize views
    AutoPtr<IView> snippetView;
    FindViewById(R::id::app_snippet, (IView**)&snippetView);
    PackageUtil::InitSnippetForInstalledApp(this, mAppInfo, snippetView);

    AutoPtr<IView> viewTmp;
    FindViewById(R::id::center_text, (IView**)&viewTmp);
    mStatusTextView = ITextView::Probe(viewTmp);
    mStatusTextView->SetText(R::string::uninstalling);

    viewTmp = NULL;
    FindViewById(R::id::device_manager_button, (IView**)&viewTmp);
    mDeviceManagerButton = IButton::Probe(viewTmp);
    IView::Probe(mDeviceManagerButton)->SetVisibility(IView::GONE);

    AutoPtr<IViewOnClickListener> listener = new InnerOnClickListener(this);
    IView::Probe(mDeviceManagerButton)->SetOnClickListener(listener);

    viewTmp = NULL;
    FindViewById(R::id::progress_bar, (IView**)&viewTmp);
    mProgressBar = IProgressBar::Probe(viewTmp);
    mProgressBar->SetIndeterminate(TRUE);

    // Hide button till progress is being displayed
    FindViewById(R::id::ok_panel, (IView**)&mOkPanel);

    viewTmp = NULL;
    FindViewById(R::id::ok_button, (IView**)&viewTmp);
    mOkButton = IButton::Probe(viewTmp);
    listener = new ViewOnClickListener(this);
    IView::Probe(mOkButton)->SetOnClickListener(listener);
    IView::Probe(mOkPanel)->SetVisibility(IView::INVISIBLE);

    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    AutoPtr<IInterface> interfaceTmp;
    serviceManager->GetService(String("package"), (IInterface**)&interfaceTmp);
    IIPackageManager* packageManager = IIPackageManager::Probe(interfaceTmp);

    AutoPtr<IIPackageDeleteObserver> observer;
    CPackageDeleteObserver::New(this, (IIPackageDeleteObserver**)&observer);
    //try {
        String packageName;
        IPackageItemInfo::Probe(mAppInfo)->GetPackageName(&packageName);
        Int32 identify = 0;
        mUser->GetIdentifier(&identify);
        packageManager->DeletePackageAsUser(packageName, observer, identify,
                mAllUsers ? IPackageManager::DELETE_ALL_USERS : 0);
    //} catch (RemoteException e) {
        // Shouldn't happen.
        //Log.e(TAG, "Failed to talk to package manager", e);
    //}
    return NOERROR;
}

ECode CUninstallAppProgress::OnClick(
    /* [in] */ IView* v)
{
    if(v == IView::Probe(mOkButton)) {
        String packageName;
        IPackageItemInfo::Probe(mAppInfo)->GetPackageName(&packageName);
        Logger::I(TAG, "Finished uninstalling pkg: %s", packageName.string());
        SetResultAndFinish(mResultCode);
    }
    return NOERROR;
}

ECode CUninstallAppProgress::DispatchKeyEvent(
    /* [in] */ IKeyEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 keyCode = 0;
    ev->GetKeyCode(&keyCode);
    if (keyCode == IKeyEvent::KEYCODE_BACK) {
        if (mResultCode == -1) {
            // Ignore back key when installation is in progress
            *result = TRUE;
            return NOERROR;
        }
        else {
            // If installation is done, just set the result code
            SetResult(mResultCode);
        }
    }
    return Activity::DispatchKeyEvent(ev, result);
}

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos
