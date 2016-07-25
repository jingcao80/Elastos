#ifndef __ELASTOS_DROID_TELECOM_CPARCELABLECONNECTION_H__
#define __ELASTOS_DROID_TELECOM_CPARCELABLECONNECTION_H__

#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Telecom_CParcelableConnection.h"
#include "elastos/droid/telecom/ParcelableConnection.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CParcelableConnection)
    , public ParcelableConnection
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CPARCELABLECONNECTION_H__
