#ifndef __ELASTOS_DROID_SERVER_INPUT_CINPUTFILTERHOST_H__
#define __ELASTOS_DROID_SERVER_INPUT_CINPUTFILTERHOST_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/server/input/CInputManagerService.h"
#include "_Elastos_Droid_Server_Input_CInputFilterHost.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

CarClass(CInputFilterHost)
    , public CInputManagerService::InputFilterHost
{
public:
    CAR_OBJECT_DECL();
};

} // Input
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_INPUT_CINPUTFILTERHOST_H__
