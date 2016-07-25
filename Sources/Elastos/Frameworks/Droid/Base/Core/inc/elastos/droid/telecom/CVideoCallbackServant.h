#ifndef __ELASTOS_DROID_TELECOM_CVIDEOCALLBACKSERVANT_H__
#define __ELASTOS_DROID_TELECOM_CVIDEOCALLBACKSERVANT_H__

#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Telecom_CVideoCallbackServant.h"
#include "elastos/droid/telecom/VideoCallbackServant.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CVideoCallbackServant)
    , public VideoCallbackServant
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CVIDEOCALLBACKSERVANT_H__
