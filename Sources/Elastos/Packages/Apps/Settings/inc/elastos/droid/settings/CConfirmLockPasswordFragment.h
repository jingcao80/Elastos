
#ifndef __ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPASSWORDFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPASSWORDFRAGMENT_H__

#include "_Elastos_Droid_Settings_CConfirmLockPasswordFragment.h"
#include "elastos/droid/settings/ConfirmLockPassword.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CConfirmLockPasswordFragment)
    , public ConfirmLockPassword::ConfirmLockPasswordFragment
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPASSWORDFRAGMENT_H__
