
#ifndef __ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPATTERNFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPATTERNFRAGMENT_H__

#include "_Elastos_Droid_Settings_CConfirmLockPatternFragment.h"
#include "elastos/droid/settings/ConfirmLockPattern.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CConfirmLockPatternFragment)
    , public ConfirmLockPattern::ConfirmLockPatternFragment
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPATTERNFRAGMENT_H__
