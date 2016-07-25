#ifndef __ELASTOS_DROID_TELECOM_CREMOTECONNECTIONMANAGER_H__
#define __ELASTOS_DROID_TELECOM_CREMOTECONNECTIONMANAGER_H__

#include "_Elastos_Droid_Telecom_CRemoteConnectionManager.h"
#include "elastos/droid/telecom/RemoteConnectionManager.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CRemoteConnectionManager)
    , public RemoteConnectionManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CREMOTECONNECTIONMANAGER_H__