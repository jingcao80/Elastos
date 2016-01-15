
#ifndef __ELASTOS_DROID_APP_CVOICE_INTERACTOR_CONFIRMATION_REQUEST_H__
#define __ELASTOS_DROID_APP_CVOICE_INTERACTOR_CONFIRMATION_REQUEST_H__

#include "_Elastos_Droid_App_CVoiceInteractorConfirmationRequest.h"
#include "elastos/droid/app/VoiceInteractor.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CVoiceInteractorConfirmationRequest)
    , public VoiceInteractor::ConfirmationRequest
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CVOICE_INTERACTOR_CONFIRMATION_REQUEST_H__