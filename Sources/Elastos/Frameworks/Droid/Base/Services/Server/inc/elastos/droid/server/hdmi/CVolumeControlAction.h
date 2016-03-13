
#ifndef __ELASTOS_DROID_SERVER_HDMI_CVOLUMECONTROLACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CVOLUMECONTROLACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CVolumeControlAction.h"
#include "elastos/droid/server/hdmi/VolumeControlAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that transmits volume change to Audio Receiver.
 * <p>
 * This action is created when a user pressed volume up/down. However, Android only provides a
 * listener for delta of some volume change instead of individual key event. Also it's hard to know
 * Audio Receiver's number of volume steps for a single volume control key. Because of this, it
 * sends key-down event until IRT timeout happens, and it will send key-up event if no additional
 * volume change happens; otherwise, it will send again key-down as press and hold feature does.
 */
CarClass(CVolumeControlAction)
    , public VolumeControlAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CVOLUMECONTROLACTION_H__
