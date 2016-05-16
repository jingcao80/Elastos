
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CACTIONBARVIEWHOMEVIEW_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CACTIONBARVIEWHOMEVIEW_H__

#include "_Elastos_Droid_Internal_Widget_CActionBarViewHomeView.h"
#include "elastos/droid/internal/widget/ActionBarView.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CActionBarViewHomeView)
    , public ActionBarView::HomeView
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CACTIONBARVIEWHOMEVIEW_H__

