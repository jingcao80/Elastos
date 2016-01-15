
#include "elastos/droid/media/CAudioServiceHelper.h"
#include "elastos/droid/media/CAudioService.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAudioServiceHelper::CAudioServiceHelper()
{}

CAudioServiceHelper::~CAudioServiceHelper()
{}

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
