#ifndef __ELASTOS_DROID_WIDGET_CDATEPICKERSAVEDSTATE_H__
#define __ELASTOS_DROID_WIDGET_CDATEPICKERSAVEDSTATE_H__

#include "_Elastos_Droid_Widget_CDatePickerSavedState.h"
#include "elastos/droid/widget/DatePicker.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CDatePickerSavedState)
    , public DatePicker::DatePickerSavedState
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif // __ELASTOS_DROID_WIDGET_CDATEPICKERSAVEDSTATE_H__
