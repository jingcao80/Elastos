
#ifndef __ELASTOS_DROID_INTERNAL_APP_CRESTRICTIONSPINACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_CRESTRICTIONSPINACTIVITY_H__

#include "_Elastos_Droid_Internal_App_CRestrictionsPinActivity.h"
#include "elastos/droid/internal/app/RestrictionsPinActivity.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CRestrictionsPinActivity)
    , public RestrictionsPinActivity
{
public:
    CAR_OBJECT_DECL()
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CRESTRICTIONSPINACTIVITY_H__
