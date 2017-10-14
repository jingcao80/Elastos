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

#ifndef __ELASTOS_DROID_HARDWARE_INPUT_CINPUTMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_INPUT_CINPUTMANAGER_H__

#include "_Elastos_Droid_Hardware_Input_CInputManager.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Vibrator.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::Vibrator;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

/**
 * Provides information about input devices and available key layouts.
 * <p>
 * Get an instance of this class by calling
 * {@link android.content.Context#getSystemService(java.lang.String)
 * Context.getSystemService()} with the argument
 * {@link android.content.Context#INPUT_SERVICE}.
 * </p>
 */
CarClass(CInputManager)
    , public Object
    , public IInputManager
{
    friend class CInputManagerInputDevicesChangedListener;
private:
    class InputDeviceListenerDelegate
        : public Handler
    {
    public:
        TO_STRING_IMPL("CInputManager::InputDeviceListenerDelegate")

        InputDeviceListenerDelegate(
            /* [in] */ IInputDeviceListener* listener);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        AutoPtr<IInputDeviceListener> mListener;
    };

    class InputDeviceVibrator
        : public Vibrator
    {
    public:
        CAR_INTERFACE_DECL()

        InputDeviceVibrator(
            /* [in] */ Int32 deviceId,
            /* [in] */ CInputManager* owner);

        CARAPI HasVibrator(
            /* [out] */ Boolean* res);

        using Vibrator::Vibrate;

        CARAPI Vibrate(
            /* [in] */ Int32 uid,
            /* [in] */ const String& opPkg,
            /* [in] */ Int64 milliseconds,
            /* [in] */ IAudioAttributes* attributes);

        CARAPI Vibrate(
            /* [in] */ Int32 uid,
            /* [in] */ const String& opPkg,
            /* [in] */ ArrayOf<Int64>* pattern,
            /* [in] */ Int32 repeat,
            /* [in] */ IAudioAttributes* attributes);

        CARAPI Cancel();

    private:
        Int32 mDeviceId;
        AutoPtr<IBinder> mToken;
        CInputManager* mOwner;
    };

public:
    /**
     * Gets an instance of the input manager.
     *
     * @return The input manager instance.
     *
     * @hide
     */
    static AutoPtr<IInputManager> GetInstance();

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CInputManager();

    ~CInputManager();

    CARAPI constructor();

    /**
     * Gets information about the input device with the specified id.
     * @param id The device id.
     * @return The input device or NULL if not found.
     */
    CARAPI GetInputDevice(
        /* [in] */ Int32 id,
        /* [out] */ IInputDevice** device);

    /**
     * Gets information about the input device with the specified descriptor.
     * @param descriptor The input device descriptor.
     * @return The input device or NULL if not found.
     * @hide
     */
    CARAPI GetInputDeviceByDescriptor(
        /* [in] */ const String& descriptor,
        /* [out] */ IInputDevice** device);

    /**
     * Gets the ids of all input devices in the system.
     * @return The input device ids.
     */
    CARAPI GetInputDeviceIds(
        /* [out, callee] */ ArrayOf<Int32>** deviceIds);

    /**
     * Registers an input device listener to receive notifications about when
     * input devices are added, removed or changed.
     *
     * @param listener The listener to register.
     * @param handler The handler on which the listener should be invoked, or NULL
     * if the listener should be invoked on the calling thread's looper.
     *
     * @see #unregisterInputDeviceListener
     */
    CARAPI RegisterInputDeviceListener(
        /* [in] */ IInputDeviceListener* listener,
        /* [in] */ IHandler* handler);

    /**
     * Unregisters an input device listener.
     *
     * @param listener The listener to unregister.
     *
     * @see #registerInputDeviceListener
     */
    CARAPI UnregisterInputDeviceListener(
        /* [in] */ IInputDeviceListener* listener);

    /**
     * Gets information about all supported keyboard layouts.
     * <p>
     * The input manager consults the built-in keyboard layouts as well
     * as all keyboard layouts advertised by applications using a
     * {@link #ACTION_QUERY_KEYBOARD_LAYOUTS} broadcast receiver.
     * </p>
     *
     * @return A list of all supported keyboard layouts.
     *
     * @hide
     */
    CARAPI GetKeyboardLayouts(
        /* [out, callee] */ ArrayOf<IKeyboardLayout*>** layouts);

    /**
     * Gets the keyboard layout with the specified descriptor.
     *
     * @param keyboardLayoutDescriptor The keyboard layout descriptor, as returned by
     * {@link KeyboardLayout#getDescriptor()}.
     * @return The keyboard layout, or NULL if it could not be loaded.
     *
     * @hide
     */
    CARAPI GetKeyboardLayout(
        /* [in] */ const String& keyboardLayoutDescriptor,
        /* [out] */ IKeyboardLayout** layout);

    /**
     * Gets the current keyboard layout descriptor for the specified input
     * device.
     *
     * @param identifier Identifier for the input device
     * @return The keyboard layout descriptor, or null if no keyboard layout has
     *         been set.
     * @hide
     */
    CARAPI GetCurrentKeyboardLayoutForInputDevice(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [out] */ String* keyboardLayoutDescriptor);

    /**
     * Sets the current keyboard layout descriptor for the specified input
     * device.
     * <p>
     * This method may have the side-effect of causing the input device in
     * question to be reconfigured.
     * </p>
     *
     * @param identifier The identifier for the input device.
     * @param keyboardLayoutDescriptor The keyboard layout descriptor to use,
     *            must not be null.
     * @hide
     */
    CARAPI SetCurrentKeyboardLayoutForInputDevice(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [in] */ const String& keyboardLayoutDescriptor);

    /**
     * Gets all keyboard layout descriptors that are enabled for the specified
     * input device.
     *
     * @param identifier The identifier for the input device.
     * @return The keyboard layout descriptors.
     * @hide
     */
    CARAPI GetKeyboardLayoutsForInputDevice(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [out, callee] */ ArrayOf<String>** keyboardLayoutDescriptors);

    /**
     * Adds the keyboard layout descriptor for the specified input device.
     * <p>
     * This method may have the side-effect of causing the input device in
     * question to be reconfigured.
     * </p>
     *
     * @param identifier The identifier for the input device.
     * @param keyboardLayoutDescriptor The descriptor of the keyboard layout to
     *            add.
     * @hide
     */
    CARAPI AddKeyboardLayoutForInputDevice(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [in] */ const String& keyboardLayoutDescriptor);

    /**
     * Removes the keyboard layout descriptor for the specified input device.
     * <p>
     * This method may have the side-effect of causing the input device in
     * question to be reconfigured.
     * </p>
     *
     * @param identifier The identifier for the input device.
     * @param keyboardLayoutDescriptor The descriptor of the keyboard layout to
     *            remove.
     * @hide
     */
    CARAPI RemoveKeyboardLayoutForInputDevice(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [in] */ const String& keyboardLayoutDescriptor);

    /**
     * Gets the TouchCalibration applied to the specified input device's coordinates.
     *
     * @param inputDeviceDescriptor The input device descriptor.
     * @return The TouchCalibration currently assigned for use with the given
     * input device. If none is set, an identity TouchCalibration is returned.
     *
     * @hide
     */
    CARAPI GetTouchCalibration(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ Int32 surfaceRotation,
        /* [out] */ ITouchCalibration** result);

    /**
     * Sets the TouchCalibration to apply to the specified input device's coordinates.
     * <p>
     * This method may have the side-effect of causing the input device in question
     * to be reconfigured. Requires {@link android.Manifest.permissions.SET_INPUT_CALIBRATION}.
     * </p>
     *
     * @param inputDeviceDescriptor The input device descriptor.
     * @param calibration The calibration to be applied
     *
     * @hide
     */
    CARAPI SetTouchCalibration(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ Int32 surfaceRotation,
        /* [in] */ ITouchCalibration* calibration);

    /**
     * Gets the mouse pointer speed.
     * <p>
     * Only returns the permanent mouse pointer speed.  Ignores any temporary pointer
     * speed set by {@link #tryPointerSpeed}.
     * </p>
     *
     * @param context The application context.
     * @return The pointer speed as a value between {@link #MIN_POINTER_SPEED} and
     * {@link #MAX_POINTER_SPEED}, or the default value {@link #DEFAULT_POINTER_SPEED}.
     *
     * @hide
     */
    CARAPI GetPointerSpeed(
        /* [in] */ IContext* context,
        /* [out] */ Int32* speed);

    /**
     * Sets the mouse pointer speed.
     * <p>
     * Requires {@link android.Manifest.permissions.WRITE_SETTINGS}.
     * </p>
     *
     * @param context The application context.
     * @param speed The pointer speed as a value between {@link #MIN_POINTER_SPEED} and
     * {@link #MAX_POINTER_SPEED}, or the default value {@link #DEFAULT_POINTER_SPEED}.
     *
     * @hide
     */
    CARAPI SetPointerSpeed(
        /* [in] */ IContext* context,
        /* [in] */ Int32 speed);

    /**
     * Changes the mouse pointer speed temporarily, but does not save the setting.
     * <p>
     * Requires {@link android.Manifest.permission.SET_POINTER_SPEED}.
     * </p>
     *
     * @param speed The pointer speed as a value between {@link #MIN_POINTER_SPEED} and
     * {@link #MAX_POINTER_SPEED}, or the default value {@link #DEFAULT_POINTER_SPEED}.
     *
     * @hide
     */
    CARAPI TryPointerSpeed(
        /* [in] */ Int32 speed);

    /**
     * Queries the framework about whether any physical keys exist on the
     * any keyboard attached to the device that are capable of producing the given
     * array of key codes.
     *
     * @param keyCodes The array of key codes to query.
     * @return A new array of the same size as the key codes array whose elements
     * are set to true if at least one attached keyboard supports the corresponding key code
     * at the same index in the key codes array.
     *
     * @hide
     */
    CARAPI DeviceHasKeys(
        /* [in] */ const ArrayOf<Int32>& keyCodes,
        /* [out, callee] */ ArrayOf<Boolean>** hasKeys);

    /**
     * Queries the framework about whether any physical keys exist on the
     * any keyboard attached to the device that are capable of producing the given
     * array of key codes.
     *
     * @param id The id of the device to query.
     * @param keyCodes The array of key codes to query.
     * @return A new array of the same size as the key codes array whose elements are set to true
     * if the given device could produce the corresponding key code at the same index in the key
     * codes array.
     *
     * @hide
     */
    CARAPI DeviceHasKeys(
        /* [in] */ Int32 id,
        /* [in] */ const ArrayOf<Int32>& keyCodes,
        /* [out, callee] */ ArrayOf<Boolean>** result);

    /**
     * Injects an input event into the event system on behalf of an application.
     * The synchronization mode determines whether the method blocks while waiting for
     * input injection to proceed.
     * <p>
     * Requires {@link android.Manifest.permission.INJECT_EVENTS} to inject into
     * windows that are owned by other applications.
     * </p><p>
     * Make sure you correctly set the event time and input source of the event
     * before calling this method.
     * </p>
     *
     * @param event The event to inject.
     * @param mode The synchronization mode.  One of:
     * {@link #INJECT_INPUT_EVENT_MODE_ASYNC},
     * {@link #INJECT_INPUT_EVENT_MODE_WAIT_FOR_RESULT}, or
     * {@link #INJECT_INPUT_EVENT_MODE_WAIT_FOR_FINISH}.
     * @return True if input event injection succeeded.
     *
     * @hide
     */
    CARAPI InjectInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 mode,
        /* [out] */ Boolean* res);

    /**
     * Gets a vibrator service associated with an input device, assuming it has one.
     * @return The vibrator, never NULL.
     * @hide
     */
    CARAPI GetInputDeviceVibrator(
        /* [in] */ Int32 deviceId,
        /* [out] */ IVibrator** vibrator);

private:

    static Boolean ContainsDeviceId(
        /* [in] */ ArrayOf<Int32>* deviceIdAndGeneration,
        /* [in] */ Int32 deviceId);

    CARAPI_(List<AutoPtr<InputDeviceListenerDelegate> >::Iterator) FindInputDeviceListenerLocked(
        /* [in] */ IInputDeviceListener* listener);

    CARAPI PopulateInputDevicesLocked();

    CARAPI_(void) OnInputDevicesChanged(
        /* [in] */ ArrayOf<Int32>* deviceIdAndGeneration);

    CARAPI_(void) SendMessageToInputDeviceListenersLocked(
        /* [in] */ Int32 what,
        /* [in] */ Int32 deviceId);

private:
    static const char* TAG;
    static const Boolean DEBUG = FALSE;

    static const Int32 MSG_DEVICE_ADDED = 1;
    static const Int32 MSG_DEVICE_REMOVED = 2;
    static const Int32 MSG_DEVICE_CHANGED = 3;

    static AutoPtr<IInputManager> sInstance;
    static Object sInstanceLock;

    AutoPtr<IIInputManager> mIm;

    // Guarded by mInputDevicesLock
    Object mInputDevicesLock;
    AutoPtr< HashMap<Int32, AutoPtr<IInputDevice> > > mInputDevices;
    AutoPtr<IInputDevicesChangedListener> mInputDevicesChangedListener;
    List<AutoPtr<InputDeviceListenerDelegate> > mInputDeviceListeners;
};

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_INPUT_CINPUTMANAGER_H__
