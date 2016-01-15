#ifndef __ELASTOS_DROID_SERVICE_VOICE_CVOICEINTERACTIONSERVICEINFO_H__
#define __ELASTOS_DROID_SERVICE_VOICE_CVOICEINTERACTIONSERVICEINFO_H__

#include "_Elastos_Droid_Service_Voice_CVoiceInteractionServiceInfo.h"
#include "elastos/droid/service/voice/VoiceInteractionServiceInfo.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

CarClass(CVoiceInteractionServiceInfo)
    , public VoiceInteractionServiceInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_VOICE_CVOICEINTERACTIONSERVICEINFO_H__
