#ifndef __ELASTOS_DROID_TELECOM_CAUDIOSTATE_H__
#define __ELASTOS_DROID_TELECOM_CAUDIOSTATE_H__

#include "_Elastos_Droid_Telecom_CAudioState.h"
#include "elastos/droid/telecom/AudioState.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CAudioState)
    , public AudioState
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CAUDIOSTATE_H__