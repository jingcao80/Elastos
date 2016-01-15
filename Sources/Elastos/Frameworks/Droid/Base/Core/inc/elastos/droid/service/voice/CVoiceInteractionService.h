#ifndef __ELASTOS_DROID_SERVICE_VOICE_CVOICEINTERACTIONSERVICE_H__
#define __ELASTOS_DROID_SERVICE_VOICE_CVOICEINTERACTIONSERVICE_H__

#include "_Elastos_Droid_Service_Voice_CVoiceInteractionService.h"
#include "elastos/droid/service/voice/VoiceInteractionService.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

CarClass(CVoiceInteractionService)
    , public VoiceInteractionService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_VOICE_CVOICEINTERACTIONSERVICE_H__
