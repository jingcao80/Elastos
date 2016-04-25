
#include "elastos/droid/hardware/hdmi/CHdmiDeviceInfoHelper.h"
#include "elastos/droid/hardware/hdmi/HdmiDeviceInfo.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CAR_INTERFACE_IMPL(CHdmiDeviceInfoHelper, Singleton, IHdmiDeviceInfoHelper)

CAR_SINGLETON_IMPL(CHdmiDeviceInfoHelper)

ECode CHdmiDeviceInfoHelper::IdForCecDevice(
    /* [in] */ Int32 address,
    /* [out] */ Int32* device)
{
    VALIDATE_NOT_NULL(device);

    return HdmiDeviceInfo::IdForCecDevice(address, device);
}

ECode CHdmiDeviceInfoHelper::IdForMhlDevice(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* device)
{
    VALIDATE_NOT_NULL(device);

    return HdmiDeviceInfo::IdForMhlDevice(portId, device);
}

ECode CHdmiDeviceInfoHelper::IdForHardware(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* hardware)
{
    VALIDATE_NOT_NULL(hardware);

    return HdmiDeviceInfo::IdForHardware(portId, hardware);
}

} // namespace Hdmi
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
