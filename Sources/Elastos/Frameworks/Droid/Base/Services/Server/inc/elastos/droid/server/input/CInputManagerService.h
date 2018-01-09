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

#ifndef __ELASTOS_DROID_SERVICE_INPUT_INPUT_CINPUTMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVICE_INPUT_INPUT_CINPUTMANAGERSERVICE_H__

#include "_Elastos.Droid.Server.h"
#include "_Elastos_Droid_Server_Input_CInputManagerService.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/server/input/InputApplicationHandle.h"
#include "elastos/droid/server/input/InputWindowHandle.h"
#include "elastos/droid/server/input/NativeInputManager.h"
#include "elastos/droid/server/input/PersistentDataStore.h"
#include "elastos/core/Object.h"

using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Hardware::Display::IDisplayViewport;
using Elastos::Droid::Hardware::Input::IInputDevicesChangedListener;
using Elastos::Droid::Hardware::Input::IKeyboardLayout;
using Elastos::Droid::Hardware::Input::ITouchCalibration;
using Elastos::Droid::Hardware::Input::IInputDevicesChangedListener;
using Elastos::Droid::Hardware::Input::IInputDeviceIdentifier;
using Elastos::Droid::Hardware::Input::IIInputManager;
using Elastos::Droid::Hardware::Input::IInputManagerInternal;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IIInputFilter;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IIInputFilterHost;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IPointerIcon;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::Widget::IToast;
using Elastos::Core::Object;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;

// these macros are defined in .../Linux/usr/include/linux/input.h
#ifdef BTN_MOUSE
#undef BTN_MOUSE
#undef SW_LID
#undef SW_KEYPAD_SLIDE
#undef SW_HEADPHONE_INSERT
#undef SW_MICROPHONE_INSERT
#undef SW_LINEOUT_INSERT
#undef SW_JACK_PHYSICAL_INSERT
#undef SW_CAMERA_LENS_COVER
#endif

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

class CInputFilterHost;

/*
 * Wraps the C++ InputManager and provides its callbacks.
 * public class InputManagerService extends IInputManager.Stub
 *       implements Watchdog.Monitor {
 */
CarClass(CInputManagerService)
    , public Object
    , public IIInputManager
    , public IWatchdogMonitor
    , public IBinder
{
    friend class NativeInputManager;

public:
    /**
     * Hosting interface for input filters to call back into the input manager.
     */
    class InputFilterHost
        : public Object
        , public IIInputFilterHost
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        InputFilterHost();

        virtual ~InputFilterHost();

        CARAPI constructor(
            /* [in] */ IIInputManager* host);

        CARAPI_(void) DisconnectLocked();

        CARAPI SendInputEvent(
            /* [in] */ IInputEvent* event,
            /* [in] */ Int32 policyFlags);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        Boolean mDisconnected;
        CInputManagerService* mHost;
    };

    /**
     * Hosting interface for input filters to call back into the input manager.
     */
    class ChainedInputFilterHost
        : public Object
        , public IIInputFilterHost
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        ChainedInputFilterHost();

        virtual ~ChainedInputFilterHost();

        CARAPI constructor(
            /* [in] */ IIInputFilter* filter,
            /* [in] */ IIInputFilterHost* next,
            /* [in] */ IIInputManager* host);

        CARAPI_(void) ConnectLocked();

        CARAPI_(void) DisconnectLocked();

        CARAPI SendInputEvent(
            /* [in] */ IInputEvent* event,
            /* [in] */ Int32 policyFlags);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        friend class CInputManagerService;
        AutoPtr<IIInputFilter> mInputFilter;
        AutoPtr<ChainedInputFilterHost> mNext;
        Boolean mDisconnected;
        CInputManagerService* mHost;
    };

private:
    /**
     * Private handler for the input manager.
     */
    class InputManagerHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CInputManagerService::InputManagerHandler")

        InputManagerHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CInputManagerService* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CInputManagerService* mHost;
    };

    class KeyboardLayoutDescriptor
        : public Object
    {
    public:
        static CARAPI_(String) Format(
            /* [in] */ const String& packageName,
            /* [in] */ const String& receiverName,
            /* [in] */ const String& keyboardName);

        static CARAPI_(AutoPtr<KeyboardLayoutDescriptor>) Parse(
            /* [in] */ const String& descriptor);

    public:
        String mPackageName;
        String mReceiverName;
        String mKeyboardLayoutName;
    };

    class InputDevicesChangedListenerRecord
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        InputDevicesChangedListenerRecord(
            /* [in] */ Int32 pid,
            /* [in] */ IInputDevicesChangedListener* listener,
            /* [in] */ CInputManagerService* host);

        CAR_INTERFACE_DECL()

        //@Override
        CARAPI ProxyDied();

        CARAPI_(void) NotifyInputDevicesChanged(
            /* [in] */ ArrayOf<Int32>* info);

    private:
        Int32 mPid;
        AutoPtr<IInputDevicesChangedListener> mListener;
        CInputManagerService* mHost;
    };

    class VibratorToken
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        VibratorToken(
            /* [in] */ Int32 deviceId,
            /* [in] */ IBinder* token,
            /* [in] */ Int32 tokenValue,
            /* [in] */ CInputManagerService* host);

        //@Override
        CARAPI ProxyDied();

    public:
        Int32 mDeviceId;
        AutoPtr<IBinder> mToken;
        Int32 mTokenValue;

        Boolean mVibrating;

    private:
        CInputManagerService* mHost;
    };

    class LocalService
        : public Object
        , public IInputManagerInternal
    {
    public:
        CAR_INTERFACE_DECL()

        LocalService(
            /* [in] */ CInputManagerService* host);

        //@Override
        CARAPI SetDisplayViewports(
            /* [in] */ IDisplayViewport* defaultViewport,
            /* [in] */ IDisplayViewport* externalTouchViewport);

        CARAPI InjectInputEvent(
            /* [in] */ IInputEvent* event,
            /* [in] */ Int32 displayId,
            /* [in] */ Int32 mode,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI SetInteractive(
            /* [in] */ Boolean interactive);

    private:
        CInputManagerService* mHost;
    };

private:
    class BroadcastReceiverInStart
        : public BroadcastReceiver
    {
    public:
        BroadcastReceiverInStart(
            /* [in] */ CInputManagerService* owner);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = "CInputManagerService::BroadcastReceiverInStart: ";
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        CInputManagerService* mOwner;
    };

    class BroadcastReceiverPackage
        : public BroadcastReceiver
    {
    public:
        BroadcastReceiverPackage(
            /* [in] */ CInputManagerService* owner);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = "CInputManagerService::BroadcastReceiverPackage: ";
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        CInputManagerService* mOwner;
    };

    class BroadcastReceiverAlias
        : public BroadcastReceiver
    {
    public:
        BroadcastReceiverAlias(
            /* [in] */ CInputManagerService* owner);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = "CInputManagerService::BroadcastReceiverAlias: ";
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        CInputManagerService* mOwner;
    };

    class KeyboardLayoutVisitorInUpdateKeyboardLayouts
        : public Object
        , public IKeyboardLayoutVisitor
    {
    public:
        CAR_INTERFACE_DECL()

        KeyboardLayoutVisitorInUpdateKeyboardLayouts(
            /* [in] */ IHashSet* availableKeyboardLayouts);

        CARAPI VisitKeyboardLayout(
            /* [in] */ IResources* resources,
            /* [in] */ const String& descriptor,
            /* [in] */ const String& label,
            /* [in] */ const String& collection,
            /* [in] */ Int32 keyboardLayoutResId,
            /* [in] */ Int32 priority);

    private:
        AutoPtr<IHashSet> mAvailableKeyboardLayouts;
    };

    class KeyboardLayoutVisitorInGetKeyboardLayouts
        : public Object
        , public IKeyboardLayoutVisitor
    {
    public:
        CAR_INTERFACE_DECL()

        KeyboardLayoutVisitorInGetKeyboardLayouts(
            /* [in] */ IArrayList* list);

        CARAPI VisitKeyboardLayout(
            /* [in] */ IResources* resources,
            /* [in] */ const String& descriptor,
            /* [in] */ const String& label,
            /* [in] */ const String& collection,
            /* [in] */ Int32 keyboardLayoutResId,
            /* [in] */ Int32 priority);

    private:
        AutoPtr<IArrayList> mList;
    };

    class KeyboardLayoutVisitorInGetKeyboardLayout
        : public Object
        , public IKeyboardLayoutVisitor
    {
    public:
        CAR_INTERFACE_DECL()

        KeyboardLayoutVisitorInGetKeyboardLayout(
            /* [in] */ ArrayOf<IKeyboardLayout*>* layouts);

        CARAPI VisitKeyboardLayout(
            /* [in] */ IResources* resources,
            /* [in] */ const String& descriptor,
            /* [in] */ const String& label,
            /* [in] */ const String& collection,
            /* [in] */ Int32 keyboardLayoutResId,
            /* [in] */ Int32 priority);

    private:
        AutoPtr< ArrayOf<IKeyboardLayout*> > mLayouts;
    };

    class KeyboardLayoutVisitorInGetKeyboardLayoutOverlay
        : public Object
        , public IKeyboardLayoutVisitor
    {
    public:
        CAR_INTERFACE_DECL()

        KeyboardLayoutVisitorInGetKeyboardLayoutOverlay(
            /* [in] */ ArrayOf<String>* layoutOverlays);

        CARAPI VisitKeyboardLayout(
            /* [in] */ IResources* resources,
            /* [in] */ const String& descriptor,
            /* [in] */ const String& label,
            /* [in] */ const String& collection,
            /* [in] */ Int32 keyboardLayoutResId,
            /* [in] */ Int32 priority);

    private:
        AutoPtr< ArrayOf<String> > mLayoutOverlays;
    };

    class ContentObserverInRegisterPointerSpeedSettingObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CInputManagerService::ContentObserverInRegisterPointerSpeedSettingObserver")

        CARAPI constructor(
            /* [in] */ CInputManagerService* owner,
            /* [in] */ IHandler* handler);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        CInputManagerService* mOwner;
    };

    class ContentObserverInRegisterShowTouchesSettingObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CInputManagerService::ContentObserverInRegisterShowTouchesSettingObserver")

        CARAPI constructor(
            /* [in] */ CInputManagerService* owner,
            /* [in] */ IHandler* handler);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        CInputManagerService* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CInputManagerService();

    virtual ~CInputManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI_(void) SetWindowManagerCallbacks(
        /* [in] */ IWindowManagerCallbacks* cbacks);

    CARAPI_(void) SetWiredAccessoryCallbacks(
        /* [in] */ IWiredAccessoryCallbacks* cbacks);

    CARAPI Start();

    // TODO(BT) Pass in paramter for bluetooth system
    CARAPI SystemRunning();

    /**
     * Gets the current state of a key or button by key code.
     * @param deviceId The input device id, or -1 to consult all devices.
     * @param sourceMask The input sources to consult, or {@link InputDevice#SOURCE_ANY} to
     * consider all input sources.  An input device is consulted if at least one of its
     * non-class input source bits matches the specified source mask.
     * @param keyCode The key code to check.
     * @return The key state.
     */
    CARAPI_(Int32) GetKeyCodeState(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 sourceMask,
        /* [in] */ Int32 keyCode);

    /**
     * Gets the current state of a key or button by scan code.
     * @param deviceId The input device id, or -1 to consult all devices.
     * @param sourceMask The input sources to consult, or {@link InputDevice#SOURCE_ANY} to
     * consider all input sources.  An input device is consulted if at least one of its
     * non-class input source bits matches the specified source mask.
     * @param scanCode The scan code to check.
     * @return The key state.
     */
    CARAPI_(Int32) GetScanCodeState(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 sourceMask,
        /* [in] */ Int32 scanCode);

    /**
     * Gets the current state of a switch by switch code.
     * @param deviceId The input device id, or -1 to consult all devices.
     * @param sourceMask The input sources to consult, or {@link InputDevice#SOURCE_ANY} to
     * consider all input sources.  An input device is consulted if at least one of its
     * non-class input source bits matches the specified source mask.
     * @param switchCode The switch code to check.
     * @return The switch state.
     */
    CARAPI_(Int32) GetSwitchState(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 sourceMask,
        /* [in] */ Int32 switchCode);

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
    CARAPI HasKeys(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 sourceMask,
        /* [in] */ const ArrayOf<Int32>& keyCodes,
        /* [in] */ ArrayOf<Boolean>* keyExists,
        /* [out] */ Boolean* hasKeys);

    /**
     * Creates an input channel that will receive all input from the input dispatcher.
     * @param inputChannelName The input channel name.
     * @return The input channel.
     */
    CARAPI MonitorInput(
        /* [in] */ const String& inputChannelName,
        /* [out] */ IInputChannel** ic);

    /**
     * Registers an input channel so that it can be used as an input event target.
     * @param inputChannel The input channel to register.
     * @param inputWindowHandle The handle of the input window associated with the
     * input channel, or null if none.
     */
    CARAPI RegisterInputChannel(
        /* [in] */ IInputChannel* inputChannel,
        /* [in] */ IInputWindowHandle* inputWindowHandle);

    /**
     * Unregisters an input channel.
     * @param inputChannel The input channel to unregister.
     */
    CARAPI UnregisterInputChannel(
        /* [in] */ IInputChannel* inputChannel);

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
    CARAPI_(void) SetInputFilter(
        /* [in] */ IIInputFilter* filter);

    /**
     * Registers a secondary input filter. These filters are always behind the "original"
     * input filter. This ensures that all input events will be filtered by the
     * {@code AccessibilityManagerService} first.
     * <p>
     * <b>Note:</b> Even though this implementation using AIDL interfaces, it is designed to only
     * provide direct access. Therefore, any filter registering should reside in the
     * system server DVM only!
     *
     * @param filter The input filter to register.
     */
    CARAPI_(void) RegisterSecondaryInputFilter(
        /* [in] */ IIInputFilter* filter);

    CARAPI_(void) UnregisterSecondaryInputFilter(
        /* [in] */ IIInputFilter* filter);

    //// @Override // Binder call
    CARAPI InjectInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 mode,
        /* [out] */ Boolean* injectIt);

    /**
     * Gets information about the input device with the specified id.
     * @param deviceId The device id.
     * @return The input device or null if not found.
     */
    //// @Override // Binder call
    CARAPI GetInputDevice(
        /* [in] */ Int32 deviceId,
        /* [out] */ IInputDevice** inputDevice);

    /**
     * Gets the ids of all input devices in the system.
     * @return The input device ids.
     */
    //// @Override // Binder call
    CARAPI GetInputDeviceIds(
        /* [out, callee] */ ArrayOf<Int32>** deviceIds);

    /**
     * Gets all input devices in the system.
     * @return The array of input devices.
     */
    CARAPI GetInputDevices(
        /* [out, callee] */ ArrayOf<IInputDevice*>** inputDevices );

    // @Override // Binder call
    CARAPI RegisterInputDevicesChangedListener(
        /* [in] */ IInputDevicesChangedListener* listener);

    // @Override // Binder call & native callback
    CARAPI GetTouchCalibrationForInputDevice(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ Int32 surfaceRotation,
        /* [out] */ ITouchCalibration** inputDevice);

    // @Override // Binder call
    CARAPI SetTouchCalibrationForInputDevice(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ Int32 surfaceRotation,
        /* [in] */ ITouchCalibration* calibration);

    // @Override // Binder call
    CARAPI GetKeyboardLayouts(
        /* [out, callee] */ ArrayOf<IKeyboardLayout*>** kbLayouts);

    // @Override // Binder call
    CARAPI GetKeyboardLayout(
        /* [in] */ const String& keyboardLayoutDescriptor,
        /* [out] */ IKeyboardLayout** kbLayout);

    // @Override // Binder call
    CARAPI GetCurrentKeyboardLayoutForInputDevice(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [out] */ String* kbLayout);

    // @Override // Binder call
    CARAPI SetCurrentKeyboardLayoutForInputDevice(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [in] */ const String& keyboardLayoutDescriptor);

    // @Override // Binder call
    CARAPI GetKeyboardLayoutsForInputDevice(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [out, callee] */ ArrayOf<String>** kbLayouts);

    // @Override // Binder call
    CARAPI AddKeyboardLayoutForInputDevice(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [in] */ const String& keyboardLayoutDescriptor);

    // @Override // Binder call
    CARAPI RemoveKeyboardLayoutForInputDevice(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [in] */ const String& keyboardLayoutDescriptor);

    CARAPI_(void) SwitchKeyboardLayout(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 direction);

    CARAPI_(void) SetInputWindows(
        /* [in] */ ArrayOf<IInputWindowHandle*>* windowHandles);

    CARAPI_(void) SetFocusedApplication(
        /* [in] */ InputApplicationHandle* application);

    CARAPI_(void) SetInputDispatchMode(
        /* [in] */ Boolean enabled,
        /* [in] */ Boolean frozen);

    CARAPI_(void) SetSystemUiVisibility(
        /* [in] */ Int32 visibility);

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
    CARAPI TransferTouchFocus(
        /* [in] */ IInputChannel* fromChannel,
        /* [in] */ IInputChannel* toChannel,
        /* [out] */ Boolean* result);

    // @Override // Binder call
    CARAPI TryPointerSpeed(
        /* [in] */ Int32 speed);

    CARAPI_(void) UpdatePointerSpeedFromSettings();

    CARAPI_(void) UpdateShowTouchesFromSettings();

    // Binder call
    // @Override
    CARAPI Vibrate(
        /* [in] */ Int32 deviceId,
        /* [in] */ const ArrayOf<Int64>& pattern,
        /* [in] */ Int32 repeat,
        /* [in] */ IBinder* token);

    // Binder call
    // @Override
    CARAPI CancelVibrate(
        /* [in] */ Int32 deviceId,
        /* [in] */ IBinder* token);

    CARAPI_(void) OnVibratorTokenDied(
        /* [in] */ VibratorToken* v);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    // Called by the heartbeat to ensure locks are not held indefinitely (for deadlock detection).
    // @Override
    CARAPI Monitor();

    // Native callback.
    CARAPI_(Boolean) FilterInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 policyFlags);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(void) ReloadKeyboardLayouts();

    CARAPI_(void) ReloadDeviceAliases();

    CARAPI_(void) SetDisplayViewportsInternal(
        /* [in] */ IDisplayViewport* defaultViewport,
        /* [in] */ IDisplayViewport* externalTouchViewport);

    CARAPI_(void) SetDisplayViewport(
        /* [in] */ Boolean external,
        /* [in] */ IDisplayViewport* viewport);

    CARAPI InjectInputEventInternal(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 mode,
        /* [out] */ Boolean* result);

    CARAPI_(void) OnInputDevicesChangedListenerDied(
        /* [in] */ Int32 pid);

    // Must be called on handler.
    CARAPI_(void) DeliverInputDevicesChanged(
        /* [in] */ ArrayOf<IInputDevice*>* oldInputDevices);

    // Must be called on handler.
    CARAPI_(void) ShowMissingKeyboardLayoutNotification(
        /* [in] */ IInputDevice* device);

    // Must be called on handler.
    CARAPI_(void) HideMissingKeyboardLayoutNotification();

    // Must be called on handler.
    CARAPI_(void) UpdateKeyboardLayouts();

    static CARAPI_(Boolean) ContainsInputDeviceWithDescriptor(
        /* [in] */ ArrayOf<IInputDevice*>* inputDevices,
        /* [in] */ const String& descriptor);

    CARAPI_(void) VisitAllKeyboardLayouts(
        /* [in] */ IKeyboardLayoutVisitor* visitor);

    CARAPI_(void) VisitKeyboardLayout(
        /* [in] */ const String& keyboardLayoutDescriptor,
        /* [in] */ IKeyboardLayoutVisitor* visitor);

    CARAPI_(void) VisitKeyboardLayoutsInPackage(
        /* [in] */ IPackageManager* pm,
        /* [in] */ IActivityInfo* receiver,
        /* [in] */ const String& keyboardName,
        /* [in] */ Int32 requestedPriority,
        /* [in] */ IKeyboardLayoutVisitor* visitor);

    /**
     * Builds a layout descriptor for the vendor/product. This returns the
     * descriptor for ids that aren't useful (such as the default 0, 0).
     */
    CARAPI GetLayoutDescriptor(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [out] */ String* layoutDescriptor);

    // Must be called on handler.
    CARAPI_(void) HandleSwitchKeyboardLayout(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 direction);

    CARAPI_(void) SetPointerSpeedUnchecked(
        /* [in] */ Int32 speed);

    CARAPI_(void) RegisterPointerSpeedSettingObserver();

    CARAPI_(Int32) GetPointerSpeedSetting();

    CARAPI_(void) RegisterShowTouchesSettingObserver();

    CARAPI_(Int32) GetShowTouchesSetting(
        /* [in] */ Int32 defaultValue);

    CARAPI_(void) CancelVibrateIfNeeded(
        /* [in] */ VibratorToken* v);

    CARAPI_(Boolean) CheckCallingPermission(
        /* [in] */ const String& permission,
        /* [in] */ const String& func);

    // Native callback.
    CARAPI_(void) NotifyConfigurationChanged(
        /* [in] */ Int64 whenNanos);

    // Native callback.
    CARAPI_(void) NotifyInputDevicesChanged(
        /* [in] */ ArrayOf<IInputDevice*>* inputDevices);

    // Native callback.
    CARAPI_(void) NotifySwitch(
        /* [in] */ Int64 whenNanos,
        /* [in] */ Int32 switchValues,
        /* [in] */ Int32 switchMask);

    // Native callback.
    CARAPI_(void) NotifyInputChannelBroken(
        /* [in] */ InputWindowHandle* inputWindowHandle);

    // Native callback.
    CARAPI_(Int64) NotifyANR(
        /* [in] */ InputApplicationHandle* inputApplicationHandle,
        /* [in] */ InputWindowHandle* inputWindowHandle,
        /* [in] */ const String& reason);

    // Native callback.
    CARAPI_(Int32) InterceptKeyBeforeQueueing(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags);

    // Native callback.
    CARAPI_(Int32) InterceptMotionBeforeQueueingNonInteractive(
        /* [in] */ Int64 whenNanos,
        /* [in] */ Int32 policyFlags);

    // Native callback.
    CARAPI_(Int64) InterceptKeyBeforeDispatching(
        /* [in] */ InputWindowHandle* focus,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags);

    // Native callback.
    CARAPI_(AutoPtr<IKeyEvent>) DispatchUnhandledKey(
        /* [in] */ InputWindowHandle* focus,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags);

    // Native callback.
    CARAPI_(Boolean) CheckInjectEventsPermission(
        /* [in] */ Int32 injectorPid,
        /* [in] */ Int32 injectorUid);

    // Native callback.
    CARAPI_(Int32) GetVirtualKeyQuietTimeMillis();

    // Native callback.
    CARAPI_(AutoPtr< ArrayOf<String> >) GetExcludedDeviceNames();

    // Native callback.
    CARAPI_(Int32) GetKeyRepeatTimeout();

    // Native callback.
    CARAPI_(Int32) GetKeyRepeatDelay();

    // Native callback.
    CARAPI_(Int32) GetHoverTapTimeout();

    // Native callback.
    CARAPI_(Int32) GetHoverTapSlop();

    // Native callback.
    CARAPI_(Int32) GetDoubleTapTimeout();

    // Native callback.
    CARAPI_(Int32) GetLongPressTimeout();

    // Native callback.
    CARAPI_(Int32) GetPointerLayer();

    // Native callback.
    CARAPI_(AutoPtr<IPointerIcon>) GetPointerIcon();

    // Native callback.
    CARAPI_(AutoPtr< ArrayOf<String> >) GetKeyboardLayoutOverlay(
        /* [in] */ IInputDeviceIdentifier* identifier);

    // Native callback.
    CARAPI_(String) GetDeviceAlias(
        /* [in] */ const String& uniqueId);

    CARAPI NativeInit(
        /* [in] */ IContext* context,
        /* [in] */ IMessageQueue* messageQueue);

    CARAPI NativeStart();

    CARAPI_(void) NativeSetDisplayViewport(
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
        /* [in] */ Int32 deviceHeight);

    CARAPI_(Int32) NativeGetScanCodeState(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 sourceMask,
        /* [in] */ Int32 scanCode);

    CARAPI_(Int32) NativeGetKeyCodeState(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 sourceMask,
        /* [in] */ Int32 keyCode);

    CARAPI_(Int32) NativeGetSwitchState(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 sourceMask,
        /* [in] */ Int32 sw);

    CARAPI_(Boolean) NativeHasKeys(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 sourceMask,
        /* [in] */ const ArrayOf<Int32>& keyCodes,
        /* [in] */ ArrayOf<Boolean>* keyExists);

    CARAPI NativeRegisterInputChannel(
        /* [in] */ IInputChannel* inputChannelObj,
        /* [in] */ IInputWindowHandle* inputWindowHandle,
        /* [in] */ Boolean monitor);

    CARAPI NativeUnregisterInputChannel(
        /* [in] */ IInputChannel* inputChannelObj);

    CARAPI_(void) NativeSetInputFilterEnabled(
        /* [in] */ Boolean enable);

    CARAPI_(Int32) NativeInjectInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 injectorPid,
        /* [in] */ Int32 injectorUid,
        /* [in] */ Int32 syncMode,
        /* [in] */ Int32 timeoutMillis,
        /* [in] */ Int32 policyFlags);

    CARAPI_(void) NativeSetInputWindows(
        /* [in] */ ArrayOf<IInputWindowHandle*>* windowHandles);

    CARAPI_(void) NativeSetInputDispatchMode(
        /* [in] */ Boolean enabled,
        /* [in] */ Boolean frozen);

    CARAPI_(void) NativeSetSystemUiVisibility(
        /* [in] */ Int32 visibility);

    CARAPI_(void) NativeSetFocusedApplication(
        /* [in] */ InputApplicationHandle* application);

    CARAPI_(Boolean) NativeTransferTouchFocus(
        /* [in] */ IInputChannel* fromChannel,
        /* [in] */ IInputChannel* toChannel);

    CARAPI_(void) NativeSetPointerSpeed(
        /* [in] */ Int32 speed);

    CARAPI_(void) NativeSetShowTouches(
        /* [in] */ Boolean enabled);

    CARAPI_(void) NativeSetInteractive(
        /* [in] */ Boolean interactive);

    CARAPI_(void) NativeReloadCalibration();

    CARAPI_(void) NativeVibrate(
        /* [in] */ Int32 deviceId,
        /* [in] */ const ArrayOf<Int64>& pattern,
        /* [in] */ Int32 repeat,
        /* [in] */ Int32 token);

    CARAPI_(void) NativeCancelVibrate(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 token);

    CARAPI_(void) NativeReloadKeyboardLayouts();

    CARAPI_(void) NativeReloadDeviceAliases();

    CARAPI_(String) NativeDump();

    CARAPI_(void) NativeMonitor();

    CARAPI_(Int32) FindInputFilterIndexLocked(
        /* [in] */ IIInputFilter* filter);

public:
    const static String TAG;
    const static Boolean DEBUG = FALSE;

    // Key states (may be returned by queries about the current state of a
    // particular key code, scan code or switch).

    /** The key state is unknown or the requested key itself is not supported. */
    const static Int32 KEY_STATE_UNKNOWN = -1;

    /** The key is up. /*/
    const static Int32 KEY_STATE_UP = 0;

    /** The key is down. */
    const static Int32 KEY_STATE_DOWN = 1;

    /** The key is down but is a virtual key press that is being emulated by the system. */
    const static Int32 KEY_STATE_VIRTUAL = 2;

    /** Scan code: Mouse / trackball button. */
    const static Int32 BTN_MOUSE = 0x110;

    // Switch code values must match bionic/libc/kernel/common/linux/input.h
    /** Switch code: Lid switch.  When set, lid is shut. */
    const static Int32 SW_LID = 0x00;

    /** Switch code: Keypad slide.  When set, keyboard is exposed. */
    const static Int32 SW_KEYPAD_SLIDE = 0x0a;

    /** Switch code: Headphone.  When set, headphone is inserted. */
    const static Int32 SW_HEADPHONE_INSERT = 0x02;

    /** Switch code: Microphone.  When set, microphone is inserted. */
    const static Int32 SW_MICROPHONE_INSERT = 0x04;

    /** Switch code: Line out.  When set, Line out (hi-Z) is inserted. */
    const static Int32 SW_LINEOUT_INSERT = 0x06;

    /** Switch code: Headphone/Microphone Jack.  When set, something is inserted. */
    const static Int32 SW_JACK_PHYSICAL_INSERT = 0x07;

    /** Switch code: Camera lens cover. When set the lens is covered. */
    const static Int32 SW_CAMERA_LENS_COVER = 0x09;

    const static Int32 SW_LID_BIT = 1 << SW_LID;
    const static Int32 SW_KEYPAD_SLIDE_BIT = 1 << SW_KEYPAD_SLIDE;
    const static Int32 SW_HEADPHONE_INSERT_BIT = 1 << SW_HEADPHONE_INSERT;
    const static Int32 SW_MICROPHONE_INSERT_BIT = 1 << SW_MICROPHONE_INSERT;
    const static Int32 SW_LINEOUT_INSERT_BIT = 1 << SW_LINEOUT_INSERT;
    const static Int32 SW_JACK_PHYSICAL_INSERT_BIT = 1 << SW_JACK_PHYSICAL_INSERT;
    const static Int32 SW_JACK_BITS =
            SW_HEADPHONE_INSERT_BIT | SW_MICROPHONE_INSERT_BIT | SW_JACK_PHYSICAL_INSERT_BIT | SW_LINEOUT_INSERT_BIT;
    const static Int32 SW_CAMERA_LENS_COVER_BIT = 1 << SW_CAMERA_LENS_COVER;

private:
    const static String EXCLUDED_DEVICES_PATH;

    const static Int32 MSG_DELIVER_INPUT_DEVICES_CHANGED = 1;
    const static Int32 MSG_SWITCH_KEYBOARD_LAYOUT = 2;
    const static Int32 MSG_RELOAD_KEYBOARD_LAYOUTS = 3;
    const static Int32 MSG_UPDATE_KEYBOARD_LAYOUTS = 4;
    const static Int32 MSG_RELOAD_DEVICE_ALIASES = 5;

    // Pointer to native input manager service object.
    NativeInputManager* mPtr;

    AutoPtr<IContext> mContext;
    AutoPtr<InputManagerHandler> mHandler;

    AutoPtr<IWindowManagerCallbacks> mWindowManagerCallbacks;
    AutoPtr<IWiredAccessoryCallbacks> mWiredAccessoryCallbacks;
    Boolean mSystemReady;
    AutoPtr<INotificationManager> mNotificationManager;

    // Persistent data store.  Must be locked each time during use.
    AutoPtr<PersistentDataStore> mDataStore;

    // List of currently registered input devices changed listeners by process id.
    Object mInputDevicesLock;
    Boolean mInputDevicesChangedPending;        // guarded by mInputDevicesLock
    AutoPtr< ArrayOf<IInputDevice*> > mInputDevices;
    AutoPtr<ISparseArray> mInputDevicesChangedListeners; // guarded by mInputDevicesLock
    AutoPtr<IArrayList> mTempInputDevicesChangedListenersToNotify; // handler thread only
    AutoPtr<IArrayList> mTempFullKeyboards;    // handler thread only
    Boolean mKeyboardLayoutNotificationShown;
    AutoPtr<IPendingIntent> mKeyboardLayoutIntent;
    AutoPtr<IToast> mSwitchedKeyboardLayoutToast;

    // State for vibrator tokens.
    Object mVibratorLock;
    HashMap< AutoPtr<IBinder>, AutoPtr<VibratorToken> > mVibratorTokens;
    Int32 mNextVibratorTokenValue;

    // State for the currently installed input filter.
    Object mInputFilterLock;
    AutoPtr<IIInputFilterHost> mInputFilterHost;         // guarded by mInputFilterLock
    AutoPtr<IArrayList> mInputFilterChain;  // guarded by mInputFilterLock

    // Input event injection constants defined in InputDispatcher.h.
    const static Int32 INPUT_EVENT_INJECTION_SUCCEEDED = 0;
    const static Int32 INPUT_EVENT_INJECTION_PERMISSION_DENIED = 1;
    const static Int32 INPUT_EVENT_INJECTION_FAILED = 2;
    const static Int32 INPUT_EVENT_INJECTION_TIMED_OUT = 3;

    // Maximum number of milliseconds to wait for input event injection.
    const static Int32 INJECTION_TIMEOUT_MILLIS = 30 * 1000;

    /** Whether to use the dev/input/event or uevent subsystem for the audio jack. */
    Boolean mUseDevInputEventForAudioJack;
};

} // Input
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SERVICE_INPUT_INPUT_INPUTMANAGERSERVICE_H__
