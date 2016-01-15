
#ifndef __ELASTOS_DROID_APP_CACTIVITY_GROUP_H__
#define __ELASTOS_DROID_APP_CACTIVITY_GROUP_H__

#include "_Elastos_Droid_App_CActivityGroup.h"
#include "elastos/droid/app/ActivityGroup.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CActivityGroup)
    , public ActivityGroup
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITY_GROUP_H__