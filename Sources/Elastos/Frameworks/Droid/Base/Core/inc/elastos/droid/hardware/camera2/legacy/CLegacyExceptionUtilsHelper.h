
#ifndef  __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CLEGACYEXCEPTIONUTILES_H__
#define  __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CLEGACYEXCEPTIONUTILES_H__

#include "_Elastos_Droid_Hardware_Camera2_Legacy_CLegacyExceptionUtilsHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CarClass(CLegacyExceptionUtilsHelper)
    , public Singleton
    , public ILegacyExceptionUtilsHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ThrowOnError(
        /* [in] */ Int32 errorFlag,
        /* [out] */ Int32* result);
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CLEGACYEXCEPTIONUTILES_H__
