
#ifndef __ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPASSWORDINTERNALACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPASSWORDINTERNALACTIVITY_H__

#include "_Elastos_Droid_Settings_CConfirmLockPasswordInternalActivity.h"
#include "elastos/droid/settings/ConfirmLockPassword.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CConfirmLockPasswordInternalActivity)
    , public ConfirmLockPasswordInternalActivity
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPASSWORDINTERNALACTIVITY_H__
