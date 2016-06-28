
#ifndef __ELASTOS_DROID_SERVER_DEVICEPOLICY_CDEVICEPOLICYMANAGERSERVICE__SETUPCONTENTOBSERVER_H__
#define __ELASTOS_DROID_SERVER_DEVICEPOLICY_CDEVICEPOLICYMANAGERSERVICE__SETUPCONTENTOBSERVER_H__

#include "_Elastos_Droid_Server_DevicePolicy_CDevicePolicyManagerServiceSetupContentObserver.h"
#include "elastos/droid/server/devicepolicy/CDevicePolicyManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace DevicePolicy {

CarClass(CDevicePolicyManagerServiceSetupContentObserver)
    , public CDevicePolicyManagerService::SetupContentObserver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DevicePolicy
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_DEVICEPOLICY_CDEVICEPOLICYMANAGERSERVICE__SETUPCONTENTOBSERVER_H__
