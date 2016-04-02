
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/CAudioStateHelper.h"
#include "elastos/droid/telecomm/telecom/AudioState.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// CAudioStateHelper::
//===============================================================
CAR_SINGLETON_IMPL(CAudioStateHelper)

CAR_INTERFACE_IMPL(CAudioStateHelper, Singleton, IAudioStateHelper)

ECode CAudioStateHelper::AudioRouteToString(
    /* [in] */ Int32 route,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = AudioState::AudioRouteToString(route);
    return NOERROR;
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos