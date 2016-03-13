
#ifndef __ELASTOS_DROID_SERVER_HDMI_CSYSTEMAUDIOSTATUSACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CSYSTEMAUDIOSTATUSACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CSystemAudioStatusAction.h"
#include "elastos/droid/server/hdmi/SystemAudioStatusAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Action to update audio status (volume or mute) of audio amplifier
 */
CarClass(CSystemAudioStatusAction)
    , public SystemAudioStatusAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CSYSTEMAUDIOSTATUSACTION_H__
