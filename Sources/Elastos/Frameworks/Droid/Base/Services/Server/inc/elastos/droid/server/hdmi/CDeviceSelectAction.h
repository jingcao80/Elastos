
#ifndef __ELASTOS_DROID_SERVER_HDMI_CDEVICESELECTACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CDEVICESELECTACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CDeviceSelectAction.h"
#include "elastos/droid/server/hdmi/DeviceSelectAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Handles an action that selects a logical device as a new active source.
 *
 * Triggered by {@link HdmiTvClient}, attempts to select the given target device
 * for a new active source. It does its best to wake up the target in standby mode
 * before issuing the command &gt;Set Stream path&lt;.
 */
CarClass(CDeviceSelectAction)
    , public DeviceSelectAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CDEVICESELECTACTION_H__
