
#ifndef __ELASTOS_DROID_HARDWARE_INPUT_CINPUT_DEVICE_IDENTIFIER_H__
#define __ELASTOS_DROID_HARDWARE_INPUT_CINPUT_DEVICE_IDENTIFIER_H__

#include "_Elastos_Droid_Hardware_Input_CInputDeviceIdentifier.h"
#include "elastos/droid/hardware/input/InputDeviceIdentifier.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

CarClass(CInputDeviceIdentifier)
    , public InputDeviceIdentifier
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_INPUT_CINPUT_DEVICE_IDENTIFIER_H__
