
#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CSPELLCHECKERSSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CSPELLCHECKERSSETTINGS_H__

#include "_Elastos_Droid_Settings_Inputmethod_CSpellCheckersSettings.h"
#include "elastos/droid/settings/inputmethod/SpellCheckersSettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

CarClass(CSpellCheckersSettings)
    , public SpellCheckersSettings
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_CSPELLCHECKERSSETTINGS_H__
