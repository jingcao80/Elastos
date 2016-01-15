
#ifndef __ELASTOS_DROID_INTERNAL_APP_CINTENTFORWARDERACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_CINTENTFORWARDERACTIVITY_H__

#include "_Elastos_Droid_Internal_App_CIntentForwarderActivity.h"
#include "elastos/droid/internal/app/IntentForwarderActivity.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CIntentForwarderActivity)
    , public IntentForwarderActivity
{
public:
    CAR_OBJECT_DECL()
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CINTENTFORWARDERACTIVITY_H__
