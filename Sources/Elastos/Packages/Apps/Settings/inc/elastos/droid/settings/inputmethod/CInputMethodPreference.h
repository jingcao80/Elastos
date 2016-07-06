
#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CINPUTMETHODPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CINPUTMETHODPREFERENCE_H__

#include "_Elastos_Droid_Settings_Inputmethod_CInputMethodPreference.h"
#include "elastos/droid/settings/inputmethod/InputMethodPreference.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

CarClass(CInputMethodPreference)
    , public InputMethodPreference
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_CINPUTMETHODPREFERENCE_H__
