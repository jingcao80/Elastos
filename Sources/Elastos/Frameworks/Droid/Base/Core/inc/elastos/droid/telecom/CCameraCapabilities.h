#ifndef __ELASTOS_DROID_TELECOM_CCAMERACAPABILITIES_H__
#define __ELASTOS_DROID_TELECOM_CCAMERACAPABILITIES_H__

#include "_Elastos_Droid_Telecom_CCameraCapabilities.h"
#include "elastos/droid/telecom/CameraCapabilities.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CCameraCapabilities)
    , public CameraCapabilities
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CCAMERACAPABILITIES_H__