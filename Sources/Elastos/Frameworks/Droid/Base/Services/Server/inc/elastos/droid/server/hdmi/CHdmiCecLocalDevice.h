
#ifndef __ELASTOS_DROID_SERVER_HDMI_CHDMICECLOCALDEVICE_H__
#define __ELASTOS_DROID_SERVER_HDMI_CHDMICECLOCALDEVICE_H__

#include "_Elastos_Droid_Server_Hdmi_CHdmiCecLocalDevice.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDevice.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Class that models a logical CEC device hosted in this system. Handles initialization,
 * CEC commands that call for actions customized per device type.
 */
CarClass(CHdmiCecLocalDevice)
    , public HdmiCecLocalDevice
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CHDMICECLOCALDEVICE_H__
