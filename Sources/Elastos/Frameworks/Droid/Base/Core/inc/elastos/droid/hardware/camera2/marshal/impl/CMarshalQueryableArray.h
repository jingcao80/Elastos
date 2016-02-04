
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_CMARSHALQUERYABLEARRAY_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_CMARSHALQUERYABLEARRAY_H__

#include "_Elastos_Droid_Hardware_Camera2_Marshal_Impl_CMarshalQueryableArray.h"
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableArray.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

CarClass(CMarshalQueryableArray)
    , public MarshalQueryableArray
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

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_CMARSHALQUERYABLEARRAY_H__
