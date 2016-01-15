
#include "elastos/droid/view/CSoundEffectConstantsHelper.h"
#include "elastos/droid/view/SoundEffectConstants.h"

using Elastos::Droid::View::SoundEffectConstants;

namespace Elastos {
namespace Droid {
namespace View {

CAR_SINGLETON_IMPL(CSoundEffectConstantsHelper)

ECode CSoundEffectConstantsHelper::GetContantForFocusDirection(
    /* [in] */ Int32 direction,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SoundEffectConstants::GetContantForFocusDirection(direction);
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos

