
#ifndef __ELASTOS_DROID_SETTINGS_CCRYPTKEEPERCONFIRMBLANK_H__
#define __ELASTOS_DROID_SETTINGS_CCRYPTKEEPERCONFIRMBLANK_H__

#include "_Elastos_Droid_Settings_CCryptKeeperConfirmBlank.h"
#include "elastos/droid/settings/CCryptKeeperConfirm.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CCryptKeeperConfirmBlank)
    , public CCryptKeeperConfirm::Blank
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCRYPTKEEPERCONFIRMBLANK_H__
