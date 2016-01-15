#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CPASSWORDENTRYKEYBOARD_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CPASSWORDENTRYKEYBOARD_H__

#include "_Elastos_Droid_Internal_Widget_CPasswordEntryKeyboard.h"
#include "elastos/droid/internal/widget/PasswordEntryKeyboard.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CPasswordEntryKeyboard)
    , public PasswordEntryKeyboard
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CPASSWORDENTRYKEYBOARD_H__
