
#ifndef __ELASTOS_DROID_HARDWARE_INPUT_CINPUTMANAGERINPUTDEVICESCHANGEDLISTENER_H__
#define __ELASTOS_DROID_HARDWARE_INPUT_CINPUTMANAGERINPUTDEVICESCHANGEDLISTENER_H__

#include "_Elastos_Droid_Hardware_Input_CInputManagerInputDevicesChangedListener.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

class CInputManager;

CarClass(CInputManagerInputDevicesChangedListener)
    , public Object
    , public IInputDevicesChangedListener
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
            /* [in] */ Handle32 owner);

    CARAPI OnInputDevicesChanged(
        /* [in] */ ArrayOf<Int32>* deviceIdAndGeneration);

private:
    CInputManager* mOwner;
};

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_INPUT_CINPUTMANAGERINPUTDEVICESCHANGEDLISTENER_H__
