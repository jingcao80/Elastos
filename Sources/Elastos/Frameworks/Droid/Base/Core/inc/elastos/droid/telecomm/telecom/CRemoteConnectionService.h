#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CREMOTECONNECTIONSERVICE_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CREMOTECONNECTIONSERVICE_H__

#include "_Elastos_Droid_Telecomm_Telecom_CRemoteConnectionService.h"
#include "elastos/droid/telecomm/telecom/RemoteConnectionService.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CRemoteConnectionService)
    , public RemoteConnectionService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CREMOTECONNECTIONSERVICE_H__