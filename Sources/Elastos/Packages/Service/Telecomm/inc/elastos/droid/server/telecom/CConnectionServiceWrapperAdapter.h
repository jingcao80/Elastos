
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CCONNECTIONSERVICEWRAPPERADAPTER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CCONNECTIONSERVICEWRAPPERADAPTER_H__

#include "_Elastos_Droid_Server_Telecom_CConnectionServiceWrapperAdapter.h"
#include "elastos/droid/server/telecom/ConnectionServiceWrapper.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CarClass(CConnectionServiceWrapperAdapter)
    , public ConnectionServiceWrapper::Adapter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CCONNECTIONSERVICEWRAPPERADAPTER_H__
