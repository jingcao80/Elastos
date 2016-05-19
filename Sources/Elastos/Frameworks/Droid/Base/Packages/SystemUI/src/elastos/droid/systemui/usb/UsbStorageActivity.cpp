#include "elastos/droid/systemui/usb/UsbStorageActivity.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::Storage::EIID_IStorageEventListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::R;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::IO::IFile;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {

//---------------------------------------------------------------------------------------------
// UsbStorageActivity::MyUsbStateReceiver
//---------------------------------------------------------------------------------------------

UsbStorageActivity::MyUsbStateReceiver::MyUsbStateReceiver(
    /* [in] */ UsbStorageActivity* host)
    : mHost(host)
{}

ECode UsbStorageActivity::MyUsbStateReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IUsbManager::ACTION_USB_STATE)) {
        mHost->HandleUsbStateChanged(intent);
    }
    return NOERROR;
}

//---------------------------------------------------------------------------------------------
// UsbStorageActivity::MyStorageListener
//---------------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(UsbStorageActivity::MyStorageListener, Object, IStorageEventListener)

UsbStorageActivity::MyStorageListener::MyStorageListener(
    /* [in] */ UsbStorageActivity* host)
    : mHost(host)
{}

ECode UsbStorageActivity::MyStorageListener::OnStorageStateChanged(
    /* [in] */ const String& path,
    /* [in] */ const String& oldState,
    /* [in] */ const String& newState)
{
    Boolean on = newState.Equals(IEnvironment::MEDIA_SHARED);
    mHost->SwitchDisplay(on);
    return NOERROR;
}

ECode UsbStorageActivity::MyStorageListener::OnUsbMassStorageConnectionChanged(
    /* [in] */ Boolean connected)
{
    return NOERROR;
}

//---------------------------------------------------------------------------------------------
// UsbStorageActivity::R1
//---------------------------------------------------------------------------------------------

UsbStorageActivity::R1::R1(
    /* [in] */ Boolean usbStorageInUse,
    /* [in] */ UsbStorageActivity* host)
    : mHost(host)
    , mUsbStorageInUse(usbStorageInUse)
{}

ECode UsbStorageActivity::R1::Run()
{
    mHost->SwitchDisplayAsync(mUsbStorageInUse);
    return NOERROR;
}

//---------------------------------------------------------------------------------------------
// UsbStorageActivity::R2
//---------------------------------------------------------------------------------------------

UsbStorageActivity::R2::R2(
    /* [in] */ UsbStorageActivity* host)
    : mHost(host)
{}

ECode UsbStorageActivity::R2::Run()
{
    IView::Probe(mHost->mUnmountButton)->SetVisibility(IView::GONE);
    IView::Probe(mHost->mMountButton)->SetVisibility(IView::GONE);
    IView::Probe(mHost->mProgressBar)->SetVisibility(IView::VISIBLE);
    // will be hidden once USB mass storage kicks in (or fails)
    return NOERROR;
}

//---------------------------------------------------------------------------------------------
// UsbStorageActivity::R3
//---------------------------------------------------------------------------------------------

UsbStorageActivity::R3::R3(
    /* [in] */ Boolean on,
    /* [in] */ UsbStorageActivity* host)
    : mHost(host)
    , mOn(on)
{}

ECode UsbStorageActivity::R3::Run()
{
    if (mOn) {
        mHost->mStorageManager->EnableUsbMassStorage();
    }
    else {
        mHost->mStorageManager->DisableUsbMassStorage();
    }
    return NOERROR;
}

//---------------------------------------------------------------------------------------------
// UsbStorageActivity::R4
//---------------------------------------------------------------------------------------------

UsbStorageActivity::R4::R4(
    /* [in] */ UsbStorageActivity* host)
    : mHost(host)
{}

ECode UsbStorageActivity::R4::Run()
{
    mHost->CheckStorageUsersAsync();
    return NOERROR;
}

//---------------------------------------------------------------------------------------------
// UsbStorageActivity::R5
//---------------------------------------------------------------------------------------------

UsbStorageActivity::R5::R5(
    /* [in] */ Int32 id,
    /* [in] */ UsbStorageActivity* host)
    : mHost(host)
    , mId(id)
{}

ECode UsbStorageActivity::R5::Run()
{
    if (!mHost->mDestroyed) {
        mHost->RemoveDialog(mId);
        mHost->ShowDialog(mId);
    }
    return NOERROR;
}

//---------------------------------------------------------------------------------------------
// UsbStorageActivity::R6
//---------------------------------------------------------------------------------------------

UsbStorageActivity::R6::R6(
    /* [in] */ UsbStorageActivity* host)
    : mHost(host)
{}

ECode UsbStorageActivity::R6::Run()
{
    Boolean enabled;
    mHost->mStorageManager->IsUsbMassStorageEnabled(&enabled);
    mHost->SwitchDisplay(enabled);
    return NOERROR;
}

//---------------------------------------------------------------------------------------------
// UsbStorageActivity::MyDialogInterfaceOnClickListener
//---------------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(UsbStorageActivity::MyDialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener)

UsbStorageActivity::MyDialogInterfaceOnClickListener::MyDialogInterfaceOnClickListener(
    /* [in] */ UsbStorageActivity* host)
    : mHost(host)
{}

ECode UsbStorageActivity::MyDialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mHost->SwitchUsbMassStorage(TRUE);
    return NOERROR;
}

//---------------------------------------------------------------------------------------------
// UsbStorageActivity
//---------------------------------------------------------------------------------------------

const String UsbStorageActivity::TAG("UsbStorageActivity");
const Int32 UsbStorageActivity::DLG_CONFIRM_KILL_STORAGE_USERS;
const Int32 UsbStorageActivity::DLG_ERROR_SHARING;
const Boolean UsbStorageActivity::localLOGV = FALSE;

CAR_INTERFACE_IMPL_3(UsbStorageActivity, Activity, IViewOnClickListener, IDialogInterfaceOnCancelListener, IUsbStorageActivity)

UsbStorageActivity::UsbStorageActivity()
    : mDestroyed(FALSE)
{
    mUsbStateReceiver = new MyUsbStateReceiver(this);
    mStorageListener = new MyStorageListener(this);
}

ECode UsbStorageActivity::constructor()
{
    return Activity::constructor();
}

ECode UsbStorageActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);

    if (mStorageManager == NULL) {
        AutoPtr<IInterface> obj;
        GetSystemService(IContext::STORAGE_SERVICE, (IInterface**)&obj);
        mStorageManager = IStorageManager::Probe(obj);
        if (mStorageManager == NULL) {
            Logger::W(TAG, "Failed to get StorageManager");
        }
    }

    CHandler::New((IHandler**)&mUIHandler);
    AutoPtr<IHandlerThread> thr;
    CHandlerThread::New(String("SystemUI UsbStorageActivity"), (IHandlerThread**)&thr);
    IThread::Probe(thr)->Start();
    AutoPtr<ILooper> looper;
    thr->GetLooper((ILooper**)&looper);
    CHandler::New(looper, (IHandler**)&mAsyncStorageHandler);

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    window->AddFlags(IWindowManagerLayoutParams::FLAG_DISMISS_KEYGUARD);

    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    Boolean isRemovable;
    env->IsExternalStorageRemovable(&isRemovable);
    if (isRemovable) {
        window->AddFlags(IWindowManagerLayoutParams::FLAG_SHOW_WHEN_LOCKED);
    }

    SetContentView(R::layout::usb_storage_activity);

    AutoPtr<IView> view;
    FindViewById(R::id::icon, (IView**)&view);
    mIcon = IImageView::Probe(view);

    AutoPtr<IView> view2;
    FindViewById(R::id::banner, (IView**)&view2);
    mBanner = ITextView::Probe(view2);

    AutoPtr<IView> view3;
    FindViewById(R::id::message, (IView**)&view3);
    mMessage = ITextView::Probe(view3);

    AutoPtr<IView> view4;
    FindViewById(R::id::mount_button, (IView**)&view4);
    mMountButton = IButton::Probe(view4);
    IView::Probe(mMountButton)->SetOnClickListener(this);

    AutoPtr<IView> view5;
    FindViewById(R::id::unmount_button, (IView**)&view5);
    mUnmountButton = IButton::Probe(view5);
    IView::Probe(mUnmountButton)->SetOnClickListener(this);

    AutoPtr<IView> view6;
    FindViewById(R::id::progress, (IView**)&view6);
    mProgressBar = IProgressBar::Probe(view6);
    return NOERROR;
}

ECode UsbStorageActivity::OnDestroy()
{
    Activity::OnDestroy();
    mDestroyed = TRUE;
    return NOERROR;
}

void UsbStorageActivity::SwitchDisplay(
    /* [in] */ Boolean usbStorageInUse)
{
    AutoPtr<R1> r = new R1(usbStorageInUse, this);
    Boolean b;
    mUIHandler->Post(r, &b);
}

void UsbStorageActivity::SwitchDisplayAsync(
    /* [in] */ Boolean usbStorageInUse)
{
    if (usbStorageInUse) {
        IView::Probe(mProgressBar)->SetVisibility(IView::GONE);
        IView::Probe(mUnmountButton)->SetVisibility(IView::VISIBLE);
        IView::Probe(mMountButton)->SetVisibility(IView::GONE);
        mIcon->SetImageResource(R::drawable::usb_android_connected);
        AutoPtr<ICharSequence> cs;
        CString::New(StringUtils::ToString(R::string::usb_storage_stop_title), (ICharSequence**)&cs);
        mBanner->SetText(cs);
        AutoPtr<ICharSequence> cs2;
        CString::New(StringUtils::ToString(R::string::usb_storage_stop_message), (ICharSequence**)&cs2);
        mMessage->SetText(cs2);
    }
    else {
        IView::Probe(mProgressBar)->SetVisibility(IView::GONE);
        IView::Probe(mUnmountButton)->SetVisibility(IView::GONE);
        IView::Probe(mMountButton)->SetVisibility(IView::VISIBLE);
        mIcon->SetImageResource(R::drawable::usb_android);
        AutoPtr<ICharSequence> cs;
        CString::New(StringUtils::ToString(R::string::usb_storage_title), (ICharSequence**)&cs);
        mBanner->SetText(cs);
        AutoPtr<ICharSequence> cs2;
        CString::New(StringUtils::ToString(R::string::usb_storage_message), (ICharSequence**)&cs2);
        mMessage->SetText(cs2);
    }
}

ECode UsbStorageActivity::OnResume()
{
    Activity::OnResume();

    mStorageManager->RegisterListener(mStorageListener);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IUsbManager::ACTION_USB_STATE, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    RegisterReceiver(mUsbStateReceiver, filter, (IIntent**)&intent);
    AutoPtr<R6> r6 = new R6(this);
    Boolean b6;
    ECode ec = mAsyncStorageHandler->Post(r6, &b6);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to read UMS enable state%08x", ec);
        // return E_EXCEPTION;
    }
    return NOERROR;
}

ECode UsbStorageActivity::OnPause()
{
    Activity::OnPause();

    UnregisterReceiver(mUsbStateReceiver);
    if (mStorageManager == NULL && mStorageListener != NULL) {
        mStorageManager->UnregisterListener(mStorageListener);
    }
    return NOERROR;
}

void UsbStorageActivity::HandleUsbStateChanged(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IBundle> bundle;
    intent->GetExtras((IBundle**)&bundle);
    Boolean connected;
    bundle->GetBoolean(IUsbManager::USB_CONNECTED, &connected);
    if (!connected) {
        // It was disconnected from the plug, so finish
        Finish();
    }
}

AutoPtr<IIMountService> UsbStorageActivity::GetMountService()
{
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("mount"));
    AutoPtr<IBinder> service = IBinder::Probe(obj);
    if (service != NULL) {
        return IIMountService::Probe(service);
    }
    return NULL;
}

AutoPtr<IDialog> UsbStorageActivity::OnCreateDialog(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* args)
{
    switch (id) {
        case DLG_CONFIRM_KILL_STORAGE_USERS: {
            AutoPtr<IAlertDialogBuilder> adb;
            CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&adb);
            adb->SetTitle(R::string::dlg_confirm_kill_storage_users_title);
            AutoPtr<MyDialogInterfaceOnClickListener> diol =
                new MyDialogInterfaceOnClickListener(this);
            adb->SetPositiveButton(R::string::dlg_ok, diol);
            adb->SetNegativeButton(R::string::cancel, NULL);
            adb->SetMessage(R::string::dlg_confirm_kill_storage_users_text);
            adb->SetOnCancelListener(this);
            AutoPtr<IAlertDialog> ad;
            adb->Create((IAlertDialog**)&ad);
            return IDialog::Probe(adb);
        }
        case DLG_ERROR_SHARING: {
            AutoPtr<IAlertDialogBuilder> adb;
            CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&adb);
            adb->SetTitle(R::string::dlg_error_title);
            adb->SetNeutralButton(R::string::dlg_ok, NULL);
            adb->SetMessage(R::string::usb_storage_error_message);
            adb->SetOnCancelListener(this);
            AutoPtr<IAlertDialog> ad;
            adb->Create((IAlertDialog**)&ad);
            return IDialog::Probe(adb);
        }
    }
    return NULL;
}

void UsbStorageActivity::ScheduleShowDialog(
    /* [in] */ const Int32 id)
{
    AutoPtr<R5> r5 = new R5(id, this);
    Boolean b5;
    mUIHandler->Post(r5, &b5);
}

void UsbStorageActivity::SwitchUsbMassStorage(
    /* [in] */ const Boolean on)
{
    // things to do on the UI thread
    AutoPtr<R2> r2 = new R2(this);
    Boolean b2;
    mUIHandler->Post(r2, &b2);

    // things to do elsewhere

    AutoPtr<R3> r3 = new R3(on, this);
    Boolean b3;
    mAsyncStorageHandler->Post(r3, &b3);
}

void UsbStorageActivity::CheckStorageUsers()
{
    AutoPtr<R4> r4 = new R4(this);
    Boolean b4;
    mAsyncStorageHandler->Post(r4, &b4);
}

void UsbStorageActivity::CheckStorageUsersAsync()
{
    AutoPtr<IIMountService> ims = GetMountService();
    if (ims == NULL) {
        // Display error dialog
        ScheduleShowDialog(DLG_ERROR_SHARING);
    }
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dir;
    env->GetExternalStorageDirectory((IFile**)&dir);
    String extStoragePath;
    dir->ToString(&extStoragePath);
    Boolean showDialog = FALSE;

    AutoPtr<ArrayOf<Int32> > stUsers;
    ims->GetStorageUsers(extStoragePath, (ArrayOf<Int32>**)&stUsers);
    if (stUsers != NULL && stUsers->GetLength() > 0) {
        showDialog = TRUE;
    }
    else {
        // List of applications on sdcard.
        AutoPtr<IInterface> obj;
        ECode ec = GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&obj);
        if (FAILED(ec)) {
            ScheduleShowDialog(DLG_ERROR_SHARING);
            // return E_REMOTE_EXCEPTION;
        }
        AutoPtr<IActivityManager> am = IActivityManager::Probe(obj);
        AutoPtr<IList> infoList;
        am->GetRunningExternalApplications((IList**)&infoList);
        Int32 size;
        infoList->GetSize(&size);
        if (infoList != NULL && size > 0) {
            showDialog = TRUE;
        }
    }

    if (showDialog) {
        // Display dialog to user
        ScheduleShowDialog(DLG_CONFIRM_KILL_STORAGE_USERS);
    }
    else {
        if (localLOGV) Logger::I(TAG, "Enabling UMS");
        SwitchUsbMassStorage(TRUE);
    }
}

ECode UsbStorageActivity::OnClick(
    /* [in] */ IView* v)
{
    if (v == IView::Probe(mMountButton)) {
       // Check for list of storage users and display dialog if needed.
        CheckStorageUsers();
    }
    else if (v == IView::Probe(mUnmountButton)) {
        if (localLOGV) Logger::I(TAG, "Disabling UMS");
        SwitchUsbMassStorage(FALSE);
    }
    return NOERROR;
}

ECode UsbStorageActivity::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    return Finish();
}

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos