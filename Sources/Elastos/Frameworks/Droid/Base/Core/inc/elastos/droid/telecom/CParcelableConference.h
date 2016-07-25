#ifndef __ELASTOS_DROID_TELECOM_CPARCELABLECONFERENCE_H__
#define __ELASTOS_DROID_TELECOM_CPARCELABLECONFERENCE_H__

#include "_Elastos_Droid_Telecom_CParcelableConference.h"
#include "elastos/droid/telecom/ParcelableConference.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CParcelableConference)
    , public ParcelableConference
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CPARCELABLECONFERENCE_H__