#ifndef __ELASTOS_DROID_TELECOM_CREMOTECONNECTION_H__
#define __ELASTOS_DROID_TELECOM_CREMOTECONNECTION_H__

#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Telecom_CRemoteConnection.h"
#include "elastos/droid/telecom/RemoteConnection.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CRemoteConnection)
    , public RemoteConnection
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CREMOTECONNECTION_H__
