#ifndef __ELASTOS_DROID_SERVER_CPROFILEMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CPROFILEMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_CProfileManagerService.h"
#include "elastos/droid/server/ProfileManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CProfileManagerService)
    , public ProfileManagerService
{
public:
    CAR_OBJECT_DECL()
};

} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SERVER_CPROFILEMANAGERSERVICE_H__
