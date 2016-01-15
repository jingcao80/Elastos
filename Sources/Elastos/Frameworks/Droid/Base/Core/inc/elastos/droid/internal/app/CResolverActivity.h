
#ifndef __ELASTOS_DROID_INTERNAL_APP_CRESOLVERACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_CRESOLVERACTIVITY_H__

#include "_Elastos_Droid_Internal_App_CResolverActivity.h"
#include "elastos/droid/internal/app/ResolverActivity.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CResolverActivity)
    , public ResolverActivity
{
public:
    CAR_OBJECT_DECL()
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CRESOLVERACTIVITY_H__
