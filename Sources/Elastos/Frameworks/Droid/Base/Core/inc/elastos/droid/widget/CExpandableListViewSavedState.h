#ifndef __ELASTOS_DROID_WIDGET_CEXPANDABLELISTVIEWSAVEDSTATE_H__
#define __ELASTOS_DROID_WIDGET_CEXPANDABLELISTVIEWSAVEDSTATE_H__

#include "_Elastos_Droid_Widget_CExpandableListViewSavedState.h"
#include "elastos/droid/widget/ExpandableListView.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CExpandableListViewSavedState)
    , public ExpandableListView::ExpandableListViewSavedState
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif // __ELASTOS_DROID_WIDGET_CEXPANDABLELISTVIEWSAVEDSTATE_H__
