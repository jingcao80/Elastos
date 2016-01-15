#ifndef __ELASTOS_DROID_SERVER_LIGHTS_CLEGACYFLASHLIGHTHACK_H__
#define __ELASTOS_DROID_SERVER_LIGHTS_CLEGACYFLASHLIGHTHACK_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_Lights_CLegacyFlashlightHack.h"
#include "elastos/droid/server/lights/LightsService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Lights {

CarClass(CLegacyFlashlightHack)
    , public LightsService::LegacyFlashlightHack
{
public:
    CAR_OBJECT_DECL();
};

} // Lights
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_LIGHTS_CLEGACYFLASHLIGHTHACK_H__
