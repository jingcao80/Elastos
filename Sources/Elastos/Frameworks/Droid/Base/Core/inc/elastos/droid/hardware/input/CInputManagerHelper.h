
#ifndef __ELASTOS_DROID_HARDWARE_INPUT_CINPUTMANAGERHELPER_H__
#define __ELASTOS_DROID_HARDWARE_INPUT_CINPUTMANAGERHELPER_H__

#include "_Elastos_Droid_Hardware_Input_CInputManagerHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

CarClass(CInputManagerHelper)
    , public Singleton
    , public IInputManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()
    /**
     * Gets an instance of the input manager.
     *
     * @return The input manager instance.
     *
     * @hide
     */
    GetInstance(
        /* [out] */ IInputManager** manager);
};

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_INPUT_CINPUTMANAGERHELPER_H__
