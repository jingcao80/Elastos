
#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CKEYBOARDLAYOUTPICKERFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CKEYBOARDLAYOUTPICKERFRAGMENT_H__

#include "_Elastos_Droid_Settings_Inputmethod_CKeyboardLayoutPickerFragment.h"
#include "elastos/droid/settings/inputmethod/KeyboardLayoutPickerFragment.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

CarClass(CKeyboardLayoutPickerFragment)
    , public KeyboardLayoutPickerFragment
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_CKEYBOARDLAYOUTPICKERFRAGMENT_H__
