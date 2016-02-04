
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_CMARSHALQUERYABLEENUM_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_CMARSHALQUERYABLEENUM_H__

#include "_Elastos_Droid_Hardware_Camera2_Marshal_Impl_CMarshalQueryableEnum.h"
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableEnum.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

CarClass(CMarshalQueryableEnum)
    , public MarshalQueryableEnum
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_CMARSHALQUERYABLEENUM_H__
