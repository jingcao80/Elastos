#ifndef __ELASTOS_DROID_WIDGET_CDATETIMEVIEW_H__
#define __ELASTOS_DROID_WIDGET_CDATETIMEVIEW_H__

#include "_Elastos_Droid_Widget_CDateTimeView.h"
#include "elastos/droid/widget/DateTimeView.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CDateTimeView)
    , public DateTimeView
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CDATETIMEVIEW_H__
