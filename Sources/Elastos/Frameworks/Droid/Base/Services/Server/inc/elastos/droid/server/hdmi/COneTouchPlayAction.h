
#ifndef __ELASTOS_DROID_SERVER_HDMI_CONETOUCHPLAYACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CONETOUCHPLAYACTION_H__

#include "_Elastos_Droid_Server_Hdmi_COneTouchPlayAction.h"
#include "elastos/droid/server/hdmi/OneTouchPlayAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that performs one touch play against TV/Display device. This action is initiated
 * via {@link android.hardware.hdmi.HdmiPlaybackClient#oneTouchPlay(OneTouchPlayCallback)} from the
 * Android system working as playback device to turn on the TV, and switch the input.
 * <p>
 * Package-private, accessed by {@link HdmiControlService} only.
 */
CarClass(COneTouchPlayAction)
    , public OneTouchPlayAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CONETOUCHPLAYACTION_H__
