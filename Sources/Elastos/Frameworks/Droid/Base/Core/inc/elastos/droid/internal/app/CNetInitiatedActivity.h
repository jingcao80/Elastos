
#ifndef __ELASTOS_DROID_INTERNAL_APP_CNETINITIATEDACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_CNETINITIATEDACTIVITY_H__

#include "_Elastos_Droid_Internal_App_CNetInitiatedActivity.h"
#include "elastos/droid/internal/app/NetInitiatedActivity.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CNetInitiatedActivity)
    , public NetInitiatedActivity
{
public:
    CAR_OBJECT_DECL()
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CNETINITIATEDACTIVITY_H__
