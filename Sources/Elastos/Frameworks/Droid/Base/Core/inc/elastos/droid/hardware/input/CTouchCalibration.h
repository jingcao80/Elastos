
#ifndef __ELASTOS_DROID_HARDWARE_INPUT_CTOUCHCALIBRATION_H__
#define __ELASTOS_DROID_HARDWARE_INPUT_CTOUCHCALIBRATION_H__

#include "_Elastos_Droid_Hardware_Input_CTouchCalibration.h"
#include "elastos/droid/hardware/input/TouchCalibration.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

CarClass(CTouchCalibration)
    , public TouchCalibration
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Input
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_INPUT_CTOUCHCALIBRATION_H__
