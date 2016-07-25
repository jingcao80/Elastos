#ifndef __ELASTOS_DROID_TELECOM_CCONNECTIONREQUEST_H__
#define __ELASTOS_DROID_TELECOM_CCONNECTIONREQUEST_H__

#include "_Elastos_Droid_Telecom_CConnectionRequest.h"
#include "elastos/droid/telecom/ConnectionRequest.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CConnectionRequest)
    , public ConnectionRequest
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CCONNECTIONREQUEST_H__