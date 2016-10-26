
#ifndef __ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPATTERNINTERNALACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPATTERNINTERNALACTIVITY_H__

#include "_Elastos_Droid_Settings_CConfirmLockPatternInternalActivity.h"
#include "elastos/droid/settings/ConfirmLockPattern.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CConfirmLockPatternInternalActivity)
    , public ConfirmLockPatternInternalActivity
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCONFIRMLOCKPATTERNINTERNALACTIVITY_H__
