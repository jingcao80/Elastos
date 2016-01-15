
#ifndef __ELASTOS_DROID_APP_CVOICE_INTERACTOR_ABPRT_VOICE_REQUEST_H__
#define __ELASTOS_DROID_APP_CVOICE_INTERACTOR_ABPRT_VOICE_REQUEST_H__

#include "_Elastos_Droid_App_CVoiceInteractorAbortVoiceRequest.h"
#include "elastos/droid/app/VoiceInteractor.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CVoiceInteractorAbortVoiceRequest)
    , public VoiceInteractor::AbortVoiceRequest
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CVOICE_INTERACTOR_ABPRT_VOICE_REQUEST_H__