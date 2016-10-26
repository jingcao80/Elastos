
#ifndef __ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPATTERN_H__
#define __ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPATTERN_H__

#include "_Elastos_Droid_Settings_CConfirmLockPattern.h"
#include "elastos/droid/settings/ConfirmLockPattern.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CConfirmLockPattern)
    , public ConfirmLockPattern
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPATTERN_H__
