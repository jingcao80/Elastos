#ifndef __ELASTOS_DROID_WIDGET_CSCROLLVIEWSAVEDSTATE_H__
#define __ELASTOS_DROID_WIDGET_CSCROLLVIEWSAVEDSTATE_H__

#include "_Elastos_Droid_Widget_CScrollViewSavedState.h"
#include "elastos/droid/widget/ScrollView.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CScrollViewSavedState)
    , public ScrollView::ScrollViewSavedState
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CSCROLLVIEWSAVEDSTATE_H__
