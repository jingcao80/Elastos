#ifndef __ELASTOS_DROID_TELECOM_CVIDEOPROFILE_H__
#define __ELASTOS_DROID_TELECOM_CVIDEOPROFILE_H__

#include "_Elastos_Droid_Telecom_CVideoProfile.h"
#include "elastos/droid/telecom/VideoProfile.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CVideoProfile)
    , public VideoProfile
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CVIDEOPROFILE_H__