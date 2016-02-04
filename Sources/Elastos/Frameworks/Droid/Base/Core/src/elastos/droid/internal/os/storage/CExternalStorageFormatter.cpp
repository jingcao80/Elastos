
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/internal/os/storage/CExternalStorageFormatter.h"
#include "elastos/droid/app/CProgressDialog.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/widget/Toast.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::Toast;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {
namespace Storage {

CExternalStorageFormatter::MyStorageEventListener::MyStorageEventListener(
    /* [in] */ CExternalStorageFormatter* owner)
    : mOwner(owner)
{
}

ECode CExternalStorageFormatter::MyStorageEventListener::OnStorageStateChanged(
    /* [in] */ const String& path,
    /* [in] */ const String& oldState,
    /* [in] */ const String& newState)
{
    Logger::I(TAG, "Received storage state changed notification that %s changed state from %s to %s",
        path.string(), oldState.string(), newState.string());
    mOwner->UpdateProgressState();
    return NOERROR;
}

CExternalStorageFormatter::MyThread::MyThread(
    /* [in] */ CExternalStorageFormatter* owner,
    /* [in] */ const String& extStoragePath)
    : mOwner(owner)
    , mExtStoragePath(extStoragePath)
{
}

ECode CExternalStorageFormatter::MyThread::Run()
{
    Boolean success = FALSE;
    AutoPtr<IIMountService> mountService = mOwner->GetMountService();
    Int32 res;
    if (FAILED(mountService->FormatVolume(mExtStoragePath, &res))) {
        AutoPtr<IToast> toast;
        Toast::MakeText(mOwner, R::string::format_error, IToast::LENGTH_LONG, (IToast**)&toast);
        toast->Show();
    }
    else
        success = TRUE;

    if (success) {
        if (mOwner->mFactoryReset) {
            AutoPtr<IIntent> intent;
            CIntent::New(IIntent::ACTION_MASTER_CLEAR, (IIntent**)&intent);
            intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
            intent->PutExtra(IIntent::EXTRA_REASON, mOwner->mReason);
            mOwner->SendBroadcast(intent);
            // Intent handling is asynchronous -- assume it will happen soon.
            mOwner->StopSelf();
            return NOERROR;
        }
    }
    // If we didn't succeed, or aren't doing a full factory
    // reset, then it is time to remount the storage.
    if (!success && mOwner->mAlwaysReset) {
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_MASTER_CLEAR, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
        intent->PutExtra(IIntent::EXTRA_REASON, mOwner->mReason);
        mOwner->SendBroadcast(intent);
    }
    else {
        if (FAILED(mountService->MountVolume(mExtStoragePath, &res))) {
            Logger::W(TAG, "Failed talking with mount service");
        }
    }
    mOwner->StopSelf();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CExternalStorageFormatter::OnCancelListener, Object, IDialogInterfaceOnCancelListener)

CExternalStorageFormatter::OnCancelListener::OnCancelListener(
    /* [in] */ CExternalStorageFormatter* owner)
    : mOwner(owner)
{
}

ECode CExternalStorageFormatter::OnCancelListener::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    return mOwner->OnCancel(dialog);
}

const String CExternalStorageFormatter::TAG("ExternalStorageFormatter");

static AutoPtr<IComponentName> InitComponentName()
{
    AutoPtr<CComponentName> cn;
    CComponentName::NewByFriend(String("android"), String("CExternalStorageFormatter"), (CComponentName**)&cn);
    return cn;
}

const AutoPtr<IComponentName> CExternalStorageFormatter::COMPONENT_NAME = InitComponentName();

CAR_OBJECT_IMPL(CExternalStorageFormatter);
CAR_INTERFACE_IMPL(CExternalStorageFormatter, Service, IExternalStorageFormatter);

CExternalStorageFormatter::CExternalStorageFormatter()
    : mFactoryReset(FALSE)
    , mAlwaysReset(FALSE)
{
    mStorageListener = new MyStorageEventListener(this);
}

ECode CExternalStorageFormatter::constructor()
{
    return NOERROR;
}

ECode CExternalStorageFormatter::OnCreate()
{
    Service::OnCreate();

    if (mStorageManager == NULL) {
        AutoPtr<IInterface> service;
        GetSystemService(IContext::STORAGE_SERVICE, (IInterface**)&service);
        mStorageManager = IStorageManager::Probe(service);
        mStorageManager->RegisterListener(mStorageListener);
    }

    AutoPtr<IInterface> service;
    GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
    mWakeLock = NULL;
    IPowerManager::Probe(service)->NewWakeLock(
        IPowerManager::PARTIAL_WAKE_LOCK, String("ExternalStorageFormatter"),
        (IPowerManagerWakeLock**)&mWakeLock);
    mWakeLock->AcquireLock();

    return NOERROR;
}

ECode CExternalStorageFormatter::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* cmd)
{
    VALIDATE_NOT_NULL(cmd)
    String action;
    intent->GetAction(&action);
    if (FORMAT_AND_FACTORY_RESET.Equals(action)) {
        mFactoryReset = TRUE;
    }
    Boolean res;
    intent->GetBooleanExtra(EXTRA_ALWAYS_RESET, FALSE, &res);
    if (res) {
        mAlwaysReset = TRUE;
    }

    intent->GetStringExtra(IIntent::EXTRA_REASON, &mReason);
    AutoPtr<IParcelable> value;
    intent->GetParcelableExtra(IStorageVolume::EXTRA_STORAGE_VOLUME, (IParcelable**)&value);
    mStorageVolume = IStorageVolume::Probe(value);

    if (mProgressDialog == NULL) {
        CProgressDialog::New(this, (IProgressDialog**)&mProgressDialog);
        AutoPtr<IDialog> dialog = IDialog::Probe(mProgressDialog);
        mProgressDialog->SetIndeterminate(TRUE);
        dialog->SetCancelable(TRUE);
        AutoPtr<IWindow> window;
        dialog->GetWindow((IWindow**)&window);
        window->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
        if (!mAlwaysReset) {
            AutoPtr<OnCancelListener> listener = new OnCancelListener(this);
            dialog->SetOnCancelListener(listener);
        }
        UpdateProgressState();
        dialog->Show();
    }

    *cmd = Service::START_REDELIVER_INTENT;
    return NOERROR;
}

ECode CExternalStorageFormatter::OnDestroy()
{
    if (mStorageManager != NULL) {
        mStorageManager->UnregisterListener(mStorageListener);
    }
    if (mProgressDialog != NULL) {
        IDialogInterface::Probe(mProgressDialog)->Dismiss();
    }
    mWakeLock->ReleaseLock();
    return Service::OnDestroy();
}

ECode CExternalStorageFormatter::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode CExternalStorageFormatter::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    AutoPtr<IIMountService> mountService = GetMountService();
    String extStoragePath;
    if (mStorageVolume == NULL)
        Environment::GetLegacyExternalStorageDirectory()->ToString(&extStoragePath);
    else
        mStorageVolume->GetPath(&extStoragePath);

    Int32 res;
    if (FAILED(mountService->MountVolume(extStoragePath, &res)))
        Logger::W(TAG, "Failed talking with mount service");

    StopSelf();
    return NOERROR;
}

void CExternalStorageFormatter::Fail(
    /* [in] */ Int32 msg)
{
    AutoPtr<IToast> toast;
    Toast::MakeText(this, msg, IToast::LENGTH_LONG, (IToast**)&toast);
    toast->Show();

    if (mAlwaysReset) {
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_MASTER_CLEAR, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
        intent->PutExtra(IIntent::EXTRA_REASON, mReason);
        SendBroadcast(intent);
    }
    StopSelf();
}

void CExternalStorageFormatter::UpdateProgressState()
{
    String status;
    if (mStorageVolume == NULL)
        status = Environment::GetExternalStorageState();
    else {
        String path;
        mStorageVolume->GetPath(&path);
        mStorageManager->GetVolumeState(path, &status);
    }

    if (Environment::MEDIA_MOUNTED.Equals(status)
        || Environment::MEDIA_MOUNTED_READ_ONLY.Equals(status)) {
        UpdateProgressDialog(R::string::progress_unmounting);
        AutoPtr<IIMountService> mountService = GetMountService();
        String extStoragePath;
        if (mStorageVolume == NULL)
            Environment::GetLegacyExternalStorageDirectory()->ToString(&extStoragePath);
        else
            mStorageVolume->GetPath(&extStoragePath);

        // Remove encryption mapping if this is an unmount for a factory reset.
        if (FAILED(mountService->UnmountVolume(extStoragePath, TRUE, mFactoryReset))) {
            Logger::W(TAG, "Failed talking with mount service");
        }
    }
    else if (Environment::MEDIA_NOFS.Equals(status)
        || Environment::MEDIA_UNMOUNTED.Equals(status)
        || Environment::MEDIA_UNMOUNTABLE.Equals(status)) {
        UpdateProgressDialog(R::string::progress_erasing);
        AutoPtr<IIMountService> mountService = GetMountService();
        String extStoragePath;
        if (mStorageVolume == NULL)
            Environment::GetLegacyExternalStorageDirectory()->ToString(&extStoragePath);
        else
            mStorageVolume->GetPath(&extStoragePath);

        if (mountService != NULL) {
            AutoPtr<MyThread> thread = new MyThread(this ,extStoragePath);
            thread->constructor();
            thread->Start();
        }
        else {
            Logger::W(TAG, "Unable to locate IIMountService");
        }
    }
    else if (Environment::MEDIA_BAD_REMOVAL.Equals(status)) {
        Fail(R::string::media_bad_removal);
    }
    else if (Environment::MEDIA_CHECKING.Equals(status)) {
        Fail(R::string::media_checking);
    }
    else if (Environment::MEDIA_REMOVED.Equals(status)) {
        Fail(R::string::media_removed);
    }
    else if (Environment::MEDIA_SHARED.Equals(status)) {
        Fail(R::string::media_shared);
    }
    else {
        Fail(R::string::media_unknown_state);
        Logger::W(TAG, "Unknown storage state: %s", status.string());
        StopSelf();
    }
}

ECode CExternalStorageFormatter::UpdateProgressDialog(
    /* [in] */ Int32 msg)
{
    if (mProgressDialog == NULL) {
        CProgressDialog::New(this, (IProgressDialog**)&mProgressDialog);
        AutoPtr<IDialog> dialog = IDialog::Probe(mProgressDialog);
        mProgressDialog->SetIndeterminate(TRUE);
        dialog->SetCancelable(FALSE);
        AutoPtr<IWindow> window;
        dialog->GetWindow((IWindow**)&window);
        window->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
        dialog->Show();
    }

    AutoPtr<ICharSequence> text;
    GetText(msg, (ICharSequence**)&text);
    IAlertDialog::Probe(mProgressDialog)->SetMessage(text);
    return NOERROR;
}

AutoPtr<IIMountService> CExternalStorageFormatter::GetMountService()
{
    if (mMountService == NULL) {
        AutoPtr<IInterface> service = ServiceManager::GetService(String("mount"));
        if (service != NULL) {
            mMountService = IIMountService::Probe(service);
        }
        else {
            Logger::E(TAG, "Can't get mount service");
        }
    }

    return mMountService;
}

} // namespace Storage
} // namespace Os
} // namespace Internal
} // namepsace Droid
} // namespace Elastos


