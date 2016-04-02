#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CCONNECTIONSERVICEADAPTER_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CCONNECTIONSERVICEADAPTER_H__

#include "_Elastos_Droid_Telecomm_Telecom_CConnectionServiceAdapter.h"
#include "elastos/droid/telecomm/telecom/ConnectionServiceAdapter.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CConnectionServiceAdapter)
    , public ConnectionServiceAdapter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CCONNECTIONSERVICEADAPTER_H__