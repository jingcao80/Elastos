
#ifndef __ELASTOS_DROID_HARDWARE_LOCATION_CACTIVITYRECOGNITIONHARDWAREHELPER_H__
#define __ELASTOS_DROID_HARDWARE_LOCATION_CACTIVITYRECOGNITIONHARDWAREHELPER_H__

#include "_Elastos_Droid_Hardware_Location_CActivityRecognitionHardwareHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

CarClass(CActivityRecognitionHardwareHelper)
    , public Singleton
    , public IActivityRecognitionHardwareHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [out] */ IActivityRecognitionHardware** instance);

    CARAPI IsSupported(
        /* [out] */ Boolean* result);
};

} // namespace Location
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_LOCATION_CACTIVITYRECOGNITIONHARDWAREHELPER_H__
