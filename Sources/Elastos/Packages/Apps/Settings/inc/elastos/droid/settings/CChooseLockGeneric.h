
#ifndef __ELASTOS_DROID_SETTINGS_CCHOOSELOCKGENERIC_H__
#define __ELASTOS_DROID_SETTINGS_CCHOOSELOCKGENERIC_H__

#include "_Elastos_Droid_Settings_CChooseLockGeneric.h"
#include "elastos/droid/settings/ChooseLockGeneric.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CChooseLockGeneric)
    , public ChooseLockGeneric
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCHOOSELOCKGENERIC_H__
