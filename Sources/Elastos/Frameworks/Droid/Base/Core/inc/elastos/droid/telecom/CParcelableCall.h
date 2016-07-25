#ifndef __ELASTOS_DROID_TELECOM_CPARCELABLECALL_H__
#define __ELASTOS_DROID_TELECOM_CPARCELABLECALL_H__

#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Telecom_CParcelableCall.h"
#include "elastos/droid/telecom/ParcelableCall.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CParcelableCall)
    , public ParcelableCall
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CPARCELABLECALL_H__
