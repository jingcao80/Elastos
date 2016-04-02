#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CREMOTECONNECTIONMANAGER_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CREMOTECONNECTIONMANAGER_H__

#include "_Elastos_Droid_Telecomm_Telecom_CRemoteConnectionManager.h"
#include "elastos/droid/telecomm/telecom/RemoteConnectionManager.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CRemoteConnectionManager)
    , public RemoteConnectionManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CREMOTECONNECTIONMANAGER_H__