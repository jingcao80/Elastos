
#ifndef __ELASTOS_DROID_SETTINGS_CCHOOSELOCKPASSWORDFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_CCHOOSELOCKPASSWORDFRAGMENT_H__

#include "_Elastos_Droid_Settings_CChooseLockPasswordFragment.h"
#include "elastos/droid/settings/CChooseLockPassword.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CChooseLockPasswordFragment)
    , public CChooseLockPassword::ChooseLockPasswordFragment
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCHOOSELOCKPASSWORDFRAGMENT_H__
