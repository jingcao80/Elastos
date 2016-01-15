
#include "elastos/droid/hardware/usb/CUsbDeviceHelper.h"
#include "elastos/droid/hardware/usb/UsbDevice.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CAR_INTERFACE_IMPL(CUsbDeviceHelper, Singleton, IUsbDeviceHelper)

CAR_SINGLETON_IMPL(CUsbDeviceHelper)

ECode CUsbDeviceHelper::GetDeviceId(
    /* [in] */ const String& name,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = UsbDevice::NativeGetDeviceId(name);
    return NOERROR;
}

ECode CUsbDeviceHelper::GetDeviceName(
    /* [in] */ Int32 id,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = UsbDevice::NativeGetDeviceName(id);
    return NOERROR;
}

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
