
#ifndef __ELASTOS_DROID_SETTINGS_CCHOOSELOCKPATTERNFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_CCHOOSELOCKPATTERNFRAGMENT_H__

#include "_Elastos_Droid_Settings_CChooseLockPatternFragment.h"
#include "elastos/droid/settings/CChooseLockPattern.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CChooseLockPatternFragment)
    , public CChooseLockPattern::ChooseLockPatternFragment
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCHOOSELOCKPATTERNFRAGMENT_H__
