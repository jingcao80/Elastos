#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CPARCELABLECONNECTION_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CPARCELABLECONNECTION_H__

#include "_Elastos_Droid_Telecomm_Telecom_CParcelableConnection.h"
#include "elastos/droid/telecomm/telecom/ParcelableConnection.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CParcelableConnection)
    , public ParcelableConnection
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CPARCELABLECONNECTION_H__