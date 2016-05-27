
#ifndef __ELASTOS_DROID_MEDIA_CAUDIOMANAGERHELPER_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOMANAGERHELPER_H__

#include "_Elastos_Droid_Media_CAudioManagerHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CAudioManagerHelper)
    , public Singleton
    , public IAudioManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Checks valid ringer mode values.
     *
     * @return true if the ringer mode indicated is valid, false otherwise.
     *
     * @see #setRingerMode(int)
     * @hide
     */
    CARAPI IsValidRingerMode(
        /* [in] */ Int32 rightMode,
        /* [out] */ Boolean* result);

    /**
     * Return true if the device code corresponds to an output device.
     * @hide
     */
    CARAPI IsOutputDevice(
        /* [in] */ Int32 device,
        /* [out] */ Boolean* result);

    /**
     * Return true if the device code corresponds to an input device.
     * @hide
     */
    CARAPI IsInputDevice(
        /* [in] */ Int32 device,
        /* [out] */ Boolean* result);
};

} // namespace Media
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIOMANAGERHELPER_H__
