#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CPARCELABLECONFERENCE_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CPARCELABLECONFERENCE_H__

#include "_Elastos_Droid_Telecomm_Telecom_CParcelableConference.h"
#include "elastos/droid/telecomm/telecom/ParcelableConference.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CParcelableConference)
    , public ParcelableConference
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CPARCELABLECONFERENCE_H__