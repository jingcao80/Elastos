
#ifndef __ELASTOS_DROID_SERVER_HDMI_CSYSTEMAUDIOAUTOINITIATIONACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CSYSTEMAUDIOAUTOINITIATIONACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CSystemAudioAutoInitiationAction.h"
#include "elastos/droid/server/hdmi/SystemAudioAutoInitiationAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

// Seq #27
CarClass(CSystemAudioAutoInitiationAction)
    , public SystemAudioAutoInitiationAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CSYSTEMAUDIOAUTOINITIATIONACTION_H__
