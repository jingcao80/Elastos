
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CCAMERACAPTURESESSIONIMPL_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CCAMERACAPTURESESSIONIMPL_H__

#include "_Elastos_Droid_Hardware_Camera2_Impl_CCameraCaptureSessionImpl.h"
#include "elastos/droid/hardware/camera2/impl/CameraCaptureSessionImpl.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Impl {

CarClass(CCameraCaptureSessionImpl)
    , public CameraCaptureSessionImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CCAMERACAPTURESESSIONIMPL_H__
