#ifndef __ELASTOS_DROID_INTERNAL_INPUTMETHOD_CINPUTMETHODSETTINGS_H__
#define __ELASTOS_DROID_INTERNAL_INPUTMETHOD_CINPUTMETHODSETTINGS_H__

#include "_Elastos_Droid_Internal_InputMethod_CInputMethodSettings.h"
#include "elastos/droid/internal/inputmethod/InputMethodUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace InputMethod {

CarClass(CInputMethodSettings)
    , public InputMethodUtils::InputMethodSettings
{
public:
    CAR_OBJECT_DECL()
};

} // namespace InputMethod
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_INPUTMETHOD_CINPUTMETHODSETTINGS_H__
