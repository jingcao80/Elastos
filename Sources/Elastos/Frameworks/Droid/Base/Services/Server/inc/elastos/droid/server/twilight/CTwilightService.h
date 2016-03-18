
#ifndef __ELASTOS_DROID_SERVER_TWILIGHT_CTWILIGHT_SERVICE_H__
#define __ELASTOS_DROID_SERVER_TWILIGHT_CTWILIGHT_SERVICE_H__

#include "_Elastos_Droid_Server_Twilight_CTwilightService.h"
#include "elastos/droid/server/twilight/TwilightService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Twilight {

CarClass(CTwilightService)
    , public TwilightService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Twilight
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_TWILIGHT_CTWILIGHT_SERVICE_H__
