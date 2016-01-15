
#ifndef __ELASTOS_DROID_APP_CNATIVE_ACTIVITY_H__
#define __ELASTOS_DROID_APP_CNATIVE_ACTIVITY_H__

#include "_Elastos_Droid_App_CNativeActivity.h"
#include "elastos/droid/app/NativeActivity.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CNativeActivity)
    , public NativeActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CNATIVE_ACTIVITY_H__