#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CPASSWORDENTRYKEYBOARDVIEW_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CPASSWORDENTRYKEYBOARDVIEW_H__

#include "_Elastos_Droid_Internal_Widget_CPasswordEntryKeyboardView.h"
#include "elastos/droid/internal/widget/PasswordEntryKeyboardView.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CPasswordEntryKeyboardView)
    , public PasswordEntryKeyboardView
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CPASSWORDENTRYKEYBOARDVIEW_H__
