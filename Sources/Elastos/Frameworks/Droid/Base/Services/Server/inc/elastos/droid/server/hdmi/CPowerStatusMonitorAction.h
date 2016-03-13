
#ifndef __ELASTOS_DROID_SERVER_HDMI_CPOWERSTATUSMONITORACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CPOWERSTATUSMONITORACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CPowerStatusMonitorAction.h"
#include "elastos/droid/server/hdmi/PowerStatusMonitorAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Action that check each device's power status.
 */
CarClass(CPowerStatusMonitorAction)
    , public PowerStatusMonitorAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CPOWERSTATUSMONITORACTION_H__
