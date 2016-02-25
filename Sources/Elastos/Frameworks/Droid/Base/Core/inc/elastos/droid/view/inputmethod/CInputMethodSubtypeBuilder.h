
#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODSUBTYPE_BUILDER_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODSUBTYPE_BUILDER_H__

#include "_Elastos_Droid_View_InputMethod_CInputMethodSubtypeBuilder.h"
#include "elastos/droid/view/inputmethod/CInputMethodSubtype.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

/**
 * InputMethodSubtypeBuilder is a builder class of InputMethodSubtype.
 * This class is designed to be used with
 * {@link android.view.inputmethod.InputMethodManager#setAdditionalInputMethodSubtypes}.
 * The developer needs to be aware of what each parameter means.
 */
CarClass(CInputMethodSubtypeBuilder)
    , public CInputMethodSubtype::InputMethodSubtypeBuilder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODSUBTYPE_BUILDER_H__
