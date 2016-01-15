
#ifndef __ELASTOS_DROID_SERVER_INPUT_CINPUTMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_INPUT_CINPUTMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_Input_CInputManagerService.h"
#include "NativeInputManager.h"
#include "input/PersistentDataStore.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "display/DisplayViewport.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/HandlerBase.h"
#include <elastos/utility/etl/HashSet.h>

using Elastos::Utility::Etl::HashSet;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IIInputFilter;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IInputFilterHost;
using Elastos::Droid::View::IPointerIcon;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Hardware::Input::IKeyboardLayout;
using Elastos::Droid::Hardware::Input::IInputDevicesChangedListener;
using Elastos::Droid::Server::Display::IDisplayViewport;
using Elastos::Droid::Server::Display::DisplayViewport;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

CarClass(CInputManagerService)
{
private:
    friend class NativeInputManager;

    class InputManagerHandler : public HandlerBase
    {
    public:
        InputManagerHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CInputManagerService* host)
            : HandlerBase(looper, NULL, TRUE)
            , mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CInputManagerService* mHost;
    };

public:
    // Key states (may be returned by queries about the current state of a
    // particular key code, scan code or switch).

    /** The key state is unknown or the requested key itself is not supported. */
    static const Int32 KEY_STATE_UNKNOWN = -1;

    /** The key is up. /*/
    static const Int32 KEY_STATE_UP = 0;

    /** The key is down. */
    static const Int32 KEY_STATE_DOWN = 1;

    /** The key is down but is a virtual key press that is being emulated by the system. */
    static const Int32 KEY_STATE_VIRTUAL = 2;

    // modify name for conflicting with macro of android
    //
    /** Scan code: Mouse / trackball button. */
    static const Int32 _BTN_MOUSE = 0x110;

    // Switch code values must match bionic/libc/kernel/common/linux/input.h
    /** Switch code: Lid switch.  When set, lid is shut. */
    static const Int32 _SW_LID = 0x00;

    /** Switch code: Keypad slide.  When set, keyboard is exposed. */
    static const Int32 _SW_KEYPAD_SLIDE = 0x0a;

    /** Switch code: Headphone.  When set, headphone is inserted. */
    static const Int32 _SW_HEADPHONE_INSERT = 0x02;

    /** Switch code: Microphone.  When set, microphone is inserted. */
    static const Int32 _SW_MICROPHONE_INSERT = 0x04;

    /** Switch code: Headphone/Microphone Jack.  When set, something is inserted. */
    static const Int32 _SW_JACK_PHYSICAL_INSERT = 0x07;

    static const Int32 SW_LID_BIT = 1 << _SW_LID;
    static const Int32 SW_KEYPAD_SLIDE_BIT = 1 << _SW_KEYPAD_SLIDE;
    static const Int32 SW_HEADPHONE_INSERT_BIT = 1 << _SW_HEADPHONE_INSERT;
    static const Int32 SW_MICROPHONE_INSERT_BIT = 1 << _SW_MICROPHONE_INSERT;
    static const Int32 SW_JACK_PHYSICAL_INSERT_BIT = 1 << _SW_JACK_PHYSICAL_INSERT;
    static const Int32 SW_JACK_BITS =
            SW_HEADPHONE_INSERT_BIT | SW_MICROPHONE_INSERT_BIT | SW_JACK_PHYSICAL_INSERT_BIT;

protected:
    static const char* TAG;
    static const Boolean DEBUG = FALSE;

private:
    static const String EXCLUDED_DEVICES_PATH;

    static const Int32 MSG_DELIVER_INPUT_DEVICES_CHANGED = 1;
    static const Int32 MSG_SWITCH_KEYBOARD_LAYOUT = 2;
    static const Int32 MSG_RELOAD_KEYBOARD_LAYOUTS = 3;
    static const Int32 MSG_UPDATE_KEYBOARD_LAYOUTS = 4;
    static const Int32 MSG_RELOAD_DEVICE_ALIASES = 5;

    // Input event injection constants defined in InputDispatcher.h.
    static const Int32 INPUT_EVENT_INJECTION_SUCCEEDED = 0;
    static const Int32 INPUT_EVENT_INJECTION_PERMISSION_DENIED = 1;
    static const Int32 INPUT_EVENT_INJECTION_FAILED = 2;
    static const Int32 INPUT_EVENT_INJECTION_TIMED_OUT = 3;

    // Maximum number of milliseconds to wait for input event injection.
    static const Int32 INJECTION_TIMEOUT_MILLIS = 30 * 1000;

public:
    /**
     * Callback interface implemented by the Window Manager.
     */
    interface WindowManagerCallbacks
    {
    public:
        virtual CARAPI_(void) NotifyConfigurationChanged() = 0;

        virtual CARAPI_(void) NotifyLidSwitchChanged(
            /* [in] */ Int64 whenNanos,
            /* [in] */ Boolean lidOpen) = 0;

        virtual CARAPI_(void) NotifyInputChannelBroken(
            /* [in] */ InputWindowHandle* inputWindowHandle) = 0;

        virtual CARAPI_(Int64) NotifyANR(
            /* [in] */ InputApplicationHandle* inputApplicationHandle,
            /* [in] */ InputWindowHandle* inputWindowHandle) = 0;

        virtual CARAPI_(Int32) InterceptKeyBeforeQueueing(
            /* [in] */ IKeyEvent* event,
            /* [in] */ Int32 policyFlags,
            /* [in] */ Boolean isScreenOn) = 0;

        virtual CARAPI_(Int32) InterceptMotionBeforeQueueingWhenScreenOff(
            /* [in] */ Int32 policyFlags) = 0;

        virtual CARAPI_(Int64) InterceptKeyBeforeDispatching(
            /* [in] */ InputWindowHandle* focus,
            /* [in] */ IKeyEvent* event,
            /* [in] */ Int32 policyFlags) = 0;

        virtual CARAPI_(AutoPtr<IKeyEvent>) DispatchUnhandledKey(
            /* [in] */ InputWindowHandle* focus,
            /* [in] */ IKeyEvent* event,
            /* [in] */ Int32 policyFlags) = 0;

        virtual CARAPI_(Int32) GetPointerLayer() = 0;
    };

    /**
     * Callback interface implemented by WiredAccessoryObserver.
     */
    interface WiredAccessoryCallbacks
    {
    public:
        virtual CARAPI_(void) NotifyWiredAccessoryChanged(
            /* [in] */ Int64 whenNanos,
            /* [in] */ Int32 switchValues,
            /* [in] */ Int32 switchMask) = 0;
    };

private:
    class MyBroadcastReceiver : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ CInputManagerService* owner);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CInputManagerService::MyBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CInputManagerService* mOwner;
    };

    class MyBroadcastReceiverEx : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ CInputManagerService* owner);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CInputManagerService::MyBroadcastReceiverEx: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CInputManagerService* mOwner;
    };

    class MyBroadcastReceiverEx2 : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ CInputManagerService* owner);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CInputManagerService::MyBroadcastReceiverEx2: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CInputManagerService* mOwner;
    };

    interface KeyboardLayoutVisitor : public ElRefBase
    {
    public:
        virtual CARAPI_(void) VisitKeyboardLayout(
            /* [in] */ IResources* resources,
            /* [in] */ const String& descriptor,
            /* [in] */ const String& label,
            /* [in] */ const String& collection,
            /* [in] */ Int32 keyboardLayoutResId) = 0;
    };

    class MyKeyboardLayoutVisitor : public KeyboardLayoutVisitor
    {
    public:
        MyKeyboardLayoutVisitor(
            /* [in] */ HashSet<String>& availableKeyboardLayouts);

        CARAPI_(void) VisitKeyboardLayout(
            /* [in] */ IResources* resources,
            /* [in] */ const String& descriptor,
            /* [in] */ const String& label,
            /* [in] */ const String& collection,
            /* [in] */ Int32 keyboardLayoutResId);

    private:
        HashSet<String>& mAvailableKeyboardLayouts;
    };

    class MyKeyboardLayoutVisitorEx : public KeyboardLayoutVisitor
    {
    public:
        MyKeyboardLayoutVisitor(
            /* [in] */ List<AutoPtr<IKeyboardLayout> >& list);

        CARAPI_(void) VisitKeyboardLayout(
            /* [in] */ IResources* resources,
            /* [in] */ const String& descriptor,
            /* [in] */ const String& label,
            /* [in] */ const String& collection,
            /* [in] */ Int32 keyboardLayoutResId);

    private:
        List<AutoPtr<IKeyboardLayout> >& mList;
    };

    class MyKeyboardLayoutVisitorEx2 : public KeyboardLayoutVisitor
    {
    public:
        MyKeyboardLayoutVisitor(
            /* [in] */ IKeyboardLayout** layout);

        CARAPI_(void) VisitKeyboardLayout(
            /* [in] */ IResources* resources,
            /* [in] */ const String& descriptor,
            /* [in] */ const String& label,
            /* [in] */ const String& collection,
            /* [in] */ Int32 keyboardLayoutResId);

    private:
        IKeyboardLayout** mLayout;
    };

    class MyContentObserver : public ContentObserver
    {
    public:
        MyContentObserver(
            /* [in] */ CInputManagerService* owner,
            /* [in] */ IHandler* handler);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        CInputManagerService* mOwner;
    };

    class MyContentObserverEx : public ContentObserver
    {
    public:
        MyContentObserver(
            /* [in] */ CInputManagerService* owner,
            /* [in] */ IHandler* handler);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        CInputManagerService* mOwner;
    };

    class MyKeyboardLayoutVisitorEx3 : public KeyboardLayoutVisitor
    {
    public:
        MyKeyboardLayoutVisitor(
            /* [in] */ ArrayOf<String>* layouts);

        CARAPI_(void) VisitKeyboardLayout(
            /* [in] */ IResources* resources,
            /* [in] */ const String& descriptor,
            /* [in] */ const String& label,
            /* [in] */ const String& collection,
            /* [in] */ Int32 keyboardLayoutResId);

    private:
        AutoPtr<ArrayOf<String> > mLayouts;
    };


    /**
     * Hosting interface for input filters to call back into the input manager.
     */
    class InputFilterHost : public ElRefBase, public IInputFilterHost
    {
    public:
        CAR_INTERFACE_DECL();

        InputFilterHost(
            /* [in] */ CInputManagerService* owner);

        CARAPI_(void) DisconnectLocked();

        //@Override
        CARAPI SendInputEvent(
            /* [in] */ IInputEvent* event,
            /* [in] */ Int32 policyFlags);

    private:
        CInputManagerService* mOwner;
        Boolean mDisconnected;
    };

    class KeyboardLayoutDescriptor : public ElRefBase
    {
    public:
        static String Format(
            /* [in] */ const String& packageName,
            /* [in] */ const String& receiverName,
            /* [in] */ const String& keyboardName);

        static AutoPtr<KeyboardLayoutDescriptor> Parse(
            /* [in] */ const String& descriptor);

    public:
        String mPackageName;
        String mReceiverName;
        String mKeyboardLayoutName;
    };

    class InputDevicesChangedListenerRecord :
        public ElRefBase,
        public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL();

        InputDevicesChangedListenerRecord(
            /* [in] */ CInputManagerService* owner,
            /* [in] */ Int32 pid,
            /* [in] */ IInputDevicesChangedListener* listener);

        //@Override
        CARAPI ProxyDied();

        CARAPI_(void) NotifyInputDevicesChanged(
            /* [in] */ ArrayOf<Int32>* info);

    private:
        CInputManagerService* mOwner;
        const Int32 mPid;
        AutoPtr<IInputDevicesChangedListener> mListener;
    };

    class VibratorToken :
        public ElRefBase,
        public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL();

        VibratorToken(
            /* [in] */ CInputManagerService* owner,
            /* [in] */ Int32 deviceId,
            /* [in] */ IBinder* token,
            /* [in] */ Int32 tokenValue);

        //@Override
        CARAPI ProxyDied();

    public:
        CInputManagerService* mOwner;
        const Int32 mDeviceId;
        AutoPtr<IBinder> mToken;
        const Int32 mTokenValue;
        Boolean mVibrating;
        Object mLock;
    };

public:
    CInputManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler);

    CARAPI_(void) SetWindowManagerCallbacks(
        /* [in] */ WindowManagerCallbacks* callbacks);

    CARAPI_(void) SetWiredAccessoryCallbacks(
        /* [in] */ WiredAccessoryCallbacks* callbacks);

    CARAPI Start();

    // TODO(BT) Pass in paramter for bluetooth system
    CARAPI_(void) SystemReady();

    //@Override
    CARAPI SetDisplayViewports(
        /* [in] */ IDisplayViewport* defaultViewport,
        /* [in] */ IDisplayViewport* externalTouchViewport);

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
     * to TRUE or FALSE based on the presence or absence of support for the corresponding
     * key codes.
     * @return True if the lookup was successful, FALSE otherwise.
     */
    //@Override // Binder call
    CARAPI HasKeys(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 sourceMask,
        /* [in] */ const ArrayOf<Int32>& keyCodes,
        /* [out] */ ArrayOf<Boolean>* keyExists,
        /* [out] */ Boolean* res);

    /**
     * Creates an input channel that will receive all input from the input dispatcher.
     * @param inputChannelName The input channel name.
     * @return The input channel.
     */
    CARAPI MonitorInput(
        /* [in] */ const String& inputChannelName,
        /* [out] */ IInputChannel** inputChannel);

    /**
     * Registers an input channel so that it can be used as an input event target.
     * @param inputChannel The input channel to register.
     * @param inputWindowHandle The handle of the input window associated with the
     * input channel, or NULL if none.
     */
    CARAPI RegisterInputChannel(
        /* [in] */ IInputChannel* inputChannel,
        /* [in] */ InputWindowHandle* inputWindowHandle);

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
     * filter is uninstalled, it can no Int64er send input events unless it is reinstalled.
     * Any events it attempts to send after it has been uninstalled will be dropped.
     *
     * @param filter The input filter, or NULL to remove the current filter.
     */
    CARAPI_(void) SetInputFilter(
        /* [in] */ IIInputFilter* filter);

    //@Override // Binder call
    CARAPI InjectInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 mode,
        /* [out] */ Boolean* res);

    /**
     * Gets information about the input device with the specified id.
     * @param deviceId The device id.
     * @return The input device or NULL if not found.
     */
    //@Override // Binder call
    CARAPI GetInputDevice(
        /* [in] */ Int32 deviceId,
        /* [out] */ IInputDevice** inputDevice);

    /**
     * Gets the ids of all input devices in the system.
     * @return The input device ids.
     */
    //@Override // Binder call
    CARAPI GetInputDeviceIds(
        /* [out, callee] */ ArrayOf<Int32>** deviceIds);

    /**
     * Gets all input devices in the system.
     * @return The array of input devices.
     */
    CARAPI_(AutoPtr<ArrayOf<IInputDevice*> >) GetInputDevices();

    //@Override // Binder call
    CARAPI RegisterInputDevicesChangedListener(
        /* [in] */ IInputDevicesChangedListener* listener);

    //@Override // Binder call
    CARAPI GetKeyboardLayouts(
        /* [out, callee] */ ArrayOf<IKeyboardLayout*>** layouts);

    //@Override // Binder call
    CARAPI GetKeyboardLayout(
        /* [in] */ const String& keyboardLayoutDescriptor,
        /* [out] */ IKeyboardLayout** layout);

    //@Override // Binder call
    CARAPI GetCurrentKeyboardLayoutForInputDevice(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [out] */ String* keyboardLayoutDescriptor);

    //@Override // Binder call
    CARAPI SetCurrentKeyboardLayoutForInputDevice(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ const String& keyboardLayoutDescriptor);

    //@Override // Binder call
    CARAPI GetKeyboardLayoutsForInputDevice(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [out] */ ArrayOf<String>** keyboardLayoutDescriptors);

    //@Override // Binder call
    CARAPI AddKeyboardLayoutForInputDevice(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ const String& keyboardLayoutDescriptor);

    //@Override // Binder call
    CARAPI RemoveKeyboardLayoutForInputDevice(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ const String& keyboardLayoutDescriptor);

    CARAPI_(void) SwitchKeyboardLayout(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 direction);

    CARAPI_(void) SetInputWindows(
        /* [in] */ ArrayOf<InputWindowHandle*>* windowHandles);

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
    CARAPI_(Boolean) TransferTouchFocus(
        /* [in] */ IInputChannel* fromChannel,
        /* [in] */ IInputChannel* toChannel);

    //@Override // Binder call
    CARAPI TryPointerSpeed(
        /* [in] */ Int32 speed);

    CARAPI_(void) UpdatePointerSpeedFromSettings();

    CARAPI_(void) UpdateShowTouchesFromSettings();

    CARAPI KeyEnterMouseMode();

    CARAPI KeyExitMouseMode();

    CARAPI KeySetMouseDistance(int distance);

    CARAPI KeySetMouseMoveCode(int left,int right,int top,int bottom);

    CARAPI KeySetMouseBtnCode(int leftbtn,int midbtn,int rightbtn);

    // Binder call
    //@Override
    CARAPI Vibrate(
        /* [in] */ Int32 deviceId,
        /* [in] */ const ArrayOf<Int64>& pattern,
        /* [in] */ Int32 repeat,
        /* [in] */ IBinder* token);

    // Binder call
    //@Override
    CARAPI CancelVibrate(
        /* [in] */ Int32 deviceId,
        /* [in] */ IBinder* token);

    CARAPI_(void) OnVibratorTokenDied(
        /* [in] */ VibratorToken* v);

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const ArrayOf<String>& args);

    // Called by the heartbeat to ensure locks are not held indefinitely (for deadlock detection).
    //@Override
    CARAPI Monitor();

    CARAPI ResetTouchCalibration();

private:
    CARAPI_(void) ReloadKeyboardLayouts();

    CARAPI_(void) ReloadDeviceAliases();

    CARAPI_(void) SetDisplayViewport(
        /* [in] */ Boolean external,
        /* [in] */ DisplayViewport* viewport);

    CARAPI_(void) OnInputDevicesChangedListenerDied(
        /* [in] */ Int32 pid);

    // Must be called on handler.
    CARAPI_(void) DeliverInputDevicesChanged(
        /* [in] */ ArrayOf<IInputDevice*>* oldInputDevices);

    // Must be called on handler.
    CARAPI_(void) ShowMissingKeyboardLayoutNotification();

    // Must be called on handler.
    CARAPI_(void) HideMissingKeyboardLayoutNotification();

    // Must be called on handler.
    CARAPI_(void) UpdateKeyboardLayouts();

    static Boolean ContainsInputDeviceWithDescriptor(
        /* [in] */ ArrayOf<IInputDevice*>* inputDevices,
        /* [in] */ const String& descriptor);

    CARAPI_(void) VisitAllKeyboardLayouts(
        /* [in] */ KeyboardLayoutVisitor* visitor);

    CARAPI_(void) VisitKeyboardLayout(
        /* [in] */ const String& keyboardLayoutDescriptor,
        /* [in] */ KeyboardLayoutVisitor* visitor);

    CARAPI_(void) VisitKeyboardLayoutsInPackage(
        /* [in] */ IPackageManager* pm,
        /* [in] */ IActivityInfo* receiver,
        /* [in] */ const String& keyboardName,
        /* [in] */ KeyboardLayoutVisitor* visitor);

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
        /* [in] */ InputWindowHandle* inputWindowHandle);

    // Native callback.
    CARAPI_(Boolean) FilterInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 policyFlags);

    // Native callback.
    CARAPI_(Int32) InterceptKeyBeforeQueueing(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags,
        /* [in] */ Boolean isScreenOn);

    // Native callback.
    CARAPI_(Int32) InterceptMotionBeforeQueueingWhenScreenOff(
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
    CARAPI_(AutoPtr<ArrayOf<String> >) GetExcludedDeviceNames();

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
    CARAPI_(AutoPtr<ArrayOf<String> >) GetKeyboardLayoutOverlay(
        /* [in] */ const String& inputDeviceDescriptor);

    // Native callback.
    CARAPI_(String) GetDeviceAlias(
        /* [in] */ const String& uniqueId);

    CARAPI_(void) NativeInit();

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
        /* [out] */ ArrayOf<Boolean>* keyExists);

    CARAPI NativeRegisterInputChannel(
        /* [in] */ IInputChannel* inputChannel,
        /* [in] */ InputWindowHandle* inputWindowHandle,
        /* [in] */ Boolean monitor);

    CARAPI NativeUnregisterInputChannel(
        /* [in] */ IInputChannel* inputChannel);

    CARAPI_(void) NativeSetInputFilterEnabled(
        /* [in] */ Boolean enable);

    CARAPI_(Int32) NativeInjectInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 injectorPid,
        /* [in] */ Int32 injectorUid,
        /* [in] */ Int32 syncMode,
        /* [in] */ Int32 timeoutMillis,
        /* [in] */ Int32 policyFlags);

    CARAPI_(void) NativeSetInputWindows(
        /* [in] */ ArrayOf<InputWindowHandle*>* windowHandles);

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

    CARAPI_(void) NativeKeyEnterMouseMode();

    CARAPI_(void) NativeKeyExitMouseMode();

    CARAPI_(void) NativeKeySetMouseDistance(
        /* [in] */ Int32 distance);

    CARAPI_(void) NativeKeySetMouseMoveCode(
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [in] */ Int32 top,
        /* [in] */ Int32 bottom);

    CARAPI_(void) NativeKeySetMouseBtnCode(
        /* [in] */ Int32 leftbtn,
        /* [in] */ Int32 midbtn,
        /* [in] */ Int32 rightbtn);

    CARAPI_(String) NativeDump();

    CARAPI_(void) NativeMonitor();

    CARAPI_(void) NativeResetTouchCalibration();

protected:
    // State for the currently installed input filter.
    Object mInputFilterLock;
    AutoPtr<IIInputFilter> mInputFilter; // guarded by mInputFilterLock
    AutoPtr<InputFilterHost> mInputFilterHost; // guarded by mInputFilterLock

    /** Whether to use the dev/input/event or uevent subsystem for the audio jack. */
    Boolean mUseDevInputEventForAudioJack;

private:
    // Pointer to native input manager service object.
    android::sp<NativeInputManager> mNativeInputManager;

    AutoPtr<IContext> mContext;
    AutoPtr<InputManagerHandler> mHandler;

    WindowManagerCallbacks* mWindowManagerCallbacks;
    WiredAccessoryCallbacks* mWiredAccessoryCallbacks;
    Boolean mSystemReady;
    AutoPtr<INotificationManager> mNotificationManager;

    // Persistent data store.  Must be locked each time during use.
    AutoPtr<PersistentDataStore> mDataStore;
    Object mDataStoreLock;

    // List of currently registered input devices changed listeners by process id.
    Object mInputDevicesLock;
    Boolean mInputDevicesChangedPending; // guarded by mInputDevicesLock
    AutoPtr<ArrayOf<IInputDevice*> > mInputDevices;
    HashMap<Int32, AutoPtr<InputDevicesChangedListenerRecord> > mInputDevicesChangedListeners; // guarded by mInputDevicesLock
    List<AutoPtr<InputDevicesChangedListenerRecord> > mTempInputDevicesChangedListenersToNotify; // handler thread only
    List<AutoPtr<IInputDevice> > mTempFullKeyboards; // handler thread only
    Boolean mKeyboardLayoutNotificationShown;
    AutoPtr<IPendingIntent> mKeyboardLayoutIntent;
    AutoPtr<IToast> mSwitchedKeyboardLayoutToast;

    // State for vibrator tokens.
    Object mVibratorLock;
    HashMap<AutoPtr<IBinder>, AutoPtr<VibratorToken> > mVibratorTokens;
    Int32 mNextVibratorTokenValue;
};

} // namespace Input
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_INPUT_CINPUTMANAGERSERVICE_H__
