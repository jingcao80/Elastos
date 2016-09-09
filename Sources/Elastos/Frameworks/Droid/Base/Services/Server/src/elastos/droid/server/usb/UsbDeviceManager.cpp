
#include "elastos/droid/server/usb/UsbDeviceManager.h"
#include "elastos/droid/server/FgThread.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Character.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/usb/f_accessory.h>

using Elastos::Droid::App::CAppOpsManagerHelper;
using Elastos::Droid::App::IAppOpsManagerHelper;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Hardware::Usb::CUsbAccessory;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IRingtone;
using Elastos::Droid::Media::CRingtone;
using Elastos::Droid::Media::IRingtoneManager;
using Elastos::Droid::Media::CRingtoneManager;
using Elastos::Droid::Media::IRingtoneManagerHelper;
using Elastos::Droid::Media::CRingtoneManagerHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::CParcelFileDescriptor;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::Storage::CStorageManagerHelper;
using Elastos::Droid::Os::Storage::IStorageManagerHelper;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Core::Character;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::IO::CFileWriter;
using Elastos::IO::IFileWriter;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::IOutputStreamWriter;
using Elastos::IO::COutputStreamWriter;
using Elastos::Utility::CScanner;
using Elastos::Utility::IScanner;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Usb {

const Boolean UsbDeviceManager::DEBUG = FALSE;
const String UsbDeviceManager::TAG("UsbDeviceManager");
const String UsbDeviceManager::USB_STATE_MATCH("DEVPATH=/devices/virtual/android_usb/android0");
const String UsbDeviceManager::ACCESSORY_START_MATCH("DEVPATH=/devices/virtual/misc/usb_accessory");
const String UsbDeviceManager::FUNCTIONS_PATH("/sys/class/android_usb/android0/functions");
const String UsbDeviceManager::STATE_PATH("/sys/class/android_usb/android0/state");
const String UsbDeviceManager::MASS_STORAGE_FILE_PATH("/sys/class/android_usb/android0/f_mass_storage/lun/file");
const String UsbDeviceManager::RNDIS_ETH_ADDR_PATH("/sys/class/android_usb/android0/f_rndis/ethaddr");
const String UsbDeviceManager::AUDIO_SOURCE_PCM_PATH("/sys/class/android_usb/android0/f_audio_source/pcm");
const Int32 UsbDeviceManager::ACCESSORY_REQUEST_TIMEOUT = 10 * 1000;
const String UsbDeviceManager::BOOT_MODE_PROPERTY("ro.bootmode");
const String UsbDeviceManager::DRIVER_NAME("/dev/usb_accessory");
const Int32 UsbDeviceManager::AUDIO_MODE_NONE;
const Int32 UsbDeviceManager::AUDIO_MODE_SOURCE;
const Int32 UsbDeviceManager::UPDATE_DELAY = 3000;// TODO 1000

const Int32 UsbDeviceManager::MSG_UPDATE_STATE = 0;
const Int32 UsbDeviceManager::MSG_ENABLE_ADB = 1;
const Int32 UsbDeviceManager::MSG_SET_CURRENT_FUNCTIONS = 2;
const Int32 UsbDeviceManager::MSG_SYSTEM_READY = 3;
const Int32 UsbDeviceManager::MSG_BOOT_COMPLETED = 4;
const Int32 UsbDeviceManager::MSG_USER_SWITCHED = 5;

ECode UsbDeviceManager::AdbSettingsObserver::constructor(
    /* [in] */ UsbDeviceManager* host)
{
    ContentObserver::constructor(NULL);
    mHost = host;
    return NOERROR;
}

ECode UsbDeviceManager::AdbSettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    Int32 isEnable = 0;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetInt32(mHost->mContentResolver, ISettingsGlobal::ADB_ENABLED, 0, &isEnable);
    Boolean enable = isEnable > 0 ? TRUE : FALSE;

    mHost->mHandler->SendMessage(UsbDeviceManager::MSG_ENABLE_ADB, enable);
    return NOERROR;
}

UsbDeviceManager::MyUEventObserver::MyUEventObserver(
    /* [in] */ UsbDeviceManager* host)
    : mHost(host)
{}

ECode UsbDeviceManager::MyUEventObserver::OnUEvent(
    /* [in] */ IUEvent* event)
{
    if (UsbDeviceManager::DEBUG) {
        Slogger::V(UsbDeviceManager::TAG, "USB UEVENT: %s", TO_CSTR(event));
    }

    String state, accessory;
    event->Get(String("USB_STATE"), &state);
    event->Get(String("ACCESSORY"), &accessory);
    if (!state.IsNull()) {
        mHost->mHandler->UpdateState(state);
    }
    else if (accessory.Equals("START")) {
        if (UsbDeviceManager::DEBUG) {
            Slogger::D(UsbDeviceManager::TAG, "got accessory start");
        }
        mHost->StartAccessoryMode();
    }
    return NOERROR;
}

UsbDeviceManager::UsbHandler::BootCompletedReceiver::BootCompletedReceiver(
    /* [in] */ UsbHandler* host)
    : mHost(host)
{}

ECode UsbDeviceManager::UsbHandler::BootCompletedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (UsbDeviceManager::DEBUG) Slogger::D(TAG, "boot completed");

    Boolean result;
    mHost->mHost->mHandler->SendEmptyMessage(UsbDeviceManager::MSG_BOOT_COMPLETED, &result);
    return NOERROR;
}

UsbDeviceManager::UsbHandler::UserSwitchedReceiver::UserSwitchedReceiver(
    /* [in] */ UsbHandler* host)
    : mHost(host)
{}

ECode UsbDeviceManager::UsbHandler::UserSwitchedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 userId;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &userId);

    AutoPtr<IMessage> msg;
    mHost->mHost->mHandler->ObtainMessage(MSG_USER_SWITCHED, userId, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

//==================================================================
// UsbDeviceManager::UsbHandler
//==================================================================
UsbDeviceManager::UsbHandler::UsbHandler(
    /* [in] */ UsbDeviceManager* host,
    /* [in] */ ILooper* looper)
    : Handler(looper)
    , mHost(host)
    , mConnected(FALSE)
    , mConfigured(FALSE)
    , mUsbNotificationId(0)
    , mAdbNotificationId(0)
    , mCurrentUser(IUserHandle::USER_NULL)
{
    mBootCompletedReceiver = (IBroadcastReceiver*)new BootCompletedReceiver(this);
    mUserSwitchedReceiver = (IBroadcastReceiver*)new UserSwitchedReceiver(this);
}

ECode UsbDeviceManager::UsbHandler::Init()
{
    // try {
    // persist.sys.usb.config should never be unset.  But if it is, set it to "adb"
    // so we have a chance of debugging what happened.
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    sysProp->Get(String("persist.sys.usb.config"), String("adb"), &mDefaultFunctions);

    // Check if USB mode needs to be overridden depending on OEM specific bootmode.
    mDefaultFunctions = mHost->ProcessOemUsbOverride(mDefaultFunctions);

    // sanity check the sys.usb.config system property
    // this may be necessary if we crashed while switching USB configurations
    String config;
    sysProp->Get(String("sys.usb.config"), String("none"), &config);
    if (!config.Equals(mDefaultFunctions)) {
        Slogger::W(TAG, "resetting config to persistent property: %s", mDefaultFunctions.string());
        sysProp->Set(String("sys.usb.config"), mDefaultFunctions);
    }

    mCurrentFunctions = mDefaultFunctions;
    AutoPtr<IFile> file;
    ECode ec = CFile::New(STATE_PATH, (IFile**)&file);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Error initializing UsbHandler 0x%08x", ec);
    }
    String state;
    ec = FileUtils::ReadTextFile(file, 0, String(NULL), &state);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Error initializing UsbHandler 0x%08x", ec);
    }
    state = state.Trim();
    UpdateState(state);
    mHost->mAdbEnabled = mHost->ContainsFunction(mCurrentFunctions, IUsbManager::USB_FUNCTION_ADB);

    // Upgrade step for previous versions that used persist.service.adb.enable
    String value;
    sysProp->Get(String("persist.service.adb.enable"), String(NULL), &value);
    if (!value.IsNullOrEmpty()) {
        Char32 enable = value.GetChar(0);
        if (enable == '1') {
            SetAdbEnabled(TRUE);
        }
        else if (enable == '0') {
            SetAdbEnabled(FALSE);
        }
        sysProp->Set(String("persist.service.adb.enable"), String(NULL));
    }

    // register observer to listen for settings changes
    AutoPtr<AdbSettingsObserver> adbsettingobsvr = new AdbSettingsObserver();
    adbsettingobsvr->constructor(mHost);
    AutoPtr<IUri> uri;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    ec = settingsGlobal->GetUriFor(ISettingsGlobal::ADB_ENABLED, (IUri**)&uri);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Error initializing UsbHandler 0x%08x", ec);
    }
    mHost->mContentResolver->RegisterContentObserver(uri, FALSE, adbsettingobsvr);

    AutoPtr<AdbNotificationObserver> adbNotificationObserver = new AdbNotificationObserver();
    adbNotificationObserver->constructor(this);

    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    uri = NULL;
    settingsSecure->GetUriFor(ISettingsSecure::ADB_PORT, (IUri**)&uri);
    mHost->mContentResolver->RegisterContentObserver(uri, FALSE, adbNotificationObserver);
    uri = NULL;
    settingsSecure->GetUriFor(ISettingsSecure::ADB_NOTIFY, (IUri**)&uri);
    mHost->mContentResolver->RegisterContentObserver(uri, FALSE, adbNotificationObserver);

    // Watch for USB configuration changes
    mHost->mUEventObserver->StartObserving(USB_STATE_MATCH);
    mHost->mUEventObserver->StartObserving(ACCESSORY_START_MATCH);

    AutoPtr<IIntent> intent;
    AutoPtr<IIntentFilter> filter1, filter2;
    CIntentFilter::New(IIntent::ACTION_BOOT_COMPLETED, (IIntentFilter**)&filter1);
    filter1->SetPriority(IIntentFilter::SYSTEM_HIGH_PRIORITY);
    CIntentFilter::New(IIntent::ACTION_USER_SWITCHED, (IIntentFilter**)&filter2);
    mHost->mContext->RegisterReceiver(mBootCompletedReceiver, filter1, (IIntent**)&intent);
    intent = NULL;
    mHost->mContext->RegisterReceiver(mUserSwitchedReceiver, filter2, (IIntent**)&intent);
    // } catch (Exception e) {
    //     Slog.e(TAG, "Error initializing UsbHandler", e);
    // }

    return NOERROR;
}

void UsbDeviceManager::UsbHandler::UpdateState(
    /* [in] */ const String& state)
{
    Int32 connected, configured;

    if (state.Equals("DISCONNECTED")) {
        connected = 0;
        configured = 0;
    }
    else if (state.Equals("CONNECTED")) {
        connected = 1;
        configured = 0;
    }
    else if (state.Equals("CONFIGURED")) {
        connected = 1;
        configured = 1;
    }
    else {
        Slogger::E(TAG, "unknown state %s", state.string());
        return;
    }

    RemoveMessages(UsbDeviceManager::MSG_UPDATE_STATE);
    AutoPtr<IMessage> m;
    this->ObtainMessage(UsbDeviceManager::MSG_UPDATE_STATE, (IMessage**)&m);
    m->SetArg1(connected);
    m->SetArg2(configured);
    // debounce disconnects to avoid problems bringing up USB tethering
    Boolean result;
    SendMessageDelayed(m, (connected == 0) ? UPDATE_DELAY : 0, &result);
}

void UsbDeviceManager::UsbHandler::SendMessage(
    /* [in] */ Int32 what,
    /* [in] */ Boolean arg)
{
    RemoveMessages(what);
    AutoPtr<IMessage> m;
    this->ObtainMessage(what, (IMessage**)&m);
    m->SetArg1(arg ? 1 : 0);
    Boolean result;
    SendMessage(m, &result);
}

void UsbDeviceManager::UsbHandler::SendMessage(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* arg)
{
    RemoveMessages(what);
    AutoPtr<IMessage> m;
    this->ObtainMessage(what, arg, (IMessage**)&m);
    Boolean result;
    SendMessage(m, &result);
}

void UsbDeviceManager::UsbHandler::SendMessage(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* arg0,
    /* [in] */ Boolean arg1)
{
    RemoveMessages(what);
    AutoPtr<IMessage> m;
    this->ObtainMessage(what, arg0, (IMessage**)&m);
    m->SetArg1(arg1 ? 1 : 0);
    Boolean result;
    SendMessage(m, &result);
}

Boolean UsbDeviceManager::UsbHandler::WaitForState(
    /* [in] */ const String& state)
{
    // wait for the transition to complete.
    // give up after 1 second.
    for (Int32 i = 0; i < 20; i++) {
        // State transition is done when sys.usb.state is set to the new configuration
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        String value;
        sysProp->Get(String("sys.usb.state"), &value);
        if (state.Equals(value)) {
            return TRUE;
        }
        SystemClock::Sleep(50);
    }
    Slogger::E(TAG, "waitForState(%s) FAILED", state.string());
    return FALSE;
}

Boolean UsbDeviceManager::UsbHandler::SetUsbConfig(
    /* [in] */ const String& config)
{
    if (UsbDeviceManager::DEBUG) Slogger::D(TAG, "setUsbConfig(%s)", (const char*) config);
    // set the new configuration
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    sysProp->Set(String("sys.usb.config"), config);
    return WaitForState(config);
}

void UsbDeviceManager::UsbHandler::SetAdbEnabled(
    /* [in] */ Boolean enable)
{
    if (UsbDeviceManager::DEBUG) Slogger::D(TAG, "setAdbEnabled: (%d)", enable);
    if (enable != mHost->mAdbEnabled) {
        mHost->mAdbEnabled = enable;
        // Due to the persist.sys.usb.config property trigger, changing adb state requires
        // switching to default function
        SetEnabledFunctions(mDefaultFunctions, TRUE);
        UpdateAdbNotification();
    }
    if (mHost->mDebuggingManager != NULL) {
        mHost->mDebuggingManager->SetAdbEnabled(mHost->mAdbEnabled);
    }
}

void UsbDeviceManager::UsbHandler::SetEnabledFunctions(
    /* [in] */ const String& str,
    /* [in] */ Boolean makeDefault)
{
    String functions = str;
    if (DEBUG) Slogger::D(TAG, "setEnabledFunctions %s makeDefault: %d",
        functions.string(), makeDefault);

    // Do not update persystent.sys.usb.config if the device is booted up
    // with OEM specific mode.
    if (!functions.IsNull() && makeDefault && !mHost->NeedsOemUsbOverride()) {
        if (!IUsbManager::USB_FUNCTION_CHARGING.Equals(functions)
            && mHost->mAdbEnabled) {
            functions = AddFunction(functions, IUsbManager::USB_FUNCTION_ADB);
        }
        else {
            functions = RemoveFunction(functions, IUsbManager::USB_FUNCTION_ADB);
        }
        if (!mDefaultFunctions.Equals(functions)) {
            if (!SetUsbConfig(String("none"))) {
                Slogger::E(TAG, "Failed to disable USB");
                // revert to previous configuration if we fail
                SetUsbConfig(mCurrentFunctions);
                return;
            }
            // setting this property will also change the current USB state
            // via a property trigger
            AutoPtr<ISystemProperties> sysProp;
            CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
            sysProp->Set(String("persist.sys.usb.config"), functions);
            if (WaitForState(functions)) {
                mCurrentFunctions = functions;
                mDefaultFunctions = functions;
            }
            else {
                Slogger::E(TAG, "Failed to switch persistent USB config to %s", functions.string());
                // revert to previous configuration if we fail
                sysProp->Set(String("persist.sys.usb.config"), mDefaultFunctions);
            }
        }
    }
    else {
        if (functions.IsNull()) {
            functions = mDefaultFunctions;
        }

        // Override with bootmode specific usb mode if needed
        functions = mHost->ProcessOemUsbOverride(functions);

        if (!IUsbManager::USB_FUNCTION_CHARGING.Equals(functions)
            && mHost->mAdbEnabled) {
            functions = AddFunction(functions, IUsbManager::USB_FUNCTION_ADB);
        }
        else {
            functions = RemoveFunction(functions, IUsbManager::USB_FUNCTION_ADB);
        }
        if (!mCurrentFunctions.Equals(functions)) {
            if (!SetUsbConfig(String("none"))) {
                Slogger::E(TAG, "Failed to disable USB");
                // revert to previous configuration if we fail
                SetUsbConfig(mCurrentFunctions);
                return;
            }
            if (SetUsbConfig(functions)) {
                mCurrentFunctions = functions;
            }
            else {
                Slogger::E(TAG, "Failed to switch USB config to %s", functions.string());
                // revert to previous configuration if we fail
                SetUsbConfig(mCurrentFunctions);
            }
        }
    }
}

void UsbDeviceManager::UsbHandler::UpdateCurrentAccessory()
{
    // We are entering accessory mode if we have received a request from the host
    // and the request has not timed out yet.
    Boolean enteringAccessoryMode = mHost->mAccessoryModeRequestTime > 0 &&
        SystemClock::GetElapsedRealtime() < mHost->mAccessoryModeRequestTime + ACCESSORY_REQUEST_TIMEOUT;

    if (mConfigured && enteringAccessoryMode) {
        // successfully entered accessory mode

        if (mHost->mAccessoryStrings != NULL) {
            mCurrentAccessory = NULL;
            CUsbAccessory::New(*mHost->mAccessoryStrings, (IUsbAccessory**)&mCurrentAccessory);
            Slogger::D(TAG, "entering USB accessory mode: %p", mCurrentAccessory.Get());
            // defer accessoryAttached if system is not ready
            if (mHost->mBootCompleted) {
                mHost->GetCurrentSettings()->AccessoryAttached(mCurrentAccessory);
            } // else handle in mBootCompletedReceiver
        }
        else {
            Slogger::E(TAG, "nativeGetAccessoryStrings failed");
        }
    }
    else if (!enteringAccessoryMode) {
        // make sure accessory mode is off
        // and restore default functions
        Slogger::D(TAG, "exited USB accessory mode");
        SetEnabledFunctions(mDefaultFunctions, FALSE);

        if (mCurrentAccessory != NULL) {
            if (mHost->mBootCompleted) {
                mHost->GetCurrentSettings()->AccessoryDetached(mCurrentAccessory);
            }
            mCurrentAccessory = NULL;
            mHost->mAccessoryStrings = NULL;
        }
    }
}

void UsbDeviceManager::UsbHandler::UpdateUsbState()
{
    // send a sticky broadcast containing current USB state
    AutoPtr<IIntent> intent;
    CIntent::New(IUsbManager::ACTION_USB_STATE,(IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    intent->PutBooleanExtra(IUsbManager::USB_CONNECTED, mConnected);
    intent->PutBooleanExtra(IUsbManager::USB_CONFIGURED, mConfigured);

    if (!mCurrentFunctions.IsNull()) {
        AutoPtr<ArrayOf<String> > functions;
        StringUtils::Split(mCurrentFunctions, String(","), (ArrayOf<String>**)&functions);
        for (Int32 i = 0; i < functions->GetLength(); i++) {
            intent->PutBooleanExtra((*functions)[i], TRUE);
        }
    }

    if (DEBUG) Slogger::D(TAG, "broadcasting %s connected: %d configured: %d",
        TO_CSTR(intent), mConnected, mConfigured);

    mHost->mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

void UsbDeviceManager::UsbHandler::UpdateAudioSourceFunction()
{
    Boolean enabled = ContainsFunction(mCurrentFunctions,
            IUsbManager::USB_FUNCTION_AUDIO_SOURCE);
    if (enabled != mHost->mAudioSourceEnabled) {
        // send a sticky broadcast containing current USB state
        AutoPtr<IIntent> intent;
        CIntent::New(IAudioManager::ACTION_USB_AUDIO_ACCESSORY_PLUG,(IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
        intent->PutExtra(String("state"), enabled ? 1 : 0);
        if (enabled) {
            AutoPtr<IScanner> scanner;
            ECode ec;
            do {
                AutoPtr<IFile> file;
                ec = CFile::New(AUDIO_SOURCE_PCM_PATH, (IFile**)&file);
                if (FAILED(ec))
                    break;
                ec = CScanner::New(file, (IScanner**)&scanner);
                if (FAILED(ec))
                    break;
                Int32 card, device;
                ec = scanner->NextInt32(&card);
                if (FAILED(ec))
                    break;
                ec = scanner->NextInt32(&device);
                if (FAILED(ec))
                    break;
                intent->PutExtra(String("card"), card);
                intent->PutExtra(String("device"), device);
            } while (0);

            if (FAILED(ec)) {
                Slogger::E(TAG, "could not open audio source PCM file ec = 0x%08x", ec);
            }
            if (scanner != NULL) {
                scanner->Close();
            }
        }

        mHost->mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
        mHost->mAudioSourceEnabled = enabled;
    }
}

ECode UsbDeviceManager::UsbHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);

    switch (what) {
        case UsbDeviceManager::MSG_UPDATE_STATE:
            HandleMsgUpdateState(arg1 == 1, arg2 == 1);
            break;
        case UsbDeviceManager::MSG_ENABLE_ADB:
            HandleMsgEnableAdb(arg1 == 1);
            break;
        case UsbDeviceManager::MSG_SET_CURRENT_FUNCTIONS: {
                AutoPtr<IInterface> seq;
                msg->GetObj((IInterface**)&seq);
                String info;
                ICharSequence::Probe(seq)->ToString(&info);
                HandleMsgSetCurrentFunctions(info, arg1 == 1);
            }
            break;
        case UsbDeviceManager::MSG_SYSTEM_READY:
            HandleMsgSystemReady();
            break;
        case UsbDeviceManager::MSG_BOOT_COMPLETED:
            HandleMsgBootCompleted();
            break;
        case UsbDeviceManager::MSG_USER_SWITCHED:
            HandleMsgUserSwitched(arg1);
            break;
    }
    return NOERROR;
}

void UsbDeviceManager::UsbHandler::HandleMsgUpdateState(
    /* [in] */ Boolean connected,
    /* [in] */ Boolean configured)
{
    mConnected = connected;
    mConfigured = configured;

    UpdateUsbNotification();
    UpdateAdbNotification();
    if (mHost->ContainsFunction(mCurrentFunctions,
            IUsbManager::USB_FUNCTION_ACCESSORY)) {
        UpdateCurrentAccessory();
    } else if (!mConnected) {
        // restore defaults when USB is disconnected
        SetEnabledFunctions(mDefaultFunctions, FALSE);
    }
    if (mHost->mBootCompleted) {
        UpdateUsbState();
        UpdateAudioSourceFunction();
    }
}

void UsbDeviceManager::UsbHandler::HandleMsgEnableAdb(
    /* [in] */ Boolean enabled)
{
    SetAdbEnabled(enabled);
}

void UsbDeviceManager::UsbHandler::HandleMsgSetCurrentFunctions(
    /* [in] */ const String& str,
    /* [in] */ Boolean useDeafault)
{
    SetEnabledFunctions(str, useDeafault);
}

void UsbDeviceManager::UsbHandler::HandleMsgSystemReady()
{
    UpdateUsbNotification();
    UpdateAdbNotification();
    UpdateUsbState();
    UpdateAudioSourceFunction();
}

void UsbDeviceManager::UsbHandler::HandleMsgBootCompleted()
{
    mHost->mBootCompleted = true;
    if (mCurrentAccessory != NULL) {
        mHost->GetCurrentSettings()->AccessoryAttached(mCurrentAccessory);
    }

    if (mHost->mDebuggingManager != NULL) {
        mHost->mDebuggingManager->SetAdbEnabled(mHost->mAdbEnabled);
    }
}

void UsbDeviceManager::UsbHandler::HandleMsgUserSwitched(
    /* [in] */ Int32 currentUser)
{
    AutoPtr<IInterface> service;
    mHost->mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&service);
    AutoPtr<IUserManager> userManager = IUserManager::Probe(service);
    Boolean res;
    userManager->HasUserRestriction(IUserManager::DISALLOW_USB_FILE_TRANSFER, &res);
    if (res) {
        Slogger::V(TAG, "Switched to user with DISALLOW_USB_FILE_TRANSFER restriction; disabling USB.");
        SetUsbConfig(String("none"));
        mCurrentUser = currentUser;
        return;
    }

    Boolean mtpActive =
        mHost->ContainsFunction(mCurrentFunctions, IUsbManager::USB_FUNCTION_MTP)
        || mHost->ContainsFunction(mCurrentFunctions, IUsbManager::USB_FUNCTION_PTP);
    if (mtpActive && mCurrentUser != IUserHandle::USER_NULL) {
        Slogger::V(TAG, "Current user switched; resetting USB host stack for MTP");
        SetUsbConfig(String("none"));
        SetUsbConfig(mCurrentFunctions);
    }
    mCurrentUser = currentUser;
}

AutoPtr<IUsbAccessory> UsbDeviceManager::UsbHandler::GetCurrentAccessory()
{
    return mCurrentAccessory;
}

void UsbDeviceManager::UsbHandler::UpdateUsbNotification()
{
    if (mHost->mNotificationManager == NULL || !mHost->mUseUsbNotification) return;

    Int32 id = 0;
    AutoPtr<IResources> r;
    mHost->mContext->GetResources((IResources**)&r);
    if (mConnected) {
        if (ContainsFunction(mCurrentFunctions, IUsbManager::USB_FUNCTION_CHARGING)) {
            id = R::string::usb_charging_notification_title;
        }
        else if (ContainsFunction(mCurrentFunctions, IUsbManager::USB_FUNCTION_MTP)) {
            id = R::string::usb_mtp_notification_title;
        }
        else if (ContainsFunction(mCurrentFunctions, IUsbManager::USB_FUNCTION_PTP)) {
            id = R::string::usb_ptp_notification_title;
        }
        else if (ContainsFunction(mCurrentFunctions,
                IUsbManager::USB_FUNCTION_MASS_STORAGE)) {
            id = R::string::usb_cd_installer_notification_title;
        }
        else if (ContainsFunction(mCurrentFunctions, IUsbManager::USB_FUNCTION_ACCESSORY)) {
            id = R::string::usb_accessory_notification_title;
        }
        else {
            AutoPtr<IAppOpsManagerHelper> aomHelper;
            CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aomHelper);
            Boolean isStrictEnable;
            aomHelper->IsStrictEnable(&isStrictEnable);
            if (isStrictEnable) {
                id = R::string::usb_choose_notification_title;
            }
            // There is a different notification for USB tethering so we don't need one here
            //if (!containsFunction(mCurrentFunctions, UsbManager.USB_FUNCTION_RNDIS)) {
            //    Slog.e(TAG, "No known USB function in updateUsbNotification");
            //}
        }
    }
    if (id != mUsbNotificationId) {
        // clear notification if title needs changing
        if (mUsbNotificationId != 0) {
            mHost->mNotificationManager->CancelAsUser(String(NULL), mUsbNotificationId,
                    UserHandle::ALL);
            mUsbNotificationId = 0;
        }
        if (id != 0) {
            AutoPtr<ICharSequence> message, title;
            r->GetText(R::string::usb_notification_message, (ICharSequence**)&message);
            r->GetText(id, (ICharSequence**)&title);

            AutoPtr<INotification> notification;
            CNotification::New((INotification**)&notification);
            notification->SetIcon(R::drawable::stat_sys_data_usb);
            notification->SetWhen(0);
            notification->SetFlags(INotification::FLAG_ONGOING_EVENT);
            notification->SetTickerText(title);
            notification->SetDefaults(0); // please be quiet
            notification->SetSound(NULL);
            notification->SetVibrate(NULL);
            notification->SetPriority(INotification::PRIORITY_MIN);

            AutoPtr<IComponentName> componentName;
            CComponentName::New(
                    String("com.android.settings"),
                    String("com.android.settings.UsbSettings"),
                    (IComponentName**)&componentName);
            AutoPtr<IIntent> intent;
            AutoPtr<IIntentHelper> intentHelper;
            CIntentHelper::AcquireSingleton((IIntentHelper**)&intentHelper);
            intentHelper->MakeRestartActivityTask(componentName, (IIntent**)&intent);

            AutoPtr<IPendingIntent> pi;
            AutoPtr<IPendingIntentHelper> pendingIntentHelper;
            CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingIntentHelper);
            pendingIntentHelper->GetActivityAsUser(mHost->mContext, 0,
                    intent, 0, NULL, UserHandle::CURRENT, (IPendingIntent**)&pi);
            Int32 color;
            r->GetColor(R::color::system_notification_accent_color, &color);
            notification->SetColor(color);
            notification->SetLatestEventInfo(mHost->mContext, title, message, pi);
            notification->SetVisibility(INotification::VISIBILITY_PUBLIC);
            mHost->mNotificationManager->NotifyAsUser(String(NULL), id, notification,
                    UserHandle::ALL);
            mUsbNotificationId = id;
        }
    }
}

void UsbDeviceManager::UsbHandler::UpdateAdbNotification()
{
    if (mHost->mNotificationManager == NULL) return;

    Int32 id;
    Boolean usbAdbActive = mHost->mAdbEnabled && mConnected;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    Int32 adbPort;
    settingsSecure->GetInt32(mHost->mContentResolver, ISettingsSecure::ADB_PORT, -1, &adbPort);
    Boolean netAdbActive = mHost->mAdbEnabled && adbPort > 0;
    Int32 adbNotify;
    settingsSecure->GetInt32(mHost->mContentResolver, ISettingsSecure::ADB_NOTIFY, 1, &adbNotify);
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String value;
    sysProp->Get(String("persist.adb.notify"), &value);
    Boolean hideNotification = value.Equals("0") || adbNotify == 0;

    if (hideNotification) {
        id = 0;
    }
    else if (usbAdbActive && netAdbActive) {
        id = R::string::adb_both_active_notification_title;
    }
    else if (usbAdbActive) {
        id = R::string::adb_active_notification_title;
    }
    else if (netAdbActive) {
        id = R::string::adb_net_active_notification_title;
    }
    else {
        id = 0;
    }

    if (id != mAdbNotificationId) {
        if (mAdbNotificationId != 0) {
            mHost->mNotificationManager->CancelAsUser(String(NULL), mAdbNotificationId, UserHandle::ALL);
        }
        if (id != 0) {
            AutoPtr<IResources> r;
            mHost->mContext->GetResources((IResources**)&r);
            AutoPtr<ICharSequence> title, message;
            r->GetText(id, (ICharSequence**)&title);
            r->GetText(R::string::adb_active_generic_notification_message, (ICharSequence**)&message);

            AutoPtr<INotification> notification;
            CNotification::New((INotification**)&notification);
            notification->SetIcon(R::drawable::stat_sys_adb);
            notification->SetWhen(0);
            notification->SetFlags(INotification::FLAG_ONGOING_EVENT);
            notification->SetTickerText(title);
            notification->SetDefaults(0); // please be quiet
            notification->SetSound(NULL);
            notification->SetVibrate(NULL);
            notification->SetPriority(INotification::PRIORITY_LOW);

            AutoPtr<IComponentName> componentName;
            CComponentName::New(
                    String("com.android.settings"),
                    String("com.android.settings.DevelopmentSettings"),
                    (IComponentName**)&componentName);
            AutoPtr<IIntentHelper> intentHelper;
            CIntentHelper::AcquireSingleton((IIntentHelper**)&intentHelper);
            AutoPtr<IIntent> intent;
            intentHelper->MakeRestartActivityTask(componentName, (IIntent**)&intent);

            AutoPtr<IPendingIntentHelper> pendingIntentHelper;
            CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingIntentHelper);
            AutoPtr<IPendingIntent> pi;
            pendingIntentHelper->GetActivityAsUser(mHost->mContext, 0,
                    intent, 0, NULL, UserHandle::CURRENT, (IPendingIntent**)&pi);

            Int32 color;
            r->GetColor(R::color::system_notification_accent_color, &color);
            notification->SetColor(color);
            notification->SetLatestEventInfo(mHost->mContext, title, message, pi);
            notification->SetVisibility(INotification::VISIBILITY_PUBLIC);
            mHost->mNotificationManager->NotifyAsUser(String(NULL), id, notification,
                    UserHandle::ALL);
        }
        mAdbNotificationId = id;
    }
}

//void UsbDeviceManager::UsbHandler::Dump(FileDescriptor fd, PrintWriter pw) {
//    pw.println("  USB Device State:");
//    pw.println("    Current Functions: " + mCurrentFunctions);
//    pw.println("    Default Functions: " + mDefaultFunctions);
//    pw.println("    mConnected: " + mConnected);
//    pw.println("    mConfigured: " + mConfigured);
//    pw.println("    mCurrentAccessory: " + mCurrentAccessory);
//    try {
//        pw.println("    Kernel state: "
//                + FileUtils.readTextFile(new File(STATE_PATH), 0, null).trim());
//        pw.println("    Kernel function list: "
//                + FileUtils.readTextFile(new File(FUNCTIONS_PATH), 0, null).trim());
//        pw.println("    Mass storage backing file: "
//                + FileUtils.readTextFile(new File(MASS_STORAGE_FILE_PATH), 0, null).trim());
//    } catch (IOException e) {
//        pw.println("IOException: " + e);
//    }
//}

UsbDeviceManager::UsbDeviceManager(
    /* [in] */ IContext* context)
    : mAccessoryModeRequestTime(0)
    , mBootCompleted(FALSE)
    , mContext(context)
    , mUseUsbNotification(FALSE)
    , mAdbEnabled(FALSE)
    , mAudioSourceEnabled(FALSE)
{
    mUEventObserver = new MyUEventObserver(this);

    context->GetContentResolver((IContentResolver**)&mContentResolver);
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    pm->HasSystemFeature(IPackageManager::FEATURE_USB_ACCESSORY, &mHasUsbAccessory);
    InitRndisAddress();

    ReadOemUsbOverrideConfig();

    AutoPtr<ILooper> looper;
    FgThread::Get()->GetLooper((ILooper**)&looper);
    mHandler = new UsbHandler(this, looper);
    mHandler->Init();

    if (NativeIsStartRequested()) {
        if (DEBUG) {
            Slogger::D(TAG, "accessory attached at boot");
        }
        StartAccessoryMode();
    }

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    Boolean secureAdbEnabled;
    sysProp->GetBoolean(String("ro.adb.secure"), FALSE, &secureAdbEnabled);
    String value;
    sysProp->Get(String("vold.decrypt"), &value);
    Boolean dataEncrypted = value.Equals("1");
    if (secureAdbEnabled && !dataEncrypted) {
        mDebuggingManager = new UsbDebuggingManager(context);
    }
}

UsbDeviceManager::~UsbDeviceManager()
{
}

void UsbDeviceManager::SetCurrentSettings(
    /* [in] */ UsbSettingsManager* settings)
{
    AutoLock lock(mLock);
    mCurrentSettings = settings;
}

AutoPtr<UsbSettingsManager> UsbDeviceManager::GetCurrentSettings()
{
    AutoLock lock(mLock);
    return mCurrentSettings;
}

void UsbDeviceManager::SystemReady()
{
    if (DEBUG) {
        Slogger::D(TAG, "systemReady");
    }

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
    mNotificationManager = INotificationManager::Probe(obj);

    // We do not show the USB notification if the primary volume supports mass storage.
    // The legacy mass storage UI will be used instead.
    Boolean massStorageSupported = FALSE;
    AutoPtr<IStorageManager> storageMgr;
    AutoPtr<IStorageManagerHelper> storageMgrHelper;
    CStorageManagerHelper::AcquireSingleton((IStorageManagerHelper**)&storageMgrHelper);
    storageMgrHelper->From(mContext, (IStorageManager**)&storageMgr);
    AutoPtr<IStorageVolume> primary;
    storageMgr->GetPrimaryVolume((IStorageVolume**)&primary);
    if (primary != NULL) {
        primary->AllowMassStorage(&massStorageSupported);
    }
    mUseUsbNotification = !massStorageSupported;

    // make sure the ADB_ENABLED setting value matches the current state
    Boolean result;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    if (FAILED(settingsGlobal->PutInt32(mContentResolver,
        ISettingsGlobal::ADB_ENABLED, mAdbEnabled ? 1 : 0, &result))) {
        // If UserManager.DISALLOW_DEBUGGING_FEATURES is on, that this setting can't be changed.
        Slogger::D(TAG, "ADB_ENABLED is restricted.");
    }

    mHandler->SendEmptyMessage(MSG_SYSTEM_READY, &result);
}

void UsbDeviceManager::StartAccessoryMode()
{
    if (!mHasUsbAccessory) return;

    mAccessoryStrings = NativeGetAccessoryStrings();
    Boolean enableAudio = (NativeGetAudioMode() == AUDIO_MODE_SOURCE);
    // don't start accessory mode if our mandatory strings have not been set
    Boolean enableAccessory = (mAccessoryStrings != NULL &&
            !((*mAccessoryStrings)[IUsbAccessory::MANUFACTURER_STRING]).IsNull() &&
            !((*mAccessoryStrings)[IUsbAccessory::MODEL_STRING]).IsNull());
    String functions;

    if (enableAccessory && enableAudio) {
        StringBuilder buf(IUsbManager::USB_FUNCTION_ACCESSORY);
        buf += ",";
        buf += IUsbManager::USB_FUNCTION_AUDIO_SOURCE;
        functions = buf.ToString();
    }
    else if (enableAccessory) {
        functions = IUsbManager::USB_FUNCTION_ACCESSORY;
    }
    else if (enableAudio) {
        functions = IUsbManager::USB_FUNCTION_AUDIO_SOURCE;
    }

    if (!functions.IsNull()) {
        mAccessoryModeRequestTime = SystemClock::GetElapsedRealtime();
        SetCurrentFunctions(functions, FALSE);
    }
}
// begin from this
void UsbDeviceManager::InitRndisAddress()
{
    // configure RNDIS ethernet address based on our serial number using the same algorithm
    // we had been previously using in kernel board files
    Int32 ETH_ALEN = 6;
    AutoPtr< ArrayOf<Int32> > address = ArrayOf<Int32>::Alloc(ETH_ALEN);
    // first byte is 0x02 to signify a locally administered address
    (*address)[0] = 0x02;

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String serial;
    sysProp->Get(String("ro.serialno"), String("1234567890ABCDEF"), &serial);
    Int32 serialLength = serial.GetLength();
    // XOR the USB serial across the remaining 5 bytes
    for (Int32 i = 0; i < serialLength; i++) {
        (*address)[i % (ETH_ALEN - 1) + 1] ^= (Int32)serial.GetChar(i);
    }

    // String addrString = String.format(Locale.US, "%02X:%02X:%02X:%02X:%02X:%02X",
    //         address[0], address[1], address[2], address[3], address[4], address[5]);
    String addrString;
    addrString.AppendFormat("%02X:%02X:%02X:%02X:%02X:%02X", (*address)[0], (*address)[1],
        (*address)[2], (*address)[3], (*address)[4], (*address)[5]);
    // try {
    if(FAILED(FileUtils::StringToFile(RNDIS_ETH_ADDR_PATH, addrString))) {
        Slogger::E(TAG, "failed to write to %s", RNDIS_ETH_ADDR_PATH.string());
    }
    // } catch (IOException e) {
    //    Slog.e(TAG, "failed to write to " + RNDIS_ETH_ADDR_PATH);
    // }
}

String UsbDeviceManager::AddFunction(
    /* [in] */ const String& functions,
    /* [in] */ const String& function)
{
    if (functions.Equals("none")) {
        return function;
    }
    StringBuilder builder(functions);
    if (!ContainsFunction(functions, function)) {
        if (!functions.IsNullOrEmpty()) {
            builder += ",";
        }
        builder += function;
    }
    return builder.ToString();
}

String UsbDeviceManager::RemoveFunction(
    /* [in] */ const String& functions,
    /* [in] */ const String& function)
{
    AutoPtr< ArrayOf<String> > split;
    StringUtils::Split(functions, String(","), (ArrayOf<String>**)&split);
    Int32 size = split->GetLength();
    for (Int32 i = 0; i < size; i++) {
        if (function.Equals((*split)[i])) {
            (*split)[i] = String(NULL);
        }
    }
    if (size == 1 && (*split)[0].IsNull()) {
        return String("none");
    }
    StringBuilder builder;
    for (Int32 i = 0; i < size; i++) {
        String s((*split)[i]);
        if (!s.IsNull()) {
            if (builder.GetLength() > 0) {
                builder += ",";
            }
            builder += s;
        }
    }
    return builder.ToString();
}

Boolean UsbDeviceManager::ContainsFunction(
    /* [in] */ const String& functions,
    /* [in] */ const String& function)
{
    Int32 index = functions.IndexOf(function);
    if (index < 0) return FALSE;
    if (index > 0 && functions.GetChar(index - 1) != ',') return FALSE;
    Int32 charAfter = index + function.GetLength();
    if (charAfter < functions.GetLength() && functions.GetChar(charAfter) != ',') return FALSE;
    return TRUE;
}

AutoPtr<IUsbAccessory> UsbDeviceManager::GetCurrentAccessory()
{
    return mHandler->GetCurrentAccessory();
}

ECode UsbDeviceManager::OpenAccessory(
    /* [in] */ IUsbAccessory* accessory,
    /* [out] */ IParcelFileDescriptor** pfd)
{
    AutoPtr<IUsbAccessory> currentAccessory = mHandler->GetCurrentAccessory();
    if (currentAccessory == NULL) {
        // throw new IllegalArgumentException("no accessory attached");
        Slogger::E(TAG, "no accessory attached");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (currentAccessory.Get() == accessory) {
        // String error = accessory.toString(NULL) + " does not match current accessory " + currentAccessory;
        Slogger::E(TAG, "%p does not match current accessory %p", accessory, currentAccessory.Get());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(GetCurrentSettings()->CheckPermission(accessory));

    AutoPtr<IParcelFileDescriptor> tmp = NativeOpenAccessory();
    *pfd = tmp;
    REFCOUNT_ADD(*pfd);
    return NOERROR;
}

void UsbDeviceManager::SetCurrentFunctions(
    /* [in] */ const String& functions,
    /* [in] */ Boolean makeDefault)
{
    if (DEBUG) {
        Slogger::D(TAG, "setCurrentFunctions(%s) default: %d", functions.string(), makeDefault);
    }

    AutoPtr<ICharSequence> seq;
    CString::New(functions, (ICharSequence**)&seq);
    mHandler->SendMessage(MSG_SET_CURRENT_FUNCTIONS, seq, makeDefault);
}

void UsbDeviceManager::SetMassStorageBackingFile(
    /* [in] */ const String& path)
{
    String newPath(path);
    if (path.IsNull()) {
        newPath = String("");
    }
    else {
        newPath = path;
    }
    if (FAILED(FileUtils::StringToFile(MASS_STORAGE_FILE_PATH, newPath))) {
        Slogger::D(TAG, "failed to write to %s", MASS_STORAGE_FILE_PATH.string());
    }
}

void UsbDeviceManager::ReadOemUsbOverrideConfig()
{
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    AutoPtr< ArrayOf<String> > configList;
    resources->GetStringArray(R::array::config_oemUsbModeOverride, (ArrayOf<String>**)&configList);
    if (configList != NULL) {
        for (Int32 i = 0, size = configList->GetLength(); i < size; i++) {
            String config = (*configList)[i];
            AutoPtr< ArrayOf<String> > items;
            StringUtils::Split(config, String(":"), (ArrayOf<String>**)&items);
            if (items->GetLength() == 3) {
                if (mOemModeMap == NULL) {
                    mOemModeMap = new HashMap<String, AutoPtr<StringPairList> >();
                }
                AutoPtr<StringPairList> overrideList = (*mOemModeMap)[(*items)[0]];
                if (overrideList == NULL) {
                    overrideList = new StringPairList();
                    (*mOemModeMap)[(*items)[0]] = overrideList;
                }
                overrideList->PushBack(Pair<String, String>((*items)[1], (*items)[2]));
            }
        }
    }
}

Boolean UsbDeviceManager::NeedsOemUsbOverride()
{
    if (mOemModeMap == NULL) return FALSE;
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String bootMode;
    sysProp->Get(BOOT_MODE_PROPERTY, String("unknown"), &bootMode);
    HashMap<String, AutoPtr<StringPairList> >::Iterator it = mOemModeMap->Find(bootMode);
    if (it != mOemModeMap->End() && it->mSecond != NULL) {
        return TRUE;
    }
    return FALSE;
}

String UsbDeviceManager::ProcessOemUsbOverride(
    /* [in] */ const String& usbFunctions)
{
    if (usbFunctions.IsNull() || mOemModeMap == NULL) {
        return usbFunctions;
    }

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String bootMode;
    sysProp->Get(BOOT_MODE_PROPERTY, String("unknown"), &bootMode);

    AutoPtr<StringPairList> overrides;
    HashMap<String, AutoPtr<StringPairList> >::Iterator it = mOemModeMap->Find(bootMode);
    if (it != mOemModeMap->End() && it->mSecond != NULL) {
        overrides = it->mSecond;
        List<Pair<String, String> >::Iterator overridesIt = overrides->Begin();
        for (; overridesIt != overrides->End(); ++overridesIt) {
            Pair<String, String> pair = *overridesIt;
            if (pair.mFirst.Equals(usbFunctions)) {
                Slogger::D(TAG, "OEM USB override: %s ==> %s", pair.mFirst.string(), pair.mSecond.string());
                return pair.mSecond;
            }
        }
    }
    // return passed in functions as is.
    return usbFunctions;
}

void UsbDeviceManager::AllowUsbDebugging(
    /* [in] */ Boolean alwaysAllow,
    /* [in] */ const String& publicKey)
{
    if (mDebuggingManager != NULL) {
        mDebuggingManager->AllowUsbDebugging(alwaysAllow, publicKey);
    }
}

void UsbDeviceManager::DenyUsbDebugging()
{
    if (mDebuggingManager != NULL) {
        mDebuggingManager->DenyUsbDebugging();
    }
}

ECode UsbDeviceManager::ClearUsbDebuggingKeys()
{
    if (mDebuggingManager != NULL) {
        mDebuggingManager->ClearUsbDebuggingKeys();
    }
    else {
        Slogger::E(TAG, "Cannot clear Usb Debugging keys, UsbDebuggingManager not enabled");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

static void set_accessory_string(int fd, int cmd, ArrayOf<String>* strArray, int index)
{
    char buffer[256];

    buffer[0] = 0;
    ioctl(fd, cmd, buffer);
    if (buffer[0]) {
        (*strArray)[index] = String(buffer);
    }
}

AutoPtr< ArrayOf<String> > UsbDeviceManager::NativeGetAccessoryStrings()
{
    Int32 fd = open(DRIVER_NAME, O_RDWR);
    if (fd < 0) {
        Slogger::D(TAG, "could not open ", DRIVER_NAME.string());
        return NULL;
    }
    AutoPtr< ArrayOf<String> > strArray = ArrayOf<String>::Alloc(6);
    set_accessory_string(fd, ACCESSORY_GET_STRING_MANUFACTURER, strArray, 0);
    set_accessory_string(fd, ACCESSORY_GET_STRING_MODEL, strArray, 1);
    set_accessory_string(fd, ACCESSORY_GET_STRING_DESCRIPTION, strArray, 2);
    set_accessory_string(fd, ACCESSORY_GET_STRING_VERSION, strArray, 3);
    set_accessory_string(fd, ACCESSORY_GET_STRING_URI, strArray, 4);
    set_accessory_string(fd, ACCESSORY_GET_STRING_SERIAL, strArray, 5);
    close(fd);
    return strArray;
}

AutoPtr<IParcelFileDescriptor> UsbDeviceManager::NativeOpenAccessory()
{
    Int32 fd = open(DRIVER_NAME, O_RDWR);
    if (fd < 0) {
        Slogger::D(TAG, "could not open ", DRIVER_NAME.string());
        return NULL;
    }
    AutoPtr<IParcelFileDescriptor> descriptor;
    CParcelFileDescriptor::New((IParcelFileDescriptor*)fd, (IParcelFileDescriptor**)&descriptor);
    return descriptor;
}

Boolean UsbDeviceManager::NativeIsStartRequested()
{
    Int32 fd = open(DRIVER_NAME, O_RDWR);
    if (fd < 0) {
        Slogger::D(TAG, "could not open ", DRIVER_NAME.string());
        return FALSE;
    }
    Int32 result = ioctl(fd, ACCESSORY_IS_START_REQUESTED);
    close(fd);
    return (result == 1);

}

Int32 UsbDeviceManager::NativeGetAudioMode()
{
    Int32 fd = open(DRIVER_NAME, O_RDWR);
    if (fd < 0) {
        Slogger::D(TAG, "could not open ", DRIVER_NAME.string());
        return FALSE;
    }
    Int32 result = ioctl(fd, ACCESSORY_GET_AUDIO_MODE);
    close(fd);
    return result;
}

} // namespace Usb
} // namespace Server
} // namepsace Droid
} // namespace Elastos
