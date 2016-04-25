
#ifndef __ELASTOS_DROID_HARDWARE_HDMI_CHDMIDEVICEINFO_H__
#define __ELASTOS_DROID_HARDWARE_HDMI_CHDMIDEVICEINFO_H__

#include "_Elastos_Droid_Hardware_Hdmi_CHdmiDeviceInfo.h"
#include "elastos/droid/hardware/hdmi/HdmiDeviceInfo.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CarClass(CHdmiDeviceInfo) , public HdmiDeviceInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_HDMI_CHDMIDEVICEINFO_H__
