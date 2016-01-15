
#ifndef __ELASTOS_DROID_WIDGET_CADAPTERVIEW_H__
#define __ELASTOS_DROID_WIDGET_CADAPTERVIEW_H__

#include "_Elastos_Droid_Widget_CAdapterView.h"
#include "elastos/droid/widget/AdapterView.h"

using Elastos::Droid::Widget::AdapterView;

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CAdapterView)
    , public AdapterView
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CADAPTERVIEW_H__

