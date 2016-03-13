
#ifndef __ELASTOS_DROID_SERVER_HDMI_CSETARCTRANSMISSIONSTATEACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CSETARCTRANSMISSIONSTATEACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CSetArcTransmissionStateAction.h"
#include "elastos/droid/server/hdmi/SetArcTransmissionStateAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that handles enabling/disabling of ARC transmission channel.
 * Once TV gets &lt;Initiate ARC&gt;, TV sends &lt;Report ARC Initiated&gt; to AV Receiver.
 * If it fails or it gets &lt;Terminate ARC&gt;, TV just disables ARC.
 */
CarClass(CSetArcTransmissionStateAction)
    , public SetArcTransmissionStateAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CSETARCTRANSMISSIONSTATEACTION_H__
