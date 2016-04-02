#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CPARCELABLECALL_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CPARCELABLECALL_H__

#include "_Elastos_Droid_Telecomm_Telecom_CParcelableCall.h"
#include "elastos/droid/telecomm/telecom/ParcelableCall.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CParcelableCall)
    , public ParcelableCall
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CPARCELABLECALL_H__