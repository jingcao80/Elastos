
#ifndef __ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPASSWORD_H__
#define __ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPASSWORD_H__

#include "_Elastos_Droid_Settings_CConfirmLockPassword.h"
#include "elastos/droid/settings/ConfirmLockPassword.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CConfirmLockPassword)
    , public ConfirmLockPassword
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPASSWORD_H__
