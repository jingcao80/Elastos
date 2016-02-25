
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CTASKSINGLEDRAINER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CTASKSINGLEDRAINER_H__

#include "_Elastos_Droid_Hardware_Camera2_Utils_CTaskSingleDrainer.h"
#include "elastos/droid/hardware/camera2/utils/TaskSingleDrainer.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CarClass(CTaskSingleDrainer) , public TaskSingleDrainer
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CTASKSINGLEDRAINER_H__
