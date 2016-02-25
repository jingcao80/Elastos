
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CCAMERADEVICEUSERSHIMHELPER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CCAMERADEVICEUSERSHIMHELPER_H__

#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_Hardware_Camera2_Legacy_CCameraDeviceUserShimHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CarClass(CCameraDeviceUserShimHelper)
    , public Singleton
    , public ICameraDeviceUserShimHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ConnectBinderShim(
        /* [in] */ IICameraDeviceCallbacks* callbacks,
        /* [in] */ Int32 cameraId,
        /* [out] */ ICameraDeviceUserShim** shim);
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CCAMERADEVICEUSERSHIMHELPER_H__
