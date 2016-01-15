#ifndef __ELASTOS_DROID_MEDIA_CAUDIODEVICE_H__
#define __ELASTOS_DROID_MEDIA_CAUDIODEVICE_H__

#include "_Elastos_Droid_Media_CAudioDevice.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::ISparseInt32Array;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Class to provide information about the audio devices.
 * @hide
 */
CarClass(CAudioDevice)
    , public Object
    , public IAudioDevice
{
public:
    CAudioDevice();

    virtual ~CAudioDevice();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IAudioDevicePortConfig* config);

    /**
     * @hide
     * CANDIDATE FOR PUBLIC API
     * @return
     */
    CARAPI IsInputDevice(
        /* [out] */ Boolean* result);

    /**
     * @hide
     * CANDIDATE FOR PUBLIC API
     * @return
     */
    CARAPI IsOutputDevice(
        /* [out] */ Boolean* result);

    /**
     * @hide
     * CANDIDATE FOR PUBLIC API
     * @return
     */
    CARAPI GetDeviceType(
        /* [out] */ Int32* result);

    /**
     * @hide
     * CANDIDATE FOR PUBLIC API
     * @return
     */
    CARAPI GetAddress(
        /* [out] */ String* result);

    /** @hide */
    static CARAPI ConvertDeviceTypeToInternalDevice(
        /* [in] */ Int32 deviceType,
        /* [out] */ Int32* result);

    /** @hide */
    static CARAPI ConvertInternalDeviceToDeviceType(
        /* [in] */ Int32 intDevice,
        /* [out] */ Int32* result);

private:
    static AutoPtr<ISparseInt32Array> INT_TO_EXT_DEVICE_MAPPING;

    static AutoPtr<ISparseInt32Array> EXT_TO_INT_DEVICE_MAPPING;

    AutoPtr<IAudioDevicePortConfig> mConfig;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CAUDIODEVICE_H__
