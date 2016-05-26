
#include "elastos/droid/os/Process.h"
#include "elastos/apps/packageinstaller/CUninstallAppProgress.h"
#include "elastos/apps/packageinstaller/PackageUtil.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::App::CPackageDeleteObserver2;
using Elastos::Droid::App::EIID_IPackageDeleteObserver;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::EIID_IIPackageDeleteObserver;
using Elastos::Droid::Content::Pm::IIPackageDeleteObserver2;
using Elastos::Droid::Content::Pm::IIPackageDeleteObserver;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::Process;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Apps::PackageInstaller::EIID_IUninstallAppProgress;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

//=====================================================================
//             CUninstallAppProgress::PackageDeleteObserver
//=====================================================================
CAR_INTERFACE_IMPL(CUninstallAppProgress::PackageDeleteObserver, Object, IIPackageDeleteObserver)

CUninstallAppProgress::PackageDeleteObserver::PackageDeleteObserver(
    /* [in] */ CUninstallAppProgress* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

ECode CUninstallAppProgress::PackageDeleteObserver::PackageDeleted(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 returnCode)
{
    // ==================before translated======================
    // Message msg = mHandler.obtainMessage(UNINSTALL_COMPLETE);
    // msg.arg1 = returnCode;
    // msg.obj = packageName;
    // mHandler.sendMessage(msg);

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

//=====================================================================
//                  CUninstallAppProgress::InnerHandler
//=====================================================================
CUninstallAppProgress::InnerHandler::InnerHandler(
    /* [in] */ CUninstallAppProgress* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(NULL != mOwner);
}

ECode CUninstallAppProgress::InnerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    // switch (msg.what) {
    //     case UNINSTALL_COMPLETE:
    //         mResultCode = msg.arg1;
    //         final String packageName = (String) msg.obj;
    //
    //         if (mCallback != null) {
    //             final IPackageDeleteObserver2 observer = IPackageDeleteObserver2.Stub
    //                     .asInterface(mCallback);
    //             try {
    //                 observer.onPackageDeleted(mAppInfo.packageName, mResultCode,
    //                         packageName);
    //             } catch (RemoteException ignored) {
    //             }
    //             finish();
    //             return;
    //         }
    //
    //         if (getIntent().getBooleanExtra(Intent.EXTRA_RETURN_RESULT, false)) {
    //             Intent result = new Intent();
    //             result.putExtra(Intent.EXTRA_INSTALL_RESULT, mResultCode);
    //             setResult(mResultCode == PackageManager.DELETE_SUCCEEDED
    //                     ? Activity.RESULT_OK : Activity.RESULT_FIRST_USER,
    //                             result);
    //             finish();
    //             return;
    //         }
    //
    //         // Update the status text
    //         final String statusText;
    //         switch (msg.arg1) {
    //             case PackageManager.DELETE_SUCCEEDED:
    //                 statusText = getString(R.string.uninstall_done);
    //                 // Show a Toast and finish the activity
    //                 Context ctx = getBaseContext();
    //                 Toast.makeText(ctx, statusText, Toast.LENGTH_LONG).show();
    //                 setResultAndFinish(mResultCode);
    //                 return;
    //             case PackageManager.DELETE_FAILED_DEVICE_POLICY_MANAGER:
    //                 Log.d(TAG, "Uninstall failed because " + packageName
    //                         + " is a device admin");
    //                 mDeviceManagerButton.setVisibility(View.VISIBLE);
    //                 statusText = getString(R.string.uninstall_failed_device_policy_manager);
    //                 break;
    //             case PackageManager.DELETE_FAILED_OWNER_BLOCKED:
    //                 UserManager userManager =
    //                         (UserManager) getSystemService(Context.USER_SERVICE);
    //                 IPackageManager packageManager = IPackageManager.Stub.asInterface(
    //                         ServiceManager.getService("package"));
    //                 List<UserInfo> users = userManager.getUsers();
    //                 int blockingUserId = UserHandle.USER_NULL;
    //                 for (int i = 0; i < users.size(); ++i) {
    //                     final UserInfo user = users.get(i);
    //                     try {
    //                         if (packageManager.getBlockUninstallForUser(packageName,
    //                                 user.id)) {
    //                             blockingUserId = user.id;
    //                             break;
    //                         }
    //                     } catch (RemoteException e) {
    //                         // Shouldn't happen.
    //                         Log.e(TAG, "Failed to talk to package manager", e);
    //                     }
    //                 }
    //                 mDeviceManagerButton.setVisibility(View.VISIBLE);
    //                 if (blockingUserId == UserHandle.USER_OWNER) {
    //                     statusText = getString(R.string.uninstall_blocked_device_owner);
    //                 } else if (blockingUserId == UserHandle.USER_NULL) {
    //                     Log.d(TAG, "Uninstall failed for " + packageName + " with code "
    //                             + msg.arg1 + " no blocking user");
    //                     statusText = getString(R.string.uninstall_failed);
    //                 } else {
    //                     String userName = userManager.getUserInfo(blockingUserId).name;
    //                     statusText = String.format(
    //                             getString(R.string.uninstall_blocked_profile_owner),
    //                             userName);
    //                 }
    //                 break;
    //             default:
    //                 Log.d(TAG, "Uninstall failed for " + packageName + " with code "
    //                         + msg.arg1);
    //                 statusText = getString(R.string.uninstall_failed);
    //                 break;
    //         }
    //         mStatusTextView.setText(statusText);
    //
    //         // Hide the progress bar; Show the ok button
    //         mProgressBar.setVisibility(View.INVISIBLE);
    //         mOkPanel.setVisibility(View.VISIBLE);
    //         break;
    //     default:
    //         break;
    // }

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

                if (0/*mCallback != NULL*/) {
                    IIPackageDeleteObserver2* observer;// = IIPackageDeleteObserver2::Probe(mCallback);
                    //try {
                        String appInfoPackageName;
                        IPackageItemInfo::Probe(mOwner->mAppInfo)->GetPackageName(&appInfoPackageName);
                        observer->OnPackageDeleted(appInfoPackageName, mOwner->mResultCode, packageName);
                    //} catch (RemoteException ignored) {
                    //}
                    //Finish();
                    return NOERROR;
                }

                AutoPtr<IIntent> intent;
                //GetIntent((IIntent**)&intent);
                Boolean resTmp = FALSE;
                intent->GetBooleanExtra(IIntent::EXTRA_RETURN_RESULT, FALSE, &resTmp);
                if (resTmp) {
                    AutoPtr<IIntent> result;
                    CIntent::New((IIntent**)&result);
                    result->PutExtra(IIntent::EXTRA_INSTALL_RESULT, mOwner->mResultCode);
                    //SetResult(mResultCode == IPackageManager::DELETE_SUCCEEDED
                    //        ? IActivity::RESULT_OK : IActivity::RESULT_FIRST_USER,
                    //               result);
                    //Finish();
                    return NOERROR;
                }

                // Update the status text
                String statusText;
                arg1 = 0;
                msg->GetArg1(&arg1);
                switch (arg1) {
                    case IPackageManager::DELETE_SUCCEEDED:
                        {
                            //GetString(R::string::uninstall_done, &statusText);
                            // Show a Toast and finish the activity
                            AutoPtr<IContext> ctx;
                            //GetBaseContext((IContext**)&ctx);

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
                            Logger::D(mOwner->TAG, String("Uninstall failed because ") + packageName
                                + String(" is a device admin"));
                            IView::Probe(mOwner->mDeviceManagerButton)->SetVisibility(IView::VISIBLE);
                            //GetString(R::string::uninstall_failed_device_policy_manager, &statusText);
                        }
                        break;
                    case IPackageManager::DELETE_FAILED_OWNER_BLOCKED:
                        {
                            AutoPtr<IInterface> interfaceTmp;
                            //GetSystemService(IContext::USER_SERVICE, (IInterface**)&interfaceTmp);
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
                                //GetString(R::string::uninstall_blocked_device_owner, &statusText);
                            }
                            else if (blockingUserId == IUserHandle::USER_NULL) {
                                msg->GetArg1(&arg1);
                                Logger::D(mOwner->TAG, String("Uninstall failed for ") + packageName + String(" with code ")
                                        + StringUtils::ToString(arg1) + String(" no blocking user"));
                                //GetString(R::string::uninstall_failed, &statusText);
                            }
                            else {
                                AutoPtr<IUserInfo> userInfo;
                                userManager->GetUserInfo(blockingUserId, (IUserInfo**)&userInfo);
                                String userName;
                                userInfo->GetName(&userName);

                                String strTmp;
                                //GetString(R::string::uninstall_blocked_profile_owner, &strTmp);
                                //--: where is String::Format
                                //statusText = String.Format(strTmp, userName);
                            }
                        }
                        break;
                    default:
                        {
                            msg->GetArg1(&arg1);
                            Logger::D(mOwner->TAG, String("Uninstall failed for ") + packageName + String(" with code ")
                                    + StringUtils::ToString(arg1));
                            //GetString(R::string::uninstall_failed, &statusText);
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
    // ==================before translated======================
    // mOwner = owner;

    assert(NULL != mOwner);
}

ECode CUninstallAppProgress::InnerOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    VALIDATE_NOT_NULL(v);
    // ==================before translated======================
    // Intent intent = new Intent();
    // intent.setClassName("com.android.settings",
    //         "com.android.settings.Settings$DeviceAdminSettingsActivity");
    // intent.setFlags(Intent.FLAG_ACTIVITY_NO_HISTORY | Intent.FLAG_ACTIVITY_NEW_TASK);
    // startActivity(intent);
    // finish();

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetClassName(String("Settings"), String("Settings.Settings$DeviceAdminSettingsActivity"));
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NO_HISTORY | IIntent::FLAG_ACTIVITY_NEW_TASK);
    //StartActivity(intent);
    //Finish();
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
}

ECode CUninstallAppProgress::OnCreate(
    /* [in] */ IBundle* icicle)
{
    VALIDATE_NOT_NULL(icicle);
    // ==================before translated======================
    // super.onCreate(icicle);
    // Intent intent = getIntent();
    // mAppInfo = intent.getParcelableExtra(PackageUtil.INTENT_ATTR_APPLICATION_INFO);
    // mAllUsers = intent.getBooleanExtra(Intent.EXTRA_UNINSTALL_ALL_USERS, false);
    // if (mAllUsers && UserHandle.myUserId() != UserHandle.USER_OWNER) {
    //     throw new SecurityException("Only owner user can request uninstall for all users");
    // }
    // mUser = intent.getParcelableExtra(Intent.EXTRA_USER);
    // if (mUser == null) {
    //     mUser = android.os.Process.myUserHandle();
    // } else {
    //     UserManager userManager = (UserManager) getSystemService(Context.USER_SERVICE);
    //     List<UserHandle> profiles = userManager.getUserProfiles();
    //     if (!profiles.contains(mUser)) {
    //         throw new SecurityException("User " + android.os.Process.myUserHandle() + " can't "
    //                 + "request uninstall for user " + mUser);
    //     }
    // }
    // mCallback = intent.getIBinderExtra(PackageInstaller.EXTRA_CALLBACK);
    // initView();

    //Activity::OnCreate(icicle);
    AutoPtr<IIntent> intent;
    //GetIntent((IIntent**)&intent);

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
        //GetSystemService(IContext::USER_SERVICE, (IInterface**)&interfaceTmp);
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

    //mCallback = intent->GetIBinderExtra(IPackageInstaller::EXTRA_CALLBACK);
    InitView();
    return NOERROR;
}

ECode CUninstallAppProgress::SetResultAndFinish(
    /* [in] */ Int32 retCode)
{
    // ==================before translated======================
    // setResult(retCode);
    // finish();

    //SetResult(retCode);
    //Finish();
    return NOERROR;
}

ECode CUninstallAppProgress::InitView()
{
    // ==================before translated======================
    // boolean isUpdate = ((mAppInfo.flags & ApplicationInfo.FLAG_UPDATED_SYSTEM_APP) != 0);
    // setTitle(isUpdate ? R.string.uninstall_update_title : R.string.uninstall_application_title);
    //
    // setContentView(R.layout.uninstall_progress);
    // // Initialize views
    // View snippetView = findViewById(R.id.app_snippet);
    // PackageUtil.initSnippetForInstalledApp(this, mAppInfo, snippetView);
    // mStatusTextView = (TextView) findViewById(R.id.center_text);
    // mStatusTextView.setText(R.string.uninstalling);
    // mDeviceManagerButton = (Button) findViewById(R.id.device_manager_button);
    // mDeviceManagerButton.setVisibility(View.GONE);
    // mDeviceManagerButton.setOnClickListener(new OnClickListener() {
    //     @Override
    //     public void onClick(View v) {
    //         Intent intent = new Intent();
    //         intent.setClassName("com.android.settings",
    //                 "com.android.settings.Settings$DeviceAdminSettingsActivity");
    //         intent.setFlags(Intent.FLAG_ACTIVITY_NO_HISTORY | Intent.FLAG_ACTIVITY_NEW_TASK);
    //         startActivity(intent);
    //         finish();
    //     }
    // });
    // mProgressBar = (ProgressBar) findViewById(R.id.progress_bar);
    // mProgressBar.setIndeterminate(true);
    // // Hide button till progress is being displayed
    // mOkPanel = (View) findViewById(R.id.ok_panel);
    // mOkButton = (Button) findViewById(R.id.ok_button);
    // mOkButton.setOnClickListener(this);
    // mOkPanel.setVisibility(View.INVISIBLE);
    // IPackageManager packageManager =
    //         IPackageManager.Stub.asInterface(ServiceManager.getService("package"));
    // PackageDeleteObserver observer = new PackageDeleteObserver();
    // try {
    //     packageManager.deletePackageAsUser(mAppInfo.packageName, observer,
    //             mUser.getIdentifier(),
    //             mAllUsers ? PackageManager.DELETE_ALL_USERS : 0);
    // } catch (RemoteException e) {
    //     // Shouldn't happen.
    //     Log.e(TAG, "Failed to talk to package manager", e);
    // }

    Int32 flags = 0;
    mAppInfo->GetFlags(&flags);
    //Boolean isUpdate = ((flags & IApplicationInfo::FLAG_UPDATED_SYSTEM_APP) != 0);
    //SetTitle(isUpdate ? R::string::uninstall_update_title : R::string::uninstall_application_title);

    //SetContentView(R::layout::uninstall_progress);
    // Initialize views
    AutoPtr<IView> snippetView;
    //FindViewById(R::id::app_snippet, (IView**)&snippetView);
    PackageUtil::InitSnippetForInstalledApp(NULL/*this*/, mAppInfo, snippetView);

    AutoPtr<IView> viewTmp;
    //FindViewById(R::id::center_text, (IView**)&viewTmp);
    mStatusTextView = ITextView::Probe(viewTmp);

    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(String("")/*R::string::uninstalling*/, (ICharSequence**)&charSequenceTmp);
    mStatusTextView->SetText(charSequenceTmp);

    viewTmp = NULL;
    //FindViewById(R::id::device_manager_button, (IView**)&viewTmp);
    mDeviceManagerButton = IButton::Probe(viewTmp);
    IView::Probe(mDeviceManagerButton)->SetVisibility(IView::GONE);

    AutoPtr<IViewOnClickListener> listener = new InnerOnClickListener(this);
    IView::Probe(mDeviceManagerButton)->SetOnClickListener(listener);

    viewTmp = NULL;
    //FindViewById(R::id::progress_bar, (IView**)&viewTmp);
    mProgressBar = IProgressBar::Probe(viewTmp);
    mProgressBar->SetIndeterminate(TRUE);

    // Hide button till progress is being displayed
    //FindViewById(R::id::ok_panel, (IView**)&mOkPanel);

    viewTmp = NULL;
    //FindViewById(R::id::ok_button, (IView**)&viewTmp);
    mOkButton = IButton::Probe(viewTmp);
    IView::Probe(mOkButton)->SetOnClickListener(this);
    IView::Probe(mOkPanel)->SetVisibility(IView::INVISIBLE);

    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    AutoPtr<IInterface> interfaceTmp;
    serviceManager->GetService(String("package"), (IInterface**)&interfaceTmp);
    IIPackageManager* packageManager = IIPackageManager::Probe(interfaceTmp);

    AutoPtr<PackageDeleteObserver> observer = new PackageDeleteObserver(this);
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
    VALIDATE_NOT_NULL(v);
    // ==================before translated======================
    // if(v == mOkButton) {
    //     Log.i(TAG, "Finished uninstalling pkg: " + mAppInfo.packageName);
    //     setResultAndFinish(mResultCode);
    // }

    if(TO_IINTERFACE(v) == TO_IINTERFACE(mOkButton)) {
        String packageName;
        IPackageItemInfo::Probe(mAppInfo)->GetPackageName(&packageName);
        Logger::I(TAG, String("Finished uninstalling pkg: ") + packageName);
        SetResultAndFinish(mResultCode);
    }
    return NOERROR;
}

ECode CUninstallAppProgress::DispatchKeyEvent(
    /* [in] */ IKeyEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(ev);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (ev.getKeyCode() == KeyEvent.KEYCODE_BACK) {
    //     if (mResultCode == -1) {
    //         // Ignore back key when installation is in progress
    //         return true;
    //     } else {
    //         // If installation is done, just set the result code
    //         setResult(mResultCode);
    //     }
    // }
    // return super.dispatchKeyEvent(ev);

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
            //SetResult(mResultCode);
        }
    }
    return NOERROR; //Activity::DispatchKeyEvent(ev, result);
}

} // namespace PackageInstaller
} // namespace Apps
} // namespace Elastos


