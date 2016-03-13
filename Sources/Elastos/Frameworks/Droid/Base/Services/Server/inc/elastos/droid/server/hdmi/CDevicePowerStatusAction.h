
#ifndef __ELASTOS_DROID_SERVER_HDMI_CDEVICEPOWERSTATUSACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CDEVICEPOWERSTATUSACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CDevicePowerStatusAction.h"
#include "elastos/droid/server/hdmi/DevicePowerStatusAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that queries the power status of other device. This action is initiated via
 * {@link HdmiPlaybackClient#queryDisplayStatus(DisplayStatusCallback)} from the Android system
 * working as playback device to get the power status of TV device.
 * <p>
 * Package-private, accessed by {@link HdmiControlService} only.
 */
CarClass(CDevicePowerStatusAction)
    , public DevicePowerStatusAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CDEVICEPOWERSTATUSACTION_H__
