
#include "elastos/droid/service/voice/CVoiceInteractionServiceHelper.h"
#include "elastos/droid/service/voice/VoiceInteractionService.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

CAR_INTERFACE_IMPL(CVoiceInteractionServiceHelper, Singleton, IVoiceInteractionServiceHelper)

CAR_SINGLETON_IMPL(CVoiceInteractionServiceHelper)

ECode CVoiceInteractionServiceHelper::IsActiveService(
    /* [in] */ IContext* context,
    /* [in] */ IComponentName* service,
    /* [out] */ Boolean* isActiveService)
{
    return VoiceInteractionService::IsActiveService(context, service, isActiveService);
}

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos

