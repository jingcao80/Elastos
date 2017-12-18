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

#ifndef __ELASTOS_DROID_VIEW_INPUTDEVICE_H__
#define __ELASTOS_DROID_VIEW_INPUTDEVICE_H__

#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Hardware::Input::IInputDeviceIdentifier;

namespace Elastos {
namespace Droid {
namespace Os {

class AndroidParcelUtils;

}
}
}

namespace Elastos {
namespace Droid {
namespace View {

class InputDevice
    : public Object
    , public IInputDevice
    , public IParcelable
{
public:
    /**
     * Provides information about the range of values for a particular {@link MotionEvent} axis.
     *
     * @see InputDevice#getMotionRange(int)
     */
    class MotionRange
        : public Object
        , public IMotionRange
    {
        friend class InputDevice;
    private:
        MotionRange(
            /* [in] */ Int32 axis,
            /* [in] */ Int32 source,
            /* [in] */ Float min,
            /* [in] */ Float max,
            /* [in] */ Float flat,
            /* [in] */ Float fuzz,
            /* [in] */ Float resolution);

    public:
        CAR_INTERFACE_DECL()

        /**
         * Gets the axis id.
         * @return The axis id.
         */
        CARAPI GetAxis(
            /* [out] */ Int32* axis);

        /**
         * Gets the source for which the axis is defined.
         * @return The source.
         */
        CARAPI GetSource(
            /* [out] */ Int32* source);


        /**
         * Determines whether the event is from the given source.
         *
         * @param source The input source to check against. This can be a specific device type,
         * such as {@link InputDevice#SOURCE_TOUCH_NAVIGATION}, or a more generic device class,
         * such as {@link InputDevice#SOURCE_CLASS_POINTER}.
         * @return Whether the event is from the given source.
         */
        CARAPI IsFromSource(
            /* [in] */ Int32 source,
            /* [out] */ Boolean* rst);

        /**
         * Gets the minimum value for the coordinate.
         * @return The minimum value.
         */
        CARAPI GetMin(
            /* [out] */ Float* minimum);

        /**
         * Gets the maximum value for the coordinate.
         * @return The minimum value.
         */
        CARAPI GetMax(
            /* [out] */ Float* maximum);

        /**
         * Gets the range of the coordinate (difference between maximum and minimum).
         * @return The range of values.
         */
        CARAPI GetRange(
            /* [out] */ Float* range);

        /**
         * Gets the extent of the center flat position with respect to this coordinate.
         * For example, a flat value of 8 means that the center position is between -8 and +8.
         * This value is mainly useful for calibrating self-centering devices.
         * @return The extent of the center flat position.
         */
        CARAPI GetFlat(
            /* [out] */ Float* flat);

        /**
         * Gets the error tolerance for input device measurements with respect to this coordinate.
         * For example, a value of 2 indicates that the measured value may be up to +/- 2 units
         * away from the actual value due to noise and device sensitivity limitations.
         * @return The error tolerance.
         */
        CARAPI GetFuzz(
            /* [out] */ Float* fuzz);

        /**
         * Gets the resolution for input device measurements with respect to this axis.
         * @return The resolution in units per millimeter, or units per radian for rotational axes.
         */
        CARAPI GetResolution(
            /* [out] */ Float* resolution);

    private:
        Int32 mAxis;
        Int32 mSource;
        Float mMin;
        Float mMax;
        Float mFlat;
        Float mFuzz;
        Float mResolution;
    };

public:
    /**
     * Gets information about the input device with the specified id.
     * @param id The device id.
     * @return The input device or NULL if not found.
     */
    static CARAPI GetDevice(
        /* [in] */ Int32 id,
        /* [out] */ IInputDevice** device);

    /**
     * Gets the ids of all input devices in the system.
     * @return The input device ids.
     */
    static CARAPI GetDeviceIds(
        /* [out, callee] */ ArrayOf<Int32>** deviceIds);

public:
    CAR_INTERFACE_DECL()

    InputDevice();

    ~InputDevice();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ Int32 generation,
        /* [in] */ Int32 controllerNumber,
        /* [in] */ const String& name,
        /* [in] */ Int32 vendorId,
        /* [in] */ Int32 productId,
        /* [in] */ const String& descriptor,
        /* [in] */ Boolean isExternal,
        /* [in] */ Int32 sources,
        /* [in] */ Int32 keyboardType,
        /* [in] */ IKeyCharacterMap* keyCharacterMap,
        /* [in] */ Boolean hasVibrator,
        /* [in] */ Boolean hasButtonUnderPad);

    /**
     * Gets the input device id.
     * <p>
     * Each input device receives a unique id when it is first configured
     * by the system.  The input device id may change when the system is restarted or if the
     * input device is disconnected, reconnected or reconfigured at any time.
     * If you require a stable identifier for a device that persists across
     * boots and reconfigurations, use {@link #getDescriptor()}.
     * </p>
     *
     * @return The input device id.
     */
    CARAPI GetId(
        /* [out] */ Int32* id);

    /**
     * The controller number for a given input device.
     * <p>
     * Each gamepad or joystick is given a unique, positive controller number when initially
     * configured by the system. This number may change due to events such as device disconnects /
     * reconnects or user initiated reassignment. Any change in number will trigger an event that
     * can be observed by registering an {@link InputManager.InputDeviceListener}.
     * </p>
     * <p>
     * All input devices which are not gamepads or joysticks will be assigned a controller number
     * of 0.
     * </p>
     *
     * @return The controller number of the device.
     */
    CARAPI GetControllerNumber(
        /* [out] */ Int32* number);

    /**
     * The set of identifying information for type of input device. This
     * information can be used by the system to configure appropriate settings
     * for the device.
     *
     * @return The identifier object for this device
     * @hide
     */
    CARAPI GetIdentifier(
        /* [out] */ IInputDeviceIdentifier** identifier);

    /**
     * Gets a generation number for this input device.
     * The generation number is incremented whenever the device is reconfigured and its
     * properties may have changed.
     *
     * @return The generation number.
     *
     * @hide
     */
    CARAPI GetGeneration(
        /* [out] */ Int32* generation);

    /**
     * Gets the vendor id for the given device, if available.
     * <p>
     * A vendor id uniquely identifies the company who manufactured the device. A value of 0 will
     * be assigned where a vendor id is not available.
     * </p>
     *
     * @return The vendor id of a given device
     */
    CARAPI GetVendorId(
        /* [out] */ Int32* id);

    /**
     * Gets the product id for the given device, if available.
     * <p>
     * A product id uniquely identifies which product within the address space of a given vendor,
     * identified by the device's vendor id. A value of 0 will be assigned where a product id is
     * not available.
     * </p>
     *
     * @return The product id of a given device
     */
    CARAPI GetProductId(
        /* [out] */ Int32* id);

    /**
     * Gets the input device descriptor, which is a stable identifier for an input device.
     * <p>
     * An input device descriptor uniquely identifies an input device.  Its value
     * is intended to be persistent across system restarts, and should not change even
     * if the input device is disconnected, reconnected or reconfigured at any time.
     * </p><p>
     * It is possible for there to be multiple {@link InputDevice} instances that have the
     * same input device descriptor.  This might happen in situations where a single
     * human input device registers multiple {@link InputDevice} instances (HID collections)
     * that describe separate features of the device, such as a keyboard that also
     * has a trackpad.  Alternately, it may be that the input devices are simply
     * indistinguishable, such as two keyboards made by the same manufacturer.
     * </p><p>
     * The input device descriptor returned by {@link #getDescriptor} should only be
     * used when an application needs to remember settings associated with a particular
     * input device.  For all other purposes when referring to a logical
     * {@link InputDevice} instance at runtime use the id returned by {@link #getId()}.
     * </p>
     *
     * @return The input device descriptor.
     */
    CARAPI GetDescriptor(
        /* [out] */ String* descriptor);

    /**
     * Returns true if the device is a virtual input device rather than a real one,
     * such as the virtual keyboard (see {@link KeyCharacterMap#VIRTUAL_KEYBOARD}).
     * <p>
     * Virtual input devices are provided to implement system-level functionality
     * and should not be seen or configured by users.
     * </p>
     *
     * @return True if the device is virtual.
     *
     * @see KeyCharacterMap#VIRTUAL_KEYBOARD
     */
    CARAPI IsVirtual(
        /* [out] */ Boolean* isVirtual);

    /**
     * Returns true if the device is external (connected to USB or Bluetooth or some other
     * peripheral bus), otherwise it is built-in.
     *
     * @return True if the device is external.
     *
     * @hide
     */
    CARAPI IsExternal(
        /* [out] */ Boolean* isExternal);

    /**
     * Returns true if the device is a full keyboard.
     *
     * @return True if the device is a full keyboard.
     *
     * @hide
     */
    CARAPI IsFullKeyboard(
        /* [out] */ Boolean* isFullKeyboard);

    /**
     * Gets the name of this input device.
     * @return The input device name.
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Gets the input sources supported by this input device as a combined bitfield.
     * @return The supported input sources.
     */
    CARAPI GetSources(
        /* [out] */ Int32* sources);

    /**
     * Determines whether the input device supports the given source or sources.
     *
     * @param source The input source or sources to check against. This can be a generic device
     * type such as {@link InputDevice#SOURCE_MOUSE}, a more generic device class, such as
     * {@link InputDevice#SOURCE_CLASS_POINTER}, or a combination of sources bitwise ORed together.
     * @return Whether the device can produce all of the given sources.
     */
    CARAPI SupportsSource(
        /* [in] */ Int32 source,
        /* [out] */ Boolean* rst);

    /**
     * Gets the keyboard type.
     * @return The keyboard type.
     */
    CARAPI GetKeyboardType(
        /* [out] */ Int32* type);

    /**
     * Gets the key character map associated with this input device.
     * @return The key character map.
     */
    CARAPI GetKeyCharacterMap(
        /* [out] */ IKeyCharacterMap** keyCharacterMap);

    CARAPI GetHasVibrator(
        /* [out] */ Boolean* hasVibrator);

    CARAPI GetHasButtonUnderPad(
        /* [out] */ Boolean* hasButtonUnderPad);

    /**
     * Gets whether the device is capable of producing the list of keycodes.
     * @param keys The list of android keycodes to check for.
     * @return An array of booleans where each member specifies whether the device is capable of
     * generating the keycode given by the corresponding value at the same index in the keys array.
     */
    CARAPI HasKeys(
        /* [in] */ ArrayOf<Int32>* keys,
        /* [out, callee] */ ArrayOf<Boolean>** rsts);

    /**
     * Gets information about the range of values for a particular {@link MotionEvent} axis.
     * If the device supports multiple sources, the same axis may have different meanings
     * for each source.  Returns information about the first axis found for any source.
     * To obtain information about the axis for a specific source, use
     * {@link #getMotionRange(int, int)}.
     *
     * @param axis The axis constant.
     * @return The range of values, or null if the requested axis is not
     * supported by the device.
     *
     * @see MotionEvent#AXIS_X
     * @see MotionEvent#AXIS_Y
     * @see #getSupportedAxes()
     */
    CARAPI GetMotionRange(
        /* [in] */ Int32 axis,
        /* [out] */ IMotionRange** motionRange);

    /**
     * Gets information about the range of values for a particular {@link MotionEvent} axis
     * used by a particular source on the device.
     * If the device supports multiple sources, the same axis may have different meanings
     * for each source.
     *
     * @param axis The axis constant.
     * @param source The source for which to return information.
     * @return The range of values, or null if the requested axis is not
     * supported by the device.
     *
     * @see MotionEvent#AXIS_X
     * @see MotionEvent#AXIS_Y
     * @see #getSupportedAxes()
     */
    CARAPI GetMotionRange(
        /* [in] */ Int32 axis,
        /* [in] */ Int32 source,
        /* [out] */ IMotionRange** motionRange);

    /**
     * Gets the ranges for all axes supported by the device.
     * @return The motion ranges for the device.
     *
     * @see #getMotionRange(int, int)
     */
    CARAPI GetMotionRanges(
        /* [out] */ IList** motionRanges);

    /**
     * Gets the vibrator service associated with the device, if there is one.
     * Even if the device does not have a vibrator, the result is never null.
     * Use {@link Vibrator#hasVibrator} to determine whether a vibrator is
     * present.
     *
     * Note that the vibrator associated with the device may be different from
     * the system vibrator.  To obtain an instance of the system vibrator instead, call
     * {@link Context#getSystemService} with {@link Context#VIBRATOR_SERVICE} as argument.
     *
     * @return The vibrator service associated with the device, never null.
     */
    CARAPI GetVibrator(
        /* [out] */ IVibrator** vibrator);

    /**
     * Reports whether the device has a button under its touchpad
     * @return Whether the device has a button under its touchpad
     * @hide
     */
    CARAPI HasButtonUnderPad(
        /* [in] */ Boolean* rst);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel *source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel *dest);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI AddMotionRange(
        /* [in] */ Int32 axis,
        /* [in] */ Int32 source,
        /* [in] */ Float min,
        /* [in] */ Float max,
        /* [in] */ Float flat,
        /* [in] */ Float fuzz,
        /* [in] */ Float resolution);

private:
    CARAPI AppendSourceDescriptionIfApplicable(
        /* [in] */ StringBuilder& description,
        /* [in] */ Int32 source,
        /* [in] */ const String& sourceName);

private:
    friend class Elastos::Droid::Os::AndroidParcelUtils;

    Int32 mId;
    Int32 mGeneration;
    Int32 mControllerNumber;
    String mName;
    Int32 mVendorId;
    Int32 mProductId;
    String mDescriptor;
    AutoPtr<IInputDeviceIdentifier> mIdentifier;
    Boolean mIsExternal;
    Int32 mSources;
    Int32 mKeyboardType;
    AutoPtr<IKeyCharacterMap> mKeyCharacterMap;
    Boolean mHasVibrator;
    Boolean mHasButtonUnderPad;
    AutoPtr<IArrayList> mMotionRanges;

    AutoPtr<IVibrator> mVibrator; // guarded by mMotionRanges during initialization
};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_INPUTDEVICE_H__
