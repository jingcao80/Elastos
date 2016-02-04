#include "elastos/droid/server/input/CInputManagerService.h"
#include "elastos/droid/server/DisplayThread.h"
#include "elastos/droid/server/LocalServices.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.Core.Server.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
//#include "elastos/droid/os/CHandler.h"
//#include "elastos/droid/app/CPendingIntent.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::EIID_IIInputManager;
using Elastos::Droid::Hardware::Input::EIID_IInputManagerInternal;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::EIID_IIInputFilterHost;
using Elastos::Droid::R;
using Elastos::Core::IArrayOf;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

//==============================================================================
//  CInputManagerService::InputManagerHandler
//==============================================================================
CInputManagerService::InputManagerHandler::InputManagerHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ CInputManagerService* host)
    : Handler(looper, NULL, TRUE/*async*/)
    , mHost(host)
{}

//@Override
ECode CInputManagerService::InputManagerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_DELIVER_INPUT_DEVICES_CHANGED:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IArrayOf* array = IArrayOf::Probe(obj);
            AutoPtr< ArrayOf<IInputDevice*> > inputDevices;
            if (array != NULL) {
                Int32 size;
                array->GetLength(&size);
                inputDevices = ArrayOf<IInputDevice*>::Alloc(size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> inputDevice;
                    array->Get(i, (IInterface**)&inputDevice);
                    inputDevices->Set(i, IInputDevice::Probe(inputDevice));
                }
            }
            mHost->DeliverInputDevicesChanged(inputDevices);
            break;
        }
        case MSG_SWITCH_KEYBOARD_LAYOUT:
        {
            Int32 arg1, arg2;
            msg->GetArg1(&arg1);
            msg->GetArg2(&arg2);
            mHost->HandleSwitchKeyboardLayout(arg1, arg2);
            break;
        }
        case MSG_RELOAD_KEYBOARD_LAYOUTS:
            mHost->ReloadKeyboardLayouts();
            break;
        case MSG_UPDATE_KEYBOARD_LAYOUTS:
            mHost->UpdateKeyboardLayouts();
            break;
        case MSG_RELOAD_DEVICE_ALIASES:
            mHost->ReloadDeviceAliases();
            break;
    }
    return NOERROR;
}


//==============================================================================
//  CInputManagerService::InputFilterHost
//==============================================================================
CAR_INTERFACE_IMPL_2(CInputManagerService::InputFilterHost, Object, IIInputFilterHost, IBinder);

CInputManagerService::InputFilterHost::InputFilterHost()
    : mDisconnected(FALSE)
    , mHost(NULL)
{}

CInputManagerService::InputFilterHost::~InputFilterHost()
{}

ECode CInputManagerService::InputFilterHost::constructor(
    /* [in] */ IIInputManager* host)
{
    mHost = (CInputManagerService*)host;
    return NOERROR;
}

void CInputManagerService::InputFilterHost::DisconnectLocked()
{
    mDisconnected = TRUE;
}

ECode CInputManagerService::InputFilterHost::SendInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 policyFlags)
{
    if (event == NULL) {
        //throw new IllegalArgumentException("event must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Object& lock = mHost->mInputFilterLock;
    synchronized (lock) {
        if (!mDisconnected) {
            NativeInjectInputEvent(mHost->mPtr, event, IDisplay::DEFAULT_DISPLAY, 0, 0,
                    IInputManager::INJECT_INPUT_EVENT_MODE_ASYNC, 0,
                    policyFlags | IWindowManagerPolicy::FLAG_FILTERED);
        }
    }
    return NOERROR;
}

ECode CInputManagerService::InputFilterHost::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "CInputManagerService::InputFilterHost";
    return NOERROR;
}


//==============================================================================
//  CInputManagerService::KeyboardLayoutDescriptor
//==============================================================================
String CInputManagerService::KeyboardLayoutDescriptor::Format(
    /* [in] */ const String& packageName,
    /* [in] */ const String& receiverName,
    /* [in] */ const String& keyboardName)
{
    return packageName + "/" + receiverName + "/" + keyboardName;
}

AutoPtr<CInputManagerService::KeyboardLayoutDescriptor>
CInputManagerService::KeyboardLayoutDescriptor::Parse(
    /* [in] */ const String& descriptor)
{
    Int32 pos = descriptor.IndexOf('/');
    if (pos < 0 || pos + 1 == descriptor.GetLength()) {
        return NULL;
    }
    Int32 pos2 = descriptor.IndexOf('/', pos + 1);
    if (pos2 < pos + 2 || pos2 + 1 == descriptor.GetLength()) {
        return NULL;
    }

    AutoPtr<KeyboardLayoutDescriptor> result = new KeyboardLayoutDescriptor();
    result->mPackageName = descriptor.Substring(0, pos);
    result->mReceiverName = descriptor.Substring(pos + 1, pos2);
    result->mKeyboardLayoutName = descriptor.Substring(pos2 + 1);
    return result;
}


//==============================================================================
//  CInputManagerService::InputDevicesChangedListenerRecord
//==============================================================================
CAR_INTERFACE_IMPL(CInputManagerService::InputDevicesChangedListenerRecord, Object, IProxyDeathRecipient);

CInputManagerService::InputDevicesChangedListenerRecord::InputDevicesChangedListenerRecord(
    /* [in] */ Int32 pid,
    /* [in] */ IInputDevicesChangedListener* listener,
    /* [in] */ CInputManagerService* host)
    : mPid(pid)
    , mListener(listener)
    , mHost(host)
{}

//@Override
ECode CInputManagerService::InputDevicesChangedListenerRecord::BinderDied()
{
    if (DEBUG) {
        Slogger::D(TAG, "Input devices changed listener for pid %d died.", mPid);
    }
    mHost->OnInputDevicesChangedListenerDied(mPid);
    return NOERROR;
}

void CInputManagerService::InputDevicesChangedListenerRecord::NotifyInputDevicesChanged(
    /* [in] */ ArrayOf<Int32>* info)
{
    ECode ec = mListener->OnInputDevicesChanged(info);
    if (FAILED(ec)) {
        Slogger::W(TAG, "Failed to notify process %d that input devices changed, assuming it died.", mPid);
        BinderDied();
    }
}


//==============================================================================
//  CInputManagerService::VibratorToken
//==============================================================================
CAR_INTERFACE_IMPL(CInputManagerService::VibratorToken, Object, IProxyDeathRecipient);

CInputManagerService::VibratorToken::VibratorToken(
    /* [in] */ Int32 deviceId,
    /* [in] */ IBinder* token,
    /* [in] */ Int32 tokenValue,
    /* [in] */ CInputManagerService* host)
    : mDeviceId(deviceId)
    , mToken(token)
    , mTokenValue(tokenValue)
    , mHost(host)
{}

//@Override
ECode CInputManagerService::VibratorToken::BinderDied()
{
    if (DEBUG) {
        Slogger::D(TAG, "Vibrator token died.");
    }
    mHost->OnVibratorTokenDied(this);
    return NOERROR;
}


//==============================================================================
//  CInputManagerService::LocalService
//==============================================================================
CAR_INTERFACE_IMPL(CInputManagerService::LocalService, Object, IInputManagerInternal);

CInputManagerService::LocalService::LocalService(
    /* [in] */ CInputManagerService* host)
    : mHost(host)
{}

ECode CInputManagerService::LocalService::SetDisplayViewports(
    /* [in] */ IDisplayViewport* defaultViewport,
    /* [in] */ IDisplayViewport* externalTouchViewport)
{
    mHost->SetDisplayViewportsInternal(defaultViewport, externalTouchViewport);
    return NOERROR;
}

ECode CInputManagerService::LocalService::InjectInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 mode,
    /* [out] */ Boolean* result)
{
    return mHost->InjectInputEventInternal(event, displayId, mode, result);
}

ECode CInputManagerService::LocalService::SetInteractive(
    /* [in] */ Boolean interactive)
{
    NativeSetInteractive(mHost->mPtr, interactive);
    return NOERROR;
}


//==============================================================================
//  CInputManagerService
//==============================================================================
const String CInputManagerService::TAG("InputManager");
const Boolean CInputManagerService::DEBUG;
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

const Int32 CInputManagerService::BTN_MOUSE;

const Int32 CInputManagerService::SW_LID;

const Int32 CInputManagerService::SW_KEYPAD_SLIDE;

const Int32 CInputManagerService::SW_HEADPHONE_INSERT;

const Int32 CInputManagerService::SW_MICROPHONE_INSERT;

const Int32 CInputManagerService::SW_LINEOUT_INSERT;

const Int32 CInputManagerService::SW_JACK_PHYSICAL_INSERT;

const Int32 CInputManagerService::SW_CAMERA_LENS_COVER;

const Int32 CInputManagerService::SW_LID_BIT;
const Int32 CInputManagerService::SW_KEYPAD_SLIDE_BIT;
const Int32 CInputManagerService::SW_HEADPHONE_INSERT_BIT;
const Int32 CInputManagerService::SW_MICROPHONE_INSERT_BIT;
const Int32 CInputManagerService::SW_LINEOUT_INSERT_BIT;
const Int32 CInputManagerService::SW_JACK_PHYSICAL_INSERT_BIT;
const Int32 CInputManagerService::SW_JACK_BITS;
const Int32 CInputManagerService::SW_CAMERA_LENS_COVER_BIT;

CAR_INTERFACE_IMPL_3(CInputManagerService, Object, IIInputManager, IWatchdogMonitor, IBinder);

CInputManagerService::CInputManagerService()
    : mPtr(0)
    , mSystemReady(FALSE)
    , mInputDevicesChangedPending(FALSE)
    , mKeyboardLayoutNotificationShown(FALSE)
    , mNextVibratorTokenValue(0)
    , mUseDevInputEventForAudioJack(FALSE)
{
    mDataStore = new PersistentDataStore();
    mInputDevices = ArrayOf<IInputDevice*>::Alloc(0);
    CSparseArray::New((ISparseArray**)&mInputDevicesChangedListeners);
    CArrayList::New((IArrayList**)&mTempInputDevicesChangedListenersToNotify);
    CArrayList::New((IArrayList**)&mTempFullKeyboards);
}

CInputManagerService::~CInputManagerService()
{}

ECode CInputManagerService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    AutoPtr<ILooper> looper;
    DisplayThread::Get()->GetLooper((ILooper**)&looper);
    mHandler = new InputManagerHandler(looper, this);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetBoolean(R::bool_::config_useDevInputEventForAudioJack, &mUseDevInputEventForAudioJack);
    Slogger::I(TAG, "Initializing input manager, mUseDevInputEventForAudioJack=%d",
            mUseDevInputEventForAudioJack);
    looper = NULL;
    mHandler->GetLooper((ILooper**)&looper);
    AutoPtr<IMessageQueue> queue;
    looper->GetQueue((IMessageQueue**)&queue);
    mPtr = NativeInit(this, mContext, queue);

    LocalServices::AddService(EIID_IInputManagerInternal, (IInputManagerInternal*)new LocalService(this));
    return NOERROR;
}

/**
 * these native functions come from ./frameworks/base/services/core/jni/com_android_server_input_InputManagerService.cpp
 */
Int64 CInputManagerService::NativeInit(
    /* [in] */ CInputManagerService* service,
    /* [in] */ IContext* context,
    /* [in] */ IMessageQueue* messageQueue)
{
}

void CInputManagerService::NativeStart(
    /* [in] */ Int64 ptr)
{

}

void CInputManagerService::NativeSetDisplayViewport(
    /* [in] */ Int64 ptr,
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

}

Int32 CInputManagerService::NativeGetScanCodeState(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ Int32 scanCode)
{

}

Int32 CInputManagerService::NativeGetKeyCodeState(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ Int32 keyCode)
{

}

Int32 CInputManagerService::NativeGetSwitchState(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ Int32 sw)
{

}

Boolean CInputManagerService::NativeHasKeys(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ ArrayOf<Int32>* keyCodes,
    /* [in] */ ArrayOf<Boolean>* keyExists)
{

}

void CInputManagerService::NativeRegisterInputChannel(
    /* [in] */ Int64 ptr,
    /* [in] */ IInputChannel* inputChannel,
    /* [in] */ InputWindowHandle* inputWindowHandle,
    /* [in] */ Boolean monitor)
{

}

void CInputManagerService::NativeUnregisterInputChannel(
    /* [in] */ Int64 ptr,
    /* [in] */ IInputChannel* inputChannel)
{

}

void CInputManagerService::NativeSetInputFilterEnabled(
    /* [in] */ Int64 ptr,
    /* [in] */ Boolean enable)
{

}

Int32 CInputManagerService::NativeInjectInputEvent(
    /* [in] */ Int64 ptr,
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 injectorPid,
    /* [in] */ Int32 injectorUid,
    /* [in] */ Int32 syncMode,
    /* [in] */ Int32 timeoutMillis,
    /* [in] */ Int32 policyFlags)
{

}

void CInputManagerService::NativeSetInputWindows(
    /* [in] */ Int64 ptr,
    /* [in] */ ArrayOf<InputWindowHandle*>* windowHandles)
{

}

void CInputManagerService::NativeSetInputDispatchMode(
    /* [in] */ Int64 ptr,
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean frozen)
{

}

void CInputManagerService::NativeSetSystemUiVisibility(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 visibility)
{

}

void CInputManagerService::NativeSetFocusedApplication(
    /* [in] */ Int64 ptr,
    /* [in] */ InputApplicationHandle* application)
{

}

Boolean CInputManagerService::NativeTransferTouchFocus(
    /* [in] */ Int64 ptr,
    /* [in] */ IInputChannel* fromChannel,
    /* [in] */ IInputChannel* toChannel)
{

}

void CInputManagerService::NativeSetPointerSpeed(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 speed)
{

}

void CInputManagerService::NativeSetShowTouches(
    /* [in] */ Int64 ptr,
    /* [in] */ Boolean enabled)
{

}

void CInputManagerService::NativeSetInteractive(
    /* [in] */ Int64 ptr,
    /* [in] */ Boolean interactive)
{

}

void CInputManagerService::NativeReloadCalibration(
    /* [in] */ Int64 ptr)
{

}

void CInputManagerService::NativeVibrate(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 deviceId,
    /* [in] */ ArrayOf<Int64>* pattern,
    /* [in] */ Int32 repeat,
    /* [in] */ Int32 token)
{

}

void CInputManagerService::NativeCancelVibrate(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 token)
{

}

void CInputManagerService::NativeReloadKeyboardLayouts(
    /* [in] */ Int64 ptr)
{

}

void CInputManagerService::NativeReloadDeviceAliases(
    /* [in] */ Int64 ptr)
{

}

String CInputManagerService::NativeDump(
    /* [in] */ Int64 ptr)
{

}

void CInputManagerService::NativeMonitor(
    /* [in] */ Int64 ptr)
{

}


void CInputManagerService::SetWindowManagerCallbacks(
    /* [in] */ IWindowManagerCallbacks* cbacks)
{

}

void CInputManagerService::SetWiredAccessoryCallbacks(
    /* [in] */ IWiredAccessoryCallbacks* cbacks)
{
}

void CInputManagerService::Start()
{
}

// TODO(BT) Pass in paramter for bluetooth system
void CInputManagerService::SystemRunning()
{
}

/**
 * Gets the current state of a key or button by key code.
 * @param deviceId The input device id, or -1 to consult all devices.
 * @param sourceMask The input sources to consult, or {@link InputDevice#SOURCE_ANY} to
 * consider all input sources.  An input device is consulted if at least one of its
 * non-class input source bits matches the specified source mask.
 * @param keyCode The key code to check.
 * @return The key state.
 */
Int32 CInputManagerService::GetKeyCodeState(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ Int32 keyCode)
{
}

/**
 * Gets the current state of a key or button by scan code.
 * @param deviceId The input device id, or -1 to consult all devices.
 * @param sourceMask The input sources to consult, or {@link InputDevice#SOURCE_ANY} to
 * consider all input sources.  An input device is consulted if at least one of its
 * non-class input source bits matches the specified source mask.
 * @param scanCode The scan code to check.
 * @return The key state.
 */
Int32 CInputManagerService::GetScanCodeState(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ Int32 scanCode)
{
}

/**
 * Gets the current state of a switch by switch code.
 * @param deviceId The input device id, or -1 to consult all devices.
 * @param sourceMask The input sources to consult, or {@link InputDevice#SOURCE_ANY} to
 * consider all input sources.  An input device is consulted if at least one of its
 * non-class input source bits matches the specified source mask.
 * @param switchCode The switch code to check.
 * @return The switch state.
 */
Int32 CInputManagerService::GetSwitchState(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ Int32 switchCode)
{
}

/**
 * Determines whether the specified key codes are supported by a particular device.
 * @param deviceId The input device id, or -1 to consult all devices.
 * @param sourceMask The input sources to consult, or {@link InputDevice#SOURCE_ANY} to
 * consider all input sources.  An input device is consulted if at least one of its
 * non-class input source bits matches the specified source mask.
 * @param keyCodes The array of key codes to check.
 * @param keyExists An array at least as large as keyCodes whose entries will be set
 * to true or false based on the presence or absence of support for the corresponding
 * key codes.
 * @return True if the lookup was successful, false otherwise.
 */
// @Override // Binder call
ECode CInputManagerService::HasKeys(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ const ArrayOf<Int32>& keyCodes,
    /* [in] */ ArrayOf<Boolean>* keyExists,
    /* [out] */ Boolean* hasKeys)
{
    return NOERROR;
}

/**
 * Creates an input channel that will receive all input from the input dispatcher.
 * @param inputChannelName The input channel name.
 * @return The input channel.
 */
ECode CInputManagerService::MonitorInput(
    /* [in] */ const String& inputChannelName,
    /* [out] */ IInputChannel** ic)
{
    return NOERROR;
}

/**
 * Registers an input channel so that it can be used as an input event target.
 * @param inputChannel The input channel to register.
 * @param inputWindowHandle The handle of the input window associated with the
 * input channel, or null if none.
 */
ECode CInputManagerService::RegisterInputChannel(
    /* [in] */ IInputChannel* inputChannel,
    /* [in] */ InputWindowHandle* inputWindowHandle)
{
    return NOERROR;
}

/**
 * Unregisters an input channel.
 * @param inputChannel The input channel to unregister.
 */
ECode CInputManagerService::UnregisterInputChannel(
    /* [in] */ IInputChannel* inputChannel)
{
    return NOERROR;
}

/**
 * Sets an input filter that will receive all input events before they are dispatched.
 * The input filter may then reinterpret input events or inject new ones.
 *
 * To ensure consistency, the input dispatcher automatically drops all events
 * in progress whenever an input filter is installed or uninstalled.  After an input
 * filter is uninstalled, it can no longer send input events unless it is reinstalled.
 * Any events it attempts to send after it has been uninstalled will be dropped.
 *
 * @param filter The input filter, or null to remove the current filter.
 */
void CInputManagerService::SetInputFilter(
    /* [in] */ IIInputFilter* filter)
{
    synchronized (mInputFilterLock) {
        IIInputFilter* oldFilter = mInputFilter;
        if (oldFilter == filter) {
            return; // nothing to do
        }

        if (oldFilter != NULL) {
            mInputFilter = NULL;
            mInputFilterHost->DisconnectLocked();
            mInputFilterHost = NULL;
            //try {
//                oldFilter->Uninstall();
            //} catch (RemoteException re) {
                /* ignore */
            //}
        }

        if (filter != NULL) {
            mInputFilter = filter;
            mInputFilterHost = new InputFilterHost();
            mInputFilterHost->constructor(IIInputManager::Probe(this));
            //try {
//                filter->Install(mInputFilterHost);
            //} catch (RemoteException re) {
                /* ignore */
            //}
        }

        NativeSetInputFilterEnabled(mPtr, filter != NULL);
    }
}

//// @Override // Binder call
ECode CInputManagerService::InjectInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 mode,
    /* [out] */ Boolean* injectIt)
{
    return NOERROR;
}

/**
 * Gets information about the input device with the specified id.
 * @param deviceId The device id.
 * @return The input device or null if not found.
 */
//// @Override // Binder call
ECode CInputManagerService::GetInputDevice(
    /* [in] */ Int32 deviceId,
    /* [out, callee] */ IInputDevice** inputDevice)
{
    return NOERROR;
}

/**
 * Gets the ids of all input devices in the system.
 * @return The input device ids.
 */
//// @Override // Binder call
ECode CInputManagerService::GetInputDeviceIds(
    /* [out, callee] */ ArrayOf<Int32>** deviceIds)
{
    return NOERROR;
}

/**
 * Gets all input devices in the system.
 * @return The array of input devices.
 */
ECode CInputManagerService::GetInputDevices(
    /* [out, callee] */ ArrayOf<IInputDevice*>** inputDevices )
{
    return NOERROR;
}

// @Override // Binder call
ECode CInputManagerService::RegisterInputDevicesChangedListener(
    /* [in] */ IInputDevicesChangedListener* listener)
{
    return NOERROR;
}

// @Override // Binder call & native callback
ECode CInputManagerService::GetTouchCalibrationForInputDevice(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [in] */ Int32 surfaceRotation,
    /* [out] */ ITouchCalibration** inputDevice)
{
    return NOERROR;
}

// @Override // Binder call
ECode CInputManagerService::SetTouchCalibrationForInputDevice(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [in] */ Int32 surfaceRotation,
    /* [in] */ ITouchCalibration* calibration)
{
    return NOERROR;
}

// @Override // Binder call
ECode CInputManagerService::GetKeyboardLayouts(
    /* [out, callee] */ ArrayOf<IKeyboardLayout*>** kbLayouts)
{
    return NOERROR;
}

// @Override // Binder call
ECode CInputManagerService::GetKeyboardLayout(
    /* [in] */ const String& keyboardLayoutDescriptor,
    /* [out, callee] */ IKeyboardLayout** kbLayout)
{
    return NOERROR;
}

// @Override // Binder call
ECode CInputManagerService::GetCurrentKeyboardLayoutForInputDevice(
    /* [in] */ IInputDeviceIdentifier* identifier,
    /* [out, callee] */ String* kbLayout)
{
    return NOERROR;
}

// @Override // Binder call
ECode CInputManagerService::SetCurrentKeyboardLayoutForInputDevice(
    /* [in] */ IInputDeviceIdentifier* identifier,
    /* [in] */ const String& keyboardLayoutDescriptor)
{
    return NOERROR;
}

// @Override // Binder call
ECode CInputManagerService::GetKeyboardLayoutsForInputDevice(
    /* [in] */ IInputDeviceIdentifier* identifier,
    /* [out, callee] */ ArrayOf<String>** kbLayouts)
{
    return NOERROR;
}

// @Override // Binder call
ECode CInputManagerService::AddKeyboardLayoutForInputDevice(
    /* [in] */ IInputDeviceIdentifier* identifier,
    /* [in] */ const String& keyboardLayoutDescriptor)
{
    return NOERROR;
}

// @Override // Binder call
ECode CInputManagerService::RemoveKeyboardLayoutForInputDevice(
    /* [in] */ IInputDeviceIdentifier* identifier,
    /* [in] */ const String& keyboardLayoutDescriptor)
{
    return NOERROR;
}

void CInputManagerService::SwitchKeyboardLayout(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 direction)
{
}

void CInputManagerService::SetInputWindows(
    /* [in] */ ArrayOf<InputWindowHandle*>* windowHandles)
{
}

void CInputManagerService::SetFocusedApplication(
    /* [in] */ InputApplicationHandle* application)
{
}

void CInputManagerService::SetInputDispatchMode(
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean frozen)
{
}

void CInputManagerService::SetSystemUiVisibility(
    /* [in] */ Int32 visibility)
{
}

/**
 * Atomically transfers touch focus from one window to another as identified by
 * their input channels.  It is possible for multiple windows to have
 * touch focus if they support split touch dispatch
 * {@link android.view.WindowManager.LayoutParams#FLAG_SPLIT_TOUCH} but this
 * method only transfers touch focus of the specified window without affecting
 * other windows that may also have touch focus at the same time.
 * @param fromChannel The channel of a window that currently has touch focus.
 * @param toChannel The channel of the window that should receive touch focus in
 * place of the first.
 * @return True if the transfer was successful.  False if the window with the
 * specified channel did not actually have touch focus at the time of the request.
 */
ECode CInputManagerService::TransferTouchFocus(
    /* [in] */ IInputChannel* fromChannel,
    /* [in] */ IInputChannel* toChannel,
    /* [out] */ Boolean* transferIt)
{
    return NOERROR;
}

// @Override // Binder call
ECode CInputManagerService::TryPointerSpeed(
    /* [in] */ Int32 speed)
{
    return NOERROR;
}

void CInputManagerService::UpdatePointerSpeedFromSettings()
{
}

void CInputManagerService::UpdateShowTouchesFromSettings()
{
}

// Binder call
// @Override
ECode CInputManagerService::Vibrate(
    /* [in] */ Int32 deviceId,
    /* [in] */ const ArrayOf<Int64>& pattern,
    /* [in] */ Int32 repeat,
    /* [in] */ IBinder* token)
{
    return NOERROR;
}

// Binder call
// @Override
ECode CInputManagerService::CancelVibrate(
    /* [in] */ Int32 deviceId,
    /* [in] */ IBinder* token)
{
    return NOERROR;
}

// @Override
ECode CInputManagerService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    return NOERROR;
}

// Called by the heartbeat to ensure locks are not held indefinitely (for deadlock detection).
// @Override
ECode CInputManagerService::Monitor()
{
    return NOERROR;
}

ECode CInputManagerService::ToString(
    /* [out] */ String* str)
{
    return NOERROR;
}


void CInputManagerService::ReloadKeyboardLayouts()
{

}

void CInputManagerService::ReloadDeviceAliases()
{

}

void CInputManagerService::SetDisplayViewportsInternal(
    /* [in] */ IDisplayViewport* defaultViewport,
    /* [in] */ IDisplayViewport* externalTouchViewport)
{
}

void CInputManagerService::SetDisplayViewport(
    /* [in] */ Boolean external,
    /* [in] */ IDisplayViewport* viewport)
{
}

ECode CInputManagerService::InjectInputEventInternal(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 mode,
    /* [out] */ Boolean* result)
{
}

void CInputManagerService::OnInputDevicesChangedListenerDied(
    /* [in] */ Int32 pid)
{
}

// Must be called on handler.
void CInputManagerService::DeliverInputDevicesChanged(
    /* [in] */ ArrayOf<IInputDevice*>* oldInputDevices)
{
}

// Must be called on handler.
void CInputManagerService::ShowMissingKeyboardLayoutNotification(
    /* [in] */ IInputDevice* device)
{
}

// Must be called on handler.
void CInputManagerService::HideMissingKeyboardLayoutNotification()
{
}

// Must be called on handler.
void CInputManagerService::UpdateKeyboardLayouts()
{
}

Boolean CInputManagerService::ContainsInputDeviceWithDescriptor(
    /* [in] */ ArrayOf<IInputDevice*>* inputDevices,
    /* [in] */ const String& descriptor)
{
}


void CInputManagerService::VisitAllKeyboardLayouts(
    /* [in] */ IKeyboardLayoutVisitor* visitor)
{
}

void CInputManagerService::VisitKeyboardLayout(
    /* [in] */ const String& keyboardLayoutDescriptor,
    /* [in] */ IKeyboardLayoutVisitor* visitor)
{
}

void CInputManagerService::VisitKeyboardLayoutsInPackage(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IActivityInfo* receiver,
    /* [in] */ const String& keyboardName,
    /* [in] */ Int32 requestedPriority,
    /* [in] */ IKeyboardLayoutVisitor* visitor)
{
}

/**
 * Builds a layout descriptor for the vendor/product. This returns the
 * descriptor for ids that aren't useful (such as the default 0, 0).
 */
ECode CInputManagerService::GetLayoutDescriptor(
    /* [in] */ IInputDeviceIdentifier* identifier,
    /* [out] */ String* layoutDescriptor)
{
}

// Must be called on handler.
void CInputManagerService::HandleSwitchKeyboardLayout(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 direction)
{
}

void CInputManagerService::SetPointerSpeedUnchecked(
    /* [in] */ Int32 speed)
{
}

void CInputManagerService::RegisterPointerSpeedSettingObserver()
{
}

Int32 CInputManagerService::GetPointerSpeedSetting()
{
}

void CInputManagerService::RegisterShowTouchesSettingObserver()
{
}

Int32 CInputManagerService::GetShowTouchesSetting(
    /* [in] */ Int32 defaultValue)
{
}

void CInputManagerService::OnVibratorTokenDied(
    /* [in] */ VibratorToken* v)
{
}

void CInputManagerService::CancelVibrateIfNeeded(
    /* [in] */ VibratorToken* v)
{
}

Boolean CInputManagerService::CheckCallingPermission(
    /* [in] */ const String& permission,
    /* [in] */ const String& func)
{
}

// Native callback.
void CInputManagerService::NotifyConfigurationChanged(
    /* [in] */ Int64 whenNanos)
{
}

// Native callback.
void CInputManagerService::NotifyInputDevicesChanged(
    /* [in] */ ArrayOf<IInputDevice*>* inputDevices)
{
}

// Native callback.
void CInputManagerService::NotifySwitch(
    /* [in] */ Int64 whenNanos,
    /* [in] */ Int32 switchValues,
    /* [in] */ Int32 switchMask)
{
}

// Native callback.
void CInputManagerService::NotifyInputChannelBroken(
    /* [in] */ InputWindowHandle* inputWindowHandle)
{
}

// Native callback.
Int64 CInputManagerService::NotifyANR(
    /* [in] */ InputApplicationHandle* inputApplicationHandle,
    /* [in] */ InputWindowHandle* inputWindowHandle,
    /* [in] */ const String& reason)
{
}

// Native callback.
Boolean CInputManagerService::FilterInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 policyFlags)
{
}

// Native callback.
Int32 CInputManagerService::InterceptKeyBeforeQueueing(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
}

// Native callback.
Int32 CInputManagerService::InterceptMotionBeforeQueueingNonInteractive(
    /* [in] */ Int64 whenNanos,
    /* [in] */ Int32 policyFlags)
{
}

// Native callback.
Int64 CInputManagerService::InterceptKeyBeforeDispatching(
    /* [in] */ InputWindowHandle* focus,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
}

// Native callback.
AutoPtr<IKeyEvent> CInputManagerService::DispatchUnhandledKey(
    /* [in] */ InputWindowHandle* focus,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
    return NULL;
}

// Native callback.
Boolean CInputManagerService::CheckInjectEventsPermission(
    /* [in] */ Int32 injectorPid,
    /* [in] */ Int32 injectorUid)
{
}

// Native callback.
Int32 CInputManagerService::GetVirtualKeyQuietTimeMillis()
{
}

// Native callback.
AutoPtr< ArrayOf<String> > CInputManagerService::GetExcludedDeviceNames()
{
}

// Native callback.
Int32 CInputManagerService::GetKeyRepeatTimeout()
{
}

// Native callback.
Int32 CInputManagerService::GetKeyRepeatDelay()
{
}

// Native callback.
Int32 CInputManagerService::GetHoverTapTimeout()
{
}

// Native callback.
Int32 CInputManagerService::GetHoverTapSlop()
{
}

// Native callback.
Int32 CInputManagerService::GetDoubleTapTimeout()
{
}

// Native callback.
Int32 CInputManagerService::GetLongPressTimeout()
{
}

// Native callback.
Int32 CInputManagerService::GetPointerLayer()
{
}

// Native callback.
AutoPtr<IPointerIcon> CInputManagerService::GetPointerIcon()
{
}

// Native callback.
AutoPtr< ArrayOf<String> > CInputManagerService::GetKeyboardLayoutOverlay(
    /* [in] */ IInputDeviceIdentifier* identifier)
{
}

// Native callback.
String CInputManagerService::GetDeviceAlias(
    /* [in] */ const String& uniqueId)
{
}

} // Input
} // Server
} // Droid
} // Elastos
