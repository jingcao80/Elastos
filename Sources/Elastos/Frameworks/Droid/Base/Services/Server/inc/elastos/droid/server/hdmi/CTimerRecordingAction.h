
#ifndef __ELASTOS_DROID_SERVER_HDMI_CTIMERRECORDINGACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CTIMERRECORDINGACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CTimerRecordingAction.h"
#include "elastos/droid/server/hdmi/TimerRecordingAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that performs timer recording.
 */
CarClass(CTimerRecordingAction)
    , public TimerRecordingAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CTIMERRECORDINGACTION_H__
