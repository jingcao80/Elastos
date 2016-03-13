
#ifndef __ELASTOS_DROID_SERVER_HDMI_CROUTINGCONTROLACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CROUTINGCONTROLACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CRoutingControlAction.h"
#include "elastos/droid/server/hdmi/RoutingControlAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action for routing control. Exchanges routing-related commands with other devices
 * to determine the new active source.
 *
 * <p>This action is initiated by various cases:
 * <ul>
 * <li> Manual TV input switching
 * <li> Routing change of a CEC switch other than TV
 * <li> New CEC device at the tail of the active routing path
 * <li> Removed CEC device from the active routing path
 * <li> Routing at CEC enable time
 * </ul>
 */
CarClass(CRoutingControlAction)
    , public RoutingControlAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CROUTINGCONTROLACTION_H__
