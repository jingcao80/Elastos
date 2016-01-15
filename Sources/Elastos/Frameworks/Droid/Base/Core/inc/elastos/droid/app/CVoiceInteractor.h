
#ifndef __ELASTOS_DROID_APP_CVOICEINTERACTOR_H__
#define __ELASTOS_DROID_APP_CVOICEINTERACTOR_H__

#include "_Elastos_Droid_App_CVoiceInteractor.h"
#include "elastos/droid/app/VoiceInteractor.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CVoiceInteractor)
    , public VoiceInteractor
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CVOICEINTERACTOR_H__