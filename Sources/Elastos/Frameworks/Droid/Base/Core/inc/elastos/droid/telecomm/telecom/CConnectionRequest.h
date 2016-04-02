#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CCONNECTIONREQUEST_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CCONNECTIONREQUEST_H__

#include "_Elastos_Droid_Telecomm_Telecom_CConnectionRequest.h"
#include "elastos/droid/telecomm/telecom/ConnectionRequest.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CConnectionRequest)
    , public ConnectionRequest
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CCONNECTIONREQUEST_H__