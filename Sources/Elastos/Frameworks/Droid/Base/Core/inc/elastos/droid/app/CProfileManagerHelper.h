#ifndef __ELASTOS_DROID_APP_CPROFILE_MANAGER_HELPER_H__
#define __ELASTOS_DROID_APP_CPROFILE_MANAGER_HELPER_H__

#include "_Elastos_Droid_App_CProfileManagerHelper.h"
#include "elastos/droid/app/ProfileManager.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Singleton.h>

using Elastos::Droid::App::IIProfileManager;
using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CProfileManagerHelper)
    , public Singleton
    , public IProfileManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetNoProfile(
        /* [out] */ IUUID** noprofile);

    CARAPI GetService(
        /* [out] */ IIProfileManager** service);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CPROFILE_MANAGER_HELPER_H__
