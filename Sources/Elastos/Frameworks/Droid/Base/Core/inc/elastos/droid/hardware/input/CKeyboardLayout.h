
#ifndef __ELASTOS_DROID_HARDWARE_INPUT_CKEYBOARD_LAYOUT_H__
#define __ELASTOS_DROID_HARDWARE_INPUT_CKEYBOARD_LAYOUT_H__

#include "_Elastos_Droid_Hardware_Input_CKeyboardLayout.h"
#include "elastos/droid/hardware/input/KeyboardLayout.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

CarClass(CKeyboardLayout)
    , public KeyboardLayout
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_INPUT_CKEYBOARD_LAYOUT_H__
