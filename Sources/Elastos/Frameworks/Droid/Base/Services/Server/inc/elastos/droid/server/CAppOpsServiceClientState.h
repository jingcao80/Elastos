#ifndef __ELASTOS_DROID_SERVER_CAPPOPSSERVICECLIENTSTATE_H__
#define __ELASTOS_DROID_SERVER_CAPPOPSSERVICECLIENTSTATE_H__

#include "_Elastos_Droid_Server_CAppOpsServiceClientState.h"
#include "elastos/droid/server/CAppOpsService.h"
#include <Elastos.CoreLibrary.Utility.h>

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CAppOpsServiceClientState)
    , public CAppOpsService::ClientState
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CAPPOPSSERVICECLIENTSTATE_H__
