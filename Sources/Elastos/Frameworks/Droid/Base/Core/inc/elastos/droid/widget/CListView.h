#ifndef __ELASTOS_DROID_WIDGET_CLISTVIEW_H__
#define __ELASTOS_DROID_WIDGET_CLISTVIEW_H__

#include "_Elastos_Droid_Widget_CListView.h"
#include "elastos/droid/widget/ListView.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CListView)
    , public ListView
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CLISTVIEW_H__
