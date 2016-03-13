
#ifndef __ELASTOS_DROID_SERVER_HDMI_CREQUESTARCINITIATIONACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CREQUESTARCINITIATIONACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CRequestArcInitiationAction.h"
#include "elastos/droid/server/hdmi/RequestArcInitiationAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that handles ARC action initiated by TV devices.
 *
 * <p>This action is created by TV's hot plug event of ARC port.
 */
CarClass(CRequestArcInitiationAction)
    , public RequestArcInitiationAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CREQUESTARCINITIATIONACTION_H__
