#ifndef __ELASTOS_DROID_TELECOM_CVIDEOCALLIMPL_H__
#define __ELASTOS_DROID_TELECOM_CVIDEOCALLIMPL_H__

#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Telecom_CVideoCallImpl.h"
#include "elastos/droid/telecom/VideoCallImpl.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CVideoCallImpl)
    , public VideoCallImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CVIDEOCALLIMPL_H__
