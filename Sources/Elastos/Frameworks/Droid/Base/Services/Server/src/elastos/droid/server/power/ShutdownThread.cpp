
#include <Elastos.Droid.Core.h>
#include <Elastos.Droid.Nfc.h>
#include "elastos/droid/server/power/ShutdownThread.h"
#include "elastos/droid/server/power/CMountShutdownObserver.h"
#include "elastos/droid/server/power/PowerManagerService.h"
// #include "elastos/droid/server/pm/CPackageManagerService.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::App::CProgressDialog;
// using Elastos::Droid::Bluetooth::IIBluetoothManager;
// using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::Pm::IIPackageManager;;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IITelephony;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Nfc::IINfcAdapter;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Os::CSystemVibrator;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Storage::EIID_IIMountShutdownObserver;
using Elastos::Droid::Os::Storage::IIMountService;
// using Elastos::Droid::Server::Pm::CPackageManagerService;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Core::ICharSequence;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Power {

const String ShutdownThread::SHUTDOWN_ACTION_PROPERTY("sys.shutdown.requested");
const String ShutdownThread::REBOOT_SAFEMODE_PROPERTY("persist.sys.safemode");
const String ShutdownThread::TAG("ShutdownThread");
const Int32 ShutdownThread::PHONE_STATE_POLL_SLEEP_MSEC = 500;
const Int32 ShutdownThread::MAX_BROADCAST_TIME = 10*1000;
const Int32 ShutdownThread::MAX_SHUTDOWN_WAIT_TIME = 20*1000;
const Int32 ShutdownThread::MAX_RADIO_WAIT_TIME = 12*1000;
const Int32 ShutdownThread::SHUTDOWN_VIBRATE_MS = 500;

Object ShutdownThread::sIsStartedGuard;
Boolean ShutdownThread::sIsStarted = FALSE;
Boolean ShutdownThread::mReboot = FALSE;
Boolean ShutdownThread::mRebootSafeMode = FALSE;
String ShutdownThread::mRebootReason(NULL);

AutoPtr<ShutdownThread> ShutdownThread::sInstance = new ShutdownThread();

AutoPtr<IAudioAttributes> ShutdownThread::InitVIBRATION_ATTRIBUTES()
{
    AutoPtr<IAudioAttributesBuilder> builder;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&builder);
    builder->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
    builder->SetUsage(IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION);
    AutoPtr<IAudioAttributes> attr;
    builder->Build((IAudioAttributes**)&attr);
    return attr;
}

AutoPtr<IAudioAttributes> ShutdownThread::VIBRATION_ATTRIBUTES = InitVIBRATION_ATTRIBUTES();

AutoPtr<IAlertDialog> ShutdownThread::sConfirmDialog;

//==============================================================================
//          ShutdownThread::MountShutdownObserver
//==============================================================================

CAR_INTERFACE_IMPL_2(ShutdownThread::MountShutdownObserver, Object, IIMountShutdownObserver, IBinder);

ShutdownThread::MountShutdownObserver::MountShutdownObserver()
{}

ShutdownThread::MountShutdownObserver::~MountShutdownObserver()
{}

ECode ShutdownThread::MountShutdownObserver::constructor(
    /* [in] */ IThread* host)
{
    mHost = (ShutdownThread*)host;
    return NOERROR;
}

ECode ShutdownThread::MountShutdownObserver::OnShutDownComplete(
    /* [in] */ Int32 statusCode)
{
    Logger::W(TAG, "Result code %d from MountService.shutdown", statusCode);
    mHost->ActionDone();
    return NOERROR;
}

//==============================================================================
//          ShutdownThread::CloseDialogReceiver
//==============================================================================

CAR_INTERFACE_IMPL(ShutdownThread::CloseDialogReceiver, BroadcastReceiver, IDialogInterfaceOnDismissListener);

ShutdownThread::CloseDialogReceiver::CloseDialogReceiver(
    /* [in] */IContext* context)
    : mContext(context)
{
    AutoPtr<IIntentFilter> filter;
    ASSERT_SUCCEEDED(CIntentFilter::New(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS, (IIntentFilter**)&filter));
    AutoPtr<IIntent> intent;
    ASSERT_SUCCEEDED(context->RegisterReceiver((IBroadcastReceiver*)this,
            filter, (IIntent**)&intent));
}

ECode ShutdownThread::CloseDialogReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return mDialog->Cancel();
}

ECode ShutdownThread::CloseDialogReceiver::OnDismiss(
    /* [in] */ IDialogInterface* unused)
{
    return mContext->UnregisterReceiver((IBroadcastReceiver*)this);
}

//==============================================================================
//          ShutdownThread::DialogInterfaceOnClickListener
//==============================================================================

CAR_INTERFACE_IMPL(ShutdownThread::DialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener);

ShutdownThread::DialogInterfaceOnClickListener::DialogInterfaceOnClickListener(
    /* [in] */ IContext* context)
    : mContext(context)
{}

ECode ShutdownThread::DialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    ShutdownThread::BeginShutdownSequence(mContext);
    return NOERROR;
}

//==============================================================================
//          ShutdownThread::ActionDoneBroadcastReceiver
//==============================================================================

ShutdownThread::ActionDoneBroadcastReceiver::ActionDoneBroadcastReceiver(
    /* [in] */ ShutdownThread* host)
    : mHost(host)
{}

ECode ShutdownThread::ActionDoneBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // We don't allow apps to cancel this, so ignore the result.
    mHost->ActionDone();
    return NOERROR;
}

//==============================================================================
//          ShutdownThread::ShutdownRadiosThread
//==============================================================================

ShutdownThread::ShutdownRadiosThread::ShutdownRadiosThread(
    /* [in] */ ArrayOf<Boolean>* done,
    /* [in] */ Int64 endTime)
    : mEndTime(endTime)
{
    Thread::constructor();
    mDone = done->Clone();
}

ECode ShutdownThread::ShutdownRadiosThread::Run()
{
    Boolean nfcOff;
    Boolean bluetoothOff;
    Boolean radioOff;

    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    AutoPtr<IInterface> obj;
    serviceManager->CheckService(String("nfc"), (IInterface**)&obj);
    AutoPtr<IINfcAdapter> nfc = IINfcAdapter::Probe(obj);

    obj = NULL;
    serviceManager->CheckService(String("phone"), (IInterface**)&obj);
    AutoPtr<IITelephony> phone = IITelephony::Probe(obj);

    // obj = NULL;
    // serviceManager->CheckService(IBluetoothAdapter::BLUETOOTH_MANAGER_SERVICE, (IInterface**)&obj);
    // AutoPtr<IIBluetoothManager> bluetooth = IIBluetoothManager::Probe(obj);

    Boolean res;

    // try {
    Int32 state;
    ECode ec = nfc->GetState(&state);
    if (FAILED(ec)) {
        Logger::E(TAG, "RemoteException during NFC shutdown");
        nfcOff = TRUE;
    }
    else {
        assert(0 && "TODO");
        // nfcOff = nfc == NULL || state == INfcAdapter::STATE_OFF;
        if (!nfcOff) {
            Logger::W(TAG, "Turning off NFC...");
            if (FAILED(nfc->Disable(FALSE, &res))) { // Don't persist new state
                Logger::E(TAG, "RemoteException during NFC shutdown");
                nfcOff = TRUE;
            }
        }
    }
    // } catch (RemoteException ex) {
    //     Log.e(TAG, "RemoteException during NFC shutdown", ex);
    //     nfcOff = true;
    // }

    Boolean isEnabled;
    // // try {
    // ec = bluetooth->IsEnabled(&isEnabled);
    // if (FAILED(ec)) {
    //     Logger::E(TAG, "RemoteException during bluetooth shutdown");
    //     bluetoothOff = TRUE;
    // }
    // else {
    //     bluetoothOff = bluetooth == NULL || !isEnabled;
    //     if (!bluetoothOff) {
    //         Logger::W(TAG, "Disabling Bluetooth...");
    //         if (FAILED(bluetooth->Disable(FALSE, &res))) {
    //             Logger::E(TAG, "RemoteException during bluetooth shutdown");
    //             bluetoothOff = TRUE;
    //         }  // disable but don't persist new state
    //     }
    // }
    // // } catch (RemoteException ex) {
    // //     Log.e(TAG, "RemoteException during bluetooth shutdown", ex);
    // //     bluetoothOff = true;
    // // }

    // try {
    ec = phone->NeedMobileRadioShutdown(&isEnabled);
    if (FAILED(ec)) {
        Logger::E(TAG, "RemoteException during radio shutdown");
        radioOff = TRUE;
    }
    else {
        radioOff = phone == NULL || !isEnabled;
        if (!radioOff) {
            Logger::W(TAG, "Turning off cellular radios...");
            if (FAILED(phone->ShutdownMobileRadios())) {
                Logger::E(TAG, "RemoteException during radio shutdown");
                radioOff = TRUE;
            }
        }
    }
    // } catch (RemoteException ex) {
    //     Log.e(TAG, "RemoteException during radio shutdown", ex);
    //     radioOff = true;
    // }

    Logger::I(TAG, "Waiting for NFC, Bluetooth and Radio...");

    while (SystemClock::GetElapsedRealtime() < mEndTime) {
        if (!bluetoothOff) {
            assert(0 && "TODO");
            // // try {
            // if (FAILED(bluetooth->IsEnabled(&res))) {
            //     Logger::E(TAG, "RemoteException during bluetooth shutdown");
            //     bluetoothOff = TRUE;
            // }
            // bluetoothOff = !res;
            // // } catch (RemoteException ex) {
            // //     Log.e(TAG, "RemoteException during bluetooth shutdown", ex);
            // //     bluetoothOff = true;
            // // }
            // if (bluetoothOff) {
            //     Logger::I(TAG, "Bluetooth turned off.");
            // }
        }
        if (!radioOff) {
            // try {
            if (FAILED(phone->NeedMobileRadioShutdown(&res))) {
                Logger::E(TAG, "RemoteException during radio shutdown");
                radioOff = TRUE;
            }
            radioOff = !res;
            // } catch (RemoteException ex) {
            //     Log.e(TAG, "RemoteException during radio shutdown", ex);
            //     radioOff = true;
            // }
            if (radioOff) {
                Logger::I(TAG, "Radio turned off.");
            }
        }
        if (!nfcOff) {
            // try {
            if (FAILED(nfc->GetState(&state))) {
                Logger::E(TAG, "RemoteException during NFC shutdown");
                nfcOff = TRUE;
            }
            assert(0 && "TODO");
            // nfcOff = state == INfcAdapter::STATE_OFF;

            // } catch (RemoteException ex) {
            //     Log.e(TAG, "RemoteException during NFC shutdown", ex);
            //     nfcOff = true;
            // }
            if (nfcOff) {
                Logger::I(TAG, "NFC turned off.");
            }
        }

        if (radioOff && bluetoothOff && nfcOff) {
            Logger::I(TAG, "NFC, Radio and Bluetooth shutdown complete.");
            (*mDone)[0] = TRUE;
            break;
        }
        SystemClock::Sleep(PHONE_STATE_POLL_SLEEP_MSEC);
    }
    return NOERROR;
}

//==============================================================================
//          ShutdownThread
//==============================================================================

ShutdownThread::ShutdownThread()
    : mActionDone(FALSE)
{
    Thread::constructor();
}

ShutdownThread::~ShutdownThread()
{
}

void ShutdownThread::Shutdown(
    /* [in] */ IContext* context,
    /* [in] */ Boolean confirm)
{
    mReboot = FALSE;
    mRebootSafeMode = FALSE;
    ShutdownInner(context, confirm);
}

void ShutdownThread::ShutdownInner(
    /* [in] */ IContext* context,
    /* [in] */ Boolean confirm)
{
    // ensure that only one thread is trying to power down.
    // any additional calls are just returned
    synchronized(sIsStartedGuard) {
        if (sIsStarted) {
            Logger::D(TAG, "Request to shutdown already running, returning.");
            return;
        }
    }

    AutoPtr<IResources> res;
    ASSERT_SUCCEEDED(context->GetResources((IResources**)&res));
    Int32 longPressBehavior;
    res->GetInteger(R::integer::config_longPressOnPowerBehavior, &longPressBehavior);
    Int32 resourceId = mRebootSafeMode
            ? R::string::reboot_safemode_confirm
            : (longPressBehavior == 2
                    ? R::string::shutdown_confirm_question
                    : R::string::shutdown_confirm);

    Logger::D(TAG, "Notifying thread to start shutdown longPressBehavior=%d", longPressBehavior);

    if (confirm) {
        AutoPtr<CloseDialogReceiver> closer = new CloseDialogReceiver(context);
        if (sConfirmDialog != NULL) {
            IDialogInterface::Probe(sConfirmDialog)->Dismiss();
        }
        AutoPtr<IAlertDialogBuilder> dialogBuilder;
        ASSERT_SUCCEEDED(CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&dialogBuilder));
        dialogBuilder->SetTitle(mRebootSafeMode
                ? R::string::reboot_safemode_title
                : R::string::power_off);
        dialogBuilder->SetMessage(resourceId);
        AutoPtr<IDialogInterfaceOnClickListener> listener = new DialogInterfaceOnClickListener(context);
        dialogBuilder->SetPositiveButton(R::string::yes, listener);
        dialogBuilder->SetNegativeButton(R::string::no, NULL);
        ASSERT_SUCCEEDED(dialogBuilder->Create((IAlertDialog**)&sConfirmDialog));

        closer->mDialog = IDialog::Probe(sConfirmDialog);
        closer->mDialog->SetOnDismissListener(closer);
        AutoPtr<IWindow> win;
        ASSERT_SUCCEEDED(closer->mDialog->GetWindow((IWindow**)&win));
        win->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
        ASSERT_SUCCEEDED(closer->mDialog->Show());
    }
    else {
        BeginShutdownSequence(context);
    }
}

void ShutdownThread::Reboot(
    /* [in] */ IContext* context,
    /* [in] */ const String& reason,
    /* [in] */ Boolean confirm)
{
    mReboot = TRUE;
    mRebootSafeMode = FALSE;
    mRebootReason = reason;
    ShutdownInner(context, confirm);
}

void ShutdownThread::RebootSafeMode(
    /* [in] */ IContext* context,
    /* [in] */ Boolean confirm)
{
    mReboot = TRUE;
    mRebootSafeMode = TRUE;
    mRebootReason = NULL;
    ShutdownInner(context, confirm);
}

void ShutdownThread::BeginShutdownSequence(
    /* [in] */ IContext* context)
{
    synchronized(sIsStartedGuard) {
        if (sIsStarted) {
            Logger::D(TAG, "Shutdown sequence already running, returning.");
            return;
        }
        sIsStarted = TRUE;
    }

    // throw up an indeterminate system dialog to indicate radio is
    // shutting down.
    AutoPtr<IProgressDialog> pd;
    CProgressDialog::New(context, (IProgressDialog**)&pd);
    AutoPtr<ICharSequence> csq;
    context->GetText(R::string::power_off, (ICharSequence**)&csq);
    IDialog::Probe(pd)->SetTitle(csq);
    csq = NULL;
    context->GetText(R::string::shutdown_progress, (ICharSequence**)&csq);
    IAlertDialog::Probe(pd)->SetMessage(csq);
    pd->SetIndeterminate(TRUE);
    IDialog::Probe(pd)->SetCancelable(FALSE);
    AutoPtr<IWindow> win;
    IDialog::Probe(pd)->GetWindow((IWindow**)&win);
    win->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
    IDialog::Probe(pd)->Show();

    sInstance->mContext = context;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    sInstance->mPowerManager = IPowerManager::Probe(obj);

    // make sure we never fall asleep again
    sInstance->mCpuWakeLock = NULL;
    // try {
    ECode ec = sInstance->mPowerManager->NewWakeLock(
            IPowerManager::PARTIAL_WAKE_LOCK, TAG + String("-cpu"),
            (IPowerManagerWakeLock**)&sInstance->mCpuWakeLock);
    if (ec == (ECode)E_SECURITY_EXCEPTION) {
        Logger::W(TAG, "No permission to acquire wake lock 0x%08x", ec);
        sInstance->mCpuWakeLock = NULL;
    }
    if (sInstance->mCpuWakeLock != NULL) {
        sInstance->mCpuWakeLock->SetReferenceCounted(FALSE);
        sInstance->mCpuWakeLock->AcquireLock();
    }
    // } catch (SecurityException e) {
    //     Log.w(TAG, "No permission to acquire wake lock", e);
    //     sInstance.mCpuWakeLock = null;
    // }

    // also make sure the screen stays on for better user experience
    sInstance->mScreenWakeLock = NULL;
    Boolean isScreenOn;
    if (sInstance->mPowerManager->IsScreenOn(&isScreenOn), isScreenOn) {
        // try {
        ec = sInstance->mPowerManager->NewWakeLock(
                IPowerManager::FULL_WAKE_LOCK, TAG + String("-screen"),
                (IPowerManagerWakeLock**)&sInstance->mScreenWakeLock);
        if (ec == (ECode)E_SECURITY_EXCEPTION) {
            Logger::W(TAG, "No permission to acquire wake lock %08x0x", ec);
            sInstance->mScreenWakeLock = NULL;
        }
        if (sInstance->mScreenWakeLock != NULL) {
            sInstance->mScreenWakeLock->SetReferenceCounted(FALSE);
            sInstance->mScreenWakeLock->AcquireLock();
        }
        // } catch (SecurityException e) {
        //     Log.w(TAG, "No permission to acquire wake lock", e);
        //     sInstance.mScreenWakeLock = null;
        // }
    }

    // start the thread that initiates shutdown
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    sInstance->mHandler = handler;
    sInstance->Start();
}

void ShutdownThread::ActionDone()
{
    synchronized (mActionDoneSync) {
        mActionDone = TRUE;
        mActionDoneSync.NotifyAll();
    }
}

ECode ShutdownThread::Run()
{
    AutoPtr<IBroadcastReceiver> br = (IBroadcastReceiver*)new ActionDoneBroadcastReceiver(this);
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);

    /*
     * Write a system property in case the system_server reboots before we
     * get to the actual hardware restart. If that happens, we'll retry at
     * the beginning of the SystemServer startup.
     */
    {
        String reason = (mReboot ? String("1") : String("0"))
                + (mRebootReason != NULL ? mRebootReason : String(""));
        sysProp->Set(SHUTDOWN_ACTION_PROPERTY, reason);
    }

    /*
     * If we are rebooting into safe mode, write a system property
     * indicating so.
     */
    if (mRebootSafeMode) {
        sysProp->Set(REBOOT_SAFEMODE_PROPERTY, String("1"));
    }

    Logger::I(TAG, "Sending shutdown broadcast...");

    // First send the high-level shut down broadcast.
    mActionDone = FALSE;
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_SHUTDOWN, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
    AutoPtr<IUserHandleHelper> handleHelper;
    ASSERT_SUCCEEDED(CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper));
    AutoPtr<IUserHandle> all;
    handleHelper->GetALL((IUserHandle**)&all);
    mContext->SendOrderedBroadcastAsUser(intent, all, String(NULL), br,
            mHandler, 0, String(NULL), NULL);

    Int64 endTime = SystemClock::GetElapsedRealtime() + MAX_BROADCAST_TIME;
    synchronized(mActionDoneSync) {
        while (!mActionDone) {
            Int64 delay = endTime - SystemClock::GetElapsedRealtime();
            if (delay <= 0) {
                Logger::W(TAG, "Shutdown broadcast timed out");
                break;
            }
            // try {
            mActionDoneSync.Wait(delay);
            // } catch (InterruptedException e) {
            // }
        }
    }

    Logger::I(TAG, "Shutting down activity manager...");

    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    AutoPtr<IInterface> obj;

    serviceManager->CheckService(String("activity"), (IInterface**)&obj);
    AutoPtr<IIActivityManager> am = IIActivityManager::Probe(obj);
    if (am != NULL) {
        // try {
        Boolean result;
        am->Shutdown(MAX_BROADCAST_TIME, &result);
        // } catch (RemoteException e) {
        // }
    }

    Logger::I(TAG, "Shutting down package manager...");

    obj = NULL;
    serviceManager->GetService(String("package"), (IInterface**)&obj);
    assert(0 && "TODO");
    // AutoPtr<CPackageManagerService> pm = (CPackageManagerService*)IIPackageManager::Probe(obj);
    // if (pm != NULL) {
    //     pm->Shutdown();
    // }

    // Shutdown radios.
    ShutdownRadios(MAX_RADIO_WAIT_TIME);

    // Shutdown MountService to ensure media is in a safe state
    AutoPtr<IIMountShutdownObserver> observer;
    CMountShutdownObserver::New((IThread*)this, (IIMountShutdownObserver**)&observer);

    Logger::I(TAG, "Shutting down MountService");

    // Set initial variables and time out time.
    mActionDone = FALSE;
    Int64 endShutTime = SystemClock::GetElapsedRealtime() + MAX_SHUTDOWN_WAIT_TIME;
    synchronized(mActionDoneSync) {
        // try {
        obj = NULL;
        serviceManager->CheckService(String("mount"), (IInterface**)&obj);
        AutoPtr<IIMountService> mount = IIMountService::Probe(obj);
        if (mount != NULL) {
            if (FAILED(mount->Shutdown(observer))) {
                Logger::E(TAG, "Exception during MountService shutdown");
            }
        }
        else {
            Logger::W(TAG, "MountService unavailable for shutdown");
        }
        // } catch (Exception e) {
        //     Log.e(TAG, "Exception during MountService shutdown", e);
        // }
        while (!mActionDone) {
            Int64 delay = endShutTime - SystemClock::GetElapsedRealtime();
            if (delay <= 0) {
                Logger::W(TAG, "Shutdown wait timed out");
                break;
            }
            // try {
            mActionDoneSync.Wait(delay);
            // } catch (InterruptedException e) {
            // }
        }
    }

    RebootOrShutdown(mReboot, mRebootReason);

    return NOERROR;
}

void ShutdownThread::ShutdownRadios(
    /* [in] */ Int32 timeout)
{
    // If a radio is wedged, disabling it may hang so we do this work in another thread,
    // just in case.
    Int64 endTime = SystemClock::GetElapsedRealtime() + timeout;
    AutoPtr< ArrayOf<Boolean> > done = ArrayOf<Boolean>::Alloc(1);
    AutoPtr<IThread> t = new ShutdownRadiosThread(done, endTime);

    t->Start();
    // try {
    t->Join(timeout);
    // } catch (InterruptedException ex) {
    // }
    if (!(*done)[0]) {
        Logger::W(TAG, "Timed out waiting for NFC, Radio and Bluetooth shutdown.");
    }
}

void ShutdownThread::RebootOrShutdown(
    /* [in] */ Boolean reboot,
    /* [in] */ const String& reason)
{
    if (reboot) {
        Logger::I(TAG, "Rebooting, reason: %s", (const char*)reason);
        PowerManagerService::LowLevelReboot(reason);
        Logger::E(TAG, "Reboot failed, will attempt shutdown instead");
    }
    else if (SHUTDOWN_VIBRATE_MS > 0) {
        // vibrate before shutting down
        AutoPtr<IVibrator> vibrator;
        CSystemVibrator::New((IVibrator**)&vibrator);
        // try {
        if (FAILED(vibrator->Vibrate(SHUTDOWN_VIBRATE_MS, VIBRATION_ATTRIBUTES))) {
            Logger::W(TAG, "Failed to vibrate during shutdown.");
        }
        // } catch (Exception e) {
        //     // Failure to vibrate shouldn't interrupt shutdown.  Just log it.
        //     Log.w(TAG, "Failed to vibrate during shutdown.", e);
        // }

        // vibrator is asynchronous so we need to wait to avoid shutting down too soon.
        // try {
        Thread::Sleep(SHUTDOWN_VIBRATE_MS);
        // } catch (InterruptedException unused) {
        // }
    }

    // Shutdown power
    Logger::I(TAG, "Performing low-level shutdown...");
    PowerManagerService::LowLevelShutdown();
}

} // namespace Power
} // namespace Server
} // namespace Droid
} // namespace Elastos

