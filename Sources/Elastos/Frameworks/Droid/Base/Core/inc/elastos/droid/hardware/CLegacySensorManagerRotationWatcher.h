#ifndef __ELASTOS_DROID_HARDWARE_CLEGACYSENSORMANAGERROTATIONWATCHER_H__
#define __ELASTOS_DROID_HARDWARE_CLEGACYSENSORMANAGERROTATIONWATCHER_H__

#include "_Elastos_Droid_Hardware_CLegacySensorManagerRotationWatcher.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IRotationWatcher;
using Elastos::Droid::View::EIID_IRotationWatcher;

namespace Elastos {
namespace Droid {
namespace Hardware {

CarClass(CLegacySensorManagerRotationWatcher)
    , public Object
    , public IRotationWatcher
{
public:
    CLegacySensorManagerRotationWatcher();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI OnRotationChanged(
        /* [in] */ Int32 rotation);
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CLEGACYSENSORMANAGERROTATIONWATCHER_H__
