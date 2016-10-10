
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_CAMERADEVICE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_CAMERADEVICE_H__

#include "Elastos.Droid.Hardware.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

class ECO_PUBLIC CameraDevice
    : public Object
    , public ICameraDevice
    , public ICloseable
{
public:
    class StateCallback
        : public Object
        , public ICameraDeviceStateCallback
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI OnClosed(
            /* [in] */ ICameraDevice* camera);
    };

    class StateListener
        : public StateCallback
        , public ICameraDeviceStateListener
    {
    public:
        CAR_INTERFACE_DECL()
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * To be inherited by android.hardware.camera2.* code only.
     * @hide
     */
    CameraDevice() {}

    virtual ~CameraDevice() {}
};

} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_CAMERADEVICE_H__
