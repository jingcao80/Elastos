
#ifndef __ELASTOS_DROID_WIDGET_CINT32HOLDER_H__
#define __ELASTOS_DROID_WIDGET_CINT32HOLDER_H__

#include "_Elastos_Droid_Widget_CInt32Holder.h"
#include "elastos/droid/widget/RadialTimePickerView.h"

using Elastos::Droid::Widget::RadialTimePickerView;

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CInt32Holder)
    , public RadialTimePickerView::Int32Holder
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CINT32HOLDER_H__
