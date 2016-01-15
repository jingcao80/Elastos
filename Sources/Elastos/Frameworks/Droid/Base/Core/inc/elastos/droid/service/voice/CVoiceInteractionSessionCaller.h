#ifndef __ELASTOS_DROID_SERVICE_VOICE_CVOICEINTERACTIONSESSIONCALLER_H__
#define __ELASTOS_DROID_SERVICE_VOICE_CVOICEINTERACTIONSESSIONCALLER_H__

#include "_Elastos_Droid_Service_Voice_CVoiceInteractionSessionCaller.h"
#include "elastos/droid/service/voice/VoiceInteractionSession.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

CarClass(CVoiceInteractionSessionCaller)
    , public VoiceInteractionSession::Caller
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_VOICE_CVOICEINTERACTIONSESSIONCALLER_H__
