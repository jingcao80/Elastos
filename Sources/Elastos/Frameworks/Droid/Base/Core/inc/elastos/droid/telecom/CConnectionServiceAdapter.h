#ifndef __ELASTOS_DROID_TELECOM_CCONNECTIONSERVICEADAPTER_H__
#define __ELASTOS_DROID_TELECOM_CCONNECTIONSERVICEADAPTER_H__

#include "_Elastos_Droid_Telecom_CConnectionServiceAdapter.h"
#include "elastos/droid/telecom/ConnectionServiceAdapter.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CConnectionServiceAdapter)
    , public ConnectionServiceAdapter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CCONNECTIONSERVICEADAPTER_H__