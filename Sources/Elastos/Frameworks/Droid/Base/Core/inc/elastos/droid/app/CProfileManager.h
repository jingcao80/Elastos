#ifndef __ELASTOS_DROID_APP_CPROFILE_MANAGER_H__
#define __ELASTOS_DROID_APP_CPROFILE_MANAGER_H__

#include "_Elastos_Droid_App_CProfileManager.h"
#include "elastos/droid/app/ProfileManager.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CProfileManager)
    , public ProfileManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CPROFILE_MANAGER_H__
