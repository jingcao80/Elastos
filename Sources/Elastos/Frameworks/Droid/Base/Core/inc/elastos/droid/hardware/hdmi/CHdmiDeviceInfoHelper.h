
#ifndef __ELASTOS_DROID_HARDWARE_HDMI_CHDMIDEVICEINFOHELPER_H__
#define __ELASTOS_DROID_HARDWARE_HDMI_CHDMIDEVICEINFOHELPER_H__

#include "_Elastos_Droid_Hardware_Hdmi_CHdmiDeviceInfoHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CarClass(CHdmiDeviceInfoHelper)
    , public Singleton
    , public IHdmiDeviceInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IdForCecDevice(
        /* [in] */ Int32 address,
        /* [out] */ Int32* device);

    CARAPI IdForMhlDevice(
        /* [in] */ Int32 portId,
        /* [out] */ Int32* device);

    CARAPI IdForHardware(
        /* [in] */ Int32 portId,
        /* [out] */ Int32* hardware);
};

} // namespace Hdmi
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_HDMI_CHDMIDEVICEINFOHELPER_H__
