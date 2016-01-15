
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/audiofx/CAutomaticGainControlHelper.h"
#include "elastos/droid/media/audiofx/CAutomaticGainControl.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAR_INTERFACE_IMPL(CAutomaticGainControlHelper, Singleton, IAutomaticGainControlHelper)

CAR_SINGLETON_IMPL(CAutomaticGainControlHelper)

/**
 * Checks if the device implements acoustic echo cancellation.
 * @return true if the device implements acoustic echo cancellation, false otherwise.
 */
ECode CAutomaticGainControlHelper::IsAvailable(
    /* [out] */ Boolean* result)
{
    return CAutomaticGainControl::IsAvailable(result);
}

ECode CAutomaticGainControlHelper::Create(
    /* [in] */ Int32 audioSession,
    /* [out] */ IAutomaticGainControl** result)
{
    return CAutomaticGainControl::New(audioSession, result);
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
