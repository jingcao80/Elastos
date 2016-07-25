#ifndef __ELASTOS_DROID_TELECOM_CCONNECTIONSERVICEADAPTERSERVANT_H__
#define __ELASTOS_DROID_TELECOM_CCONNECTIONSERVICEADAPTERSERVANT_H__

#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Telecom_CConnectionServiceAdapterServant.h"
#include "elastos/droid/telecom/ConnectionServiceAdapterServant.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CConnectionServiceAdapterServant)
    , public ConnectionServiceAdapterServant
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CCONNECTIONSERVICEADAPTERSERVANT_H__
