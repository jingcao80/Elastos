#ifndef __ELASTOS_DROID_WIDGET_CTABLEROWLAYOUTPARAMS_H__
#define __ELASTOS_DROID_WIDGET_CTABLEROWLAYOUTPARAMS_H__

#include "_Elastos_Droid_Widget_CTableRowLayoutParams.h"
#include "elastos/droid/widget/TableRow.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CTableRowLayoutParams)
    , public TableRow::LayoutParams
{
public:
    CAR_OBJECT_DECL()
};


}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CTABLEROWLAYOUTPARAMS_H__
