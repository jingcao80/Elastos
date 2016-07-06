
#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CKEYBOARDLAYOUTDIALOGFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CKEYBOARDLAYOUTDIALOGFRAGMENT_H__

#include "_Elastos_Droid_Settings_Inputmethod_CKeyboardLayoutDialogFragment.h"
#include "elastos/droid/settings/inputmethod/KeyboardLayoutDialogFragment.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

CarClass(CKeyboardLayoutDialogFragment)
    , public KeyboardLayoutDialogFragment
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_CKEYBOARDLAYOUTDIALOGFRAGMENT_H__
