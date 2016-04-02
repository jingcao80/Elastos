#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CAUDIOSTATE_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CAUDIOSTATE_H__

#include "_Elastos_Droid_Telecomm_Telecom_CAudioState.h"
#include "elastos/droid/telecomm/telecom/AudioState.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CAudioState)
    , public AudioState
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CAUDIOSTATE_H__