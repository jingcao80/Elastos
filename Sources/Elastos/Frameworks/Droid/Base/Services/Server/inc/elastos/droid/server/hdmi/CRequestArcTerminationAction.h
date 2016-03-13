
#ifndef __ELASTOS_DROID_SERVER_HDMI_CREQUESTARCTERMINATIONACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CREQUESTARCTERMINATIONACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CRequestArcTerminationAction.h"
#include "elastos/droid/server/hdmi/RequestArcTerminationAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action to handle <Request ARC Termination>.
 *
 * <p>It's initiated by user's manual termination or ARC channel close from TV.
 */
CarClass(CRequestArcTerminationAction)
    , public RequestArcTerminationAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CREQUESTARCTERMINATIONACTION_H__
