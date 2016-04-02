#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CCAMERACAPABILITIES_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CCAMERACAPABILITIES_H__

#include "_Elastos_Droid_Telecomm_Telecom_CCameraCapabilities.h"
#include "elastos/droid/telecomm/telecom/CameraCapabilities.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CCameraCapabilities)
    , public CameraCapabilities
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CCAMERACAPABILITIES_H__