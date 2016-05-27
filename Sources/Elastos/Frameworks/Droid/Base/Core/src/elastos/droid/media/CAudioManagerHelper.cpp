
#include "elastos/droid/media/CAudioManagerHelper.h"
#include "elastos/droid/media/CAudioManager.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioManagerHelper, Singleton, IAudioManagerHelper)

CAR_SINGLETON_IMPL(CAudioManagerHelper)

ECode CAudioManagerHelper::IsValidRingerMode(
    /* [in] */ Int32 ringerMode,
    /* [out] */ Boolean* result)
{
    return CAudioManager::IsValidRingerMode(ringerMode, result);
}

ECode CAudioManagerHelper::IsOutputDevice(
    /* [in] */ Int32 device,
    /* [out] */ Boolean* result)
{
    return CAudioManager::IsOutputDevice(device, result);
}

/**
 * Return true if the device code corresponds to an input device.
 * @hide
 */
ECode CAudioManagerHelper::IsInputDevice(
    /* [in] */ Int32 device,
    /* [out] */ Boolean* result)
{
    return CAudioManager::IsInputDevice(device, result);
}

} // namespace Media
} // namespace Droid
} // namespace Elastos
