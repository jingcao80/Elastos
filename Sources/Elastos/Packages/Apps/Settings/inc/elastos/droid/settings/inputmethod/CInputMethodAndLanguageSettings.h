
#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CINPUTMETHODANDLANGUAGESETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CINPUTMETHODANDLANGUAGESETTINGS_H__

#include "_Elastos_Droid_Settings_Inputmethod_CInputMethodAndLanguageSettings.h"
#include "elastos/droid/settings/inputmethod/InputMethodAndLanguageSettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

CarClass(CInputMethodAndLanguageSettings)
    , public InputMethodAndLanguageSettings
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_CINPUTMETHODANDLANGUAGESETTINGS_H__
