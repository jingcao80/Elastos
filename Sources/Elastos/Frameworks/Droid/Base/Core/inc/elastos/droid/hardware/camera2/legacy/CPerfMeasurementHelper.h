
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CPERFMEASUREMENTHELPER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CPERFMEASUREMENTHELPER_H__

#include "_Elastos_Droid_Hardware_Camera2_Legacy_CPerfMeasurementHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CarClass(CPerfMeasurementHelper)
    , public Singleton
    , public IPerfMeasurementHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns true if the Gl timing methods will work, false otherwise.
     *
     * <p>Must be called within a valid GL context.</p>
     */
    CARAPI IsGlTimingSupported(
        /* [out] */ Boolean* result);
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CPERFMEASUREMENTHELPER_H__
