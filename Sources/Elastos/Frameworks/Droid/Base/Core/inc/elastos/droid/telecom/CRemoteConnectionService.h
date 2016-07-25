#ifndef __ELASTOS_DROID_TELECOM_CREMOTECONNECTIONSERVICE_H__
#define __ELASTOS_DROID_TELECOM_CREMOTECONNECTIONSERVICE_H__

#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Telecom_CRemoteConnectionService.h"
#include "elastos/droid/telecom/RemoteConnectionService.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CRemoteConnectionService)
    , public RemoteConnectionService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CREMOTECONNECTIONSERVICE_H__
