#ifndef __ELASTOS_DROID_APP_CPROFILE_H__
#define __ELASTOS_DROID_APP_CPROFILE_H__

#include "_Elastos_Droid_App_CProfile.h"
#include "elastos/droid/app/Profile.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CProfile)
    , public Profile
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CPROFILE_H__
