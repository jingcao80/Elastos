#ifndef __ELASTOS_DROID_SERVICE_VOICE_CVOICEINTERACTIONSESSIONREQUEST_H__
#define __ELASTOS_DROID_SERVICE_VOICE_CVOICEINTERACTIONSESSIONREQUEST_H__

#include "_Elastos_Droid_Service_Voice_CVoiceInteractionSessionRequest.h"
#include "elastos/droid/service/voice/VoiceInteractionSession.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

CarClass(CVoiceInteractionSessionRequest)
    , public VoiceInteractionSession::Request
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_VOICE_CVOICEINTERACTIONSESSIONREQUEST_H__
