#ifndef __ELASTOS_DROID_WIDGET_CDATEPICKER_H__
#define __ELASTOS_DROID_WIDGET_CDATEPICKER_H__

#include "_Elastos_Droid_Widget_CDatePicker.h"
#include "elastos/droid/widget/DatePicker.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CDatePicker)
    , public DatePicker
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CDATEPICKER_H__
