#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CVIDEOCALLIMPL_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CVIDEOCALLIMPL_H__

#include "_Elastos_Droid_Telecomm_Telecom_CVideoCallImpl.h"
#include "elastos/droid/telecomm/telecom/VideoCallImpl.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CVideoCallImpl)
    , public VideoCallImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CVIDEOCALLIMPL_H__