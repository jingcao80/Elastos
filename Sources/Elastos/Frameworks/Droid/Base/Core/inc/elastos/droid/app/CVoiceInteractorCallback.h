
#ifndef __ELASTOS_DROID_APP_CVOICE_INTERACTOR_CALLBACK_H__
#define __ELASTOS_DROID_APP_CVOICE_INTERACTOR_CALLBACK_H__

#include "_Elastos_Droid_App_CVoiceInteractorCallback.h"
#include "elastos/droid/app/VoiceInteractor.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CVoiceInteractorCallback)
    , public VoiceInteractor::VoiceInteractorCallback
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CVOICE_INTERACTOR_CALLBACK_H__