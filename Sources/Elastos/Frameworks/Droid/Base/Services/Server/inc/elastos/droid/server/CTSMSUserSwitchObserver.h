#ifndef __ELASTOS_DROID_SERVER_CTSMSUSERSWITCHOBSERVER_H__
#define __ELASTOS_DROID_SERVER_CTSMSUSERSWITCHOBSERVER_H__

#include "_Elastos_Droid_Server_CTSMSUserSwitchObserver.h"
#include "elastos/droid/server/CTextServicesManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CTSMSUserSwitchObserver)
    , public CTextServicesManagerService::TSMSUserSwitchObserver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CTSMSUSERSWITCHOBSERVER_H__
