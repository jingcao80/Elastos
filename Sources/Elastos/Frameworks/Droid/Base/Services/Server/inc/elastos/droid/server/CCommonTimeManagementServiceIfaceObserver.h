
#ifndef __ELASTOS_DROID_SERVER_CCOMMON_TIME_MANAGERMENT_SERVICE_IFACEOBSERVER_H__
#define __ELASTOS_DROID_SERVER_CCOMMON_TIME_MANAGERMENT_SERVICE_IFACEOBSERVER_H__

#include "_Elastos_Droid_Server_CCommonTimeManagementServiceIfaceObserver.h"
#include "elastos/droid/server/CCommonTimeManagementService.h"

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CCommonTimeManagementServiceIfaceObserver)
    , public CCommonTimeManagementService::IfaceObserver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CCOMMON_TIME_MANAGERMENT_SERVICE_IFACEOBSERVER_H__
