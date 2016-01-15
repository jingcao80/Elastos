#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CEIDTABLEINPUTCONNECTION_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CEIDTABLEINPUTCONNECTION_H__

#include "_Elastos_Droid_Internal_Widget_CEditableInputConnection.h"
#include "elastos/droid/internal/widget/EditableInputConnection.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CEditableInputConnection)
    , public EditableInputConnection
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_WIDGET_CEIDTABLEINPUTCONNECTION_H__
