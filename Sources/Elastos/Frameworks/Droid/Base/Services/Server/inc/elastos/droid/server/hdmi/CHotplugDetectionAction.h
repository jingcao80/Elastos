
#ifndef __ELASTOS_DROID_SERVER_HDMI_CHOTPLUGDETECTIONACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CHOTPLUGDETECTIONACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CHotplugDetectionAction.h"
#include "elastos/droid/server/hdmi/HotplugDetectionAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

// Seq #3
CarClass(CHotplugDetectionAction)
    , public HotplugDetectionAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CHOTPLUGDETECTIONACTION_H__
