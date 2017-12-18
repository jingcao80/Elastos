
#include "elastos/droid/hardware/input/IInputManager.h"
#include "elastos/droid/os/AndroidParcelUtils.h"
#include <binder/Parcel.h>

using Elastos::Droid::Os::AndroidParcelUtils;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

const String IInputManagerProxy::DESCRIPTOR("android.hardware.input.IInputManager");
const Int32 IInputManagerProxy::TRANSACTION_getInputDevice = android::IBinder::FIRST_CALL_TRANSACTION + 0;
const Int32 IInputManagerProxy::TRANSACTION_getInputDeviceIds = android::IBinder::FIRST_CALL_TRANSACTION + 1;
const Int32 IInputManagerProxy::TRANSACTION_registerInputDevicesChangedListener = android::IBinder::FIRST_CALL_TRANSACTION + 17;

CAR_INTERFACE_IMPL(IInputManagerProxy, Object, IIInputManager)

IInputManagerProxy::IInputManagerProxy(
    /* [in] */ const android::sp<android::IBinder>& remote)
    : mRemote(remote)
{}

ECode IInputManagerProxy::GetInputDevice(
    /* [in] */ Int32 deviceId,
    /* [out] */ IInputDevice** device)
{
    VALIDATE_NOT_NULL(device);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteInt32(data, deviceId);
    mRemote->transact(TRANSACTION_getInputDevice, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IInputDevice> result = AndroidParcelUtils::ReadInputDevice(reply);
        *device = result;
        REFCOUNT_ADD(*device);
    }
    else {
        *device = NULL;
    }
    return NOERROR;
}

ECode IInputManagerProxy::GetInputDeviceIds(
    /* [out, callee] */ ArrayOf<Int32>** deviceIds)
{
    VALIDATE_NOT_NULL(deviceIds);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    mRemote->transact(TRANSACTION_getInputDeviceIds, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    AutoPtr< ArrayOf<Int32> > result = AndroidParcelUtils::CreateInt32Array(reply);
    *deviceIds = result;
    REFCOUNT_ADD(*deviceIds);
    return NOERROR;
}

ECode IInputManagerProxy::HasKeys(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 sourceMask,
    /* [in] */ const ArrayOf<Int32>& keyCodes,
    /* [out] */ ArrayOf<Boolean>* keyExists,
    /* [out] */ Boolean* res)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputManagerProxy::TryPointerSpeed(
    /* [in] */ Int32 speed)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputManagerProxy::InjectInputEvent(
    /* [in] */ IInputEvent* ev,
    /* [in] */ Int32 mode,
    /* [out] */ Boolean* res)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputManagerProxy::GetTouchCalibrationForInputDevice(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [in] */ Int32 rotation,
    /* [out] */ ITouchCalibration** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputManagerProxy::SetTouchCalibrationForInputDevice(
    /* [in] */ const String& inputDeviceDescriptor,
    /* [in] */ Int32 rotation,
    /* [in] */ ITouchCalibration* calibration)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputManagerProxy::GetKeyboardLayouts(
    /* [out, callee] */ ArrayOf<IKeyboardLayout*>** layouts)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputManagerProxy::GetKeyboardLayout(
    /* [in] */ const String& keyboardLayoutDescriptor,
    /* [out] */ IKeyboardLayout** layout)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputManagerProxy::GetCurrentKeyboardLayoutForInputDevice(
    /* [in] */ IInputDeviceIdentifier* identifier,
    /* [out] */ String* keyboardLayoutDescriptor)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputManagerProxy::SetCurrentKeyboardLayoutForInputDevice(
    /* [in] */ IInputDeviceIdentifier* identifier,
    /* [in] */ const String& keyboardLayoutDescriptor)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputManagerProxy::GetKeyboardLayoutsForInputDevice(
    /* [in] */ IInputDeviceIdentifier* identifier,
    /* [out, callee] */ ArrayOf<String>** keyboardLayoutDescriptors)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputManagerProxy::AddKeyboardLayoutForInputDevice(
    /* [in] */ IInputDeviceIdentifier* identifier,
    /* [in] */ const String& keyboardLayoutDescriptor)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputManagerProxy::RemoveKeyboardLayoutForInputDevice(
    /* [in] */ IInputDeviceIdentifier* identifier,
    /* [in] */ const String& keyboardLayoutDescriptor)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputManagerProxy::RegisterInputDevicesChangedListener(
    /* [in] */ IInputDevicesChangedListener* listener)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteIInputDevicesChangedListener(data, listener);
    mRemote->transact(TRANSACTION_registerInputDevicesChangedListener, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

ECode IInputManagerProxy::Vibrate(
    /* [in] */ Int32 deviceId,
    /* [in] */ const ArrayOf<Int64>& pattern,
    /* [in] */ Int32 repeat,
    /* [in] */ IBinder* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputManagerProxy::CancelVibrate(
    /* [in] */ Int32 deviceId,
    /* [in] */ IBinder* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
