
#ifndef __ELASTOS_DROID_SERVER_HDMI_CDEVICEDISCOVERYACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CDEVICEDISCOVERYACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CDeviceDiscoveryAction.h"
#include "elastos/droid/server/hdmi/DeviceDiscoveryAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that handles device discovery sequences.
 * Device discovery is launched when TV device is woken from "Standby" state
 * or enabled "Control for Hdmi" from disabled state.
 *
 * <p>Device discovery goes through the following steps.
 * <ol>
 *   <li>Poll all non-local devices by sending &lt;Polling Message&gt;
 *   <li>Gather "Physical address" and "device type" of all acknowledged devices
 *   <li>Gather "OSD (display) name" of all acknowledge devices
 *   <li>Gather "Vendor id" of all acknowledge devices
 * </ol>
 */
CarClass(CDeviceDiscoveryAction)
    , public DeviceDiscoveryAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CDEVICEDISCOVERYACTION_H__
