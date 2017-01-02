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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/internal/os/storage/CExternalStorageFormatter.h"
#include "elastos/droid/app/CProgressDialog.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/storage/CStorageManagerHelper.h"
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
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::Storage::IStorageManagerHelper;
using Elastos::Droid::Os::Storage::CStorageManagerHelper;
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
    /* [in] */ IIMountService* mountService)
    : mOwner(owner)
    , mMountService(mountService)
{
}

ECode CExternalStorageFormatter::MyThread::Run()
{
    Boolean success = FALSE;
    AutoPtr<IStorageVolume> physicalVol;
    AutoPtr<IArrayList> physicalVols;
    String extStoragePath(NULL);
    // try {
    AutoPtr<ArrayOf<IStorageVolume*> > volumes;
    mMountService->GetVolumeList((ArrayOf<IStorageVolume*>**)&volumes);
    AutoPtr<IStorageManagerHelper> hlp;
    CStorageManagerHelper::AcquireSingleton((IStorageManagerHelper**)&hlp);
    hlp->GetPhysicalExternalVolume(volumes, (IArrayList**)&physicalVols);
    // find external storage path if storage volume not specified
    if (mOwner->mStorageVolume == NULL) {
        Int32 size = 0;
        physicalVols->GetSize(&size);
        if (size == 0) {
            mOwner->UpdateProgressDialog(R::string::progress_nomediapresent);
            return NOERROR;
        }
        else {
            AutoPtr<IInterface> p;
            physicalVols->Get(0, (IInterface**)&p);
            physicalVol = IStorageVolume::Probe(p);
            physicalVol->GetPath(&extStoragePath);
        }
    }
    //else use the specified storage volume
    else {
        mOwner->mStorageVolume->GetPath(&extStoragePath);
    }
    Int32 res = 0;
    mMountService->FormatVolume(extStoragePath, &res);
    mOwner->mIsFormatSuccess = TRUE;
    success = TRUE;
    // } catch (Exception e) {
    //     Looper.prepare();
    //     Toast.makeText(ExternalStorageFormatter.this,
    //             toastTextResId, Toast.LENGTH_LONG).show();
    // }
    if (success) {
        if (mOwner->mFactoryReset) {
            AutoPtr<IIntent> intent;
            CIntent::New(IIntent::ACTION_MASTER_CLEAR, (IIntent**)&intent);
            intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
            intent->PutExtra(IIntent::EXTRA_REASON, mOwner->mReason);
            intent->PutExtra(IExternalStorageFormatter::EXTRA_WIPE_MEDIA, mOwner->mWipeInternalStorage);
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
        intent->PutExtra(IExternalStorageFormatter::EXTRA_WIPE_MEDIA, mOwner->mWipeInternalStorage);
        mOwner->SendBroadcast(intent);
    }
    else {
        // try {
        Int32 size = 0;
        physicalVols->GetSize(&size);
        if(size == 0) {
            mOwner->UpdateProgressDialog(R::string::progress_nomediapresent);
            return NOERROR;
        }
        else {
            AutoPtr<IInterface> p;
            physicalVols->Get(0, (IInterface**)&p);
            physicalVol = IStorageVolume::Probe(p);
            if (mOwner->mStorageVolume == NULL) {
                physicalVol->GetPath(&extStoragePath);
            }
            else {
                mOwner->mStorageVolume->GetPath(&extStoragePath);
            }
            Int32 res = 0;
            mMountService->MountVolume(extStoragePath, &res);
        }
        // } catch (RemoteException e) {
        //     Log.w(TAG, "Failed talking with mount service", e);
        // }
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
    , mWipeInternalStorage(FALSE)
    , mIsFormatSuccess(FALSE)
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
    intent->GetBooleanExtra(IExternalStorageFormatter::EXTRA_WIPE_MEDIA, FALSE, &mWipeInternalStorage);

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

    AutoPtr<ArrayOf<IStorageVolume*> > volumes;
    mountService->GetVolumeList((ArrayOf<IStorageVolume*>**)&volumes);
    AutoPtr<IStorageManagerHelper> hlp;
    CStorageManagerHelper::AcquireSingleton((IStorageManagerHelper**)&hlp);
    AutoPtr<IArrayList> physicalVols;
    hlp->GetPhysicalExternalVolume(volumes, (IArrayList**)&physicalVols);
    String extStoragePath(NULL);
    // find external storage path if storage volume not specified
    if (mStorageVolume == NULL) {
        Int32 size = 0;
        physicalVols->GetSize(&size);
        if (size == 0) {
            UpdateProgressDialog(R::string::progress_nomediapresent);
        }
        else {
            AutoPtr<IInterface> p;
            physicalVols->Get(0, (IInterface**)&p);
            AutoPtr<IStorageVolume> physicalVol = IStorageVolume::Probe(p);
            IObject::Probe(physicalVol)->ToString(&extStoragePath);
            Int32 res;
            mountService->MountVolume(extStoragePath, &res);
        }
    }
    //else use the specified storage volume
    else {
        mStorageVolume->GetPath(&extStoragePath);
        Int32 res;
        mountService->MountVolume(extStoragePath, &res);
    }

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
    String status(NULL);
    String extStoragePath(NULL);
    AutoPtr<IStorageVolume> physicalVol;
    Boolean isUsbStorage = FALSE;

    AutoPtr<IIMountService> mountService = GetMountService();
    AutoPtr<ArrayOf<IStorageVolume*> > volumes;
    mountService->GetVolumeList((ArrayOf<IStorageVolume*>**)&volumes);
    AutoPtr<IStorageManagerHelper> hlp;
    CStorageManagerHelper::AcquireSingleton((IStorageManagerHelper**)&hlp);
    AutoPtr<IArrayList> physicalVols;
    hlp->GetPhysicalExternalVolume(volumes, (IArrayList**)&physicalVols);
    // find external storage path if storage volume not specified
    if (mStorageVolume == NULL) {
        Int32 size = 0;
        physicalVols->GetSize(&size);
        if (size == 0) {
            UpdateProgressDialog(R::string::progress_nomediapresent);
            return;
        }
        else {
            AutoPtr<IInterface> p;
            physicalVols->Get(0, (IInterface**)&p);
            physicalVol = IStorageVolume::Probe(p);
            String path;
            physicalVol->GetPath(&path);
            mStorageManager->GetVolumeState(path, &status);
            Int32 id = 0;
            physicalVol->GetDescriptionId(&id);
            isUsbStorage = id == R::string::storage_usb;
        }
    }
    //else use the specified storage volume
    else {
        String path;
        mStorageVolume->GetPath(&path);
        mStorageManager->GetVolumeState(path, &status);
        Int32 id = 0;
        mStorageVolume->GetDescriptionId(&id);
        isUsbStorage = id == R::string::storage_usb;
    }

    if (IEnvironment::MEDIA_MOUNTED.Equals(status)
            || IEnvironment::MEDIA_MOUNTED_READ_ONLY.Equals(status)) {
        UpdateProgressDialog(isUsbStorage ? R::string::usb_progress_unmounting :
                                            R::string::sd_progress_unmounting);

        if (mIsFormatSuccess) return;

        AutoPtr<IIMountService> mountService = GetMountService();
        AutoPtr<ArrayOf<IStorageVolume*> > volumes;
        mountService->GetVolumeList((ArrayOf<IStorageVolume*>**)&volumes);
        AutoPtr<IArrayList> physicalVols;
        hlp->GetPhysicalExternalVolume(volumes, (IArrayList**)&physicalVols);
        // find external storage path if storage volume not specified
        if (mStorageVolume == NULL) {
            Int32 size = 0;
            physicalVols->GetSize(&size);
            if (size == 0) {
                UpdateProgressDialog(R::string::progress_nomediapresent);
                return;
            }
            else {
                AutoPtr<IInterface> p;
                physicalVols->Get(0, (IInterface**)&p);
                physicalVol = IStorageVolume::Probe(p);
                physicalVol->GetPath(&extStoragePath);
                String str;
                IObject::Probe(physicalVol)->ToString(&str);
                Logger::E(TAG, "physicalVol : %s", (const char*)str);
                mountService->UnmountVolume(extStoragePath, TRUE, mFactoryReset);
            }
        }
        //else use the specified storage volume
        else {
            mStorageVolume->GetPath(&extStoragePath);
            mountService->UnmountVolume(extStoragePath, TRUE, mFactoryReset);
        }
    }
    else if (IEnvironment::MEDIA_NOFS.Equals(status)
            || IEnvironment::MEDIA_UNMOUNTED.Equals(status)
            || IEnvironment::MEDIA_UNMOUNTABLE.Equals(status)) {
        UpdateProgressDialog(isUsbStorage ? R::string::usb_progress_erasing :
                                            R::string::sd_progress_erasing);
        Int32 toastTextResId = isUsbStorage ? R::string::usb_format_error :
                                                  R::string::sd_format_error;
        AutoPtr<IIMountService> mountService = GetMountService();
        if (mountService != NULL) {
            AutoPtr<MyThread> thread = new MyThread(this, mountService);
            thread->constructor();
            thread->Start();
        }
        else {
            Logger::W(TAG, "Unable to locate IMountService");
        }
    }
    else if (IEnvironment::MEDIA_BAD_REMOVAL.Equals(status)) {
        Fail(isUsbStorage ? R::string::usb_media_bad_removal : R::string::sd_media_bad_removal);
    }
    else if (IEnvironment::MEDIA_CHECKING.Equals(status)) {
        Fail(isUsbStorage ? R::string::usb_media_checking : R::string::sd_media_checking);
    }
    else if (IEnvironment::MEDIA_REMOVED.Equals(status)) {
        Fail(isUsbStorage ? R::string::usb_media_removed : R::string::sd_media_removed);
    }
    else if (IEnvironment::MEDIA_SHARED.Equals(status)) {
        Fail(isUsbStorage ? R::string::usb_media_shared : R::string::sd_media_shared);
    }
    else {
        Fail(R::string::media_unknown_state);
        Logger::W(TAG, "Unknown storage state: %s", (const char*)status);
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


