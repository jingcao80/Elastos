
#include "input/CInputManagerService.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/NativeMessageQueue.h"
#include "elastos/droid/view/NativeInputChannel.h"
#include "util/Xml.h"
#include "util/XmlUtils.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::CObjectContainer;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::IO::ICloseable;
using Elastos::IO::IReader;
using Elastos::IO::IStreams;
using Elastos::IO::CStreams;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::CFileReader;
using Elastos::IO::IFileReader;
using Elastos::IO::IInputStream;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::IInputStreamReader;
// using Elastos::Droid::App::CPendingIntentHelper;
// using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothAdapterHelper;
using Elastos::Droid::Bluetooth::CBluetoothAdapterHelper;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::NativeMessageQueue;
using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::IInputChannelHelper;
using Elastos::Droid::View::CInputChannelHelper;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IPointerIconHelper;
using Elastos::Droid::View::CPointerIconHelper;
using Elastos::Droid::View::NativeInputChannel;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::EIID_IInputFilterHost;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::XmlUtils;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Hardware::Input::CKeyboardLayout;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Server::Display::DisplayViewport;
using Elastos::Droid::Server::Display::EIID_DisplayViewport;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

const char* CInputManagerService::TAG = "InputManager";
const Boolean CInputManagerService::DEBUG ;
const String CInputManagerService::EXCLUDED_DEVICES_PATH("etc/excluded-input-devices.xml");

const Int32 CInputManagerService::MSG_DELIVER_INPUT_DEVICES_CHANGED;
const Int32 CInputManagerService::MSG_SWITCH_KEYBOARD_LAYOUT;
const Int32 CInputManagerService::MSG_RELOAD_KEYBOARD_LAYOUTS;
const Int32 CInputManagerService::MSG_UPDATE_KEYBOARD_LAYOUTS;
const Int32 CInputManagerService::MSG_RELOAD_DEVICE_ALIASES;

const Int32 CInputManagerService::INPUT_EVENT_INJECTION_SUCCEEDED;
const Int32 CInputManagerService::INPUT_EVENT_INJECTION_PERMISSION_DENIED;
const Int32 CInputManagerService::INPUT_EVENT_INJECTION_FAILED;
const Int32 CInputManagerService::INPUT_EVENT_INJECTION_TIMED_OUT;

const Int32 CInputManagerService::INJECTION_TIMEOUT_MILLIS;
const Int32 CInputManagerService::KEY_STATE_UNKNOWN;
const Int32 CInputManagerService::KEY_STATE_UP;
const Int32 CInputManagerService::KEY_STATE_DOWN;
const Int32 CInputManagerService::KEY_STATE_VIRTUAL;
const Int32 CInputManagerService::_BTN_MOUSE;
const Int32 CInputManagerService::_SW_LID;
const Int32 CInputManagerService::_SW_KEYPAD_SLIDE;
const Int32 CInputManagerService::_SW_HEADPHONE_INSERT;
const Int32 CInputManagerService::_SW_MICROPHONE_INSERT;
const Int32 CInputManagerService::_SW_JACK_PHYSICAL_INSERT;
const Int32 CInputManagerService::SW_LID_BIT;
const Int32 CInputManagerService::SW_KEYPAD_SLIDE_BIT;
const Int32 CInputManagerService::SW_HEADPHONE_INSERT_BIT ;
const Int32 CInputManagerService::SW_MICROPHONE_INSERT_BIT;
const Int32 CInputManagerService::SW_JACK_PHYSICAL_INSERT_BIT;
const Int32 CInputManagerService::SW_JACK_BITS;

ECode CInputManagerService::InputManagerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case CInputManagerService::MSG_DELIVER_INPUT_DEVICES_CHANGED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IObjectContainer* container = IObjectContainer::Probe(obj);
            AutoPtr<ArrayOf<IInputDevice*> > array;
            if (container) {
                Int32 count;
                container->GetObjectCount(&count);
                array = ArrayOf<IInputDevice*>::Alloc(count);
                AutoPtr<IObjectEnumerator> emu;
                container->GetObjectEnumerator((IObjectEnumerator**)&emu);
                Boolean hasNext;
                Int32 i = 0;
                while (emu->MoveNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> itemObj;
                    emu->Current((IInterface**)&itemObj);
                    IInputDevice* id = IInputDevice::Probe(itemObj);
                    array->Set(i++, id);
                }
            }
            if (array == NULL) {
                array = ArrayOf<IInputDevice*>::Alloc(0);
            }

            mHost->DeliverInputDevicesChanged(array);
            break;
        }
        case CInputManagerService::MSG_SWITCH_KEYBOARD_LAYOUT: {
            Int32 arg1, arg2;
            msg->GetArg1(&arg1);
            msg->GetArg2(&arg2);
            mHost->HandleSwitchKeyboardLayout(arg1, arg2);
            break;
        }
        case CInputManagerService::MSG_RELOAD_KEYBOARD_LAYOUTS:
            mHost->ReloadKeyboardLayouts();
            break;
        case CInputManagerService::MSG_UPDATE_KEYBOARD_LAYOUTS:
            mHost->UpdateKeyboardLayouts();
            break;
        case CInputManagerService::MSG_RELOAD_DEVICE_ALIASES:
            mHost->ReloadDeviceAliases();
            break;
    }

    return NOERROR;
}

CInputManagerService::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CInputManagerService* owner)
    : mOwner(owner)
{}

//@Override
ECode CInputManagerService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mOwner->UpdatePointerSpeedFromSettings();
    mOwner->UpdateShowTouchesFromSettings();

    return NOERROR;
}

CInputManagerService::MyBroadcastReceiverEx::MyBroadcastReceiver(
    /* [in] */ CInputManagerService* owner)
    : mOwner(owner)
{}

//@Override
ECode CInputManagerService::MyBroadcastReceiverEx::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mOwner->UpdateKeyboardLayouts();

    return NOERROR;
}

CInputManagerService::MyBroadcastReceiverEx2::MyBroadcastReceiver(
    /* [in] */ CInputManagerService* owner)
    : mOwner(owner)
{}

//@Override
ECode CInputManagerService::MyBroadcastReceiverEx2::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mOwner->ReloadDeviceAliases();

    return NOERROR;
}

CInputManagerService::MyKeyboardLayoutVisitor::MyKeyboardLayoutVisitor(
    /* [in] */ HashSet<String>& availableKeyboardLayouts)
    : mAvailableKeyboardLayouts(availableKeyboardLayouts)
{}

void CInputManagerService::MyKeyboardLayoutVisitor::VisitKeyboardLayout(
    /* [in] */ IResources* resources,
    /* [in] */ const String& descriptor,
    /* [in] */ const String& label,
    /* [in] */ const String& collection,
    /* [in] */ Int32 keyboardLayoutResId)
{
    mAvailableKeyboardLayouts.Insert(descriptor);
}

CInputManagerService::MyKeyboardLayoutVisitorEx::MyKeyboardLayoutVisitor(
    /* [in] */ List<AutoPtr<IKeyboardLayout> >& list)
    : mList(list)
{}

void CInputManagerService::MyKeyboardLayoutVisitorEx::VisitKeyboardLayout(
    /* [in] */ IResources* resources,
    /* [in] */ const String& descriptor,
    /* [in] */ const String& label,
    /* [in] */ const String& collection,
    /* [in] */ Int32 keyboardLayoutResId)
{
    AutoPtr<IKeyboardLayout> layout;
    CKeyboardLayout::New(descriptor, label, collection, (IKeyboardLayout**)&layout);
    mList.PushBack(layout);
}

CInputManagerService::MyKeyboardLayoutVisitorEx2::MyKeyboardLayoutVisitor(
    /* [in] */ IKeyboardLayout** layout)
    : mLayout(layout)
{}

void CInputManagerService::MyKeyboardLayoutVisitorEx2::VisitKeyboardLayout(
    /* [in] */ IResources* resources,
    /* [in] */ const String& descriptor,
    /* [in] */ const String& label,
    /* [in] */ const String& collection,
    /* [in] */ Int32 keyboardLayoutResId)
{
    assert(mLayout);
    CKeyboardLayout::New(descriptor, label, collection, mLayout);
}

CInputManagerService::MyContentObserver::MyContentObserver(
    /* [in] */ CInputManagerService* owner,
    /* [in] */ IHandler* handler)
    : ContentObserver(handler)
    , mOwner(owner)
{}

ECode CInputManagerService::MyContentObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    mOwner->UpdatePointerSpeedFromSettings();
    return NOERROR;
}

CInputManagerService::MyContentObserverEx::MyContentObserver(
    /* [in] */ CInputManagerService* owner,
    /* [in] */ IHandler* handler)
    : ContentObserver(handler)
    , mOwner(owner)
{}

ECode CInputManagerService::MyContentObserverEx::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    mOwner->UpdateShowTouchesFromSettings();
    return NOERROR;
}

CInputManagerService::MyKeyboardLayoutVisitorEx3::MyKeyboardLayoutVisitor(
    /* [in] */ ArrayOf<String>* layouts)
{
    mLayouts = layouts;
}

void CInputManagerService::MyKeyboardLayoutVisitorEx3::VisitKeyboardLayout(
    /* [in] */ IResources* resources,
    /* [in] */ const String& descriptor,
    /* [in] */ const String& label,
    /* [in] */ const String& collection,
    /* [in] */ Int32 keyboardLayoutResId)
{
    (*mLayouts)[0] = descriptor;

    AutoPtr<IInputStream> inputStream;
    resources->OpenRawResource(keyboardLayoutResId, (IInputStream**)&inputStream);
    AutoPtr<IInputStreamReader> reader;
    CInputStreamReader::New(inputStream, (IInputStreamReader**)&reader);
    AutoPtr<IStreams> stream;
    CStreams::AcquireSingleton((IStreams**)&stream);
    stream->ReadFully(reader, &((*mLayouts)[1]));
}

CAR_INTERFACE_IMPL(CInputManagerService::InputFilterHost, IInputFilterHost);

CInputManagerService::InputFilterHost::InputFilterHost(
    /* [in] */ CInputManagerService* owner)
    : mOwner(owner)
    , mDisconnected(FALSE)
{
}

void CInputManagerService::InputFilterHost::DisconnectLocked()
{
    mDisconnected = TRUE;
}

//@Override
ECode CInputManagerService::InputFilterHost::SendInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 policyFlags)
{
    if (event == NULL) {
        Slogger::E(TAG, "event must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(&mOwner->mInputFilterLock);
    if (!mDisconnected) {
        mOwner->NativeInjectInputEvent(event, 0, 0,
            IInputManager::INJECT_INPUT_EVENT_MODE_ASYNC, 0,
            policyFlags | IWindowManagerPolicy::FLAG_FILTERED);
    }

    return NOERROR;
}

String CInputManagerService::KeyboardLayoutDescriptor::Format(
    /* [in] */ const String& packageName,
    /* [in] */ const String& receiverName,
    /* [in] */ const String& keyboardName)
{
    return packageName + "/" + receiverName + "/" + keyboardName;
}

AutoPtr<CInputManagerService::KeyboardLayoutDescriptor> CInputManagerService::KeyboardLayoutDescriptor::Parse(
    /* [in] */ const String& descriptor)
{
    Int32 pos = descriptor.IndexOf('/');
    if (pos < 0 || UInt32(pos + 1) == descriptor.GetLength()) {
        return NULL;
    }
    Int32 pos2 = descriptor.IndexOf('/', pos + 1);
    if (pos2 < pos + 2 || UInt32(pos2 + 1) == descriptor.GetLength()) {
        return NULL;
    }

    AutoPtr<KeyboardLayoutDescriptor> result = new KeyboardLayoutDescriptor();
    result->mPackageName = descriptor.Substring(0, pos);
    result->mReceiverName = descriptor.Substring(pos + 1, pos2);
    result->mKeyboardLayoutName = descriptor.Substring(pos2 + 1);

    return result;
}

CAR_INTERFACE_IMPL(
    CInputManagerService::InputDevicesChangedListenerRecord, IProxyDeathRecipient);

CInputManagerService::InputDevicesChangedListenerRecord::InputDevicesChangedListenerRecord(
    /* [in] */ CInputManagerService* owner,
    /* [in] */ Int32 pid,
    /* [in] */ IInputDevicesChangedListener* listener)
    : mOwner(owner)
    , mPid(pid)
    , mListener(listener)
{
}

//@Override
ECode CInputManagerService::InputDevicesChangedListenerRecord::ProxyDied()
{
    PFL_EX("ProxyDied()");
    if (DEBUG) {
        Slogger::D(TAG, "Input devices changed listener for pid %d died.", mPid);
    }
    mOwner->OnInputDevicesChangedListenerDied(mPid);

    return NOERROR;
}

void CInputManagerService::InputDevicesChangedListenerRecord::NotifyInputDevicesChanged(
    /* [in] */ ArrayOf<Int32>* info)
{
    if (FAILED(mListener->OnInputDevicesChanged(info))) {
        Slogger::W(TAG, "Failed to notify process %d that input devices changed,"
            " assuming it died.", mPid);
        ProxyDied();
    }
}

CAR_INTERFACE_IMPL(CInputManagerService::VibratorToken, IProxyDeathRecipient);

CInputManagerService::VibratorToken::VibratorToken(
    /* [in] */ CInputManagerService* owner,
    /* [in] */ Int32 deviceId,
    /* [in] */ IBinder* token,
    /* [in] */ Int32 tokenValue)
    : mOwner(owner)
    , mDeviceId(deviceId)
    , mToken(token)
    , mTokenValue(tokenValue)
{
}

//@Override
ECode CInputManagerService::VibratorToken::ProxyDied()
{
    PFL_EX("ProxyDied()");
    if (DEBUG) {
        Slogger::D(TAG, "Vibrator token died.");
    }
    mOwner->OnVibratorTokenDied(this);

    return NOERROR;
}

CInputManagerService::CInputManagerService()
    : mUseDevInputEventForAudioJack(FALSE)
    , mWindowManagerCallbacks(NULL)
    , mWiredAccessoryCallbacks(NULL)
    , mSystemReady(FALSE)
    , mInputDevicesChangedPending(FALSE)
    , mKeyboardLayoutNotificationShown(FALSE)
    , mNextVibratorTokenValue(0)
{
    mDataStore = new PersistentDataStore();
    mInputDevices = ArrayOf<IInputDevice*>::Alloc(0);
}

ECode CInputManagerService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler)
{
    mContext = context;
    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);
    mHandler = new InputManagerHandler(looper, this);

    AutoPtr<IResources> resources;
    ASSERT_SUCCEEDED(context->GetResources((IResources**)&resources));
    ASSERT_SUCCEEDED(resources->GetBoolean(R::bool_::config_useDevInputEventForAudioJack,
        &mUseDevInputEventForAudioJack));

    Slogger::I(TAG, "Initializing input manager, mUseDevInputEventForAudioJack=%d",
        mUseDevInputEventForAudioJack);

    NativeInit();
    return NOERROR;
}

void CInputManagerService::SetWindowManagerCallbacks(
    /* [in] */ WindowManagerCallbacks* callbacks)
{
    mWindowManagerCallbacks = callbacks;
}

void CInputManagerService::SetWiredAccessoryCallbacks(
    /* [in] */ WiredAccessoryCallbacks* callbacks)
{
    mWiredAccessoryCallbacks = callbacks;
}

ECode CInputManagerService::Start()
{
    Slogger::I(TAG, "Starting input manager");
    FAIL_RETURN(NativeStart());

    // Add ourself to the Watchdog monitors.
    // AutoPtr<IWatchdog> watchdog;
    // CWatchdog::AcquireSingleton((IWatchdog**)&watchdog);
    // watchdog->AddMonitor(IMonitor::Probe(this));

    RegisterPointerSpeedSettingObserver();
    RegisterShowTouchesSettingObserver();

    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IIntent::ACTION_USER_SWITCHED, (IIntentFilter**)&intentFilter);
    AutoPtr<MyBroadcastReceiver> receiver = new MyBroadcastReceiver(this);
    AutoPtr<IIntent> result;
    mContext->RegisterReceiver(
        receiver, intentFilter, String(NULL),
        mHandler, (IIntent**)&result);

    UpdatePointerSpeedFromSettings();
    UpdateShowTouchesFromSettings();

    return NOERROR;
}

void CInputManagerService::SystemReady()
{
    if (DEBUG) {
        Slogger::D(TAG, "System ready.");
    }
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&service);
    mNotificationManager = INotificationManager::Probe(service);
    mSystemReady = TRUE;

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_PACKAGE_ADDED, (IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
    filter->AddDataScheme(String("package"));
    AutoPtr<MyBroadcastReceiverEx> myBREx = new MyBroadcastReceiver(this);
    AutoPtr<IIntent> result;
    mContext->RegisterReceiver(
        myBREx.Get(), filter, String(NULL), mHandler, (IIntent**)&result);

    filter = NULL;
    result = NULL;
    CIntentFilter::New(IBluetoothDevice::ACTION_ALIAS_CHANGED, (IIntentFilter**)&filter);
    AutoPtr<MyBroadcastReceiverEx2> myBREx2 = new MyBroadcastReceiver(this);
    mContext->RegisterReceiver(
        myBREx2, filter,String(NULL), mHandler, (IIntent**)&result);

    Boolean bval;
    mHandler->SendEmptyMessage(MSG_RELOAD_DEVICE_ALIASES, &bval);
    mHandler->SendEmptyMessage(MSG_UPDATE_KEYBOARD_LAYOUTS, &bval);
}

void CInputManagerService::ReloadKeyboardLayouts()
{
    if (DEBUG) {
        Slogger::D(TAG, "Reloading keyboard layouts.");
    }
    NativeReloadKeyboardLayouts();
}

void CInputManagerService::ReloadDeviceAliases()
{
    if (DEBUG) {
        Slogger::D(TAG, "Reloading device names.");
    }
    NativeReloadDeviceAliases();
}

ECode CInputManagerService::SetDisplayViewports(
    /* [in] */ IDisplayViewport* defaultViewport,
    /* [in] */ IDisplayViewport* externalTouchViewport)
{
    AutoPtr<DisplayViewport> dvp;
    if (defaultViewport)
        dvp = reinterpret_cast<DisplayViewport*>(defaultViewport->Probe(EIID_DisplayViewport));

    AutoPtr<DisplayViewport> etvp;
    if (externalTouchViewport)
        etvp = reinterpret_cast<DisplayViewport*>(externalTouchViewport->Probe(EIID_DisplayViewport));

    if (dvp && dvp->mValid) {
        SetDisplayViewport(FALSE, dvp);
    }

    if (etvp && etvp->mValid) {
        SetDisplayViewport(TRUE, etvp);
    }
    else if (dvp && dvp->mValid) {
        SetDisplayViewport(TRUE, dvp);
    }

    return NOERROR;
}

void CInputManagerService::SetDisplayViewport(
    /* [in] */ Boolean external,
    /* [in] */ DisplayViewport* viewport)
{
    Int32 logicalLeft, logicalTop, logicalRight, logicalBottom;
    viewport->mLogicalFrame->GetLeft(&logicalLeft);
    viewport->mLogicalFrame->GetTop(&logicalTop);
    viewport->mLogicalFrame->GetRight(&logicalRight);
    viewport->mLogicalFrame->GetBottom(&logicalBottom);

    Int32 physicalLeft, physicalTop, physicalRight, physicalBottom;
    viewport->mPhysicalFrame->GetLeft(&physicalLeft);
    viewport->mPhysicalFrame->GetTop(&physicalTop);
    viewport->mPhysicalFrame->GetRight(&physicalRight);
    viewport->mPhysicalFrame->GetBottom(&physicalBottom);

    NativeSetDisplayViewport(external,
        viewport->mDisplayId, viewport->mOrientation,
        logicalLeft, logicalTop, logicalRight, logicalBottom,
        physicalLeft, physicalTop, physicalRight, physicalBottom,
        viewport->mDeviceWidth, viewport->mDeviceHeight);
}

Int32 CInputManagerService::GetKeyCodeState(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ Int32 keyCode)
{
    return NativeGetKeyCodeState(deviceId, sourceMask, keyCode);
}

Int32 CInputManagerService::GetScanCodeState(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ Int32 scanCode)
{
    return NativeGetScanCodeState(deviceId, sourceMask, scanCode);
}

Int32 CInputManagerService::GetSwitchState(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ Int32 switchCode)
{
    return NativeGetSwitchState(deviceId, sourceMask, switchCode);
}

ECode CInputManagerService::HasKeys(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ const ArrayOf<Int32>& keyCodes,
    /* [out] */ ArrayOf<Boolean>* keyExists,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    if (keyCodes.GetLength() <= 0) {
        Slogger::E(TAG, "keyCodes must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (keyExists == NULL || keyExists->GetLength() < keyCodes.GetLength()) {
        Slogger::E(TAG, "keyExists must not be NULL and must be at "
            "least as large as keyCodes.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *res = NativeHasKeys(deviceId, sourceMask, keyCodes, keyExists);
    return NOERROR;
}

ECode CInputManagerService::MonitorInput(
    /* [in] */ const String& inputChannelName,
    /* [out] */ IInputChannel** inputChannel)
{
    VALIDATE_NOT_NULL(inputChannel);
    *inputChannel = NULL;

    if (inputChannelName.IsNull()) {
        Slogger::E(TAG, "inputChannelName must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInputChannelHelper> icHelper;
    CInputChannelHelper::AcquireSingleton((IInputChannelHelper**)&icHelper);
    AutoPtr<IInputChannel> inputChannel0;
    AutoPtr<IInputChannel> inputChannel1;
    FAIL_RETURN(icHelper->OpenInputChannelPair(
        inputChannelName, (IInputChannel**)&inputChannel0, (IInputChannel**)&inputChannel1));
    FAIL_RETURN(NativeRegisterInputChannel(inputChannel0, NULL, TRUE));
    FAIL_RETURN(inputChannel0->Dispose()); // don't need to retain the Java object reference
    *inputChannel = inputChannel1;
    REFCOUNT_ADD(*inputChannel);

    return NOERROR;
}

ECode CInputManagerService::RegisterInputChannel(
    /* [in] */ IInputChannel* inputChannel,
    /* [in] */ InputWindowHandle* inputWindowHandle)
{
    if (inputChannel == NULL) {
        Slogger::E(TAG, "inputChannel must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NativeRegisterInputChannel(inputChannel, inputWindowHandle, FALSE);
}

ECode CInputManagerService::UnregisterInputChannel(
    /* [in] */ IInputChannel* inputChannel)
{
    if (inputChannel == NULL) {
        Slogger::E(TAG, "inputChannel must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NativeUnregisterInputChannel(inputChannel);
}

void CInputManagerService::SetInputFilter(
    /* [in] */ IIInputFilter* filter)
{
    AutoLock lock(&mInputFilterLock);

    AutoPtr<IIInputFilter> oldFilter = mInputFilter;
    if (oldFilter.Get() == filter) {
        return; // nothing to do
    }

    if (oldFilter != NULL) {
        mInputFilter = NULL;
        mInputFilterHost->DisconnectLocked();
        mInputFilterHost = NULL;
        oldFilter->Uninstall();
    }

    if (filter != NULL) {
        mInputFilter = filter;
        mInputFilterHost = new InputFilterHost(this);
        filter->Install(mInputFilterHost);
    }

    NativeSetInputFilterEnabled(filter != NULL);
}

ECode CInputManagerService::InjectInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 mode,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    if (event == NULL) {
        Slogger::E(TAG, "event must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mode != IInputManager::INJECT_INPUT_EVENT_MODE_ASYNC
        && mode != IInputManager::INJECT_INPUT_EVENT_MODE_WAIT_FOR_FINISH
        && mode != IInputManager::INJECT_INPUT_EVENT_MODE_WAIT_FOR_RESULT) {
        Slogger::E(TAG, "mode is invalid");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 pid = Binder::GetCallingPid();
    Int32 uid = Binder::GetCallingUid();
    Int64 ident = Binder::ClearCallingIdentity();
    Int32 result;

    result = NativeInjectInputEvent(event, pid, uid, mode,
            INJECTION_TIMEOUT_MILLIS, IWindowManagerPolicy::FLAG_DISABLE_KEY_REPEAT);

    Binder::RestoreCallingIdentity(ident);

    switch (result) {
        case INPUT_EVENT_INJECTION_PERMISSION_DENIED:
            Slogger::W(TAG, "Input event injection from pid %d permission denied.", pid);
            Slogger::E(TAG, "Injecting to another application requires INJECT_EVENTS permission");

            return E_SECURITY_EXCEPTION;

        case INPUT_EVENT_INJECTION_SUCCEEDED:
            *res = TRUE;
            break;

        case INPUT_EVENT_INJECTION_TIMED_OUT:
            Slogger::W(TAG, "Input event injection from pid %d timed out.", pid);
            *res = FALSE;
            break;

        case INPUT_EVENT_INJECTION_FAILED:
        default:
            Slogger::W(TAG, "Input event injection from pid %d failed", pid);
            *res = FALSE;
            break;
    }

    return NOERROR;
}

ECode CInputManagerService::GetInputDevice(
    /* [in] */ Int32 deviceId,
    /* [out] */ IInputDevice** inputDevice)
{
    VALIDATE_NOT_NULL(inputDevice);
    *inputDevice = NULL;

    AutoLock lock(mInputDevicesLock);

    Int32 count = mInputDevices->GetLength();
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInputDevice> device = (*mInputDevices)[i];
        Int32 id;
        device->GetId(&id);
        if (id == deviceId) {
            *inputDevice = device;
            REFCOUNT_ADD(*inputDevice);

            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CInputManagerService::GetInputDeviceIds(
    /* [out, callee] */ ArrayOf<Int32>** deviceIds)
{
    VALIDATE_NOT_NULL(deviceIds);

    AutoLock lock(mInputDevicesLock);

    Int32 count = mInputDevices->GetLength();
    *deviceIds = ArrayOf<Int32>::Alloc(count);
    if (*deviceIds == NULL) return E_OUT_OF_MEMORY_ERROR;

    REFCOUNT_ADD(*deviceIds);
    for (Int32 i = 0; i < count; i++) {
        Int32 id;
        (*mInputDevices)[i]->GetId(&id);
        (**deviceIds)[i] = id;
    }

    return NOERROR;
}

AutoPtr<ArrayOf<IInputDevice*> > CInputManagerService::GetInputDevices()
{
    AutoLock lock(mInputDevicesLock);

    return mInputDevices;
}

ECode CInputManagerService::RegisterInputDevicesChangedListener(
    /* [in] */ IInputDevicesChangedListener* listener)
{
    if (listener == NULL) {
        Slogger::E(TAG, "listener must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(mInputDevicesLock);

    Int32 callingPid = Binder::GetCallingPid();

    if (mInputDevicesChangedListeners.Find(callingPid)
        != mInputDevicesChangedListeners.End()) {
        Slogger::E(TAG, "The calling process has already "
            "registered an InputDevicesChangedListener.");
        return E_SECURITY_EXCEPTION;
    }

    AutoPtr<InputDevicesChangedListenerRecord> record =
        new InputDevicesChangedListenerRecord(this, callingPid, listener);

    AutoPtr<IProxy> proxy = (IProxy*)listener->Probe(EIID_IProxy);
    if (proxy != NULL && FAILED(proxy->LinkToDeath(record, 0))) {
        // give up
        return E_RUNTIME_EXCEPTION;
    }

    mInputDevicesChangedListeners[callingPid] = record;

    return NOERROR;
}

void CInputManagerService::OnInputDevicesChangedListenerDied(
    /* [in] */ Int32 pid)
{
    AutoLock lock(mInputDevicesLock);

    mInputDevicesChangedListeners.Erase(pid);
}

void CInputManagerService::DeliverInputDevicesChanged(
    /* [in] */ ArrayOf<IInputDevice*>* oldInputDevices)
{
    // Scan for changes.
    Int32 numFullKeyboardsAdded = 0;
    mTempInputDevicesChangedListenersToNotify.Clear();
    mTempFullKeyboards.Clear();
    AutoPtr<ArrayOf<Int32> > deviceIdAndGeneration;
    {
        AutoLock lock(mInputDevicesLock);

        if (!mInputDevicesChangedPending) {
            return;
        }
        mInputDevicesChangedPending = FALSE;

        HashMap<Int32, AutoPtr<InputDevicesChangedListenerRecord> >::Iterator iter
            = mInputDevicesChangedListeners.Begin();
        for (; iter != mInputDevicesChangedListeners.End(); ++iter) {
            mTempInputDevicesChangedListenersToNotify.PushBack(iter->mSecond);
        }

        Int32 numDevices = mInputDevices->GetLength();
        deviceIdAndGeneration = ArrayOf<Int32>::Alloc(numDevices * 2);
        for (Int32 i = 0; i < numDevices; i++) {
            AutoPtr<IInputDevice> inputDevice = (*mInputDevices)[i];
            Int32 temp;
            inputDevice->GetId(&temp);
            (*deviceIdAndGeneration)[i * 2] = temp;
            inputDevice->GetGeneration(&temp);
            (*deviceIdAndGeneration)[i * 2 + 1] = temp;

            Boolean isVirtual, isFullKeyboard;
            if (!(inputDevice->IsVirtual(&isVirtual), isVirtual)
                && (inputDevice->IsFullKeyboard(&isFullKeyboard), isFullKeyboard)) {
                String descriptor;
                inputDevice->GetDescriptor(&descriptor);
                if (!ContainsInputDeviceWithDescriptor(oldInputDevices, descriptor)) {
                    mTempFullKeyboards.Insert(numFullKeyboardsAdded, inputDevice);
                    numFullKeyboardsAdded++;
                }
                else {
                    mTempFullKeyboards.PushBack(inputDevice);
                }
            }
        }
    }

    // Notify listeners.
    List<AutoPtr<InputDevicesChangedListenerRecord> >::Iterator iter
        = mTempInputDevicesChangedListenersToNotify.Begin();
    for (; iter != mTempInputDevicesChangedListenersToNotify.End(); ++iter) {
        (*iter)->NotifyInputDevicesChanged(deviceIdAndGeneration);
    }

    mTempInputDevicesChangedListenersToNotify.Clear();

    // Check for missing keyboard layouts.
    if (mNotificationManager != NULL) {
        Boolean missingLayoutForExternalKeyboard = FALSE;
        Boolean missingLayoutForExternalKeyboardAdded = FALSE;
        {
            AutoLock lock(&mDataStoreLock);
            List<AutoPtr<IInputDevice> >::Iterator iter = mTempFullKeyboards.Begin();
            for (Int32 i = 0; iter != mTempFullKeyboards.End(); ++iter, i++) {
                String descriptor;
                (*iter)->GetDescriptor(&descriptor);
                if (mDataStore->GetCurrentKeyboardLayout(descriptor).IsNull()) {
                    missingLayoutForExternalKeyboard = TRUE;
                    if (i < numFullKeyboardsAdded) {
                        missingLayoutForExternalKeyboardAdded = TRUE;
                    }
                }
            }
        }

        if (missingLayoutForExternalKeyboard) {
            if (missingLayoutForExternalKeyboardAdded) {
                ShowMissingKeyboardLayoutNotification();
            }
        }
        else if (mKeyboardLayoutNotificationShown) {
            HideMissingKeyboardLayoutNotification();
        }
    }
    mTempFullKeyboards.Clear();
}

void CInputManagerService::ShowMissingKeyboardLayoutNotification()
{
    if (!mKeyboardLayoutNotificationShown) {
        if (mKeyboardLayoutIntent == NULL) {
            AutoPtr<IIntent> intent;
            CIntent::New(String("android.settings.INPUT_METHOD_SETTINGS"), (IIntent**)&intent);
            intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                    | IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED
                    | IIntent::FLAG_ACTIVITY_CLEAR_TOP);

            AutoPtr<IPendingIntentHelper> piHelper;
            CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);
            piHelper->GetActivityAsUser(
                mContext, 0, intent, 0, NULL, UserHandle::CURRENT,
                (IPendingIntent**)&mKeyboardLayoutIntent);

        }

        AutoPtr<IResources> r;
        mContext->GetResources((IResources**)&r);
        AutoPtr<INotificationBuilder> builder;
        CNotificationBuilder::New(mContext, (INotificationBuilder**)&builder);
        String str;
        r->GetString(R::string::select_keyboard_layout_notification_title, &str);
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        builder->SetContentTitle(cs);
        r->GetString(R::string::select_keyboard_layout_notification_message, &str);
        cs = NULL;
        CString::New(str, (ICharSequence**)&cs);
        builder->SetContentText(cs);
        builder->SetContentIntent(mKeyboardLayoutIntent);
        builder->SetSmallIcon(R::drawable::ic_settings_language);
        builder->SetPriority(INotification::PRIORITY_LOW);
        AutoPtr<INotification> notification;
        builder->Build((INotification**)&notification);
        mNotificationManager->NotifyAsUser(String(NULL),
            R::string::select_keyboard_layout_notification_title,
            notification, UserHandle::ALL);

        mKeyboardLayoutNotificationShown = TRUE;
    }
}

void CInputManagerService::HideMissingKeyboardLayoutNotification()
{
    if (mKeyboardLayoutNotificationShown) {
        mKeyboardLayoutNotificationShown = FALSE;
        mNotificationManager->CancelAsUser(String(NULL),
            R::string::select_keyboard_layout_notification_title,
            UserHandle::ALL);
    }
}

void CInputManagerService::UpdateKeyboardLayouts()
{
    // Scan all input devices state for keyboard layouts that have been uninstalled.
    HashSet<String> availableKeyboardLayouts;
    AutoPtr<MyKeyboardLayoutVisitor> myKV = new MyKeyboardLayoutVisitor(availableKeyboardLayouts);
    VisitAllKeyboardLayouts(myKV);
    {
        AutoLock lock(&mDataStoreLock);
        mDataStore->RemoveUninstalledKeyboardLayouts(availableKeyboardLayouts);
        mDataStore->SaveIfNeeded();
    }

    // Reload keyboard layouts.
    ReloadKeyboardLayouts();
}

Boolean CInputManagerService::ContainsInputDeviceWithDescriptor(
    /* [in] */ ArrayOf<IInputDevice*>* inputDevices,
    /* [in] */ const String& descriptor)
{
    Int32 numDevices = inputDevices->GetLength();
    for (Int32 i = 0; i < numDevices; i++) {
        String descriptor2;
        (*inputDevices)[i]->GetDescriptor(&descriptor2);
        if (descriptor2.Equals(descriptor)) {
            return TRUE;
        }
    }
    return FALSE;
}

ECode CInputManagerService::GetKeyboardLayouts(
    /* [out, callee] */ ArrayOf<IKeyboardLayout*>** layouts)
{
    VALIDATE_NOT_NULL(layouts);

    List<AutoPtr<IKeyboardLayout> > list;
    AutoPtr<MyKeyboardLayoutVisitorEx> myKVEx = new MyKeyboardLayoutVisitor(list);
    VisitAllKeyboardLayouts(myKVEx);

    *layouts = ArrayOf<IKeyboardLayout*>::Alloc(list.GetSize());
    if (*layouts == NULL)
        return E_OUT_OF_MEMORY_ERROR;

    REFCOUNT_ADD(*layouts);

    List<AutoPtr<IKeyboardLayout> >::Iterator iter = list.Begin();
    for (Int32 i = 0; iter != list.End(); ++iter, ++i) {
        (*layouts)->Set(i, iter->Get());
    }

    return NOERROR;
}

ECode CInputManagerService::GetKeyboardLayout(
    /* [in] */ const String& keyboardLayoutDescriptor,
    /* [out] */ IKeyboardLayout** layout)
{
    VALIDATE_NOT_NULL(layout);
    *layout = NULL;

    if (keyboardLayoutDescriptor.IsNull()) {
        Slogger::E(TAG, "keyboardLayoutDescriptor must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<MyKeyboardLayoutVisitorEx2> myKVEx2 = new MyKeyboardLayoutVisitor(layout);
    VisitKeyboardLayout(keyboardLayoutDescriptor, myKVEx2);
    if (*layout == NULL) {
        Logger::W(TAG, "Could not get keyboard layout with descriptor '%d'.",
            keyboardLayoutDescriptor.string());
    }

    return NOERROR;
}

void CInputManagerService::VisitAllKeyboardLayouts(
    /* [in] */ KeyboardLayoutVisitor* visitor)
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IIntent> intent;
    CIntent::New(IInputManager::ACTION_QUERY_KEYBOARD_LAYOUTS, (IIntent**)&intent);

    AutoPtr<IObjectContainer> listcontainer;
    pm->QueryBroadcastReceivers(
        intent, IPackageManager::GET_META_DATA, (IObjectContainer**)&listcontainer);
    AutoPtr<IObjectEnumerator> enumerator;
    listcontainer->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    Boolean hasNext = FALSE;
    while (enumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IResolveInfo> resolveInfo;
        enumerator->Current((IInterface**)&resolveInfo);

        AutoPtr<IActivityInfo> activityInfo;
        resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        VisitKeyboardLayoutsInPackage(pm, activityInfo, String(NULL), visitor);
    }
}

void CInputManagerService::VisitKeyboardLayout(
    /* [in] */ const String& keyboardLayoutDescriptor,
    /* [in] */ KeyboardLayoutVisitor* visitor)
{
    AutoPtr<KeyboardLayoutDescriptor> d =
        KeyboardLayoutDescriptor::Parse(keyboardLayoutDescriptor);
    if (d != NULL) {
        AutoPtr<IPackageManager> pm;
        mContext->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IComponentName> componentName;
        CComponentName::New(
            d->mPackageName, d->mReceiverName, (IComponentName**)&componentName);
        AutoPtr<IActivityInfo> receiver;
        pm->GetReceiverInfo(
            componentName, IPackageManager::GET_META_DATA, (IActivityInfo**)&receiver);
        VisitKeyboardLayoutsInPackage(pm, receiver, d->mKeyboardLayoutName, visitor);
    }
}

void CInputManagerService::VisitKeyboardLayoutsInPackage(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IActivityInfo* receiver,
    /* [in] */ const String& keyboardName,
    /* [in] */ KeyboardLayoutVisitor* visitor)
{
    AutoPtr<IBundle> metaData;
    receiver->GetMetaData((IBundle**)&metaData);
    if (metaData == NULL) {
        return;
    }

    String packageName, receiverName;
    receiver->GetPackageName(&packageName);
    receiver->GetName(&receiverName);

    Int32 configResId;
    metaData->GetInt32(IInputManager::META_DATA_KEYBOARD_LAYOUTS, &configResId);
    if (configResId == 0) {
        Logger::W(TAG, "Missing meta-data '%s' on receiver %s/%s",
            IInputManager::META_DATA_KEYBOARD_LAYOUTS.string(),
            packageName.string(), receiverName.string());
        return;
    }

    AutoPtr<ICharSequence> receiverLabel;
    receiver->LoadLabel(pm, (ICharSequence**)&receiverLabel);
    String collection("");
    if (receiverLabel != NULL) {
        receiverLabel->ToString(&collection);
    }

    ECode ec;
    AutoPtr<IApplicationInfo> appInfo;
    AutoPtr<IResources> resources;
    AutoPtr<IXmlResourceParser> parser;

    ec = receiver->GetApplicationInfo((IApplicationInfo**)&appInfo);
    if (FAILED(ec))
        goto _Exit3_;

    ec = pm->GetResourcesForApplication(appInfo, (IResources**)&resources);
    if (FAILED(ec))
        goto _Exit3_;

    ec = resources->GetXml(configResId, (IXmlResourceParser**)&parser);
    if (FAILED(ec))
        goto _Exit3_;

    ec = XmlUtils::BeginDocument(parser, String("keyboard-layouts"));
    if (FAILED(ec))
        goto _Exit2_;

    for (;;) {
        ec = XmlUtils::NextElement(parser);
        if (FAILED(ec))
            goto _Exit2_;

        String element;
        ec = parser->GetName(&element);
        if (FAILED(ec))
            goto _Exit2_;

        if (element.IsNull()) {
            break;
        }
        if (element.Equals("keyboard-layout")) {
            String name;
            String label;
            Int32 keyboardLayoutResId;

            Int32 size = ArraySize(R::styleable::KeyboardLayout);
            AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
            layout->Copy(R::styleable::KeyboardLayout, size);

            AutoPtr<ITypedArray> a;
            ec = resources->ObtainAttributes(
                IAttributeSet::Probe(parser), layout, (ITypedArray**)&a);

            if (FAILED(ec))
                goto _Exit2_;

            ec = a->GetString(R::styleable::KeyboardLayout_name, &name);
            if (FAILED(ec))
                goto _Exit1_;

            ec = a->GetString(R::styleable::KeyboardLayout_label, &label);
            if (FAILED(ec))
                goto _Exit1_;

            ec = a->GetResourceId(
                R::styleable::KeyboardLayout_keyboardLayout, 0,
                &keyboardLayoutResId);
            if (FAILED(ec))
                goto _Exit1_;

            if (name.IsNull() || label.IsNull() || keyboardLayoutResId == 0) {
                Logger::W(
                    TAG, "Missing required 'name', 'label' or 'keyboardLayout' "
                    "attributes in keyboard layout resource from receiver %s/%s",
                    packageName.string(), receiverName.string());
            }
            else {
                String descriptor = KeyboardLayoutDescriptor::Format(
                        packageName, receiverName, name);
                if (keyboardName.IsNull() || name.Equals(keyboardName)) {
                    visitor->VisitKeyboardLayout(
                        resources, descriptor, label, collection, keyboardLayoutResId);
                }
            }
_Exit1_:
            a->Recycle();
        }
        else {
            Logger::W(TAG, "Skipping unrecognized element '%s"
                "' in keyboard layout resource from receiver %s/%s",
                element.string(), packageName.string(), receiverName.string());
        }
    }
_Exit2_:
    parser->Close();

_Exit3_:
    if (FAILED(ec)) {
        Logger::W(TAG, "Could not parse keyboard layout resource from receiver %s, %s",
            packageName.string(), receiverName.string());
    }
}

ECode CInputManagerService::GetCurrentKeyboardLayoutForInputDevice(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [out] */ String* keyboardLayoutDescriptor)
{
    VALIDATE_NOT_NULL(keyboardLayoutDescriptor);

    if (inputDeviceDescriptor == NULL) {
        Slogger::E(TAG, "inputDeviceDescriptor must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(&mDataStoreLock);
    *keyboardLayoutDescriptor = mDataStore->GetCurrentKeyboardLayout(inputDeviceDescriptor);

    return NOERROR;
}

ECode CInputManagerService::SetCurrentKeyboardLayoutForInputDevice(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [in] */ const String& keyboardLayoutDescriptor)
{
    if (!CheckCallingPermission(
        Elastos::Droid::Manifest::permission::SET_KEYBOARD_LAYOUT,
        String("setCurrentKeyboardLayoutForInputDevice()"))) {
        Slogger::E(TAG, "Requires SET_KEYBOARD_LAYOUT permission");
        return E_SECURITY_EXCEPTION;
    }

    if (inputDeviceDescriptor.IsNull()) {
        Slogger::E(TAG, "inputDeviceDescriptor must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (keyboardLayoutDescriptor.IsNull()) {
        Slogger::E(TAG, "keyboardLayoutDescriptor must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(&mDataStoreLock);
    if (mDataStore->SetCurrentKeyboardLayout(
            inputDeviceDescriptor, keyboardLayoutDescriptor)) {
        Boolean result;
        mHandler->SendEmptyMessage(MSG_RELOAD_KEYBOARD_LAYOUTS, &result);
    }
    mDataStore->SaveIfNeeded();

    return NOERROR;
}

ECode CInputManagerService::GetKeyboardLayoutsForInputDevice(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [out, callee] */ ArrayOf<String>** keyboardLayoutDescriptors)
{
    VALIDATE_NOT_NULL(keyboardLayoutDescriptors);

    if (inputDeviceDescriptor.IsNull()) {
        Slogger::E(TAG, "inputDeviceDescriptor must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(&mDataStoreLock);
    AutoPtr<ArrayOf<String> > temp = mDataStore->GetKeyboardLayouts(inputDeviceDescriptor);
    *keyboardLayoutDescriptors = temp;
    REFCOUNT_ADD(*keyboardLayoutDescriptors);

    return NOERROR;
}

ECode CInputManagerService::AddKeyboardLayoutForInputDevice(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [in] */ const String& keyboardLayoutDescriptor)
{
    if (!CheckCallingPermission(
        Elastos::Droid::Manifest::permission::SET_KEYBOARD_LAYOUT,
        String("addKeyboardLayoutForInputDevice()"))) {
        Slogger::E(TAG, "Requires SET_KEYBOARD_LAYOUT permission");
        return E_SECURITY_EXCEPTION;
    }

    if (inputDeviceDescriptor.IsNull()) {
        Slogger::E(TAG, "inputDeviceDescriptor must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (keyboardLayoutDescriptor.IsNull()) {
        Slogger::E(TAG, "keyboardLayoutDescriptor must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(&mDataStoreLock);
    String oldLayout = mDataStore->GetCurrentKeyboardLayout(inputDeviceDescriptor);
    if (mDataStore->AddKeyboardLayout(inputDeviceDescriptor, keyboardLayoutDescriptor)
        && !oldLayout.Equals(mDataStore->GetCurrentKeyboardLayout(inputDeviceDescriptor))) {
        Boolean result;
        mHandler->SendEmptyMessage(MSG_RELOAD_KEYBOARD_LAYOUTS, &result);
    }
    mDataStore->SaveIfNeeded();

    return NOERROR;
}

ECode CInputManagerService::RemoveKeyboardLayoutForInputDevice(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [in] */ const String& keyboardLayoutDescriptor)
{
    if (!CheckCallingPermission(
        Elastos::Droid::Manifest::permission::SET_KEYBOARD_LAYOUT,
        String("removeKeyboardLayoutForInputDevice()"))) {
        Slogger::E(TAG, "Requires SET_KEYBOARD_LAYOUT permission");
        return E_SECURITY_EXCEPTION;
    }

    if (inputDeviceDescriptor.IsNull()) {
        Slogger::E(TAG, "inputDeviceDescriptor must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (keyboardLayoutDescriptor.IsNull()) {
        Slogger::E(TAG, "keyboardLayoutDescriptor must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(&mDataStoreLock);
    String oldLayout = mDataStore->GetCurrentKeyboardLayout(inputDeviceDescriptor);
    if (mDataStore->RemoveKeyboardLayout(inputDeviceDescriptor, keyboardLayoutDescriptor)
        && !oldLayout.Equals(mDataStore->GetCurrentKeyboardLayout(inputDeviceDescriptor))) {
        Boolean result;
        mHandler->SendEmptyMessage(MSG_RELOAD_KEYBOARD_LAYOUTS, &result);
    }
    mDataStore->SaveIfNeeded();

    return NOERROR;
}

void CInputManagerService::SwitchKeyboardLayout(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 direction)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_SWITCH_KEYBOARD_LAYOUT, (IMessage**)&msg);
    msg->SetArg1(deviceId);
    msg->SetArg2(direction);
    Boolean result;
    mHandler->SendMessage(msg, &result);
}

void CInputManagerService::HandleSwitchKeyboardLayout(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 direction)
{
    AutoPtr<IInputDevice> device;
    GetInputDevice(deviceId, (IInputDevice**)&device);
    if (device != NULL) {
        String inputDeviceDescriptor;
        device->GetDescriptor(&inputDeviceDescriptor);
        Boolean changed;
        String keyboardLayoutDescriptor;
        {
            AutoLock lock(&mDataStoreLock);

            changed = mDataStore->SwitchKeyboardLayout(inputDeviceDescriptor, direction);
            keyboardLayoutDescriptor = mDataStore->GetCurrentKeyboardLayout(
                    inputDeviceDescriptor);
            mDataStore->SaveIfNeeded();
        }

        if (changed) {
            if (mSwitchedKeyboardLayoutToast != NULL) {
                mSwitchedKeyboardLayoutToast->Cancel();
                mSwitchedKeyboardLayoutToast = NULL;
            }
            if (keyboardLayoutDescriptor != NULL) {
                AutoPtr<IKeyboardLayout> keyboardLayout;
                GetKeyboardLayout(keyboardLayoutDescriptor, (IKeyboardLayout**)&keyboardLayout);
                if (keyboardLayout != NULL) {
                    AutoPtr<IToastHelper> toastHelper;
                    CToastHelper::AcquireSingleton((IToastHelper**)&toastHelper);
                    String label;
                    keyboardLayout->GetLabel(&label);
                    AutoPtr<ICharSequence> labelcs;
                    CString::New(label, (ICharSequence**)&labelcs);
                    toastHelper->MakeText(
                        mContext, labelcs, IToast::LENGTH_SHORT,
                        (IToast**)&mSwitchedKeyboardLayoutToast);
                    mSwitchedKeyboardLayoutToast->Show();

                }
            }

            ReloadKeyboardLayouts();
        }
    }
}

void CInputManagerService::SetInputWindows(
    /* [in] */ ArrayOf<InputWindowHandle*>* windowHandles)
{
    NativeSetInputWindows(windowHandles);
}

void CInputManagerService::SetFocusedApplication(
    /* [in] */ InputApplicationHandle* application)
{
    NativeSetFocusedApplication(application);
}

void CInputManagerService::SetInputDispatchMode(
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean frozen)
{
    NativeSetInputDispatchMode(enabled, frozen);
}

void CInputManagerService::SetSystemUiVisibility(
    /* [in] */ Int32 visibility)
{
    NativeSetSystemUiVisibility(visibility);
}

Boolean CInputManagerService::TransferTouchFocus(
    /* [in] */ IInputChannel* fromChannel,
    /* [in] */ IInputChannel* toChannel)
{
    if (fromChannel == NULL) {
        Slogger::E(TAG, "fromChannel must not be NULL");
        assert(0);
        return FALSE;
        //return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (toChannel == NULL) {
        Slogger::E(TAG, "toChannel must not be NULL");
        assert(0);
        return FALSE;
        //return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NativeTransferTouchFocus(fromChannel, toChannel);
}

ECode CInputManagerService::TryPointerSpeed(
    /* [in] */ Int32 speed)
{
    if (!CheckCallingPermission(
        Elastos::Droid::Manifest::permission::SET_POINTER_SPEED,
        String("tryPointerSpeed()"))) {
        Slogger::E(TAG, "Requires SET_POINTER_SPEED permission");
        return E_SECURITY_EXCEPTION;
    }

    if (speed < IInputManager::MIN_POINTER_SPEED
        || speed > IInputManager::MAX_POINTER_SPEED) {
        Slogger::E(TAG, "speed out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    SetPointerSpeedUnchecked(speed);

    return NOERROR;
}

void CInputManagerService::UpdatePointerSpeedFromSettings()
{
    Int32 speed = GetPointerSpeedSetting();
    SetPointerSpeedUnchecked(speed);
}

void CInputManagerService::SetPointerSpeedUnchecked(
    /* [in] */ Int32 speed)
{
    speed = Elastos::Core::Math::Min(Elastos::Core::Math::Max(speed, IInputManager::MIN_POINTER_SPEED),
            IInputManager::MAX_POINTER_SPEED);
    NativeSetPointerSpeed(speed);
}

void CInputManagerService::RegisterPointerSpeedSettingObserver()
{
    AutoPtr<IContentObserver> settingsObserver =
        new MyContentObserver(this, mHandler);

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    AutoPtr<IUri> uri;
    ss->GetUriFor(ISettingsSystem::POINTER_SPEED, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, TRUE, settingsObserver, IUserHandle::USER_ALL);
}

Int32 CInputManagerService::GetPointerSpeedSetting()
{
    Int32 speed = IInputManager::DEFAULT_POINTER_SPEED;

    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    ss->GetInt32ForUser(
        resolver, ISettingsSystem::POINTER_SPEED,
        IUserHandle::USER_CURRENT, &speed);

    return speed;
}

void CInputManagerService::UpdateShowTouchesFromSettings()
{
    Int32 setting = GetShowTouchesSetting(0);
    NativeSetShowTouches(setting != 0);
}


void CInputManagerService::RegisterShowTouchesSettingObserver()
{
    AutoPtr<IContentObserver> settingsObserver =
        new MyContentObserver(this, mHandler);

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    AutoPtr<IUri> uri;
    ss->GetUriFor(ISettingsSystem::SHOW_TOUCHES, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, TRUE, settingsObserver, IUserHandle::USER_ALL);
}

Int32 CInputManagerService::GetShowTouchesSetting(
    /* [in] */ Int32 defaultValue)
{
    Int32 result = defaultValue;

    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    ss->GetInt32ForUser(
        resolver, ISettingsSystem::SHOW_TOUCHES,
        IUserHandle::USER_CURRENT, &result);

    return result;
}

ECode CInputManagerService::Vibrate(
    /* [in] */ Int32 deviceId,
    /* [in] */ const ArrayOf<Int64>& pattern,
    /* [in] */ Int32 repeat,
    /* [in] */ IBinder* token)
{
    if (repeat >= pattern.GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    AutoPtr<VibratorToken> v;
    {
        AutoLock lock(&mVibratorLock);
        HashMap<AutoPtr<IBinder>, AutoPtr<VibratorToken> >::Iterator find
                = mVibratorTokens.Find(token);
        if (find == mVibratorTokens.End()) {
            v = new VibratorToken(this, deviceId, token, mNextVibratorTokenValue++);
            AutoPtr<IProxy> proxy = (IProxy*)token->Probe(EIID_IProxy);
            if (FAILED(proxy->LinkToDeath(v, 0))) {
                // give up
                return E_RUNTIME_EXCEPTION;
            }
            mVibratorTokens[token] =  v;
        }
    }

    AutoLock lock(v->mLock);
    v->mVibrating = TRUE;
    NativeVibrate(deviceId, pattern, repeat, v->mTokenValue);

    return NOERROR;
}

ECode CInputManagerService::CancelVibrate(
    /* [in] */ Int32 deviceId,
    /* [in] */ IBinder* token)
{
    AutoPtr<VibratorToken> v;
    {
        AutoLock lock(&mVibratorLock);
        HashMap<AutoPtr<IBinder>, AutoPtr<VibratorToken> >::Iterator find
                = mVibratorTokens.Find(token);
        if (find == mVibratorTokens.End() || find->mSecond->mDeviceId != deviceId) {
            return NOERROR; // nothing to cancel
        }
        v = find->mSecond;
    }

    CancelVibrateIfNeeded(v);

    return NOERROR;
}

void CInputManagerService::OnVibratorTokenDied(
    /* [in] */ VibratorToken* v)
{
    {
        AutoLock lock(&mVibratorLock);
        mVibratorTokens.Erase(v->mToken);
    }

    CancelVibrateIfNeeded(v);
}

void CInputManagerService::CancelVibrateIfNeeded(
    /* [in] */ VibratorToken* v)
{
    AutoLock lock(v->mLock);
    if (v->mVibrating) {
        NativeCancelVibrate(v->mDeviceId, v->mTokenValue);
        v->mVibrating = FALSE;
    }
}

ECode CInputManagerService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const ArrayOf<String>& args)
{
    Int32 result;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, &result));
    if (result != IPackageManager::PERMISSION_GRANTED) {
        pw->PrintStringln(
            String("Permission Denial: can't dump InputManager from from pid=")
            + StringUtils::Int32ToString(Binder::GetCallingPid())
            + ", uid=" + StringUtils::Int32ToString(Binder::GetCallingUid()));
        return NOERROR;
    }

    pw->PrintStringln(String("INPUT MANAGER (dumpsys input)\n"));
    String dumpStr = NativeDump();
    if (!dumpStr.IsNull()) {
        pw->PrintStringln(dumpStr);
    }

    return NOERROR;
}

Boolean CInputManagerService::CheckCallingPermission(
    /* [in] */ const String& permission,
    /* [in] */ const String& func)
{
    // Quick check: if the calling permission is me, it's all okay.
    Int32 pid = Process::MyPid();
    if (Binder::GetCallingPid() == pid) {
        return TRUE;
    }

    Int32 value;
    FAIL_RETURN(mContext->CheckCallingPermission(permission, &value));
    if (value == IPackageManager::PERMISSION_GRANTED) {
        return TRUE;
    }

    String msg = String("Permission Denial: ") + func + " from pid="
            + StringUtils::Int32ToString(Binder::GetCallingPid())
            + ", uid=" + StringUtils::Int32ToString(Binder::GetCallingUid())
            + " requires " + permission;
    Slogger::W(TAG, msg);

    return FALSE;
}

ECode CInputManagerService::Monitor()
{
    {
        AutoLock lock(&mInputFilterLock);
    }
    NativeMonitor();

    return NOERROR;
}

ECode CInputManagerService::ResetTouchCalibration()
{
    NativeResetTouchCalibration();
    return NOERROR;
}

void CInputManagerService::NotifyConfigurationChanged(
    /* [in] */ Int64 whenNanos)
{
    mWindowManagerCallbacks->NotifyConfigurationChanged();
}

void CInputManagerService::NotifyInputDevicesChanged(
    /* [in] */ ArrayOf<IInputDevice*>* inputDevices)
{
    AutoLock lock(mInputDevicesLock);

    if (!mInputDevicesChangedPending) {
        mInputDevicesChangedPending = TRUE;

        AutoPtr<IObjectContainer> container;
        CObjectContainer::New((IObjectContainer**)&container);
        if (inputDevices) {
            for (Int32 i = 0; i < inputDevices->GetLength(); ++i) {
                container->Add((*inputDevices)[i]);
            }
        }

        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_DELIVER_INPUT_DEVICES_CHANGED, (IMessage**)&msg);
        msg->SetObj(container);
        Boolean result;
        mHandler->SendMessage(msg, &result);
    }

    mInputDevices = inputDevices;
}

void CInputManagerService::NotifySwitch(
    /* [in] */ Int64 whenNanos,
    /* [in] */ Int32 switchValues,
    /* [in] */ Int32 switchMask)
{
    if (DEBUG) {
        Slogger::D(TAG, "notifySwitch: values=%x, mask=%x",
            switchValues, switchMask);
    }

    if ((switchMask & SW_LID_BIT) != 0) {
        Boolean lidOpen = ((switchValues & SW_LID_BIT) == 0);
        mWindowManagerCallbacks->NotifyLidSwitchChanged(whenNanos, lidOpen);
    }

    if (mUseDevInputEventForAudioJack && (switchMask & SW_JACK_BITS) != 0) {
        mWiredAccessoryCallbacks->NotifyWiredAccessoryChanged(
            whenNanos, switchValues, switchMask);
    }
}

void CInputManagerService::NotifyInputChannelBroken(
    /* [in] */ InputWindowHandle* inputWindowHandle)
{
    mWindowManagerCallbacks->NotifyInputChannelBroken(inputWindowHandle);
}

Int64 CInputManagerService::NotifyANR(
    /* [in] */ InputApplicationHandle* inputApplicationHandle,
    /* [in] */ InputWindowHandle* inputWindowHandle)
{
    return mWindowManagerCallbacks->NotifyANR(inputApplicationHandle, inputWindowHandle);
}

Boolean CInputManagerService::FilterInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 policyFlags)
{
    {
        AutoLock lock(&mInputFilterLock);
        if (mInputFilter != NULL) {
            mInputFilter->FilterInputEvent(event, policyFlags);
            return FALSE;
        }
    }
    event->Recycle();
    return TRUE;
}

Int32 CInputManagerService::InterceptKeyBeforeQueueing(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags,
    /* [in] */ Boolean isScreenOn)
{
    return mWindowManagerCallbacks->InterceptKeyBeforeQueueing(
            event, policyFlags, isScreenOn);
}

Int32 CInputManagerService::InterceptMotionBeforeQueueingWhenScreenOff(
    /* [in] */ Int32 policyFlags)
{
    return mWindowManagerCallbacks->InterceptMotionBeforeQueueingWhenScreenOff(policyFlags);
}

Int64 CInputManagerService::InterceptKeyBeforeDispatching(
    /* [in] */ InputWindowHandle* focus,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
    return mWindowManagerCallbacks->InterceptKeyBeforeDispatching(focus, event, policyFlags);
}

AutoPtr<IKeyEvent> CInputManagerService::DispatchUnhandledKey(
    /* [in] */ InputWindowHandle* focus,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
    return mWindowManagerCallbacks->DispatchUnhandledKey(focus, event, policyFlags);
}

Boolean CInputManagerService::CheckInjectEventsPermission(
    /* [in] */ Int32 injectorPid,
    /* [in] */ Int32 injectorUid)
{
    Int32 value;
    FAIL_RETURN(mContext->CheckPermission(
        Elastos::Droid::Manifest::permission::INJECT_EVENTS,
        injectorPid, injectorUid, &value));
    return value == IPackageManager::PERMISSION_GRANTED;
}

Int32 CInputManagerService::GetVirtualKeyQuietTimeMillis()
{
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Int32 value = 0;
    resources->GetInteger(R::integer::config_virtualKeyQuietTimeMillis, &value);
    return value;
}

AutoPtr<ArrayOf<String> > CInputManagerService::GetExcludedDeviceNames()
{
    //TODO::
    //
    return ArrayOf<String>::Alloc(0);

    List<String> names;

    // Read partner-provided list of excluded input devices
    AutoPtr<IXmlPullParser> parser;;

    // Environment.getRootDirectory() is a fancy way of saying ANDROID_ROOT or "/system".
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> root;
    env->GetRootDirectory((IFile**)&root);
    AutoPtr<IFile> confFile;
    CFile::New(root, EXCLUDED_DEVICES_PATH, (IFile**)&confFile);

    AutoPtr<IFileReader> confreader;

    ECode ec = CFileReader::New(confFile, (IFileReader**)&confreader);
    if (FAILED(ec))
        goto _Exit_;

    parser = Xml::NewPullParser();
    ec = parser->SetInput(confreader);
    if (FAILED(ec))
        goto _Exit_;

    ec = XmlUtils::BeginDocument(parser, String("devices"));
    if (FAILED(ec))
        goto _Exit_;

    while (TRUE) {
        ec = XmlUtils::NextElement(parser);
        if (FAILED(ec))
            goto _Exit_;

        String name;
        ec = parser->GetName(&name);
        if (FAILED(ec))
            goto _Exit_;

        if (!name.Equals("device")) {
            break;
        }

        ec = parser->GetAttributeValue(String(NULL), String("name"), &name);
        if (FAILED(ec))
            goto _Exit_;

        if (name != NULL) {
            names.PushBack(name);
        }
    }
_Exit_:
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        // It's ok if the file does not exist.
    }
    else if (FAILED(ec)) {
        String path;
        confFile->GetAbsolutePath(&path);
        Slogger::E(TAG, "Exception while parsing '%s'", path.string());
    }

    if (confreader != NULL)
        ICloseable::Probe(confreader)->Close();

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(names.GetSize());
    List<String>::Iterator iter = names.Begin();
    for (Int32 i = 0; iter != names.End(); ++iter)
        (*array)[i] = *iter;

    return array;
}

Int32 CInputManagerService::GetKeyRepeatTimeout()
{
    AutoPtr<IViewConfigurationHelper> vh;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&vh);
    Int32 value;
    vh->GetKeyRepeatTimeout(&value);
    return value;
}

Int32 CInputManagerService::GetKeyRepeatDelay()
{
    AutoPtr<IViewConfigurationHelper> vh;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&vh);
    Int32 value;
    vh->GetKeyRepeatDelay(&value);
    return value;
}

Int32 CInputManagerService::GetHoverTapTimeout()
{
    AutoPtr<IViewConfigurationHelper> vh;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&vh);
    Int32 value;
    vh->GetHoverTapTimeout(&value);
    return value;
}

Int32 CInputManagerService::GetHoverTapSlop()
{
    AutoPtr<IViewConfigurationHelper> vh;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&vh);
    Int32 value;
    vh->GetHoverTapSlop(&value);
    return value;
}

Int32 CInputManagerService::GetDoubleTapTimeout()
{
    AutoPtr<IViewConfigurationHelper> vh;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&vh);
    Int32 value;
    vh->GetDoubleTapTimeout(&value);
    return value;
}

Int32 CInputManagerService::GetLongPressTimeout()
{
    AutoPtr<IViewConfigurationHelper> vh;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&vh);
    Int32 value;
    vh->GetLongPressTimeout(&value);
    return value;
}

Int32 CInputManagerService::GetPointerLayer()
{
    return mWindowManagerCallbacks->GetPointerLayer();
}

AutoPtr<IPointerIcon> CInputManagerService::GetPointerIcon()
{
    AutoPtr<IPointerIconHelper> ph;
    CPointerIconHelper::AcquireSingleton((IPointerIconHelper**)&ph);
    AutoPtr<IPointerIcon> icon;
    ph->GetDefaultIcon(mContext, (IPointerIcon**)&icon);

    return icon;
}

AutoPtr<ArrayOf<String> > CInputManagerService::GetKeyboardLayoutOverlay(
    /* [in] */ const String& inputDeviceDescriptor)
{
    if (!mSystemReady) {
        return NULL;
    }

    String keyboardLayoutDescriptor;
    GetCurrentKeyboardLayoutForInputDevice(
        inputDeviceDescriptor, &keyboardLayoutDescriptor);
    if (keyboardLayoutDescriptor.IsNull()) {
        return NULL;
    }

    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(2);
    AutoPtr<MyKeyboardLayoutVisitorEx3> myKVEx3 = new MyKeyboardLayoutVisitor(result);
    VisitKeyboardLayout(keyboardLayoutDescriptor, myKVEx3);
    if ((*result)[0].IsNull()) {
        Logger::W(TAG, "Could not get keyboard layout with descriptor '%s'.",
            keyboardLayoutDescriptor.string());
        return NULL;
    }

    return result;
}

String CInputManagerService::GetDeviceAlias(
    /* [in] */ const String& uniqueId)
{
    AutoPtr<IBluetoothAdapterHelper> bh;
    CBluetoothAdapterHelper::AcquireSingleton((IBluetoothAdapterHelper**)&bh);
    Boolean res;
    bh->CheckBluetoothAddress(uniqueId, &res);
    if (res) {
        // TODO(BT) mBluetoothService.getRemoteAlias(uniqueId)
        return String(NULL);
    }
    return String(NULL);
}

void CInputManagerService::NativeInit()
{
    AutoPtr<IMessageQueue> cq;
    mHandler->GetMessageQueue((IMessageQueue**)&cq);
    Handle32 hq;
    cq->GetNativeMessageQueue(&hq);
    assert(hq);
    NativeMessageQueue* queue = (NativeMessageQueue*)hq;
    mNativeInputManager = new NativeInputManager(
        mContext, this, queue->GetLooper());
}

ECode CInputManagerService::NativeStart()
{
    android::status_t result = mNativeInputManager->getInputManager()->start();
    if (result) {
        Logger::E(TAG, "Input manager could not be started.");
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

void CInputManagerService::NativeSetDisplayViewport(
    /* [in] */ Boolean external,
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 rotation,
    /* [in] */ Int32 logicalLeft,
    /* [in] */ Int32 logicalTop,
    /* [in] */ Int32 logicalRight,
    /* [in] */ Int32 logicalBottom,
    /* [in] */ Int32 physicalLeft,
    /* [in] */ Int32 physicalTop,
    /* [in] */ Int32 physicalRight,
    /* [in] */ Int32 physicalBottom,
    /* [in] */ Int32 deviceWidth,
    /* [in] */ Int32 deviceHeight)
{
    android::DisplayViewport v;
    v.displayId = displayId;
    v.orientation = rotation;
    v.logicalLeft = logicalLeft;
    v.logicalTop = logicalTop;
    v.logicalRight = logicalRight;
    v.logicalBottom = logicalBottom;
    v.physicalLeft = physicalLeft;
    v.physicalTop = physicalTop;
    v.physicalRight = physicalRight;
    v.physicalBottom = physicalBottom;
    v.deviceWidth = deviceWidth;
    v.deviceHeight = deviceHeight;
    mNativeInputManager->setDisplayViewport(external, v);
}

Int32 CInputManagerService::NativeGetScanCodeState(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ Int32 scanCode)
{
    return mNativeInputManager->getInputManager()->getReader()->getScanCodeState(
            deviceId, uint32_t(sourceMask), scanCode);
}

Int32 CInputManagerService::NativeGetKeyCodeState(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ Int32 keyCode)
{
    return mNativeInputManager->getInputManager()->getReader()->getKeyCodeState(
            deviceId, uint32_t(sourceMask), keyCode);
}

Int32 CInputManagerService::NativeGetSwitchState(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ Int32 sw)
{
    return mNativeInputManager->getInputManager()->getReader()->getSwitchState(
            deviceId, uint32_t(sourceMask), sw);
}

Boolean CInputManagerService::NativeHasKeys(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ const ArrayOf<Int32>& keyCodes,
    /* [in] */ ArrayOf<Boolean>* keyExists)
{
    int32_t* codes = (int32_t*)keyCodes.GetPayload();
    uint8_t* flags = (uint8_t*)keyExists->GetPayload();
    Int32 numCodes = keyCodes.GetLength();
    if (numCodes == keyExists->GetLength()) {
        return mNativeInputManager->getInputManager()->getReader()->hasKeys(
                deviceId, uint32_t(sourceMask), numCodes, codes, flags);
    }

    return FALSE;
}

static void HandleInputChannelDisposed(
    /* [in] */ IInputChannel* inputChannelObj,
    /* [in] */ const android::sp<android::InputChannel>& inputChannel,
    /* [in] */ void* data)
{
    NativeInputManager* im = static_cast<NativeInputManager*>(data);
    im->unregisterInputChannel(inputChannel);
}

ECode CInputManagerService::NativeRegisterInputChannel(
    /* [in] */ IInputChannel* inputChannelObj,
    /* [in] */ InputWindowHandle* inputWindowHandleObj,
    /* [in] */ Boolean monitor)
{
    Handle32 nativeInputChannel;
    inputChannelObj->GetNativeInputChannel(&nativeInputChannel);
    android::sp<android::InputChannel> inputChannel =
        ((NativeInputChannel*)nativeInputChannel)->getInputChannel();
    if (inputChannel == NULL) {
        Logger::E(TAG, "inputChannel is not initialized");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    android::sp<android::InputWindowHandle> inputWindowHandle =
            InputWindowHandle::GetHandle(inputWindowHandleObj);

    android::status_t status = mNativeInputManager->registerInputChannel(
            inputChannel, inputWindowHandle, monitor);
    if (status) {
        Logger::E(TAG, "Failed to register input channel.  status=%d", status);
        return E_RUNTIME_EXCEPTION;
    }

    if (!monitor) {
        ((NativeInputChannel*)nativeInputChannel)->setDisposeCallback(
            HandleInputChannelDisposed, (void*)mNativeInputManager.get());
    }

    return NOERROR;
}

ECode CInputManagerService::NativeUnregisterInputChannel(
    /* [in] */ IInputChannel* inputChannelObj)
{
    Handle32 nativeInputChannel;
    inputChannelObj->GetNativeInputChannel(&nativeInputChannel);
    android::sp<android::InputChannel> inputChannel =
        ((NativeInputChannel*)nativeInputChannel)->getInputChannel();
    if (inputChannel == NULL) {
        Logger::E(TAG, "inputChannel is not initialized");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    ((NativeInputChannel*)nativeInputChannel)->setDisposeCallback(NULL, NULL);

    android::status_t status = mNativeInputManager->unregisterInputChannel(inputChannel);
    if (status && status != android::BAD_VALUE) { // ignore already unregistered channel
        Logger::E(TAG, "Failed to unregister input channel.  status=%d", status);
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

void CInputManagerService::NativeSetInputFilterEnabled(
    /* [in] */ Boolean enable)
{
    mNativeInputManager->getInputManager()->getDispatcher()->setInputFilterEnabled(enable);
}

static void KeyEvent_toNative(
        /* [in] */ IKeyEvent* event,
        /* [in] */ android::KeyEvent* androidEvent)
{
    Int32 deviceId;
    event->GetDeviceId(&deviceId);
    Int32 source;
    event->GetSource(&source);
    Int32 metaState;
    event->GetMetaState(&metaState);
    Int32 action;
    event->GetAction(&action);
    Int32 keyCode;
    event->GetKeyCode(&keyCode);
    Int32 scanCode;
    event->GetScanCode(&scanCode);
    Int32 repeatCount;
    event->GetRepeatCount(&repeatCount);
    Int32 flags;
    event->GetFlags(&flags);
    Int64 downTime;
    event->GetDownTime(&downTime);
    Int64 eventTime;
    event->GetEventTime(&eventTime);

    androidEvent->initialize(deviceId, source, action, flags, keyCode, scanCode, metaState, repeatCount,
            milliseconds_to_nanoseconds(downTime),
            milliseconds_to_nanoseconds(eventTime));
}

static void MotionEvent_toNative(
    /* [in] */ IMotionEvent* event,
    /* [in] */ android::MotionEvent* androidEvent)
{

    AutoPtr<IParcel> parcel;
    CParcel::New((IParcel**)&parcel);
    IParcelable::Probe(event)->WriteToParcel(parcel);

    android::Parcel* androidParcel;
    parcel->GetElementPayload((Handle32*)&androidParcel);

    androidParcel->setDataPosition(0);
    androidParcel->readInt32();
    androidEvent->readFromParcel(androidParcel);
}

Int32 CInputManagerService::NativeInjectInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 injectorPid,
    /* [in] */ Int32 injectorUid,
    /* [in] */ Int32 syncMode,
    /* [in] */ Int32 timeoutMillis,
    /* [in] */ Int32 policyFlags)
{
    if (IKeyEvent::Probe(event)) {
        android::KeyEvent keyEvent;
        KeyEvent_toNative(IKeyEvent::Probe(event), &keyEvent);

        return mNativeInputManager->getInputManager()->getDispatcher()->injectInputEvent(
                &keyEvent, injectorPid, injectorUid, syncMode, timeoutMillis,
                uint32_t(policyFlags));
    }
    else if (IMotionEvent::Probe(event)) {
        android::MotionEvent motionEvent;
        MotionEvent_toNative(IMotionEvent::Probe(event), &motionEvent);

        return mNativeInputManager->getInputManager()->getDispatcher()->injectInputEvent(
                &motionEvent, injectorPid, injectorUid, syncMode, timeoutMillis,
                uint32_t(policyFlags));
    }
    else {
        Logger::E(TAG, "Invalid input event type.");
        return INPUT_EVENT_INJECTION_FAILED;
    }
}

void CInputManagerService::NativeSetInputWindows(
    /* [in] */ ArrayOf<InputWindowHandle*>* windowHandles)
{
    mNativeInputManager->setInputWindows(windowHandles);
}

void CInputManagerService::NativeSetInputDispatchMode(
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean frozen)
{
    mNativeInputManager->setInputDispatchMode(enabled, frozen);
}

void CInputManagerService::NativeSetSystemUiVisibility(
    /* [in] */ Int32 visibility)
{
    mNativeInputManager->setSystemUiVisibility(visibility);
}

void CInputManagerService::NativeSetFocusedApplication(
    /* [in] */ InputApplicationHandle* application)
{
    mNativeInputManager->setFocusedApplication(application);
}

Boolean CInputManagerService::NativeTransferTouchFocus(
    /* [in] */ IInputChannel* fromChannelObj,
    /* [in] */ IInputChannel* toChannelObj)
{
    Handle32 nativeInputChannel;
    fromChannelObj->GetNativeInputChannel(&nativeInputChannel);
    android::sp<android::InputChannel> fromChannel =
        ((NativeInputChannel*)nativeInputChannel)->getInputChannel();

    toChannelObj->GetNativeInputChannel(&nativeInputChannel);
    android::sp<android::InputChannel> toChannel =
        ((NativeInputChannel*)nativeInputChannel)->getInputChannel();


    if (fromChannel == NULL || toChannel == NULL) {
        return false;
    }

    return mNativeInputManager->getInputManager()->getDispatcher()->
            transferTouchFocus(fromChannel, toChannel);
}

void CInputManagerService::NativeSetPointerSpeed(
    /* [in] */ Int32 speed)
{
    mNativeInputManager->setPointerSpeed(speed);
}

void CInputManagerService::NativeSetShowTouches(
    /* [in] */ Boolean enabled)
{
    mNativeInputManager->setShowTouches(enabled);
}

void CInputManagerService::NativeVibrate(
    /* [in] */ Int32 deviceId,
    /* [in] */ const ArrayOf<Int64>& elPattern,
    /* [in] */ Int32 repeat,
    /* [in] */ Int32 token)
{
    size_t patternSize = elPattern.GetLength();
    if (patternSize > MAX_VIBRATE_PATTERN_SIZE) {
        ALOGI("Skipped requested vibration because the pattern size is %d "
                "which is more than the maximum supported size of %d.",
                patternSize, MAX_VIBRATE_PATTERN_SIZE);
        return; // limit to reasonable size
    }

    nsecs_t pattern[patternSize];
    for (size_t i = 0; i < patternSize; i++) {
        pattern[i] = Elastos::Core::Math::Max(Int64(0), Elastos::Core::Math::Min(elPattern[i],
            MAX_VIBRATE_PATTERN_DELAY_NSECS / 1000000LL)) * 1000000LL;
    }

    mNativeInputManager->getInputManager()->getReader()->vibrate(
        deviceId, pattern, patternSize, repeat, token);
}

void CInputManagerService::NativeCancelVibrate(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 token)
{
    mNativeInputManager->getInputManager()->getReader()->cancelVibrate(deviceId, token);
}

void CInputManagerService::NativeReloadKeyboardLayouts()
{
    mNativeInputManager->getInputManager()->getReader()->requestRefreshConfiguration(
        android::InputReaderConfiguration::CHANGE_KEYBOARD_LAYOUTS);
}

void CInputManagerService::NativeReloadDeviceAliases()
{
    mNativeInputManager->getInputManager()->getReader()->requestRefreshConfiguration(
        android::InputReaderConfiguration::CHANGE_DEVICE_ALIAS);
}

void CInputManagerService::NativeKeyEnterMouseMode()
{
    mNativeInputManager->getInputManager()->getReader()->keyEnterMouseMode();
}

void CInputManagerService::NativeKeyExitMouseMode()
{
    mNativeInputManager->getInputManager()->getReader()->keyExitMouseMode();
}

void CInputManagerService::NativeKeySetMouseDistance(
    /* [in] */ Int32 distance)
{
    mNativeInputManager->getInputManager()->getReader()->keySetMouseDistance(distance);
}

void CInputManagerService::NativeKeySetMouseMoveCode(
    /* [in] */ Int32 left,
    /* [in] */ Int32 right,
    /* [in] */ Int32 top,
    /* [in] */ Int32 bottom)
{
    mNativeInputManager->getInputManager()->getReader()->keySetMouseMoveCode(left, right, top, bottom);
}

void CInputManagerService::NativeKeySetMouseBtnCode(
    /* [in] */ Int32 leftbtn,
    /* [in] */ Int32 midbtn,
    /* [in] */ Int32 rightbtn)
{
    mNativeInputManager->getInputManager()->getReader()->keySetMouseBtnCode(leftbtn, midbtn, rightbtn);
}

ECode CInputManagerService::KeyEnterMouseMode()
{
    NativeKeyEnterMouseMode();
    return NOERROR;
}

ECode CInputManagerService::KeyExitMouseMode()
{
    NativeKeyExitMouseMode();
    return NOERROR;
}

ECode CInputManagerService::KeySetMouseDistance(
    /* [in] */ Int32 distance)
{
    NativeKeySetMouseDistance(distance);
    return NOERROR;
}

ECode CInputManagerService::KeySetMouseMoveCode(
    /* [in] */ Int32 left,
    /* [in] */ Int32 right,
    /* [in] */ Int32 top,
    /* [in] */ Int32 bottom)
{
    NativeKeySetMouseMoveCode(left,right,top,bottom);
    return NOERROR;
}

ECode CInputManagerService::KeySetMouseBtnCode(
    /* [in] */ Int32 leftbtn,
    /* [in] */ Int32 midbtn,
    /* [in] */ Int32 rightbtn)
{
    NativeKeySetMouseBtnCode(leftbtn,midbtn,rightbtn);
    return NOERROR;
}

String CInputManagerService::NativeDump()
{
    android::String8 dump;
    mNativeInputManager->dump(dump);
    return String(dump.string());
}

void CInputManagerService::NativeMonitor()
{
    mNativeInputManager->getInputManager()->getReader()->monitor();
    mNativeInputManager->getInputManager()->getDispatcher()->monitor();
}

void CInputManagerService::NativeResetTouchCalibration()
{
    mNativeInputManager->getInputManager()->getReader()->resetTouchCalibration();
}

} // namespace Input
} // namespace Server
} // namepsace Droid
} // namespace Elastos
