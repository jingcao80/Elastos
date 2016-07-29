
#ifndef __ELASTOS_DROID_SETTINGS_CCHOOSELOCKGENERICFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_CCHOOSELOCKGENERICFRAGMENT_H__

#include "_Elastos_Droid_Settings_CChooseLockGenericFragment.h"
#include "elastos/droid/settings/ChooseLockGeneric.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CChooseLockGenericFragment)
    , public ChooseLockGeneric::ChooseLockGenericFragment
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCHOOSELOCKGENERICFRAGMENT_H__
