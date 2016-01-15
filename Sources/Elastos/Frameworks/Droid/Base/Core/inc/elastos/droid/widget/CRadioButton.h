#ifndef __ELASTOS_DROID_WIDGET_CRADIOBUTTON_H__
#define __ELASTOS_DROID_WIDGET_CRADIOBUTTON_H__

#include "_Elastos_Droid_Widget_CRadioButton.h"
#include "elastos/droid/widget/RadioButton.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CRadioButton)
    , public RadioButton
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CRADIOBUTTON_H__
