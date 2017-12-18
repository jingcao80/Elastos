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

#ifndef __ELASTOS_DROID_HARDWARE_INPUT_IINPUTMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_INPUT_IINPUTMANAGER_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IInputEvent;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

class IInputManagerProxy
    : public Object
    , public IIInputManager
{
public:
    IInputManagerProxy(
        /* [in] */ const android::sp<android::IBinder>& remote);

    CAR_INTERFACE_DECL();

    CARAPI GetInputDevice(
        /* [in] */ Int32 deviceId,
        /* [out] */ IInputDevice** device);

    CARAPI GetInputDeviceIds(
        /* [out, callee] */ ArrayOf<Int32>** deviceIds);

    CARAPI HasKeys(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 sourceMask,
        /* [in] */ const ArrayOf<Int32>& keyCodes,
        /* [out] */ ArrayOf<Boolean>* keyExists,
        /* [out] */ Boolean* res);

    CARAPI TryPointerSpeed(
        /* [in] */ Int32 speed);

    CARAPI InjectInputEvent(
        /* [in] */ IInputEvent* ev,
        /* [in] */ Int32 mode,
        /* [out] */ Boolean* res);

    // Calibrate input device position
    CARAPI GetTouchCalibrationForInputDevice(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ Int32 rotation,
        /* [out] */ ITouchCalibration** result);

    CARAPI SetTouchCalibrationForInputDevice(
        /* [in] */ const String& inputDeviceDescriptor,
        /* [in] */ Int32 rotation,
        /* [in] */ ITouchCalibration* calibration);

    CARAPI GetKeyboardLayouts(
        /* [out, callee] */ ArrayOf<IKeyboardLayout*>** layouts);

    CARAPI GetKeyboardLayout(
        /* [in] */ const String& keyboardLayoutDescriptor,
        /* [out] */ IKeyboardLayout** layout);

    CARAPI GetCurrentKeyboardLayoutForInputDevice(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [out] */ String* keyboardLayoutDescriptor);

    CARAPI SetCurrentKeyboardLayoutForInputDevice(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [in] */ const String& keyboardLayoutDescriptor);

    CARAPI GetKeyboardLayoutsForInputDevice(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [out, callee] */ ArrayOf<String>** keyboardLayoutDescriptors);

    CARAPI AddKeyboardLayoutForInputDevice(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [in] */ const String& keyboardLayoutDescriptor);

    CARAPI RemoveKeyboardLayoutForInputDevice(
        /* [in] */ IInputDeviceIdentifier* identifier,
        /* [in] */ const String& keyboardLayoutDescriptor);

    CARAPI RegisterInputDevicesChangedListener(
        /* [in] */ IInputDevicesChangedListener* listener);

    CARAPI Vibrate(
        /* [in] */ Int32 deviceId,
        /* [in] */ const ArrayOf<Int64>& pattern,
        /* [in] */ Int32 repeat,
        /* [in] */ IBinder* token);

    CARAPI CancelVibrate(
        /* [in] */ Int32 deviceId,
        /* [in] */ IBinder* token);

private:
    android::sp<android::IBinder> mRemote;

    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_getInputDevice;
    static const Int32 TRANSACTION_getInputDeviceIds;
    static const Int32 TRANSACTION_registerInputDevicesChangedListener;
};

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_INPUT_IINPUTMANAGER_H__
