
#ifndef  __ELASTOS_DROID_WIDGET_INTERNAL_CSCROLLINGTABCONTAINERTABVIEW_H__
#define  __ELASTOS_DROID_WIDGET_INTERNAL_CSCROLLINGTABCONTAINERTABVIEW_H__

#include "_Elastos_Droid_Internal_Widget_CScrollingTabContainerTabView.h"
#include "elastos/droid/internal/widget/ScrollingTabContainerView.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CScrollingTabContainerTabView), public ScrollingTabContainerView::TabView
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_INTERNAL_CSCROLLINGTABCONTAINERTABVIEW_H__
