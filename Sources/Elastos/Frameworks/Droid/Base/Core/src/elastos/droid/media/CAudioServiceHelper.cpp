
#include "elastos/droid/media/CAudioServiceHelper.h"
#include "elastos/droid/media/CAudioService.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioServiceHelper, Singleton, IAudioServiceHelper)

CAR_SINGLETON_IMPL(CAudioServiceHelper)

ECode CAudioServiceHelper::StreamToString(
    /* [in] */ Int32 stream,
    /* [out] */ String* result)
{
    return CAudioService::StreamToString(stream, result);
}

ECode CAudioServiceHelper::GetValueForVibrateSetting(
    /* [in] */ Int32 existingValue,
    /* [in] */ Int32 vibrateType,
    /* [in] */ Int32 vibrateSetting,
    /* [out] */ Int32* result)
{
    return CAudioService::GetValueForVibrateSetting(existingValue, vibrateType, vibrateSetting, result);
}

} //namespace Media
} //namespace Droid
} //namespace Elastos
