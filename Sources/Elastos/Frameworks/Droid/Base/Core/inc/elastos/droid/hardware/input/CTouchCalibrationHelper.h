
#ifndef __ELASTOS_DROID_HARDWARE_INPUT_CTOUCHCALIBRATIONHELPER_H__
#define __ELASTOS_DROID_HARDWARE_INPUT_CTOUCHCALIBRATIONHELPER_H__

#include "_Elastos_Droid_Hardware_Input_CTouchCalibrationHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

CarClass(CTouchCalibrationHelper)
    , public Singleton
    , public ITouchCalibrationHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetIDENTITY(
        /* [out] */ ITouchCalibration** tc);
};

} // namespace Input
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_INPUT_CTOUCHCALIBRATIONHELPER_H__
