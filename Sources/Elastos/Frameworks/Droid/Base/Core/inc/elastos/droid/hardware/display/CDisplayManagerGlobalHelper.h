
#ifndef __ELASTOS_DROID_HARDWARE_DISPLAY_CDISPLAYMANAGERGLOBALHELPER_H__
#define __ELASTOS_DROID_HARDWARE_DISPLAY_CDISPLAYMANAGERGLOBALHELPER_H__

#include "_Elastos_Droid_Hardware_Display_CDisplayManagerGlobalHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

CarClass(CDisplayManagerGlobalHelper)
    , public Singleton
    , public IDisplayManagerGlobalHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ IDisplayManagerGlobal** global);
};

} // namespace Display
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_DISPLAY_CDISPLAYMANAGERGLOBALHELPER_H__
