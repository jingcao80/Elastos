
#ifndef __ELASTOS_DROID_APP_CACTIVITY_H__
#define __ELASTOS_DROID_APP_CACTIVITY_H__

#include "_Elastos_Droid_App_CActivity.h"
#include "elastos/droid/app/Activity.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CActivity)
    , public Activity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITY_H__