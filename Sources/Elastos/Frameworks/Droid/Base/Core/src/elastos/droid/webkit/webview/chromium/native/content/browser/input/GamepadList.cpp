
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/GamepadList.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/GamepadList_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Droid::Hardware::Input::EIID_IInputDeviceListener;
using Elastos::Droid::Os::Build;
using Elastos::Droid::View::IInputDeviceHelper;
//TODO using Elastos::Droid::View::CInputDeviceHelper;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::EIID_IInputEvent;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

//==================================================================
//              GamepadList::InnerInputDeviceListener
//==================================================================

CAR_INTERFACE_IMPL(GamepadList::InnerInputDeviceListener, Object, IInputDeviceListener);

GamepadList::InnerInputDeviceListener::InnerInputDeviceListener(
    /* [in] */ GamepadList* owner)
    : mOwner(owner)
{
}

//@Override
ECode GamepadList::InnerInputDeviceListener::OnInputDeviceChanged(
    /* [in] */ Int32 deviceId)
{
    mOwner->OnInputDeviceChangedImpl(deviceId);
    return NOERROR;
}

//@Override
ECode GamepadList::InnerInputDeviceListener::OnInputDeviceRemoved(
    /* [in] */ Int32 deviceId)
{
    mOwner->OnInputDeviceRemovedImpl(deviceId);
    return NOERROR;
}

//@Override
ECode GamepadList::InnerInputDeviceListener::OnInputDeviceAdded(
    /* [in] */ Int32 deviceId)
{
    mOwner->OnInputDeviceAddedImpl(deviceId);
    return NOERROR;
}

//==================================================================
//                      GamepadList::LazyHolder
//==================================================================

const AutoPtr<GamepadList> GamepadList::LazyHolder::INSTANCE = new GamepadList();

//==================================================================
//                            GamepadList
//==================================================================

const Int32 GamepadList::MAX_GAMEPADS;

//@TargetApi(Build.VERSION_CODES.JELLY_BEAN)
GamepadList::GamepadList()
    : mAttachedToWindowCounter(0)
    , mIsGamepadAccessed(FALSE)
{
    mGamepadDevices = ArrayOf<GamepadDevice*>::Alloc(MAX_GAMEPADS);
    mInputDeviceListener = new InnerInputDeviceListener(this);
}

//@TargetApi(Build.VERSION_CODES.JELLY_BEAN)
void GamepadList::InitializeDevices()
{
    // Get list of all the attached input devices.
    AutoPtr< ArrayOf<Int32> > deviceIds;
    mInputManager->GetInputDeviceIds((ArrayOf<Int32>**)&deviceIds);
    AutoPtr<IInputDeviceHelper> helper;
    //TODO CInputDeviceHelper::AcquireSingleton((IInputDeviceHelper**)&helper);
    for (Int32 i = 0; i < deviceIds->GetLength(); i++) {
        AutoPtr<IInputDevice> inputDevice;
        helper->GetDevice((*deviceIds)[i], (IInputDevice**)&inputDevice);
        // Check for gamepad device
        if (IsGamepadDevice(inputDevice)) {
            // Register a new gamepad device.
            RegisterGamepad(inputDevice);
        }
    }
}

/**
 * Notifies the GamepadList that a {@link ContentView} is attached to a window and it should
 * prepare itself for gamepad input. It must be called before {@link onGenericMotionEvent} and
 * {@link dispatchKeyEvent}.
 */
void GamepadList::OnAttachedToWindow(
    /* [in] */ IContext* context)
{
    assert(ThreadUtils::RunningOnUiThread());
    if (!IsGamepadSupported()) return;
    GetInstance()->AttachedToWindow(context);
}

//@TargetApi(Build.VERSION_CODES.JELLY_BEAN)
void GamepadList::AttachedToWindow(
    /* [in] */ IContext* context)
{
    if (mAttachedToWindowCounter++ == 0) {
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::INPUT_SERVICE, (IInterface**)&obj);
        mInputManager = IInputManager::Probe(obj);
        {
            AutoLock lock(mLock);
            InitializeDevices();
        }
        // Register an input device listener.
        mInputManager->RegisterInputDeviceListener(mInputDeviceListener, NULL);
    }
}

/**
 * Notifies the GamepadList that a {@link ContentView} is detached from it's window.
 */
//@SuppressLint("MissingSuperCall")
void GamepadList::OnDetachedFromWindow()
{
    assert(ThreadUtils::RunningOnUiThread());
    if (!IsGamepadSupported()) return;
    GetInstance()->DetachedFromWindow();
}

//@TargetApi(Build.VERSION_CODES.JELLY_BEAN)
void GamepadList::DetachedFromWindow()
{
    if (--mAttachedToWindowCounter == 0) {
        {
            AutoLock lock(mLock);
            for (Int32 i = 0; i < MAX_GAMEPADS; ++i) {
                (*mGamepadDevices)[i] = NULL;
            }
        }

        mInputManager->UnregisterInputDeviceListener(mInputDeviceListener);
        mInputManager = NULL;
    }
}

// ------------------------------------------------------------

void GamepadList::OnInputDeviceChangedImpl(
    /* [in] */ Int32 deviceId)
{
}

void GamepadList::OnInputDeviceRemovedImpl(
    /* [in] */ Int32 deviceId)
{
    AutoLock lock(mLock);
    UnregisterGamepad(deviceId);
}

void GamepadList::OnInputDeviceAddedImpl(
    /* [in] */ Int32 deviceId)
{
    AutoPtr<IInputDeviceHelper> helper;
    //TODO CInputDeviceHelper::AcquireSingleton((IInputDeviceHelper**)&helper);
    AutoPtr<IInputDevice> inputDevice;
    helper->GetDevice(deviceId, (IInputDevice**)&helper);
    if (!IsGamepadDevice(inputDevice)) return;

    {
        AutoLock lock(mLock);
        RegisterGamepad(inputDevice);
    }
}

// ------------------------------------------------------------

AutoPtr<GamepadList> GamepadList::GetInstance()
{
    assert(IsGamepadSupported());
    return LazyHolder::INSTANCE;
}

Int32 GamepadList::GetDeviceCount()
{
    Int32 count = 0;
    for (Int32 i = 0; i < MAX_GAMEPADS; i++) {
        if (GetDevice(i) != NULL) {
            count++;
        }
    }

    return count;
}

Boolean GamepadList::IsDeviceConnected(
    /* [in] */ Int32 index)
{
    if (index < MAX_GAMEPADS && GetDevice(index) != NULL) {
        return TRUE;
    }

    return FALSE;
}

AutoPtr<GamepadDevice> GamepadList::GetDeviceById(
    /* [in] */ Int32 deviceId)
{
    for (Int32 i = 0; i < MAX_GAMEPADS; i++) {
        AutoPtr<GamepadDevice> gamepad = (*mGamepadDevices)[i];
        if (gamepad != NULL && gamepad->GetId() == deviceId) {
            return gamepad;
        }
    }

    return NULL;
}

AutoPtr<GamepadDevice> GamepadList::GetDevice(
    /* [in] */ Int32 index)
{
    // Maximum 4 Gamepads can be connected at a time starting at index zero.
    assert(index >= 0 && index < MAX_GAMEPADS);
    return (*mGamepadDevices)[index];
}

/**
 * Handles key events from the gamepad devices.
 * @return True if the event has been consumed.
 */
Boolean GamepadList::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    if (!IsGamepadSupported()) return FALSE;
    if (!IsGamepadEvent(event)) return FALSE;
    return GetInstance()->HandleKeyEvent(event);
}

Boolean GamepadList::HandleKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    AutoLock lock(mLock);

    if (!mIsGamepadAccessed) return FALSE;
    AutoPtr<GamepadDevice> gamepad = GetGamepadForEvent(IInputEvent::Probe(event));
    if (gamepad == NULL) return FALSE;
    return gamepad->HandleKeyEvent(event);
}

/**
 * Handles motion events from the gamepad devices.
 * @return True if the event has been consumed.
 */
Boolean GamepadList::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    if (!IsGamepadSupported()) return FALSE;
    if (!IsGamepadEvent(event)) return FALSE;
    return GetInstance()->HandleMotionEvent(event);
}

Boolean GamepadList::HandleMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    AutoLock lock(mLock);

    if (!mIsGamepadAccessed) return FALSE;
    AutoPtr<GamepadDevice> gamepad = GetGamepadForEvent(IInputEvent::Probe(event));
    if (gamepad == NULL) return FALSE;
    return gamepad->HandleMotionEvent(event);
}

Int32 GamepadList::GetNextAvailableIndex()
{
    // When multiple gamepads are connected to a user agent, indices must be assigned on a
    // first-come first-serve basis, starting at zero. If a gamepad is disconnected, previously
    // assigned indices must not be reassigned to gamepads that continue to be connected.
    // However, if a gamepad is disconnected, and subsequently the same or a different
    // gamepad is then connected, index entries must be reused.

    for (Int32 i = 0; i < MAX_GAMEPADS; ++i) {
        if (GetDevice(i) == NULL) {
            return i;
        }
    }

    // Reached maximum gamepads limit.
    return -1;
}

Boolean GamepadList::RegisterGamepad(
    /* [in] */ IInputDevice* inputDevice)
{
    Int32 index = GetNextAvailableIndex();
    if (index == -1) return FALSE; // invalid index

    AutoPtr<GamepadDevice> gamepad = new GamepadDevice(index, inputDevice);
    (*mGamepadDevices)[index] = gamepad;

    return TRUE;
}

void GamepadList::UnregisterGamepad(
    /* [in] */ Int32 deviceId)
{
    AutoPtr<GamepadDevice> gamepadDevice = GetDeviceById(deviceId);
    if (gamepadDevice == NULL) return; // Not a registered device.
    Int32 index = gamepadDevice->GetIndex();
    (*mGamepadDevices)[index] = NULL;
}

Boolean GamepadList::IsGamepadDevice(
    /* [in] */ IInputDevice* inputDevice)
{
    Int32 sources;
    inputDevice->GetSources(&sources);
    return ((sources & IInputDevice::SOURCE_JOYSTICK) ==
            IInputDevice::SOURCE_JOYSTICK);
}

AutoPtr<GamepadDevice> GamepadList::GetGamepadForEvent(
    /* [in] */ IInputEvent* event)
{
    Int32 id;
    event->GetDeviceId(&id);
    return GetDeviceById(id);
}

/**
 * @return True if the motion event corresponds to a gamepad event.
 */
Boolean GamepadList::IsGamepadEvent(
    /* [in] */ IMotionEvent* event)
{
    AutoPtr<IInputEvent> inputEvent = IInputEvent::Probe(event);
    Int32 source;
    inputEvent->GetSource(&source);
    return ((source & IInputDevice::SOURCE_JOYSTICK) == IInputDevice::SOURCE_JOYSTICK);
}

/**
 * @return True if event's keycode corresponds to a gamepad key.
 */
Boolean GamepadList::IsGamepadEvent(
    /* [in] */ IKeyEvent* event)
{
    Int32 keyCode;
    event->GetKeyCode(&keyCode);
    switch (keyCode) {
    // Specific handling for dpad keys is required because
    // KeyEvent.isGamepadButton doesn't consider dpad keys.
        case IKeyEvent::KEYCODE_DPAD_UP:
        case IKeyEvent::KEYCODE_DPAD_DOWN:
        case IKeyEvent::KEYCODE_DPAD_LEFT:
        case IKeyEvent::KEYCODE_DPAD_RIGHT:
            return TRUE;
        default: {
            AutoPtr<IKeyEventHelper> helper;
            CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
            Boolean bFlag;
            helper->IsGamepadButton(keyCode, &bFlag);
            return bFlag;
        }
    }
}

Boolean GamepadList::IsGamepadSupported()
{
    return Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN;
}

//@CalledByNative
void GamepadList::UpdateGamepadData(
    /* [in] */ Int64 webGamepadsPtr)
{
    if (!IsGamepadSupported()) return;
    GetInstance()->GrabGamepadData(webGamepadsPtr);
}

void GamepadList::GrabGamepadData(
    /* [in] */ Int64 webGamepadsPtr)
{
    AutoLock lock(mLock);

    for (Int32 i = 0; i < MAX_GAMEPADS; i++) {
        const AutoPtr<GamepadDevice> device = GetDevice(i);
        if (device != NULL) {
            device->UpdateButtonsAndAxesMapping();
            NativeSetGamepadData(webGamepadsPtr, i, device->IsStandardGamepad(), TRUE,
                    device->GetName(), device->GetTimestamp(), device->GetAxes(),
                    device->GetButtons());
        }
        else {
            NativeSetGamepadData(webGamepadsPtr, i, FALSE, FALSE, String(NULL), 0, NULL, NULL);
        }
    }
}

//@CalledByNative
void GamepadList::NotifyForGamepadsAccess(
    /* [in] */ Boolean isAccessPaused)
{
    if (!IsGamepadSupported()) return;
    GetInstance()->SetIsGamepadAccessed(!isAccessPaused);
}

void GamepadList::SetIsGamepadAccessed(
    /* [in] */ Boolean isGamepadAccessed)
{
    AutoLock lock(mLock);
    mIsGamepadAccessed = isGamepadAccessed;
    if (isGamepadAccessed) {
        for (Int32 i = 0; i < MAX_GAMEPADS; i++) {
            AutoPtr<GamepadDevice> gamepadDevice = GetDevice(i);
            if (gamepadDevice == NULL) continue;
            gamepadDevice->ClearData();
        }
    }
}

void GamepadList::NativeSetGamepadData(
    /* [in] */ Int64 webGamepadsPtr,
    /* [in] */ Int32 index,
    /* [in] */ Boolean mapping,
    /* [in] */ Boolean connected,
    /* [in] */ const String& devicename,
    /* [in] */ Int64 timestamp,
    /* [in] */ ArrayOf<Float>* axes,
    /* [in] */ ArrayOf<Float>* buttons)
{
    Elastos_GamepadList_nativeSetGamepadData(TO_IINTERFACE(this), webGamepadsPtr,
            index, mapping, connected, devicename, timestamp, axes, buttons);
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
