#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CABSLISTVIEWAUTOSCROLLER_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CABSLISTVIEWAUTOSCROLLER_H__

#include "_Elastos_Droid_Internal_Widget_CAbsListViewAutoScroller.h"
#include "elastos/droid/internal/widget/AutoScrollHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CAbsListViewAutoScroller)
    , public AbsListViewAutoScroller
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_WIDGET_CABSLISTVIEWAUTOSCROLLER_H__
