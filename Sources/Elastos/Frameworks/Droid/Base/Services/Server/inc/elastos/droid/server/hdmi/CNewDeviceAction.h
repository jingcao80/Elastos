
#ifndef __ELASTOS_DROID_SERVER_HDMI_CNEWDEVICEACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CNEWDEVICEACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CNewDeviceAction.h"
#include "elastos/droid/server/hdmi/NewDeviceAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that discovers the information of a newly found logical device.
 *
 * This action is created when receiving &lt;Report Physical Address&gt;, a CEC command a newly
 * connected HDMI-CEC device broadcasts to announce its advent. Additional commands are issued in
 * this action to gather more information on the device such as OSD name and device vendor ID.
 *
 * <p>The result is made in the form of {@link HdmiDeviceInfo} object, and passed to service
 * for the management through its life cycle.
 *
 * <p>Package-private, accessed by {@link HdmiControlService} only.
 */
CarClass(CNewDeviceAction)
    , public NewDeviceAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CNEWDEVICEACTION_H__
